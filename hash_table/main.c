#include <stdio.h>
#include <stdlib.h>
#include "hash_table.h"
#include <pthread.h>
#include <unistd.h>

pthread_t pid1, pid2;

void worker1(void *arg) {
    printf("\nworker1 pid : %d \n", pid1);
    struct hash* table = hash_create();

    table->set("1-first", "worker1");
    table->set("1-second", "worker1 test");
    table->set("1-three", "worker1 sad code");
    table->set("1-four", "worker1,let's go");

    table->print();
    hash_destroy();
}

void worker2(void *arg) {
    printf("\nworker2 pid : %d \n", pid2);
    struct hash* table = hash_create();

    table->set("2-first", "worker2");
    table->set("2-second", "worker2 test");

    table->print();
    hash_destroy();
}

int main(int argc, char* argv[]) {
    printf("\nmain: \n");
    struct hash* table = hash_create();

    table->set("first3", "danxi");
    table->set("second3", "test");
    table->set("three3", "sad code");
    table->set("four3", "let's go");
    table->set("four3", "lgo");

    table->print();

    char * t1 = (char *)table->find("first3");
    char * t2 = (char *)table->find("second3");

    printf("found: %s  %s \n", t1, t2);
    printf("found: %s \n", (char*)table->find("four3"));

    table->erase("four3");
    table->print();
    printf("found: %s \n", (char*)table->find("four3"));

    hash_destroy();

    pthread_create(&pid1, NULL, worker1, NULL);
    pthread_create(&pid2, NULL, worker2, NULL);

    pthread_join(pid1, NULL);
    pthread_join(pid2, NULL);

    return 0;
}
