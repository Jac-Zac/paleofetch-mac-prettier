//
//  paleofetch.c
//  paleofetch-mac
//
//  Created by DesantBucie on 07/04/2021.
//

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

#include "paleofetch.h"
#include "sysctl_info.h"

#if defined(__MACH__) || defined(__APPLE__)
#include "macintosh.h"
#include "logos/macintosh_logo.h"
#endif


struct conf {
    char *label;    
    char *(*function)();
    bool cached;
} config[] = CONFIG;

char *get_colors1() {
    char *colors1 = malloc(BUFFER512);
    char *s = colors1;

    for(int i = 0; i < 8; i++) {
        sprintf(s, "\033[4%dm   ", i);
        s += 8;
    }
    snprintf(s, 5, "\033[0m");

    return colors1;
}

char *get_colors2() {
    char *colors2 = malloc(BUFFER512);
    char *s = colors2;

    for(int i = 8; i < 16; i++) {
        sprintf(s, "\033[48;5;%dm   ", i);
        s += 12 + (i >= 10 ? 1 : 0);
    }
    snprintf(s, 5, "\033[0m");

    return colors2;
}
char *get_uptime()
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
        if(days > 0)
            snprintf(days_string, BUFFER64, "%u %s", days, (days > 1 ? "days" : "day"));
        if(hours > 0)
            snprintf(hours_string, BUFFER64, "%u %s", hours, (hours == 0 || hours > 1 ? "hours" : "hour"));
        snprintf(minutes_string, BUFFER64, "%u %s", minutes, (minutes == 0 || minutes > 1 ? "minutes" : "minute"));
        snprintf(ret_string, BUFFER64 , "%s %s %s", days_string, hours_string, minutes_string);
        
        free(days_string); free(hours_string); free(minutes_string);
        
        return ret_string;    
}
char *get_shell()
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
char *hostname_underline()
{
        // Composing username@hostname second time,
        // to properly calculate string lenght without ESC chars etc
    
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
/* int check_for_pkg_info()
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
 char *look_for_package_managers()
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
 char *get_user_and_host()
{
        char *userhost = malloc(BUFFER256);;
        snprintf(userhost, BUFFER256, "%s%s%s%s%s", "\033[1m", getenv("USER"), "\033[0m@\033[1m", details.nodename, "\033[0m");
        return userhost;
}
/* void cache info()
{
        FILE *cache_file;
        if(cache_file = fopen("~/.cache/paleofetch", "r")!=NULL)
        }
        else
        {
                
        }
}*/
char *get_cpu(){
    return get_sysctlbyname_info_str(CPU);
}
char *get_terminal(){
    return getenv("TERM_PROGRAM");
}
char *spacer(){
    return " ";
}
char *get_machine(){
    return get_sysctl_info_str(CTL_HW, HW_MODEL);
}
int main()
{       
        int ret = uname(&details);

        uint logo_size = sizeof logo / sizeof *logo;
        uint config_size = sizeof config / sizeof *config;
        //sets which size we should base our iteration on, logo size or info size.
        uint which_bigger = logo_size > config_size ? logo_size : config_size;

        for(uint i = 0; i < which_bigger; i++)
        {
            // if i > count, we will print only logo, as we already are iterating on it's size.
            if(i >= COUNT(config)) {
                printf("%s\033[0m", logo[i]);
            }
            // If we run out of logo, but we have still info to print, we will have to 
            // print spaces corresponding to logos width, and then print our info.
            else if( i >= COUNT(logo) && i <= COUNT(config)) {
                for(int j = 0; j < logo_line_lenght; j++){
                    printf("%c",' ');
                }
                printf("\033[1m%s\033[0m%s" , config[i].label, config[i].function());
            }
            // The only option left is that we have both info and logo to print.
            else { 
                printf("%s\033[0m ", logo[i]);
                printf("\033[1m%s\033[0m%s" , config[i].label, config[i].function());
            }
            puts("");
        }
        return ret;
}

