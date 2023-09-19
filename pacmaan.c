#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define FILAS 24
#define COLUMNAS 22

// Estructura que representa la posición en el laberinto
typedef struct {
  int fila;
  int columna;
} Posicion;

// Laberinto
char laberinto[FILAS][COLUMNAS + 1]; // +1 para el carácter nulo terminador

// Posición actual de Pacman
Posicion posicionPacman;

int score = 0; // Puntaje inicial

// Función para imprimir el laberinto
void imprimirLaberinto() {
  for (int i = 0; i < FILAS; i++) {
    printf("%s", laberinto[i]);
  }
}

// Función para mover a Pacman
void moverPacman(char direccion) {
  // Determinar la nueva posición según la dirección
  Posicion nuevaPosicion = posicionPacman;

  if (direccion == 'W' || direccion == 'w') {
    nuevaPosicion.fila--;
  } else if (direccion == 'A' || direccion == 'a') {
    nuevaPosicion.columna--;
  } else if (direccion == 'S' || direccion == 's') {
    nuevaPosicion.fila++;
  } else if (direccion == 'D' || direccion == 'd') {
    nuevaPosicion.columna++;
  }

  // Verificar si la nueva posición es válida y realizar el movimiento
  if (nuevaPosicion.fila >= 0 && nuevaPosicion.fila < FILAS &&
      nuevaPosicion.columna >= 0 && nuevaPosicion.columna < COLUMNAS) {
    char nuevaCasilla = laberinto[nuevaPosicion.fila][nuevaPosicion.columna];
    if (nuevaCasilla == 'X') {
      score += 7; // Aumentar el puntaje en 7 si se pasa por una X
      laberinto[posicionPacman.fila][posicionPacman.columna] = ' ';
      laberinto[nuevaPosicion.fila][nuevaPosicion.columna] = 'p';
      posicionPacman = nuevaPosicion;
    }
    if (nuevaCasilla == 'x') {
      score += 3; // Aumentar el puntaje en 3 si se pasa por una x
      laberinto[posicionPacman.fila][posicionPacman.columna] = ' ';
      laberinto[nuevaPosicion.fila][nuevaPosicion.columna] = 'p';
      posicionPacman = nuevaPosicion;
    }
    if (nuevaCasilla == ' ' || nuevaCasilla == 'p') {
      // Actualizar la posición de Pacman
      laberinto[posicionPacman.fila][posicionPacman.columna] = ' ';
      laberinto[nuevaPosicion.fila][nuevaPosicion.columna] = 'p';
      posicionPacman = nuevaPosicion;
    }
  }
}

// Función para actualizar la posición de Pacman en el laberinto
void actualizarPunto() {
  laberinto[posicionPacman.fila][posicionPacman.columna] = 'p';
}

// Función para mostrar el puntaje en pantalla
void mostrarPuntaje() {
  printf("\nSCORE: %d\n", score);
}

int main() {
  FILE *archivo;
  archivo = fopen("laberinto.txt", "r");
  if (archivo == NULL) {
    printf("No se pudo abrir el archivo laberinto.txt\n");
    return 1;
  } 
  // Saltar la primera línea del archivo
  char temp[COLUMNAS + 2];
  fgets(temp, COLUMNAS + 2, archivo);

  // Leer el laberinto del archivo
  for (int i = 0; i < FILAS; i++) {
    fgets(laberinto[i], COLUMNAS + 2,
          archivo); // +2 para incluir el salto de línea y el carácter nulo
                    // terminador
  }

  imprimirLaberinto();

  fclose(archivo);

  

  // Encontrar la posición inicial de Pacman (p)
  for (int i = 0; i < FILAS; i++) {
    for (int j = 0; j < COLUMNAS; j++) {
      if (laberinto[i][j] == 'p') {
        posicionPacman.fila = i;
        posicionPacman.columna = j;
        break;
      }
    }
  }

  // Juego principal
  char direccion;
  bool juegoTerminado = false;

  while (!juegoTerminado) {
    system("clear"); // Limpiar la consola (funciona en sistemas linux)

    actualizarPunto(); // Actualizar la posición de Pacman en el laberinto

    imprimirLaberinto();
    mostrarPuntaje();
    printf("Ingresa una dirección (W/A/S/D) para mover a Pacman: ");
    scanf(" %c", &direccion);

    moverPacman(direccion);

    // Verificar si se han recogido todos los puntos
    bool puntosRecogidos = true;
    int i = 0;
    int j = 0;

    while (i < FILAS && puntosRecogidos) {
      while (j < COLUMNAS && puntosRecogidos) {
      if (laberinto[i][j] == 'x') {
        puntosRecogidos = false;
         }    
      j++;
     }
    i++;
   j = 0;
}


    // Mostrar la posición actual de Pacman
    printf("Posición actual de Pacman: (%d, %d)\n", posicionPacman.fila,
           posicionPacman.columna);
  }
  

  return 0;
}
