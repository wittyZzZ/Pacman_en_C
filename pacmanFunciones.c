#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <windows.h>
#include <string.h>
#include "pacmanFunciones.h"

// Definición de colores
#define BLUE 1
#define RED 4
#define GREEN 10
#define YELLOW 6
#define PURPLE 13
#define WHITE 15

// FUNCIÓN PARA CREAR UNA MATRIZ 
char** crearMatriz(int filas, int columnas) {
    char** matriz = malloc(filas * sizeof(char*));
    for (int i = 0; i < filas; i++) {
        matriz[i] = malloc(columnas * sizeof(char));
    }
    return matriz;
}

// COPIAR MATRIZ 
char** copiarMatriz(int filas,int columnas,char** matrizOg) {
    char** newMatriz = crearMatriz(filas,columnas);
    for (int i = 0;i < filas;i++) {
        for (int j = 0;j < columnas;j++) {
            newMatriz[i][j] = matrizOg[i][j];
        }
    }
    return newMatriz;
}

void initializeMatchStats(matchData* match,int id) {
    match->id = id;
    match->muertes = 0;
    match->scoreFinal = 0;
    match->fantasmasComidos = 0;
    match->bigDots = 0;
    match->smallDots = 0;
    match->gameTime = 0;
}

// DELIMITAR LABERINTO
void delimitarLaberinto(int filas,int columnas,char** laberint) {
    // Delimitar filas
    for (int j = 0; j < columnas;j++){
        laberint[0][j] = '#';
    }
    for (int j = 0; j < columnas;j++){
        laberint[filas-1][j] = '#';
    }

    // Delimitar columnas
    for (int i = 0; i < filas;i++){
        laberint[i][0] = '#';
    }
    for (int i = 0; i < filas;i++){
        laberint[i][columnas-1] = '#';
    }
}

// OBTIENE LOS CARACTERES DEL LABERINTO Y LO ADAPTA. GUARDA COORDENADAS RELEVANTES
void obtenerLaberinto(FILE* fp,int filas,int columnas,char** laberint,cord* pacmanSpawn,cord* ghostSpawn,cord* pasillo1,cord* pasillo2,int* conteoPuntitosEnMapa) {
    char caracter;
    for (int i=1;i<filas-1;i++){
        for (int j=1;j<columnas-1;j++){
            caracter = fgetc(fp); //file get caracter
            if (caracter == 'x') {
                laberint[i][j] = '.';
                (*conteoPuntitosEnMapa)++;
            } else if (caracter == 'X') {
                laberint[i][j] = 'o';
                (*conteoPuntitosEnMapa)++;
            } else if (caracter == 'w') {
                laberint[i][j] = '#';
            // Pasillo    
            } else if (caracter == 'P') {
                laberint[i][j] = ' ';
                if (j == 17) {
                    laberint[i][18] = ' ';
                    pasillo2->x = i;
                    pasillo2->y = 18;
                } 
                if (j == 1) {
                    laberint[i][0] = ' ';
                    pasillo1->x = i;
                    pasillo1->y = 0;
                }
            } else if (caracter == 'p') {
                laberint[i][j] = 'C';
                pacmanSpawn->x = i; // Se guardan las coordenadas del punto de aparición de Pacman
                pacmanSpawn->y = j;
            } else if (caracter == 'S') {
                laberint[i][j] = ' ';
                ghostSpawn->x = i; // Se guardan las coordenadas del punto de aparición de los fantasmas
                ghostSpawn->y = j;
            } else if (caracter == ' ') {
                laberint[i][j] = ' ';
            }
        }
        caracter = fgetc(fp); // OBTIENE SALTO DE LINEA
    }
    // Cerrar el archivo fuente
    fclose(fp);
}

