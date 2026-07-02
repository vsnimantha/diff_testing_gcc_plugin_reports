/*
 * reftrack_test3.c — Destructor + ignored functions
 * ──────────────────────────────────────────────────
 * Plain C.
 *
 * Plugin args required:
 *   -fplugin-arg-compiled_plugin-addref=obj_addref
 *   -fplugin-arg-compiled_plugin-removeref=obj_removeref
 *
 * Coverage targets:
 *   handle_refcount_attribute / FUNCTION_DECL IGNORE (flag=1)
 *   handle_refcount_attribute / FUNCTION_DECL DESTRUCTOR (flag=4)
 *   pre_genericize / check_fn_attributes        : DESTRUCTOR flag found
 *   is_valid_destructor_fn                      : validates void(*)(Tracked*)
 *   is_destructor                               : used in skip_function check
 *   handle_gimple_call / skip destructor        : destructor call not instrumented
 *   handle_gimple_call / skip ignored fn        : ignored fn call skipped
 *   reftrack_cleanup / add_arg_cleanup          : skipped for destructor fn
 *   is_ignored_function                         : checked for each callee
 *   gcc_pass_cb                                 : PLUGIN_PASS_EXECUTION fires
 *   for_each_block / for_each_block_var         : block traversal in cleanup
 *   traverse_dfs_gimple                         : DFS over GIMPLE_BIND/TRY
 */

#include <stdlib.h>

/* ── Tracked struct ─────────────────────────────────────────────── */
struct Obj;
void obj_addref  (const struct Obj *o);
void obj_removeref(const struct Obj *o);

struct __attribute__((reftrack(obj_addref, obj_removeref))) Obj {
    int   type;
    int   refcount;
    void *data;
};

/* ── Ref functions ──────────────────────────────────────────────── */
static int g_ref = 0;
void obj_addref  (const struct Obj *o) { if (o) g_ref++; }
void obj_removeref(const struct Obj *o) { if (o) g_ref--; }

/* ── Destructor (flag=4 = REFTRACK_DESTRUCTOR_FN_FLAG) ─────────── */
void obj_destroy(struct Obj *o) __attribute__((reftrack(4)));
void obj_destroy(struct Obj *o)
{
    if (!o) return;
    free(o->data);
    free(o);
}

/* ── Ignored function (flag=1 = REFTRACK_IGNORE_FLAG) ───────────── */
struct Obj *obj_internal_get(void) __attribute__((reftrack(1)));
struct Obj *obj_internal_get(void)
{
    /* body not instrumented — plugin ignores this function */
    struct Obj *o = malloc(sizeof(struct Obj));
    if (o) { o->type = 99; o->refcount = 1; o->data = NULL; }
    return o;
}

/* ── Normal functions ───────────────────────────────────────────── */
static struct Obj *obj_create(int type)
{
    struct Obj *o = malloc(sizeof(struct Obj));
    if (o) { o->type = type; o->refcount = 1; o->data = NULL; }
    return o;
}

/* Takes tracked ptr param: add_arg_cleanup runs for this fn */
static int obj_get_type(struct Obj *o)
{
    return o ? o->type : -1;
}

/* Calls ignored fn then destructor */
static void exercise_ignored_and_destructor(void)
{
    /* call to ignored fn: handle_gimple_call skip path */
    struct Obj *hidden = obj_internal_get();

    struct Obj *normal = obj_create(1);

    int t1 = obj_get_type(hidden);
    int t2 = obj_get_type(normal);
    (void)t1; (void)t2;

    /* call to destructor: handle_gimple_call destructor-skip path */
    obj_destroy(hidden);
    obj_destroy(normal);
}

int main(void)
{
    struct Obj *a = obj_create(10);
    struct Obj *b = obj_create(20);

    /* simple tracked assignment */
    struct Obj *c = a;

    int ta = obj_get_type(a);
    int tb = obj_get_type(b);
    (void)ta; (void)tb; (void)c;

    exercise_ignored_and_destructor();

    obj_destroy(a);
    obj_destroy(b);

    return 0;
}