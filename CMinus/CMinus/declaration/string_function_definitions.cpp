#include "../type/string_type.h"
#include "../type/primitive_type.h"

#include "string_function_definitions.h"

void cminus::declaration::string::helper::retrieve_info(essential_info &info, std::shared_ptr<memory::reference> context){
	if (context != nullptr){
		runtime::value_guard guard(runtime::object::allow_access, true);
		auto string_type = runtime::object::global_storage->get_raw_string_type();

		info.data = string_type->find("data_", context, false);
		info.size = string_type->find("size_", context, false);
	}
	else{//Use current context
		info.data = runtime::object::current_storage->find("data_", true);
		info.size = runtime::object::current_storage->find("size_", true);
	}

	info.data_address = info.data->read_scalar<std::size_t>();
	info.size_value = info.size->read_scalar<std::size_t>();
}

void cminus::declaration::string::helper::retrieve_info(member_info &info, std::shared_ptr<memory::reference> context){
	if (context != nullptr){
		runtime::value_guard guard(runtime::object::allow_access, true);
		auto string_type = runtime::object::global_storage->get_raw_string_type();

		info.data = string_type->find("data_", context, false);
		info.size = string_type->find("size_", context, false);
	}
	else{//Use current context
		info.data = runtime::object::current_storage->find("data_", true);
		info.size = runtime::object::current_storage->find("size_", true);
	}
}

void cminus::declaration::string::helper::retrieve_info(address_info &info, std::shared_ptr<memory::reference> context){
	member_info minfo{};
	retrieve_info(minfo, context);

	info.data = minfo.data->read_scalar<std::size_t>();	//Data address
	info.size = minfo.size->get_address();				//Size address
}

void cminus::declaration::string::helper::retrieve_info(data_address_size_value_info &info, std::shared_ptr<memory::reference> context){
	member_info minfo{};
	retrieve_info(minfo, context);

	info.data = minfo.data->read_scalar<std::size_t>();	//Data address
	info.size = minfo.size->read_scalar<std::size_t>();	//Size value
}

void cminus::declaration::string::helper::retrieve_info(value_info &info, std::shared_ptr<memory::reference> context){
	data_address_size_value_info ainfo{};
	retrieve_info(ainfo, context);

	info.data = reinterpret_cast<char *>(runtime::object::memory_object->get_block(ainfo.data)->get_data());//Data value
	info.size = ainfo.size;//Size value
}

std::shared_ptr<cminus::memory::reference> cminus::declaration::string::helper::retrieve_data(std::shared_ptr<memory::reference> context){
	if (context == nullptr)
		return runtime::object::current_storage->find("data_", true);
	return runtime::object::global_storage->get_raw_string_type()->find("data_", context, false);
}

std::shared_ptr<cminus::memory::reference> cminus::declaration::string::helper::retrieve_size(std::shared_ptr<memory::reference> context){
	if (context == nullptr)
		return runtime::object::current_storage->find("size_", true);
	return runtime::object::global_storage->get_raw_string_type()->find("size_", context, false);
}

