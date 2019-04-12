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
enum direcciones_t {izquierda, derecha, estatico};
enum estados_t {menu, opciones, juego, juegoTerminado, pausa};
enum clavesColor_t {magenta, lima};

struct puntuacion_t {
    int disparos;
    int puntos;
    int nivel;
};

struct platillo_t {
    SDL_Rect vida;
    int vivo;
    enum direcciones_t direccion;
};

struct enemigo_t {
    SDL_Rect vida;
    enum colores_t color;
    int vivo;
    int puntos;
};

struct invasores_t {
    struct enemigo_t enemigos[5][10];
    enum direcciones_t direccion;
    int asesinado;
    int velocidad;
    int estado;
    int estadoVelocidad;
    Uint32 estadoTiempo;
};

struct base_t {
    SDL_Rect vida;
};

struct jugador_t {
    SDL_Rect vida;
    int vidas;
};

struct bala_t {
    SDL_Rect vida;
    int vivo;
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
struct invasores_t invasores;
struct platillo_t platillo;
struct base_t base[BASE];
struct jugador_t jugador;
struct bala_t balas[BALAS_P];
struct bala_t balasEnemigas[BALAS_E];
int pausarDistancia;
Uint32 pausarTiempo;
enum estados_t estado;
Uint32 tiempoTitulo;

int dibujarChar(char* c, int x, int y) {
    SDL_Rect* fuente;
    SDL_Rect* destino;
}

void dibujarString(char* s, int x, int y) {
	for (int i = 0; i < strlen(s); i++) {
		h(s[i], x, y);
		x += 20;
    }
}

void pausa(int distancia) {
    estado = pausa;
    pausarTiempo = SDL_GetTicks();
    pausarDistancia = distancia;
}

int cargarImagen(char* nombreArchivo, SDL_Surface* superficie, enum clavesColor_t claveColor) {
    SDL_Surface* temp = SDL_LoadBMP(nombreArchivo);
    Uint32 claveColor;
    if (temp == NULL) {
        printf("Unable to load %s\n", nombreArchivo);
        return 1;
    }
    if (claveColor == magenta) {
        claveColor = SDL_MapRGB(temp->format, 255, 0, 255);
    }
    else if (claveColor == lima) {
        claveColor = SDL_MapRGB(temp->format, 0, 255, 0);
    }
    SDL_SetColorKey(temp, SDL_TRUE, claveColor);
    superficie = SDL_DisplayFormat(temp);
    if (superficie == NULL) {
        printf("Unable to convert bitmap\n");
        return 1;
    }
    SDL_FreeSurface(temp);
    return 0;
}

void iniciarInvasores() {
    invasores.direccion = derecha;
    invasores.velocidad = 1;
    invasores.estado = 0;
    invasores.asesinado = 0;
    invasores.estadoVelocidad = 1000;
    invasores.estadoTiempo = SDL_GetTicks();
    int x = 0;
    int y = 30;
    for(int i = 0; i < 5; i++) {
        for(int j = 0; j < 10; j++) {
            invasores.enemigos[i][j].vivo = 1;
            invasores.enemigos[i][j].vida.x = x;
            invasores.enemigos[i][j].vida.y = y;
            invasores.enemigos[i][j].vida.w = ANCHO_E;
            invasores.enemigos[i][j].vida.h = ALTO_E;
            x += ANCHO_E + 15;
            if (i == 0) {
                invasores.enemigos[i][j].color = morado;
                invasores.enemigos[i][j].puntos = 30;
            }
            else if (i >= 1 && i < 3) {
                invasores.enemigos[i][j].color = verde;
                invasores.enemigos[i][j].puntos = 20;
            } else {
                invasores.enemigos[i][j].color = rojo;
                invasores.enemigos[i][j].puntos = 10;
            }
        }
        x = 0;
        y += ALTO_E + 15;        
    }
}

void iniciarJugador() {
    jugador.vida.x = (ANCHO / 2) - (ANCHO_P / 2);
    jugador.vida.y = ALTO - (ALTO_P + 10);
    jugador.vida.w = ANCHO_P;
    jugador.vida.h = ALTO_P;
    jugador.vidas = 3;
}

void iniciarBases() {
    int baseTotal = ANCHO_BASE * 4;
    int spaceLeft = ANCHO - baseTotal;
    int evenSpace = spaceLeft / 5;
    int x = evenSpace;
    int y = 500;
    for(int i = 0; i < BASE; i++) {
        base[i].vida.x = x;
		base[i].vida.y = y;
		base[i].vida.w = ANCHO_BASE;
        base[i].vida.h = ALTO_BASE;
        x += ANCHO_BASE + evenSpace;
    }
}

void iniciarPlatillo() {
    platillo.vida.x = 0;	
	platillo.vida.y	= 0;
	platillo.vida.w	= 30;
	platillo.vida.h = 20;
	platillo.vivo = 0;
    platillo.direccion = derecha;
}

void iniciarBalas(struct bala_t *b, int max) {
    for(int i = 0; i < max; i++) {
        b[i].vivo = 0;
		b[i].vida.x = 0;
		b[i].vida.y = 0;
		b[i].vida.w = ANCHO_BASE;
        b[i].vida.h = ALTO_BASE;
    }
}

void dibujarFondo() {
    SDL_Rect fuente;
	fuente.x = 0;
	fuente.y = 0;
	fuente.w = pantalla->w;
	fuente.h = pantalla->h;
	SDL_FillRect(pantalla, &fuente, 0);
}

int main(){
	printf("Hola Daniel");
	return 0;
}