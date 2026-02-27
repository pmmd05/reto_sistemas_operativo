#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

int main() {
    key_t key = ftok("alternancia", 65);   // clave única para la memoria
    int shmid = shmget(key, sizeof(int), 0666 | IPC_CREAT);
    int *num = (int*) shmat(shmid, (void*)0, 0);

    *num = 0;  // valor inicial

    pid_t pid = fork();

    if (pid < 0) {
        perror("Error en fork");
        exit(1);
    }

    if (pid == 0) {
        // Proceso hijo
        for (int i = 0; i < 5; i++) {
            (*num)++;  // incrementa el valor
            printf("Hijo imprime: %d\n", *num);
            sleep(1);
        }
        shmdt(num);
    } else {
        // Proceso padre
        for (int i = 0; i < 5; i++) {
            printf("Padre imprime: %d\n", *num);
            sleep(1);
        }
        wait(NULL); // espera al hijo
        shmdt(num);
        shmctl(shmid, IPC_RMID, NULL); // libera memoria compartida
    }

    return 0;
}