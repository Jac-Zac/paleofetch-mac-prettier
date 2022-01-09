SRC=src/paleofetch.c src/macintosh.c src/sysctl_info.c
BIN=-o paleofetch
CFLAGS=-Wall -Wextra -Wpedantic
FRAMEWORKS=-framework Cocoa -framework IOKit
CC=clang
OPT= -Ofast

file: clean 
		@echo "Compiling paleofetch, please wait..."
		@$(CC) $(SRC) $(CFLAGS) $(OPT) $(FRAMEWORKS) $(BIN)
install: file
		@sudo cp paleofetch /usr/local/bin
clean:
		@echo "Cleaning..."	
		@rm -f paleofetch
dump:
		@echo "Dumping..."
		@$(CC) $(SRC) -S
