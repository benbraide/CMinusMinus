#include "core_storage.h"

cminus::storage::core::core()
	: compiled("Core", runtime::object::global_storage){
	add_callable_(std::make_shared<declaration::core::heap_alloc>(*this), 0u);
	add_callable_(std::make_shared<declaration::core::heap_dealloc>(*this), 0u);
	add_callable_(std::make_shared<declaration::core::heap_realloc>(*this), 0u);
	add_callable_(std::make_shared<declaration::core::is_heap>(*this), 0u);

	add_callable_(std::make_shared<declaration::core::begin>(*this, true), 0u);
	add_callable_(std::make_shared<declaration::core::begin>(*this, false), 0u);

	add_callable_(std::make_shared<declaration::core::rbegin>(*this, true), 0u);
	add_callable_(std::make_shared<declaration::core::rbegin>(*this, false), 0u);

	add_callable_(std::make_shared<declaration::core::end>(*this, true), 0u);
	add_callable_(std::make_shared<declaration::core::end>(*this, false), 0u);

	add_callable_(std::make_shared<declaration::core::rend>(*this, true), 0u);
	add_callable_(std::make_shared<declaration::core::rend>(*this, false), 0u);

	compile_();
}

cminus::storage::core::~core(){
	destroy_entries_();
}
