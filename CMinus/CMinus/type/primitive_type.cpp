#include "primitive_type.h"

cminus::type::primitive::~primitive() = default;

bool cminus::type::primitive::is_exact(const object &target) const{
	return (dynamic_cast<const primitive *>(&target) != nullptr && object::is_exact(target));
}

int cminus::type::primitive::get_score(const object &target) const{
	return get_score_value(is_exact(target) ? score_result_type::exact : score_result_type::nil);
}

bool cminus::type::primitive::is(query_type type) const{
	return (type == query_type::primitive || object::is(type));
}
