# ASIS-C
This repository is the reference implementation for the `Advanced Simple Instruction Set Computer`.

## Efficiency
ASIS-C aims to be *the* fastest computer archetecture out there... Just kidding! While not quite as
as slow as molasses in January, it's not very efficient. Everything is byte-aligned, which makes
binaries bigger than they should be and somewhat slower to execute.

That said, what are you going to be building in this that requires sheer **speed**? Nothing.

## Building
Get a C toolchain, and run the makefile.

```
make
```

## Running
ASIS-C makes use of an "assembly language". However, its more of a notation than anything else.
Use the `spec.md` file to write a `.hwasm` (Human Writtable ASeMbly Code, kinda a lie), then use
the `examples/machinify.sh` script to convert that into the binary format.  Use the
`examples/gen_str.py` Python script to generate instructions for loading in strings.

Below is an example for adding two numbers from user input.

```
00000000: 0a26 5370 6563 6966 7920 7477 6f20 6e75  .&Specify two nu
00000010: 6d62 6572 7320 746f 2061 6464 2074 6f67  mbers to add tog
00000020: 6574 6865 7221 0000 0c24 0924 0d0d 010c  ether!...$.$....
00000030: 01                                       .
```

Woops! That's the binary format. This is the actual ASIS-C notation (the indentation doens't matter
but there must be spaces inbetween the byte-aligned elements):

```
A 26 53 70 65 63 69 66 79 20 74 77 6f 20 6e 75 6d 62 65 72 73 20 74 6f 20 61 64 64 20 74 6f 67 65 74 68 65 72 21 0 0
C 24
9 24

D D
1
C 1
```

The first three lines are for loading in the string, printing the string, and then popping the
string off the stack. The rest is for reading from standard input, adding the numbers, and then
printing the resulting number.

Run it with `asisc examples/add.asisc` and you'll see:

```
Allocating 49 bytes for instruction buffer...
Allocating 4096 bytes for stack buffer...
Loading binary...
Running machine code!
(OUT) > Specify two numbers to add together!
(IN ) > 5
(IN ) > 5
(OUT) > 10 (0xa)
Cleaning up...
```

Woohoo!

## Authors
Milo Banks

