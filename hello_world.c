#include <stdio.h>
#include <signal.h>
#include <bpf/libbpf.h>
#include "hello_world.skel.h"

static volatile bool exiting = false;

static void sig_handler(int sig) {
    exiting = true;
}

static int handle_event(void *ctx, void *data, size_t data_sz) {
    printf("%s\n", (char *)data);
    return 0;
}
static int my_libbpf_print(enum libbpf_print_level level,
                          const char *format, va_list args)
{
    return vfprintf(stderr, format, args);
}
int main(int argc, char **argv) {
    struct hello_world_bpf *skel;
    struct ring_buffer *rb = NULL;
    int err;
    libbpf_set_print(my_libbpf_print);
    skel = hello_world_bpf__open();
    if (!skel) {
        fprintf(stderr, "Failed to open BPF skeleton\n");
        return 1;
    }
    err = hello_world_bpf__load(skel);
    if (err) {
        fprintf(stderr, "Failed to load BPF skeleton\n");
        goto cleanup;
    }
    err = hello_world_bpf__attach(skel);
    if (err) {
        fprintf(stderr, "Failed to attach BPF skeleton\n");
        goto cleanup;
    }
    rb = ring_buffer__new(bpf_map__fd(skel->maps.ringbuf), handle_event, NULL, NULL);
    if (!rb) {
        err = -1;
        fprintf(stderr, "Failed to create ring buffer\n");
        goto cleanup;
    }
    printf("Successfully started! Press Ctrl-C to stop.\n");
    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);
    while (!exiting) {
        err = ring_buffer__poll(rb, 100 /* timeout_ms */);
        if (err == -EINTR) {
            err = 0;
            break;
        }
        if (err < 0) {
            printf("Error polling ring buffer: %d\n", err);
            break;
        }
    }
cleanup:
    ring_buffer__free(rb);
    hello_world_bpf__destroy(skel);
    return -err;
}
