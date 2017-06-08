#SLOB BEST FIT PLAN OF ATTACK

After looking through slob.c in the /mm folder and reading through Chapter 12 of the Linux Kernel Development book it seems like the slob_alloc() function will be the primary function to modify for this assignment.

This can be determined by the comments from Matt Mackall (the original author I assume) where after determining which slob list to allocate from the function uses the list_for_each_entry macro.
From there it does a first fit check using the sp page pointer to see if that page has enough room. Theres an explicit "is there enough room on this page" comment that makes this sort of clear.

So my goal from there will be modify it so it finds the smallest suitable page to utilize for allocation instead.

Also for implementing the syscalls for memory debugging purposes I'll be adding that to the syscall table found in /arch/x86/syscalls/syscall_32.tbl