// IMPRIMIR LABERINTO
void imprimirLaberinto(int filas, int columnas,char** laberint,int namnam) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // Conecta el codigo con la consola
    for (int i=0;i<filas;i++){
        for (int j=0;j<columnas;j++){
            if (laberint[i][j] == 'C' || laberint[i][j] == '.' || laberint[i][j] == 'o') {
                SetConsoleTextAttribute(hConsole,YELLOW);
                printf("%c",laberint[i][j]);
                SetConsoleTextAttribute(hConsole,WHITE);
            } else if (laberint[i][j] == '#') {
                SetConsoleTextAttribute(hConsole,BLUE);
                printf("%c",laberint[i][j]);
                SetConsoleTextAttribute(hConsole,WHITE);
            } else if (laberint[i][j] == 'B' || laberint[i][j] == 'M' || laberint[i][j] == 'K' || laberint[i][j] == 'E') {
                if (namnam == 0) {
                    SetConsoleTextAttribute(hConsole,PURPLE);
                    printf("%c",laberint[i][j]);
                    SetConsoleTextAttribute(hConsole,WHITE);
                } else {
                    SetConsoleTextAttribute(hConsole,WHITE);
                    printf("%c",laberint[i][j]);
                }
            } else if (laberint[i][j] == '6') {
                SetConsoleTextAttribute(hConsole,RED);
                printf("%c",laberint[i][j]);
                SetConsoleTextAttribute(hConsole,WHITE);
            } else if (laberint[i][j] == 'J') {
                SetConsoleTextAttribute(hConsole,GREEN);
                printf("%c",laberint[i][j]);
                SetConsoleTextAttribute(hConsole,WHITE);
            } else {
                printf("%c",laberint[i][j]);
            }
        }
        printf("\n");
    }
}


// INICIALIZAR PACMAN
void initializePacman(pacman* pacman,cord* pacmanSpawn,int vidas) {
    pacman->coordenadas = *pacmanSpawn; // Coordenadas iniciales
    pacman->score = 0;
    pacman->vidas = vidas;
    pacman->vx = 0;
    pacman->vy = 0;
}

// INICIALIZAR FANTASMA
void initializeGhost(ghost* ghost,int spawnSpaces,time_t spawnTimer,cord ghostSpawn) {
    ghost->spawned = 0;
    ghost->spawnTimer = spawnTimer;
    ghost->coordenadas = ghostSpawn;
    ghost->comido = ' ';
    ghost->vx = -1;
    ghost->vy = 0;
    ghost->spawnSpaces = spawnSpaces;
}

// PERMITE A LOS FANTASMAS SPAWNEAR
void letGhostSpawn(ghost* ghost,char** maze) {
    if (ghost->spawned == 0) {
        double currentGameTime = time(NULL);
        if (difftime(currentGameTime,ghost->spawnTimer) >= 4) {
            ghost->spawnSpaces -= 1;
            if (ghost->spawnSpaces == 0) {
                maze[ghost->coordenadas.x][ghost->coordenadas.y] = 'B';
                ghost->spawned = 1;
            }
        }   
    }
}

