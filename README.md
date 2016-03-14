# Using MSRs to monitor the C-Boxes

This repository contains some tools to monitor the UNC_CBO_CACHE_LOOKUP event of the C-Boxes. 
The code is distributed under the GNU General Public License, version 2.

This code was used to build the paper "[Reverse Engineering Intel Last-Level Cache Complex Addressing Using Performance Counters](http://s3.eurecom.fr/docs/raid15_maurice.pdf)" by Clémentine Maurice, Nicolas Le Scouarnec, Christoph Neumann, Olivier Heen and Aurélien Francillon, published at RAID 2015.



## One note before starting

**Warning**: This code is provided as-is. It reads and writes to Model Specific Registers (MSRs). Randomly writing to your CPU MSRs can lead to potential damage to your system/CPU. You are responsible for protecting yourself, your property and data, and others from any risks caused by this code. 

This code should work on Intel Core CPUs, from Sandy Bridge to Skylake micro-architecture. The addresses of MSRs and values to write to MSRs are OS-independant, but this code is Linux-specific.



## "scan" program

The "scan" program outputs the performance counters for the UNC_CBO_CACHE_LOOKUP event for each CBo, every 0.1 second, 20 times.

To run it:

- Compile

```
$ make
```

- Charge the msr module (root)

```
$ sudo modprobe msr
```

- Run the scan program (root needed to access the MSRs), eg for a 2-core machine

```
$ sudo ./scan -c 2
```



## FAQ

**Q: You just told me that writing random values to the MSRs was potentially dangerous, where can I verify the values provided in this code?**

A: I am glad you asked! You're opening a small door to hell, but here you go:

- Uncore performance monitoring for Intel Core processors is described in [1] Section 18.8.6. The section is marked for Sandy Bridge, but same applies to microarchitectures Sandy Bridge to Skylake.
- MSR addresses for Intel Core processors can be found in [1] Section 35.8.1, Table 35-17.
- The values are derived from the MSR layout in [1] Section 18.8.6 and from the events described in [1] Section 19.6, Table 19-16.



[1] Intel® 64 and IA-32 Architectures Software Developer’s Manual, Volume 3. Order Number: 325384-057US. December 2015




