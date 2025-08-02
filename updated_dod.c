#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <semaphore.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>


void ChildProcess(int shmPTR[], sem_t *mutex); 
void ParentProcess(int shmPTR[], sem_t *mutex); 

int main(int  argc, char *argv[]){
  int fd, *shmPTR; 
  sem_t *mutex; 

  //open file for shared counter
  fd = open("log.txt", O_RDWR| O_CREAT, S_IRWXU);
  ftruncate(fd, sizeof(int)); 
  shmPTR = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);  
  close(fd); 

  shmPTR[0] = 0; //BankAccount


  //initialize semaphore
  if((mutex = sem_open("dodsemaphore", O_CREAT, 0644, 1)) == SEM_FAILED){
    perror("semaphore initializion"); 
    exit(1); 
  }

  if(fork() == 0){
    srand(time(0) + getpid());
    while(1){
      ChildProcess(shmPTR, mutex); 
    }
  }else{
    srand(time(0) + getpid());
    while(1){
      ParentProcess(shmPTR, mutex); 
    }
  }

  sem_close(mutex); 
  sem_unlink("dodsemaphore"); 
  munmap(shmPTR, sizeof(int)); 
  return 0; 
}

void ChildProcess(int shmPTR[], sem_t *mutex){
  sleep(rand() % 3); 
  sem_wait(mutex); 

  printf("Poor Student: Attempting to Check Balance\n"); 
  int account = shmPTR[0];

  if(rand() % 2 == 0){
    int withdrawal = rand() % 51;
    printf("Poor Student needs $%d\n", withdrawal); 

    if(withdrawal <= account){
      account -= withdrawal; 
      printf("Student: Withdraws: $%d, Balance = $%d\n", withdrawal, account); 
    }else{
      printf("Student: Not enough cash\n"); 
    }
    shmPTR[0] = account; 
  }else{
    printf("Poor Student: Last Checking Balance = $%d\n", account); 
  }
      
    sem_post(mutex); 
}

void ParentProcess(int shmPTR[], sem_t *mutex){
  sleep(rand() % 3);
  sem_wait(mutex); 
  printf("Dear Old Dad: Attempting to Check Balance\n"); 
  int account = shmPTR[0]; 

  if(rand() % 2 == 0){
    if(account <= 100){
      int deposit = rand() % 101; 
      if(deposit % 2 == 0){
        account += deposit; 
        printf("Dear Old Dad: Deposits: $%d, Balance = $%d\n", deposit, account); 
      } else{
        printf("Dear Old Dad: Doesn't have any money to give\n"); 
      }
      shmPTR[0] = account; 
    }else{
      printf("Dear Old Dad: Thinks Student has enough Cash ($%d)\n", account); 
    }
  }else{
    printf("Dear Old Dad: Last Checking Balance: = $%d\n", account); 
  }
  sem_post(mutex); 
}