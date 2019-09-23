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

cminus::type::in_memory_variadic::in_memory_variadic(const variadic &target, std::list<std::shared_ptr<memory::reference>> &args)
	: variadic(target.get_base_type()), args_(&args){
	if (!args_->empty()){
		types_.reserve(args_->size());
		for (std::size_t index = 0u; index < args_->size(); ++index)
			types_.push_back(base_type_->convert(conversion_type::clone, base_type_));
	}
}

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
	return (base_type_->get_size() * get_count());
}

std::size_t cminus::type::in_memory_variadic::get_memory_size() const{
	return (base_type_->get_memory_size() * get_count());
}

std::size_t cminus::type::in_memory_variadic::get_count() const{
	return types_.size();
}

std::shared_ptr<cminus::memory::reference> cminus::type::in_memory_variadic::get_indexed(std::shared_ptr<memory::reference> data, std::size_t index) const{
	if (types_.size() <= index)
		return nullptr;

	if (types_[index]->is(query_type::ref)){
		return std::make_shared<memory::indirect_reference>(
			(data->get_address() + (types_[index]->get_memory_size() * index)),
			types_[index],
			data->get_attributes().get_list(),
			nullptr
		);
	}

	return std::make_shared<memory::reference>(
		(data->get_address() + (types_[index]->get_memory_size() * index)),
		types_[index],
		data->get_attributes().get_list(),
		nullptr
	);
}

void cminus::type::in_memory_variadic::construct_(std::shared_ptr<memory::reference> target, const std::list<std::shared_ptr<memory::reference>> &args) const{
	std::size_t index = 0u;
	for (auto arg : *args_){//Construct entries
		if (auto entry = get_indexed(target, index); entry != nullptr)
			entry->get_type()->construct(entry->get_type(), entry, arg);
		else
			break;
	}
}

cminus::type::expansion_variadic::expansion_variadic(std::shared_ptr<in_memory_variadic> base_type)
	: object("", nullptr), base_type_(base_type){}

cminus::type::expansion_variadic::~expansion_variadic() = default;

void cminus::type::expansion_variadic::extend_argument_list(std::shared_ptr<memory::reference> data, std::list<std::shared_ptr<memory::reference>> &list) const{
	auto count = base_type_->get_count();
	for (std::size_t index = 0u; index < count; ++index)
		list.push_back(base_type_->get_indexed(data, index));
}

std::size_t cminus::type::expansion_variadic::get_size() const{
	return base_type_->get_size();
}

std::size_t cminus::type::expansion_variadic::get_memory_size() const{
	return base_type_->get_memory_size();
}

bool cminus::type::expansion_variadic::is_exact(const object &target) const{
	return false;
}

int cminus::type::expansion_variadic::get_score(const object &target) const{
	return get_score_value(score_result_type::nil);
}

std::shared_ptr<cminus::memory::reference> cminus::type::expansion_variadic::cast(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type, cast_type type) const{
	return nullptr;
}

std::shared_ptr<cminus::type::in_memory_variadic> cminus::type::expansion_variadic::get_base_type() const{
	return base_type_;
}