// MOVIMIENTO DE PACMAN
void pacmanMovement(pacman* pacman,char** maze,int* conteoPuntitosAux,int* namnam,int* segundosParaComer,time_t* timeToEat,cord* pasillo1,cord* pasillo2,cord* ghostSpawn,ghost* ghost1,ghost* ghost2,ghost* ghost3,ghost* ghost4,int* muertePacman,matchData* match) {
    int lx,ly;
    lx = pacman->coordenadas.x + pacman->vx;
    ly = pacman->coordenadas.y + pacman->vy;

    // PARED
    if (maze[lx][ly] == '#') {
        pacman->vx = 0;
        pacman->vy = 0;
    // PUNTO NORMAL 3
    } else if (maze[lx][ly] == '.') {
        pacman->score += 3;
        (*conteoPuntitosAux) -= 1;
        maze[lx][ly] = 'C';
        maze[pacman->coordenadas.x][pacman->coordenadas.y] = ' ';
        pacman->coordenadas.x = lx;
        pacman->coordenadas.y = ly;
        match->smallDots += 1;
    // PUNTO GRANDE 7
    } else if (maze[lx][ly] == 'o') {
        pacman->score += 7;
        (*conteoPuntitosAux) -= 1;
        maze[lx][ly] = 'C';
        maze[pacman->coordenadas.x][pacman->coordenadas.y] = ' ';
        pacman->coordenadas.x = lx;
        pacman->coordenadas.y = ly;
        (*namnam) = 1;
        (*timeToEat) = time(NULL);
        match->bigDots += 1;
    // GUINDA
    } else if (maze[lx][ly] == '6') {
        maze[lx][ly] = 'C';
        maze[pacman->coordenadas.x][pacman->coordenadas.y] = ' ';
        pacman->coordenadas.x = lx;
        pacman->coordenadas.y = ly;
        (*segundosParaComer) += 2;
    // PLATANO
    } else if (maze[lx][ly] == 'J') {
        maze[lx][ly] = 'C';
        maze[pacman->coordenadas.x][pacman->coordenadas.y] = ' ';
        pacman->coordenadas.x = lx;
        pacman->coordenadas.y = ly;
        maze[ghost1->coordenadas.x][ghost1->coordenadas.y] = ghost1->comido;
        maze[ghost2->coordenadas.x][ghost2->coordenadas.y] = ghost2->comido;
        maze[ghost3->coordenadas.x][ghost3->coordenadas.y] = ghost3->comido;
        maze[ghost4->coordenadas.x][ghost4->coordenadas.y] = ghost4->comido;
        initializeGhost(ghost1,1,time(NULL),*ghostSpawn);
        initializeGhost(ghost2,7,time(NULL),*ghostSpawn);
        initializeGhost(ghost3,13,time(NULL),*ghostSpawn);
        initializeGhost(ghost4,19,time(NULL),*ghostSpawn);
    // PASILLO 1
    } else if (lx == pasillo1->x && ly == pasillo1->y)  {
        maze[pasillo2->x][pasillo2->y] = 'C';
        maze[pacman->coordenadas.x][pacman->coordenadas.y] = ' ';
        pacman->coordenadas.x = pasillo2->x;
        pacman->coordenadas.y = pasillo2->y;
    // PASILLO 2    
    } else if (lx == pasillo2->x && ly == pasillo2->y)  {
        maze[pasillo1->x][pasillo1->y] = 'C';
        maze[pacman->coordenadas.x][pacman->coordenadas.y] = ' ';
        pacman->coordenadas.x = pasillo1->x;
        pacman->coordenadas.y = pasillo1->y;
    // SPAWN DE FANTASMAS (NO DEBE PODER PASAR)
    } else if (lx == ghostSpawn->x && ly == ghostSpawn->y) {
        pacman->vx = 0;
        pacman->vy = 0;
    // ESPACIO LIBRE
    } else if (maze[lx][ly] == ' ') {
        maze[lx][ly] = 'C';
        maze[pacman->coordenadas.x][pacman->coordenadas.y] = ' ';
        pacman->coordenadas.x = lx;
        pacman->coordenadas.y = ly;
    // COLISION CON FANTASMAS
    } else if (maze[lx][ly] == ghost1->letra) {
        if ((*namnam) == 0) {
            maze[lx][ly] = ghost1->letra;
            maze[pacman->coordenadas.x][pacman->coordenadas.y] = ' ';
            pacman->coordenadas.x = lx;
            pacman->coordenadas.y = ly;
            (*muertePacman) = 1;
        } else {
            pacman->score += 10;
            if (ghost1->comido == '.') {
                pacman->score += 3;
                (*conteoPuntitosAux) -= 1;
            } else if (ghost1->comido == 'o') {
                pacman->score += 3;
                (*conteoPuntitosAux) -= 1;
            } else if (ghost1->comido == '6') {
                (*segundosParaComer) += 2;
            }
            maze[lx][ly] = 'C';
            maze[pacman->coordenadas.x][pacman->coordenadas.y] = ' ';
            pacman->coordenadas.x = lx;
            pacman->coordenadas.y = ly;
            initializeGhost(ghost1,1,time(NULL),*ghostSpawn);
            match->fantasmasComidos += 1;
        }
    } else if (maze[lx][ly] == ghost2->letra) {
        if ((*namnam) == 0) {
            maze[lx][ly] = ghost2->letra;
            maze[pacman->coordenadas.x][pacman->coordenadas.y] = ' ';
            pacman->coordenadas.x = lx;
            pacman->coordenadas.y = ly;
            (*muertePacman) = 1;
        } else {
            pacman->score += 10;
            if (ghost2->comido == '.') {
                pacman->score += 3;
                (*conteoPuntitosAux)  -= 1;
            } else if (ghost2->comido == 'o') {
                pacman->score += 3;
                (*conteoPuntitosAux)  -= 1;
            } else if (ghost2->comido == '6') {
                (*segundosParaComer) += 2;
            }
            maze[lx][ly] = 'C';
            maze[pacman->coordenadas.x][pacman->coordenadas.y] = ' ';
            pacman->coordenadas.x = lx;
            pacman->coordenadas.y = ly;
            initializeGhost(ghost2,7,time(NULL),*ghostSpawn);
            match->fantasmasComidos += 1;
        }  
    } else if (maze[lx][ly] == ghost3->letra) {
        if ((*namnam) == 0) {
            maze[lx][ly] = ghost3->letra;
            maze[pacman->coordenadas.x][pacman->coordenadas.y] = ' ';
            pacman->coordenadas.x = lx;
            pacman->coordenadas.y = ly;
            (*muertePacman) = 1;
        } else {
            pacman->score += 10;
            if (ghost3->comido == '.') {
                pacman->score += 3;
                (*conteoPuntitosAux)  -= 1;
            } else if (ghost3->comido == 'o') {
                pacman->score += 3;
                (*conteoPuntitosAux) -= 1;
            } else if (ghost3->comido == '6') {
                (*segundosParaComer) += 2;
            }
            maze[lx][ly] = 'C';
            maze[pacman->coordenadas.x][pacman->coordenadas.y] = ' ';
            pacman->coordenadas.x = lx;
            pacman->coordenadas.y = ly;
            initializeGhost(ghost3,13,time(NULL),*ghostSpawn);
            match->fantasmasComidos += 1;
        } 
    } else if (maze[lx][ly] == ghost4->letra) {
        if ((*namnam) == 0) {
            maze[lx][ly] = ghost4->letra;
            maze[pacman->coordenadas.x][pacman->coordenadas.y] = ' ';
            pacman->coordenadas.x = lx;
            pacman->coordenadas.y = ly;
            (*muertePacman) = 1;
        } else {
            pacman->score += 10;
            if (ghost4->comido == '.') {
                pacman->score += 3;
                (*conteoPuntitosAux) -= 1;
            } else if (ghost4->comido == 'o') {
                pacman->score += 3;
                (*conteoPuntitosAux) -= 1;
            } else if (ghost4->comido == '6') {
                (*segundosParaComer) += 2;
            }
            maze[lx][ly] = 'C';
            maze[pacman->coordenadas.x][pacman->coordenadas.y] = ' ';
            pacman->coordenadas.x = lx;
            pacman->coordenadas.y = ly;
            initializeGhost(ghost4,19,time(NULL),*ghostSpawn);
            match->fantasmasComidos += 1;
        }   
    }
}

