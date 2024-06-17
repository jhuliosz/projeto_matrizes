#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

int n, T;
int **A, **B, **C, **D, **E;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
double soma_tempo = 0.0, mult_tempo = 0.0, red_tempo = 0.0, total_tempo = 0.0;

// Função para alocar matriz dinamicamente
int **alocar_matriz(int n) {
    int **matriz = malloc(n * sizeof(int *));
    if (matriz == NULL) handle_error("malloc");
    matriz[0] = malloc(n * n * sizeof(int));
    if (matriz[0] == NULL) handle_error("malloc");
    for (int i = 1; i < n; i++) {
        matriz[i] = matriz[0] + i * n;
    }
    return matriz;
}

// Função para ler matriz de arquivo
void ler_matriz(const char *filename, int **matriz) {
    FILE *file = fopen(filename, "r");
    if (!file) handle_error("fopen");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            fscanf(file, "%d", &matriz[i][j]);
        }
    }
    fclose(file);
}

// Função para escrever matriz em arquivo
void escrever_matriz(const char *filename, int **matriz) {
    FILE *file = fopen(filename, "w");
    if (!file) handle_error("fopen");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            fprintf(file, "%d ", matriz[i][j]);
        }
        fprintf(file, "\n");
    }
    fclose(file);
}

// Função para somar duas matrizes (executada por thread)
void *somar_matrizes(void *arg) {
    clock_t start = clock();
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            D[i][j] = A[i][j] + B[i][j];
        }
    }
    clock_t end = clock();
    soma_tempo = (double)(end - start) / CLOCKS_PER_SEC;
    pthread_exit(NULL);
}

// Função para multiplicar duas matrizes (executada por thread)
void *multiplicar_matrizes(void *arg) {
    clock_t start = clock();
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            E[i][j] = 0;
            for (int k = 0; k < n; k++) {
                E[i][j] += C[i][k] * D[k][j];
            }
        }
    }
    clock_t end = clock();
    mult_tempo = (double)(end - start) / CLOCKS_PER_SEC;
    pthread_exit(NULL);
}

// Função para reduzir matriz (executada por thread)
void *reduzir_matriz(void *arg) {
    clock_t start = clock();
    int soma = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            soma += E[i][j];
        }
    }
    clock_t end = clock();
    red_tempo = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Redução: %d\n", soma);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 8) {
        fprintf(stderr, "Uso: %s T n arqA.dat arqB.dat arqC.dat arqD.dat arqE.dat\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    T = atoi(argv[1]);
    n = atoi(argv[2]);

    A = alocar_matriz(n);
    B = alocar_matriz(n);
    C = alocar_matriz(n);
    D = alocar_matriz(n);
    E = alocar_matriz(n);

    ler_matriz(argv[3], A);
    ler_matriz(argv[4], B);
    ler_matriz(argv[5], C);

    pthread_t threads[T];

    clock_t total_start = clock();

    // Passo 1: Soma das matrizes A e B
    pthread_create(&threads[0], NULL, somar_matrizes, NULL);
    pthread_join(threads[0], NULL);

    escrever_matriz(argv[6], D);
    
    // Passo 2: Multiplicação das matrizes D e C
    pthread_create(&threads[0], NULL, multiplicar_matrizes, NULL);
    pthread_join(threads[0], NULL);

    escrever_matriz(argv[7], E);
    
    // Passo 3: Redução da matriz E
    pthread_create(&threads[0], NULL, reduzir_matriz, NULL);
    pthread_join(threads[0], NULL);

    clock_t total_end = clock();
    total_tempo = (double)(total_end - total_start) / CLOCKS_PER_SEC;

    printf("Tempo soma: %f segundos.\n", soma_tempo);
    printf("Tempo multiplicação: %f segundos.\n", mult_tempo);
    printf("Tempo redução: %f segundos.\n", red_tempo);
    printf("Tempo total: %f segundos.\n", total_tempo);

    free(A[0]); free(A);
    free(B[0]); free(B);
    free(C[0]); free(C);
    free(D[0]); free(D);
    free(E[0]); free(E);

    return 0;
}

