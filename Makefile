file: clean 
		@echo "Compiling paleofetch, please wait..."
		@clang src/paleofetch.c -o paleofetch  -framework Cocoa -framework IOKit
clean:
		@echo "Cleaning..."	
		@rm -f paleofetch
		@rm -f paleofetch.s
dump:
		@echo "Dumping..."
		@clang src/paleofetch.c -S
