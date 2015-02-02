#define _GNU_SOURCE
#include "zfisher.h"
#include <sys/types.h>
#include <unistd.h>

char sz_open[] = "open";

//--- open hook: open
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

__attribute__((constructor))
void init() {
	log_hook("[init_fisher] pid: %d", getpid());
	api_register(sz_open);
	hook_apis();
}

__attribute__((destructor))
void fini() {
	log_hook("[fini_fisher] pid: %d", getpid());
}
