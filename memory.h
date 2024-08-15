struct memory;

struct memory *memory_init(void);

int memory_load_rom(struct memory *m, char *file);

void memory_free(struct memory *m);

unsigned char memory_get(struct memory *m, int index);

void memory_set(struct memory *m, int index, unsigned char value);

unsigned short memory_get_opcode(struct memory *m, int pc);
