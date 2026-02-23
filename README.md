# JIT AOT COMPILERS IN VMs COURSE

## IR GENERAL PRINCIPALS

### BASIC BLOCKS PRINCIPALS

- Basic block is a sequence of instructions
which can only be entered at the beginning
and exited at the end
- Each basic block has only zero, one or two successors
- Each basic block has any number of predecessors
- Method always starts with "start" basic block
- Method always ends with "return" basic block
- Method can have any number of basic blocks

### IR INSTRUCTION PRINCIPALS

- Each instruction has unique id in basic block
- Each instruction has zero or more input instructions
- Each instruction has zero or more user instructions

## IR INSTRUCTIONS TYPES AND DUMP SYNTAX

### BASICBLOCK SYNTAX

Basic block with given id, can have zero, one or two successors, and any number of predecessors
``` bb 0 ```
``` bb 0 -> bb1 ```
``` bb 0 -> bb1, bb2 ```
``` bb 0, bb1 -> bb 2 -> bb2, bb3 ```
``` bb 0, bb 2 -> bb2, bb3 ```
``` bb 0, bb 1, bb 3, bb 2 -> bb 2 -> bb2, bb3 ```

### PARAM INSTRUCTION

Read a parameter with given id and type from method
``` 1.u32 param 0 ```

### CONSTANT INSTRUCTION

Return a constant value of given type
``` 2.u64 const 42 ```

### CAST INSTRUCTION

Cast input from other instruction from one given type to another
``` 3.u32 cast v2 ```

### BINARY OPERATIONS INSTRUCTIONS

Calculate result of given operation between two input instructions
``` 4.u32 add v1 v3 ```
``` 4.u32 sub v1 v3 ```
``` 4.u32 mul v1 v3 ```
``` 4.u32 div v1 v3 ```
``` 4.u32 and v1 v3 ```
``` 4.u32 or v1 v3 ```

### RETURN INSTRUCTION

Return value from method
``` 5.u32 return v1 ```

### CALL INSTRUCTION

Call method with given id and pass given parameters
and return value from method
``` 6.u32 call v2 v3 v4 ```
v2 - method id
v3, v4 - parameters from input instructions

### IF INSTRUCTION

Compare two input values and if condition is true
go to first successor basic block
else go to second successor basic block
``` 8.void if.gt v1 v3 ```
if flags:
gt, lt, ge, le, eq, ne

### PHI INSTRUCTION

Merge any number of input values from predecessors basic blocks and return one value
``` 9.u32 phi v1 v3 ```
input values from predecessors basic blocks
goes in order same as order
of predecessors basic blocks
