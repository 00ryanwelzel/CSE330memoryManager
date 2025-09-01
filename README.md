# memoryManager

## (Ryan Welzel 6/19/2025)

Linux kernel module that takes a PID and virtual mem addr then finds either the physical mem addr, the swap page, or indicates the page is not available.

---

Features:
- Locates the location from a process and its virtual address.


Requirements:
- Linux OS
- GCC compiler
- Swapfile


Enable swapfile with:
- sudo dd if=/dev/zero of=/mnt/1GiB.swap bs=1024

Note: run vm with low memory >=4GiB
