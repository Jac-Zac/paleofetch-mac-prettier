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

#if defined(__MACH__) || defined(__APPLE__) 
        #include "macintosh.c"
        #define DE "\e[1mDE:\e[0m Aqua"
        #define OS_VERS "kern.osproductversion"
#endif

#define ESC 27
#define BUFFER32 32 * sizeof(char)
#define BUFFER64 64 * sizeof(char)
#define BUFFER256 256 * sizeof(char)
#define CPU "machdep.cpu.brand_string"
#define MEM_SIZE "hw.memsize" 
#define HOSTNAME "kern.hostname"
#define PAGES "vm.pages"
//#define LOGICAL_CPU hw.logicalcpu
#define MODEL "hw.model"
#define COUNT(x) (int)(sizeof x / sizeof *x)

#define halt_and_catch_fire(fmt, status) \
	do { \
        	if(status != 0) { \
            		fprintf(stderr, "paleofetch: " fmt "\n"); \
            		exit(status); \
        	} \
	} while(0)

/*static char *bold_and_color_string_constructor(const char *input, short color)
{
        char *ret_sring = malloc(BUFFER64);
        char *color = malloc(BUFFER32);
        snprintf(color, BUFFER32, "%s%hd%s", "\e[38;5;", color, ";1m");
        char *reset = "\e[0m";
        snprintf(ret_string, BUFFER64, "%s%s%s", color, input, reset);
}*/
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
static char* concat(const char *s1, const char *s2)
{
        size_t string_size = strlen(s1) + strlen(s2) + 1;
        char *result = malloc(string_size); // +1 for the null-terminator
        // in real code you would check for errors in malloc here
        strlcpy(result, s1, string_size);
        strlcat(result, s2, string_size);
        return result;
}
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
                halt_and_catch_fire("sysctl error", 127);
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
                halt_and_catch_fire("sysctl error", 127);
                return 0;
        }
        return sysctl_info;
}
//This code is from stackoverflow, no idea what it is apart it gets memory pages from somewhere.
static int get_mem_from_vm_stat()
{
        int pagesize = get_sysctl_info_int(CTL_HW, HW_PAGESIZE) / 1024;

        mach_msg_type_number_t count = HOST_VM_INFO_COUNT;

        vm_statistics64_data_t vmstat;
        if (host_statistics (mach_host_self(), HOST_VM_INFO, (host_info_t) &vmstat, &count) != KERN_SUCCESS)
        {
                halt_and_catch_fire("Failed to get VM statistics.", 127);
        }

        unsigned int total = ((vmstat.compressor_page_count + vmstat.wire_count + vmstat.active_count + vmstat.speculative_count) * 4) / (1024);
        total += vmstat.purgeable_count / (1024 * 1024);
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
static char *hostname_underline(const char *input)
{       
        /*Composing username@hostname second time, 
        * to properly calculate string lenght without ESC chars etc*/
	
        char *userhost = malloc(BUFFER256);
        size_t string_size = BUFFER256;
        snprintf(userhost, string_size, "%s%c%s", getenv("USER"), '@', input);
        size_t underline = strlen(userhost);
	char *ret_string = malloc(underline * sizeof(char));
	int i = 0;
        for(; i < underline; i++)
        {
                ret_string[i] = '-';
        }
	ret_string[i+1] = '\n';
	return ret_string;
}
static int count_used_memory()
{
        int used_memory = get_mem_from_vm_stat();
        return used_memory;
}
static char *get_ram_usage()
{
        long ram_size = get_sysctl_info_int(CTL_HW, HW_MEMSIZE);
        short ram_size_short = ram_size / (1024*1024);
        int used_memory = count_used_memory();
        char *ram_usage = malloc(BUFFER64);
        snprintf(ram_usage, BUFFER64, "%dMB/%dMB %d%c", used_memory, ram_size_short, used_memory * 100/ram_size_short , 37);
        return ram_usage;
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
        char *cmd_name ="sw_vers -productName";
        char *os = malloc(BUFFER256);
        sprintf(os, "%s %s %s", get_os_name(cmd_name), get_sysctlbyname_info_str(OS_VERS),get_os_name(cmd_build));
        return os;
}
int main(int argc, char *argv[])
{
	char *table_of_info[BUFFER256];
        struct utsname details;
        int ret = uname(&details);
        table_of_info[0] = get_user_and_host(details.nodename); 
	table_of_info[1] = hostname_underline(details.nodename);
        table_of_info[2] = get_shell();
        table_of_info[3] = complete_os(); 
        table_of_info[4] = get_sysctl_info_str(CTL_HW, HW_MODEL);
        table_of_info[5] = get_sysctlbyname_info_str(CPU);
        table_of_info[7] = get_ram_usage();
        table_of_info[9] = get_resolution_and_gpu();
        short used_memory = count_used_memory();
        table_of_info[10] = get_colors1();
        table_of_info[11] = get_colors2();

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
