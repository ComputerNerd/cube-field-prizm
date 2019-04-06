#include <stdlib.h>
#include <fxcg/display.h>
#include <fxcg/rtc.h>
#include "cube.h"
#include "fixed.h"
#include "graphic_functions.h"
#include "rand.h"
#include "time.h"

Cube** cubes=0;

static Cube* cube_new(int x, int z, char color)
{
	Cube* cube;
	if(cube = malloc(sizeof(Cube))) {
		cube->x = x;
		cube->z = FIX(z);
		cube->color = color;
	}
	return cube;
}

static int cube_count(Cube** list)
{
	int i=0;
	while(list[i]) i++;
	return i;
}

static Cube** cube_add(Cube** list, Cube* cube)
{
	int size;
	Cube** tmp;
	size = cube_count(list);
	if(tmp = realloc(list, (size+2)*sizeof(Cube*))) {
		list = tmp;
		list[size] = cube;
		list[size+1] = 0;
	}
	return list;
}

static Cube** cube_del(Cube** list, Cube* cube)
{
	int i, size;
	size = cube_count(list);
	for(i=0 ; list[i] && list[i]!=cube ; i++); //find cube
	if(list[i]) {
		free(cube);
		for( ; i<size ; i++) list[i] = list[i+1]; //delete cube
		list = realloc(list, size*sizeof(Cube*)); //resize list
	}
	return list;
}

static void cube_drawBackground(int angle, int mode)
{
	int x=LCD_WIDTH_PX, y=0;
	if(mode == 1) FillVRAM(0x0000);
	else Bdisp_AllClr_VRAM();
	y = UNFIX( fmul(FIX(x),fsin(FIX(angle))) );
	x = UNFIX( fmul(FIX(x),fcos(FIX(angle))) );
	Line((LCD_WIDTH_PX>>1)-x, (LCD_HEIGHT_PX>>1)-y+10, (LCD_WIDTH_PX>>1)+x, (LCD_HEIGHT_PX>>1)+y+10, 0x0000);
}

void cube_init()
{
	if(!cubes) {
		cubes = malloc(sizeof(Cube*));
		cubes[0] = 0;
	}
}

int ticksGlobal;
void cube_move(int speed)
{
	int i=0;
	int tmp=RTC_GetTicks();
	while(cubes[i]) {
		int tmp_diff = tmp - ticksGlobal;
		if (tmp_diff <= 0)
			tmp_diff = 1;
		cubes[i]->z-=speed * tmp_diff;
		if(UNFIX(cubes[i]->z)<=0) cubes = cube_del(cubes, cubes[i]);
		else i++;
	}
	ticksGlobal = tmp;
}

#define abs(x) ((x)<0?-(x):x)
int cube_collision(int x)
{
	int i=0;
	while(cubes[i]) {
		if(UNFIX(cubes[i]->z) == 15 && abs(cubes[i]->x-x)<3)
			return 1;
		i++;
	}
	return 0;
}

void cube_generate(int x, int mode)
{
	static int m=0, c=0, cx, cz;
	if(mode != m) cx=x, cz=0, m=mode;
	else cz++;
	if(++c>2) {
		switch(mode) {
			case 0: cubes = cube_add(cubes, cube_new(rand()%200-100+x, 75, rand()%3)); break;
			case 1:
				cubes = cube_add(cubes, cube_new(cx-(cz<30?50-cz:20), 75, rand()%3));
				cubes = cube_add(cubes, cube_new(cx+(cz<30?50-cz:20), 75, rand()%3));
				break;
			case 2:
				if(cz>50) cx += (cz&32?3:-3);
				cubes = cube_add(cubes, cube_new(cx-(cz<30?45-cz:15), 75, rand()%3));
				cubes = cube_add(cubes, cube_new(cx+(cz<30?45-cz:15), 75, rand()%3));
				break;
			case 3:
				if(cz<50) {
					cubes = cube_add(cubes, cube_new(cx-(cz<30?45-cz:15), 75, rand()%3));
					cubes = cube_add(cubes, cube_new(cx+(cz<30?45-cz:15), 75, rand()%3));
				} else {
					int i = (abs((cz-50+40)%80-40))+15;
					cubes = cube_add(cubes, cube_new(cx-i, 75, rand()%3));
					cubes = cube_add(cubes, cube_new(cx+i, 75, rand()%3));
					if(i>=30) {
						cubes = cube_add(cubes, cube_new(cx-(i-30), 75, rand()%3));
						cubes = cube_add(cubes, cube_new(cx+(i-30), 75, rand()%3));
					}
				}
		}
		c=0;
	}
}

