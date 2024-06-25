// 2024-1 Operating Systems (ITP30002) - HW #4
// File  : vmsim_main.c
// Author: Hyunseo Lee (22100600) <hslee@handong.ac.kr>

#include "vmsim_main.h"

int main(int argc, char *argv[]) {
    if (argc < 2 || argc > MAX_PROCESSES + 1) {
        fprintf(stderr, "Usage: %s <process image files... up to %d files>\n",
                argv[0], MAX_PROCESSES);
        exit(EXIT_FAILURE);
    }

    // Initialize
    initialize();

    // Load process
    for (int i = 1; i < argc; i++) {
        load(argv[i], i - 1);
    }

    // Execute process
    simulate();

    // TODO: free memory - process list, frame, ...

    return 0;
}

// Initialization
void initialize() {
    int i;

    // Physical memory
    phy_memory = (char *)malloc(PHY_MEM_SIZE);

    // Register set
    for (i = 0; i < MAX_REGISTERS; i++) {
        register_set[i] = 0;
    }

    // Initialize process list
    for (i = 0; i < MAX_PROCESSES; i++) {
        process_list[i] = NULL;
    }

    // Initialize clock
    clock = 0;
    num_processes = 0;

    // Initialize free frame list
    for (i = 0; i < NUM_PAGES; i++) {
        free_frame_list[i] = 1;  // 1 indicates the frame is free
    }
}

// Load process from file
void load(const char *filename, int pid) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Failed to open file");
        exit(EXIT_FAILURE);
    }

    Process *process = (Process *)malloc(sizeof(Process));
    process->pid = pid;
    fscanf(file, "%d %d", &process->size, &process->num_inst);
    process->pc = 0;

    process->page_table = (PageTableEntry *)malloc(NUM_PAGES * sizeof(PageTableEntry));
    for (int i = 0; i < NUM_PAGES; i++) {
        process->page_table[i].frame_number = -1;
        process->page_table[i].valid = 0;
    }

    for (int i = 0; i < MAX_REGISTERS; i++) {
        process->temp_reg_set[i] = 0;
    }

    char instruction[INSTRUCTION_SIZE];
    for (int i = 0; i < process->num_inst; i++) {
        fscanf(file, " %[^\n]s", instruction);
        write_page(process, process->pc, instruction, INSTRUCTION_SIZE);
        process->pc += INSTRUCTION_SIZE;
    }
    process->pc = 0;

    process_list[num_processes++] = process;

    fclose(file);
}

// Simulation
void simulate() {
    while (num_processes > 0) {
        for (int i = 0; i < num_processes; i++) {
            Process *process = process_list[i];
            if (execute(process)) {
                print_register_set(process->pid);
                free(process->page_table);
                free(process);
                process_list[i] = process_list[--num_processes];
            }
            clock++;
        }
    }
}

// Execute an instruction using program counter
int execute(Process *process) {
    char instruction[INSTRUCTION_SIZE];
    char opcode;

    memcpy(register_set, process->temp_reg_set, sizeof(register_set));
    read_page(process, process->pc, instruction, INSTRUCTION_SIZE);
    process->pc += INSTRUCTION_SIZE;

    opcode = instruction[0];
    switch (opcode) {
        case 'M':
            op_move(process, instruction);
            break;
        case 'A':
            op_add(process, instruction);
            break;
        case 'L':
            op_load(process, instruction);
            break;
        case 'S':
            op_store(process, instruction);
            break;
        default:
            printf("Unknown Opcode (%c) \n", opcode);
    }

    memcpy(process->temp_reg_set, register_set, sizeof(register_set));

    return process->pc >= process->num_inst * INSTRUCTION_SIZE;
}

// Find the first free frame
int find_first_free_frame() {
    for (int i = 0; i < NUM_PAGES; i++) {
        if (free_frame_list[i] == 1) {
            return i;
        }
    }
    return -1;  // No free frames available
}

// Read up to 'count' bytes from the 'virt_addr' into 'buf'
void read_page(Process *process, int virt_addr, void *buf, size_t count) {
    int page_num = virt_addr / PAGE_SIZE;
    int offset = virt_addr % PAGE_SIZE;
    int frame_num;

    if (!process->page_table[page_num].valid) {  // Page fault
        frame_num = find_first_free_frame();
        if (frame_num == -1) {
            fprintf(stderr, "No free frames available\n");
            exit(EXIT_FAILURE);
        }
        free_frame_list[frame_num] = 0;  // Mark the frame as used
        process->page_table[page_num].frame_number = frame_num;
        process->page_table[page_num].valid = 1;

        // Log the page fault
        print_log(process->pid, "Page fault at virtual address 0x%04x (page_number=%d) --> Allocated frame_number=%d", virt_addr, page_num, frame_num);
    } else {
        frame_num = process->page_table[page_num].frame_number;
    }

    memcpy(buf, &phy_memory[frame_num * PAGE_SIZE + offset], count);
}

// Write 'buf' up to 'count' bytes at the 'virt_addr'
void write_page(Process *process, int virt_addr, const void *buf, size_t count) {
    int page_num = virt_addr / PAGE_SIZE;
    int offset = virt_addr % PAGE_SIZE;
    int frame_num;

    if (!process->page_table[page_num].valid) {  // Page fault
        frame_num = find_first_free_frame();
        if (frame_num == -1) {
            fprintf(stderr, "No free frames available\n");
            exit(EXIT_FAILURE);
        }
        free_frame_list[frame_num] = 0;  // Mark the frame as used
        process->page_table[page_num].frame_number = frame_num;
        process->page_table[page_num].valid = 1;

        // Log the page fault
        print_log(process->pid, "Page fault at virtual address 0x%04x (page_number=%d) --> Allocated frame_number=%d", virt_addr, page_num, frame_num);
    } else {
        frame_num = process->page_table[page_num].frame_number;
    }

    memcpy(&phy_memory[frame_num * PAGE_SIZE + offset], buf, count);
}

// Print log with format string
void print_log(int pid, const char *format, ...) {
    va_list args;
    va_start(args, format);
    printf("[Clock=%2d][PID=%d] ", clock, pid);
    vprintf(format, args);
    printf("\n");
    fflush(stdout);
    va_end(args);
}

// Print values in the register set
void print_register_set(int pid) {
    int i;
    char str[256], buf[16];
    strcpy(str, "[RegisterSet]:");
    for (i = 0; i < MAX_REGISTERS; i++) {
        sprintf(buf, " R[%d]=%d", i, register_set[i]);
        strcat(str, buf);
        if (i != MAX_REGISTERS - 1)
            strcat(str, ",");
    }
    print_log(pid, "%s", str);
}