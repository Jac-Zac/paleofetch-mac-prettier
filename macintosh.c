#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/sysctl.h>
#include <sys/utsname.h>

#include <CoreFoundation/CoreFoundation.h>

#include "logos/darwin.h"

#define ESC 27
#define BUFFER 256
#define CPU machdep.cpu.brand_string
#define LOGICAL_CPU hw.logicalcpu
#define PCHYSICAL_CPU hw.pchysicalcpu
#define MEM_SIZE hw.memsize
#define SWAP_USG vm.swapusage
#define BOOT_TIME kern.boottime
#define OS_VERSION kern.osversion

#if defined(__MACH__) || defined(__APPLE__) 
        const char *os = "macOS";
#endif
void turn_bold_on()
{
        printf("%c[1m", ESC);
}
void turn_bold_off()
{
        printf("%c[0m", ESC);
}
static char get_os_version()
{
        char os[BUFFER] ={'\0'};
        FILE *os_name = popen("sw_vers -productName", "r");
        sprintf(os, "%s" , os_name);
        pclose(os_name);
        printf("%s", os[0]);
        return 'c';
        

}
static char *get_shell()
{
        char *shell = malloc(5);
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
        char *get_os = get_os_version();
        if (ret==0)
        {
                printf("%s@%s\n", getenv("USER"), details.nodename);
                printf("-----------------\n");
                printf("Shell: %s\n", shell);
                printf("System: %s %s\n", get_os, details.machine);
                printf("Kernel: %s %s\n", details.sysname, details.release);
                printf("Terminal: %s\n", getenv("TERM_PROGRAM"));
        }
        return ret;
}
