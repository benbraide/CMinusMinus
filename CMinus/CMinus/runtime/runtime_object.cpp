#include "runtime_object.h"

cminus::memory::object *cminus::runtime::object::memory_object = nullptr;

thread_local bool cminus::runtime::object::is_system = false;
