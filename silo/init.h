#ifndef NETWORK_INIT_H
#define NETWORK_INIT_H

#include "tuple.h"

void silo_init(){
  epoch = 1;
  thread_local_epochs = aligned_alloc(CACHE_LINE_SIZE , THREAD_NUM * sizeof(uint64_t));
  table = aligned_alloc(CACHE_LINE_SIZE, TUPLE_NUM * sizeof(struct tuple));

  for(size_t i = 0; i < TUPLE_NUM ; i++){
    struct tuple *t = &table[i];
    t->tid_word.epoch = 1;
    t->tid_word.latest = true;
    t->tid_word.lock = false;
    t->value = 0;
  }
}


#endif //NETWORK_INIT_H
