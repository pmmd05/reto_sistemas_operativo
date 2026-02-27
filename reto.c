#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

int main() {
    // Crear clave única para la memoria compartida
    key_t key = ftok("memsicomp", 65);
    int shmid = shmget(key, sizeof(int), 0666 | IPC_CREAT);
    int *num = (int*) shmat(shmid, (void*)0, 0);

    *num = 0;  // valor inicial en memoria compartida

    pid_t pid = fork();

    if (pid < 0) {
        perror("Error en fork");
        exit(1);
    }

    if (pid == 0) {
        // Proceso hijo
        for (int i = 0; i < 5; i++) {
            (*num)++;  // incrementa el valor compartido
            printf("Hijo imprime: %d\n", *num);
            sleep(1);
        }
        shmdt(num); // desconectar memoria compartida
    } else {
        // Proceso padre
        for (int i = 0; i < 5; i++) {
            printf("Padre imprime: %d\n", *num);
            sleep(1);
        }
        wait(NULL); // esperar al hijo
        shmdt(num); // desconectar memoria compartida
        shmctl(shmid, IPC_RMID, NULL); // liberar memoria compartida
    }

    return 0;
}