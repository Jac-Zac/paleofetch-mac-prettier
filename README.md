paleofetch-mac
==========

A rewrite of [neofetch](https://github.com/dylanaraps/neofetch) in C.
Currently showing only some info without logo.


Why use paleofetch over neofetch?
-----------------------------------------
One major reason is the performance improvement. For example: neofetch finishes running after about 2.2 seconds on my Mac where as paleofetch can finish running in a blazing fast 3 milliseconds(probably) with caching and .5s with no caching(due to slow acces to system_profiler which takes around 0.4s).

Note: this testing occured on only 1 computer, it's not a good representation on the performance benefit you may gain.

Note2: This is my first c program. I could make a lot of mistakes.

Example output:

![example output](example.png)

Dependencies
------------
Having a Mac.


Compiling
---------
For now i'm compiling single file, probably will configure `make` when i'll finish

FAQ
---

Q: Do you really run neofetch every time you open a terminal?  
ss7m: Yes, I like the way it looks and like that it causes my prompt to start midway
down the screen. I do acknowledge that the information it presents is not actually useful.
DB: Yea, me too that's why I forked it.
