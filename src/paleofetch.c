#include <ctype.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <sys/sysctl.h>
#include <sys/utsname.h>

#if defined(__MACH__) || defined(__APPLE__) 
        #include "macintosh.h"
#endif

#define ESC 27
#define BUFFER32 32 * sizeof(char)
#define BUFFER64 64 * sizeof(char)
#define BUFFER256 256 * sizeof(char)
#define CPU "machdep.cpu.brand_string"
#define MEM_SIZE "hw.memsize" 
#define HOSTNAME "kern.hostname"
#define PAGES "vm.pages"
#define OS_VERS "kern.osproductversion"
//#define LOGICAL_CPU hw.logicalcpu
//#define PCHYSICAL_CPU hw.pchysicalcpu
//#define SWAP_USG vm.swapusage
//#define BOOT_TIME kern.boottime
#define MODEL "hw.model"

//First two functions from original paleofetch.
static char *get_colors1()
{
        char *colors1 = malloc(BUFFER256);
        char *s = colors1;

        for(int i = 0; i < 8; i++) 
        {
                //why 3 spaces here v ?
                snprintf(s, 256, "\e[4%dm   ", i);
                s += 8;
        }
        strlcat(s, "\e[0m", 5);

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
static char* concat(const char *s1, const char *s2)
{
        size_t string_size = strlen(s1) + strlen(s2) + 1;
        char *result = malloc(string_size); // +1 for the null-terminator
        // in real code you would check for errors in malloc here
        strlcpy(result, s1, string_size);
        strlcat(result, s2, string_size);
        return result;
}
static char *exec_system_profiler(const char *cmd)
{
        FILE *stdout_file = popen(cmd, "r");
        char *file_ret = malloc(BUFFER256);
        if(stdout_file)
        {
                fgets(file_ret, BUFFER256, stdout_file);
                pclose(stdout_file);
        }
        return file_ret;
}
static char *get_resolution_and_gpu()
{
        // Stupid solution but i didn't found out anything else
        char *cmd ="system_profiler -json SPDisplaysDataType | awk '/spdisplays_pixelresolution/ || /sppci_model/' | cut -d '\"' -f 4";
        char *file_ret = exec_system_profiler(cmd);
        int i = 0;
        while(file_ret[i] != 'x') i++;
        int j = i - 4;
        int k = 0;
        char *resolution = malloc(BUFFER32);
        for(;j < i + 5; j++)
        {
                if(file_ret[j] == 'x'|| (file_ret[j] >= 48 && file_ret[j] <= 57))
                        resolution[k]=file_ret[j];
                k++;
        }
        resolution[k]='\0';
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
        int mib[2];
        char *sysctl_info;
        size_t sysctl_info_lenght;
        mib[0] = input1;
        mib[1] = input2;
        sysctl(mib, 2, NULL, &sysctl_info_lenght, NULL, 0);
        sysctl_info = malloc(sysctl_info_lenght);
        int n = sysctl(mib, 2, sysctl_info, &sysctl_info_lenght, NULL, 0);
        if (n != 0) 
        {
                perror("sysctl error");
                return 0;
        }
        return sysctl_info;
}
static int64_t get_sysctl_info_int(const int input1, const int input2)
{
        int mib[2];
        mib[0] = input1;
        mib[1] = input2;
        int64_t sysctl_info;  
        size_t sysctl_info_length = sizeof(sysctl_info);
        int n = sysctl(mib, 2, &sysctl_info, &sysctl_info_length, NULL, 0);
        if (n != 0) 
        {
                perror("sysctl error");
                return 0;
        }
        return sysctl_info;
}
static int get_mem_from_vm_stat()
{
        int pagesize = get_sysctl_info_int(CTL_HW, HW_PAGESIZE);

        mach_msg_type_number_t count = HOST_VM_INFO_COUNT;

        vm_statistics_data_t vmstat;
        if (host_statistics (mach_host_self(), HOST_VM_INFO, (host_info_t) &vmstat, &count) != KERN_SUCCESS)
        {
                fprintf (stderr, "Failed to get VM statistics.");
        }

        int total = (vmstat.wire_count + vmstat.active_count + vmstat.inactive_count + vmstat.free_count) * pagesize / (1024 * 1024);
        return total;
}
static char *get_shell()
{
        char *shell = malloc(BUFFER256);
        char *shell_path = getenv("SHELL");
        int len = strlen(shell_path);
        while(shell_path[len] != '/')
        {
                shell = &shell_path[len];
                len--;
        }
        return shell;
}
static void print_underline(const char *input)
{       
        /*Composing username@hostname second time, 
        * to properly calculate string lenght without ESC chars etc*/
        char *userhost = malloc(BUFFER256);
        size_t string_size = BUFFER256;
        snprintf(userhost, string_size, "%s%c%s", getenv("USER"), '@', input);
        size_t underline = strlen(userhost);
        printf("%c[1m", ESC);
        for(int i = 0; i < underline; i++)
        {
                putchar('-');
        }
        printf("%c[0m", ESC);
}
static long get_ram_size()
{
        long ram_size = get_sysctl_info_int(CTL_HW, HW_MEMSIZE);
        short ram_size_short = ram_size / (1024*1024);
        return ram_size_short;
}
static short count_used_memory()
{
        int used_memory = get_mem_from_vm_stat();
        return used_memory;
}
/*static char *check_for_pkg_info()
{
        if(system("which pkg_info > /dev/null 2>&1"))
        {
                char *cmd = "pkg_info | wc -l";
                FILE *stdout_file = popen(cmd, "r");
                char *ret_str = malloc(BUFFER32);
                if (stdout_file)
                {
                        fgets(ret_str, BUFFER32, stdout_file);
                        pclose(stdout_file);
                }
        }
}
static char *look_for_package_manager()
{
        
}*/
static char *get_user_and_host(const char *hostname)
{
        char *userhost = malloc(BUFFER256);
        size_t string_size = BUFFER256;
        
        snprintf(userhost, string_size, "%s%s%s%s%s", "\e[1m", getenv("USER"), "\e[0m@\e[1m", hostname, "\e[0m");
        /*strlcpy(userhost, "\e[1m", string_size);
        strlcat(userhost, getenv("USER"), string_size);
        strlcat(userhost, "\e[0m@\e[1m", string_size);
        strlcat(userhost, get_sysctl_info_str(HOSTNAME), string_size);
        strlcat(userhost, "\e[0m", string_size);*/
        return userhost;
}
int main(int argc, char *argv[])
{
        struct utsname details;
        int ret = uname(&details);
        char *userhost = get_user_and_host(details.nodename); 
        char *shell = get_shell();
        char *os_version = get_sysctlbyname_info_str(OS_VERS);
        char *pc_name = get_sysctl_info_str(CTL_HW, HW_MODEL);
        char *cmd_build = "sw_vers -buildVersion";
        char *cmd_name ="sw_vers -productName";
        char *cpu_string = get_sysctlbyname_info_str(CPU);
        char *os_name = get_os_name(cmd_name);
        short ram = get_ram_size();
        char *os_build = get_os_name(cmd_build);
        char *resolution = get_resolution_and_gpu();
        short used_memory = count_used_memory();
        char *colors1 = get_colors1();
        char *colors2 = get_colors2();
        if (ret==0)
        {
                printf("%s\n", userhost);
                print_underline(details.nodename);
                printf("\nShell: %s\n", shell);
                printf("System: %s %s %s %s\n", os_name, os_version, os_build, details.machine);
                printf("Kernel: %s %s\n", details.sysname, details.release);
                printf("Host: %s\n", pc_name);
                printf("CPU: %s\n", cpu_string);
                printf("Memory: %hdMB / %hdMB \(%d%c%c\n", used_memory, ram, (used_memory *100 /ram), 37, 41);
                printf("Terminal: %s\n", getenv("TERM_PROGRAM"));
                printf("Resolution: %s\n\n", resolution);
                printf("%s\n", colors1);
                printf("%s\n\n", colors2);
        }
        return ret;
}
