static char *get_title(),
            *get_bar(),
            *get_os(),
            *get_kernel(),
            *get_host(),
            *get_uptime(),
            *get_battery_percentage(),
            *get_packages_pacman(),
            *get_shell(),
            *get_resolution(),
            *get_terminal(),
            *get_cpu(),
            *get_gpu1(),
            *get_gpu2(),
            *get_memory(),
            *get_disk_usage_root(),
            *get_disk_usage_home(),
            *get_colors1(),
            *get_colors2(),
            *spacer();

#define SPACER {"", spacer, false},
#define REMOVE(A) { (A), NULL, sizeof(A) - 1 , 0 }
#define REPLACE(A, B) { (A), (B), sizeof(A) - 1, sizeof(B) - 1 }

#define CONFIG \
{ \
   /* name            function                 cached */\
	{ "",             get_title,               	false }, \
	{ "",             get_bar,                	false }, \
	{ "OS: ",         get_os,                  	true  }, \
	{ "Host: ",       get_host,                	true  }, \
	{ "Kernel: ",     get_kernel,              	false }, \
	{ "Uptime: ",     get_uptime,              	false }, \
	{ "Battery: ",    get_battery_percentage,  	false }, \
	{ "Packages: ",   get_packages,   		false }, \
	{ "Shell: ",      get_shell,             	false }, \
	{ "Resolution: ", get_resolution,        	false }, \
	{ "Terminal: ",   get_terminal,          	false }, \
	{ "CPU: ",        get_cpu,                 	true  }, \
	{ "GPU: ",        get_gpu1,                	true  }, \
	{ "Memory: ",     get_memory,              	false }, \
	{ "",             get_colors1,             	false }, \
	{ "",             get_colors2,             	false }, \
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
        int color1;
        int color2;
};
