#include <fxcg/rtc.h>

#include "text.h"
#include "cube.h"
#include "graphic_functions.h"
#include "key.h"
#include "rand.h"

void PrintInt(unsigned int n)
{
	char str[20]="0";
	int i, l=0;
	for(i=n ; i ; i/=10)
		l++;
	for(i=n ; i ; i/=10)
		str[--l] = i%10+'0';
	Print_OS(str, 0, 0);
}

void wait(int ticks)
{
	static int time=0;
	while(time_getTicks()-time < ticks);
	time = time_getTicks();
}

int main(void) {
	int angle=0, x=0, run=0, score, best_score=0, mode, color, speed, speedup;
	char str[20];
	srand(RTC_GetTicks());
	cube_init();
	while(key_down(K_EXE));
	ticksGlobal = RTC_GetTicks();
	while(!key_down(K_EXIT)) {
		if(run) {
			switch(++score) {
				case 100: mode=0; break;
				case 1000: mode=3; break;
				case 1200: mode=0; break;
				case 2100: mode=2; break;
				case 2400: mode=0; break;
				case 3400: mode=1; break;
				case 3500: speedup=1; break;
				case 3580: speedup=0; speed++; color=1; mode=0; break;
				case 4000: mode=3; break;
				case 4280: mode=0; break;
				case 4800: mode=2; break;
				case 5000: mode=0; break;
				case 6000: speedup=1; break;
				case 6080: speedup=0; speed++; color=2; break;
				case 7000: mode=2; break;
				case 7400: mode=0; break;
			}
			if(cube_collision(x)) { run=0; if(score>best_score) best_score=score; }
			if(key_down(K_LEFT)) { if(angle<20) angle++; x--; } else if(angle > 0) angle--;
			if(key_down(K_RIGHT)) { if(angle>-20) angle--; x++; } else if(angle < 0) angle++;
			cube_move((speed+1)<<13);
		} else {
			if(angle > 0) angle--;
			if(angle < 0) angle++;
			mode = color = score = speed = speedup = 0;
			cube_move(1<<12);
			if(key_down(K_EXE)) run = mode = 1;
		}
		cube_generate(x, mode);
		cube_draw(x, angle, color);
		if(run) {
			if(speedup) {
				RoundedSquareAlpha((LCD_WIDTH_PX-150)/2, (LCD_HEIGHT_PX-20)/2, 150, 20, 6, 0x8410, 20);
				text_print((LCD_WIDTH_PX-2*text_width("SPEED UP"))/2, (LCD_HEIGHT_PX-10)/2, "SPEED UP", 2, 0x100000);
			}
			cube_drawShip();
			text_print(2, 2, itoa(score, str), 1, 0x0000);
		} else {
			char hs[30] = "High Score : ";
			itoa(best_score, hs+13);
			RoundedSquareAlpha((LCD_WIDTH_PX-200)/2, (LCD_HEIGHT_PX-70)/2, 200, 80, 10, 0x0000, 16);
			text_print((LCD_WIDTH_PX-3*text_width("CUBEFIELD"))/2, (LCD_HEIGHT_PX-80)/2+20, "CUBEFIELD", 3, 0xFFFF);
			text_print((LCD_WIDTH_PX-text_width("Use LEFT and RIGHT to turn"))/2, (LCD_HEIGHT_PX-80)/2+40, "Use LEFT and RIGHT to turn", 1, 0xFFFF);
			if(best_score) text_print((LCD_WIDTH_PX-text_width(hs))/2, (LCD_HEIGHT_PX-80)/2+50, hs, 1, 0xFFFF);
			text_print((LCD_WIDTH_PX-text_width("Press EXE"))/2, (LCD_HEIGHT_PX-80)/2+65, "Press EXE", 1, 0xFFFF);
		}
		Bdisp_PutDisp_DD();
	}
	return 1;
}
