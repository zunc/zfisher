#define _GNU_SOURCE
#include "zfisher.h"
#include <sys/types.h>
#include <unistd.h>
//#include <stddef.h>

//--- open hook: open
char sz_open[] = "open";
typedef int (*hook_open)(char *filename, int access, int permission);

int open(char *filename, int access, int permission) {
	log_hook("open(\"%s\")", filename);
	struct handler* h = api_get(sz_open);
	if (!h) {
		log_fatal(" - api_get(\"%s\") fail", sz_open);
	}
	hook_open _open = h->original;
	return _open(filename, access, permission);
}
//--- close hook: open

//--- open hook: pread
char sz_pread[] = "pread";
typedef ssize_t(*hook_pread)(int fd, void *buf, size_t count, off_t offset);
int pread_cnt = 0;
long pread_total = 0;

ssize_t pread(int fd, void *buf, size_t count, off_t offset) {
	//log_hook("open(\"%s\")", filename);
	hook_pread _pread = ((struct handler*) api_get(sz_pread))->original;
	pread_cnt++;
	pread_total += count;
	return _pread(fd, buf, count, offset);
}
//--- close hook: open

//--- open hook: pwrite
char sz_pwrite[] = "pwrite";
typedef ssize_t(*hook_pwrite)(int fd, const void *buf, size_t count, off_t offset);
int pwrite_cnt = 0;
long pwrite_total = 0;

ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset) {
	hook_pwrite _pwrite = ((struct handler*) api_get(sz_pwrite))->original;
	pwrite_cnt++;
	pwrite_total += count;
	return _pwrite(fd, buf, count, offset);
}
//--- close hook: open

__attribute__((constructor))
void init() {
	log_hook("[init_fisher] pid: %d", getpid());
	api_register(sz_open);
	api_register(sz_pread);
	api_register(sz_pwrite);
	hook_apis();
}

__attribute__((destructor))
void fini() {
	log_hook("[fini_fisher] pid: %d", getpid());
	log_hook("--- pread: summary");
	log_hook(" - pread(cnt: %d, total: %ld)", pread_cnt, pread_total);
	log_hook(" - pwrite(cnt: %d, total: %ld)", pwrite_cnt, pwrite_total);
}
