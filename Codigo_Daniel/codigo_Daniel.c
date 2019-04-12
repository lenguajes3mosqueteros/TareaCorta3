#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <SDL/SDL.h/>
#define ANCHO_PANTALLA 800 
#define ALTO_PANTALLA 600
#define ANCHO 600 
#define ALTO 600
#define ANCHO_E 30
#define ALTO_E 30
#define ANCHO_J 30
#define ALTO_J 10
#define ANCHO_B 5
#define ALTO_B 15
#define BALAS_J 1
#define BALAS_E 3
#define BASE 4
#define ANCHO_BASE 60
#define ALTO_BASE 40

enum direcciones_t {izquierda, derecha, estatico};
enum estados_t {menu, opciones, juego, juegoTerminado, pausa};
enum clavesColor_t {magenta, lima};

struct puntuacion_t {
    int disparos;
    int puntos;
    int nivel;
};

struct disco_t {
    SDL_Rect limite;
    int vivo;
    enum direcciones_t direccion;
};

struct enemigo_t {
    SDL_Rect limite;
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
    Uint32 estadoduracion;
};

struct base_t {
    SDL_Rect limite;
};

struct jugador_t {
    SDL_Rect limite;
    int vidas;
};

struct bala_t {
    SDL_Rect limite;
    int vivo;
};

static SDL_Surface* pantalla;
static SDL_Surface* pantallaInicio;
static SDL_Surface* cMap;
static SDL_Surface* mapaInvasores;
static SDL_Surface* imagenJugador;
static SDL_Surface* imagenDisco;
static SDL_Surface* imagenBase[4];
static SDL_Surface* imagenDetrimento;
static SDL_Surface* imagenDetrimentoSuperior;
static SDL_Surface* imagenJuegoTerminado;
struct puntuacion_t puntuacion;
struct invasores_t invasores;
struct disco_t disco;
struct base_t base[BASE];
struct jugador_t jugador;
struct bala_t balas[BALAS_J];
struct bala_t balasEnemigas[BALAS_E];
int pausaDuracion;
Uint32 pausaTiempo;
enum estados_t estado;
Uint32 duracionTitulo;

int dibujarChar(char c, int x, int y) {
    SDL_Rect fuente, destino;
    char *map[] = {"ABCDEFGHIJKLMNOPQRSTUVWXYZ", "abcdefghijklmnopqrstuvwxyz", "!@#$%^&*()_+{}|:\"<>?,.;'-=", "0123456789"};
    fuente.x = 0;
	fuente.y = 0;
	fuente.w = 20;
	fuente.h = 20;
	destino.x = x;
	destino.y = y;
	destino.w = 20;
	destino.h = 20;
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < strlen(map[i]); j++){
            if (c == map[i][j]) {
                SDL_BlitSurface(cMap, &fuente, pantalla, &destino);
                return 0;
            }
            fuente.x += 20;
        }
        fuente.y += 20;
        fuente.x = 0;
    }
    return 0;
}

void dibujarString(char* s, int x, int y) {
	for (int i = 0; i < strlen(s); i++) {
		dibujarChar(s[i], x, y);
		x += 20;
    }
}

void pausar(int duracion) {
    estado = pausa;
    pausaTiempo = SDL_GetTicks();
    pausaDuracion = duracion;
}

int cargarImagen(char* nombreArchivo, SDL_Surface** superficie, enum clavesColor_t claveDeColor) {
    SDL_Surface* temp = SDL_LoadBMP(nombreArchivo);
    Uint32 claveColor;
    if (temp == NULL) {
        printf("Unable to load %s\n", nombreArchivo);
        return 1;
    }
    if (claveDeColor == magenta) {
        claveColor = SDL_MapRGB(temp->format, 255, 0, 255);
    }
    else if (claveDeColor == lima) {
        claveColor = SDL_MapRGB(temp->format, 0, 255, 0);
    }
    SDL_SetColorKey(temp, SDL_TRUE, claveColor);
    (*superficie) = SDL_DisplayFormat(temp);
    if ((*superficie) == NULL) {
        printf("Unable to convert bitmap\n");
        return 1;
    }
    SDL_FreeSurface(temp);
    return 0;
}

