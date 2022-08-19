#include <assert.h>
#include <memory.h>
#include <pthread.h>
#include <stdio.h>
#include "silo/tx.h"
#include "silo/init.h"
#include "lib/error_functions.h"

atomic_bool wait = true;

void *worker(void *thread_id){
  printf("Hi, from %zu\n", (size_t)thread_id);

  while (wait){;}

  struct tx t;
  tx_init(&t);
  for(size_t i = 0; i < 10000; i++){
    value v = tx_read(&t, 1);
    v++;
    tx_write(&t, 1, v);
  }

  pthread_exit(NULL);
}

int main(){
  silo_init();

  pthread_t threads[4];
  for(size_t i = 0; i < 4; i++){
    if(pthread_create(&threads[i], NULL, worker, (void*)i))
      errExit("pthread_create");
  }

  atomic_store(&wait, false);

  pthread_exit(NULL);
  return 0;
}