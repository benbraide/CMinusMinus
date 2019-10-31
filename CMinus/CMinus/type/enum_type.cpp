#include "../storage/global_storage.h"

#include "proxy_type.h"
#include "enum_type.h"

cminus::type::enum_::enum_(const std::string &name, storage_base *parent)
	: primitive(name, parent){}

cminus::type::enum_::~enum_(){
	destroy_entries_();
	if (address_ != 0u){
		try{
			runtime::object::memory_object->deallocate_block(address_);
		}
		catch (...){}
		address_ = 0u;
	}
}

const std::string &cminus::type::enum_::get_name() const{
	return type_base::get_name();
}

std::string cminus::type::enum_::get_qname() const{
	return type_base::get_qname();
}

cminus::storage::object *cminus::type::enum_::get_parent() const{
	return type_base::get_parent();
}

void cminus::type::enum_::print_value(io::writer &writer, std::shared_ptr<memory::reference> data) const{
	auto index = read_value_as_<std::size_t>(data);
	if (items_.size() <= index)
		throw runtime::exception::out_of_range();

	auto qname = get_qname();
	writer.write_buffer(qname.data(), qname.size());
	writer.write_buffer("::");

	auto it = std::next(items_.begin(), index);
	writer.write_buffer(it->data(), it->size());
}

std::size_t cminus::type::enum_::get_size() const{
	return size_;
}

cminus::evaluator::object::id_type cminus::type::enum_::get_evaluator_id() const{
	return evaluator::object::id_type::enum_;
}

void cminus::type::enum_::add(std::shared_ptr<declaration::object> entry, std::size_t address){
	throw declaration::exception::bad_declaration();
}

void cminus::type::enum_::add_entry(std::shared_ptr<declaration::object> entry, std::shared_ptr<memory::reference> value, bool check_existing){
	throw declaration::exception::bad_declaration();
}

void cminus::type::enum_::add(const std::string &name){
	if (name.empty())
		throw storage::exception::unnamed_entry();

	lock_guard guard(*this);
	add_(name);
}

std::shared_ptr<cminus::memory::reference> cminus::type::enum_::get_item(std::size_t index) const{
	lock_guard guard(*this);
	if (index < items_.size())
		return std::make_shared<memory::scalar_reference<std::size_t>>(std::make_shared<proxy>(*const_cast<enum_ *>(this)), index);
	return nullptr;
}

std::size_t cminus::type::enum_::get_item_index(const std::string &name) const{
	lock_guard guard(*this);
	std::size_t index = 0u;
	for (auto &item : items_){
		if (item == name)
			return index;
		++index;
	}

	return static_cast<std::size_t>(-1);
}

void cminus::type::enum_::compile(){
	lock_guard guard(*this);
	compile_();
}

bool cminus::type::enum_::exists_(const std::string &name) const{
	for (auto &item : items_){
		if (item == name)
			return true;
	}

	return false;
}

std::shared_ptr<cminus::memory::reference> cminus::type::enum_::find_(const std::string &name, std::shared_ptr<memory::reference> context, std::size_t address) const{
	if (auto it = items_map_.find(name); it != items_map_.end())
		return std::make_shared<memory::rval_reference>(it->second, std::make_shared<proxy>(*const_cast<enum_ *>(this)));
	return nullptr;
}

void cminus::type::enum_::add_(const std::string &name){
	if (!exists_(name))
		items_.push_back(name);
	else
		throw storage::exception::duplicate_entry();
}

void cminus::type::enum_::compile_(){
	if (items_.empty())
		throw declaration::exception::bad_declaration();

	if (items_.size() <= std::numeric_limits<unsigned __int8>::max())
		compile_<unsigned __int8>(*runtime::object::memory_object);
	else if (items_.size() <= std::numeric_limits<unsigned __int16>::max())
		compile_<unsigned __int16>(*runtime::object::memory_object);
	else if (items_.size() <= std::numeric_limits<unsigned __int32>::max())
		compile_<unsigned __int32>(*runtime::object::memory_object);
	else
		compile_<unsigned __int64>(*runtime::object::memory_object);
}
