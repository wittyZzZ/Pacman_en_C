#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <windows.h>
#include <string.h>
#include "pacmanFunciones.h"

// Para compilar:
// - gcc main.c pacmanfunciones.c
// - a.exe laberinto.txt

// Definición de colores
#define BLUE 1
#define RED 4
#define GREEN 10
#define YELLOW 6
#define PURPLE 13
#define WHITE 15

// GameOver
char gameOver[8][90] = {"  /$$$$$$   /$$$$$$  /$$      /$$ /$$$$$$$$        /$$$$$$  /$$    /$$ /$$$$$$$$ /$$$$$$$ ",
                        " /$$__  $$ /$$__  $$| $$$    /$$$| $$_____/       /$$__  $$| $$   | $$| $$_____/| $$__  $$",
                        "| $$  \\__/| $$  \\ $$| $$$$  /$$$$| $$            | $$  \\ $$| $$   | $$| $$      | $$  \\ $$",
                        "| $$ /$$$$| $$$$$$$$| $$ $$/$$ $$| $$$$$         | $$  | $$|  $$ / $$/| $$$$$   | $$$$$$$/",
                        "| $$|_  $$| $$__  $$| $$  $$$| $$| $$__/         | $$  | $$ \\  $$ $$/ | $$__/   | $$__  $$",
                        "| $$  \\ $$| $$  | $$| $$\\  $ | $$| $$            | $$  | $$  \\  $$$/  | $$      | $$  \\ $$",
                        "|  $$$$$$/| $$  | $$| $$ \\/  | $$| $$$$$$$$      |  $$$$$$/   \\  $/   | $$$$$$$$| $$  | $$",
                        " \\______/ |__/  |__/|__/     |__/|________/       \\______/     \\_/    |________/|__/  |__/"
};

// You Won
char youWon[8][80] = {  " /$$     /$$ /$$$$$$  /$$   /$$       /$$      /$$  /$$$$$$  /$$   /$$       /$$",
                        "|  $$   /$$//$$__  $$| $$  | $$      | $$  /$ | $$ /$$__  $$| $$$ | $$      | $$",
                        " \\  $$ /$$/| $$  \\ $$| $$  | $$      | $$ /$$$| $$| $$  \\ $$| $$$$| $$      | $$",
                        "  \\  $$$$/ | $$  | $$| $$  | $$      | $$/$$ $$ $$| $$  | $$| $$ $$ $$      | $$",
                        "   \\  $$/  | $$  | $$| $$  | $$      | $$$$_  $$$$| $$  | $$| $$  $$$$      |__/",
                        "    | $$   | $$  | $$| $$  | $$      | $$$/ \\  $$$| $$  | $$| $$\\  $$$          ",
                        "    | $$   |  $$$$$$/|  $$$$$$/      | $$/   \\  $$|  $$$$$$/| $$ \\  $$       /$$",
                        "    |__/    \\______/  \\______/       |__/     \\__/ \\______/ |__/  \\__/      |__/"
};

// ----------------------------------------------------Funciones-------------------------------------------------------- //
// --------------------------------------------------------------------------------------------------------------------- //

// ABRIR UN ARCHIVO EN MODO LECTURA
FILE* abrirArchivo(char *nombreFile) {
    FILE *archivo;
    archivo = fopen(nombreFile,"r");
    if (archivo == NULL) {
        printf("Error al abrir el archivo %s\n",nombreFile);
        return NULL;
    }
    printf("El archivo %s se abre correctamente\n",nombreFile);
    return archivo;
}

// IMPRIMIR GAMEOVER
void printGameOver() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole,RED);
    for (int i = 0;i < 8;i++) {
        for (int j = 0;j < 90;j++) {
            printf("%c",gameOver[i][j]);
        }
        printf("\n"); 
    }
    SetConsoleTextAttribute(hConsole,WHITE);
}

// IMPRIMIR YOU WON
void printYouWon() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole,GREEN);
    for (int i = 0;i < 8;i++) {
        for (int j = 0;j < 80;j++) {
            printf("%c",youWon[i][j]);
        }
        printf("\n"); 
    }
    SetConsoleTextAttribute(hConsole,WHITE);
}

