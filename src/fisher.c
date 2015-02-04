#define _GNU_SOURCE
#include "zfisher.h"
#include "pony.h"
#include <sys/types.h>
#include <unistd.h>
//#include <stddef.h>

// profiler implement: fisher stat

enum FStat {
	FS_OPEN = 0,
	FS_PREAD = 1,
	FS_PWRITE = 2
};

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

struct log_pread {
	int fd;
	long count;
	long offset;
};

int log_pread_parser(void *log, char *buff, size_t size) {
	assert(log);
	struct log_pread* record = log;
	return snprintf(buff, size, "%d,%ld,%ld", record->fd, record->count, record->offset);
}

ssize_t pread(int fd, void *buf, size_t count, off_t offset) {
	//log_hook("open(\"%s\")", filename);
	hook_pread _pread = ((struct handler*) api_get(sz_pread))->original;
	pread_cnt++;
	pread_total += count;
	struct timespec begin, end;
	gettime(&begin);
	ssize_t ret = _pread(fd, buf, count, offset);
	gettime(&end);
	long span = (end.tv_sec - begin.tv_sec)*1e9 + (end.tv_nsec - begin.tv_nsec);
	struct log_pread* log = malloc(sizeof (struct log_pread));
	log->fd = fd;
	log->count = count;
	log->offset = offset;
	pony_push(FS_PREAD, span, log);
	return ret;
}

struct pony_interface pread_stat = {
	.profile_id = FS_PREAD,
	.name = sz_pread,
	.parser = log_pread_parser
};
//--- close hook: pread

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
//--- close hook: pwrite

__attribute__((constructor))
void init() {
	log_hook("[init_fisher] pid: %d", getpid());
	api_register(sz_open);
	api_register(sz_pread);
	api_register(sz_pwrite);
	hook_apis();
	// init profiler
	pony_init();
	pony_register(&pread_stat);
}

__attribute__((destructor))
void fini() {
	log_hook("[fini_fisher] pid: %d", getpid());
	log_hook("--- pread: summary");
	log_hook(" - pread(cnt: %d, total: %ld)", pread_cnt, pread_total);
	log_hook(" - pwrite(cnt: %d, total: %ld)", pwrite_cnt, pwrite_total);
	log_hook("--- pony dump");
	pony_dump();

	// free all
	// <!> stuff: too lazy to implement #.#
}
