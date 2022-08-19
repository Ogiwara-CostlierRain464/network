#include "tuple.h"
_Atomic epoch_t epoch;
alignas(CACHE_LINE_SIZE) uint64_t global_epoch;
alignas(CACHE_LINE_SIZE) uint64_t *thread_local_epochs;
alignas(CACHE_LINE_SIZE) struct tuple *table;