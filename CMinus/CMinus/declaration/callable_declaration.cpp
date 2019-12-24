#include "../node/node_object.h"
#include "../memory/memory_reference.h"

#include "variable_declaration.h"
#include "callable_declaration.h"

cminus::declaration::callable::~callable() = default;

void cminus::declaration::callable::add_parameter(const std::string &name, std::shared_ptr<type::object> type, std::shared_ptr<node::object> init){
	add_parameter(std::make_shared<declaration::variable>(
		name,														//Name
		type,														//Type
		attribute::collection::list_type{},							//Attributes
		declaration::flags::nil,									//Flags
		init														//Initialization
	));
}

std::shared_ptr<cminus::memory::reference> cminus::declaration::callable::call(std::shared_ptr<memory::reference> context, const std::vector<std::shared_ptr<memory::reference>> &args) const{
	return call(context, args, static_cast<std::size_t>(-1));
}

std::shared_ptr<cminus::memory::reference> cminus::declaration::callable::call(std::shared_ptr<memory::reference> context, const std::vector<std::shared_ptr<memory::reference>> &args, std::size_t required_size) const{
	if (get_score(context, args, required_size) <= type::object::get_score_value(type::object::score_result_type::nil))
		throw exception::function_not_found();
	return call_(context, args, required_size);
}
