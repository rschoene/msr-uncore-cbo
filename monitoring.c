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

#include "monitoring.h"
#include "util.h"


void monitor_cbo(){
  int i;

  // Disable counters
  uint64_t val[] = {val_disable_ctrs};
  wrmsr_on_cpu_0(msr_unc_perf_global_ctr,1,val);


  // Reset counters
  val[0] = val_reset_ctrs;
  for(i=0; i<nb_cores; i++){
    wrmsr_on_cpu_0(msr_unc_cbo_per_ctr0[i],1,val);
  }

  // Select event to monitor
  val[0] = val_select_evt_core;
  for(i=0; i<nb_cores; i++){
    wrmsr_on_cpu_0(msr_unc_cbo_perfevtsel0[i], 1, val);
  }

  // Enable counting
  val[0] = val_enable_ctrs;
  wrmsr_on_cpu_0(msr_unc_perf_global_ctr, 1, val);


  // Launch program to monitor
  // TODO replace usleep by some function you want to monitor
  usleep(100000);

  // Read counter
  int * cboxes = calloc(max_slices, sizeof(int));
  for(i=0; i<nb_cores; i++){
    cboxes[i] = rdmsr_on_cpu_0(msr_unc_cbo_per_ctr0[i]);
  }

  // Pretty print
  for(i=0; i<nb_cores; i++){
    printf(" % 10d", cboxes[i]);
  }
  printf("\n");



  free(cboxes);
}
