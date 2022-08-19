#include <assert.h>
#include <memory.h>
#include <pthread.h>
#include <stdio.h>
#include "silo/tx.h"
#include "silo/init.h"
#include "lib/error_functions.h"

atomic_bool wait = true;

void *worker(void *thread_id){
  while (wait){;}

  ssize_t num_commited = 0;
  for(size_t i = 0; i < 10000; i++){
    struct tx t;
    tx_init(&t);
    value v = tx_read(&t, 1);
    v++;
    tx_write(&t, 1, v);
    enum result r = tx_commit(&t);
    if(r == commited)
      num_commited++;
  }

  printf("Num commited: %zd\n", num_commited);

  pthread_exit(NULL);
}

int main(){
  silo_init();

  pthread_t threads[THREAD_NUM];
  for(size_t i = 0; i < THREAD_NUM; i++){
    if(pthread_create(&threads[i], NULL, worker, (void*)i))
      errExit("pthread_create");
  }

  atomic_store(&wait, false);

  for(size_t i = 0; i < THREAD_NUM; i++){
    void *ret_val;
    pthread_join(threads[i], &ret_val);
    if(ret_val == PTHREAD_CANCELED){
      printf("Thread %zu has been canceled.\n", i);
    }else{
      printf("Thread %zu return code: %d\n", i, (int)ret_val);
    }
  }
  printf("Final value of key 1: %d\n", table[1].value);
  pthread_exit(NULL);
}