void iniciarPuntuacion() {
    puntuacion.disparos = 0;
    puntuacion.puntos = 0;
    puntuacion.nivel = 1;
}

void iniciarInvasores() {
    invasores.direccion = derecha;
    invasores.velocidad = 1;
    invasores.estado = 0;
    invasores.asesinado = 0;
    invasores.estadoVelocidad = 1000;
    invasores.estadoduracion = SDL_GetTicks();
    int x = 0;
    int y = 30;
    for(int i = 0; i < 5; i++) {
        for(int j = 0; j < 10; j++) {
            invasores.enemigos[i][j].vivo = 1;
            invasores.enemigos[i][j].limite.x = x;
            invasores.enemigos[i][j].limite.y = y;
            invasores.enemigos[i][j].limite.w = ANCHO_E;
            invasores.enemigos[i][j].limite.h = ALTO_E;
            x += ANCHO_E + 15;
            if (i == 0) {
                invasores.enemigos[i][j].puntos = 10;
            }
            else if (i >= 1 && i < 3) {
                invasores.enemigos[i][j].puntos = 20;
            } else {
                invasores.enemigos[i][j].puntos = 40;
            }
        }
        x = 0;
        y += ALTO_E + 15;        
    }
}

void iniciarJugador() {
    jugador.limite.x = (ANCHO / 2) - (ANCHO_J / 2);
    jugador.limite.y = ALTO - (ALTO_J + 10);
    jugador.limite.w = ANCHO_J;
    jugador.limite.h = ALTO_J;
    jugador.vidas = 3;
}

void iniciarBases() {
    int baseTotal = ANCHO_BASE * 4;
    int espacioIzquierdo = ANCHO - baseTotal;
    int espacioCerrado = espacioIzquierdo / 5;
    int x = espacioCerrado;
    int y = 500;
    for(int i = 0; i < BASE; i++) {
        base[i].limite.x = x;
		base[i].limite.y = y;
		base[i].limite.w = ANCHO_BASE;
        base[i].limite.h = ALTO_BASE;
        x += ANCHO_BASE + espacioCerrado;
    }
    cargarImagen("base.bmp", &imagenBase[0], magenta);
    cargarImagen("base.bmp", &imagenBase[1], magenta);
    cargarImagen("base.bmp", &imagenBase[2], magenta);
    cargarImagen("base.bmp", &imagenBase[3], magenta);
}

void iniciarDisco() {
    disco.limite.x = 0;	
	disco.limite.y	= 0;
	disco.limite.w	= 30;
	disco.limite.h = 20;
	disco.vivo = 0;
    disco.direccion = derecha;
}

