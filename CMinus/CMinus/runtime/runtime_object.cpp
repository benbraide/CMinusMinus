#include "../storage/global_storage.h"

#include "runtime_object.h"

cminus::memory::object *cminus::runtime::object::memory_object = nullptr;

thread_local unsigned int cminus::runtime::object::state = runtime::flags::nil;

cminus::storage::global *cminus::runtime::object::global_storage = nullptr;

thread_local cminus::storage::object *cminus::runtime::object::current_storage = nullptr;

thread_local std::shared_ptr<cminus::memory::reference> cminus::runtime::object::current_exception;
