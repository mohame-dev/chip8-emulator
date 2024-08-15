struct screen;

struct screen *screen_init();

void set_pixel(struct screen *s, int x, int y);

int get_pixel(struct screen *s, int x, int y);

void draw(struct screen *s);

void clear_screen(struct screen *s);

void screen_destroy(struct screen *s);
