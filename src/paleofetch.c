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

#define ESC 27
#define BUFFER32 32 * sizeof(char)
#define BUFFER64 64 * sizeof(char)
#define BUFFER256 256 * sizeof(char)
#define CPU "machdep.cpu.brand_string"
#define MEM_SIZE "hw.memsize"
#define HOSTNAME "kern.hostname"
#define PAGES "vm.pages"
#define LOGICAL_CPU "hw.logicalcpu"
#define MODEL "hw.model"
#define COUNT(x) (int)(sizeof x / sizeof *x)

#if defined(__MACH__) || defined(__APPLE__)
        #include "macintosh.c"
        #define OS_VERS "kern.osproductversion"
        #define KERN_VERS "kern.osrelease"
#endif
#define halt_and_catch_fire(fmt, status) \
    do { \
            if(status != 0) { \
                    fprintf(stderr, "paleofetch: " fmt "\n"); \
                    exit(status); \
            } \
    } while(0)

static char *get_colors1()
{
        char *colors1 = malloc(BUFFER256);
        char *s = colors1;

        for(int i = 0; i < 8; i++)
        {
                //why 3 spaces here?
                snprintf(s, 256, "\e[4%dm   ", i);
                s += 8;
        }
        strlcat(s, "\e[0m", BUFFER256);

        return colors1;
}
static char *get_colors2()
{
        char *colors2 = malloc(BUFFER256);
        char *s = colors2;

        for(int i = 8; i < 16; i++) {
                sprintf(s, "\e[48;5;%dm   ", i);
                s += 12 + (i >= 10 ? 1 : 0);
        }
        strlcat(s, "\e[0m", BUFFER256);

        return colors2;
}
/*static char *concat(const char *s1, const char *s2)
{
        size_t string_size = strlen(s1) + strlen(s2) + 1;
        char *result = malloc(string_size); // +1 for the null-terminator
        // in real code you would check for errors in malloc here
        strlcpy(result, s1, string_size);
        strlcat(result, s2, string_size);
        return result;
}*/
static char *get_sysctlbyname_info_str(const char *input)
{
        char *sysctl_info;
        size_t sysctl_info_length;
        sysctlbyname(input, NULL, &sysctl_info_length, NULL, 0);
        sysctl_info = malloc(sysctl_info_length);
        int n = sysctlbyname(input, sysctl_info, &sysctl_info_length, NULL, 0);
        if (n != 0)
        {
                halt_and_catch_fire("sysctlbyname error", 127);
                return 0;
        }
        return sysctl_info;
}
static char *get_resolution()
{
        short screen_width = CGDisplayPixelsWide(CGMainDisplayID());
        short screen_height = CGDisplayPixelsHigh(CGMainDisplayID());
        char *resolution = malloc(BUFFER64);
        snprintf(resolution, BUFFER64, "%hd%c%hd", screen_width, 'x', screen_height);
        return resolution;
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
        for(int i = 0; i <strlen(os_name);i++)
        {
                if(os_name[i] == '\n')
                        os_name[i] = '\0';
        }
        return os_name;

}
static char *get_sysctl_info_str(const int input1, const int input2)
{
        int mib[2] = {input1, input2};
        char *sysctl_info;
        size_t sysctl_info_lenght;
        sysctl(mib, 2, NULL, &sysctl_info_lenght, NULL, 0);
        sysctl_info = malloc(sysctl_info_lenght);
        int n = sysctl(mib, 2, sysctl_info, &sysctl_info_lenght, NULL, 0);
        if (n != 0)
        {
                halt_and_catch_fire("sysctl error", 127);
                return 0;
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
                halt_and_catch_fire("sysctl error", 127);
                return 0;
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
        short hours = time / 60;
        float minutes = ((time / 60) - hours) * 6000 / 100;
        char *ret_string = malloc(BUFFER32);
        char *hours_string = malloc(BUFFER32);
        char *minutes_string = malloc(BUFFER32);
        if (hours > 1 || hours == 0)
        {
                snprintf(hours_string, BUFFER32, "%hd %s", hours, "hours");
        }
        else
        {
                snprintf(hours_string, BUFFER32, "%hd %s", hours, "hour");
        }
        if (minutes > 1 || minutes == 0)
        {
                snprintf(minutes_string, BUFFER32, "%0.f %s", minutes, "minutes");
        }
        else
        {
                snprintf(minutes_string, BUFFER32, "%0.f %s", minutes, "minute");
        }
        snprintf(ret_string, BUFFER32 , "%s %s", hours_string, minutes_string);
        return ret_string;
}
//This is based on StackOverflow code and vm_stat source code from apple.
static int get_mem_from_vm_stat()
{
        mach_msg_type_number_t count = HOST_VM_INFO_COUNT;

        vm_statistics64_data_t vmstat;
        if (host_statistics (mach_host_self(), HOST_VM_INFO, (host_info_t) &vmstat, &count) != KERN_SUCCESS)
        {
                halt_and_catch_fire("Failed to get VM statistics.", 127);
        }
        // You may have noticed that USED RAM amount is not exactly the same as in activity
        // monitor, but i cannot really find a way to make this perfectly right. About +-100MB could be wrong.
        int total = (vmstat.compressor_page_count + vmstat.wire_count + vmstat.active_count + vmstat.speculative_count) /1024 * 4;
        return total;
}
static char *get_shell()
{
        char *shell      = malloc(BUFFER256);
        char *shell_path = getenv("SHELL");
        unsigned int len = (unsigned int) strlen(shell_path);
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
        size_t underline = strlen(userhost);
        char *ret_string = malloc(underline * sizeof(char)+1);
        int i = 0;
        for(; i < underline; i++)
        {
                ret_string[i] = '-';
        }
        ret_string[i+1] = '\n';
        return ret_string;
}
static char *get_ram_usage()
{
        long ram_size = get_sysctl_info_int(CTL_HW, HW_MEMSIZE);
        short ram_size_short = ram_size / (1024*1024);
        long used_memory = get_mem_from_vm_stat();
        char *ram_usage = malloc(BUFFER64);
        snprintf(ram_usage, BUFFER64, "%ldMB/%dMB %c%ld%s", used_memory, ram_size_short, '(', used_memory * 100/ram_size_short , "%)");
        return ram_usage;
}
static char *get_kernel()
{
        char *kernel = malloc(BUFFER64);
        strlcpy(kernel, "Darwin ", BUFFER64);
        strlcat(kernel, get_sysctl_info_str(CTL_KERN, KERN_OSRELEASE), BUFFER64);
        return kernel;
        
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
        snprintf(userhost, BUFFER256, "%s%s%s%s%s", "\e[1m", getenv("USER"), "\e[0m@\e[1m", hostname, "\e[0m");
        return userhost;
}
static char *complete_os()
{
        char *cmd_build = "sw_vers -buildVersion";
        char *cmd_name  = "sw_vers -productName";
        char *os        = malloc(BUFFER256);
        sprintf(os, "%s %s %s", get_os_name(cmd_name), get_sysctlbyname_info_str(OS_VERS),get_os_name(cmd_build));
        return os;
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
int main(int argc, const char *argv[])
{
        char *table_of_info[BUFFER256];
        struct utsname details;
        int ret = uname(&details);
        table_of_info[0]        = get_user_and_host(details.nodename);
        table_of_info[1]        = hostname_underline(details.nodename);
        table_of_info[2]        = get_shell();
        table_of_info[3]        = complete_os();
        table_of_info[4]        = get_kernel();
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

        for(int i = 0; i < COUNT(logo); i++)
        {
                printf("%s\e[0m ", logo[i]);
                if(table_of_info[i] != NULL)
                {
                        printf("%s" , table_of_info[i]);
                }
                printf("\n");
        }
        return ret;
}

