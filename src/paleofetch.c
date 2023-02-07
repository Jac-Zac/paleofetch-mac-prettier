//
//  paleofetch.c
//  paleofetch-mac
//
//  Created by DesantBucie on 07/04/2021.
//
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/sysctl.h>

#include "config.h"
#include "paleofetch.h"
#include "sysctl_info.h"

#if defined(__MACH__) || defined(__APPLE__)
#include "macintosh.h"
#include "logos/macintosh_logo.h"
#endif

struct conf {
    char *label;
    void (*function)();
    bool cached;
} config[] = CONFIG;

typedef struct {
    char str[256];
} String;

void print_hardware(void) {
    printf("┌─────────  \033[1;32mHardware Information\033[0m  ─────────┐");
}

void print_software(void) {
    printf("├─────────  \033[1;34mSoftware Information\033[0m  ─────────┤");
}

void print_end_info(void){
    printf("└──────────────────────────────────────────┘");
}

void get_colors_dots(void) {

    // Start spacing
    printf("%*c", 42, ' ');

        for (int i = 7; i >= 0; i--) {
        printf("\033[3%dm   ", i);
    }
    printf("\033[0m");
}

void get_colors1(char *colors1) {

    char *s = colors1;

    for(int i = 0; i < 8; i++) {
        sprintf(s, "\033[4%dm   ", i);
        s += 8;
    }
    strlcat(s, "\033[0m", 5);
}

void get_colors2(char *colors2) {

    char *s = colors2;

    for(int i = 8; i < 16; i++) {
        sprintf(s, "\033[48;5;%dm   ", i);
        s += 12 + (i >= 10 ? 1 : 0);
    }
    strlcat(s,"\033[0m", 5);
}

