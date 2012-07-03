#include <stdio.h>
#include <string.h>

struct cvm_instance {
	/* pointer to initial program */
	const unsigned int *code;
	/* vm can issue halt command */
	unsigned int running;
	/* program counter */
	unsigned int pc;
	/* main registers */
	unsigned short r[4];
	/* additional memory */
	unsigned short m[256];
};

struct cvm_instruction {
	/* these hold the decoded instruction */
	unsigned int r1, r2, r3, num, imm;
};

static void fetch_and_decode(const struct cvm_instance *vm, struct cvm_instruction *vi)
{
	unsigned int instruction = vm->code[vm->pc];

	vi->num = (instruction & 0xF000) >> 12;
	vi->r1 = (instruction & 0xF00) >> 8;
	vi->r2 = (instruction & 0xF0) >> 4;
	vi->r3 = instruction & 0xF;
	vi->imm = instruction & 0xFF;
}

static void evaluate(struct cvm_instance *vm, const struct cvm_instruction *vi)
{
	switch(vi->num) {
	case 0:
		printf("noop\n");
		break;
	case 1:
		printf("loadi r%d <- %d\n", vi->r1, vi->imm);
		vm->r[vi->r1] = vi->imm;
		break;
	case 2:
		printf("add r%d <- r%d + r%d\n", vi->r1, vi->r2, vi->r3);
		vm->r[vi->r1] = vm->r[vi->r2] + vm->r[vi->r3];
		break;
	case 3:
		printf("shiftl r%d <- r%d >> %d\n", vi->r1, vi->r2, vi->r3);
		vm->r[vi->r1] = vm->r[vi->r2] >> vi->r3;
		break;
	case 4:
		printf("load r%d <- @0x%02x\n", vi->r1, vi->imm);
		vm->r[vi->r1] = vm->m[vi->imm];
		break;
	case 5:
		printf("store @0x%02x <- r%d\n", vi->imm, vi->r1);
		vm->m[vi->imm] = vm->r[vi->r1];
		break;
	case 6:
		printf("syscall %d\n", vi->imm);
		switch (vi->imm) {
		case 0:
			printf("%s\n", (const char *) &vm->m[vm->r[0]]);
			break;
		}
		break;
	case 7:
		printf("shiftr r%d <- r%d << %d\n", vi->r1, vi->r2, vi->r3);
		vm->r[vi->r1] = vm->r[vi->r2] << vi->r3;
		break;
	case 8:
		printf("halt\n");
		vm->running = 0;
		break;
	}

	++vm->pc;
}

static void show(const struct cvm_instance *vm)
{
	printf("regs: %04u %04u %04u %04u\n",
		vm->r[0], vm->r[1], vm->r[2], vm->r[3]);
}

static void run(const unsigned int *program)
{
	struct cvm_instruction vi;
	struct cvm_instance vm;

	memset(&vm, 0, sizeof(vm));
	vm.code = program;
	vm.running = 1;

	while (vm.running) {
		fetch_and_decode(&vm, &vi);
		evaluate(&vm, &vi);
	}

	show(&vm);
}

int main(void)
{
	const unsigned int sample[] = {
		0x1064,
		0x11C8,
		0x2201,
		0x5200,
		0x4201,
		0x4200,
		0x3302,
		0x0000,
		0x0000,
		0x8000,
	};
	const unsigned int hello[] = {
		0x1065,
		0x7008,
		0x1148,
		0x2001,
		0x5000,
		0x106C,
		0x7008,
		0x116C,
		0x2001,
		0x5001,
		0x1020,
		0x7008,
		0x116F,
		0x2001,
		0x5002,
		0x106F,
		0x7008,
		0x1177,
		0x2001,
		0x5003,
		0x106C,
		0x7008,
		0x1172,
		0x2001,
		0x5004,
		0x1021,
		0x7008,
		0x1164,
		0x2001,
		0x5005,
		0x1000,
		0x5006,
		0x6000,
		0x8000,
	};

	run(sample);
	run(hello);

	return 0;
}
