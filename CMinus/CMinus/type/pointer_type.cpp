#include "../storage/global_storage.h"

#include "class_type.h"
#include "pointer_type.h"

cminus::type::pointer_primitive::pointer_primitive(std::shared_ptr<object> base_type)
	: primitive(""), base_type_(base_type){
	if (base_type_ == nullptr)
		name_ = "NullptrType";
	else
		name_ = (base_type_->get_name() + "*");
}

cminus::type::pointer_primitive::~pointer_primitive() = default;

std::string cminus::type::pointer_primitive::get_qname() const{
	return ((base_type_ == nullptr) ? "NullptrType" : (base_type_->get_qname() + "*"));
}

void cminus::type::pointer_primitive::print_value(io::writer &writer, std::shared_ptr<memory::reference> data) const{
	if (base_type_ != nullptr){
		auto address = data->read_scalar<std::size_t>();
		auto hex_address = runtime::to_hex_string<std::size_t>::get(address, 8);

		writer.write_buffer(hex_address.data(), hex_address.size());
		writer.write_scalar('{');

		if (address != 0u){
			if (auto base_value = std::make_shared<memory::reference>(address, base_type_); base_value != nullptr)
				base_type_->print_value(writer, base_value);
			else
				throw memory::exception::allocation_failure();
		}
		else
			writer.write_buffer("nullptr");

		writer.write_scalar('}');
	}
	else
		writer.write_buffer("nullptr");
}

std::size_t cminus::type::pointer_primitive::get_size() const{
	return sizeof(void *);
}

bool cminus::type::pointer_primitive::is_exact(const object &target) const{
	if (primitive::is_exact(target))
		return true;

	auto pointer_target = dynamic_cast<const pointer_primitive *>(target.remove_proxy());
	return (pointer_target != nullptr && (base_type_ == nullptr) == (pointer_target->base_type_ == nullptr) && (base_type_ == nullptr || base_type_->is_exact(*pointer_target->base_type_)));
}

std::shared_ptr<cminus::memory::reference> cminus::type::pointer_primitive::cast(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type, cast_type type) const{
	if (is_inferred() || target_type->is_inferred())
		return nullptr;

	auto pointer_target_type = target_type->as<pointer_primitive>();
	if (pointer_target_type == nullptr){
		if (type != cast_type::reinterpret)
			return nullptr;

		if (auto number_target_type = target_type->as<number_primitive>(); number_target_type != nullptr){
			switch (number_target_type->get_state()){
			case number_primitive::state_type::small_integer:
				return std::make_shared<memory::scalar_reference<__int16>>(
					storage::get_cached_type<__int16>::type(),
					static_cast<__int16>(data->read_scalar<std::size_t>())
				);
			case number_primitive::state_type::integer:
				return std::make_shared<memory::scalar_reference<__int32>>(
					storage::get_cached_type<__int32>::type(),
					static_cast<__int32>(data->read_scalar<std::size_t>())
				);
			case number_primitive::state_type::big_integer:
				return std::make_shared<memory::scalar_reference<__int64>>(
					storage::get_cached_type<__int64>::type(),
					static_cast<__int64>(data->read_scalar<std::size_t>())
				);
			case number_primitive::state_type::unsigned_small_integer:
				return std::make_shared<memory::scalar_reference<unsigned __int16>>(
					storage::get_cached_type<unsigned __int16>::type(),
					static_cast<unsigned __int16>(data->read_scalar<std::size_t>())
				);
			case number_primitive::state_type::unsigned_integer:
				return std::make_shared<memory::scalar_reference<unsigned __int32>>(
					storage::get_cached_type<unsigned __int32>::type(),
					static_cast<unsigned __int32>(data->read_scalar<std::size_t>())
				);
			case number_primitive::state_type::unsigned_big_integer:
				return std::make_shared<memory::scalar_reference<unsigned __int64>>(
					storage::get_cached_type<unsigned __int64>::type(),
					static_cast<unsigned __int64>(data->read_scalar<std::size_t>())
				);
			default:
				break;
			}
		}

		return nullptr;
	}

	auto data_is_lval = data->is_lvalue();
	if (is_non_const_ref_cast(type))//Any *&
		return ((is_exact(*pointer_target_type) && data_is_lval && !data->is_const()) ? data : nullptr);

	if (type != cast_type::reinterpret && !is_static_cast(type))
		return nullptr;

	if (pointer_target_type->base_type_ == nullptr)
		return nullptr;

	auto is_nullptr = (base_type_ == nullptr);
	if (!is_nullptr)
		return copy_data(data, target_type);

	if (base_type_->is_const() && !pointer_target_type->base_type_->is_const())
		return nullptr;

	if (base_type_->is_exact(*pointer_target_type->base_type_))
		return ((!data_is_lval || (type != cast_type::static_ && type != cast_type::reinterpret)) ? data : copy_data(data, target_type));

	if (type == cast_type::reinterpret || is_nullptr || pointer_target_type->base_type_->is<void_primitive>())
		return copy_data(data, target_type);

	if (auto class_base = base_type_->as<class_>(), class_target_base = pointer_target_type->base_type_->as<class_>(); class_base != nullptr && class_target_base != nullptr){
		if (auto base_offset = class_base->compute_base_offset(*class_target_base); base_offset != static_cast<std::size_t>(-1)){
			return std::make_shared<memory::scalar_reference<std::size_t>>(
				target_type->remove_const_ref(target_type),
				(data->read_scalar<std::size_t>() + base_offset)
			);
		}
	}

	return nullptr;
}

