#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <sys/sysctl.h>
#include <sys/utsname.h>

#include "logos/darwin.h"

#define ESC 27
#define BUFFER32 32
#define BUFFER256 256
#define CPU "machdep.cpu.brand_string"
#define LOGICAL_CPU hw.logicalcpu
#define PCHYSICAL_CPU hw.pchysicalcpu
#define MEM_SIZE hw.memsize
#define SWAP_USG vm.swapusage
#define BOOT_TIME kern.boottime
#define MODEL "hw.model"

#if defined(__MACH__) || defined(__APPLE__) 
#endif
static void turn_bold_on()
{
        printf("%c[1m", ESC);
}
static void turn_bold_off()
{
        printf("%c[0m", ESC);
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
    char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}
static char *exec_system_profiler(char *cmd)
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
static char *get_sysctl_info(char *input)
{
        char *sysctl_info = malloc(BUFFER256);
        size_t sysctl_info_length = BUFFER256;
        int n = sysctlbyname(input, sysctl_info, &sysctl_info_length, NULL, 0);
        if (n != 0) 
        {
                perror("sysctlbyname");
                return 0;
        }
        return sysctl_info;
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
        turn_bold_on();
        for(int i = 0; i < underline; i++)
        {
                printf("%c", '-');
        }
        turn_bold_off();
}
int main()
{
        struct utsname details;
        int ret = uname(&details);
        char *userhost = malloc(BUFFER256);
        strcpy(userhost, getenv("USER"));
        strcat(userhost, "@");
        strcat(userhost, details.nodename);
        char *shell = get_shell();
        char *os_version = get_sysctl_info("kern.osproductversion");
        char *pc_name = get_sysctl_info("hw.model");
        char *cmd_build = "sw_vers -buildVersion";
        char *cmd_name ="sw_vers -productName";
        char *cpu_string = get_sysctl_info("machdep.cpu.brand_string");
        char *os_name = get_os_name(cmd_name);
        char *os_build = get_os_name(cmd_build);
        char *resolution = get_resolution_and_gpu();
        if (ret==0)
        {
                printf("%s\n", userhost);
                print_underline(userhost);
                printf("\nShell: %s\n", shell);
                printf("System: %s %s %s %s\n", os_name, os_version, os_build, details.machine);
                printf("Kernel: %s %s\n", details.sysname, details.release);
                printf("Host: %s\n", pc_name);
                printf("CPU: %s\n", cpu_string);
                printf("Terminal: %s\n", getenv("TERM_PROGRAM"));
                printf("Resolution: %s\n", resolution);
        }
        return ret;
}
