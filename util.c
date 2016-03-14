/* ----------------------------------------------------------------------- *
 *   The code for the functions rdmsr_on_cpu_0 and wrmsr_on_cpu_0 are
 *   originally part of msr-tools
 *   Copyright 2000 Transmeta Corporation - All Rights Reserved
 *   Copyright 2004-2008 H. Peter Anvin - All Rights Reserved
 *   Modifications by Clémentine Maurice on March 14, 2016
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 *   Boston MA 02110-1301, USA; either version 2 of the License, or
 *   (at your option) any later version; incorporated herein by reference.
 *    
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *   
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * ----------------------------------------------------------------------- */

#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>


#include "util.h"


/*
 * Read an MSR on CPU 0
 */
uint64_t rdmsr_on_cpu_0(uint32_t reg)
{
	uint64_t data;
	int cpu = 0;
	char * msr_file_name = "/dev/cpu/0/msr";
	
  static int fd = -1;  
  
  
	if (fd < 0) {
    fd = open(msr_file_name, O_RDONLY);
    if(fd < 0) {
		  if (errno == ENXIO) {
			  fprintf(stderr, "rdmsr: No CPU %d\n", cpu);
			  exit(2);
		  } else if (errno == EIO) {
			  fprintf(stderr, "rdmsr: CPU %d doesn't support MSRs\n",
				  cpu);
			  exit(3);
		  } else {
			  perror("rdmsr: open");
			  exit(127);
		  }
	  }
	}

	if (pread(fd, &data, sizeof data, reg) != sizeof data) {
		if (errno == EIO) {
			fprintf(stderr, "rdmsr: CPU %d cannot read "
				"MSR 0x%08"PRIx32"\n",
				cpu, reg);
			exit(4);
		} else {
			perror("rdmsr: pread");
			exit(127);
		}
	}

	//close(fd);

	return data;
}


/*
 * Write to an MSR on CPU 0
 */
void wrmsr_on_cpu_0(uint32_t reg, int valcnt, uint64_t *regvals)
{
  uint64_t data;
  char * msr_file_name = "/dev/cpu/0/msr";
  int cpu = 0;

	static int fd = -1;

	if(fd < 0){
	  fd = open(msr_file_name, O_WRONLY);
	  if (fd < 0) {
		  if (errno == ENXIO) {
			  fprintf(stderr, "wrmsr: No CPU %d\n", cpu);
			  exit(2);
		  } else if (errno == EIO) {
			  fprintf(stderr, "wrmsr: CPU %d doesn't support MSRs\n",
				  cpu);
			  exit(3);
		  } else {
			  perror("wrmsr: open");
			  exit(127);
		  }
	  }
	}

	while (valcnt--) {
		data=*regvals++;
		if (pwrite(fd, &data, sizeof data, reg) != sizeof data) {
			if (errno == EIO) {
				fprintf(stderr,
					"wrmsr: CPU %d cannot set MSR "
					"0x%08"PRIx32" to 0x%016"PRIx64"\n",
					cpu, reg, data);
				exit(4);
			} else {
				perror("wrmsr: pwrite");
				exit(127);
			}
		}
	}

	//close(fd);

	return;
}

