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

void cminus::type::array_primitive::destruct(std::shared_ptr<memory::reference> target) const{
	auto start_address = target->get_address();
	auto base_memory_size = base_type_->get_memory_size();

	auto is_ref_base = base_type_->is_ref();
	std::shared_ptr<memory::reference> base_value;

	for (auto count = count_; 0u < count; --count){
		if (is_ref_base)
			base_value = std::make_shared<memory::indirect_reference>((start_address + (base_memory_size * count)), base_type_);
		else
			base_value = std::make_shared<memory::reference>((start_address + (base_memory_size * count)), base_type_);

		if (base_value != nullptr)
			base_type_->destruct(base_value);
		else
			throw memory::exception::allocation_failure();
	}
}

void cminus::type::array_primitive::print_value(io::writer &writer, std::shared_ptr<memory::reference> data) const{
	writer.write_scalar('[');

	auto start_address = data->get_address();
	auto base_memory_size = base_type_->get_memory_size();

	for (std::size_t count = 0u; count < count_; ++count){
		if (count != 0u)
			writer.write_scalar(',');

		auto base_value = std::make_shared<memory::reference>((start_address + (base_memory_size * count)), base_type_);
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

std::shared_ptr<cminus::memory::reference> cminus::type::array_primitive::begin(std::shared_ptr<memory::reference> data) const{
	return std::make_shared<memory::scalar_reference<std::size_t>>(
		runtime::object::global_storage->get_pointer_type(base_type_, false),
		data->get_address()
	);
}

std::shared_ptr<cminus::memory::reference> cminus::type::array_primitive::rbegin(std::shared_ptr<memory::reference> data) const{
	return std::make_shared<memory::scalar_reference<std::size_t>>(
		runtime::object::global_storage->get_reversed_pointer_type(base_type_, false),
		((data->get_address() + (base_type_->get_memory_size() * count_)) - base_type_->get_memory_size())
	);
}

std::shared_ptr<cminus::memory::reference> cminus::type::array_primitive::end(std::shared_ptr<memory::reference> data) const{
	return std::make_shared<memory::scalar_reference<std::size_t>>(
		runtime::object::global_storage->get_pointer_type(base_type_, false),
		(data->get_address() + (base_type_->get_memory_size() * count_))
	);
}

std::shared_ptr<cminus::memory::reference> cminus::type::array_primitive::rend(std::shared_ptr<memory::reference> data) const{
	return std::make_shared<memory::scalar_reference<std::size_t>>(
		runtime::object::global_storage->get_reversed_pointer_type(base_type_, false),
		(data->get_address() - base_type_->get_memory_size())
	);
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

bool cminus::type::array_primitive::is_forward_traversable() const{
	return true;
}

bool cminus::type::array_primitive::is_reverse_traversable() const{
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

void cminus::type::array_primitive::construct_(std::shared_ptr<memory::reference> target, const std::vector<std::shared_ptr<memory::reference>> &args) const{
	if (args.size() != 1u)
		throw runtime::exception::construction_failure();

	auto arg_array_type = args[0]->get_type()->as<array_primitive>();
	if (arg_array_type == nullptr)
		throw runtime::exception::construction_failure();

	auto target_base_memory_size = base_type_->get_memory_size(), arg_base_memory_size = arg_array_type->base_type_->get_memory_size();
	auto target_address = target->get_address(), arg_address = args[0]->get_address();

	auto target_base_is_ref = base_type_->is_ref(), arg_base_is_ref = arg_array_type->base_type_->is_ref();
	std::shared_ptr<memory::reference> target_index, arg_index;

	for (std::size_t index = 0u; index < count_; ++index){
		if (target_base_is_ref)
			target_index = std::make_shared<memory::indirect_reference>((target_address + (target_base_memory_size * index)), base_type_);
		else
			target_index = std::make_shared<memory::reference>((target_address + (target_base_memory_size * index)), base_type_);

		if (target_index == nullptr)
			throw memory::exception::allocation_failure();

		if (index < arg_array_type->count_){
			if (arg_base_is_ref)
				arg_index = std::make_shared<memory::indirect_reference>((arg_address + (arg_base_memory_size * index)), arg_array_type->base_type_);
			else
				arg_index = std::make_shared<memory::reference>((arg_address + (arg_base_memory_size * index)), arg_array_type->base_type_);

			if (arg_index != nullptr)
				base_type_->construct(target_index, arg_index);
			else
				throw memory::exception::allocation_failure();
		}
		else//Default construction
			base_type_->construct(target_index);
	}
}

bool cminus::type::array_primitive::is_exact_(const object &target) const{
	if (target.is<initializer_list_primitive>(false))
		return false;

	auto array_target = target.as<array_primitive>(false);
	return (array_target != nullptr && base_type_->is_exact(*array_target->base_type_));
}

int cminus::type::array_primitive::get_score_(const object &target, bool is_lval, bool is_const) const{
	if (target.is<initializer_list_primitive>())
		return get_score_value(score_result_type::nil);

	auto array_target = target.as<array_primitive>();
	std::shared_ptr<object> target_base_type;

	if (array_target == nullptr){//Try pointer
		if (base_type_->is_ref())
			return get_score_value(score_result_type::nil);

		if (auto pointer_target = target.as<pointer_primitive>(); pointer_target != nullptr){
			if ((target_base_type = pointer_target->get_base_type()) == nullptr)
				return get_score_value(score_result_type::nil);
		}
		else
			return get_score_value(score_result_type::nil);
	}
	else
		target_base_type = array_target->get_base_type();

	auto base_is_const = base_type_->is_const(), target_base_is_const = target_base_type->is_const();
	if ((base_is_const && !target_base_is_const) || !base_type_->remove_const_ref()->is_exact(*target_base_type->remove_const_ref()))
		return get_score_value(score_result_type::nil);

	return get_score_value(((array_target == nullptr) ? score_result_type::assignable : score_result_type::exact), ((base_is_const == target_base_is_const) ? 0 : 1));
}

std::shared_ptr<cminus::memory::reference> cminus::type::array_primitive::cast_(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type, cast_type type) const{
	if ((type != cast_type::static_rval && type != cast_type::static_))
		return nullptr;

	if (get_score_(*target_type, false, false) == get_score_value(score_result_type::nil))
		return nullptr;

	if (target_type->is<pointer_primitive>())
		return std::make_shared<memory::scalar_reference<std::size_t>>(target_type->remove_const_ref(target_type), data->get_address());

	return ((type == cast_type::static_rval) ? data : nullptr);
}

cminus::type::initializer_list_primitive::initializer_list_primitive(std::shared_ptr<object> base_type, std::size_t count)
	: array_primitive(base_type, count){
	name_ = ("initializer_list<" + base_type_->get_name() + ">");
}

cminus::type::initializer_list_primitive::~initializer_list_primitive() = default;

std::string cminus::type::initializer_list_primitive::get_qname() const{
	return ("initializer_list<" + base_type_->get_qname() + ">");
}

std::shared_ptr<cminus::type::object> cminus::type::initializer_list_primitive::get_inferred(std::shared_ptr<object> target) const{
	if (!is_inferred())
		return nullptr;

	auto initializer_list_target = target->as<initializer_list_primitive>();
	if (initializer_list_target == nullptr || initializer_list_target->is_inferred() || !base_type_->is_exact(*initializer_list_target->base_type_))
		return nullptr;

	return std::make_shared<initializer_list_primitive>(initializer_list_target->base_type_, initializer_list_target->count_);
}

bool cminus::type::initializer_list_primitive::is_exact_(const object &target) const{
	auto initializer_list_target = target.as<initializer_list_primitive>(false);
	return (initializer_list_target != nullptr && base_type_->is_exact(*initializer_list_target->base_type_));
}

int cminus::type::initializer_list_primitive::get_score_(const object &target, bool is_lval, bool is_const) const{
	auto array_target = target.as<array_primitive>();
	if (array_target == nullptr)
		return get_score_value(score_result_type::nil);

	auto array_base_type = array_target->get_base_type();
	auto base_is_const = base_type_->is_const(), array_base_is_const = array_base_type->is_const();

	if ((base_is_const && !array_base_is_const) || !base_type_->remove_const_ref()->is_exact(*array_base_type->remove_const_ref()))
		return get_score_value(score_result_type::nil);

	return get_score_value(target.is<initializer_list_primitive>() ? score_result_type::assignable : score_result_type::exact, ((base_is_const == array_base_is_const) ? 0 : 1));
}