// MOVIMIENTO DE LOS FANTASMAS
void ghostMovement(ghost* ghost,pacman* pacman,char** maze,cord* pasillo1,cord* pasillo2,cord* ghostSpawn,int* muertePacman,int namnam,matchData* match) {
    int arrayGhostMov[2] = {1,-1};
    int gx,gy;
    if (ghost->spawned == 1) {
        gx = ghost->coordenadas.x + ghost->vx;
        gy = ghost->coordenadas.y + ghost->vy;
        // PARED
        if (maze[gx][gy] == '#' || maze[gx][gy] == 'B' || maze[gx][gy] == 'M' || maze[gx][gy] == 'K' || maze[gx][gy] == 'E') {
            if (ghost->vx != 0) {
                ghost->vx = 0;
                srand(time(NULL));
                int rIndex = rand() % 2;
                ghost->vy = arrayGhostMov[rIndex];
            } else {
                ghost->vy = 0;
                srand(time(NULL));
                int rIndex = rand() % 2;
                ghost->vx = arrayGhostMov[rIndex];
            }
        // PUNTO NORMAL    
        } else if (maze[gx][gy] == '.') {
            maze[ghost->coordenadas.x][ghost->coordenadas.y] = ghost->comido;
            maze[gx][gy] = ghost->letra;
            ghost->comido = '.';
            ghost->coordenadas.x = gx;
            ghost->coordenadas.y = gy;
        // PUNTO GRANDE 
        } else if (maze[gx][gy] == 'o') {
            maze[ghost->coordenadas.x][ghost->coordenadas.y] = ghost->comido;
            maze[gx][gy] = ghost->letra;
            ghost->comido = 'o';
            ghost->coordenadas.x = gx;
            ghost->coordenadas.y = gy;
        // GUINDA
        } else if (maze[gx][gy] == '6') {
            maze[ghost->coordenadas.x][ghost->coordenadas.y] = ghost->comido;
            maze[gx][gy] = ghost->letra;
            ghost->comido = '6';
            ghost->coordenadas.x = gx;
            ghost->coordenadas.y = gy;
        // PLATANO
        } else if (maze[gx][gy] == 'J') {
            maze[ghost->coordenadas.x][ghost->coordenadas.y] = ghost->comido;
            maze[gx][gy] = ghost->letra;
            ghost->comido = 'J';
            ghost->coordenadas.x = gx;
            ghost->coordenadas.y = gy;
        // PASILLO 1
        } else if (gx == pasillo1->x && gy == pasillo1->y)  {
            maze[pasillo2->x][pasillo2->y] = ghost->letra;
            maze[ghost->coordenadas.x][ghost->coordenadas.y] = ' ';
            ghost->coordenadas.x = pasillo2->x;
            ghost->coordenadas.y = pasillo2->y;
        // PASILLO 2    
        } else if (gx == pasillo2->x && gy == pasillo2->y)  {
            maze[pasillo1->x][pasillo1->y] = ghost->letra;
            maze[ghost->coordenadas.x][ghost->coordenadas.y] = ' ';
            ghost->coordenadas.x = pasillo1->x;
            ghost->coordenadas.y = pasillo1->y;
        // SPAWN DE FANTASMAS (NO DEBE PODER PASAR)
        } else if (gx == ghostSpawn->x && gy == ghostSpawn->y) {
            ghost->vx = 0;
            ghost->vy = -1;
        // ESPACIO LIBRE
        } else if (maze[gx][gy] == ' ') {
            maze[ghost->coordenadas.x][ghost->coordenadas.y] = ghost->comido;
            maze[gx][gy] = ghost->letra;
            ghost->comido = ' ';
            ghost->coordenadas.x = gx;
            ghost->coordenadas.y = gy;
        // COLISION CON PACMAN
        } else if (maze[gx][gy] == 'C') {
            if (namnam == 0) {
                maze[ghost->coordenadas.x][ghost->coordenadas.y] = ghost->comido;
                maze[gx][gy] = ghost->letra;
                ghost->comido = ' ';
                ghost->coordenadas.x = gx;
                ghost->coordenadas.y = gy;
                (*muertePacman) = 1;
            } else {
                maze[ghost->coordenadas.x][ghost->coordenadas.y] = ghost->comido;
                maze[gx][gy] = 'C';
                pacman->score += 10;
                match->fantasmasComidos += 1;
                if (ghost->letra == 'B') {
                    initializeGhost(ghost,1,time(NULL),*ghostSpawn);
                } else if (ghost->letra == 'M') {
                    initializeGhost(ghost,7,time(NULL),*ghostSpawn);
                } else if (ghost->letra == 'K') {
                    initializeGhost(ghost,13,time(NULL),*ghostSpawn);
                } else {
                    initializeGhost(ghost,19,time(NULL),*ghostSpawn);
                }
            }  
        }
    }
}

