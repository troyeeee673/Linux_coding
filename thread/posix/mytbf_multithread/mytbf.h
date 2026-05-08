#ifndef __MYTBF_H
#define __MYTBF_H

#define MYTBF_MAX	1024
typedef void mytbf_t;

mytbf_t*  mytbf_init(int cps, int burst);

int mytbf_fetchToken(mytbf_t*, int);

int mytbf_returnToken(mytbf_t *, int);

int mytbf_destroy(mytbf_t*);

#endif

