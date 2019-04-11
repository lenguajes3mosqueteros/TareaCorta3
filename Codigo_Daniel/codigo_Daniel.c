#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "SDL/SDL.h"
#define ANCHO_PANTALLA 800 
#define ALTO_PANTALLA 600
#define ANCHO 600 
#define ALTO 600
#define ANCHO_E 30
#define ALTO_E 30
#define ANCHO_P 30
#define ALTO_P 10
#define ANCHO_B 5
#define ALTO_B 15
#define BALAS_P 1
#define BALAS_E 3
#define BASE 4
#define ANCHO_BASE 60
#define ALTO_BASE 40

enum colores_t {rojo, verde, morado};
enum direcciones_t {left, right, stationary};
enum estados_t {menu, options, game, gameOver, pause};
enum clavesColor_t {magenta, lime};

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

static SDL_Surface* pantalla;
static SDL_Surface* pantallaInicio;
static SDL_Surface* mapaC;
static SDL_Surface* mapaInvasores;
static SDL_Surface* imagenJugador;
static SDL_Surface* imagenPlatillo;
static SDL_Surface* imagenBase[4];
static SDL_Surface* imagenDaño;
static SDL_Surface* imagenDañoSuperior;
static SDL_Surface* imagenJuegoTerminado;
struct score_t puntuacion;
struct invaders_t invasores;
struct saucer_t platillo;
struct base_t base[BASE];
struct player_t jugador;
struct bullet_t balas[P_BULLETS];
struct bullet_t balasEnemigas[E_BULLETS];
int pausarDistancia;
Uint32 pausarTiempo;
enum states_t estado;
Uint32 tiempoTitulo;

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

void invadersInit() {
    invaders.direction = right;
    invaders.speed = 1;
    invaders.state = 0;
    invaders.killed = 0;
    invaders.stateSpeed = 1000;
    invaders.stateTime = SDL_GetTicks();
    int x = 0;
    int y = 30;
    for(int i = 0; i < 5; i++) {
        for(int j = 0; j < 10; j++) {
            invaders.enemies[i][j].alive = 1;
            invaders.enemies[i][j].hitbox.x = x;
            invaders.enemies[i][j].hitbox.y = y;
            invaders.enemies[i][j].hitbox.w = E_WIDTH;
            invaders.enemies[i][j].hitbox.h = E_HEIGHT;
            x += E_WIDTH + 15;
            if (i == 0) {
                invaders.enemies[i][j].color = purple;
                invaders.enemies[i][j].points = 30;
            }
            else if (i >= 1 && i < 3) {
                invaders.enemies[i][j].color = green;
                invaders.enemies[i][j].points = 20;
            } else {
                invaders.enemies[i][j].color = red;
                invaders.enemies[i][j].points = 10;
            }
        }
        x = 0;
        y += E_HEIGHT + 15;        
    }
}

void playerInit() {
    player.hitbox.x = (WIDTH / 2) - (P_WIDTH / 2);
    player.hitbox.y = HEIGHT - (P_HEIGHT + 10);
    player.hitbox.w = P_WIDTH;
    player.hitbox.h = P_HEIGHT;
    player.lives = 3;
}

void basesInit() {
    int baseTotal = BASE_WIDTH * 4;
    int spaceLeft = WIDTH - baseTotal;
    int evenSpace = spaceLeft / 5;
    int x = evenSpace;
    int y = 500;
    for(int i = 0; i < BASE; i++) {
        base[i].hitbox.x = x;
		base[i].hitbox.y = y;
		base[i].hitbox.w = BASE_WIDTH;
        base[i].hitbox.h = BASE_HEIGHT;
        x += BASE_WIDTH + evenSpace;
    }
}

void saucerInit() {
    saucer.hitbox.x = 0;	
	saucer.hitbox.y	= 0;
	saucer.hitbox.w	= 30;
	saucer.hitbox.h = 20;
	saucer.alive = 0;
    saucer.direction = right;
}

void bulletsInit(struct bullet_t *b, int max) {
    for(int i = 0; i < max; i++) {
        b[i].alive = 0;
		b[i].hitbox.x = 0;
		b[i].hitbox.y = 0;
		b[i].hitbox.w = B_WIDTH;
        b[i].hitbox.h = B_HEIGHT;
    }
}

void backgroundDraw() {
    SDL_Rect src;
	src.x = 0;
	src.y = 0;
	src.w = screen->w;
	src.h = screen->h;
	SDL_FillRect(screen, &src, 0);
}

int main(){
	printf("Hola Daniel");
	return 0;
}