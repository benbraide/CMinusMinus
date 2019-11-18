#include "../node/literal_node.h"
#include "../storage/global_storage.h"
#include "../declaration/variable_declaration.h"
#include "../declaration/string_function_definitions.h"

#include "string_type.h"

cminus::type::string::string()
	: class_("String", nullptr){
	add_variable_(std::make_shared<declaration::variable>(
		"size_",																//Name
		runtime::object::global_storage->get_size_type(),						//Type
		attribute::collection::list_type{},										//Attributes
		declaration::flags::private_,											//Flags
		std::shared_ptr<node::object>()											//Initialization
	), 0u);

	add_variable_(std::make_shared<declaration::variable>(
		"data_",																//Name
		runtime::object::global_storage->get_char_pointer_type(false),			//Type
		attribute::collection::list_type{},										//Attributes
		declaration::flags::private_,											//Flags
		std::shared_ptr<node::object>()											//Initialization
	), 0u);

	add_callable_(std::make_shared<declaration::string::destructor_def>(*this), 0u);
	add_callable_(std::make_shared<declaration::string::default_constructor_def>(*this), 0u);

	add_callable_(std::make_shared<declaration::string::copy_constructor_def>(*this), 0u);
	add_callable_(std::make_shared<declaration::string::sub_copy_constructor_def>(*this), 0u);

	add_callable_(std::make_shared<declaration::string::buffer_constructor_def>(*this), 0u);
	add_callable_(std::make_shared<declaration::string::fill_constructor_def>(*this), 0u);

	add_callable_(std::make_shared<declaration::string::empty_def>(*this), 0u);
	add_callable_(std::make_shared<declaration::string::size_def>(*this), 0u);

	add_callable_(std::make_shared<declaration::string::data_def>(*this, true), 0u);
	add_callable_(std::make_shared<declaration::string::data_def>(*this, false), 0u);

	add_callable_(std::make_shared<declaration::string::begin_def>(*this, true), 0u);
	add_callable_(std::make_shared<declaration::string::begin_def>(*this, false), 0u);

	add_callable_(std::make_shared<declaration::string::end_def>(*this, true), 0u);
	add_callable_(std::make_shared<declaration::string::end_def>(*this, false), 0u);

	add_callable_(std::make_shared<declaration::string::at_def>(*this, true), 0u);
	add_callable_(std::make_shared<declaration::string::at_def>(*this, false), 0u);

	add_callable_(std::make_shared<declaration::string::find_copy_def>(*this, true), 0u);
	add_callable_(std::make_shared<declaration::string::find_copy_def>(*this, false), 0u);

	add_callable_(std::make_shared<declaration::string::find_sub_copy_def>(*this, true), 0u);
	add_callable_(std::make_shared<declaration::string::find_sub_copy_def>(*this, false), 0u);

	add_callable_(std::make_shared<declaration::string::find_buffer_def>(*this, true), 0u);
	add_callable_(std::make_shared<declaration::string::find_buffer_def>(*this, false), 0u);

	add_callable_(std::make_shared<declaration::string::find_sized_buffer_def>(*this, true), 0u);
	add_callable_(std::make_shared<declaration::string::find_sized_buffer_def>(*this, false), 0u);

	add_callable_(std::make_shared<declaration::string::find_single_def>(*this, true), 0u);
	add_callable_(std::make_shared<declaration::string::find_single_def>(*this, false), 0u);

	add_callable_(std::make_shared<declaration::string::get_sub_def>(*this), 0u);
	add_callable_(std::make_shared<declaration::string::resize_def>(*this), 0u);

	add_callable_(std::make_shared<declaration::string::erase_def>(*this), 0u);
	add_callable_(std::make_shared<declaration::string::clear_def>(*this), 0u);

	add_callable_(std::make_shared<declaration::string::assign_copy_def>(*this, true), 0u);
	add_callable_(std::make_shared<declaration::string::assign_copy_def>(*this, false), 0u);

	add_callable_(std::make_shared<declaration::string::assign_sub_copy_def>(*this, true), 0u);
	add_callable_(std::make_shared<declaration::string::assign_sub_copy_def>(*this, false), 0u);

	add_callable_(std::make_shared<declaration::string::assign_buffer_def>(*this, true), 0u);
	add_callable_(std::make_shared<declaration::string::assign_buffer_def>(*this, false), 0u);

	add_callable_(std::make_shared<declaration::string::assign_fill_def>(*this, true), 0u);
	add_callable_(std::make_shared<declaration::string::assign_fill_def>(*this, false), 0u);

	add_callable_(std::make_shared<declaration::string::insert_copy_def>(*this), 0u);
	add_callable_(std::make_shared<declaration::string::insert_sub_copy_def>(*this), 0u);

	add_callable_(std::make_shared<declaration::string::insert_buffer_def>(*this), 0u);
	add_callable_(std::make_shared<declaration::string::insert_fill_def>(*this), 0u);

	add_callable_(std::make_shared<declaration::string::index_operator_def>(*this, true), 0u);
	add_callable_(std::make_shared<declaration::string::index_operator_def>(*this, false), 0u);

	add_callable_(std::make_shared<declaration::string::operator_def<declaration::string::assignment_operator>>(*this), 0u);
	add_callable_(std::make_shared<declaration::string::operator_def<declaration::string::compound_assignment_operator>>(*this), 0u);
	add_callable_(std::make_shared<declaration::string::operator_def<declaration::string::concatenation_operator>>(*this), 0u);
	add_callable_(std::make_shared<declaration::string::operator_def<declaration::string::spaceship_operator>>(*this), 0u);

	add_callable_(std::make_shared<declaration::string::operator_def<declaration::string::relational_operator>>(*this, operators::id::less), 0u);
	add_callable_(std::make_shared<declaration::string::operator_def<declaration::string::relational_operator>>(*this, operators::id::less_or_equal), 0u);
	add_callable_(std::make_shared<declaration::string::operator_def<declaration::string::relational_operator>>(*this, operators::id::equal), 0u);
	add_callable_(std::make_shared<declaration::string::operator_def<declaration::string::relational_operator>>(*this, operators::id::not_equal), 0u);
	add_callable_(std::make_shared<declaration::string::operator_def<declaration::string::relational_operator>>(*this, operators::id::greater_or_equal), 0u);
	add_callable_(std::make_shared<declaration::string::operator_def<declaration::string::relational_operator>>(*this, operators::id::greater), 0u);

	compile_();
}

