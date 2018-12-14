#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

//need to create this ourselves
union semun {
  int val;
  struct semid_ds * buf;
  unsigned short * array;
  struct seminfo * __buf;
};

int main(int argc, char ** argv) {
  char comm;
  int shmid;
  int semid;
  int fd;
  int size;
  struct stat st;

  //bad 
  if (argc < 2) {
    printf("Needs command line argument\n");
    return -1;
  }
  //assume our person uses normal people spacing
  if (!strncmp(argv[1], "-c", 2) || !strncmp(argv[1], "-r", 2) || !strncmp(argv[1], "-v", 2)) {
  	comm = argv[1][1];
  }
  else {
    printf("Needs command line argument\n");
    return -1;
  }

  key_t key = ftok(".", 'R');
  if (comm == 'c') {
    shmid = shmget(key, 1024, 0644 | IPC_CREAT);
    semid = semget(key, 1, 0644 | IPC_CREAT | IPC_EXCL);
    semctl(semid, 0, SETVAL, 1);
    fd = open("book", O_CREAT | O_TRUNC, 0644);
    close(fd);
  }
  else if (comm == 'r') {
    shmid = shmget(key, 1024, 0);
    semid = semget(key, 1, 0);
    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, IPC_RMID, 0);
    stat("book", &st);
    size = (int) st.st_size;
    fd = open("book", O_RDONLY);
    char *buffer = malloc(size);
    read(fd, buffer, size);
    close(fd);
    remove("book");
    printf("The story so far: \n%s\n", buffer);
    free(buffer);
  }
  else if (comm == 'v') {
    stat("book", & st);
    size = (int) st.st_size;
    fd = open("book", O_RDONLY);
    char * bufffer = malloc(size);
    read(fd, bufffer, size);
    close(fd);
    printf("The story so far: \n%s\n", bufffer);
    free(bufffer);
  }

  return 0;
}