void cube_draw(int _x, int angle, int mode)
{
	const short color[] = {0xF9A0, 0xFCE0, 0xFE60};
	int i = cube_count(cubes)-1;
	cube_drawBackground(angle, mode);
	angle = FIX(angle);
	while(i>=0) {
		int j, x[]={-1,1,1,-1}, y[]={-2,-2,0,0}, tmp;
		for(j=0 ; j<4 ; j++) {
			x[j] = FIX( 10*x[j]+5*(cubes[i]->x-_x) ); //translation
			y[j] = FIX( 10*y[j] );
			tmp  = fmul(x[j],fcos(angle)) - fmul(y[j],fsin(angle)); // rotation
			y[j] = fmul(x[j],fsin(angle)) + fmul(y[j],fcos(angle));
			x[j] = tmp;
			x[j] = fdiv( (2*x[j]), (cubes[i]->z>>4)); //perspective
			y[j] = fdiv( (2*y[j]+FIX(LCD_HEIGHT_PX/2)), (cubes[i]->z>>4));
			x[j] = UNFIX(x[j]) + LCD_WIDTH_PX/2; //centring
			y[j] = UNFIX(y[j]) + LCD_HEIGHT_PX/2;
		}
		switch(mode) {
			case 0: FilledConvexPolygon(x, y, 4, color[cubes[i]->color]); Polygon(x, y, 4, 0); break;
			case 1: Polygon(x, y, 4, 0x07E0); break;
			case 2: FilledConvexPolygon(x, y, 4, 0); break;
		}
		i--;
	}
}

void cube_drawShip()
{
	const unsigned short bmp[] = {
		65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,0,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,
		65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,0,0,0,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,
		65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,0,40147,0,40147,0,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,
		65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,0,40147,40147,0,40147,40147,0,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,
		65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,0,40147,40147,40147,0,40147,40147,40147,0,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,
		65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,0,40147,40147,40147,40147,0,40147,40147,40147,40147,0,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,
		65535,65535,65535,65535,65535,65535,65535,65535,65535,0,40147,40147,40147,40147,40147,0,40147,40147,40147,40147,40147,0,65535,65535,65535,65535,65535,65535,65535,65535,65535,
		65535,65535,65535,65535,65535,65535,65535,65535,0,40147,40147,40147,40147,0,0,12678,0,0,40147,40147,40147,40147,0,65535,65535,65535,65535,65535,65535,65535,65535,
		65535,65535,65535,65535,65535,65535,65535,0,40147,40147,40147,0,0,12678,12678,12678,12678,12678,0,0,40147,40147,40147,0,65535,65535,65535,65535,65535,65535,65535,
		65535,65535,65535,65535,65535,65535,0,40147,40147,0,0,12678,12678,12678,12678,12678,12678,12678,12678,12678,0,0,40147,40147,0,65535,65535,65535,65535,65535,65535,
		65535,65535,65535,65535,65535,0,40147,0,0,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,0,0,40147,0,65535,65535,65535,65535,65535,
		65535,65535,65535,65535,0,0,0,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,0,0,0,65535,65535,65535,65535,
		65535,65535,65535,0,0,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,0,0,65535,65535,65535,
		65535,65535,0,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,0,65535,65535,
		65535,0,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,12678,0,65535,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	};
	CopySpriteMasked(bmp, (LCD_WIDTH_PX-31)/2, LCD_HEIGHT_PX-16-10, 31, 16, 0xFFFF);
}
