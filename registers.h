struct registers;

struct registers *registers_init(void);

void registers_set(struct registers *r, unsigned char index, unsigned char value);

unsigned char registers_get(struct registers *r, unsigned short index);

void registers_free(struct registers *r);
