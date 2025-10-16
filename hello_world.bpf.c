#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>

struct {
    __uint(type, BPF_MAP_TYPE_RINGBUF);
    __uint(max_entries, 256 * 1024);
} ringbuf SEC(".maps");

SEC("tracepoint/raw_syscalls/sys_enter")
int hello(void *ctx)
{
    char msg[] = "Hello, World!";
    bpf_ringbuf_output(&ringbuf, msg, sizeof(msg), 0);
    return 0;
}
char LICENSE[] SEC("license") = "Dual BSD/GPL";
