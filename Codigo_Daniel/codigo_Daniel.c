#include <stdlib.h>
#include <stdio.h>
#include "SDL/SDL.h"
#define SCREEN_WIDTH 800 
#define SCREEN_HEIGHT 600
#define E_WIDTH 30
#define WIDTH 600 
#define HEIGHT 600
#define E_WIDTH 30
#define E_HEIGHT 30
#define P_WIDTH 30
#define P_HEIGHT 10
#define B_WIDTH 5
#define B_HEIGHT 15
#define P_BULLETS 1
#define E_BULLETS 3
#define BASE 4
#define BASE_WIDTH 60
#define BASE_HEIGHT 40

enum colors_t {red, green, purple};
enum directions_t {left, right, stationary};
enum states_t {menu, options, game, gameOver, pause};
enum colorKeys_t {magenta, lime};

struct score_t {
    int shots;
    int points;
    int level;
};

struct saucer_t {
    SDL_Rect hitbox;
    int alive;
    enum directions_t direction;
};

struct enemy_t {
    SDL_Rect hitbox;
    enum colors_t color;
    int alive;
    int points;
};

struct invaders_t {
    struct enemy_t enemies[5][10];
    enum directions_t direction;
    int killed;
    int speed;
    int state;
    int stateSpeed;
    Uint32 stateTime;
};

struct base_t {
    SDL_Rect hitbox;
};

struct player_t {
    SDL_Rect hitbox;
    int lives;
};

struct bullet_t {
    SDL_Rect hitbox;
    int alive;
};

static SDL_Surface* screen;
static SDL_Surface* titleScreen;
static SDL_Surface* cMap;
static SDL_Surface* invadersMap;
static SDL_Surface* playerImg;
static SDL_Surface* saucerImg;
static SDL_Surface* baseImg[4];
static SDL_Surface* damageImg;
static SDL_Surface* damageTopImg;
static SDL_Surface* gameOverImg;
struct score_t score;
struct invaders_t invaders;
struct saucer_t saucer;
struct base_t base[BASE];
struct player_t player;
struct bullet_t bullets[P_BULLETS];
struct bullet_t eBullets[E_BULLETS];
int pauseLen;
Uint32 pauseTime;
enum states_t state;
Uint32 titleTime;

int drawChar(char* c, int x, int y) {
    SDL_Rect* src;
    SDL_Rect* dest;
}

void drawString(char* s, int x, int y) {
    int i;
	for (i = 0; i < strlen(s); i++) {
		drawChar(s[i],x,y);
		x += 20;
    }
}

void paused(int len) {
    state = pause;
    pauseTime = SDL_GetTicks();
    pauseLen = len;
}

int loadImage(char* filename, SDL_Surface* surface, enum colorKeys_t colorKey) {
    SDL_Surface* temp = SDL_LoadBMP(filename);
    Uint32 colorKey;
    if (temp == NULL) {
        printf("Unable to load %s\n", filename);
        return 1;
    }
    if (colorKey == magenta) {
        colorKey = SDL_MapRGB(temp->format, 255, 0, 255);
    }
    else if (colorKey == lime) {
        colorKey = SDL_MapRGB(temp->format, 0, 255, 0);
    }
    SDL_SetColorKey(temp, SDL_TRUE, colorKey);
    surface = SDL_DisplayFormat(temp);
    if (surface == NULL) {
        printf("Unable to convert bitmap\n");
        return 1;
    }
    SDL_FreeSurface(temp);
    return 0;
}

int main(){
	printf("Hola Daniel");
	return 0;
}