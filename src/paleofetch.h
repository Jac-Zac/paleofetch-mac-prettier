//
//  paleofetch.h 
//  paleofetch-mac
//
//  Created by DesantBucie on 07/04/2021.
//

#ifndef PALEOFETCH_H
#define PALEOFETCH_H
#include <sys/utsname.h>

static char     *get_colors1(),
                *get_colors2(),
                *get_uptime(),
                *get_shell(),
                *get_cpu(),
                *get_terminal(),
                *get_machine(),
                *spacer(),
                *get_os_name(),
                *hostname_underline(),
                *get_user_and_host();

static int64_t get_sysctl_info_int(const int input1, const int input2);

typedef unsigned long ulong;
struct utsname details;

#define BUFFER32 32 * sizeof(char)
#define BUFFER64 64 * sizeof(char)
#define BUFFER256 256 * sizeof(char)
#define BUFFER512 512 * sizeof(char)
#define CPU "machdep.cpu.brand_string"
#define MEM_SIZE "hw.memsize"
#define HOSTNAME "kern.hostname"
#define PAGES "vm.pages"
#define LOGICAL_CPU "hw.logicalcpu"
#define MODEL "hw.model"
#define COUNT(x) (uint)(sizeof x / sizeof *x)

#define halt_and_catch_fire(fmt, status) \
        do { \
                if(status != 0) { \
                        fprintf(stderr, "paleofetch: " fmt "\n"); \
                        exit(status); \
                } \
        } while(0)

#define SPACER {"", spacer, false},
#define REMOVE(A) { (A), NULL, sizeof(A) - 1 , 0 }
#define REPLACE(A, B) { (A), (B), sizeof(A) - 1, sizeof(B) - 1 }

#define CONFIG \
{ \
    /* name            function             cached */\
    { "",             get_user_and_host,    false }, \
    { "",             hostname_underline,    false }, \
	{ "Shell: ",      get_shell,            false }, \
	{ "Kernel: ",     get_kernel,           false }, \
    { "Machine: ",    get_machine,          false }, \
	{ "Uptime: ",     get_uptime,           false }, \
	{ "Resolution: ", get_resolution,       false }, \
	{ "Terminal: ",   get_terminal,         false }, \
	{ "CPU: ",        get_cpu,              false }, \
	{ "GPU: ",        get_gpu,              false }, \
    SPACER \
	{ "",             get_colors1,          false }, \
	{ "",             get_colors2,          false }, \
}


#define CPU_CONFIG \
{ \
	REMOVE("(R)"), \
	REMOVE("(TM)"), \
	REMOVE("Dual-Core"), \
	REMOVE("Quad-Core"), \
	REMOVE("Six-Core"), \
	REMOVE("Eight-Core"), \
	REMOVE("Core"), \
	REMOVE("CPU"), \
}

#define GPU_CONFIG \
{ \
	REMOVE("Corporation"), \
}

#endif
