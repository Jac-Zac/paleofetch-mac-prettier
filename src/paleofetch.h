//
//  paleofetch.h 
//  paleofetch-mac
//
//  Created by DesantBucie on 07/04/2021.
//
static char     *get_colors1(),
                *get_colors2(),
		*get_uptime(),
                *get_shell(),
                *get_sysctlbyname_info_str(const char *input),
                *get_os_name(const char *cmd),
                *get_sysctl_info_str(const int input1, const int input2),
                *hostname_underline(const char *input),
                *get_user_and_host(const char *hostname);

static int64_t get_sysctl_info_int(const int input1, const int input2);

typedef unsigned long ulong;

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
        /* name            function                     cached */\
	{ "Kernel: ",     get_kernel,           false }, \
	{ "Uptime: ",     get_uptime,           false }, \
	{ "Shell: ",      get_shell,            false }, \
	{ "Resolution: ", get_resolution,       false }, \
	{ "Terminal: ",   get_terminal,         false }, \
	{ "CPU: ",        get_cpu,              true  }, \
	{ "GPU: ",        get_gpu,              true  }, \
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
struct color_numbers
{
        short color1;
        short color2;
};
