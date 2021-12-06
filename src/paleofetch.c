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
#include <sys/sysctl.h>

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
        size_t len = sizeof(struct timeval);
        int mib[2] = {CTL_KERN, KERN_BOOTTIME};
        int err = sysctl(mib, 2, &boottime, &len, NULL, 0);
        if (err != 0){
            halt_and_catch_fire("sysctl error", EXIT_FAILURE);
        }

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
            snprintf(days_string, BUFFER64, "%u %s", days, (days > 1 ? "days " : "day "));
        if(hours > 0)
            snprintf(hours_string, BUFFER64, "%u %s", hours, (hours == 0 || hours > 1 ? "hours " : "hour "));

        snprintf(minutes_string, BUFFER64, "%u %s", minutes, (minutes == 0 || minutes > 1 ? "minutes" : "minute"));
        snprintf(ret_string, BUFFER64 , "%s%s%s", days_string, hours_string, minutes_string);
        
        free(days_string); free(hours_string); free(minutes_string);
        
        return ret_string;    
}
char *get_shell()
{
        char *shell = getenv("SHELL");
        //Push pointer at the end
        shell += strlen(shell);
        //Remove full path, pointer -1, so "/" isn't included
        while(*(shell-1) != '/')
        {
                shell--;
        }
        return shell;
}
char *hostname_underline()
{
        // Composing username@hostname second time,
        // to properly calculate string lenght without ESC chars
    
        char *userhost     = malloc(BUFFER256);
        size_t string_size = BUFFER256;
        snprintf(userhost, string_size, "%s%c%s", getenv("USER"), '@', details.nodename);
        size_t underline = strlen(userhost) * sizeof(char);
        free(userhost);
        char *ret_string = malloc(underline);
        char *s = ret_string;
        for(int i = 0; i < (int)underline; i++)
        {
                *s = '-';
                s++;
        }
        *s = '\0';
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
static char *cache_file_path(){
        char *path = malloc(BUFFER256);
        if(getenv("XDG_CACHE_HOME")){
            snprintf(path, BUFFER256, "%s/.cache/paleofetch", getenv("HOME"));
        }
        else {
            snprintf(path, BUFFER256, "%s/.paleofetch", getenv("HOME"));
        }
        return path;
}
int check_cache_file(_Bool recache)
{
        FILE *cache_file;
        if(fopen(cache_file_path(), "r") == NULL || recache == true)
        {   
            cache_file = fopen(cache_file_path(), "w"); 
            if(cache_file == NULL)
                return 1;
            for(uint i = 0; i < COUNT(config); i++){
                fprintf(cache_file,"%s|", config[i].function());
            }
            fclose(cache_file);
        }
        return 0;
}
char **get_cached_value(){
    FILE *cache_file = fopen(cache_file_path(), "r");
    if(cache_file == NULL){
        return NULL;
    }
    char *file_ret = malloc(BUFFER512);
    fgets(file_ret, 512, cache_file);
    char *token;
    char **list = malloc(COUNT(config)+1 * sizeof(char*));
    char **list_cpy;
    list_cpy = list;
    token = strtok(file_ret, "|");
    while(token != NULL){
        *list_cpy = token;
        token = strtok(NULL,"|");
        list_cpy++;
    }
    list_cpy = list;
    while(*list_cpy != NULL){
        list_cpy++;
    }
    return list; 
}
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
    return get_sysctl_info(CTL_HW, HW_MODEL);
}
int main(int argc, char **argv)
{       
        
        int ret = uname(&details);
        uint logo_size = COUNT(logo);
        uint config_size = COUNT(config);
        //sets which size we should base our iteration on, logo size or info size.
        uint which_bigger = logo_size > config_size ? logo_size : config_size;
        if(argv[1] != NULL) {
            check_cache_file(true);
        }    
        else{ 
            check_cache_file(false);
        }
        char **cached_list = get_cached_value();

        for(uint i = 0; i < which_bigger; i++)
        {
            // if i > count, we will print only logo, as we already are iterating on it's size.
            if(i >= COUNT(config)) {
                printf("%s\033[0m", logo[i]);
            }
            // If we run out of logo, but we have still info to print, we will have to 
            // print spaces corresponding to logos width, and then print our info.
            else {
                if( i >= COUNT(logo) && i <= COUNT(config)) {
                    for(int j = 0; j < logo_line_lenght; j++){
                        printf("%c",' ');
                    }
                    printf("\033[1m%s\033[0m",config[i].label);
                    if(config[i].cached == true && cached_list[i] != NULL){
                        printf("%s", cached_list[i]);
                    }
                    else{
                        printf("%s", config[i].function());
                    }
                }
                // The only option left is that we have both info and logo to print.
                else { 
                    printf("%s\033[0m ", logo[i]);
                    printf("\033[1m%s\033[0m",config[i].label);
                    if(config[i].cached == true && cached_list[i] != NULL){
                        printf("%s", cached_list[i]);
                    }
                    else {
                        printf("%s", config[i].function());
                    }
                }
            }
            puts("");
        }
        return ret;
}

