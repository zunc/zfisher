# zfisher

* Easy to hooking application in *nix

## How to hook ?
* Google function prototype
* Write some code:
```
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
```
* Hook by one line
```
api_register(sz_open);
```
* Compile source
* Use it by command
```
LD_PRELOAD=$LIB_HOOK application
```

## Demo
* Hooking "open" API on /bin/cat:
```
khoai@khoai:bin$ LD_PRELOAD=.$LIB_HOOK /bin/cat /etc/issue
HOOK [init_fisher] pid: 21060
HOOK  - original("open") = 0x7f1b5b8a5150
HOOK open("/etc/issue")
Ubuntu 14.04.1 LTS \n \l
```
* Hooking "pread/pwrite" API on KyotoCabinet:
```
khoai@CPU11244:zkc$ LD_PRELOAD=$LIB_HOOK ./bin/zkcd debug
HOOK [init_fisher] pid: 8311
HOOK  - original("open") = 0x7f58f235fbd0
HOOK  - original("pread") = 0x7f58f235fc90
HOOK  - original("pwrite") = 0x7f58f235fcf0
--- kcTest  !
HOOK open("db/kctest.kch")
HOOK open("db/kctest.kch.wal")
HOOK [fini_fisher] pid: 8311
HOOK --- pread: summary
HOOK  - pread(cnt: 3983, total: 2045157)
HOOK  - pwrite(cnt: 997, total: 996792)
```