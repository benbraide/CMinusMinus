#include "variadic_type.h"

cminus::type::variadic::variadic(std::shared_ptr<object> base_type)
	: object(base_type->get_name() + ("..."), nullptr), base_type_(base_type){}

cminus::type::variadic::~variadic() = default;

std::size_t cminus::type::variadic::get_size() const{
	return 0u;
}

bool cminus::type::variadic::is_exact(const object &target) const{
	auto variadic_target = dynamic_cast<variadic *>(target.get_non_proxy());
	return (variadic_target != nullptr && base_type_->is_exact(*variadic_target->base_type_));
}

int cminus::type::variadic::get_score(const object &target) const{
	return get_score_value(score_result_type::nil);
}

std::shared_ptr<cminus::memory::reference> cminus::type::variadic::cast(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type, cast_type type) const{
	return nullptr;
}

bool cminus::type::variadic::is(query_type type, const object *arg) const{
	return (type == query_type::variadic);
}

std::shared_ptr<cminus::type::object> cminus::type::variadic::get_base_type() const{
	return base_type_;
}
