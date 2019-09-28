#include "special_function_declaration.h"

#include "callable_declaration_group.h"

cminus::declaration::callable_group::~callable_group() = default;

void cminus::declaration::callable_group::add(std::shared_ptr<callable> entry){
	if (entry->get_id() == get_id())
		add_(entry);
	else
		throw exception::function_expected();
}

std::shared_ptr<cminus::memory::reference> cminus::declaration::callable_group::call(const std::list<std::shared_ptr<memory::reference>> &args) const{
	std::size_t count = 0;
	auto entry = find(args, &count);

	if (count == 0u || entry == nullptr)
		throw exception::function_not_found();

	if (1u < count)
		throw exception::ambiguous_function_call();

	if (auto parent = entry->get_parent(); parent != nullptr && !parent->is_accessible(entry->get_flags()))
		throw storage::exception::inaccessible_entry();

	return entry->call_(args);
}
