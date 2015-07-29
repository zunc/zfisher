/*
 * fisher:          hook udp_io edition
 * use to monitor QUIC library
 * programmer:      zunc (dungcoivb@gmail.com)
 * last modified:   2015/8/29
 */

#define _GNU_SOURCE
#include "zfisher.h"
#include <sys/types.h>
#include <unistd.h>

//--- hook: recvfrom
char sz_recvfrom[] = "recvfrom";
//ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
//                 struct sockaddr *src_addr, socklen_t *addrlen);
typedef int (*hook_recvfrom)(int sockfd, void *buf, size_t len, int flags,
    void *src_addr, void *addrlen);
int cnt_recvfrom = 0;
long total_recvfrom = 0;

ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
    void *src_addr, void *addrlen) {
    cnt_recvfrom++;
    total_recvfrom += len;
    hook_recvfrom _ptr_orecvfrom =
        ((struct handler*) api_get(sz_recvfrom))->original;
    return _ptr_orecvfrom(sockfd, buf, len, flags, src_addr, addrlen);
}
//--- close hook: recvfrom

//--- open hook: sendto
char sz_sendto[] = "sendto";
//ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
//               const struct sockaddr *dest_addr, socklen_t addrlen);
typedef ssize_t(*hook_sendto)(int sockfd, void *buf, size_t len, int flags,
    void *dest_addr, unsigned int addrlen);
int cnt_sendto = 0;
long total_sendto = 0;

ssize_t sendto(int sockfd, void *buf, size_t len, int flags,
    void *dest_addr, unsigned int addrlen) {
    //log_hook("open(\"%s\")", filename);
    hook_sendto ptr_osendto = ((struct handler*) api_get(sz_sendto))->original;
    cnt_sendto++;
    total_sendto += len;
    return ptr_osendto(sockfd, buf, len, flags, dest_addr, addrlen);
}

//--- close hook: sendto

__attribute__((constructor))
void init() {
    log_hook("[init] pid: %d", getpid());
    api_register(sz_recvfrom);
    api_register(sz_sendto);
    hook_apis();
}

__attribute__((destructor))
void fini() {
    log_hook("[fini] pid: %d", getpid());
    log_hook("--- summary");
    log_hook(" - sendto(cnt: %d, total: %ld)", cnt_sendto, total_sendto);
    log_hook(" - recvfrom(cnt: %d, total: %ld)", cnt_recvfrom, total_recvfrom);
    //pony_dump();

    // free all
    // <!> stuff: too lazy to implement #.#
}

void _exit(int status) {
    log_hook("_exit");
}
