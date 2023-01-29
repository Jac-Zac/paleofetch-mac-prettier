//
//  paleofetch.h
//  paleofetch-mac
//
//  Created by DesantBucie on 07/04/2021.
//

#ifndef PALEOFETCH_H
#define PALEOFETCH_H

void            get_colors1(char *),
                get_colors2(char *),
                get_colors_dots(void),
                print_hardware(void),
                print_software(void),
                print_end_info(void),
                get_uptime(char *),
                get_shell(char *),
                get_cpu(char *),
                get_terminal(char *),
                get_machine(char *),
                spacer(char *),
                hostname_underline(char *),
                get_user_and_host(char *),
                *cache_file_path(void);

char            **get_cached_value(char **);

int             check_cache_file(_Bool);

#define COUNT(x) (uint)(sizeof x / sizeof *x)
#define SPACER {"", spacer, false},
//#define REMOVE(A) { (A), NULL, sizeof(A) - 1 , 0 }
//#define REPLACE(A, B) { (A), (B), sizeof(A) - 1, sizeof(B) - 1 }


/*#define CPU_CONFIG \
{ \
	REMOVE("(R)"), \
	REMOVE("(TM)"), \
	REMOVE("Dual-Core"), \
	REMOVE("Quad-Core"), \
	REMOVE("Six-Core"), \
	REMOVE("Eight-Core"), \
	REMOVE("Core"), \
	REMOVE("CPU"), \
}

#define GPU_CONFIG \
{ \
	REMOVE("Corporation"), \
}*/

#endif //PALEOFETCH_H

