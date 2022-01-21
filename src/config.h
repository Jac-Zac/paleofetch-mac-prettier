#define CONFIG \
{ \
    /* name            function                 cached  */\
    { "",             get_user_and_host,        true  }, \
    { "",             hostname_underline,       true  }, \
	{ "Shell: ",      get_shell,                true  }, \
	{ "OS: ",		  get_complete_os,          true  }, \
	{ "Kernel: ",     get_kernel,               true  }, \
    { "Machine: ",    get_machine,              true  }, \
	{ "Uptime: ",     get_uptime,               false }, \
	{ "Resolution: ", get_resolution,           true  }, \
	{ "Terminal: ",   get_terminal,             false }, \
    { "Battery: ",    get_battery_procentage,   false }, \
	{ "RAM: ", 		  get_ram_usage,		    false }, \
	{ "CPU: ",        get_cpu,                  true  }, \
	{ "GPU: ",        get_gpu,                  true  }, \
	SPACER \
	{ "",             get_colors1,              true  }, \
	{ "",             get_colors2,              true  }, \
}

