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
		std::shared_ptr<memory::reference>()									//Initialization
	), 0u);

	add_variable_(std::make_shared<declaration::variable>(
		"data_",																//Name
		runtime::object::global_storage->get_char_pointer_type(false),			//Type
		attribute::collection::list_type{},										//Attributes
		declaration::flags::private_,											//Flags
		std::shared_ptr<memory::reference>()									//Initialization
	), 0u);

	cminus::runtime::object::memory_object;

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

	compile_();
}

cminus::type::string::~string(){
	destroy_entries_();
}

bool cminus::type::string::is_constructible(std::shared_ptr<memory::reference> target) const{
	return target->get_type()->is(query_type::numeric);
}

void cminus::type::string::print_value(io::writer &writer, std::shared_ptr<memory::reference> data) const{
	auto str_data = runtime::object::global_storage->get_string_value(data);
	writer.write_buffer(str_data.data(), str_data.size());
}

int cminus::type::string::get_score(const type_base &target) const{
	return (target.is(query_type::numeric) ? get_score_value(score_result_type::assignable) : class_::get_score(target));
}

std::shared_ptr<cminus::memory::reference> cminus::type::string::cast(std::shared_ptr<memory::reference> data, std::shared_ptr<type_base> target_type, cast_type type) const{
	if (type != cast_type::static_ && type != cast_type::rval_static)
		return nullptr;

	auto number_target_type = dynamic_cast<const number_primitive *>(target_type->convert(conversion_type::remove_ref_const, target_type)->get_non_proxy());
	if (number_target_type != nullptr){
		if (target_type->is(query_type::ref) && !target_type->is(query_type::const_))
			return nullptr;//Same type required

		switch (number_target_type->get_state()){
		case number_primitive::state_type::integer:
			return std::make_shared<memory::scalar_reference<__int32>>(
				runtime::object::global_storage->get_cached_type(storage::global::cached_type::integer),
				std::atoi(runtime::object::global_storage->get_string_value(data).data())
			);
		case number_primitive::state_type::long_integer:
			return std::make_shared<memory::scalar_reference<__int64>>(
				runtime::object::global_storage->get_cached_type(storage::global::cached_type::long_integer),
				std::strtoll(runtime::object::global_storage->get_string_value(data).data(), nullptr, 10)
			);
		case number_primitive::state_type::unsigned_integer:
			return std::make_shared<memory::scalar_reference<unsigned __int32>>(
				runtime::object::global_storage->get_cached_type(storage::global::cached_type::unsigned_integer),
				static_cast<unsigned __int32>(std::strtoul(runtime::object::global_storage->get_string_value(data).data(), nullptr, 10))
			);
		case number_primitive::state_type::unsigned_long_integer:
			return std::make_shared<memory::scalar_reference<unsigned __int64>>(
				runtime::object::global_storage->get_cached_type(storage::global::cached_type::unsigned_long_integer),
				std::strtoull(runtime::object::global_storage->get_string_value(data).data(), nullptr, 10)
			);
		case number_primitive::state_type::real:
			return std::make_shared<memory::scalar_reference<float>>(
				runtime::object::global_storage->get_cached_type(storage::global::cached_type::real),
				std::strtof(runtime::object::global_storage->get_string_value(data).data(), nullptr)
			);
		case number_primitive::state_type::long_real:
			return std::make_shared<memory::scalar_reference<long double>>(
				runtime::object::global_storage->get_cached_type(storage::global::cached_type::long_real),
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

bool cminus::type::string::is(query_type type, const type_base *arg) const{
	return (type == query_type::string || type == query_type::primitive || class_::is(type, arg));
}
