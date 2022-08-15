#include "tuple.h"
_Atomic epoch_t epoch;
alignas(64) uint64_t global_epoch;
alignas(64) uint64_t *thread_local_epochs;
alignas(64) struct tuple *table;