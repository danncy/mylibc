#include <stdio.h>
#include <stdlib.h>
#include "hash_table.h"
#include <pthread.h>
#include <unistd.h>

pthread_t pid1, pid2;

void worker1(void *arg) {
    printf("\nworker1 pid : %d \n", pid1);
    hash_create();

    hash_set("1-first", "worker1");
    hash_set("1-second", "worker1 test");
    hash_set("1-three", "worker1 sad code");
    hash_set("1-four", "worker1,let's go");

    hash_iter_print();
    hash_destroy();
}

void worker2(void *arg) {
    printf("\nworker2 pid : %d \n", pid2);
    hash_create();

    hash_set("2-first", "worker2");
    hash_set("2-second", "worker2 test");

    hash_iter_print();
    hash_destroy();
}

int main(int argc, char* argv[]) {
    printf("\nmain: \n");
    hash_create();

    hash_set("first3", "danxi");
    hash_set("second3", "test");
    hash_set("three3", "sad code");
    hash_set("four3", "let's go");
    hash_set("four3", "lgo");

    hash_iter_print();

    char * t1 = (char *)hash_lookup("first3");
    char * t2 = (char *)hash_lookup("second3");

    printf("found: %s  %s \n", t1, t2);
    printf("found: %s \n", (char*)hash_lookup("four3"));

    hash_delete("four3");
    hash_iter_print();
    printf("found: %s \n", (char*)hash_lookup("four3"));

    hash_destroy();



    //pthread_create(&pid1, NULL, worker1, NULL);
    //pthread_create(&pid2, NULL, worker2, NULL);

    //pthread_join(pid1, NULL);
    //pthread_join(pid2, NULL);

    return 0;
}
