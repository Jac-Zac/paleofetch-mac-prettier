SRC=src/paleofetch.c src/macintosh.c src/sysctl_info.c
OBJ=paleofetch.o macintosh.o sysctl_info.o 
BIN=-o paleofetch
CFLAGS=-Wall -Wextra -framework Cocoa -framework IOKit
CC=clang
OPT= -Ofast

file: clean 
		@echo "Compiling paleofetch, please wait..."
		@$(CC) $(SRC) $(CFLAGS) $(OPT) $(BIN)
install: file
		@sudo cp paleofetch /usr/local/bin
clean:
		@echo "Cleaning..."	
		@rm -f paleofetch
dump:
		@echo "Dumping..."
		@$(CC) $(SRC) -S
