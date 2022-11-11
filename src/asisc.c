#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <sys/stat.h>

#ifndef STARTING_STACK_SIZE
#define STARTING_STACK_SIZE 4096
#endif /* STARTING_STACK_SIZE */

#ifndef STACK_DATATYPE
#define STACK_DATATYPE uint32_t
#endif /* STACK_DATATYPE */

#define NOP     0x0000
#define ADD     0x0001
#define SUB     0x0002
#define MUL     0x0003
#define DIV     0x0004
#define PUS     0x0005
#define POS     0x0006
#define EQU     0x0007
#define GRT     0x0008
#define POP     0x0009
#define PUSH    0x000A
#define JMPIF   0x000B
#define JMP     0x000C
#define PNT     0x000D
#define RED     0x000E
#define DUP     0x000F

struct vm {
    size_t stack_ptr;
    size_t stack_len;
    size_t stack_max;
    STACK_DATATYPE* stack;

    size_t instruction_ptr;
    size_t num_instructions;
    size_t max_instructions;
    uint8_t* instruction_buffer;

    _Bool compare_status;
} vm;

void print_usage(void) {
    printf("asisc <raw machine code>\n");
}

void print_stack(struct vm* vm) {
    for (size_t i = 0; i < vm->stack_len; i++) {
        printf("0x%x, ", vm->stack[i]);
    }

    printf("\n");
}

void print_instructions(struct vm* vm) {
    for (size_t i = 0; i < vm->num_instructions; i++) {
        printf("0x%x, ", vm->instruction_buffer[i]);
    }

    printf("\n");
}

int push_stack(struct vm* vm, STACK_DATATYPE value) {
    if (vm->stack_len > vm->stack_max) {
        printf("%d > %d\n", vm->stack_len, vm->stack_max);
        print_stack(vm);
        vm->stack = realloc(vm->stack, vm->stack_max*2);
        vm->stack_max *= 2;

        if (vm->stack == NULL) {
            perror("realloc");
            return EXIT_FAILURE;
        }
    }

    vm->stack[vm->stack_ptr] = value;
    vm->stack_len += 1;
    vm->stack_ptr += 1;

    return EXIT_SUCCESS;
}

void pop_stack(struct vm* vm) {
    // Not nessisary, easier to debug.
    vm->stack[vm->stack_ptr] = 0;
    vm->stack_len -= 1;
    vm->stack_ptr -= 1;
}

_Bool suitable_for_arith(struct vm* vm) {
    if (vm->stack_len < 1) {
        fprintf(stderr, "Machine state incompatable with arithmetic (stack length=%zu).\n",
                vm->stack_len);

        return 0;
    }

    return 1;
}

