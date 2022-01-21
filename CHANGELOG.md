### v0.5a - 2022-01-20
* Removed many unnecessary allocations, instead copying function results to one String located in main
* Some work is being done to support IOKit calls more easily, but M1 seems different than Intel mac version(some headers are missing).
* battery on by default
* trying to make portable makefile, but if project grows this will be problematic

### v0.5 - 2022-01-09
* Fixed many possible memory leaks(memory is now freed)
* Added battery procentage for mac, but still problematic, for unknown reason
* Added malloc\_s which tries three times allocating if ptr = NULL, if not possible exits. 

### v0.4 - 2021-12-05
* Added Caching
* Tried to simplify code when possible.

### v0.3b - 2021-11-27
* merge get\_sysctl\_info\_int and get\_sysctl\_info\_str into one.
* fixed date printing two spaces, when minutes are equal to zero.

### v0.3 - 2021-11-22
* reworked program to work more like original(author was way smarter than me)
* Added printing labels by default

### v0.2b - 2021-11-02
* support for m1, m1 gpu etc
* fixed memory on m1(hopefully works on intel64 as well)
* added days in uptime

### v0.2a - 2021-09-16
* mac gpu support
* Cleaned code, moved macOS only code to macintosh.c
* Added architecutre to OS name

### v0.1f - 2021-04-21
* Managed to get system resolution from Cocoa, paleofetch is much much faster now

### v0.1e - 2021-04-20
* Changed licence, basic functionality achieved, now development will freeze, and I will learn math.

### v0.1d - 2021-04-18
* Added speculative pages to used memory. Chnages in README

### v0.1c - 2021-04-17
* Fixed reading memory, finally logo is printed.

### v0.1b - 2021-04-14
* Implemented faster getting stats from vm.

### v0.1a - 2021-04-12
* Minimum Needed to show info. Still in monochorome and code is a mess. No Caching.
