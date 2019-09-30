#include "../memory/memory_reference.h"

#include "callable_declaration.h"

cminus::declaration::callable::~callable() = default;

std::shared_ptr<cminus::memory::reference> cminus::declaration::callable::call(std::shared_ptr<memory::reference> context, const std::list<std::shared_ptr<memory::reference>> &args) const{
	if (get_score(context, args) <= type::object::get_score_value(type::object::score_result_type::nil))
		throw exception::function_not_found();
	return call_(context, args);
}
