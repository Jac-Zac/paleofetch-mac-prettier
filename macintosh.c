#include <ctype.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <sys/sysctl.h>
#include <sys/utsname.h>

#include "logos/darwin.h"

#define ESC 27
#define BUFFER32 32
#define BUFFER64 64
#define BUFFER256 256
#define CPU "machdep.cpu.brand_string"
#define LOGICAL_CPU hw.logicalcpu
#define PCHYSICAL_CPU hw.pchysicalcpu
#define SWAP_USG vm.swapusage
#define BOOT_TIME kern.boottime
#define MODEL "hw.model"

#if defined(__MACH__) || defined(__APPLE__) 
#endif
static void print_bold_string(const char *input)
{
       printf("%c[1m%s%c[0m", ESC, input, ESC); 
}
static char *get_colors1() {
    char *colors1 = malloc(BUFFER256);
    char *s = colors1;

    for(int i = 0; i < 8; i++) {
        sprintf(s, "\e[4%dm   ", i);
        s += 8;
    }
    snprintf(s, 5, "\e[0m");

    return colors1;
}
static char *get_colors2() {
    char *colors2 = malloc(BUFFER256);
    char *s = colors2;

    for(int i = 8; i < 16; i++) {
        sprintf(s, "\e[48;5;%dm   ", i);
        s += 12 + (i >= 10 ? 1 : 0);
    }
    snprintf(s, 5, "\e[0m");

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
/*static char *remove_double_spaces(char *input)
{
       * i    j   
         *[  ][  ][r][i][n][g]
         *
         *
        char * ret_string = malloc(BUFFER256);
        int i = 0;
        int j = 1;
        int k = 0;
        bool isEmpty = true;
        while(input[j])
        {
               if(input[i] == ' ' && input[j] == ' ')
               {
                        if(isEmpty)
                        {
                                i+=2;
                                j+=2;
                        }
                        else
                        {
                                ret_string[k];
                                k++;
                        }
               }
               else
               {
                       isEmpty = false;
                       ret_string[k] = input[i];
                       k++;
               }
        }
        return ret_string;
}*/
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
static char *get_sysctl_info_str(const char *input)
{
        char *sysctl_info = malloc(BUFFER64);
        size_t sysctl_info_length = BUFFER64;
        int n = sysctlbyname(input, sysctl_info, &sysctl_info_length, NULL, 0);
        if (n != 0) 
        {
                perror("sysctlbyname");
                return 0;
        }
        return sysctl_info;
}
static int64_t get_sysctl_info_int(const char *input)
{
        int64_t sysctl_info;  
        size_t sysctl_info_length = sizeof(sysctl_info);
        int n = sysctlbyname(input, &sysctl_info, &sysctl_info_length, NULL, 0);
        if (n != 0) 
        {
                perror("sysctlbyname");
                return 0;
        }
        return sysctl_info;
}
static int get_mem_from_vm_stat(const char *cmd)
{
        FILE *stdout_file = popen(cmd, "r");
        char *memory = malloc(BUFFER256);
        if (stdout_file)
        {
                fgets(memory, BUFFER256, stdout_file);
                pclose(stdout_file);
        }
        int j = 0;
        char *ret_memory = malloc(BUFFER64);
        for(int i = 0; i < strlen(memory); i++)
        {
                if(memory[i] >= 48 && memory[i] <= 57)
                {
                        ret_memory[j] = memory[i];        
                        j++;
                }
        }
        int ret_memory_int = atoi(ret_memory);
        return ret_memory_int;
}
static char *get_shell()
{
        char *shell = malloc(BUFFER256);
        char *shell_path = getenv("SHELL");
        int len = strlen(shell_path);
        int i = 0;
        while(shell_path[len] != '/')
        {
                shell = &shell_path[len];
                len--;
                i++;
        }
        return shell;
}
static void print_underline(const char *userhost)
{       
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
        long ram_size = get_sysctl_info_int("hw.memsize");
        short ram_size_short = ram_size / (1024*1024);
        return ram_size_short;
}
static short count_used_memory()
{
        int get_memory_active = get_mem_from_vm_stat("vm_stat | grep 'active'|grep -v 'inactive'");
        int get_memory_wired =  get_mem_from_vm_stat("vm_stat | grep 'wired'");
        int get_memory_occupied = get_mem_from_vm_stat("vm_stat | grep 'occupied'");
        short used_memory = (get_memory_active + get_memory_wired + get_memory_occupied) * 4 / 1024;
        return used_memory;
}
static char *look_for_package_manager()
{

}
int main(int argc, char *argv[])
{
        struct utsname details;
        int ret = uname(&details);
        char *userhost = malloc(BUFFER256);
        strlcpy(userhost, getenv("USER"),BUFFER256);
        strlcat(userhost, "@", BUFFER256);
        strlcat(userhost, details.nodename, BUFFER256);
        char *shell = get_shell();
        char *os_version = get_sysctl_info_str("kern.osproductversion");
        char *pc_name = get_sysctl_info_str("hw.model");
        char *cmd_build = "sw_vers -buildVersion";
        char *cmd_name ="sw_vers -productName";
        char *cpu_string = get_sysctl_info_str("machdep.cpu.brand_string");
        char *os_name = get_os_name(cmd_name);
        short ram = get_ram_size();
        char *os_build = get_os_name(cmd_build);
        char *resolution = get_resolution_and_gpu();
        short used_memory = count_used_memory();
        if (ret==0)
        {
                printf("%s\n", userhost);
                print_underline(userhost);
                printf("\nShell: %s\n", shell);
                printf("System: %s %s %s %s\n", os_name, os_version, os_build, details.machine);
                printf("Kernel: %s %s\n", details.sysname, details.release);
                printf("Host: %s\n", pc_name);
                printf("CPU: %s\n", cpu_string);
                printf("Memory: %dMB / %hdMB\n", used_memory, ram);
                printf("Terminal: %s\n", getenv("TERM_PROGRAM"));
                printf("Resolution: %s\n", resolution);
        }
        return ret;
}
