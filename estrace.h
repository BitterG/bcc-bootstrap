#ifndef __ESTRACE_H
#define __ESTRACE_H

#define TASK_COMM_LEN 16
#define NAME_MAX 255
#define INVALID_UID ((uid_t)-1)

struct args_t {
    long int sys_id;
    long int args[6];
};

struct event {
	/* user terminology for pid: */
	__u64 ts;
	pid_t pid;
	uid_t uid;
	int ret;
	long int sys_id;
	__u64 callers[2];
	char comm[TASK_COMM_LEN];
	char fname[NAME_MAX];
};

#endif /* __ESTRACE_H */
