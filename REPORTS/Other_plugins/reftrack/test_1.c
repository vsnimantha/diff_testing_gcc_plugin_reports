/*
 * reftrack_test1.c — Basic reference tracking
 * ─────────────────────────────────────────────
 * Plain C. No system headers except what's needed for malloc/free.
 *
 * Plugin args required:
 *   -fplugin-arg-compiled_plugin-addref=my_addref
 *   -fplugin-arg-compiled_plugin-removeref=my_removeref
 *
 * Coverage targets:
 *   plugin_init                    : version check, lang check, addref/removeref argv
 *   register_attributes            : PLUGIN_ATTRIBUTES fires
 *   handle_refcount_attribute      : RECORD_TYPE with 2 explicit args [2-arg path]
 *   pre_genericize / capture_given_fn : addref/removeref names matched
 *   gcc_parse_type_cb              : fires for each struct type
 *   collect_struct_with_rc         : struct with tracked fields
 *   is_valid_ref_fn                : validates addref/removeref signatures
 *   reftrack_pass / instrument_gseq:
 *     handle_gimple_assign         : tracked lhs, simple rhs -> transform_assign
 *     handle_gimple_assign         : lhs not tracked -> skip
 *     handle_gimple_assign         : lhs == rhs -> skip
 *     transform_assign / simple    : addref(rhs), removeref(lhs)
 *   handle_gimple_call             : tracked arg -> addref before call
 *   handle_gimple_call             : lhs tracked -> removeref+addref
 *   reftrack_cleanup / add_arg_cleanup  : fn param is tracked ptr
 *   reftrack_cleanup / add_block_cleanup: local tracked ptr vars
 *   collect_tracked_fields         : direct tracked ptr field
 *   emit_removeref_p               : local var with prior assignment
 */

#include <stdlib.h>

/* ── Tracked struct ─────────────────────────────────────────────── */

/* Forward-declare the ref functions before the struct attribute */
struct Node;
void my_addref(const struct Node *n);
void my_removeref(const struct Node *n);

struct __attribute__((reftrack(my_addref, my_removeref))) Node {
    int   value;
    int   refcount;
};

/* ── Ref functions (pre_genericize captures these by name) ──────── */

static int g_addref_count   = 0;
static int g_removeref_count = 0;

void my_addref(const struct Node *n)
{
    if (n) g_addref_count++;
}

void my_removeref(const struct Node *n)
{
    if (n) g_removeref_count++;
}

/* ── Helper functions ───────────────────────────────────────────── */

/* Returns a tracked pointer — handle_gimple_call lhs tracking */
static struct Node *node_create(int val)
{
    struct Node *n = malloc(sizeof(struct Node));
    if (n) { n->value = val; n->refcount = 1; }
    return n;
}

/* Takes a tracked pointer arg — handle_gimple_call arg tracking */
static int node_get_value(const struct Node *n)
{
    return n ? n->value : -1;
}

/* Takes tracked ptr param — add_arg_cleanup path */
static void node_print(struct Node *n)
{
    if (!n) return;
    /* local non-tracked vars: handle_gimple_assign lhs-not-tracked skip */
    int v = n->value;
    int r = n->refcount;
    (void)v; (void)r;
}

/* ── Main ───────────────────────────────────────────────────────── */

int main(void)
{
    /* local tracked ptr vars -> add_block_cleanup */
    struct Node *a = node_create(10);   /* lhs tracked, call returns tracked */
    struct Node *b = node_create(20);

    /* simple tracked assignment: p = q -> transform_assign simple path */
    struct Node *c = a;

    /* lhs == rhs: self-assign skip */
    a = a;

    /* non-tracked assignment: skip */
    int x = 5;
    x = x + 1;
    (void)x;

    /* pass tracked ptr as arg -> handle_gimple_call arg addref */
    node_print(a);
    node_print(b);

    int va = node_get_value(a);
    int vb = node_get_value(c);
    (void)va; (void)vb;

    /* tracked ptr reassignment: removeref(b), addref(a), b=a */
    b = a;

    free(a);
    free(b);
    (void)c;

    return 0;
}