cminus::type::string::~string(){
	destroy_entries_();
}

void cminus::type::string::print_value(io::writer &writer, std::shared_ptr<memory::reference> data) const{
	auto str_data = runtime::object::global_storage->get_string_value(data);
	writer.write_buffer(str_data.data(), str_data.size());
}

std::shared_ptr<cminus::memory::reference> cminus::type::string::cast(std::shared_ptr<memory::reference> data, std::shared_ptr<type_base> target_type, cast_type type) const{
	if (!is_static_rval_cast(type))
		return nullptr;

	auto number_target_type = target_type->as<number_primitive>();
	if (number_target_type != nullptr){
		switch (number_target_type->get_state()){
		case number_primitive::state_type::small_integer:
			return std::make_shared<memory::scalar_reference<__int16>>(
				runtime::object::global_storage->get_cached_type(storage::global::cached_type::small_integer),
				static_cast<__int16>(std::atoi(runtime::object::global_storage->get_string_value(data).data()))
			);
		case number_primitive::state_type::integer:
			return std::make_shared<memory::scalar_reference<__int32>>(
				runtime::object::global_storage->get_cached_type(storage::global::cached_type::integer),
				std::atoi(runtime::object::global_storage->get_string_value(data).data())
			);
		case number_primitive::state_type::big_integer:
			return std::make_shared<memory::scalar_reference<__int64>>(
				runtime::object::global_storage->get_cached_type(storage::global::cached_type::big_integer),
				std::strtoll(runtime::object::global_storage->get_string_value(data).data(), nullptr, 10)
			);
		case number_primitive::state_type::unsigned_small_integer:
			return std::make_shared<memory::scalar_reference<unsigned __int16>>(
				runtime::object::global_storage->get_cached_type(storage::global::cached_type::unsigned_small_integer),
				static_cast<unsigned __int16>(std::strtoul(runtime::object::global_storage->get_string_value(data).data(), nullptr, 10))
			);
		case number_primitive::state_type::unsigned_integer:
			return std::make_shared<memory::scalar_reference<unsigned __int32>>(
				runtime::object::global_storage->get_cached_type(storage::global::cached_type::unsigned_integer),
				static_cast<unsigned __int32>(std::strtoul(runtime::object::global_storage->get_string_value(data).data(), nullptr, 10))
			);
		case number_primitive::state_type::unsigned_big_integer:
			return std::make_shared<memory::scalar_reference<unsigned __int64>>(
				runtime::object::global_storage->get_cached_type(storage::global::cached_type::unsigned_big_integer),
				std::strtoull(runtime::object::global_storage->get_string_value(data).data(), nullptr, 10)
			);
		case number_primitive::state_type::small_float:
			return std::make_shared<memory::scalar_reference<float>>(
				runtime::object::global_storage->get_cached_type(storage::global::cached_type::small_float),
				std::strtof(runtime::object::global_storage->get_string_value(data).data(), nullptr)
			);
		case number_primitive::state_type::float_:
			return std::make_shared<memory::scalar_reference<double>>(
				runtime::object::global_storage->get_cached_type(storage::global::cached_type::float_),
				std::strtod(runtime::object::global_storage->get_string_value(data).data(), nullptr)
			);
		case number_primitive::state_type::big_float:
			return std::make_shared<memory::scalar_reference<long double>>(
				runtime::object::global_storage->get_cached_type(storage::global::cached_type::big_float),
				std::strtold(runtime::object::global_storage->get_string_value(data).data(), nullptr)
			);
		default:
			break;
		}

		return nullptr;
	}

	return class_::cast(data, target_type, type);
}

std::shared_ptr<cminus::evaluator::object> cminus::type::string::get_evaluator() const{
	return runtime::object::global_storage->get_evaluator(evaluator::object::id_type::string);
}

bool cminus::type::string::is_constructible_from(const type_base &target_type, bool is_lval, bool is_const) const{
	return (target_type.is<number_primitive>() || class_::is_constructible_from(target_type, is_lval, is_const));
}

int cminus::type::string::get_score_(const type_base &target, bool is_lval, bool is_const) const{
	return get_score_value(target.is<number_primitive>() ? score_result_type::assignable : score_result_type::nil);
}