std::size_t cminus::declaration::string::helper::allocate_block(std::size_t buffer_size, allocation_type how, std::size_t split_index, std::shared_ptr<memory::reference> context){
	essential_info info{};
	retrieve_info(info, context);

	if ((how == allocation_type::nil) && buffer_size == info.size_value && info.data_address != 0u)
		return info.data_address;

	auto computed_size = buffer_size;
	switch (how){
	case allocation_type::extend:
		computed_size += info.size_value;
		break;
	case allocation_type::shrink:
		if (info.size_value < computed_size)
			computed_size -= info.size_value;
		else
			computed_size = 0u;
		break;
	default:
		break;
	}

	auto new_data_block = runtime::object::memory_object->allocate_block(computed_size + 1u);
	if (new_data_block == nullptr)
		throw memory::exception::allocation_failure();

	auto new_data_address = new_data_block->get_address();
	if (new_data_address == 0u)
		throw memory::exception::allocation_failure();

	runtime::object::memory_object->write_scalar((new_data_address + computed_size), '\0');//Null terminate
	if (how == allocation_type::extend){//Copy previous data
		if (split_index < info.size_value)//Copy bytes after split index
			runtime::object::memory_object->write((new_data_address + split_index + buffer_size), (info.data_address + split_index), (info.size_value - split_index));

		if (0u < split_index)//Copy bytes before split index
			runtime::object::memory_object->write(new_data_address, info.data_address, split_index);
	}
	else if (how == allocation_type::shrink){//Copy applicable bytes
		if (split_index < info.size_value)//Copy bytes after split index
			runtime::object::memory_object->write((new_data_address + split_index), (info.data_address + split_index + buffer_size), (computed_size - split_index));

		if (0u < split_index)//Copy bytes before split index
			runtime::object::memory_object->write(new_data_address, info.data_address, split_index);
	}

	info.data->write_scalar(new_data_address);//Copy new address
	info.size->write_scalar(computed_size);//Update size

	if (info.data_address != 0u)//Free previous block
		runtime::object::memory_object->deallocate_block(info.data_address);

	return new_data_address;
}

void cminus::declaration::string::helper::assign(const char *buffer, std::size_t buffer_size, bool fill, std::shared_ptr<memory::reference> context){
	auto data_address = allocate_block(buffer_size, allocation_type::nil, 0u, context);
	if (buffer != nullptr && fill)//Fill byte
		runtime::object::memory_object->set(data_address, static_cast<std::byte>(*buffer), buffer_size);
	else if (buffer != nullptr)//Copy characters
		runtime::object::memory_object->write_buffer(data_address, buffer, buffer_size);
}

void cminus::declaration::string::helper::insert(const char *buffer, std::size_t buffer_size, std::size_t offset, bool fill, std::shared_ptr<memory::reference> context){
	auto data_address = allocate_block(buffer_size, allocation_type::extend, offset, context);
	if (buffer != nullptr && fill)//Fill byte
		runtime::object::memory_object->set((data_address + offset), static_cast<std::byte>(*buffer), buffer_size);
	else if (buffer != nullptr)//Copy characters
		runtime::object::memory_object->write_buffer((data_address + offset), buffer, buffer_size);
}

void cminus::declaration::string::helper::erase(std::size_t buffer_size, std::size_t offset, std::shared_ptr<memory::reference> context){
	allocate_block(buffer_size, allocation_type::shrink, offset, context);
}

std::size_t cminus::declaration::string::helper::find_buffer(const char *buffer, std::size_t buffer_size, std::size_t offset, std::shared_ptr<memory::reference> context){
	value_info info{};
	retrieve_info(info, context);

	if (auto result = std::string_view(info.data, info.size).find(buffer, offset, buffer_size); result != std::string_view::npos)
		return result;

	return type::get_max_non_nan<unsigned __int64>::value();
}

char *cminus::declaration::string::helper::read_data(const std::string &name, std::shared_ptr<memory::reference> context){
	return reinterpret_cast<char *>(runtime::object::memory_object->get_block(helper::read_value<std::size_t>(name, context))->get_data());
}

cminus::declaration::string::default_constructor_def::~default_constructor_def() = default;

void cminus::declaration::string::default_constructor_def::evaluate_body_() const{
	helper::assign(nullptr, 0u, false, nullptr);
}

cminus::declaration::string::copy_constructor_def::~copy_constructor_def() = default;

void cminus::declaration::string::copy_constructor_def::evaluate_body_() const{
	helper::value_info info{};
	helper::retrieve_info(info, runtime::object::current_storage->find("other", true));
	helper::assign(info.data, info.size, false, nullptr);
}

cminus::declaration::string::sub_copy_constructor_def::~sub_copy_constructor_def() = default;

