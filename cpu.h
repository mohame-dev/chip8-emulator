struct CPU;

struct CPU *CPU_init(char *file);

void CPU_free(struct CPU *cpu);

int cycle(struct CPU *cpu);

int render(struct CPU *cpu);
