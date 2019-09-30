#include "../type/string_type.h"
#include "../type/primitive_type.h"
#include "../storage/global_storage.h"

#include "string_function_definitions.h"

void cminus::declaration::string::helper::retrieve_info(member_info &info, std::shared_ptr<memory::reference> context){
	if (context != nullptr){
		auto string_type = runtime::object::global_storage->get_cached_type(storage::global::cached_type::string);
		auto class_type = dynamic_cast<type::class_ *>(string_type.get());

		info.data = class_type->find("data_", context, false);
		info.size = class_type->find("size_", context, false);
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

void cminus::declaration::string::helper::retrieve_info(data_address_siz_value_info &info, std::shared_ptr<memory::reference> context){
	member_info minfo{};
	retrieve_info(minfo, context);

	info.data = minfo.data->read_scalar<std::size_t>();	//Data address
	info.size = minfo.size->read_scalar<std::size_t>();	//Size value
}

void cminus::declaration::string::helper::retrieve_info(value_info &info, std::shared_ptr<memory::reference> context){
	data_address_siz_value_info ainfo{};
	retrieve_info(ainfo, context);

	info.data = reinterpret_cast<char *>(runtime::object::memory_object->get_block(ainfo.data)->get_data());//Data value
	info.size = ainfo.size;//Size value
}

std::size_t cminus::declaration::string::helper::allocate_block(std::size_t buffer_size, allocation_type how, std::size_t split_index){
	auto size = runtime::object::current_storage->find("size_", true);
	auto size_value = size->read_scalar<std::size_t>();

	auto data = runtime::object::current_storage->find("data_", true);
	auto data_address = data->read_scalar<std::size_t>();

	if ((how == allocation_type::nil) && buffer_size == size_value && data_address != 0u)
		return data_address;

	auto computed_size = buffer_size;
	switch (how){
	case allocation_type::extend:
		computed_size += size_value;
		break;
	case allocation_type::shrink:
		if (size_value < computed_size)
			computed_size -= size_value;
		else
			computed_size = 0u;
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
		if (split_index < size_value)//Copy bytes after split index
			runtime::object::memory_object->write((new_data_address + split_index + buffer_size), (data_address + split_index), (size_value - split_index));

		if (0u < split_index)//Copy bytes before split index
			runtime::object::memory_object->write(new_data_address, data_address, split_index);
	}
	else if (how == allocation_type::shrink){//Copy applicable bytes
		if (split_index < size_value)//Copy bytes after split index
			runtime::object::memory_object->write((new_data_address + split_index), (data_address + split_index + buffer_size), (computed_size - split_index));

		if (0u < split_index)//Copy bytes before split index
			runtime::object::memory_object->write(new_data_address, data_address, split_index);
	}

	data->write_scalar(new_data_address);//Copy new address
	size->write_scalar(computed_size);//Update size

	if (data_address != 0u)//Free previous block
		runtime::object::memory_object->deallocate_block(data_address);

	return new_data_address;
}

void cminus::declaration::string::helper::assign(const char *buffer, std::size_t buffer_size, bool fill){
	auto data_address = allocate_block(buffer_size, allocation_type::nil, 0u);
	if (buffer != nullptr && fill)//Fill byte
		runtime::object::memory_object->set(data_address, static_cast<std::byte>(*buffer), buffer_size);
	else if (buffer != nullptr)//Copy characters
		runtime::object::memory_object->write_buffer(data_address, buffer, buffer_size);
}

void cminus::declaration::string::helper::insert(const char *buffer, std::size_t buffer_size, std::size_t offset, bool fill){
	auto data_address = allocate_block(buffer_size, allocation_type::extend, offset);
	if (buffer != nullptr && fill)//Fill byte
		runtime::object::memory_object->set((data_address + offset), static_cast<std::byte>(*buffer), buffer_size);
	else if (buffer != nullptr)//Copy characters
		runtime::object::memory_object->write_buffer((data_address + offset), buffer, buffer_size);
}

void cminus::declaration::string::helper::erase(std::size_t buffer_size, std::size_t offset){
	allocate_block(buffer_size, allocation_type::shrink, offset);
}

std::size_t cminus::declaration::string::helper::find_buffer(const char *buffer, std::size_t buffer_size, std::size_t offset){
	value_info info{};
	retrieve_info(info, nullptr);

	if (auto result = std::string_view(info.data, info.size).find(buffer, offset, buffer_size); result != std::string_view::npos)
		return result;

	return type::get_max_non_nan<unsigned __int64>::value();
}

cminus::declaration::string::default_constructor_def::~default_constructor_def() = default;

void cminus::declaration::string::default_constructor_def::evaluate_body_() const{
	helper::assign(nullptr, 0u, false);
}

cminus::declaration::string::copy_constructor_def::~copy_constructor_def() = default;

void cminus::declaration::string::copy_constructor_def::evaluate_body_() const{
	helper::value_info info{};
	helper::retrieve_info(info, runtime::object::current_storage->find("other", true));
	helper::assign(info.data, info.size, false);
}

cminus::declaration::string::sub_copy_constructor_def::~sub_copy_constructor_def() = default;

void cminus::declaration::string::sub_copy_constructor_def::evaluate_body_() const{
	helper::value_info info{};
	helper::retrieve_info(info, runtime::object::current_storage->find("other", true));

	auto position_value = helper::read_value<std::size_t>("position");
	if (info.size <= position_value)
		throw runtime::exception::out_of_range();

	helper::assign((info.data + position_value), std::min((info.size - position_value), helper::read_value<std::size_t>("size")), false);
}

cminus::declaration::string::buffer_constructor_def::~buffer_constructor_def() = default;

void cminus::declaration::string::buffer_constructor_def::evaluate_body_() const{
	auto data = reinterpret_cast<char *>(runtime::object::memory_object->get_block(helper::read_value<std::size_t>("data"))->get_data());
	helper::assign(data, ((data == nullptr) ? 0u : std::min(helper::read_value<std::size_t>("size"), std::strlen(data))), false);
}

cminus::declaration::string::fill_constructor_def::~fill_constructor_def() = default;

void cminus::declaration::string::fill_constructor_def::evaluate_body_() const{
	auto fill = helper::read_value<char>("fill");
	helper::assign(&fill, helper::read_value<std::size_t>("size"), true);
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
	throw runtime::exception::return_interrupt(runtime::object::global_storage->get_boolean_value(helper::read_value<std::size_t>("size_") == 0u));
}

cminus::declaration::string::size_def::~size_def() = default;

void cminus::declaration::string::size_def::evaluate_body_() const{
	throw runtime::exception::return_interrupt(std::make_shared<memory::scalar_reference<std::size_t>>(
		runtime::object::global_storage->get_cached_type(storage::global::cached_type::unsigned_long_integer),
		helper::read_value<std::size_t>("size_")
	));
}
