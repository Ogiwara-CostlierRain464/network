#ifndef NETWORK_INIT_H
#define NETWORK_INIT_H

#include "tuple.h"

void silo_init(){
  epoch = 0;
  global_epoch = 0;
  thread_local_epochs = malloc(224 * sizeof(uint64_t));
  table = malloc(1000 * sizeof(struct tuple));
}


#endif //NETWORK_INIT_H
