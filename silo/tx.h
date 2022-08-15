#ifndef TX
#define TX

#include "tuple.h"

struct read_operation{
  key key;
  value value;
  struct tid_word tid_word;
};

struct write_operation{
  key key;
  value value;
  struct tuple *ptr;
};

struct tx{
  struct read_operation reads[10];
  ssize_t num_read;
  struct write_operation writes[10];
  ssize_t num_write;

  struct tid_word
    max_read_tid,
    max_write_tid,
    most_recently_chosen_tid;
};

value tx_read(struct tx*, key);
void tx_write(struct tx*, key, value);
void tx_commit(struct tx*);

#endif