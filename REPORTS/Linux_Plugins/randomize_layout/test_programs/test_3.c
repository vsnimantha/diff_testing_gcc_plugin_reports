
/*
 * test3_pure_ops_structs.c
 * ────────────────────────
 * NO system headers. Plain C only.
 * All struct initializers use designated (.field=) syntax because
 * the plugin adds designated_init to every shuffled struct.
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

struct MinimalOps {
    int (*open)(int flags);
    int (*close)(void);
};

struct DeviceOps {
    int  (*probe)(int id);
    void (*remove)(int id);
    int  (*suspend)(int level);
    int  (*resume)(int level);
    int  (*ioctl)(unsigned int cmd, unsigned long arg);
};

struct FileOps {
    int  (*open)(const char *path, int flags);
    int  (*close)(int fd);
    long (*read)(int fd, void *buf, long n);
    long (*write)(int fd, const void *buf, long n);
    long (*seek)(int fd, long offset, int whence);
    int  (*flush)(int fd);
    int  (*fsync)(int fd);
};

struct NetworkOps {
    int  (*connect)(unsigned int addr, unsigned short port);
    int  (*disconnect)(int sock);
    long (*send)(int sock, const void *buf, long n);
    long (*recv)(int sock, void *buf, long n);
    int  (*bind)(unsigned int addr, unsigned short port);
    int  (*listen)(int sock, int backlog);
};

struct InnerOps {
    void (*init)(void);
    void (*shutdown)(void);
};

struct BusOps {
    int (*attach)(int id);
    int (*detach)(int id);
    int (*reset)(int id);
};

/* mixed: fptr + data -> is_pure_ops=0 */
struct DriverInfo {
    const char *name;
    int         version;
    int  (*probe)(int id);
    void (*remove)(int id);
};

static int  dev_probe  (int id) { write_str("probe "); write_int(id); return 0; }
static void dev_remove (int id) { write_str("remove "); write_int(id); }
static int  dev_suspend(int l)  { write_str("suspend "); write_int(l); return 0; }
static int  dev_resume (int l)  { write_str("resume "); write_int(l); return 0; }
static int  dev_ioctl(unsigned int c, unsigned long a)
    { write_str("ioctl "); write_int(c); (void)a; return 0; }

static int  file_open (const char *p, int f) { (void)p;(void)f; return 3; }
static int  file_close(int fd)               { (void)fd; return 0; }
static long file_read (int fd, void *b, long n)       { (void)fd;(void)b; return n; }
static long file_write(int fd, const void *b, long n) { (void)fd;(void)b; return n; }
static long file_seek (int fd, long o, int w) { (void)fd;(void)o;(void)w; return 0; }
static int  file_flush(int fd)               { (void)fd; return 0; }
static int  file_fsync(int fd)               { (void)fd; return 0; }

static int  min_open (int f) { (void)f; return 0; }
static int  min_close(void)  { return 0; }

static void inner_init    (void) { write_str("init\n"); }
static void inner_shutdown(void) { write_str("shutdown\n"); }

static int bus_attach(int id) { write_str("attach "); write_int(id); return 0; }
static int bus_detach(int id) { write_str("detach "); write_int(id); return 0; }
static int bus_reset (int id) { write_str("reset ");  write_int(id); return 0; }

static int  net_connect   (unsigned int a, unsigned short p) { (void)a;(void)p; return 0; }
static int  net_disconnect(int s)                            { (void)s; return 0; }
static long net_send      (int s, const void *b, long n)     { (void)s;(void)b; return n; }
static long net_recv      (int s, void *b, long n)           { (void)s;(void)b; return n; }
static int  net_bind      (unsigned int a, unsigned short p) { (void)a;(void)p; return 0; }
static int  net_listen    (int s, int bl)                    { (void)s;(void)bl; return 0; }

static void exercise_device(const struct DeviceOps *ops, int id)
{
    ops->probe(id);
    ops->suspend(1);
    ops->resume(1);
    ops->ioctl(0x1234, 0UL);
    ops->remove(id);
}

static void exercise_file(const struct FileOps *ops)
{
    int fd = ops->open("/tmp/t", 0);
    char buf[8] = {0};
    ops->read(fd, buf, 8);
    ops->write(fd, "hi", 2);
    ops->flush(fd);
    ops->fsync(fd);
    ops->close(fd);
}

int main(void)
{
    struct MinimalOps mops = {
        .open  = min_open,
        .close = min_close,
    };
    write_str("open="); write_int(mops.open(0));

    struct DeviceOps dops = {
        .probe   = dev_probe,
        .remove  = dev_remove,
        .suspend = dev_suspend,
        .resume  = dev_resume,
        .ioctl   = dev_ioctl,
    };
    exercise_device(&dops, 7);

    struct FileOps fops = {
        .open  = file_open,
        .close = file_close,
        .read  = file_read,
        .write = file_write,
        .seek  = file_seek,
        .flush = file_flush,
        .fsync = file_fsync,
    };
    exercise_file(&fops);

    struct InnerOps inner = {
        .init     = inner_init,
        .shutdown = inner_shutdown,
    };
    inner.init();
    inner.shutdown();

    struct BusOps bus = {
        .attach = bus_attach,
        .detach = bus_detach,
        .reset  = bus_reset,
    };
    bus.attach(1);
    bus.reset(1);
    bus.detach(1);

    struct NetworkOps nops = {
        .connect    = net_connect,
        .disconnect = net_disconnect,
        .send       = net_send,
        .recv       = net_recv,
        .bind       = net_bind,
        .listen     = net_listen,
    };
    nops.connect(0x7f000001, 80);
    nops.listen(0, 5);

    struct DriverInfo drv = {
        .name    = "mydrv",
        .version = 2,
        .probe   = dev_probe,
        .remove  = dev_remove,
    };
    write_str("ver="); write_int(drv.version);
    drv.probe(42);

    return 0;
}