// SPAWNEO DE GUINDAS EN EL MAPA
void spawnGuindas(time_t* startGuindasTime,int guindasTimeSpawn,cord* pacmanSpawn,char** maze,int* segundosParaComer,ghost* ghost1,ghost* ghost2,ghost* ghost3,ghost* ghost4) {
    time_t currentGameTime = time(NULL);
    double tiempoDeGuinda = difftime(currentGameTime,(*startGuindasTime));
    if (tiempoDeGuinda == guindasTimeSpawn) {
        if (maze[pacmanSpawn->x][pacmanSpawn->y] == 'C') {
            (*segundosParaComer) += 2;
        } else if (maze[pacmanSpawn->x][pacmanSpawn->y] == ghost1->letra) {
            ghost1->comido = '6';
        } else if (maze[pacmanSpawn->x][pacmanSpawn->y] == ghost2->letra) {
            ghost2->comido = '6';
        } else if (maze[pacmanSpawn->x][pacmanSpawn->y] == ghost3->letra) {
            ghost3->comido = '6';
        } else if (maze[pacmanSpawn->x][pacmanSpawn->y] == ghost4->letra) {
            ghost4->comido = '6';
        } else {
            maze[pacmanSpawn->x][pacmanSpawn->y] = '6';
        }
        (*startGuindasTime) = time(NULL);
    }
}

