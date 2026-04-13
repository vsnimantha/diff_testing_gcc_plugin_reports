/*
 * reftrack_test2.c — malloc/free replacement + ternary assignment
 * ────────────────────────────────────────────────────────────────
 * Plain C.
 *
 * Plugin args required:
 *   -fplugin-arg-reftrack-addref=rc_addref
 *   -fplugin-arg-reftrack-removeref=rc_removeref
 *   -fplugin-arg-reftrack-alloc=rc_malloc_
 *   -fplugin-arg-reftrack-orig_alloc=malloc
 *   -fplugin-arg-reftrack-free=rc_free_
 *   -fplugin-arg-reftrack-orig_free=free
 *   -fplugin-arg-reftrack-replace
 *
 * Coverage targets:
 *   plugin_init argv: alloc/orig_alloc/free/orig_free/replace
 *   handle_refcount_attribute / RECORD 2 args  : explicit rc_addref/rc_removeref
 *   pre_genericize / capture_given_fn          : alloc + free names matched
 *   reftrack_cleanup / replace_mem_fun         : malloc->rc_malloc_, free->rc_free_
 *   handle_gimple_call / lhs tracked           : rc_malloc_ return value tracked
 *   reftrack_cleanup / add_block_cleanup       : local tracked ptrs cleaned up
 *   collect_tracked_fields / struct with tracked ptr field : Container.item
 *   transform_assign / ternary (rhs2/rhs3)     : p = cond ? a : b
 */

#include <stdlib.h>

/* ── rc_malloc_ / rc_free_ (captured by pre_genericize by name) ── */
void *rc_malloc_(size_t sz)      { return malloc(sz); }
void  rc_free_  (void *p)        { free(p); }

/* ── Ref functions ────────────────────────────────────────────────*/
struct Item;
void rc_addref  (const struct Item *p);
void rc_removeref(const struct Item *p);

static int g_refs = 0;
void rc_addref  (const struct Item *p) { if (p) g_refs++; }
void rc_removeref(const struct Item *p) { if (p) g_refs--; }

/* ── Tracked struct with explicit 2-arg attribute ────────────────*/
struct __attribute__((reftrack(rc_addref, rc_removeref))) Item {
    int   id;
    int   weight;
};

/* ── Struct containing a tracked pointer field ───────────────────
 * collect_tracked_fields recurses into Container to find Item*    */
struct Container {
    struct Item *item;
    int          count;
};

/* ── Helpers ──────────────────────────────────────────────────────*/
static struct Item *item_alloc(int id, int weight)
{
    /* malloc replaced with rc_malloc_ by replace_mem_fun */
    struct Item *it = malloc(sizeof(struct Item));
    if (it) { it->id = id; it->weight = weight; }
    return it;
}

static void item_free(struct Item *it)
{
    /* free replaced with rc_free_ by replace_mem_fun */
    free(it);
}

/* ternary -> transform_assign rhs2/rhs3 path */
static struct Item *item_select(int use_first, struct Item *a, struct Item *b)
{
    struct Item *chosen = use_first ? a : b;
    return chosen;
}

static void use_item(struct Item *it)
{
    if (!it) return;
    int w = it->weight;
    (void)w;
}

int main(void)
{
    struct Item *x = item_alloc(1, 100);
    struct Item *y = item_alloc(2, 200);

    /* ternary: transform_assign rhs2 path */
    struct Item *p = item_select(1, x, y);
    struct Item *q = item_select(0, x, y);

    use_item(p);
    use_item(q);

    /* Container with tracked ptr field -> collect_tracked_fields */
    struct Container c;
    c.item  = x;
    c.count = 1;
    (void)c;

    /* tracked reassignment: removeref(x), addref(y), x=y */
    x = y;
    (void)x;

    item_free(y);

    return 0;
}