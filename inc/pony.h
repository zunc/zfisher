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
	struct list list;
};

struct pony_profile {
	struct list list;
	char *name;
	char fused; // used flag
};

#define MAX_PROFILE	64
#define USE_MS		0

static struct pony_profile pony[MAX_PROFILE]; // = LIST_HEAD_INIT(pony);

void pony_init() {
	int i;
	for (i = 0; i < MAX_PROFILE; i++) {
		pony[i].fused = 0;
	}
}

void pony_register(int pid, char *name) {
	// pid : profile id
	assert(pid < MAX_PROFILE);
	//LIST_HEAD_INIT
	pony[pid].list.n = pony[pid].list.p = &pony[pid].list;
	pony[pid].name = strdup(name);
	pony[pid].fused = 1;
}

void pony_push(int pid, long time) {
	struct profile* p = malloc(sizeof (struct profile));
	p->time = time;
	assert(pony[pid].fused);
	LIST_ADDQ(&pony[pid].list, &p->list);
}

void pony_dump() {
	int i;
	for (i = 0; i < MAX_PROFILE; i++) {
		struct pony_profile* pn = &pony[i];
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

			list_for_each_entry(p, &pn->list, list) {
#if USE_MS
				long time = p->time / 1e3;
#else
				long time = p->time;
#endif
				total_time += time;
				cnt++;
				//printf(" %ld", time);
			}
			if (cnt) avg = (double) total_time / cnt;
			printf(" - %s(%ld): total(%ld %s), avg(%.2f %s)\n",
					pn->name, cnt, total_time, unit, avg, unit);
		}
	}
}

#endif	/* PONY_H */

