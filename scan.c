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
 
#define _GNU_SOURCE
#include <getopt.h>
#include <sched.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <cpuid.h>


#include "scan.h"
#include "util.h"
#include "monitoring.h"



void print_help() {
  fprintf(stderr, "  >> Usage: sudo ./scan -c nb_cores\n");
}


/*
 * Declare architecture- and core-dependent variables
 */

int nb_cores;
int max_slices;

unsigned long long msr_unc_perf_global_ctr;
unsigned long long * msr_unc_cbo_perfevtsel0;
unsigned long long * msr_unc_cbo_per_ctr0;
unsigned long long val_enable_ctrs;
unsigned long long val_disable_ctrs;
unsigned long long val_select_evt_core;
unsigned long long val_reset_ctrs;

static int get_cpu_architecture() {
    unsigned int model;
    int name[4] = {0, 0, 0, 0};
    __cpuid(0, model, name[0], name[2], name[1]);
    if(strcmp((char*)name, "GenuineIntel") != 0) return -1;
    return model;
}

int main(int argc, char **argv){

  /*
   * Pin to core
   */
  int mask = 0;
  cpu_set_t my_set;
  CPU_ZERO(&my_set);
  CPU_SET(mask, &my_set);
  sched_setaffinity(0, sizeof(cpu_set_t), &my_set);


  /*
   * Options
   */
  int opt,c;
  c=0;

  while ((opt = getopt(argc, argv, "c:h")) != -1){
    switch(opt) {
      case 'h':
        print_help();
        exit(1);
      case 'c':
        nb_cores = atoi(optarg);
        c = 1;
        break;
      default:
        print_help();
        exit(1);
    }
  }

  if(!c){
    print_help();
    exit(1);
  }

  if(get_cpu_architecture() >= 0x16) {
      // >= skylake
      msr_unc_perf_global_ctr = 0xe01;
      val_enable_ctrs = 0x20000000;
  } else {
      msr_unc_perf_global_ctr = 0x391;
      val_enable_ctrs = 0x2000000f;
  }
	/*
	 * Initialize architecture-dependent variables
	 */
    max_slices = 24;
    msr_unc_cbo_perfevtsel0 = (unsigned long long []) {0x700, 0x710, 0x720, 0x730};
    msr_unc_cbo_per_ctr0 = (unsigned long long []) {0x706, 0x716, 0x726, 0x736};
    val_disable_ctrs = 0x0;
    val_select_evt_core = 0x408f34;
    val_reset_ctrs = 0x0;


  /*
   * Verify number of cores is coherent with micro-architecture
   */
   if(nb_cores > max_slices){
     fprintf(stderr,"Specified number of cores (%d) incoherent with theoretical maximum number of core (%d). \n", nb_cores, max_slices);
     print_help();
     exit(1);
   }

	printf("Number of cores: %d\n", nb_cores);

  unsigned long long const nb_loops = 20;
  int i;


  /*
   * Monitor CBos
   */
  for(i=0; i<nb_loops; i++){
    monitor_cbo();
  }



  return 0;
}
