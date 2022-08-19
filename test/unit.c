#include <assert.h>
#include <memory.h>
#include "../silo/tx.h"
#include "../silo/init.h"

int main(){
  silo_init();
  struct tx t;
  tx_init(&t);

  value v = tx_read(&t, 1);
  v++;
  tx_write(&t, 1, v);
  enum result r = tx_commit(&t);
  assert(r == commited);
  assert(table[1].value == v);

  return 0;
}