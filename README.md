# paleofetch-mac-prettier

## This is a repo that adapted [`paleofetch-mac`](https://gitlab.com/DesantBucie/paleofetch-mac)
> I have added some aesthetic changes to make it look like my old neofetch config

#### Original README content
A rewrite of [neofetch](https://github.com/dylanaraps/neofetch) started by [ss7m for linux](https://github.com/ss7m/paleofetch).

Not much code was left, but name is beautiful and similar logic.

## IMPORTANT

This is developed on M1 arm64. It should work on x86_64, if not make an issue. No support for powerpc, but maybe will add it.

## Why use paleofetch over neofetch?

One major reason is the performance improvement.

At least for now paleofetch execution time is around 0.05s while neofetch will execute at least a second.

The plan is to support all BSDs(FreeBSD, NetBSD, OpenBSD, maybe Dragonfly), but for now only macOS.

![example output](.assets/example.png)

## Dependencies

Having a Mac, or one of the BSDs installed, you could be very helpful.

## Recaching

Paleofetch uses caching to run faster. To recache data run `paleofetch -r`

## Compiling

Enter folder and type `make` (can be bmake, gnumake also)

There is also option `make clean` to clean and `make dump` to make assembly code for debuging

## FAQ

**Q**: Do you really run neofetch every time you open a terminal?

**ss7m**: Yes, I like the way it looks and like that it causes my prompt to start midway
down the screen. I do acknowledge that the information it presents is not actually useful.

**DB**: Yea, me too that's why I forked it.
