#include <assert.h>
#include <memory.h>
#include "silo/tx.h"
#include "silo/init.h"

int main(){
  silo_init();
  struct tx t;
  tx_init(&t);

  value v = tx_read(&t, 1);
  v++;
  tx_write(&t, 1, v);
  tx_commit(&t);

  return 0;
}