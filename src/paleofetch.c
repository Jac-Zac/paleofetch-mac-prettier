//
//  paleofetch.c
//  paleofetch-mac
//
//  Created by DesantBucie on 07/04/2021.
//
#include <ctype.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

#include <sys/sysctl.h>
#include <sys/utsname.h>

#include "paleofetch.h"

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

#if defined(__MACH__) || defined(__APPLE__)
        #include "macintosh.c"
#endif


static char *get_colors1() {
    char *colors1 = malloc(BUFFER512);
    char *s = colors1;

    for(int i = 0; i < 8; i++) {
        sprintf(s, "\033[4%dm   ", i);
        s += 8;
    }
    snprintf(s, 5, "\033[0m");

    return colors1;
}

static char *get_colors2() {
    char *colors2 = malloc(BUFFER512);
    char *s = colors2;

    for(int i = 8; i < 16; i++) {
        sprintf(s, "\033[48;5;%dm   ", i);
        s += 12 + (i >= 10 ? 1 : 0);
    }
    snprintf(s, 5, "\033[0m");

    return colors2;
}
static char *get_sysctlbyname_info_str(const char *input)
{
        char *sysctl_info;
        size_t sysctl_info_length;
        sysctlbyname(input, NULL, &sysctl_info_length, NULL, 0);
        sysctl_info = malloc(sysctl_info_length);
        int n = sysctlbyname(input, sysctl_info, &sysctl_info_length, NULL, 0);
        if (n != 0)
        {
                halt_and_catch_fire("sysctlbyname error", EXIT_FAILURE);
        }
        return sysctl_info;
}
static char *get_os_name(const char *cmd)
{
        FILE *stdout_file = popen(cmd, "r");
        char *os_name = malloc(8);
        if (stdout_file)
        {
                fgets(os_name, 8, stdout_file);
                pclose(stdout_file);
        }
        for (uint i = strlen(os_name); i != 0; i--)
        {
                if(os_name[i] == '\n')
                {
                        os_name[i] = '\0';
                        break;
                }      
        }
        return os_name;

}
static char *get_sysctl_info_str(const int input1, const int input2)
{
        int mib[2] = {input1, input2};
        size_t sysctl_info_lenght;
        sysctl(mib, 2, NULL, &sysctl_info_lenght, NULL, 0);
        char *sysctl_info = malloc(sysctl_info_lenght);
        int n = sysctl(mib, 2, sysctl_info, &sysctl_info_lenght + 1, NULL, 0);
        if (n != 0)
        {
                halt_and_catch_fire("sysctl error", EXIT_FAILURE);
        }
        return sysctl_info;
}
static int64_t get_sysctl_info_int(const int input1, const int input2)
{
        int mib[2] = {input1, input2};
        int64_t sysctl_info;
        size_t sysctl_info_length = sizeof(sysctl_info);
        int n = sysctl(mib, 2, &sysctl_info, &sysctl_info_length, NULL, 0);
        if (n != 0)
        {
                halt_and_catch_fire("sysctl error", EXIT_FAILURE);
        }
        return sysctl_info;
}
static void *get_sysctl_info_ptr(const int input1, const int input2)
{
        int mib[2] = {input1, input2};
        size_t sysctl_info_length;
        sysctl(mib, 2, NULL, &sysctl_info_length, NULL, 0);
        printf("%lu", sysctl_info_length);
        void *sysctl_info = malloc(sysctl_info_length);
        int n = sysctl(mib, 2, &sysctl_info, &sysctl_info_length + 1, NULL, 0);
        if (n != 0)
        {
                halt_and_catch_fire("sysctl error", EXIT_FAILURE);
        }
        return sysctl_info;
}
static char *get_uptime()
{
        struct timeval boottime;
        size_t len = sizeof(boottime);
        int mib[2] = {CTL_KERN, KERN_BOOTTIME};
        sysctl(mib, 2, &boottime, &len, NULL, 0);
        time_t bsec = boottime.tv_sec, csec = time(NULL);
        float time = difftime(csec, bsec);
        time = time / 60;
        uint hours = time / 60;
        float minutes = ((time / 60) - hours) * 6000 / 100;
        char *ret_string = malloc(BUFFER64);
        char *hours_string = malloc(BUFFER64);
        char *minutes_string = malloc(BUFFER64);
        snprintf(hours_string, BUFFER64, "%u %s", hours, (hours == 0 || hours > 1 ? "hours" : "hour"));
        snprintf(minutes_string, BUFFER64, "%0.f %s", minutes, (minutes == 0 || minutes > 1 ? "minutes" : "minute"));
        snprintf(ret_string, BUFFER64 , "%s %s", hours_string, minutes_string);
        return ret_string;
}
static char *get_shell()
{
        char *shell      = malloc(BUFFER256);
        char *shell_path = getenv("SHELL");
        uint len = (uint) strlen(shell_path);
        //Remove path
        while(shell_path[len] != '/')
        {
                shell = &shell_path[len];
                len--;
        }
        return shell;
}
static char *hostname_underline(const char *input)
{
        //
        // Composing username@hostname second time,
        // to properly calculate string lenght without ESC chars etc
        //
    
        char *userhost     = malloc(BUFFER256);
        size_t string_size = BUFFER256;
        snprintf(userhost, string_size, "%s%c%s", getenv("USER"), '@', input);
        size_t underline = strlen(userhost) * sizeof(char);
        char *ret_string = malloc(underline);
        uint i = 0;
        for(;i < underline; i++)
        {
                ret_string[i] = '-';
        }
        ret_string[i] = '\0';
        return ret_string;
}
/*static int check_for_pkg_info()
{
        int ret_int = 0;
        if(access("/opt/pkg/sbin/pkg_info"))
        {
                char *cmd = "pkg_info | wc -l";
                FILE *stdout_file = popen(cmd, "r");
                char *ret_str = malloc(BUFFER32);
                if (stdout_file)
                {
                        fgets(ret_str, BUFFER32, stdout_file);
                        pclose(stdout_file);
                }
                ret_int = atoi(ret_str);
        }
        else
        {
                printf("No pkg_info");
        }
        return ret_int;
}
static char *look_for_package_managers()
{
        char *packages = malloc(BUFFER256);
        int number = check_for_pkg_info();
        if(number != 0)
        {
                snprintf(packages, BUFFER256, "%d", check_for_pkg_info());
                strlcat(packages, "\40pkg_info\41", BUFFER256);
        }
        return packages;
}
*/
static char *get_user_and_host(const char *hostname)
{
        char *userhost = malloc(BUFFER256);;
        snprintf(userhost, BUFFER256, "%s%s%s%s%s", "\033[1m", getenv("USER"), "\033[0m@\033[1m", hostname, "\033[0m");
        return userhost;
}
/*static void cache info()
{
        FILE *cache_file;
        if(cache_file = fopen("~/.cache/paleofetch", "r")!=NULL)
        {
                
        }
        else
        {
                
        }
}*/
int main()
{       
        char *table_of_info[50];
        struct utsname details;
        int ret = uname(&details);
        table_of_info[0]        = get_user_and_host(details.nodename);
        table_of_info[1]        = hostname_underline(details.nodename);
        table_of_info[2]        = get_shell();
        table_of_info[3]        = complete_os(details.machine);
        table_of_info[4]        = get_kernel(details.release);
        table_of_info[5]        = get_sysctl_info_str(CTL_HW, HW_MODEL);
        table_of_info[6]        = get_sysctlbyname_info_str(CPU);
        table_of_info[7]        = get_uptime();
        table_of_info[8]        = get_ram_usage();
        table_of_info[9]        = getenv("TERM_PROGRAM");
        table_of_info[10]       = get_resolution();
        table_of_info[11]       = get_gpu();
        table_of_info[12]       = "";
        table_of_info[13]       = get_colors1();
        table_of_info[14]       = get_colors2();
        
        for(uint i = 15; i <= 20; i++){
            table_of_info[i] = "";
        }
        
        for(uint i = 0; i < COUNT(logo); i++)
        {
                printf("%s\033[0m ", logo[i]);
                if(table_of_info[i] != NULL)
                {
                        printf("%s" , table_of_info[i]);
                }
                printf("\n");
        }
        return ret;
}

