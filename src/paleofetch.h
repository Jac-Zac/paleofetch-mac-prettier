//
//  paleofetch.h 
//  paleofetch-mac
//
//  Created by DesantBucie on 07/04/2021.
//

#ifndef PALEOFETCH_H
#define PALEOFETCH_H

char            *get_colors1(void),
                *get_colors2(void),
                *get_uptime(void),
                *get_shell(void),
                *get_cpu(void),
                *get_terminal(void),
                *get_machine(void),
                *spacer(void),
                *hostname_underline(void),
                *get_user_and_host(void);

typedef unsigned long ulong;

#define COUNT(x) (uint)(sizeof x / sizeof *x)
#define SPACER {"", spacer, false},
//#define REMOVE(A) { (A), NULL, sizeof(A) - 1 , 0 }
//#define REPLACE(A, B) { (A), (B), sizeof(A) - 1, sizeof(B) - 1 }

#define CONFIG \
{ \
    /* name            function             cached */\
    { "",             get_user_and_host,    false }, \
    { "",             hostname_underline,   false }, \
	{ "Shell: ",      get_shell,            false }, \
	{ "OS: ",		  complete_os,          false }, \
	{ "Kernel: ",     get_kernel,           false }, \
    { "Machine: ",    get_machine,          false }, \
	{ "Uptime: ",     get_uptime,           false }, \
	{ "Resolution: ", get_resolution,       false }, \
	{ "Terminal: ",   get_terminal,         false }, \
	{ "RAM: ", 		  get_ram_usage,		false }, \
	{ "CPU: ",        get_cpu,              false }, \
	{ "GPU: ",        get_gpu,              false }, \
	SPACER \
	{ "",             get_colors1,          false }, \
	{ "",             get_colors2,          false }, \
}


/*#define CPU_CONFIG \
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
}*/

#endif //PALEOFETCH_H
