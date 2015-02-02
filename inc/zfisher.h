/* 
 * File:   zfisher.h
 * Author: khoai
 *
 * Created on February 1, 2015, 1:56 PM
 */

#ifndef ZFISHER_H
#define	ZFISHER_H

#define _GNU_SOURCE
#include <dlfcn.h>

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "util.h"
#include "config.h"
#include "list.h"

// api handler

struct handler {
	char* name; // api name
	void* original; // original api pointer
	struct list list;
};

static struct list apis = LIST_HEAD_INIT(apis);

void api_register(char* name) {
	struct handler* h = malloc(sizeof (struct handler));
	h->name = name;
	h->original = NULL;
	LIST_ADDQ(&apis, &h->list);
}

void api_unregister(void* api) {
	struct handler* h = api;
	LIST_DEL(&h->list);
	LIST_INIT(&h->list);
}

void* api_get(const char* name) {
	struct handler *h;

	list_for_each_entry(h, &apis, list) {
		if (!strcmp(h->name, name))
			return h;
	}
	return NULL;
}

static void hook_apis(void) {
	struct handler *h;

	list_for_each_entry(h, &apis, list) {
		char *msg = NULL;
		char *name = h->name;
		h->original = dlsym(RTLD_NEXT, name);
		if ((h->original == NULL) || ((msg = dlerror()) != NULL)) {
			log_hook("[%s] failed : %s", name, msg);
			exit(1);
		} else {
			log_hook(" - original(\"%s\") = %p", name, h->original);
		}
	}
}

#endif	/* ZFISHER_H */

