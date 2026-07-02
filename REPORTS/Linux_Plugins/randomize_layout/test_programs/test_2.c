/*
 * test2_randomize_layout.c
 * ────────────────────────
 * NO system headers. Plain C only.
 * All randomized structs initialized field-by-field (no positional {})
 * because the plugin sets designated_init after shuffling.
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

/* ── structs with randomize_layout ─────────────────────────────── */

struct __attribute__((randomize_layout)) SmallPair {
    int key;
    int value;
};

struct __attribute__((randomize_layout)) NetworkPacket {
    unsigned int   src_ip;
    unsigned int   dst_ip;
    unsigned short src_port;
    unsigned short dst_port;
    unsigned char  protocol;
    unsigned char  ttl;
    unsigned short checksum;
};

struct __attribute__((randomize_layout)) ProcessInfo {
    int           pid;
    int           ppid;
    unsigned long flags;
    int           priority;
    int           nice;
    unsigned long start_time;
    int           uid;
    int           gid;
};

struct __attribute__((randomize_layout)) CryptoKey {
    unsigned int  key_len;
    unsigned int  algorithm;
    int           flags;
    int           ref_count;
    unsigned long expiry;
};

struct __attribute__((randomize_layout)) TimerEvent {
    unsigned long deadline;
    unsigned long interval;
    int           id;
    int           flags;
    int           repeat;
};

/* ── no_randomize_layout → relayout_struct path S ──────────────── */
struct __attribute__((no_randomize_layout)) StableABI {
    int           version;
    unsigned int  magic;
    unsigned long offset;
};

/* ── helpers ────────────────────────────────────────────────────── */

static void init_packet(struct NetworkPacket *p,
                        unsigned int src, unsigned int dst,
                        unsigned short sp, unsigned short dp)
{
    struct NetworkPacket *q = p;   /* same-type ptr → find_bad_casts BI */
    q->src_ip   = src;  q->dst_ip   = dst;
    q->src_port = sp;   q->dst_port = dp;
    q->protocol = 6;    q->ttl      = 64;
    q->checksum = 0xFFFF;
}

static void swap_pair(struct SmallPair *a, struct SmallPair *b)
{
    struct SmallPair tmp;
    tmp.key   = a->key;   tmp.value = a->value;
    a->key    = b->key;   a->value  = b->value;
    b->key    = tmp.key;  b->value  = tmp.value;
}

int main(void)
{
    /* SmallPair */
    struct SmallPair sp1, sp2;
    sp1.key = 10; sp1.value = 20;
    sp2.key = 30; sp2.value = 40;
    swap_pair(&sp1, &sp2);
    write_str("sp1.key="); write_int(sp1.key);

    /* NetworkPacket — 3 vars → finish_decl fires 3 times */
    struct NetworkPacket pkt1, pkt2, pkt3;
    init_packet(&pkt1, 0x0a000001, 0x0a000002, 1234,  80);
    init_packet(&pkt2, 0xc0a80001, 0xc0a80002, 5678, 443);
    init_packet(&pkt3, 0x7f000001, 0x7f000001, 9999,  22);
    write_str("proto="); write_int(pkt1.protocol);
    write_str("ttl=");   write_int(pkt2.ttl);
    write_str("port=");  write_int(pkt3.src_port);

    /* ProcessInfo */
    struct ProcessInfo pi;
    pi.pid = 1234; pi.ppid = 1; pi.flags = 0x400UL;
    pi.priority = 20; pi.nice = 0; pi.start_time = 1000000UL;
    pi.uid = 1000; pi.gid = 1000;
    write_str("pid="); write_int(pi.pid);
    write_str("uid="); write_int(pi.uid);

    /* CryptoKey */
    struct CryptoKey k;
    k.key_len = 16; k.algorithm = 1;
    k.flags = 0; k.ref_count = 1; k.expiry = 9999999UL;
    write_str("klen="); write_int(k.key_len);

    /* TimerEvent */
    struct TimerEvent t;
    t.deadline = 5000UL; t.interval = 1000UL;
    t.id = 42; t.flags = 0; t.repeat = 1;
    write_str("tid="); write_int(t.id);

    /* StableABI — no_randomize_layout */
    struct StableABI sa;
    sa.version = 1; sa.magic = 0xdeadbeefU; sa.offset = 0UL;
    write_str("ver="); write_int(sa.version);

    return 0;
}