int run_machine_code(struct vm* vm) {
    while (1) {
        _Bool increment_instruction_ptr = 1;

        if (vm->instruction_ptr == vm->num_instructions)
            break;

        if (vm->instruction_ptr > vm->num_instructions) {
            fprintf(stderr, "Instruction pointer (0x%x) points to invalid virtual memory location "
                    "(max=0x%x).\n",
                    (unsigned int) vm->instruction_ptr, (unsigned int) vm->num_instructions-1);

            return EXIT_FAILURE;
        }

        switch (vm->instruction_buffer[vm->instruction_ptr]) {
        case NOP:
            break;
        case ADD:
            if (!suitable_for_arith(vm))
                return EXIT_FAILURE;

            int addendA = vm->stack[vm->stack_ptr-1];
            int addendB = vm->stack[vm->stack_ptr-2];
            if (push_stack(vm, addendA + addendB))
                return EXIT_FAILURE;

            break;
        case SUB:
            if (!suitable_for_arith(vm))
                return EXIT_FAILURE;

            int minuendA = vm->stack[vm->stack_ptr-1];
            int minuendB = vm->stack[vm->stack_ptr-2];
            if (push_stack(vm, minuendA - minuendB))
                    return EXIT_FAILURE;

            break;
        case MUL:
            if (!suitable_for_arith(vm))
                return EXIT_FAILURE;

            int factorA = vm->stack[vm->stack_ptr-1];
            int factorB = vm->stack[vm->stack_ptr-2];
            if (push_stack(vm, factorA * factorB))
                    return EXIT_FAILURE;

            break;
        case DIV:
            if (!suitable_for_arith(vm))
                return EXIT_FAILURE;

            int dividendA = vm->stack[vm->stack_ptr-1];
            int dividendB = vm->stack[vm->stack_ptr-2];
            if (push_stack(vm, dividendA / dividendB))
                    return EXIT_FAILURE;

            break;
        case PUS:
            if (push_stack(vm, vm->stack_ptr))
                return EXIT_FAILURE;

            break;
        case POS:
            vm->stack_ptr = vm->instruction_buffer[vm->instruction_ptr+1];
            pop_stack(vm);

            break;
        case EQU:
            if (!suitable_for_arith(vm))
                return EXIT_FAILURE;

            int equA = vm->stack[vm->stack_ptr-1];
            int equB = vm->stack[vm->stack_ptr-2];
            vm->compare_status = (equA == equB);

            break;
        case GRT:
            if (!suitable_for_arith(vm))
                return EXIT_FAILURE;

            int grtA = vm->stack[vm->stack_ptr-1];
            int grtB = vm->stack[vm->stack_ptr-2];
            vm->compare_status = (grtA > grtB);

            break;
        case POP:
            for (int i = 0; i < vm->instruction_buffer[vm->instruction_ptr+1]; i++) {
                pop_stack(vm);
            }

            vm->instruction_ptr += 1;

            break;
        case PUSH: {
            int num_values = vm->instruction_buffer[vm->instruction_ptr+1];

            if (num_values == 1) {
                if (push_stack(vm, vm->instruction_buffer[vm->instruction_ptr+2]))
                    return EXIT_FAILURE;
            } else {
                for (int i = 2; i < num_values; i++) {
                    if (push_stack(vm, vm->instruction_buffer[vm->instruction_ptr+i]))
                        return EXIT_FAILURE;
                }
            }

            vm->instruction_ptr += 1 + num_values;

            break;
        }
        case JMPIF:
            if (vm->compare_status)
                vm->instruction_ptr = vm->stack[vm->stack_ptr-1];

            break;
        case JMP:
            vm->instruction_ptr = vm->stack[vm->stack_ptr-1];

            break;
        case PNT: {
            size_t stack_offset = vm->instruction_buffer[vm->instruction_ptr+1];
            unsigned int start_at = vm->stack_len - stack_offset;

            if (vm->stack_len < stack_offset) {
                fprintf(stderr, "Stack offset (%zu) cannot exceed stack length (%zu).\n",
                        stack_offset, vm->stack_len);
                return EXIT_FAILURE;
            }

            // Printing a single character?
            if (stack_offset == 1) {
                // Trying to print a non-control character?
                if (vm->stack[stack_offset] > 31 && vm->stack[stack_offset] != 127) {
                    printf("\033[1;30m(OUT)\033[0m > %c\n", vm->stack[start_at]);
                } else {
                    printf("\033[1;30m(OUT)\033[0m > %d (0x%x)\n", vm->stack[start_at], vm->stack[start_at]);
                }
            } else {
                printf("\033[1;30m(OUT)\033[0m > ");
                for (size_t i = start_at; i < vm->stack_len; i++) {
                    printf("%c", vm->stack[i]);
                }

                printf("\n");
            }

            vm->instruction_ptr += 1;

            break;
        }
        case RED: {
            char buf[5];
            int in;

            printf("\033[1;30m(IN )\033[0m > ");
            fflush(stdout);
            read(STDIN_FILENO, buf, 3);
            buf[4] = '\0';
            in = atoi(buf);

            if (push_stack(vm, in))
                return EXIT_FAILURE;

            break;
        }
        case DUP:
            if (push_stack(vm, vm->stack[vm->stack_ptr-1]))
                return EXIT_FAILURE;

            break;
        default:
            fprintf(stderr, "Unknown opcode 0x%x\n", vm->instruction_buffer[vm->instruction_ptr]);

            return EXIT_FAILURE;
        }

        if (increment_instruction_ptr)
            vm->instruction_ptr++;
    }

    return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Incorrect number of arguments.\n");
        print_usage();

        return EXIT_FAILURE;
    }

    char* filename = argv[1];
    struct stat sb;

    if (stat(filename, &sb) == -1) {
        perror("stat");

        return EXIT_FAILURE;
    }

    struct vm vm;
    vm.compare_status = 0;

    printf("Allocating %ld bytes for instruction buffer...\n", sb.st_size);
    vm.instruction_buffer = malloc(sb.st_size);
    vm.num_instructions = sb.st_size;
    vm.instruction_ptr = 0;
    if (vm.instruction_buffer == NULL) {
        perror("malloc");

        return EXIT_FAILURE;
    }

    printf("Allocating %d bytes for stack buffer...\n", STARTING_STACK_SIZE);
    vm.stack = malloc(STARTING_STACK_SIZE);
    vm.stack_max = STARTING_STACK_SIZE;
    vm.stack_ptr = 0;
    vm.stack_len = 0;
    if (vm.stack == NULL) {
        perror("malloc");

        return EXIT_FAILURE;
    }

    printf("Loading binary...\n");
    FILE* fp;
    fp = fopen(filename, "rb");
    if (fp == NULL) {
        perror("fopen");

        return EXIT_FAILURE;
    }

    fread(vm.instruction_buffer, sizeof(unsigned char), vm.num_instructions, fp);

    fclose(fp);

    printf("Running machine code!\n");
    int rc = run_machine_code(&vm);

    printf("Cleaning up...\n");
    free(vm.instruction_buffer);
    free(vm.stack);

    return rc;
}

