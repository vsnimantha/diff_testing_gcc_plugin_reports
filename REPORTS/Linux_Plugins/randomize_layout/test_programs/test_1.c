
/*
 * test1_plain_structs.c
 * ─────────────────────
 * NO system headers — the plugin segfaults on glibc internal structs
 * (e.g. cookie_io_functions_t from <stdio.h>) because ORIG_TYPE_NAME
 * returns a bad pointer for anonymous/system types.
 *
 * We hand-roll all I/O via the write() syscall so the compiler sees
 * only our own struct types, none from system headers.
 *
 * Coverage targets:
 *   finish_type          : fires for every struct we define          [L]
 *   randomize_type       : path O — no attr, not pure-ops → mark
 *                          randomize_considered, skip relayout
 *   is_pure_ops_struct   : returns 0 for all structs here (Z3)
 *   find_bad_casts_execute: pointer assignments between our structs
 *   randomize_layout_finish_decl: path CD (no randomize_performed)
 */

/* hand-rolled output — avoids all system headers */
static void write_str(const char *s)
{
    __asm__ volatile (
        "syscall"
        :
        : "a"(1), "D"(1), "S"(s),
          "d"(__builtin_strlen(s))
        : "rcx", "r11", "memory"
    );
}

static void write_int(long n)
{
    char buf[24];
    int  i = 23;
    buf[i] = '\n';
    if (n == 0) { buf[--i] = '0'; }
    else {
        int neg = n < 0;
        unsigned long u = neg ? (unsigned long)(-n) : (unsigned long)n;
        while (u) { buf[--i] = '0' + (u % 10); u /= 10; }
        if (neg) buf[--i] = '-';
    }
    __asm__ volatile (
        "syscall"
        :
        : "a"(1), "D"(1), "S"(buf + i),
          "d"(24 - i)
        : "rcx", "r11", "memory"
    );
}

/* ── single-field struct (relayout_struct path Q: num_fields < 2) ── */
struct SingleField { int value; };

/* ── two-field structs ───────────────────────────────────────────── */
struct Point2D  { double x; double y; };
struct Point3D  { double x; double y; double z; };
struct IntPair  { int first; int second; };
struct LongPair { long a; long b; };

/* ── larger structs ──────────────────────────────────────────────── */
struct Rectangle { double x; double y; double width; double height; };

struct Employee {
    int           id;
    int           department;
    double        salary;
    int           active;
    int           years;
};

struct ColorRGBA {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

/* ── nested structs ──────────────────────────────────────────────── */
struct Address {
    int zip;
    int region;
    int country;
};

struct Person {
    int           age;
    int           id;
    struct Address home;
    struct Address work;
};

/* ── self-referential struct (linked list) ───────────────────────── */
struct ListNode {
    int              data;
    struct ListNode *next;
};

/* ── struct with pointer fields ──────────────────────────────────── */
struct NodeWrapper {
    struct ListNode *head;
    struct ListNode *tail;
    int              count;
};

/* ── blob with void* (find_bad_casts BE/BF paths) ────────────────── */
struct BlobRef {
    void  *data;
    long   size;
    int    flags;
};

/* ── free functions ──────────────────────────────────────────────── */

static int add(int a, int b)        { return a + b; }
static int multiply(int a, int b)   { return a * b; }

static double dist_sq(struct Point2D a, struct Point2D b)
{
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    return dx*dx + dy*dy;
}

/* same-type pointer assignment → find_bad_casts path BI */
static struct ListNode *list_push(struct ListNode *head, int val)
{
    /* use a static pool to avoid malloc (no stdlib) */
    static struct ListNode pool[64];
    static int idx = 0;
    struct ListNode *node = &pool[idx++];
    node->data = val;
    node->next = head;
    return node;
}

static int list_sum(const struct ListNode *head)
{
    int total = 0;
    const struct ListNode *n = head;
    while (n) { total += n->data; n = n->next; }
    return total;
}

/* void* usage → find_bad_casts BE/BF */
static struct BlobRef make_blob(void *ptr, long sz)
{
    struct BlobRef b;
    b.data  = ptr;
    b.size  = sz;
    b.flags = 0;
    return b;
}

/* non-pointer arithmetic → find_bad_casts path BD */
static int compute(int a, int b, int c)
{
    int x = a + b;
    int y = x * c;
    return y - a;
}

int main(void)
{
    struct SingleField sf; sf.value = 42;
    write_str("sf="); write_int(sf.value);

    struct Point2D p1 = {1.0, 2.0}, p2 = {4.0, 6.0};
    (void)dist_sq(p1, p2);
    write_str("dist_sq ok");
    /* newline already in write_int; just confirm execution */

    struct Employee e = {1, 3, 75000.0, 1, 5};
    write_str("emp id="); write_int(e.id);

    struct Person p = {30, 7, {12345, 1, 1}, {12346, 1, 1}};
    write_str("person age="); write_int(p.age);

    struct ListNode *list = (void*)0;
    for (int i = 1; i <= 5; i++) list = list_push(list, i);
    write_str("list_sum="); write_int(list_sum(list));

    int buf[4] = {1,2,3,4};
    struct BlobRef blob = make_blob(buf, 16);
    write_str("blob_size="); write_int(blob.size);

    write_str("compute="); write_int(compute(3, 4, 5));

    write_str("add=");      write_int(add(6, 7));
    write_str("multiply="); write_int(multiply(6, 7));

    struct ColorRGBA c = {255, 128, 0, 255};
    write_str("color_r="); write_int(c.r);

    struct NodeWrapper nw = {(void*)0, (void*)0, 0};
    write_str("nw_count="); write_int(nw.count);

    return 0;
}