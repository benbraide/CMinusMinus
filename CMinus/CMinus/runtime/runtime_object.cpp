#include "../storage/global_storage.h"

#include "runtime_object.h"

cminus::memory::object *cminus::runtime::object::memory_object = nullptr;

thread_local bool cminus::runtime::object::is_system = false;

cminus::storage::global *cminus::runtime::object::global_storage = nullptr;

thread_local cminus::storage::object *cminus::runtime::object::current_storage = nullptr;