// POSICIONA LO QUE SE IMPRIME EN CONSOLA
void setCursorPosition(int x, int y) {
    COORD coordinates = {x,y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coordinates);
}

// ------------------------------------------------------Main----------------------------------------------------------- //
// --------------------------------------------------------------------------------------------------------------------- //

int main(int argc, char* argv[]) {

    // ABRIR EL ARCHIVO
    FILE* fp = abrirArchivo(argv[1]);

    int filas,columnas,guindas;
    char caracter;

    // Obtiene las filas,columnas y guindas del archivo
    fscanf(fp,"%d %d %d",&filas,&columnas,&guindas);

    // Crea laberinto a partir de las filas y columnas obtenidas del archivo
    char** laberinto = crearMatriz(filas+2,columnas+2);

    caracter = fgetc(fp); // OBTIENE SALTO DE LINEA 

    // Coordenadas del punto de aparición de Pacman en el laberinto
    cord pacmanSpawn;
    // Coordenadas del punto de aparición de los fantasmas en el laberinto
    cord ghostSpawn;
    // Coordenadas del pasillo 1
    cord pasillo1;
    // Coordenadas del pasillo 2
    cord pasillo2;

    // Conteo de los puntos totales que estan esparcidos en el laberinto
    int conteoPuntitosEnMapa = 0;

    // Delimitar el laberinto
    delimitarLaberinto(filas+2,columnas+2,laberinto);

    // Inicializar laberinto
    // Obtiene los carácters del archivo
    obtenerLaberinto(fp,filas+2,columnas+2,laberinto,&pacmanSpawn,&ghostSpawn,&pasillo1,&pasillo2,&conteoPuntitosEnMapa);

    // Creacion de laberinto auxiliar el cual sufrirá los cambios durante el juego, para preservar asi el original intacto
    char** laberintoAux;
    // Creacion de entidades del juego
    pacman pacmanX;
    ghost blinky;blinky.letra = 'B';
    ghost pinky;pinky.letra = 'M';
    ghost inky;inky.letra = 'K';
    ghost clyde;clyde.letra = 'E';

    int bestScores[3];bestScores[0] = 0;bestScores[1] = 0;bestScores[2] = 0;

    int salirDelJuego = 0;


    //-------------------------------------------------SALIR DEL JUEGO----------------------------------------------------------------//
    //--------------------------------------------------------------------------------------------------------------------------------//
        
    // La opción de salir del juego estará disponible cada vez que Pacman gane o pierda sus 3 vidas.
    // Se le preguntará al usuario si desea volver a jugar.
    while (salirDelJuego != 1) {

        system("cls");
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole,GREEN);
        printf("\n@ Bienvenido a Pacman implementado en C!\n\n@ Para controlar a Pacman, utiliza las teclas W A S D\n\n@ Buena suerte! :)");
        SetConsoleTextAttribute(hConsole,WHITE);
        Sleep(5000);
        
        laberintoAux = copiarMatriz(filas+2,columnas+2,laberinto);

        //--------------------------------------------INICIALIZANDO PACMAN----------------------------------------------------------------//
        //--------------------------------------------------------------------------------------------------------------------------------//
        initializePacman(&pacmanX,&pacmanSpawn,3);

        //-------------------------------------------INICIALIZANDO FANTASMAS--------------------------------------------------------------//
        //--------------------------------------------------------------------------------------------------------------------------------//
    
        initializeGhost(&blinky,1,10,ghostSpawn); // B
        initializeGhost(&pinky,7,10,ghostSpawn);  // M
        initializeGhost(&inky,13,10,ghostSpawn);  // K
        initializeGhost(&clyde,19,10,ghostSpawn); // E

        //-------------------------------------------INICIALIZANDO VARIABLES DE JUEGO-----------------------------------------------------//
        //--------------------------------------------------------------------------------------------------------------------------------//

        matchData match;
        initializeMatchStats(&match,1);

        int finDelJuego = 0;
        // Flag que indica si Pacman murio 
        int muertePacman = 0;
        int conteoPuntitosAux = conteoPuntitosEnMapa;
        double tiempoDeJuego;
        double tiempoDeComida;
        double totalGameTime = 0;
        // Es la frecuencia de tiempo en la cual spawnearan guindas
        int guindasTimeSpawn = (180/guindas) - 15;
        // Es la frecuencia de tiempo en la cual spawnearan platanos
        int platanoTimeSpawn = (180/2) - 15;

        // Flag que indica si Pacman puede comerse a los fantasmas
        int namnam = 0;
        int segundosParaComer = 8;
        time_t timeToEat = 10;
        time_t currentGameTime;

        time_t startGameTime = time(NULL);
        time_t startGuindasTime = time(NULL);
        time_t startPlatanoTime = time(NULL);

        // Limpia la consola de comandos
        system("cls");

        //-----------------------------------------------------FIN DEL JUEGO--------------------------------------------------------------//
        //--------------------------------------------------------------------------------------------------------------------------------//

        // Mientras el flag de "finDeJuego" sea distinto de 1, es decir, False, el juego sigue. 
        while (finDelJuego != 1) {

            // Verifica si Pacman ha muerto para ver si se reinicia el tablero
            if (muertePacman == 1) {
                match.scoreFinal = pacmanX.score;
                // Se reinicia el tablero
                laberintoAux = copiarMatriz(filas+2,columnas+2,laberinto); 
                // Se reinicia Pacman
                initializePacman(&pacmanX,&pacmanSpawn,pacmanX.vidas-1); 
                match.muertes += 1;
                totalGameTime += match.gameTime;
                if (pacmanX.vidas != 0) {
                    match.fantasmasComidos = 0;
                    match.smallDots = 0;
                    match.bigDots = 0;
                    match.gameTime = 0;
                }
                // Se reinician los Fantasmas
                initializeGhost(&blinky,1,10,ghostSpawn);
                initializeGhost(&pinky,7,10,ghostSpawn);
                initializeGhost(&inky,13,10,ghostSpawn);
                initializeGhost(&clyde,19,10,ghostSpawn);

                time_t timeToEat = 10;
                namnam = 0;
                segundosParaComer = 8;
            
                conteoPuntitosAux = conteoPuntitosEnMapa; // Reinicio de la cuenta de puntitos comidos.

                // PARPADEO DE TABLERO
                if (pacmanX.vidas != 0) {
                    parpadeoTablero(filas+2,columnas+2,laberintoAux,namnam,&pacmanX);
                }

                // Reinicio de temporizadores del juego
                startGameTime = time(NULL);
                startGuindasTime = time(NULL);
                startPlatanoTime = time(NULL);

                muertePacman = 0;
            }

            //--------------------------------------------------------USER INPUT--------------------------------------------------------------//
            //--------------------------------------------------------------------------------------------------------------------------------//
            
            // Recibe las teclas que presiona el usuario para controlar a Pacman
            userInput(&pacmanX);
                
            //---------------------------------------------------MOVIMIENTO PACMAN---------------------------------------------------------//
            //-----------------------------------------------------------------------------------------------------------------------------//

            pacmanMovement(&pacmanX,laberintoAux,&conteoPuntitosAux,&namnam,&segundosParaComer,&timeToEat,&pasillo1,&pasillo2,&ghostSpawn,&blinky,&pinky,&inky,&clyde,&muertePacman,&match);

            //-----------------------------------------------------SPAWN FANTASMAS------------------------------------------------------------//
            //--------------------------------------------------------------------------------------------------------------------------------//

            letGhostSpawn(&blinky,laberintoAux);
            letGhostSpawn(&pinky,laberintoAux);
            letGhostSpawn(&inky,laberintoAux);
            letGhostSpawn(&clyde,laberintoAux);

            //---------------------------------------------------MOVIMIENTO FANTASMAS---------------------------------------------------------//
            //--------------------------------------------------------------------------------------------------------------------------------//
                // - Los fantasmas podran bloquearse entre sí, no se podrán atravesar.
                // - Los fantasmas no podrán salir del spawn todos al mismo tiempo, lo que conlleva a realizar un "temporizador" para cada uno para permitir
                //   que estos vayan spawneando por orden.
                // - Si dos fantasmas van a chocar entre si, solo cambiaran de dirección.
                // - Los fantasmas si podrán atravesar los puntos y frutas.
                // - Si un fantasma (no comible) choca con Pacman, Pacman pierde una vida.

            ghostMovement(&blinky,&pacmanX,laberintoAux,&pasillo1,&pasillo2,&ghostSpawn,&muertePacman,namnam,&match);
            ghostMovement(&pinky,&pacmanX,laberintoAux,&pasillo1,&pasillo2,&ghostSpawn,&muertePacman,namnam,&match);
            ghostMovement(&inky,&pacmanX,laberintoAux,&pasillo1,&pasillo2,&ghostSpawn,&muertePacman,namnam,&match);
            ghostMovement(&clyde,&pacmanX,laberintoAux,&pasillo1,&pasillo2,&ghostSpawn,&muertePacman,namnam,&match);

            //----SPAWN DE GUINDAS----//
            // Con un temporizador, ir spawneando las guindas.
            spawnGuindas(&startGuindasTime,guindasTimeSpawn,&pacmanSpawn,laberintoAux,&segundosParaComer,&blinky,&pinky,&inky,&clyde);

            //----SPAWN DE PLATANOS----//
            // Con un temporizador, ir spawneando los platanos.
            spawnPlatanos(&startPlatanoTime,platanoTimeSpawn,&pacmanSpawn,laberintoAux,&blinky,&pinky,&inky,&clyde,&ghostSpawn);

            //----TIEMPO PARA COMER FANTASMAS----//
            if (namnam == 1) {
                currentGameTime = time(NULL);
                tiempoDeComida = difftime(currentGameTime,timeToEat);
                if (tiempoDeComida >= segundosParaComer) {
                    namnam = 0;
                }
            }
            
            //----TEMPORIZADOR----//
            // Si el temporizador llega a cero, Pacman pierde una vida, y el flag "muertePacman" cambiará a 1, es decir, True.
            currentGameTime = time(NULL);
            tiempoDeJuego = difftime(currentGameTime,startGameTime);
            if (tiempoDeJuego >= 180) {
                muertePacman = 1;
            }
            match.gameTime = tiempoDeJuego;

            // IMPRIMIR LABERINTO
            SetConsoleTextAttribute(hConsole,GREEN);
            printf("Score: %d   Vidas: %d\n",pacmanX.score,pacmanX.vidas);
            SetConsoleTextAttribute(hConsole,WHITE);
            imprimirLaberinto(filas+2,columnas+2,laberintoAux,namnam);
            SetConsoleTextAttribute(hConsole,GREEN);
            printf("Tiempo de Juego: %.0lf",tiempoDeJuego);
            SetConsoleTextAttribute(hConsole,WHITE);

            Sleep(100);
            setCursorPosition(0,0);

            // Si Pacman pierde todas sus vidas, el flag de "finDelJuego" se vuelve 1, es decir, True.
            if (pacmanX.vidas == 0 || conteoPuntitosAux == 0) {
                totalGameTime += match.gameTime;
                finDelJuego = 1;
            } else {
                match.scoreFinal = pacmanX.score;
            }

        }

        // INFORMAR SOBRE RESULTADO DE JUEGO
        system("cls");
        if (pacmanX.vidas == 0) {
            printGameOver();
            Sleep(2000);
        } else {
            printYouWon();
            Sleep(2000);
        }

        // INFORMAR SOBRE ESTADISTICAS Y GENERAR ARCHIVO "resultadosDD-MM-YY.out"
        SetConsoleTextAttribute(hConsole,GREEN);

        printStats(&match,totalGameTime);

        generateStatsFile(&match,totalGameTime);
        
        // Guardar en Array el Score e informar si es un nuevo record
        int scoreGuardado = 0;
        int i = 0;
        while (i < 3 && scoreGuardado == 0) {
            if (bestScores[i] < match.scoreFinal) {
                if (bestScores[i] != 0) {
                    printf("\n@ OBTUVISTE UN NUEVO RECORD! :D\n");
                } 
                bestScores[i] = match.scoreFinal;
                scoreGuardado = 1;
            }
            i++;
        }

        printf("\n\n0. Jugar otra vez\n1. Salir del Juego\nEscoja una opcion: ");
        SetConsoleTextAttribute(hConsole,WHITE);
        scanf("%d",&salirDelJuego); 
    }

    system("cls");
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole,GREEN);
    printf("\nADIOS,TEN UN BUEN DIA!\n");
    SetConsoleTextAttribute(hConsole,WHITE);
            
    return 0;
}