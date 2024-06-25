// 2024-1 Operating Systems (ITP30002) - HW #4
// File  : vmsim_main.h
// Author: Hyunseo Lee (22100600) <hslee@handong.ac.kr>

#ifndef VMSIM_MAIN_H
#define VMSIM_MAIN_H

#include "vmsim.h"

// Functions
void initialize();
void load(const char *filename, int pid);
void simulate();
int execute(Process *process);
void read_page(Process *process, int virt_addr, void *buf, size_t count);
void write_page(Process *process, int virt_addr, const void *buf, size_t count);
void print_log(int pid, const char *format, ...);
void print_register_set(int pid);

extern void op_move(Process *process, char *instruction);
extern void op_add(Process *process, char *instruction);
extern void op_load(Process *process, char *instruction);
extern void op_store(Process *process, char *instruction);

// Global variables
char *phy_memory;
int register_set[MAX_REGISTERS];
int clock = 0;
Process *process_list[MAX_PROCESSES];
int num_processes;

int free_frame_list[NUM_PAGES];  // Array to track free frames
// TODO: You can add more functions and variables

#endif