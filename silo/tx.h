#ifndef TX
#define TX

#include "tuple.h"

struct read_operation{
  key key;
  struct tuple* ptr;
  value value;
  struct tid_word tid_word;
};

struct write_pperation{
  key key;
  struct Tuple* ptr;
};

struct Tx{
  struct read_operation reads[100];
  ssize_t num_read
};

value tx_read(key);
void tx_write(key, value);
void tx_commit();

#endif