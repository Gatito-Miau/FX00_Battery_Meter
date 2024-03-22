#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <cstdlib>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#include "sdl_functions.h"
#include "gen_functions.h"
#include "GP2X_Joy_Conf.h"

using namespace std;

typedef unsigned char uchar;
typedef unsigned short int usint;
typedef const int cint;
typedef Uint8 ussint;

/*
** F200's /dev/mmsp2adc defines and ioctl modes
*/
#define IOCTL_GP2X_MMSP2ADC_CONTROL    _IOR('v', 0x00 ,unsigned long)
#define BATT_MODE			0
#define REMOCON_MODE		1
#define BATT_REMOCON_MODE	2
typedef struct
{
	unsigned short batt;
	unsigned short remocon;

}MMSP2ADC;


	cint SCREEN_WIDTH 		= 320;
	cint SCREEN_HEIGHT 		= 240;
	cint SCREEN_BBP			= 16;
	cint SCREEN_MODE		= SDL_HWSURFACE|SDL_HWACCEL|SDL_SRCALPHA|SDL_DOUBLEBUF;
	cint SCREEN_LAYERS		= 4;
	
	// Layers' numbers
	#define MAIN_SURFACE 	0
	#define BACKGROUND		1
	#define BATT_METER		2
	#define LR_STUFF		3

	SDL_Surface** layers;
	
    bool done = false;
    usint tim = 1000;	// ms between battery readings
    usint timdiv = 50;	// tim divider for the loop
    usint timcnt = 0;	// loop counter
    ussint mode = 0;	// 0 = F100, 1 = F200, 2 = PC/DEBUG
	int tmp = 0;
	SDL_Event eventHandle;
	SDL_Joystick *joy;
	TTF_Font* font;
		int font_size = SCREEN_WIDTH/32;
		SDL_Color font_color = { 255, 255, 255 };
	SDL_Rect *LRParts = NULL;
	SDL_Rect *LRPos = NULL;
	
	/*
	** Battery levels
	*/
	SDL_Rect *BattLvs = NULL;	// SDL_Rect with the coordinates for each level
	SDL_Rect *BattPos = NULL;
	#define BATT_LEVEL_HIGH 0
	#define BATT_LEVEL_MID 1
	#define BATT_LEVEL_LOW 2
	#define BATT_LEVEL_EMPTY 3

