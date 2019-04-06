#ifndef _CUBE
#define _CUBE

typedef struct {
	int x, z;
	char color;
} Cube;

void cube_init();
void cube_move();
int cube_collision(int x);
void cube_generate(int x, int mode);
void cube_draw(int x, int angle, int mode);
void cube_drawShip();

#endif
