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
 
extern int nb_cores;
extern int max_slices;
extern int nb_pokes;

// Core MSRs and values
extern unsigned long long msr_unc_perf_global_ctr;
extern unsigned long long * msr_unc_cbo_perfevtsel0;
extern unsigned long long * msr_unc_cbo_per_ctr0;
extern unsigned long long val_enable_ctrs;
extern unsigned long long val_disable_ctrs;
extern unsigned long long val_select_evt_core;
extern unsigned long long val_reset_ctrs;


void monitor_cbo();
