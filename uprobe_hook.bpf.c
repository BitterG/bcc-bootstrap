#define __TARGET_ARCH_x86
#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include "uprobe_hook.h"

struct {
    __uint(type, BPF_MAP_TYPE_RINGBUF);
    __uint(max_entries, 256 * 1024);
} ringbuf SEC(".maps");

// SEC("uprobe//apex/com.android.runtime/lib64/bionic/libc.so:syscall")
// int handle_hook_libc_exit__getpid(struct pt_regs *ctx) {
//       //解析返回值方法PT_REGS_RC
//       unsigned int ret = PT_REGS_PARM1(ctx);
//       //获取ringbuffer事件
//       struct event *event = bpf_ringbuf_reserve(&ringbuf, sizeof(*event), 0);
//       if (!event) return 0;
//       //将返回值写入事件
//       //这里参数名pid没改，正常应该改成对应名字
//       event->pid = ret;
//       //提交事件到ringbuffer
//       bpf_ringbuf_submit(event, 0);
//       return 0;
// }

//问价读取的hook
// int open(const char *pathname, int flags, mode_t mode);
SEC("uprobe//apex/com.android.runtime/lib64/bionic/libc.so:open")
int BPF_UPROBE(handle_hook_libc_enter__syscall, const char* pathname, int flags) {
      //获取ringbuffer事件
      struct event_open *event_open = bpf_ringbuf_reserve(&ringbuf, sizeof(*event_open), 0);
      if (!event_open) return 0;
      //将返回值写入事件
      //这里参数名pid没改，正常应该改成对应名字
      // event_open->pathname = pathname;
      bpf_probe_read_user_str(&event_open->pathname, sizeof(event_open->pathname), pathname);
      //提交事件到ringbuffer
      bpf_ringbuf_submit(event_open, 0);
      return 0;
}

// SEC("uprobe//apex/com.android.runtime/lib64/bionic/libc.so:syscall")
// int BPF_UPROBE(handle_hook_libc_enter__syscall, long __number) {
//       //获取ringbuffer事件
//       struct event *event = bpf_ringbuf_reserve(&ringbuf, sizeof(*event), 0);
//       if (!event) return 0;
//       //将返回值写入事件
//       //这里参数名pid没改，正常应该改成对应名字
//       event->pid = __number;
//       //提交事件到ringbuffer
//       bpf_ringbuf_submit(event, 0);
//       return 0;
// }

// SEC("uretprobe//apex/com.android.runtime/lib64/bionic/libc.so:getpid")
// int handle_hook_libc_exit__getpid(struct pt_regs *ctx) {
//       //解析返回值方法PT_REGS_RC
//       pid_t ret = PT_REGS_RC(ctx);
//       //获取ringbuffer事件
//       struct event *event = bpf_ringbuf_reserve(&ringbuf, sizeof(*event), 0);
//       if (!event) return 0;
//       //将返回值写入事件
//       event->pid = ret;
//       //提交事件到ringbuffer
//       bpf_ringbuf_submit(event, 0);
//       return 0;
// }

char LICENSE[] SEC("license") = "GPL";