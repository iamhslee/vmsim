# vmsim
2024-1 Operating Systems (ITP30002) - HW #4

A homework assignment to implement a virtual memory simulator using C language.

### Author
Hyunseo Lee (22100600) <hslee@handong.ac.kr>

## 1. How to build
This program is written in C language on Ubuntu 22.04.1 LTS. To build this program, you need to use Ubuntu 22.04.1 LTS with GCC and Make installed.

After unarchiving the folder, you should see the following files:
```bash
$ ls
Makefile     README.md    proc0.txt    proc1.txt    proc2.txt    vmsim.h      vmsim_main.c vmsim_main.h vmsim_op.c   vmsim_op.h
```

To build the program, run the following command in the terminal:
```bash
$ make
```

To cleanup the build files, run the following command in the terminal:
```bash
$ make clean
```

## 2. Usage
To run the program, run the following command in the terminal:
```bash
$ /vmsim <process_image.txt>
```

`process_image`: A text file that contains the process image.

The process image file should contain the following format:
```txt
<size of process image> <number of instructions>
<Instruction 1>
<Instruction 2>
...
```

Here is an example of the process image file:
```txt
16384 9
M 0 10
M 1 20
M 2 30
A 3 0 1
A 4 1 2
S 3 0x1000
S 4 0x2000
L 5 0x1000
L 6 0x2000
```

## 3. Instructions

There are 4 types of instructions:
- `M`: Move integer value into register
    - Ex) `M 0 10` means move 10 into register #0.
- `A`: Add the integer value of RegSrc1 and the integer value of RegSrc2. The result is stored in RegDst.
    - Ex) `A 3 0 1` means add the integer value of register #0 and the integer value of register #1. The result is stored in register #3.
- `L`: Load 4-bytes integer value from the specified memory address into register.
    - Ex) `L 5 0x1000` means load 4-bytes integer value from the memory address 0x1000 into register #5.
- `S`: Store 4-bytes integer value of register at the specifed memory address.
    - Ex) `S 3 0x1000` means store 4-bytes integer value of register #3 at the memory address 0x1000.
