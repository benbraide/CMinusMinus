#include "../memory/memory_reference.h"

#include "callable_declaration.h"

cminus::declaration::callable::~callable() = default;

std::shared_ptr<cminus::memory::reference> cminus::declaration::callable::call(std::shared_ptr<memory::reference> context, const std::vector<std::shared_ptr<memory::reference>> &args) const{
	return call(context, args, static_cast<std::size_t>(-1));
}

std::shared_ptr<cminus::memory::reference> cminus::declaration::callable::call(std::shared_ptr<memory::reference> context, const std::vector<std::shared_ptr<memory::reference>> &args, std::size_t required_size) const{
	if (get_score(context, args, required_size) <= type::object::get_score_value(type::object::score_result_type::nil))
		throw exception::function_not_found();
	return call_(context, args, required_size);
}
