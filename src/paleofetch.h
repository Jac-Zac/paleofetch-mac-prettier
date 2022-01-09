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

char            **get_cached_value(char **);

char            *cache_file_path(void);

int             check_cache_file(_Bool);

typedef unsigned long ulong;

#define COUNT(x) (uint)(sizeof x / sizeof *x)
#define SPACER {"", spacer, false},
//#define REMOVE(A) { (A), NULL, sizeof(A) - 1 , 0 }
//#define REPLACE(A, B) { (A), (B), sizeof(A) - 1, sizeof(B) - 1 }

#define CONFIG \
{ \
    /* name            function             cached       freeable?     */\
    { "",             get_user_and_host,        true  }, \
    { "",             hostname_underline,       true  }, \
	{ "Shell: ",      get_shell,                true  }, \
	{ "OS: ",		  complete_os,              true  }, \
	{ "Kernel: ",     get_kernel,               true  }, \
    { "Machine: ",    get_machine,              true  }, \
	{ "Uptime: ",     get_uptime,               false }, \
	{ "Resolution: ", get_resolution,           true  }, \
	{ "Terminal: ",   get_terminal,             false }, \
    /* 1:50 times, malloc heap crashes { "Battery: ",    get_battery_procentage,   false },*/ \
	{ "RAM: ", 		  get_ram_usage,		    false }, \
	{ "CPU: ",        get_cpu,                  true  }, \
	{ "GPU: ",        get_gpu,                  true  }, \
	SPACER \
	{ "",             get_colors1,              true  }, \
	{ "",             get_colors2,              true  }, \
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
