/*
 * reftrack_test4.c — Multiple tracked types + deep cleanup + edge cases
 * ────────────────────────────────────────────────────────────────────────
 * Plain C.
 *
 * Plugin args required:
 *   -fplugin-arg-reftrack-addref=generic_addref
 *   -fplugin-arg-reftrack-removeref=generic_removeref
 *   -fplugin-arg-reftrack-log_level=2
 *
 * NOTE: All structs use explicit 2-arg __attribute__((reftrack(fn1,fn2))).
 *       The 0-arg form requires default fns to be registered BEFORE the
 *       struct type is completed, which is impossible since pre_genericize
 *       fires per-function after parsing — so 0-arg always errors.
 *
 * Coverage targets:
 *   plugin_init argv: log_level
 *   handle_refcount_attribute / RECORD 2 args  : three structs, each explicit
 *   is_valid_ref_fn                            : validates each pair of fns
 *   multiple ref_structs in G.ref_structs map
 *   pre_genericize / capture_given_fn          : addref/removeref name match
 *   gcc_pass_cb                                : PLUGIN_PASS_EXECUTION fires
 *   for_each_block / for_each_block_var        : block var traversal in cleanup
 *   traverse_dfs_gimple / GIMPLE_BIND          : nested bind blocks
 *   collect_tracked_fields / struct with tracked ptr field : Session.handle
 *   add_block_cleanup / multiple tracked vars  : 3 tracked locals cleaned up
 *   add_arg_cleanup / two tracked params       : process(Handle*, Event*)
 *   handle_gimple_assign / array_elem skip     : array of tracked ptrs
 *   is_array_of_tracked_struct                 : array type check
 *   c_type_name / get_ultimate_type            : typedef chain traversal
 *   transform_assign / simple                  : h = ha reassignment
 */

#include <stdlib.h>

/* ── Forward declarations ───────────────────────────────────────── */
struct Handle;
struct Event;
struct Buffer;

void handle_addref  (const struct Handle *h);
void handle_removeref(const struct Handle *h);
void event_addref   (const struct Event  *e);
void event_removeref(const struct Event  *e);
void buf_addref     (const struct Buffer *b);
void buf_removeref  (const struct Buffer *b);

/* Also declared as default names so plugin_init argv captures them */
void generic_addref  (const struct Handle *h);
void generic_removeref(const struct Handle *h);

/* ── Three tracked structs, all with explicit 2-arg attribute ────── */

struct __attribute__((reftrack(handle_addref, handle_removeref))) Handle {
    int fd;
    int flags;
};

struct __attribute__((reftrack(event_addref, event_removeref))) Event {
    int   type;
    int   priority;
    void *payload;
};

struct __attribute__((reftrack(buf_addref, buf_removeref))) Buffer {
    void  *data;
    size_t size;
    int    refcount;
};

/* ── Struct containing tracked pointer fields ────────────────────
 * collect_tracked_fields recurses into Session                     */
struct Session {
    struct Handle *handle;
    struct Event  *event;
    int            id;
};

/* ── Ref function implementations ────────────────────────────────*/
static int g_href = 0, g_eref = 0, g_bref = 0;

void handle_addref  (const struct Handle *h) { if (h) g_href++; }
void handle_removeref(const struct Handle *h) { if (h) g_href--; }
void event_addref   (const struct Event  *e) { if (e) g_eref++; }
void event_removeref(const struct Event  *e) { if (e) g_eref--; }
void buf_addref     (const struct Buffer *b) { if (b) g_bref++; }
void buf_removeref  (const struct Buffer *b) { if (b) g_bref--; }

/* These match the plugin argv names */
void generic_addref  (const struct Handle *h) { (void)h; }
void generic_removeref(const struct Handle *h) { (void)h; }

/* ── typedef chain (c_type_name / get_ultimate_type) ─────────────*/
typedef struct Handle HandleT;
typedef HandleT       HandleAlias;

/* ── Allocators ──────────────────────────────────────────────────*/
static struct Handle *handle_create(int fd)
{
    struct Handle *h = malloc(sizeof(struct Handle));
    if (h) { h->fd = fd; h->flags = 0; }
    return h;
}

static struct Event *event_create(int type)
{
    struct Event *e = malloc(sizeof(struct Event));
    if (e) { e->type = type; e->priority = 0; e->payload = NULL; }
    return e;
}

static struct Buffer *buf_create(size_t sz)
{
    struct Buffer *b = malloc(sizeof(struct Buffer));
    if (b) { b->data = malloc(sz); b->size = sz; b->refcount = 1; }
    return b;
}

/* Two tracked ptr params -> add_arg_cleanup for both */
static int process(struct Handle *h, struct Event *e)
{
    if (!h || !e) return -1;
    struct Buffer *tmp = buf_create(64);
    int result = h->fd + e->type;
    free(tmp->data);
    free(tmp);
    return result;
}

/* Nested blocks -> traverse_dfs_gimple GIMPLE_BIND path */
static void nested_blocks(struct Handle *h)
{
    if (h->fd > 0) {
        struct Event *inner_e = event_create(42);
        int v = inner_e->type;
        (void)v;
        free(inner_e);
    }
    {
        struct Buffer *inner_b = buf_create(32);
        free(inner_b->data);
        free(inner_b);
    }
}

/* Array of tracked ptrs: is_array_of_tracked_struct -> skip */
static void use_handle_array(void)
{
    struct Handle *arr[4];
    for (int i = 0; i < 4; i++)
        arr[i] = handle_create(i);
    /* array elem assignment -> handle_gimple_assign array skip */
    arr[0] = arr[1];
    for (int i = 0; i < 4; i++)
        free(arr[i]);
}

/* typedef usage -> c_type_name/get_ultimate_type */
static HandleAlias *make_alias(int fd)
{
    return handle_create(fd);
}

int main(void)
{
    /* 3 local tracked ptrs -> add_block_cleanup for all 3 */
    struct Handle *h   = handle_create(1);
    struct Event  *e   = event_create(5);
    struct Buffer *buf = buf_create(128);

    int r = process(h, e);
    (void)r;

    nested_blocks(h);
    use_handle_array();

    HandleAlias *ha = make_alias(99);

    /* Session with tracked ptr fields -> collect_tracked_fields */
    struct Session sess;
    sess.handle = h;
    sess.event  = e;
    sess.id     = 1;
    (void)sess;

    /* tracked reassignment: removeref(h), addref(ha), h=ha */
    h = ha;

    free(h);
    free(e);
    free(buf->data);
    free(buf);

    return 0;
}