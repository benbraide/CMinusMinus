#include "variadic_type.h"

cminus::type::variadic::variadic(std::shared_ptr<object> base_type)
	: object(base_type->get_name() + ("..."), nullptr), base_type_(base_type){}

cminus::type::variadic::~variadic() = default;

std::size_t cminus::type::variadic::get_size() const{
	return 0u;
}

void cminus::type::variadic::print_value(io::writer &writer, std::shared_ptr<memory::reference> data) const{
	throw runtime::exception::not_supported();
}

bool cminus::type::variadic::is_exact(const object &target) const{
	if (object::is_exact(target))
		return true;

	auto variadic_target = target.as<variadic>(false);
	return (variadic_target != nullptr && base_type_->is_exact(*variadic_target->base_type_));
}

int cminus::type::variadic::get_score(const object &target, bool is_lval, bool is_const) const{
	return get_score_value(score_result_type::nil);
}

std::shared_ptr<cminus::memory::reference> cminus::type::variadic::cast(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type, cast_type type) const{
	return nullptr;
}

std::shared_ptr<cminus::type::object> cminus::type::variadic::get_base_type() const{
	return base_type_;
}

cminus::type::in_memory_variadic::in_memory_variadic(std::shared_ptr<object> base_type, std::size_t count)
	: variadic(base_type), count_(count){}

cminus::type::in_memory_variadic::~in_memory_variadic() = default;

void cminus::type::in_memory_variadic::destruct(std::shared_ptr<memory::reference> target) const{
	auto count = get_count();
	for (std::size_t index = 0u; index < count; ++index){
		if (auto entry = get_indexed(target, index); entry != nullptr)
			entry->get_type()->destruct(entry);
		else
			break;
	}
}

std::size_t cminus::type::in_memory_variadic::get_size() const{
	return (base_type_->get_size() * count_);
}

std::size_t cminus::type::in_memory_variadic::get_memory_size() const{
	return (base_type_->get_memory_size() * count_);
}

std::size_t cminus::type::in_memory_variadic::get_count() const{
	return count_;
}

std::shared_ptr<cminus::memory::reference> cminus::type::in_memory_variadic::get_indexed(std::shared_ptr<memory::reference> data, std::size_t index) const{
	if (count_ <= index)
		return nullptr;

	/*if (base_type_->is(query_type::ref)){
		return std::make_shared<memory::indirect_reference>(
			(data->get_address() + (base_type_->get_memory_size() * index)),
			base_type_,
			nullptr
		);
	}*/

	return std::make_shared<memory::reference>(
		(data->get_address() + (base_type_->get_memory_size() * index)),
		base_type_
	);
}

void cminus::type::in_memory_variadic::construct_(std::shared_ptr<memory::reference> target, const std::vector<std::shared_ptr<memory::reference>> &args) const{
	std::size_t index = 0u;
	for (auto arg : args){//Construct entries
		if (auto entry = get_indexed(target, index); entry != nullptr)
			entry->get_type()->construct(entry, arg);
		else
			break;
	}
}

cminus::type::expansion_variadic::~expansion_variadic() = default;

void cminus::type::expansion_variadic::extend_argument_list(std::shared_ptr<memory::reference> data, std::vector<std::shared_ptr<memory::reference>> &list) const{
	if (count_ == 0u)
		return;

	list.reserve(list.size() + count_);
	for (std::size_t index = 0u; index < count_; ++index)
		list.push_back(get_indexed(data, index));
}

void cminus::type::expansion_variadic::print_value(io::writer &writer, std::shared_ptr<memory::reference> data) const{
	writer.write_scalar('[');

	auto is_first = true;
	for (std::size_t index = 0u; index < count_; ++index){
		if (is_first)
			is_first = false;
		else
			writer.write_buffer(", ");

		base_type_->print_value(writer, get_indexed(data, index));
	}

	writer.write_scalar(']');
}
