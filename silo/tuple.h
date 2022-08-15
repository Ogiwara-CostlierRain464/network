#ifndef TUPLE
#define TUPLE

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdalign.h>
#include <stdatomic.h>

typedef int key;
typedef int value;

struct tid_word{
  union {
    _Atomic uint64_t body;
    struct {
      bool lock: 1;
      bool latest: 1;
      bool absent: 1;
      uint64_t tid: 29;
      uint64_t epoch: 32;
    };
  };
};

static bool tid_eq(struct tid_word t1, struct tid_word t2){
  return t1.body == t2.body;
}

static bool tid_neq(struct tid_word t1, struct tid_word t2){
  return !tid_eq(t1, t2);
}

struct tuple{
  alignas(64) struct tid_word tid_word;
  value value;
};

typedef uint32_t epoch_t;
_Atomic extern epoch_t epoch;
alignas(64) extern uint64_t global_epoch;
alignas(64) extern uint64_t *thread_local_epochs;
alignas(64) extern struct tuple *table;



#endif