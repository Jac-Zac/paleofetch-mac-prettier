#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/sysctl.h>
#include <sys/utsname.h>

#include <CoreGraphics/CGDisplayConfiguration.h>

#include "logos/darwin.h"

#define ESC 27
#define BUFFER32 32
#define BUFFER256 256
#define CPU machdep.cpu.brand_string
#define LOGICAL_CPU hw.logicalcpu
#define PCHYSICAL_CPU hw.pchysicalcpu
#define MEM_SIZE hw.memsize
#define SWAP_USG vm.swapusage
#define BOOT_TIME kern.boottime

#if defined(__MACH__) || defined(__APPLE__) 
        const char *os = "macOS";
#endif
static void turn_bold_on()
{
        printf("%c[1m", ESC);
}
static void turn_bold_off()
{
        printf("%c[0m", ESC);
}
static char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}
static char* get_resolution()
{
        char *cmd ="system_profiler SPDisplaysDataType | grep Resolution";
        FILE *stdout_file = popen(cmd, "r");
        char *file_ret = malloc(BUFFER256);
        if(stdout_file)
        {
                fgets(file_ret, BUFFER256, stdout_file);
                pclose(stdout_file);
        }
        int i = 0;
        while(file_ret[i] != 'x') i++;
        int j = i - 5;
        int k = 0;
        char *resolution = malloc(BUFFER32);
        for(;j < i + 6; j++)
        {
                resolution[k]=file_ret[j];
                k++;
        }
        return resolution;
}
/*static char *check_shell_version(char *shell)
{
        char *shell_version = malloc(BUFFER256);
        char *shell_cmd = malloc(BUFFER32);
        strcpy(shell_cmd, shell);
        char *version = " --version";
        strcat(shell_cmd, version);
        FILE *stdout_file = popen(shell_cmd, "r");
        free(shell_cmd);
        if(stdout_file)
        {
                fgets(shell_version, BUFFER256, stdout_file);
                pclose(stdout_file);
        }
        return shell_version;

}*/
static char *get_os_name(char *cmd) 
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
static char *get_os_version()
{
        char *os_version = malloc(BUFFER32);
        size_t os_version_length = BUFFER32;
        int n = sysctlbyname("kern.osproductversion", os_version, &os_version_length, NULL, 0);
        if (n != 0) 
        {
                perror("sysctlbyname");
                return 0;
        }
        return os_version;
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
int main()
{
        struct utsname details;
        int ret = uname(&details);
        char *shell = get_shell();
        //char *shell_version = check_shell_version(shell);
        char *os_version = get_os_version();
        char *cmd_build = "sw_vers -buildVersion";
        char *cmd_name ="sw_vers -productName";
        char *os_name = get_os_name(cmd_name);
        char *os_build = get_os_name(cmd_build);
        char *resolution = get_resolution();
        if (ret==0)
        {
                printf("%s@%s\n", getenv("USER"), details.nodename);
                printf("-----------------\n");
                printf("Shell: %s\n", shell);
                printf("System: %s %s %s %s\n", os_name, os_version, os_build, details.machine);
                printf("Kernel: %s %s\n", details.sysname, details.release);
                printf("Terminal: %s\n", getenv("TERM_PROGRAM"));
                printf("Resolution: %s\n", resolution);
        }
        return ret;
}