ussint Init(int &argc, char** argv){
	
    if ( SDL_Init( SDL_INIT_VIDEO|SDL_INIT_JOYSTICK) < 0 ){
    	printf( "Unable to init SDL: %s\n", SDL_GetError() );
        return 1;
    }
	atexit(SDL_Quit);
	
	joy = SDL_JoystickOpen(0);
	if ( !joy ){
        printf("Unable to open joystick: %s\n", SDL_GetError());
    }
    
	layers = new SDL_Surface* [SCREEN_LAYERS];
	    
    layers[MAIN_SURFACE] = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BBP, SCREEN_MODE);
	if ( !layers[MAIN_SURFACE] ){
        printf("Unable to set %ix%ix%i video: %s\n", SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BBP, SDL_GetError());
        return 2;
    }
    
    layers[BACKGROUND] = load_image("body.png");
	if (!layers[BACKGROUND]){
        printf("Unable to load the background bitmap: %s\n", SDL_GetError());
        return 3;
    }
    
    layers[BATT_METER] = load_image("battery.png");
    if (!layers[BATT_METER]){
        printf("Unable to load the battery meter bitmap: %s\n", SDL_GetError());
        return 4;
    }   
        
    layers[LR_STUFF] = load_image("frame.png");
    if (!layers[LR_STUFF]){
        printf("Unable to load the LR buttom's frame bitmap: %s\n", SDL_GetError());
        return 5;
    }
    
    if (TTF_Init() < 0){
    	printf("Error initializing SDL_ttf: %s\n", TTF_GetError());
    	return 6;
    }
	font = TTF_OpenFont("libmono.ttf", font_size);
	if (font == NULL){
    	printf("Error loading font: %s\n", TTF_GetError());
    	return 7;
    }
    
    BattLvs = new SDL_Rect [4];
		if(BattLvs == NULL){
			printf("Unable to allocate the battery meter's coodinates. BattLvs still points to NULL\n");
			return 8;
		}
		for (int i=0;i<4;i++){
			//BattLvs[i] = {0, ((layers[BATT_METER]->h/4)*(i+1))-(layers[BATT_METER]->h/4), layers[BATT_METER]->w, layers[BATT_METER]->h/4};
			BattLvs[i].x = 0;
			BattLvs[i].y = ((layers[BATT_METER]->h/4)*(i+1))-(layers[BATT_METER]->h/4);
			BattLvs[i].w = layers[BATT_METER]->w;
			BattLvs[i].h = layers[BATT_METER]->h/4;
	        printf("BattLvs[%i] - x=%i y=%i w=%i h=%i\n", i, BattLvs[i].x , BattLvs[i].y, BattLvs[i].w, BattLvs[i].h);
		}
	BattPos = new SDL_Rect;
		if(BattPos == NULL){
			printf("Unable to allocate the battery meter's position coodinates. BattPos still points to NULL\n");
			return 9;
		}
		//BattPos[0] = {(layers[MAIN_SURFACE]->w/2)-(layers[BATT_METER]->w/2), layers[MAIN_SURFACE]->h/32,0,0};
		BattPos[0].x = (layers[MAIN_SURFACE]->w/2)-(layers[BATT_METER]->w/2);
		BattPos[0].y = (layers[MAIN_SURFACE]->h/2)-(((layers[BATT_METER]->h/4)+font_size+(font_size*1.2))/2);
		BattPos[0].w = 0;
		BattPos[0].h = 0;
		printf("BattPos[0] - x=%i y=%i w=%i h=%i\n", BattPos[0].x , BattPos[0].y, BattPos[0].w, BattPos[0].h);
	
	LRParts = new SDL_Rect [3];
		if(LRParts == NULL){
			printf("Unable to allocate the LR frame's coodinates. LRPos still points to NULL\n");
			return 10;
		}
		for (int i=0;i<2;i++){
			LRParts[i].x = ((layers[LR_STUFF]->w/2)*(i+1))-(layers[LR_STUFF]->w/2);
			LRParts[i].y = 0; 
			LRParts[i].w = (layers[LR_STUFF]->w/2);
			LRParts[i].h = 29; //layers[LR_STUFF]->h;
	        printf("LRParts[%i] - x=%i y=%i w=%i h=%i\n", i, LRParts[i].x , LRParts[i].y, LRParts[i].w, LRParts[i].h);
		}
		LRParts[2].x = 0;
		LRParts[2].y = 29; 
		LRParts[2].w = (layers[LR_STUFF]->w/2);
		LRParts[2].h = 29; 
	        printf("LRParts[3] - x=%i y=%i w=%i h=%i\n", LRParts[2].x , LRParts[2].y, LRParts[2].w, LRParts[2].h);
	LRPos = new SDL_Rect [3];
		if(LRPos == NULL){
			printf("Unable to allocate the LR frame's position coodinates. LRPos still points to NULL\n");
			return 11;
		}
		LRPos[0].x = 0;
		LRPos[0].y = 0;
		LRPos[0].w = 0;
		LRPos[0].h = 0;
		LRPos[1].x = (layers[MAIN_SURFACE]->w)-LRParts[1].w;
		LRPos[1].y = 0;
		LRPos[1].w = 0;
		LRPos[1].h = 0;
		LRPos[2].x = (layers[MAIN_SURFACE]->w)-LRParts[2].w;
		LRPos[2].y = (layers[MAIN_SURFACE]->h)-LRParts[2].h;
		LRPos[2].w = 0;
		LRPos[2].h = 0;
		for (int i=0;i<3;i++){
			printf("LRPos[%i] - x=%i y=%i w=%i h=%i\n", i, LRPos[i].x , LRPos[i].y, LRPos[i].w, LRPos[i].h);
		}
	
	// Modes:
	// 0 = F100
	// 1 = F200
	// 2 = PC/DEBUG
	tmp = open("/dev/mmsp2adc",O_RDONLY);
		if(tmp < 0){
			printf("\"/dev/mmsp2adc\" Not found...\n");
			tmp = open("/dev/batt",O_RDONLY);
			if(tmp < 0){
				printf("\"/dev/batt\" Not found...\n");
				printf("Using PC/DEBUG mode\n");
				mode = 2;
			}else{
				printf("\"/dev/batt\" found...\n");
				printf("Using F100 mode\n");
				mode = 0;
			}
		}else{
			printf("\"/dev/mmsp2adc\" found...\n");
			printf("Using F200 mode\n");
			mode = 1;
			close(tmp);
		}
	
	if (argc>1){
		if( strcmp(argv[1], "-debug") == 0) { printf("-debug detected\nForcing PC/DEBUG mode\n"); mode=2;}
	}
	
	SDL_ShowCursor(SDL_DISABLE);
	
	return 0;
}
void shutdown(){
    for (int i = 1;i<SCREEN_LAYERS;i++){
		cout << "Freeing surface " << i << (hex) << " at " << layers[i] << endl;
		SDL_FreeSurface(layers[i]);
    }
    TTF_CloseFont( font );
	TTF_Quit();
    SDL_Quit();
    printf("Exited cleanly\n");
}
void eventsHandler(SDL_Event *event){
	while (SDL_PollEvent(event)){
		switch (event->type){
			case SDL_QUIT:
				timcnt=timdiv;
				done = true;
				break;
			case SDL_KEYDOWN:
				switch (event->key.keysym.sym){
					case SDLK_ESCAPE:
						done = true;
						timcnt=timdiv;
						break;
					case SDLK_RIGHT:
						if(++mode>2){mode=0;}
						timcnt=timdiv;
						break;
					case SDLK_LEFT:
						if(mode==0){mode=2;}
						else{mode--;}
						timcnt=timdiv;
						break;
					default:
						break;
				}
			case SDL_JOYBUTTONDOWN:
				switch(event->jbutton.button){
					case GP2X_BUTTON_START:
						done = true;
						timcnt=timdiv;
						break;
					case GP2X_BUTTON_R:
						if(++mode>2){mode=0;}
						timcnt=timdiv;
						break;
					case GP2X_BUTTON_L:
						if(mode==0){mode=2;}
						else{mode--;}
						timcnt=timdiv;
						break;
				}
        }
	}
}
ussint calcbatt(usint val){
	/*
	** Values used in mmsp2_ADC.c
	*/
	if(val<=608){tmp=3;}
	if(val>608){tmp=2;}
	if(val>720){tmp=1;}
	if(val>832){tmp=0;}
return (ussint)tmp;	
}
void drawinfo (usint batt){
	char *aaa = new_char(256);
	SDL_Surface* textSurface = NULL;
	SDL_Rect textLocation = {0,0,0,0};
	switch (mode){
		case 0:
			if (batt > 0){
				sprintf (aaa, "Charge level: %i (%i)", 3-calcbatt(batt), batt);
				textSurface =  TTF_RenderText_Blended(font, aaa, font_color);
					textLocation.x = (layers[MAIN_SURFACE]->w/2)-(textSurface->w/2);
					textLocation.y = BattPos->y+layers[BATT_METER]->h/4+(font_size*1.2)*1;
				SDL_BlitSurface(textSurface, NULL, layers[MAIN_SURFACE], &textLocation);
				SDL_FreeSurface(textSurface);
			}else{
				sprintf (aaa, "Error reading \"/dev/batt\"");
				textSurface =  TTF_RenderText_Blended(font, aaa, font_color);
					textLocation.x = (layers[MAIN_SURFACE]->w/2)-(textSurface->w/2);
					textLocation.y = BattPos->y+layers[BATT_METER]->h/4+(font_size*1.2)*1;
				SDL_BlitSurface(textSurface, NULL, layers[MAIN_SURFACE], &textLocation);
				SDL_FreeSurface(textSurface);
			}
			
			sprintf (aaa, "GP2X F100");
			textSurface =  TTF_RenderText_Blended(font, aaa, font_color);
				textLocation.x = (layers[MAIN_SURFACE]->w/2)-(textSurface->w/2);
				textLocation.y = (font_size*1.2)-font_size;
			SDL_BlitSurface(textSurface, NULL, layers[MAIN_SURFACE], &textLocation);
			SDL_FreeSurface(textSurface);
			break;
		case 1:
			if (batt > 0){
				sprintf (aaa, "Charge level: %i (%i Aprox.)", 3-calcbatt(batt), batt);
				textSurface =  TTF_RenderText_Blended(font, aaa, font_color);
					textLocation.x = (layers[MAIN_SURFACE]->w/2)-(textSurface->w/2);
					textLocation.y = BattPos->y+layers[BATT_METER]->h/4+(font_size*1.2)*1;
				SDL_BlitSurface(textSurface, NULL, layers[MAIN_SURFACE], &textLocation);
				SDL_FreeSurface(textSurface);
			}else{
				sprintf (aaa, "Error reading \"/dev/mmsp2adc\"");
				textSurface =  TTF_RenderText_Blended(font, aaa, font_color);
					textLocation.x = (layers[MAIN_SURFACE]->w/2)-(textSurface->w/2);
					textLocation.y = BattPos->y+layers[BATT_METER]->h/4+(font_size*1.2)*1;
				SDL_BlitSurface(textSurface, NULL, layers[MAIN_SURFACE], &textLocation);
				SDL_FreeSurface(textSurface);
			}
			
			sprintf (aaa, "GP2X F200");
			textSurface =  TTF_RenderText_Blended(font, aaa, font_color);
				textLocation.x = (layers[MAIN_SURFACE]->w/2)-(textSurface->w/2);
				textLocation.y = (font_size*1.2)-font_size;
			SDL_BlitSurface(textSurface, NULL, layers[MAIN_SURFACE], &textLocation);
			SDL_FreeSurface(textSurface);
			break;
		case 2:
			sprintf (aaa, "Charge level: %i (%i)", 3-calcbatt(batt), batt);
			textSurface =  TTF_RenderText_Blended(font, aaa, font_color);
				textLocation.x = (layers[MAIN_SURFACE]->w/2)-(textSurface->w/2);
				textLocation.y = BattPos->y+layers[BATT_METER]->h/4+(font_size*1.2)*1;
			SDL_BlitSurface(textSurface, NULL, layers[MAIN_SURFACE], &textLocation);
			SDL_FreeSurface(textSurface);
			
			sprintf (aaa, "PC/DEBUG Mode");
			textSurface =  TTF_RenderText_Blended(font, aaa, font_color);
				textLocation.x = (layers[MAIN_SURFACE]->w/2)-(textSurface->w/2);
				textLocation.y = (font_size*1.2)-font_size;
			SDL_BlitSurface(textSurface, NULL, layers[MAIN_SURFACE], &textLocation);
			SDL_FreeSurface(textSurface);
			break;
	}
	
	delete aaa;
}
void blitAll(usint batt){
	SDL_BlitSurface(layers[BACKGROUND],0,layers[MAIN_SURFACE],0);
	SDL_BlitSurface(layers[BATT_METER], &BattLvs[calcbatt(batt)], layers[MAIN_SURFACE], BattPos);
	
	drawinfo(batt);
	
	SDL_BlitSurface(layers[LR_STUFF], &LRParts[0], layers[MAIN_SURFACE], &LRPos[0]);
	SDL_BlitSurface(layers[LR_STUFF], &LRParts[1], layers[MAIN_SURFACE], &LRPos[1]);
	SDL_BlitSurface(layers[LR_STUFF], &LRParts[2], layers[MAIN_SURFACE], &LRPos[2]);
}
usint ReadBatt(ussint workingmode){
	/*
	** Returns a value /dev/batt alike
	*/
	// Modes:
	// 0 = F100
	// 1 = F200
	// 2 = PC/DEBUG
	MMSP2ADC adcValue = {4,0};
	int average = 0;
	usint num = 0;
	switch(workingmode){
		case (0):	// F100
			tmp = open("/dev/batt", O_RDONLY);
				if(tmp < 0) {printf("Error opening \"/dev/batt\"\n");return 0;}
				else{
					for (int i=0;i<12;i++){
						read (tmp, &num, 2);
						average += num;
					}
				}
			close(tmp);
			printf("Value from /dev/batt: %u\n", usint(average/12));
			return usint(average/12);
			break;
		case (1):	// F200
			tmp = open("/dev/mmsp2adc",O_RDONLY);
				if(tmp < 0) {printf("Error opening \"/dev/mmsp2adc\"\n");return 0;}
				else{
					ioctl(tmp,IOCTL_GP2X_MMSP2ADC_CONTROL,BATT_MODE);
					while (adcValue.batt > 3){
						read(tmp, (char *)&adcValue, sizeof(MMSP2ADC));
					}
				}
			close(tmp);
			printf("Value from /dev/mmsp2adc: %u\n", adcValue.batt);
			switch(adcValue.batt){
				case 3: return 600; break;
				case 2: return 609; break;
				case 1: return 721; break;
				case 0: return 833; break;
			}
			break;
		case (2):	// PC/DEBUG
			tmp = 500 + rand()%400;
			printf("Random value: %i\n", tmp);
			return (usint)tmp;
			break;
	}
	return 0;
}
int main ( int argc, char** argv ){
	tmp = Init(argc, argv);
    if (tmp != 0){printf("Error during initialization: Error %u\n", tmp);return 1;}
    else{printf("Initialization completed succesfully!\n");}
	
	while (!done)
    {
    	blitAll(ReadBatt(mode));
		
        SDL_Flip(layers[MAIN_SURFACE]);
        
        for(timcnt=0; timcnt < timdiv; timcnt++){ 
			eventsHandler(&eventHandle);
			SDL_Delay(tim/timdiv);
    	}
    }
    
	shutdown();
    return 0;
}