void get_uptime(char *ret_string)
{
    struct timeval boottime;
    size_t len = sizeof(struct timeval);
    int mib[2] = {CTL_KERN, KERN_BOOTTIME};
    int err = sysctl(mib, 2, &boottime, &len, NULL, 0);
    if (err != 0){
        halt_and_catch_fire("sysctl error", EXIT_FAILURE);
    }

    time_t const bsec = boottime.tv_sec, csec = time(NULL);
    float const time = difftime(csec, bsec) / 60;
    uint const days = time / (60 * 24);
    uint const hours = (uint)(time / 60) % 24;
    uint const minutes = (uint)time % 60;

    char days_string[BUFF_64] = {0};
    char hours_string[BUFF_64] = {0};
    char minutes_string[BUFF_64] = {0};

    if(days > 0)
        snprintf(days_string, BUFF_64, "%u %s", days, (days > 1 ? "days " : "day "));
    if(hours > 0)
        snprintf(hours_string, BUFF_64, "%u %s", hours, (hours == 0 || hours > 1 ? "hours " : "hour "));

    snprintf(minutes_string, BUFF_64, "%u %s", minutes, (minutes == 0 || minutes > 1 ? "minutes" : "minute"));
    snprintf(ret_string, BUFF_64 , "%s%s%s", days_string, hours_string, minutes_string);

}
void get_shell(char *shell)
{
        char *const shell_path = getenv("SHELL");
        // Manual says, don't touch original pointer
        char *s = shell_path;

        if(shell_path == NULL){
            strcpy(shell,"Unknown");
            return;
        }
        //After last '/' there will be name of the shell
        s = strrchr(s,'/');
        s++;
        strlcpy(shell,s, 8);
}
void hostname_underline(char *ret_string)
{
        // Composing <username>@<hostname> second time,
        // to properly calculate string lenght without ESC chars

        char userhost[BUFF_256];
        size_t const string_size = BUFF_256;

        snprintf(userhost, string_size, "%s%c%s", getenv("USER"), '@', details.nodename);
        size_t const underline = strlen(userhost);

        char *s = ret_string;
        for(size_t i = 0; i < underline; i++)
        {
                *s = '-';
                s++;
        }
        *s = '\0';
}
/* int check_for_pkg_info()
{
        int ret_int = 0;
        if(access("/opt/pkg/sbin/pkg_info"))
        {
                char *cmd = "pkg_info | wc -l";
                FILE *stdout_file = popen(cmd, "r");
                char *ret_str = malloc_s(BUFF_32);
                if (stdout_file)
                {
                        fgets(ret_str, BUFF_32, stdout_file);
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
        char *packages = malloc_s(BUFF_256);
        int number = check_for_pkg_info();
        if(number != 0)
        {
                snprintf(packages, BUFF_256, "%d", check_for_pkg_info());
                strlcat(packages, "\40pkg_info\41", BUFF_256);
        }
        return packages;
}
*/
void get_user_and_host(char *userhost)
{
    snprintf(userhost, BUFF_256, "%s%s%s%s%s", "\033[1m", getenv("USER"), "\033[0m@\033[1m", details.nodename, "\033[0m");
}
void *cache_file_path(){
    char *const path = malloc_s(BUFF_256);
    if(getenv("XDG_CACHE_HOME")){
        snprintf(path, BUFF_256, "%s/.cache/paleofetch", getenv("HOME"));
    }
    else {
        snprintf(path, BUFF_256, "%s/.paleofetch", getenv("HOME"));
    }
    return path;
}
int check_cache_file(_Bool const recache)
{
    FILE *cache_file;
    String test;
    void *path = cache_file_path();
    if(fopen(path, "r") == NULL || recache == true)
    {
        cache_file = fopen(path, "w");
        if(cache_file == NULL)
            return 1;
        for(uint i = 0; i < COUNT(config); i++){
            config[i].function(test.str);
            fprintf(cache_file,"%s|", test.str);
        }
        fclose(cache_file);
    }
    free(path);
    return 0;
}
char **get_cached_value(char **file_ret){
    void *path = cache_file_path();
    FILE *const cache_file = fopen(path, "r");
    free(path);
    if(cache_file == NULL){
        return NULL;
    }
    fseek(cache_file, 0L, SEEK_END);
    uint file_length = ftell(cache_file);
    fseek(cache_file, 0L, SEEK_SET);
    *file_ret = malloc_s(file_length);
    fgets(*file_ret, file_length, cache_file);
    fclose(cache_file);
    char **const list = malloc((COUNT(config)) * sizeof(char*));
    if(!list){
        free(file_ret);
        halt_and_catch_fire("Malloc error", 127);
    }
    char **list_ptr = list;
    char *file_ret_cpy = *file_ret;
    while ((*list_ptr = strsep(&file_ret_cpy, "|")) != NULL) {
        list_ptr++; // strsep manual example 1:1
    }
    return list;
}
void get_cpu(char *cpu){
    char *cpu_name = get_sysctlbyname_info_str(CPU);
    strlcpy(cpu, cpu_name, 256);
    free(cpu_name);
}
void get_terminal(char *terminal){
    if(getenv("TERM_PROGRAM") == NULL){
        if (getenv("TERM") == NULL){
            strcpy(terminal, "Unknown");
            return;
        }
        strcpy(terminal, getenv("TERM"));
        return;
    }
    strlcpy(terminal, getenv("TERM_PROGRAM"), BUFF_256);
}
void spacer(char *spacer){
    strcpy(spacer, " ");
}
void get_machine(char *machine){
    char *model = get_sysctl_info(CTL_HW, HW_MODEL);
    strlcpy(machine, model, BUFF_256);
    free(model);
}
int main(int argc, char **argv)
{

    int const ret = uname(&details);

    uint const logo_size = COUNT(logo);
    uint const config_size = COUNT(config);
    //sets which size we should base our iteration on, logo size or info size.
    uint const which_bigger = logo_size > config_size ? logo_size : config_size;

    if(argc == 1) {
        check_cache_file(false);
    }
    else{
        if(!strcmp(argv[1], "-r"))
            check_cache_file(true);
        else {
            printf("Usage: paleofetch [-r (recache)]\n");
            return 0;
        }
    }
    char **file_ret = malloc(sizeof(char *));
    char **cached_list = get_cached_value(file_ret);
    String test;

    for(uint i = 0; i < which_bigger; i++)
    {
        // if i > count, we will print only logo, as we already are iterating on it's size.
        if(i >= config_size) {
            printf("%s\033[0m", logo[i]);
        }
        // If we run out of logo, but we have still info to print, we will have to
        // print spaces corresponding to logos width, and then print our info.
        else if( i >= logo_size && i <= config_size) {
            for(int j = 0; j < logo_line_lenght; j++){
                printf("%c",' ');
            }
            printf("\033[1m%s\033[0m",config[i].label);
            if(config[i].cached == true && cached_list[i] != NULL){
                printf("%s", cached_list[i]);
            }
            else{
                config[i].function(test.str);
                printf("%s", test.str);
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
                config[i].function(test.str);
                printf("%s", test.str);
            }
        }

        puts("");
    }
    free(cached_list);
    free(*file_ret);
    free(file_ret);
    return ret;
}