void iniciarBalas(struct bala_t *b, int max) {
    for(int i = 0; i < max; i++) {
        b[i].vivo = 0;
		b[i].limite.x = 0;
		b[i].limite.y = 0;
		b[i].limite.w = ANCHO_BASE;
        b[i].limite.h = ALTO_BASE;
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

void dibujarHUD() {
    SDL_Rect r;
    r.x = ANCHO;
    r.y = 0;
    r.h = ALTO_PANTALLA;
    r.w = ANCHO_PANTALLA - ANCHO;
    SDL_FillRect(pantalla, &r, 41);
    char etiquetaPuntuacion[] = "Puntuacion";
    dibujarString(etiquetaPuntuacion, ANCHO, 0);
    char etiquetaNum[10];
    SDL_snprintf(etiquetaNum, 10, "%d", puntuacion.puntos);
    dibujarString(etiquetaNum, ANCHO, 20);
    char nivel[] = "Nivel";
    dibujarString(nivel, ANCHO, 60);
    char nivelNum[2];
    SDL_snprintf(nivelNum, 2, "%d", puntuacion.nivel);
    dibujarString(nivelNum, ANCHO, 80);
    char vidas[] = "vidas";
    dibujarString(vidas, ANCHO, 120);
    char limitesNum[2];
    SDL_snprintf(limitesNum, 2, "%d", jugador.vidas);
    dibujarString(limitesNum, ANCHO, 140);
}

void dibujarPantallaInicio() {
    SDL_Rect fuente;
    SDL_Rect destino;
    fuente.x = 0;
    fuente.y = 0;
    fuente.w = pantallaInicio->w;
    fuente.h = pantallaInicio->h;
    destino.x = (ANCHO_PANTALLA / 2) - (pantallaInicio->w / 2);
    destino.y = 0;
    destino.w = pantallaInicio->w;
    destino.h = pantallaInicio->h;
    SDL_BlitSurface(pantallaInicio, &fuente, pantalla, &destino);
}

void dibujarDisco() {
    SDL_Rect fuente;
	fuente.x = 0;
	fuente.y = 0;
	fuente.w = 30;
	fuente.h = 20;
	if (disco.vivo == 1) {
		SDL_BlitSurface(imagenDisco, &fuente, pantalla, &disco.limite);
	}
}

void dibujarInvasores() {
    SDL_Rect fuente, destino;
    fuente.w = ANCHO_E;
    fuente.h = ALTO_E;
    for(int i = 0; i < 5; i++) {
        for(int j = 0; j < 10; j++) {
            if (invasores.enemigos[i][j].vivo == 1) {
                if (i == 0) {
                    if (invasores.estado == 0) {
                        fuente.x = 0;
                        fuente.y = 0;
                    } else {
                        fuente.x = 30;
                        fuente.y = 0;
                    } 
                }
                else if (i > 0 && i < 3) {
                    if (invasores.estado == 0) {
                        fuente.x = ALTO_E;
                        fuente.y = 0;
                    } else {
                        fuente.x = 30;
                        fuente.y = 0;
                    } 
                } else {
                    if (invasores.estado == 0) {
                        fuente.x = 0;
                        fuente.y = ALTO_E * 2;
                    } else {
                        fuente.x = 30;
                        fuente.y = ALTO_E * 2;
                    } 
                }
                destino.x = invasores.enemigos[i][j].limite.x;
                destino.y = invasores.enemigos[i][j].limite.y;
                destino.w = invasores.enemigos[i][j].limite.w;
                destino.h = invasores.enemigos[i][j].limite.h;
                SDL_BlitSurface(mapaInvasores, &fuente, pantalla, &destino);
            }
        }
    }
}

void dibujarBases() {
    SDL_Rect fuente;
	fuente.x = 0;
	fuente.y = 0;
	fuente.w = ANCHO_BASE;
	fuente.h = ALTO_BASE;
    for(int i = 0; i < BASE; i++) {
        SDL_BlitSurface(imagenBase[i], &fuente, pantalla, &base[i].limite);
    }
    
}

void dibujarJugador() {
    SDL_Rect fuente;
	fuente.x = 0;
	fuente.y = 0;
	fuente.w = ANCHO_J;
	fuente.h = ALTO_J;
	SDL_BlitSurface(imagenJugador, &fuente, pantalla, &jugador.limite);
}

void dibujarBalas(struct bala_t* b, int max) {
    for(int i = 0; i < max; i++) {
        if (b[i].vivo == 1) {
            SDL_FillRect(pantalla, &b[i].limite, 255);
        }
    }
}

void dibujarJuegoTerminado() {
    SDL_Rect fuente, destino;
	fuente.x = 0;
	fuente.y = 0;
	fuente.w = imagenJuegoTerminado->w;
	fuente.h = imagenJuegoTerminado->h;
	destino.x = (ANCHO / 2) - (imagenJuegoTerminado->w / 2);
	destino.y = (ALTO / 2) - (imagenJuegoTerminado->h / 2);
	destino.w = imagenJuegoTerminado->w;
	destino.h = imagenJuegoTerminado->h;
	SDL_BlitSurface(imagenJuegoTerminado, &fuente, pantalla, &destino);
}

void velocidadInvasores() {
    switch (invasores.asesinado)
    {
        case 10:
            invasores.velocidad = 2;
            invasores.estadoVelocidad = 800;
            break;
        case 20:
            invasores.velocidad = 4;
            invasores.estadoVelocidad = 600;
            break;
        case 30:
            invasores.velocidad = 2;
            invasores.estadoVelocidad = 800;
            break;
    }
}

int moverBalas(struct bala_t* b, int max, int velocidad) {
    for(int i = 0; i < max; i++) {
        if (b[i].vivo == 1) {
            b[i].limite.y += velocidad;
            if (b[i].limite.y <= 0) {
                b[i].vivo = 0;
            }
            else if (b[i].limite.y + b[i].limite.y >= ALTO) {
                b[i].vivo = 0;
            }
        }
    }
    return 0;
}

void moverInvasoresVertical() {
    for(int i = 0; i < 5; i++) {
        for(int j = 0; j < 10; j++) {
            invasores.enemigos[i][j].limite.y += 15;
        }
    }
}

int moverInvasores(int velocidad) {
    velocidadInvasores();
    switch (invasores.direccion)
    {
        case izquierda:
            for(int i = 0; i < 10; i++) {
                for(int j = 0; j < 5; j++){
                    if (invasores.enemigos[j][i].vivo == 1) {
                        if (invasores.enemigos[j][i].limite.x <= 0) {
                            invasores.direccion = derecha;
                            moverInvasoresVertical();
                            return 0;
                        }
                        if (invasores.estadoduracion + invasores.estadoVelocidad < SDL_GetTicks()) {
                            invasores.estadoduracion = SDL_GetTicks();
                            if (invasores.estado == 1) {
                                invasores.estado = 0;
                            } else {
                                invasores.estado = 1;
                            }
                        }
                        invasores.enemigos[i][j].limite.x -= invasores.velocidad;
                    }
                }
            }
            break;
        case derecha:
            for(int i = 9; i >= 0; i--) {
                for(int j = 0; j < 5; i++) {
                    if (invasores.enemigos[j][i].vivo == 1) {
                        if (invasores.enemigos[j][i].limite.x + ANCHO_E >= ANCHO) {
                            invasores.direccion = izquierda;
                            moverInvasoresVertical();
                            return 0;
                        }
                    } 
                   if (invasores.estadoduracion + invasores.estadoVelocidad < SDL_GetTicks()) {
                        invasores.estadoduracion = SDL_GetTicks();
                        if (invasores.estado == 1) {
                            invasores.estado = 0;
                        } else {
                            invasores.estado = 1;
                        }
                    }
                    invasores.enemigos[i][j].limite.x -= invasores.velocidad;
                }
            }
        default:
            break;
    }
    return 0;
}

void moverJugador(enum direcciones_t direccion) {
    if (direccion == izquierda) {
        if (jugador.limite.x > 0) {
            jugador.limite.x -= 10;
        }
    }
    else if (direccion == derecha) {
        if (jugador.limite.x + jugador.limite.w < ANCHO) {
            jugador.limite.x += 10;
        }
    }
}

void moverDisco() {
    if (disco.vivo == 1) {
        if (disco.direccion == izquierda) {
            disco.limite.x -= 5;
            if (disco.limite.x < 0) {
                disco.vivo = 0;
                disco.limite.x = 0;
                disco.direccion = derecha;
            }
        }
        if (disco.direccion == derecha) {
            disco.limite.x += 5;
            if (disco.limite.x + disco.limite.w > ANCHO) {
                disco.vivo = 0;
                disco.limite.x = ANCHO - disco.limite.w;
                disco.direccion = izquierda;
            }
        } 
    }
}

int colision(SDL_Rect a, SDL_Rect b) {
    if (a.y + a.h < b.y) {
		return 0;
	}		
	if (a.y > b.y + b.h) {		
		return 0;
	}		
	if (a.x > b.x + b.w) {		
		return 0;
	}	
	if (a.x + a.w < b.x) {		
		return 0;
	}
	return 1;
}

void DetrimentoBala(struct base_t* base, int b, struct bala_t* bala, int l) {
    int x, y;
    SDL_Rect fuente, destino;
    SDL_LockSurface(imagenBase[b]);
    Uint8* pixeles = (Uint8*)imagenBase[b]->pixels;
    int pix;
    if (l == 0) {
        x = (bala->limite.x + 3) - base->limite.x;
        y = base->limite.y - 1;
        for(int i = 0; i < base->limite.h; i++) {
            if (x >= ANCHO_BASE) {
                x = ANCHO_BASE - 1;
            }
            pix = y * imagenBase[b]->pitch + x;
            if (pixeles[pix] != 200) {
                bala->vivo = 0;
                fuente.x = 0;
				fuente.y = 0;
				fuente.w = 11;
				fuente.h = 15;
				destino.x = x - 3;
				destino.y = y - 14;
				destino.w = 11;
				destino.h = 15;
                SDL_UnlockSurface(imagenBase[b]);
                SDL_BlitSurface(imagenDetrimento, &fuente, imagenBase[b], &destino);
                break;
            }
            y--;
        } 
    }
    if (l == 1) {
        x = (bala->limite.x + 3) - base->limite.x;
        y = 0;
        for(int i = 0; i < base->limite.h; i++) {
            if (x >= ANCHO_BASE) {
                x = ANCHO_BASE - 1;
            }
            pix = y * imagenBase[b]->pitch + x;
            if (pixeles[pix] != 200) {
                bala->vivo = 0;
                fuente.x = 0;
				fuente.y = 0;
				fuente.w = 11;
				fuente.h = 15;
				destino.x = x - 3;
				destino.y = y - 14;
				destino.w = 11;
				destino.h = 15;
                SDL_UnlockSurface(imagenBase[b]);
                SDL_BlitSurface(imagenDetrimento, &fuente, imagenBase[b], &destino);
                break;
            }
            y++;
        } 
    }
    SDL_UnlockSurface(imagenBase[b]);
}

void DetrimentoEnemigo(struct enemigo_t* enemigo, struct base_t* base, int indice) {
    int x, y;
    SDL_Rect destino;
    if (invasores.direccion == derecha) {
        x = (enemigo->limite.x + invasores.velocidad) - base->limite.x;
        y = enemigo->limite.y - base->limite.y;
        if (x > 0) {
            destino.x = 0;
            destino.y = y;
            destino.w = x;
            destino.h = enemigo->limite.h;
            SDL_FillRect(imagenBase[indice], &destino, 227);
        }
    }
    else if (invasores.direccion == izquierda) {
        x = (enemigo->limite.x + (enemigo->limite.w - 1)) + invasores.velocidad;
        x = x - base->limite.x;
        y = enemigo->limite.y - base->limite.y;
        if (x < base->limite.w) {
            destino.x = x;
            destino.y = y;
            destino.w = base->limite.w - 1;
            destino.h = enemigo->limite.h;
            SDL_FillRect(imagenBase[indice], &destino, 227);
        }
    }
}

void colisionEnemigoBase() {
    int c;
    for(int i = 0; i < 5; i++) {
        for(int j = 0; j < 10; j++) {
            for(int k = 0; k < BASE; i++) {
                if (invasores.enemigos[i][j].vivo == 1) {
                    c = colision(invasores.enemigos[i][j].limite, base[k].limite);
                    if (c == 1) {
                        DetrimentoEnemigo(&invasores.enemigos[i][j], &base[k], k);
                    }
                }
            }
        }
    }   
}

void colisionJugador() {
    int c;
    for(int i = 0; i < BALAS_E; i++) {
        if (balasEnemigas[i].vivo == 1) {
            c = colision(balasEnemigas[i].limite, jugador.limite);
            if (c == 1) {
                if (jugador.vidas >= 0) {
                    jugador.vidas--;
                    pausar(500);
                }
            }
        }
    } 
}

void colisionEnemiga() {
    int c;
    for(int i = 0; i < 5; i++) {
        for(int j = 0; j < 10; j++) {
            if (invasores.enemigos[i][j].vivo == 1) {
                for(int k = 0; k < BALAS_J; k++) {
                    if (balas[k].vivo == 1) {
                        c = colision(balas[k].limite, invasores.enemigos[i][j].limite);
                        if (c == 1) {
                            invasores.enemigos[i][j].vivo = 0;
                            balas[k].vivo = 0;
                            balas[k].limite.x = 0;
                            balas[k].limite.y = 0;
                            invasores.asesinado++;
                            puntuacion.puntos += invasores.enemigos[i][j].puntos;
                        }
                    }
                }
            }
        }
    }
}

void colisionDisco() {
    int c;
    if (disco.vivo == 1) {
        for(int i = 0; i < BALAS_J; i++) {
            if (balas[i].vivo == 1) {
                c = colision(balas[i].limite, disco.limite);
                if (c == 1) {
                    int r = rand() % 3;
                    switch (r) {
                        case 0:
                            puntuacion.puntos += 50;
                            break;
                        case 1:
                            puntuacion.puntos += 150;
                            break;
                        case 2:
                            puntuacion.puntos += 300;
                            break;
                        default:
                            break;
                    }
                    disco.vivo = 0;
                    if (disco.direccion == izquierda) {
                        disco.limite.x = 0;
                        disco.direccion = derecha;
                    }
                    else if (disco.direccion == derecha) {
                        disco.limite.x = ANCHO - disco.limite.x;
                        disco.direccion = izquierda;
                    }
                }
            }
        }
    }
}

int colisionJugadorEnemigo() {
    int c;
    for(int i = 0; i < 5; i++) {
        for(int j = 0; j < 10; j++) {
            if (invasores.enemigos[i][j].vivo == 1) {
                c = colision(jugador.limite, invasores.enemigos[i][j].limite);
                if (c == 1) {
                    jugador.vidas--;
                    pausar(500);
                    iniciarInvasores();
                    iniciarBases();
                    return 1;
                }
            }
        }
    }
    return 0;
}

void colisionBalaBase(struct bala_t* bala, int max, int l) {
    int c;
    for(int i = 0; i < max; i++) {
        for(int j = 0; j < BASE; j++) {
            if (bala[i].vivo == 1) {
                c = colision(bala[i].limite, base[j].limite);
                if (c == 1) {
                    DetrimentoBala(&base[j], j, &bala[i], l);
                }
            }
        }
    }
}

void juegoTerminadoIA() {
    if (jugador.vidas == 0) {
        estado = juegoTerminado;
    }
}

void disparoJugador() {
    for(int i = 0; i < BALAS_J; i++) {
        if (balas[i].vivo == 1) {
            puntuacion.disparos++;
            balas[i].limite.x = jugador.limite.x + (ANCHO_J / 2);
            balas[i].limite.y = jugador.limite.y - (balas[i].limite.h + 10);
            balas[i].vivo = 1;
            break;
        }
    }
}

void calcularNivel() {
    if (invasores.asesinado != 0 && invasores.asesinado % 50 == 0) {
        puntuacion.nivel++;
        iniciarInvasores();
        iniciarBases();
        iniciarDisco();
        pausar(500);
    }
}

void discoIA() {
    if (puntuacion.disparos != 0 && puntuacion.puntos % 20 == 0) {
        disco.vivo = 1;
    }
}

void enemigoIA() {
    for(int i = 0; i < 10; i++) {
        for(int j = 4; j >= 0; j--) {
            if (invasores.enemigos[j][i].vivo == 1) {
                int puntoMedio = jugador.limite.x + (jugador.limite.w / 2);
                int inicio = invasores.enemigos[j][i].limite.x;
                int final = invasores.enemigos[j][i].limite.x + invasores.enemigos[j][i].limite.w;
                if (puntoMedio > inicio && puntoMedio < final) {
                    for(int k = 0; k < BALAS_E; k++) {
                        if (balasEnemigas[k].vivo == 0) {
                            int r = rand() % 30;
                        }
                        break;
                    }
                }
            }
            break;  
        }
    }
}

void juegoPausado() {
    if (SDL_GetTicks() > (pausaTiempo + pausaDuracion)) {
        estado = juego;
    }
}

int main(){
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Unable to initialize SDL: %s\n", SDL_GetError());
        return 1;
    }
    atexit(SDL_Quit);
    SDL_WM_SetCaption("Space Invaders", "P");
    pantalla = SDL_SetVideoMode(ANCHO_PANTALLA, ALTO_PANTALLA, 8, SDL_DOUBLEBUF);    
    if (pantalla == NULL) {
        printf("Unable to set video mode: %s\n", SDL_GetError());
        return 1;
    }
    cargarImagen("tilescreen.bmp", &pantallaInicio, magenta);
    cargarImagen("cmap.bmp", &cMap, magenta);
    cargarImagen("invaders.bmp", &mapaInvasores, magenta);
    cargarImagen("player.bmp", &imagenJugador, magenta);
    cargarImagen("saucer.bmp", &imagenDisco, magenta);
    cargarImagen("gameover.bmp", &imagenJuegoTerminado, magenta);
    cargarImagen("damage.bmp", &imagenDetrimento, magenta);
    cargarImagen("damagetop.bmp", &imagenDetrimentoSuperior, magenta);
    Uint32 sigSegundo = SDL_GetTicks();
    int dormir = 0;
    Uint8* estadoTecla = 0;
    int parar = 0;
    SDL_Event evento;
    iniciarPuntuacion();
    iniciarInvasores();
    iniciarBases();
    iniciarJugador();
    iniciarDisco();
    iniciarBalas(balas, BALAS_J);
    iniciarBalas(balasEnemigas, BALAS_E);
    estado = menu;
    duracionTitulo = SDL_GetTicks();
    while(parar == 0){
        estadoTecla = SDL_GetKeyState(NULL);
        while(SDL_PollEvent(&evento)){
            switch (evento.type) {
                case SDL_KEYDOWN:
                    switch (evento.key.keysym.sym)
                    {
                        case SDLK_ESCAPE:
                            parar = 1;
                        break;
                        case SDLK_SPACE:
                            if (estado == menu) {
                                estado = juego;
                            }
                            else if (estado == juego) {
                                disparoJugador();
                                discoIA();
                            }
                            else if (estado == juegoTerminado) {
                                iniciarInvasores();
                                iniciarBases();
                                iniciarPuntuacion();
                                iniciarJugador();
                                estado = juego;
                            }
                        break;
                        default:
                        break;
                    }
                default:
                break;
            }
        }
        dibujarFondo();
        if (estado == menu) {
            char inicio[] = "Presione la barra espaciadora para comenzar";
            SDL_Rect fuente[60];
            if (duracionTitulo + 2000 < SDL_GetTicks()) {
                fuente[0].x = 180;
				fuente[0].y = 40;
				fuente[0].w = 440;
				fuente[0].h = 230;
                SDL_FillRect(pantalla, &fuente[0], 248);
            } else {
                int y = 0;
                for(int i = 0; i < 60; i++)
                {
                    fuente[i].x = 0;
					fuente[i].y = y;
					fuente[i].w = ANCHO_PANTALLA;
					fuente[i].h = 10;
					SDL_FillRect(pantalla, &fuente[i], 227);
                    y += 10; 
                }
                for (int i = 0; i < 60; i++) {
                    SDL_FillRect(pantalla, &fuente[i], rand() % 255);
                }
            }
            dibujarPantallaInicio();
            dibujarString(inicio, (ANCHO_PANTALLA / 2) - (strlen(inicio) * 10), 500);
        }
        else if (estado == juego) {
            if (estadoTecla[SDLK_LEFT]) {
                moverJugador(izquierda);
            }
            else if (estadoTecla[SDLK_RIGHT]) {
                moverJugador(derecha);
            }
            dibujarHUD();
            dibujarJugador();
            dibujarBases();
            dibujarInvasores();
            dibujarDisco();
            dibujarBalas(balas, BALAS_J);
            dibujarBalas(balasEnemigas, BALAS_E);
            colisionEnemiga();
            colisionJugador();
            colisionEnemigoBase();
            colisionDisco();
            colisionBalaBase(balasEnemigas, BALAS_E, 1);
            colisionBalaBase(balas, BALAS_J, 0);
            colisionJugadorEnemigo();
            moverInvasores(invasores.velocidad);
            moverDisco();
            moverBalas(balas, BALAS_J, -30);
            moverBalas(balasEnemigas, BALAS_E, 15);
            calcularNivel();
            enemigoIA();
            juegoTerminadoIA();
            juegoPausado();
        }
        else if (estado == juegoTerminado) {
            dibujarHUD();
            dibujarJugador();
            dibujarBases();
            dibujarInvasores();
            dibujarDisco();
            dibujarBalas(balas, BALAS_J);
            dibujarBalas(balasEnemigas, BALAS_E);
            dibujarJuegoTerminado();
        }
        else if (estado == pausa) {
            dibujarHUD();
            dibujarJugador();
            dibujarBases();
            dibujarInvasores();
            dibujarDisco();
            dibujarBalas(balas, BALAS_J);
            dibujarBalas(balasEnemigas, BALAS_E);
            juegoPausado();
        }
        SDL_Flip(pantalla);
        sigSegundo += 1000 / 30;
        dormir = sigSegundo - SDL_GetTicks();
        if (dormir >= 0) {
            SDL_Delay(dormir);
        }
    }
	return 0;
}