// SPAWNEO DE PLATANOS EN EL MAPA
void spawnPlatanos(time_t* startPlatanoTime,int platanoTimeSpawn,cord* pacmanSpawn,char** maze,ghost* ghost1,ghost* ghost2,ghost* ghost3,ghost* ghost4,cord* ghostSpawn) {
    time_t currentGameTime = time(NULL);
    double tiempoDePlatano = difftime(currentGameTime,(*startPlatanoTime));
    if (tiempoDePlatano == platanoTimeSpawn) {
        if (maze[pacmanSpawn->x][pacmanSpawn->y] == 'C') {
            maze[ghost1->coordenadas.x][ghost1->coordenadas.y] = ghost1->comido;
            maze[ghost2->coordenadas.x][ghost2->coordenadas.y] = ghost2->comido;
            maze[ghost3->coordenadas.x][ghost3->coordenadas.y] = ghost3->comido;
            maze[ghost4->coordenadas.x][ghost4->coordenadas.y] = ghost4->comido;  
            initializeGhost(ghost1,1,time(NULL),*ghostSpawn);
            initializeGhost(ghost2,7,time(NULL),*ghostSpawn);
            initializeGhost(ghost3,13,time(NULL),*ghostSpawn);
            initializeGhost(ghost4,19,time(NULL),*ghostSpawn);
        } else if (maze[pacmanSpawn->x][pacmanSpawn->y] == ghost1->letra) {
            ghost1->comido = 'J';
        } else if (maze[pacmanSpawn->x][pacmanSpawn->y] == ghost2->letra) {
            ghost2->comido = 'J';
        } else if (maze[pacmanSpawn->x][pacmanSpawn->y] == ghost3->letra) {
            ghost3->comido = 'J';
        } else if (maze[pacmanSpawn->x][pacmanSpawn->y] == ghost4->letra) {
            ghost4->comido = 'J';
        } else {
            maze[pacmanSpawn->x][pacmanSpawn->y] = 'J';
        }
        (*startPlatanoTime) = time(NULL);
    }
}

