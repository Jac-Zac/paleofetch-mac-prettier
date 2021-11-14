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

#include "paleofetch.h"
#include "sysctl_info.h"

//#if defined(__MACH__) || defined(__APPLE__)
        #include "macintosh.h"
//#endif


struct conf {
    char *label;    
    char *(*function)();
    _Bool cached;
} config[] = CONFIG;

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
static char *get_uptime()
{
        struct timeval boottime;
        size_t len = sizeof(boottime);
        int mib[2] = {CTL_KERN, KERN_BOOTTIME};
        sysctl(mib, 2, &boottime, &len, NULL, 0);
        time_t bsec = boottime.tv_sec, csec = time(NULL);
        float time = difftime(csec, bsec) / 60;
        uint days = time / (60 * 24);
        uint hours = (uint)(time / 60) % 24;
        uint minutes = (uint)time % 60;

        char *ret_string = malloc(BUFFER64);
        char *days_string = malloc(BUFFER64);
        char *hours_string = malloc(BUFFER64);
        char *minutes_string = malloc(BUFFER64);
        snprintf(days_string, BUFFER64, "%u %s", days, (days == 0 || days > 1 ? "days" : "day"));
        snprintf(hours_string, BUFFER64, "%u %s", hours, (hours == 0 || hours > 1 ? "hours" : "hour"));
        snprintf(minutes_string, BUFFER64, "%u %s", minutes, (minutes == 0 || minutes > 1 ? "minutes" : "minute"));
        snprintf(ret_string, BUFFER64 , "%s %s %s", days_string, hours_string, minutes_string);
        free(days_string); free(hours_string); free(minutes_string);
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
static char *hostname_underline()
{
        //
        // Composing username@hostname second time,
        // to properly calculate string lenght without ESC chars etc
        //
    
        char *userhost     = malloc(BUFFER256);
        size_t string_size = BUFFER256;
        snprintf(userhost, string_size, "%s%c%s", getenv("USER"), '@', details.nodename);
        size_t underline = strlen(userhost) * sizeof(char);
        free(userhost);
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
static char *get_user_and_host()
{
        char *userhost = malloc(BUFFER256);;
        snprintf(userhost, BUFFER256, "%s%s%s%s%s", "\033[1m", getenv("USER"), "\033[0m@\033[1m", details.nodename, "\033[0m");
        return userhost;
}
/*static void cache info()
{
        FILE *cache_file;
        if(cache_file = fopen("~/.cache/paleofetch", "r")!=NULL)
        }
        else
        {
                
        }
}*/
static char *get_cpu(){
    return get_sysctlbyname_info_str(CPU);
}
static char *get_terminal(){
    return getenv("TERM_PROGRAM");
}
static char *spacer(){
    return "\n";
}
static char *get_machine(){
    return get_sysctl_info_str(CTL_HW, HW_MODEL);
}
int main()
{       
        char *table_of_info[50];
        int ret = uname(&details);
        table_of_info[14]       = get_colors2();
        
        for(uint i = 0; i < COUNT(logo); i++)
        {
            if( i>= COUNT(config)) {
                printf("%s\033[0m\n", logo[i]);
            }
            else { 
                printf("%s\033[0m ", logo[i]);
                if(table_of_info[i] != NULL)
                {
                        printf("%s" , config[i].function());
                }
                puts("");
            }
        }
        return ret;
        
}

