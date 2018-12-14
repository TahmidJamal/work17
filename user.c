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
  struct semid_ds *buf;
  unsigned short *array;  
  struct seminfo *__buf;  
};

int main() {
  //size of data 
  size_t size = 1024;
  char input[size];

  //create the sembuf for semop()
  struct sembuf sb;
  sb.sem_op = -1;
  sb.sem_num = 0;
  sb.sem_flg = SEM_UNDO;

  //get the semaophore and shared memory ids
  key_t key = ftok(".", 'R');
  int shmid = shmget(key, size, 0);
  int semid = semget(key, 1, 0);

  //data check
  printf("Checking if memory segment is available\n");
  if (!semctl(semid, 0, GETVAL, 1)) { 
    printf("Dang, segment is not available\n");
    return -1;
  }

  semop(semid, &sb, 1); //up

  //create data
  char *data = shmat(shmid, 0, 0);
  if (!data[0]) {
    printf("Nothing has been added yet\n");
  }
  else {
    int fd = open("book", O_RDONLY);
    lseek(fd, -1 * *data, SEEK_END);
    char *buffer = malloc(*data);
    read(fd, buffer, *data);
    printf("Most recent line: \n -> %s \n", buffer);
    close(fd);
    free(buffer);
  }

  printf("What should the next line be?: \n");
  fgets(input, sizeof(input), stdin);
  int fd = open("book", O_APPEND | O_WRONLY);
  *data = strlen(input);
  write(fd, input, *data);
  close(fd);
  shmdt(data);
  
  sb.sem_op = 1;
  semop(semid, &sb, 1);
  return 0;
}