void cminus::declaration::string::sub_copy_constructor_def::evaluate_body_() const{
	helper::value_info info{};
	helper::retrieve_info(info, runtime::object::current_storage->find("other", true));

	auto position_value = helper::read_value<std::size_t>("position", nullptr);
	if (info.size <= position_value)
		throw runtime::exception::out_of_range();

	helper::assign((info.data + position_value), std::min((info.size - position_value), helper::read_value<std::size_t>("size", nullptr)), false, nullptr);
}

cminus::declaration::string::buffer_constructor_def::~buffer_constructor_def() = default;

void cminus::declaration::string::buffer_constructor_def::evaluate_body_() const{
	auto data = helper::read_data("data", nullptr);
	helper::assign(data, ((data == nullptr) ? 0u : std::min(helper::read_value<std::size_t>("size", nullptr), std::strlen(data))), false, nullptr);
}

cminus::declaration::string::fill_constructor_def::~fill_constructor_def() = default;

void cminus::declaration::string::fill_constructor_def::evaluate_body_() const{
	auto fill = helper::read_value<char>("fill", nullptr);
	helper::assign(&fill, helper::read_value<std::size_t>("size", nullptr), true, nullptr);
}

cminus::declaration::string::destructor_def::~destructor_def() = default;

void cminus::declaration::string::destructor_def::evaluate_body_() const{
	helper::member_info info{};
	helper::retrieve_info(info, nullptr);

	info.size->write_scalar(static_cast<std::size_t>(0));
	if (auto data_address = info.data->read_scalar<std::size_t>(); data_address != 0u){//Free memory
		runtime::object::memory_object->deallocate_block(data_address);
		info.data->write_scalar(static_cast<std::size_t>(0));
	}
}

cminus::declaration::string::empty_def::~empty_def() = default;

void cminus::declaration::string::empty_def::evaluate_body_() const{
	throw runtime::exception::return_interrupt(runtime::object::global_storage->get_boolean_value(helper::read_value<std::size_t>("size_", nullptr) == 0u));
}

cminus::declaration::string::size_def::~size_def() = default;

void cminus::declaration::string::size_def::evaluate_body_() const{
	throw runtime::exception::return_interrupt(std::make_shared<memory::scalar_reference<std::size_t>>(
		runtime::object::global_storage->get_size_type(),
		helper::read_value<std::size_t>("size_", nullptr)
	));
}

cminus::declaration::string::data_def::~data_def() = default;

void cminus::declaration::string::data_def::evaluate_body_() const{
	throw runtime::exception::return_interrupt(helper::retrieve_data(nullptr));
}

cminus::declaration::string::begin_def::~begin_def() = default;

void cminus::declaration::string::begin_def::evaluate_body_() const{
	throw runtime::exception::return_interrupt(helper::retrieve_data(nullptr));
}

cminus::declaration::string::end_def::~end_def() = default;

void cminus::declaration::string::end_def::evaluate_body_() const{
	helper::data_address_size_value_info info{};
	helper::retrieve_info(info, nullptr);
	throw runtime::exception::return_interrupt(std::make_shared<memory::scalar_reference<std::size_t>>(
		runtime::object::global_storage->get_char_pointer_type(false),
		(info.data + info.size)//Address data is pointing to plus size as offset
	));
}

cminus::declaration::string::at_def::~at_def() = default;

void cminus::declaration::string::at_def::evaluate_body_() const{
	helper::data_address_size_value_info info{};
	helper::retrieve_info(info, nullptr);

	auto position_value = helper::read_value<std::size_t>("position", nullptr);
	if (info.size <= position_value)
		throw runtime::exception::out_of_range();

	throw runtime::exception::return_interrupt(std::make_shared<memory::reference>(
		(info.data + position_value),//Address data is pointing to plus offset
		runtime::object::global_storage->get_char_type()
	));
}
