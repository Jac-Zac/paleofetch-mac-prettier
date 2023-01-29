#define CONFIG \
{ \
    /* name            function                 cached  */\
	{ "",             print_hardware,              false  }, \
	SPACER \
    { " \033[33m\033[0m    ",     get_machine,              true  }, \
	{ " \033[33m󰻠\033[0m    ",     get_cpu,                  true  }, \
	{ " \033[33m﬙\033[0m    ",     get_gpu,                  true  }, \
	{ " \033[33m\033[0m    ",     get_ram_usage,		     false }, \
    { " \033[33m\033[0m    ",     get_battery_procentage,   false }, \
	SPACER \
	{ "",             print_software,              false  }, \
	SPACER \
	{ " \033[33m\033[0m    ",	    get_complete_os,          true  }, \
	{ " \033[33m\033[0m    ",     get_kernel,               true  }, \
	{ " \033[33m\033[0m    ",     get_terminal,             false }, \
	{ " \033[33m󱆃\033[0m    ",     get_shell,                true  }, \
	{ " \033[33m󱦟\033[0m    ",     get_uptime,               false }, \
	SPACER \
	{ "",             print_end_info,               false  }, \
	{ "",             get_colors_dots,              false  }, \
	/* { "",             get_colors1,              true  }, \ */\
	/* { "",             get_colors2,              true  }, \ */\
	SPACER \
}

    /*
    { "",             get_user_and_host,        true  },\
    { "",             hostname_underline,       true  }, \
    */\
	/* { "Resolution: ", get_resolution,           true  }, */\

