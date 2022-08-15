#include "tx.h"
#include "../lib/tlpi_hdr.h"
#include <stdatomic.h>
#include <memory.h>

value tx_read(struct tx* tx,key key){
  struct tid_word before, after;
  value data;

  struct tuple *t = &table[key];

  for(;;){
    do{
      before.body = atomic_load(&t->tid_word.body);
    } while(before.lock);

    data = t->value;

    after.body = atomic_load(&t->tid_word.body);

    if(tid_eq(before, after)){
      break;
    }else{
      continue;
    }
  }
  tx->reads[tx->num_read] = (struct read_operation){
    .key = key,
    .value = data,
    .tid_word = after
  };
  tx->num_read++;
  return data;
}

void tx_write(struct tx* tx, key key, value val){
  struct tuple *t = &table[key];
  tx->writes[tx->num_write] = (struct write_operation){
    .key = key,
    .value = val,
    .ptr = t
  };
  tx->num_write++;
}

void tx_lock_write_set(struct tx* tx);
bool tx_exist_in_write_set(struct tx* tx, struct tuple* t);
void tx_unlock_write_set(struct tx* tx);

void tx_commit(struct tx* tx){
  // TODO sort write order
  tx_lock_write_set(tx);

  atomic_thread_fence(memory_order_acquire);
  _Atomic epoch_t e = atomic_load(&epoch);
  atomic_thread_fence(memory_order_release);

  for(size_t i = 0; i < tx->num_read; i++){
    struct read_operation *op = &tx->reads[i];
    struct tuple *t = &table[op->key];

    struct tid_word when_read = op->tid_word;
    struct tid_word now;
    now.body = atomic_load(&t->tid_word.body);

    if(tid_neq(now, when_read)
    || !now.latest
    || (now.lock && !tx_exist_in_write_set(tx, t))){
      tx_unlock_write_set(tx);
      return;
    }

    tx->max_read_tid.body = max(tx->max_read_tid.body, now.body);
  }

  struct tid_word a, b, c;
  a.body = max(tx->max_read_tid.body, tx->max_write_tid.body);
  a.body++;

  b.body = tx->most_recently_chosen_tid.body;
  b.body++;

  c.epoch = e;

  struct tid_word max;
  max.body = max(max(a.body, b.body), c.body);
  max.lock = false;
  max.latest = true;
  tx->most_recently_chosen_tid = max;

  for(size_t i = 0; i < tx->num_write; i++){
    atomic_store(&tx->writes[i].ptr->value, tx->writes[i].value);
    atomic_store(&tx->writes[i].ptr->tid_word.body, max.body);
  }
}

void tx_lock_write_set(struct tx* tx){
  struct tid_word expected, desired;
  for(size_t i = 0; i < tx->num_write; i++){
    key k = tx->writes[i].key;
    expected.body = atomic_load(&table[k].tid_word.body);

    for(;;){
      if(expected.lock){
        expected.body = atomic_load(&table[k].tid_word.body);
      }else{
        desired.body = expected.body;
        desired.lock = true;
        if(atomic_compare_exchange_weak(&table[k].tid_word.body, &expected.body, desired.body)){
          break;
        }
      }
    }

    tx->max_write_tid.body = max(tx->max_write_tid.body, expected.body);
  }
}

void tx_unlock_write_set(struct tx* tx){
  struct tid_word expected, desired;

  for(size_t i = 0; i < tx->num_write; i++){
    expected.body = atomic_load(&tx->writes[i].ptr->tid_word.body);
    desired.body = expected.body;
    desired.lock = false;
    atomic_store(&tx->writes[i].ptr->tid_word.body, desired.body);
  }
}

bool tx_exist_in_write_set(struct tx* tx, struct tuple* t){
  for(size_t i = 0; i < tx->num_write; i++){
    if(tx->writes[i].ptr == t)
      return true;
  }
  return false;
}