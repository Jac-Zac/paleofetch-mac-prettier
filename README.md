# paleofetch-bsd

A rewrite of [neofetch](https://github.com/dylanaraps/neofetch) in C started by [ss7m for linux](https://github.com/ss7m/paleofetch).

Not much code was left, but name is beautiful.

## IMPORTANT

This is developed on M1 arm64. It should work on x86_64, but mistakes may happen. No support for powerpc, but maybe will add it.

## Why use paleofetch over neofetch?

One major reason is the performance improvement.

At least for now paleofetch execution time is around 0.05s while neofetch will execute at least a second.

The plan is to support all BSDs(FreeBSD, NetBSD, OpenBSD, maybe Dragonfly), but for now only macOS.

![example outpur](.gitlab/example.png)

## Dependencies

Having a Mac, or one of the BSDs, you could be very helpful.

## Compiling

Enter folder and type `make file` or `make`

If you don't have it just type `clang src/paleofetch.c -o paleofetch  -framework Cocoa -framework IOKit`

There is also option `make clean` to clean and `make dump` to make assembly code for debuging

## FAQ

**Q**: Do you really run neofetch every time you open a terminal?  

**ss7m**: Yes, I like the way it looks and like that it causes my prompt to start midway
down the screen. I do acknowledge that the information it presents is not actually useful.

**DB**: Yea, me too that's why I forked it.
