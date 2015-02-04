/* 
 * File:   pony.h
 * Author: dungln
 *
 * Created on February 3, 2015, 1:50 PM
 */

#ifndef PONY_H
#define	PONY_H

#include "list.h"
#include <assert.h>

struct profile {
	long time;
	void *data;
	struct list list;
};

typedef int (*log_parser)(void *log, char *buff, size_t size);

struct pony_interface {
	int profile_id;
	char *name;
	log_parser parser;
};

struct pony_profile {
	struct list list;
	char fused; // used flag
	struct pony_interface* handler;
};

#define MAX_PROFILE	64
#define USE_MS		0

static struct pony_profile pony[MAX_PROFILE]; // = LIST_HEAD_INIT(pony);

void pony_init() {
	int i;
	for (i = 0; i < MAX_PROFILE; i++) {
		pony[i].fused = 0;
		pony[i].handler = NULL;
	}
}

void pony_register(struct pony_interface* handler) {
	// pid : profile id
	assert(handler);
	assert(handler->profile_id < MAX_PROFILE);
	int pid = handler->profile_id;
	//LIST_HEAD_INIT
	pony[pid].list.n = pony[pid].list.p = &pony[pid].list;
	pony[pid].fused = 1;
	pony[pid].handler = handler;
}

void pony_push(int pid, long time, void *data) {
	struct profile* p = malloc(sizeof (struct profile));
	p->time = time;
	p->data = data;
	assert(pony[pid].fused);
	LIST_ADDQ(&pony[pid].list, &p->list);
}

void pony_dump() {
	int i;
	char buff[256] = {0};
	for (i = 0; i < MAX_PROFILE; i++) {
		struct pony_profile* pn = &pony[i];
		struct pony_interface* pi = pony[i].handler;
		if (pn->fused) {
			struct profile *p;
			long cnt = 0;
			long total_time = 0;
			double avg = 0;
#if USE_MS
			char unit[] = "ms";
#else 
			char unit[] = "us";
#endif
			char log_file[256] = {0};
			sprintf(log_file, "pn_%s.csv", pi->name);
			FILE *fle = fopen(log_file, "wt");
			if (!fle) {
				printf("fopen(%s) fail", log_file);
				return;
			}

			list_for_each_entry(p, &pn->list, list) {
#if USE_MS
				long time = p->time / 1e3;
#else
				long time = p->time;
#endif
				total_time += time;
				cnt++;
				pi->parser(p->data, buff, sizeof (buff));
				fputs(buff, fle);
				fprintf(fle, ",%ld\n", time);
			}
			fclose(fle);
			if (cnt) avg = (double) total_time / cnt;
			printf(" - %s(%ld): total(%ld %s), avg(%.2f %s)\n",
					pi->name, cnt, total_time, unit, avg, unit);
		}
	}
}

#endif	/* PONY_H */

