/* 
 * File:   util.h
 * Author: khoai
 *
 * Created on February 1, 2015, 9:18 AM
 */

#ifndef UTIL_H
#define	UTIL_H

#define ATOMIC_INCREASE1(x)	__sync_fetch_and_add(x, 1)
#define ATOMIC_DECREASE1(x)	__sync_fetch_and_sub(x, 1)
#define ATOMIC_READ(x)		__sync_fetch_and_add(x, 0)
#define ATOMIC_INCREASE(x, n)	__sync_fetch_and_add(x, n)
#define ATOMIC_DECREASE(x, n)	__sync_fetch_and_sub(x, n)

#define GRAY(s)   "\033[1;30m" s "\033[0m"
#define RED(s)    "\033[0;31m" s "\033[0m"
#define GREEN(s)  "\033[0;32m" s "\033[0m"
#define YELLOW(s) "\033[1;33m" s "\033[0m"

#define log(format, loglevel, ...) printf("%s " format "\n", loglevel, ## __VA_ARGS__)
#define log_err(format, ...)	log(format, GRAY("ERR"), ## __VA_ARGS__)
#define log_fatal(format, ...)	log(format, RED("FATAL"), ## __VA_ARGS__); exit(1)
#define log_warn(format, ...)	log(format, YELLOW("WARN"), ## __VA_ARGS__)
#define log_info(format, ...)	log(format, GREEN("INFO"), ## __VA_ARGS__)
#define log_hook(format, ...)	log(format, YELLOW("HOOK"), ## __VA_ARGS__)

#endif	/* UTIL_H */

