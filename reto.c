#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

typedef struct {
    int acceso; // 0 = turno del padre, 1 = turno del hijo
    int suma;   // valor compartido
} MemoriaCompartida;

int main() {
    // Crear clave única para la memoria compartida
    key_t key = ftok("memsicomp", 65);
    int shmid = shmget(key, sizeof(MemoriaCompartida), 0666 | IPC_CREAT);
    MemoriaCompartida *mem = (MemoriaCompartida*) shmat(shmid, (void*)0, 0);

    // Inicialización
    mem->acceso = 0; // empieza el padre
    mem->suma = 0;   // valor inicial

    pid_t pid = fork();

    if (pid < 0) {
        perror("Error en fork");
        exit(1);
    }

    if (pid == 0) {
        // Proceso hijo
        for (int i = 0; i < 5; i++) {
            while (mem->acceso != 1) {
                // espera su turno
                usleep(1000);
            }
            mem->suma++; // incrementa el valor
            printf("Hijo imprime: %d\n", mem->suma);
            mem->acceso = 0; // cede el turno al padre
        }
        shmdt(mem);
    } else {
        // Proceso padre
        for (int i = 0; i < 5; i++) {
            while (mem->acceso != 0) {
                // espera su turno
                usleep(1000);
            }
            printf("Padre imprime: %d\n", mem->suma);
            mem->acceso = 1; // cede el turno al hijo
        }
        wait(NULL);
        shmdt(mem);
        shmctl(shmid, IPC_RMID, NULL); // liberar memoria compartida
    }

    return 0;
}