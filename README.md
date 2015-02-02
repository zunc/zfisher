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
LD_PRELOAD=libzhookd.so application
```

## Demo
* Hooking open API on /bin/cat:
```
khoai@khoai:bin$ LD_PRELOAD=./libzhookd.so /bin/cat /etc/issue
HOOK [init_fisher] pid: 21060
HOOK  - original("open") = 0x7f1b5b8a5150
HOOK open("/etc/issue")
Ubuntu 14.04.1 LTS \n \l
```
