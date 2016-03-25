#ifndef UTMPLIB_H
#define UTMPLIB_H

#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <utmp.h>

#define NRECS 16
#define NULLUT ((struct utmp* )NULL)
#define UTSIZE (sizeof(struct utmp))

static char utmpbuf[NRECS * UTSIZE];
static int num_recs;
static int cur_rec;
static int fd_utmp = -1 ;

int utmp_open(char *filename);
struct utmp* utmp_next();
int utmp_reload();
void utmp_close();

#endif // UTMPLIB_H

