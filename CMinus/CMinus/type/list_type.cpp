#include "../storage/global_storage.h"

#include "modified_type.h"
#include "pointer_type.h"
#include "list_type.h"

cminus::type::array_primitive::array_primitive(std::shared_ptr<object> base_type, std::size_t count)
	: primitive(base_type_->get_name() + "[" + ((count == 0u) ? "" : std::to_string(count)) + "]"), base_type_(base_type), count_(count){}

cminus::type::array_primitive::~array_primitive() = default;

std::string cminus::type::array_primitive::get_qname() const{
	return (base_type_->get_qname() + "[" + ((count_ == 0u) ? "" : std::to_string(count_)) + "]");
}

void cminus::type::array_primitive::print_value(io::writer &writer, std::shared_ptr<memory::reference> data) const{
	writer.write_scalar('[');

	auto is_ref_base = base_type_->is_ref();
	auto start_address = data->get_address();

	std::shared_ptr<memory::reference> base_value;
	for (std::size_t count = 0u; count < count_; ++count){
		if (count != 0u)
			writer.write_scalar(',');

		if (is_ref_base)
			base_value = std::make_shared<memory::indirect_reference>((start_address + (sizeof(void *) * count)), base_type_);
		else
			base_value = std::make_shared<memory::reference>((start_address + (sizeof(void *) * count)), base_type_);

		if (base_value != nullptr)
			base_type_->print_value(writer, base_value);
		else
			throw memory::exception::allocation_failure();
	}

	writer.write_scalar(']');
}

std::size_t cminus::type::array_primitive::get_size() const{
	return (base_type_->get_size() * count_);
}

std::size_t cminus::type::array_primitive::get_memory_size() const{
	return (base_type_->get_memory_size() * count_);
}

bool cminus::type::array_primitive::is_exact(const object &target) const{
	if (primitive::is_exact(target))
		return true;

	auto array_target = target.as<array_primitive>(false);
	return (array_target != nullptr && base_type_->is_exact(*array_target->base_type_));
}

std::shared_ptr<cminus::memory::reference> cminus::type::array_primitive::cast(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type, cast_type type) const{
	return nullptr;
}

cminus::evaluator::object::id_type cminus::type::array_primitive::get_evaluator_id() const{
	return evaluator::object::id_type::array_;
}

std::shared_ptr<cminus::type::object> cminus::type::array_primitive::get_inferred(std::shared_ptr<object> target) const{
	std::size_t count = 0u;
	std::shared_ptr<object> base_type;

	auto array_target = target->as<array_primitive>();
	if (array_target == nullptr){
		if (auto initializer_list_target = target->as<initializer_list_primitive>(); initializer_list_target != nullptr){
			count = initializer_list_target->get_count();
			base_type = initializer_list_target->get_base_type();
		}
	}
	else{
		count = ((count_ == 0u) ? array_target->count_ : count_);
		base_type = array_target->base_type_;
	}

	if (base_type == nullptr || base_type->is_inferred())
		return nullptr;

	if (!base_type_->is_inferred()){
		if (count_ == 0u && base_type_->is_exact(*base_type))
			return std::make_shared<array_primitive>(base_type, count);
		return nullptr;
	}

	if (auto inferred_base_type = base_type_->get_inferred(base_type); inferred_base_type != nullptr){
		if (base_type->is_const() && !inferred_base_type->is_const())
			inferred_base_type = std::make_shared<constant>(inferred_base_type);
		return std::make_shared<array_primitive>(inferred_base_type, count);
	}

	return nullptr;
}

bool cminus::type::array_primitive::can_be_iterated() const{
	return true;
}

bool cminus::type::array_primitive::can_be_inferred_from(const object &target) const{
	auto array_target = target.as<array_primitive>();
	if (array_target == nullptr){
		auto initializer_list_target = target.as<initializer_list_primitive>();
		if (initializer_list_target == nullptr)
			return false;

		if (!base_type_->is_inferred())
			return (count_ == 0u && base_type_->is_exact(*initializer_list_target->get_base_type()));

		return base_type_->can_be_inferred_from(*initializer_list_target->get_base_type());
	}

	if (!base_type_->is_inferred())
		return (count_ == 0u && base_type_->is_exact(*array_target->base_type_));

	return base_type_->can_be_inferred_from(*array_target->base_type_);
}

bool cminus::type::array_primitive::is_inferred() const{
	return (count_ == 0u || base_type_->is_inferred());
}

std::shared_ptr<cminus::type::object> cminus::type::array_primitive::get_base_type() const{
	return base_type_;
}

std::size_t cminus::type::array_primitive::get_count() const{
	return count_;
}

int cminus::type::array_primitive::get_score_(const object &target, bool is_lval, bool is_const) const{
	return 0;
}

cminus::type::initializer_list_primitive::initializer_list_primitive(std::shared_ptr<object> base_type, std::size_t count)
	: primitive("initializer_list<" + base_type_->get_name() + ">"), base_type_(base_type), count_(count){}

cminus::type::initializer_list_primitive::~initializer_list_primitive() = default;

std::string cminus::type::initializer_list_primitive::get_qname() const{
	return ("initializer_list<" + base_type_->get_qname() + ">");
}

void cminus::type::initializer_list_primitive::print_value(io::writer &writer, std::shared_ptr<memory::reference> data) const{
	throw runtime::exception::not_supported();
}

std::size_t cminus::type::initializer_list_primitive::get_size() const{
	return (base_type_->get_size() * count_);
}

std::size_t cminus::type::initializer_list_primitive::get_memory_size() const{
	return (base_type_->get_memory_size() * count_);
}

bool cminus::type::initializer_list_primitive::is_exact(const object &target) const{
	if (primitive::is_exact(target))
		return true;

	auto initializer_list_target = target.as<initializer_list_primitive>(false);
	return (initializer_list_target != nullptr && base_type_->is_exact(*initializer_list_target->base_type_));
}

std::shared_ptr<cminus::memory::reference> cminus::type::initializer_list_primitive::cast(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type, cast_type type) const{
	return nullptr;
}

std::shared_ptr<cminus::type::object> cminus::type::initializer_list_primitive::get_inferred(std::shared_ptr<object> target) const{
	if (!is_inferred())
		return nullptr;

	auto initializer_list_target = target->as<initializer_list_primitive>();
	if (initializer_list_target == nullptr || initializer_list_target->is_inferred() || !base_type_->is_exact(*initializer_list_target->base_type_))
		return nullptr;

	return std::make_shared<initializer_list_primitive>(initializer_list_target->base_type_, initializer_list_target->count_);
}

bool cminus::type::initializer_list_primitive::can_be_inferred_from(const object &target) const{
	if (!is_inferred())
		return false;

	auto initializer_list_target = target.as<initializer_list_primitive>();
	return (initializer_list_target != nullptr && base_type_->is_exact(*initializer_list_target->base_type_));
}

bool cminus::type::initializer_list_primitive::is_inferred() const{
	return (count_ == 0u);
}

std::shared_ptr<cminus::type::object> cminus::type::initializer_list_primitive::get_base_type() const{
	return base_type_;
}

std::size_t cminus::type::initializer_list_primitive::get_count() const{
	return count_;
}

int cminus::type::initializer_list_primitive::get_score_(const object &target, bool is_lval, bool is_const) const{
	return 0;
}
