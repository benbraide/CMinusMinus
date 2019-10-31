#include "special_function_declaration.h"

#include "callable_declaration_group.h"

cminus::declaration::callable_group::~callable_group() = default;

void cminus::declaration::callable_group::add(std::shared_ptr<callable> entry){
	if (entry->get_id() == get_id())
		add_(entry);
	else
		throw exception::function_expected();
}

std::shared_ptr<cminus::declaration::callable> cminus::declaration::callable_group::find(std::shared_ptr<memory::reference> context, const std::vector<std::shared_ptr<memory::reference>> &args) const{
	return find(context, args, static_cast<std::size_t>(-1));
}

std::shared_ptr<cminus::declaration::callable> cminus::declaration::callable_group::find(std::shared_ptr<memory::reference> context, const std::vector<callable::arg_info> &args) const{
	return find(context, args, static_cast<std::size_t>(-1));
}

std::shared_ptr<cminus::declaration::callable> cminus::declaration::callable_group::find_by_args(const std::vector<std::shared_ptr<memory::reference>> &args) const{
	return find_by_args(args, static_cast<std::size_t>(-1));
}

std::shared_ptr<cminus::declaration::callable> cminus::declaration::callable_group::find_by_args(const std::vector<callable::arg_info> &args) const{
	return find_by_args(args, static_cast<std::size_t>(-1));
}

std::shared_ptr<cminus::memory::reference> cminus::declaration::callable_group::call(std::shared_ptr<memory::reference> context, const std::vector<std::shared_ptr<memory::reference>> &args) const{
	return call(context, args, static_cast<std::size_t>(-1));
}

std::shared_ptr<cminus::memory::reference> cminus::declaration::callable_group::call(std::shared_ptr<memory::reference> context, const std::vector<std::shared_ptr<memory::reference>> &args, std::size_t required_size) const{
	auto entry = find(context, args, required_size);
	if (entry == nullptr)
		throw exception::function_not_found();

	if (auto parent = entry->get_parent(); parent != nullptr && !parent->is_accessible(entry->get_flags()))
		throw storage::exception::inaccessible_entry();

	return entry->call_(context, args, required_size);
}
