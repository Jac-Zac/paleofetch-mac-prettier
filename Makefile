CFLAGS= -o paleofetch -Wall -Wextra -pedantic -framework Cocoa -framework IOKit
CC=clang
OPT= -Ofast

file: clean 
		@echo "Compiling paleofetch, please wait..."
		@CC src/paleofetch.c src/macintosh.c $(CFLAGS) $(OPT)
install: file
		@sudo cp paleofetch /usr/local/bin
clean:
		@echo "Cleaning..."	
		@rm -f paleofetch
		@rm -f paleofetch.s
dump:
		@echo "Dumping..."
		@CC src/paleofetch.c -S
