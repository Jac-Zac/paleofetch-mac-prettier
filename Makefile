CFLAGS= -o paleofetch -Wall -Wextra -pedantic -framework Cocoa -framework IOKit

file: clean 
		@echo "Compiling paleofetch, please wait..."
		@clang src/paleofetch.c $(CFLAGS)
install: file
		@sudo cp paleofetch /usr/local/bin
clean:
		@echo "Cleaning..."	
		@rm -f paleofetch
		@rm -f paleofetch.s
dump:
		@echo "Dumping..."
		@clang src/paleofetch.c -S
