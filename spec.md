# ASIS-C Specification
ASIS-C is a really, you guessed it, simple archetecture. It is a stack-based virtual machine, which
makes it easier to code for, and for VM implementation (I only have an hour and a half to implement
this).

## "Register"s

### Stack pointer
The only "register" is the stack pointer register, which is just an index into the stack
arary.

#### Modifying stack pointer
The stack pointer can be modified by pushing its value onto the stack with the `pus` instruction,
modifying it, and then poping up back off and back onto the register with the `pos` instruction.

## Instructions

### Alignment
Instructions and operands are aligned on 8-bit boundaries for ease of implementation. Take the
following:

```
0x0000000a  pus
0x0000000a  sub
0x0000000a  3
0x0000000a  nop
0x0000000a  pos
```

### Instruction List
Below is a list of all supported instructions. Whew, that's not many! Hard cheese (new favourite
saying).

| Mnemonic | Opcode | Description | Arguments |
| :------- | ------ | ----------- | --------: |
| NOP      | 0x0000 (0)  | Do nothing. | None. |
| ADD      | 0x0001 (1)  | Add data at stack pointer with data just below stack pointer. | None.
| SUB      | 0x0002 (2)  | Subtract data at stack pointer with data just below stack pointer. | None. |
| MUL      | 0x0003 (3)  | Multiply data at stack pointer with data just below stack pointer. | None. |
| DIV      | 0x0004 (4)  | Divide data at stack pointer with data just below stack pointer. | None. |
| PUS      | 0x0005 (5)  | Push stack pointer onto the stack. | None. |
| POS      | 0x0006 (6)  | Pop top of stack onto the stack pointer. | None. |
| EQU      | 0x0007 (7)  | Check if top two elements of the stack are equal. | None. |
| GRT      | 0x0008 (8)  | Check if the top stack element is greater than the second top. | None. |
| POP      | 0x0009 (9)  | Pop value(s) off stack. | Number of values to pop. |
| PUSH     | 0x000A (10) | Push value onto stack. | How many values you wish to push, the value(s) you wish to push. |
| JMPIF    | 0x000B (11) | Jump to address at top of stack if comparison is true. | None. |
| JMP      | 0x000C (12) | Jump unconditionally to address at top of stack. | None. |
| PNT      | 0x000D (13) | Print to host's stdout. | Negative stack offset to print from. |
| RED      | 0x000E (14) | Read value from host's stdin and push it onto the stack. | None. |
| DUP      | 0x000F (15) | Duplicate the top of the stack. | None. |

## Undefined behaviour
Anything else is undefined behaviour. Suck it.

