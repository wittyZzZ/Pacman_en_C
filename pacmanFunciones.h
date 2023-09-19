#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <windows.h>
#include <string.h>

// ---------------------------------------------------Estructuras------------------------------------------------------- //
// --------------------------------------------------------------------------------------------------------------------- //

typedef struct coord{
    int x;
    int y;
}cord;

typedef struct pacman{
    cord coordenadas;
    int vidas;
    int score;
    int vx;
    int vy;
}pacman;

typedef struct fantasma{
    char letra;
    cord coordenadas;
    int vx;
    int vy;
    int spawned;
    char comido;
    int spawnSpaces; // Iteraciones para spawnear
    time_t spawnTimer; // Temporizador de spawneo
}ghost;

typedef struct datosDePartida{
    int id;
    int scoreFinal;
    int fantasmasComidos;
    int muertes;
    int smallDots;
    int bigDots;
    double gameTime;
}matchData;

// ---------------------------------------------------Funciones--------------------------------------------------------- //
// --------------------------------------------------------------------------------------------------------------------- //

char** crearMatriz(int filas, int columnas);

char** copiarMatriz(int filas,int columnas,char** matrizOg);

void initializeMatchStats(matchData* match,int id);

void delimitarLaberinto(int filas,int columnas,char** laberint);

void obtenerLaberinto(FILE* fp,int filas,int columnas,char** laberint,cord* pacmanSpawn,cord* ghostSpawn,cord* pasillo1,cord* pasillo2,int* conteoPuntitosEnMapa);

void imprimirLaberinto(int filas, int columnas,char** laberint,int namnam);

void initializePacman(pacman* pacman,cord* pacmanSpawn,int vidas);

void initializeGhost(ghost* ghost,int spawnSpaces,time_t spawnTimer,cord ghostSpawn);

void letGhostSpawn(ghost* ghost,char** maze);

void pacmanMovement(pacman* pacman,char** maze,int* conteoPuntitosAux,int* namnam,int* segundosParaComer,time_t* timeToEat,cord* pasillo1,cord* pasillo2,cord* ghostSpawn,ghost* ghost1,ghost* ghost2,ghost* ghost3,ghost* ghost4,int* muertePacman,matchData* match);

void ghostMovement(ghost* ghost,pacman* pacman,char** maze,cord* pasillo1,cord* pasillo2,cord* ghostSpawn,int* muertePacman,int namnam,matchData* match);

void spawnGuindas(time_t* startGuindasTime,int guindasTimeSpawn,cord* pacmanSpawn,char** maze,int* segundosParaComer,ghost* ghost1,ghost* ghost2,ghost* ghost3,ghost* ghost4);

void spawnPlatanos(time_t* startPlatanoTime,int platanoTimeSpawn,cord* pacmanSpawn,char** maze,ghost* ghost1,ghost* ghost2,ghost* ghost3,ghost* ghost4,cord* ghostSpawn);

void userInput(pacman* pacman);

void parpadeoTablero(int filas,int columnas,char** laberint,int param,pacman* pacman);

void obtenerFechaActual(char* fecha);

void printStats(matchData* match,double totalGameTime);

void generateStatsFile(matchData* match,double totalGameTime);


