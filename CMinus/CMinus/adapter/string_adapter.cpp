#include "string_adapter.h"

cminus::adapter::string::string(){
	value_ = runtime::object::global_storage->create_string("", false);
	data_ = runtime::object::global_storage->get_string_value(value_).data();
}

cminus::adapter::string::string(const string &other)
	: string(std::string_view(other.data_)){}

cminus::adapter::string::string(const std::string_view &value){
	value_ = runtime::object::global_storage->create_string(value, false);
	data_ = runtime::object::global_storage->get_string_value(value_).data();
}

cminus::adapter::string::string(const std::string &value)
	: string(std::string_view(value.data(), value.size())){}

cminus::adapter::string::string(const char *value)
	: string(std::string_view(value)){}

cminus::adapter::string::string(const char *value, std::size_t size)
	: string(std::string_view(value, size)){}

cminus::adapter::string::string(std::size_t size, char value){
	auto decl = std::make_shared<cminus::declaration::variable>(
		"",
		runtime::object::global_storage->get_string_type(),
		cminus::attribute::collection::list_type{},
		cminus::declaration::flags::nil,
		std::shared_ptr<memory::reference>()
	);

	value_ = decl->evaluate(0u, std::vector<std::shared_ptr<memory::reference>>{
		std::make_shared<memory::scalar_reference<std::size_t>>(storage::get_cached_type<std::size_t>::type(), size),
		std::make_shared<memory::scalar_reference<char>>(storage::get_cached_type<char>::type(), value)
	});

	data_ = runtime::object::global_storage->get_string_value(value_).data();
}

cminus::adapter::string::string(std::shared_ptr<memory::reference> value){
	auto decl = std::make_shared<cminus::declaration::variable>(
		"",
		runtime::object::global_storage->get_string_type(),
		cminus::attribute::collection::list_type{},
		cminus::declaration::flags::nil,
		value
	);

	value_ = decl->evaluate(0u);
	data_ = runtime::object::global_storage->get_string_value(value_).data();
}

bool cminus::adapter::string::empty() const{
	auto result = runtime::object::global_storage->get_raw_string_type()->find_function("IsEmpty")->call(value_, std::vector<std::shared_ptr<memory::reference>>{});
	return (result->read_scalar<type::boolean_constant>() == type::boolean_constant::true_);
}

std::size_t cminus::adapter::string::size() const{
	auto result = runtime::object::global_storage->get_raw_string_type()->find_function("GetSize")->call(value_, std::vector<std::shared_ptr<memory::reference>>{});
	return result->read_scalar<std::size_t>();
}

const char *cminus::adapter::string::data() const{
	auto result = runtime::object::global_storage->get_raw_string_type()->find_function("GetData")->call(value_, std::vector<std::shared_ptr<memory::reference>>{});
	return reinterpret_cast<char *>(runtime::object::memory_object->get_block(result->read_scalar<std::size_t>())->get_data());
}

const char *cminus::adapter::string::begin() const{
	auto result = runtime::object::global_storage->get_raw_string_type()->find_function("Begin")->call(value_, std::vector<std::shared_ptr<memory::reference>>{});
	return reinterpret_cast<char *>(runtime::object::memory_object->get_block(result->read_scalar<std::size_t>())->get_data());
}

const char *cminus::adapter::string::end() const{
	auto result = runtime::object::global_storage->get_raw_string_type()->find_function("End")->call(value_, std::vector<std::shared_ptr<memory::reference>>{});
	return reinterpret_cast<char *>(runtime::object::memory_object->get_block(result->read_scalar<std::size_t>())->get_data());
}

char cminus::adapter::string::at(std::size_t position) const{
	auto result = runtime::object::global_storage->get_raw_string_type()->find_function("At")->call(value_, std::vector<std::shared_ptr<memory::reference>>{});
	return result->read_scalar<char>();
}

std::size_t cminus::adapter::string::find(const string &other, std::size_t offset) const{
	return find(std::string_view(other.data_), offset);
}

std::size_t cminus::adapter::string::find(const std::string_view &value, std::size_t offset) const{
	auto result = runtime::object::global_storage->get_raw_string_type()->find_function("Find")->call(value_, std::vector<std::shared_ptr<memory::reference>>{
		runtime::object::global_storage->create_string(value),
		std::make_shared<memory::scalar_reference<std::size_t>>(storage::get_cached_type<std::size_t>::type(), offset)
	});

	return result->read_scalar<std::size_t>();
}

std::size_t cminus::adapter::string::find(const std::string &value, std::size_t offset) const{
	return find(std::string_view(value.data(), value.size()), offset);
}

std::size_t cminus::adapter::string::find(const char *value, std::size_t offset) const{
	return find(std::string_view(value), offset);
}

std::size_t cminus::adapter::string::find(const char *value, std::size_t size, std::size_t offset) const{
	return find(std::string_view(value, size), offset);
}

std::size_t cminus::adapter::string::find(std::shared_ptr<cminus::memory::reference> value, std::size_t offset) const{
	auto result = runtime::object::global_storage->get_raw_string_type()->find_function("Find")->call(value_, std::vector<std::shared_ptr<memory::reference>>{
		value,
		std::make_shared<memory::scalar_reference<std::size_t>>(storage::get_cached_type<std::size_t>::type(), offset)
	});

	return result->read_scalar<std::size_t>();
}

void cminus::adapter::string::resize(std::size_t size, char fill){
	auto result = runtime::object::global_storage->get_raw_string_type()->find_function("Resize")->call(value_, std::vector<std::shared_ptr<memory::reference>>{
		std::make_shared<memory::scalar_reference<std::size_t>>(storage::get_cached_type<std::size_t>::type(), size),
		std::make_shared<memory::scalar_reference<char>>(storage::get_cached_type<char>::type(), fill)
	});

	data_ = runtime::object::global_storage->get_string_value(value_).data();
}

void cminus::adapter::string::erase(std::size_t position, std::size_t size){
	auto result = runtime::object::global_storage->get_raw_string_type()->find_function("Erase")->call(value_, std::vector<std::shared_ptr<memory::reference>>{
		std::make_shared<memory::scalar_reference<std::size_t>>(storage::get_cached_type<std::size_t>::type(), position),
		std::make_shared<memory::scalar_reference<std::size_t>>(storage::get_cached_type<std::size_t>::type(), size)
	});

	data_ = runtime::object::global_storage->get_string_value(value_).data();
}

void cminus::adapter::string::clear(){
	auto result = runtime::object::global_storage->get_raw_string_type()->find_function("Clear")->call(value_, std::vector<std::shared_ptr<memory::reference>>{});
	data_ = runtime::object::global_storage->get_string_value(value_).data();
}