cminus::evaluator::object::id_type cminus::type::pointer_primitive::get_evaluator_id() const{
	return evaluator::object::id_type::pointer;
}

std::shared_ptr<cminus::type::object> cminus::type::pointer_primitive::get_inferred(std::shared_ptr<object> target) const{
	if (base_type_ == nullptr)
		return nullptr;

	auto pointer_target = target->as<pointer_primitive>();
	if (pointer_target == nullptr || pointer_target->base_type_ == nullptr)
		return nullptr;

	if (auto inferred_base_type = base_type_->get_inferred(pointer_target->base_type_); inferred_base_type != nullptr)
		return std::make_shared<pointer_primitive>(inferred_base_type);

	return nullptr;
}

bool cminus::type::pointer_primitive::can_be_inferred_from(const object &target) const{
	if (base_type_ == nullptr)
		return false;

	auto pointer_target = target.as<pointer_primitive>();
	return (pointer_target != nullptr && pointer_target->base_type_ != nullptr && base_type_->can_be_inferred_from(*pointer_target->base_type_));
}

bool cminus::type::pointer_primitive::is_inferred() const{
	return (base_type_ != nullptr && base_type_->is_inferred());
}

std::shared_ptr<cminus::type::object> cminus::type::pointer_primitive::get_base_type() const{
	return base_type_;
}

bool cminus::type::pointer_primitive::is_nullptr() const{
	return (base_type_ == nullptr);
}

int cminus::type::pointer_primitive::get_score_(const object &target, bool is_lval, bool is_const) const{
	auto pointer_target = dynamic_cast<const pointer_primitive *>(target.remove_const_ref());
	if (pointer_target == nullptr)
		return get_score_value(score_result_type::nil);

	if (pointer_target->base_type_ == nullptr)//Any * ==> Nullptr
		return get_score_value(score_result_type::nil);

	if (base_type_ == nullptr)//Nullptr ==> Any *
		get_score_value(score_result_type::assignable);

	auto is_const_base = base_type_->is_const();
	auto is_const_target_base = pointer_target->base_type_->is_const();

	if (is_const_base && !is_const_target_base)//Const Any * ==> Any *
		return get_score_value(score_result_type::nil);

	if (pointer_target->base_type_->is<void_primitive>())//Any * ==> Void *
		return get_score_value(score_result_type::assignable, ((is_const_base == is_const_target_base) ? 0 : -1));

	if (base_type_->is_exact(*pointer_target->base_type_))
		return get_score_value(score_result_type::exact, ((is_const_base == is_const_target_base) ? 0 : -1));

	if (pointer_target->base_type_->can_be_inferred_from(*base_type_))
		return get_score_value(score_result_type::inferable, ((is_const_base == is_const_target_base) ? 0 : -1));

	if (auto class_base = base_type_->as<class_>(), class_target_base = pointer_target->base_type_->as<class_>(); class_base != nullptr && class_target_base != nullptr){
		if (class_base->is_base_type(*class_target_base, true))
			return get_score_value(score_result_type::assignable, ((is_const_base == is_const_target_base) ? 0 : -1));
	}

	return get_score_value(score_result_type::nil);
}
