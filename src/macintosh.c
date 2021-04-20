#pragma once
#include <mach/host_info.h>
#include <mach/mach_host.h>
#include <mach/task_info.h>
#include <mach/task.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/graphics/IOGraphicsLib.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreFoundation/CFDictionary.h>
#include <SystemConfiguration/SystemConfiguration.h>

struct color_numbers darwin_colors = {76,184};
char *logo[] =
{
"\e[38;5;76;1m                    'c.        ",
"\e[38;5;76;1m                 ,xNMM.        ",
"\e[38;5;76;1m               .OMMMMo         ",
"\e[38;5;76;1m               OMMM0,          ",
"\e[38;5;76;1m     .;loddo:' loolloddol;.    ",
"\e[38;5;76;1m   cKMMMMMMMMMMNWMMMMMMMMMM0:  ",
"\e[38;5;184;1m .KMMMMMMMMMMMMMMMMMMMMMMMWd.  ",
"\e[38;5;184;1m XMMMMMMMMMMMMMMMMMMMMMMMX.    ",
"\e[38;5;208;1m;MMMMMMMMMMMMMMMMMMMMMMMM:     ",
"\e[38;5;208;1m:MMMMMMMMMMMMMMMMMMMMMMMM:     ",
"\e[38;5;196;1m.MMMMMMMMMMMMMMMMMMMMMMMMX.    ",
"\e[38;5;196;1m kMMMMMMMMMMMMMMMMMMMMMMMMWd.  ",
"\e[38;5;129;1m .XMMMMMMMMMMMMMMMMMMMMMMMMMMk ",
"\e[38;5;129;1m  .XMMMMMMMMMMMMMMMMMMMMMMMMK. ",
"\e[38;5;39;1m    kMMMMMMMMMMMMMMMMMMMMMMd   ",
"\e[38;5;39;1m     ;KMMMMMMMWXXWMMMMMMMk.    ",
"\e[38;5;39;1m       .cooc,.    .,coo:.      ",
"\e[0m                               "
};
static char *exec_system_profiler(const char *cmd)
{
        FILE *stdout_file = popen(cmd, "r");
        char *file_ret = malloc(256 * sizeof(char));
        if(stdout_file)
        {
                fgets(file_ret, 256 * sizeof(char), stdout_file);
                pclose(stdout_file);
        }
        return file_ret;
}

