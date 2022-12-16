/* ----------------------------------------------------------------------- *
 *
 *   Copyright 2016 Clémentine Maurice - All Rights Reserved
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 *   Boston MA 02110-1301, USA; either version 2 of the License, or
 *   (at your option) any later version. *    
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
 
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/perf_event.h>
#include <asm/unistd.h>
#include <sys/mman.h>
#include <assert.h>
#include <sys/ioctl.h>
#include <string.h>

#include "monitoring.h"
#include "util.h"

#define ARRAY_SIZE (1024*1024)
#define BUFFER_SIZE (256)


static long perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
			    int cpu, int group_fd, unsigned long flags)
{
	int ret;

	ret = syscall(__NR_perf_event_open, hw_event, pid, cpu,
	               group_fd, flags);
	return ret;
}


/* from https://stackoverflow.com/questions/39448276/how-to-use-clflush */
/* from Reverse Engineering Intel Last-Level Cache Complex Addressing Using Performance Counters */
void polling ( char* addr ){
  for ( int i =0; i < 100000 ; i ++){
    asm volatile ("clflush (%0)" :: "r"(addr));
  }
}
/* from https://github.com/clementine-m/msr-uncore-cbo/ */
void monitor_cbo(){

  // setup counters
  struct perf_event_attr pe[nb_cores];

  char* array=malloc(ARRAY_SIZE);
  char* values=malloc(ARRAY_SIZE/64);

  long pe_fd[nb_cores];

  for(int i=0; i<nb_cores; i++){
    // try to get the type from sysfs
    char buffer[BUFFER_SIZE];
    sprintf(buffer,"/sys/bus/event_source/devices/uncore_cbox_%d/type",i);
    int fd=open(buffer,O_RDONLY);
    assert(fd>0);
    assert((read(fd, buffer, BUFFER_SIZE)!=-1));
    int type=atoi(buffer);
    memset(&pe[i],0,sizeof(struct perf_event_attr));
    pe[i].type=type;
    pe[i].config=val_select_evt_core;
    pe[i].disabled=1; 
    close(fd);
  }

  for(int i=0; i<nb_cores; i++){
    pe_fd[i]=perf_event_open(&pe[i],-1,0,-1,0);
  }

  for (char* address=array;address<&array[ARRAY_SIZE];address+=64){
  // Reset and enable
    for(int i=0; i<nb_cores; i++){
      ioctl(pe_fd[i], PERF_EVENT_IOC_RESET, 0);
      ioctl(pe_fd[i], PERF_EVENT_IOC_ENABLE, 0);
    }
  // Launch program to monitor
    polling(address);
    for(int i=0; i<nb_cores; i++){
  // Disable
      ioctl(pe_fd[i], PERF_EVENT_IOC_DISABLE, 0);
    }

  // Read and find max
    long long maximum=0;
    char maximum_cbo=-1;

    for(int i=0; i<nb_cores; i++){
      long long value;
      read(pe_fd[i],&value,sizeof(long long));
      if (value>maximum){
        maximum=value;
        maximum_cbo=i;
      }
    }
    values[(address-array)/64]=maximum_cbo;
  }

  for(char* address=array;address<&array[ARRAY_SIZE];address+=64){
    printf(" %p %d", address,values[(address-array)/64]);
    printf("\n");
  }

  for(int i=0; i<nb_cores; i++){
    close(pe_fd[i]);
  }
}
