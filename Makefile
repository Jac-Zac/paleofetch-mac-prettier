OBJ=paleofetch.o sysctl_info.o
CFLAGS=-Wall -Wextra -Wpedantic
CC=clang
OPT=-O3
TARGET=/usr/local/bin/
MAC_OBJ=macintosh.o
FRAMEWORKS=-framework Cocoa -framework IOKit

paleofetch: clean $(OBJ)
	@if [ "$$(uname -s)" == "Darwin" ]; then \
		$(CC) ${CFLAGS} $(OPT) -c src/macintosh.c; \
		$(CC) $(OBJ) $(MAC_OBJ) $(FRAMEWORKS) -o $@; \
	else \
		$(CC) $(OBJ) -o $@; \
	fi ;
install: paleofetch
	@mkdir -p $(TARGET)
	@if [ -x "$$(command -v doas)" ]; then \
		doas cp paleofetch $(TARGET); \
	else \
		sudo cp paleofetch $(TARGET); \
	fi
clean:
	@echo "Cleaning..."
	@rm -f paleofetch a.out *.o
dump:
	@echo "Dumping..."
	@$(CC) $(SRC) -S

%.o: src/%.c
	$(CC) $(CFLAGS) $(OPT) -c $<

sysctl_info.o: src/sysctl_info.c src/sysctl_info.h
paleofetch.o: src/paleofetch.c src/paleofetch.h src/config.h
macintosh.o: src/macintosh.c src/macintosh.h
