#ifndef NETWORK_INIT_H
#define NETWORK_INIT_H

#include "tuple.h"

void silo_init(){
  epoch = 0;
  global_epoch = 0;
  thread_local_epochs = aligned_alloc(CACHE_LINE_SIZE ,224 * sizeof(uint64_t));
  table = aligned_alloc(CACHE_LINE_SIZE, 1000 * sizeof(struct tuple));
}


#endif //NETWORK_INIT_H
