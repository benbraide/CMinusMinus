#include "../memory/memory_reference.h"

#include "function_declaration_base.h"

cminus::declaration::function_base::~function_base() = default;

std::shared_ptr<cminus::memory::reference> cminus::declaration::function_base::call(const std::vector<std::shared_ptr<memory::reference>> &args) const{
	if (get_score(args) <= type::object::get_score_value(type::object::score_result_type::nil))
		throw exception::function_not_found();
	return call_(args);
}
