#ifndef TUPLE
#define TUPLE

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdalign.h>

typedef int key;
typedef int value;

struct tid_word{
  union {
    uint64_t body;
    struct {
      bool lock: 1;
      bool latest: 1;
      bool absent: 1;
      uint64_t tid: 29;
      uint64_t epoch: 32;
    };
  };
};

struct tuple{
  alignas(64) struct tid_word tid_word;
  value value;
};



#endif