# INF205: Resource-Efficient Programming
This repository contains my solutions to the worksheets given at the course INF205 at NMBU.

## Worksheet 1
This worksheet has two problems. The first is `is-prime` which checks if a number is prime. The second is `circle-problem` which checks how many integers pairs $(x, y)$ which are within a circle of radius $r$.

```bash
# compile the code
g++ is-prime.cpp -o is-prime
g++ circle-problem.cpp -o circle-problem

# run the code
./is-prime <number>
./circle-problem <radius>
```

## Worksheet 2
This worksheet implements the truncated-shifted Lennard-Jones potential model to calculate the total energy in a system of molecules. The molecules, which are in an $a\times a\times a$ box, are read in, and the energy printed to the terminal.

```bash
# compile the code
make

# run the code
./readxyz <position.xyz> [velocity.xyz]
```

(`<...>` is a required arguemnt, `[...]` is an optional argument).

## Worksheet 3
This worksheet extends the previous worksheet. A program for generatring molecule position files is implemented (`genxyz`). In addition, `readxyz` now assumes periodic boundary conditions, a linked-cell data structure is implemented, and heuristics are included for data-structure selection.

```bash
# compile the code
make

# run genxyz to generate a position file
./genxyz <box-size> <density> <file-name.xyz>

# calculate energy of generated file
./readxyz
```