// OBTIENE EL INPUT DEL USUARIO PARA CONTROLAR A PACMAN
void userInput(pacman* pacman) {
    // Recibe las teclas que presiona el usuario para controlar a Pacman
    if (_kbhit()) {
        char ch = _getch();
        pacman->vx = 0; // Se reinicia el movimiento de Pacman
        pacman->vy = 0; // Se reinicia el movimiento de Pacman
        // a = 97; d = 100; w = 119; s = 115
        switch(ch) {
            case 97: // A
                pacman->vy = -1;
                break;
            case 100: // D
                pacman->vy = 1;
                break;
            case 119: // W
                pacman->vx = -1;
                Sleep(50);
                break;
            case 115: // S
                pacman->vx = 1;
                Sleep(50);
                break;
        }
    }
}

// PARPADEO DE TABLERO AL MORIR PACMAN
void parpadeoTablero(int filas,int columnas,char** laberint,int param,pacman* pacman) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole,GREEN);printf("Score: %d   Vidas: %d\n",pacman->score,pacman->vidas);SetConsoleTextAttribute(hConsole,WHITE);
    imprimirLaberinto(filas,columnas,laberint,param);
    Sleep(300);system("cls");Sleep(300);
    SetConsoleTextAttribute(hConsole,GREEN);printf("Score: %d   Vidas: %d\n",pacman->score,pacman->vidas);SetConsoleTextAttribute(hConsole,WHITE);
    imprimirLaberinto(filas,columnas,laberint,param);
    Sleep(300);system("cls");Sleep(300);
    SetConsoleTextAttribute(hConsole,GREEN);printf("Score: %d   Vidas: %d\n",pacman->score,pacman->vidas);SetConsoleTextAttribute(hConsole,WHITE);
    imprimirLaberinto(filas,columnas,laberint,param);
    Sleep(300);system("cls");Sleep(300);
}

// OBTIENE LA FECHA ACTUAL EN FORMATO DE STRING
void obtenerFechaActual(char* fecha) {
    time_t tiempo;
    struct tm* fechaActual;

    // Obtener el tiempo actual
    tiempo = time(NULL);
    fechaActual = localtime(&tiempo);

    // Formatear la fecha en formato "dd/mm/yy"
    sprintf(fecha, "%02d-%02d-%02d", fechaActual->tm_mday, fechaActual->tm_mon + 1, fechaActual->tm_year % 100);
}

// IMPRIMIR ESTADISTICAS DE JUEGO
void printStats(matchData* match,double totalGameTime) {
    printf("\n\n   Estadisticas de Juego:\n//-----------------------//\n");
    printf("- Puntos chicos comidos: %d\n",match->smallDots);
    printf("- Puntos grandes comidos: %d\n",match->bigDots);
    printf("- Fantasmas comidos: %d\n",match->fantasmasComidos);
    printf("- Muertes de Pacman: %d\n",match->muertes);
    printf("- Tiempo Total de Juego: %0.lf segundos\n",totalGameTime);
    printf("- Puntaje Obtenido: %d\n",match->scoreFinal);
}

// GENERAR ARCHIVO DE ESTADISTICAS
void generateStatsFile(matchData* match,double totalGameTime) {
    char resultados[32] = "resultados";
    char fecha[9];
    obtenerFechaActual(fecha);
    char out[] = ".out";
    strcat(resultados,fecha);strcat(resultados,out);

    FILE* fOut = fopen(resultados,"a");
    fprintf(fOut,"   Estadisticas de Juego:\n//-----------------------//\n");
    fprintf(fOut,"- Puntos chicos comidos: %d\n",match->smallDots);
    fprintf(fOut,"- Puntos grandes comidos: %d\n",match->bigDots);
    fprintf(fOut,"- Fantasmas comidos: %d\n",match->fantasmasComidos);
    fprintf(fOut,"- Muertes de Pacman: %d\n",match->muertes);
    fprintf(fOut,"- Tiempo Total de Juego: %0.lf segundos\n",totalGameTime);
    fprintf(fOut,"- Puntaje Obtenido: %d\n",match->scoreFinal);
    fclose(fOut);
}