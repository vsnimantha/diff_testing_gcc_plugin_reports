
/*
 * test4_edge_cases.c
 * ──────────────────
 * NO system headers. Plain C only.
 * All randomized structs initialized field-by-field.
 */

static void write_int(long n)
{
    char buf[24]; int i = 23; buf[i] = '\n';
    if (n == 0) { buf[--i] = '0'; }
    else {
        unsigned long u = (n<0)?(unsigned long)(-n):(unsigned long)n;
        while(u){buf[--i]='0'+(u%10);u/=10;}
        if(n<0)buf[--i]='-';
    }
    __asm__ volatile("syscall"::
        "a"(1),"D"(1),"S"(buf+i),"d"(24-i):"rcx","r11","memory");
}
static void write_str(const char *s)
{
    __asm__ volatile("syscall"::
        "a"(1),"D"(1),"S"(s),"d"(__builtin_strlen(s)):"rcx","r11","memory");
}

/* ── flexible array (relayout_struct path V) ─────────────────────── */
struct __attribute__((randomize_layout)) FlexPacket {
    unsigned int   seq;
    unsigned int   ack;
    unsigned short flags;
    unsigned short checksum;
    unsigned char  data[0];
};

/* ── single-field randomized (path Q: num_fields < 2) ───────────── */
struct __attribute__((randomize_layout)) SingleRandom {
    long value;
};

/* ── multi-field randomized ──────────────────────────────────────── */
struct __attribute__((randomize_layout)) SecureHeader {
    unsigned long magic;
    unsigned int  version;
    unsigned int  length;
    unsigned int  flags;
    unsigned int  reserved;
    unsigned long timestamp;
};

/* ── typedef alias: same type → path R (randomize_performed) ──────── */
typedef struct SecureHeader SecureHeaderAlias;

/* ── no_randomize_layout → path S ────────────────────────────────── */
struct __attribute__((no_randomize_layout)) WireFormat {
    unsigned int header;
    unsigned int payload_len;
    unsigned int crc;
};

/* ── plain structs for find_bad_casts ────────────────────────────── */
struct TypeA { int x; int y; };
struct TypeB { int x; int y; };

struct BlobRef { void *data; long size; };

/* ── same-type ptr assignment → find_bad_casts BI ────────────────── */
static void init_secure(struct SecureHeader *hdr,
                        unsigned long magic, unsigned int ver)
{
    struct SecureHeader *h = hdr;
    h->magic     = magic;
    h->version   = ver;
    h->length    = 32;
    h->flags     = 0;
    h->reserved  = 0;
    h->timestamp = 9999999UL;
}

/* void* copy → find_bad_casts BE/BF */
static void copy_blob(void *dst, const void *src, long n)
{
    const unsigned char *s = (const unsigned char *)src;
    unsigned char       *d = (unsigned char *)dst;
    for (long i = 0; i < n; i++) d[i] = s[i];
}

/* non-pointer arithmetic → find_bad_casts BD */
static int compute(int a, int b, int c)
{
    return (a + b) * c - a;
}

/* local struct init → handle_local_var_initializers */
static int use_local_structs(void)
{
    struct TypeA a; a.x = 1; a.y = 2;
    struct TypeB b; b.x = 3; b.y = 4;
    return a.x + b.x;
}

/* static global initializer → check_bad_casts_in_constructor */
static struct WireFormat g_wire = {
    .header      = 0x1234,
    .payload_len = 256,
    .crc         = 0x1234 ^ 256,
};

/* FlexPacket pool */
static unsigned char flex_pool[256];

static struct FlexPacket *make_flex(unsigned int seq)
{
    struct FlexPacket *fp = (struct FlexPacket *)flex_pool;
    fp->seq      = seq;
    fp->ack      = 0;
    fp->flags    = 0x002;
    fp->checksum = 0xFFFF;
    fp->data[0]  = 0xAB;
    return fp;
}

int main(void)
{
    /* FlexPacket — flexible array path V */
    struct FlexPacket *fp = make_flex(1);
    write_str("seq=");   write_int(fp->seq);
    write_str("flags="); write_int(fp->flags);

    /* SingleRandom — path Q */
    struct SingleRandom sr; sr.value = 99;
    write_str("sr="); write_int(sr.value);

    /* SecureHeader — full shuffle, field-by-field init */
    struct SecureHeader sh;
    init_secure(&sh, 0xCAFEBABEUL, 2);
    write_str("ver="); write_int(sh.version);

    /* typedef alias — path R */
    SecureHeaderAlias sha;
    init_secure(&sha, 0xDEADBEEFUL, 3);
    write_str("alias_ver="); write_int(sha.version);

    /* global WireFormat — no_randomize_layout path S */
    write_str("crc="); write_int(g_wire.crc);

    /* WireFormat local */
    struct WireFormat wf;
    wf.header = 0xABCD; wf.payload_len = 128; wf.crc = 0;
    write_str("wf_hdr="); write_int(wf.header);

    /* void* copy — BE/BF */
    int src[4] = {1,2,3,4}, dst[4] = {0,0,0,0};
    copy_blob(dst, src, 16);
    write_str("dst0="); write_int(dst[0]);

    /* arithmetic — BD */
    write_str("compute="); write_int(compute(3,4,5));

    /* local struct init */
    write_str("local="); write_int(use_local_structs());

    /* BlobRef with void* */
    struct BlobRef blob; blob.data = src; blob.size = 16;
    write_str("bsz="); write_int(blob.size);

    return 0;
}