#include "storage_object.h"

cminus::storage::object::~object(){
	destroy_entries_();
}

std::string cminus::storage::object::get_qname() const{
	return ((get_parent() == nullptr) ? get_name() : (get_parent()->get_qname() + "::" + get_name()));
}

void cminus::storage::object::add(std::shared_ptr<declaration::object> entry, std::size_t address){
	auto &name = entry->get_name();
	if (name.empty())
		throw exception::unnamed_entry();

	if (auto function_entry = std::dynamic_pointer_cast<declaration::function_base>(entry); function_entry != nullptr){//Add function
		std::lock_guard<std::mutex> guard(lock_);
		if (exists_(name, entry_type::function))
			throw exception::duplicate_entry();
		return add_(function_entry, address);
	}

	if (exists(name, entry_type::mem_ref))
		throw exception::duplicate_entry();

	std::lock_guard<std::mutex> guard(lock_);
	//add_(entry);
}

void cminus::storage::object::add(const std::string &name){
	if (name.empty())
		throw exception::unnamed_entry();

	std::lock_guard<std::mutex> guard(lock_);
	if (exists_(name, entry_type::nil))
		throw exception::duplicate_entry();

	auto entry = std::make_shared<memory::undefined_reference>(nullptr);
	entries_.push_back(entry);

	if (!name.empty())
		named_entries_[name] = entry.get();
}

void cminus::storage::object::del(const std::string &name){
	std::lock_guard<std::mutex> guard(lock_);
	del_(name);
}

bool cminus::storage::object::exists(const std::string &name, entry_type type) const{
	std::lock_guard<std::mutex> guard(lock_);
	return exists_(name, type);
}

std::shared_ptr<cminus::memory::reference> cminus::storage::object::find(const std::string &name, bool search_tree) const{
	{//Scoped lock
		std::lock_guard<std::mutex> guard(lock_);
		if (auto entry = find_(name); entry != nullptr)
			return entry;
	}

	if (!search_tree)
		return nullptr;

	if (auto parent = get_parent(); parent != nullptr)
		return parent->find(name, true);

	return nullptr;
}

std::shared_ptr<cminus::attribute::object> cminus::storage::object::find_attribute(const std::string &name, bool search_tree) const{
	{//Scoped lock
		std::lock_guard<std::mutex> guard(lock_);
		if (auto entry = find_attribute_(name); entry != nullptr)
			return entry;
	}

	if (!search_tree)
		return nullptr;

	if (auto parent = get_parent(); parent != nullptr)
		return parent->find_attribute(name, true);

	return nullptr;
}

std::shared_ptr<cminus::type::object> cminus::storage::object::find_type(const std::string &name, bool search_tree) const{
	{//Scoped lock
		std::lock_guard<std::mutex> guard(lock_);
		if (auto entry = find_type_(name); entry != nullptr)
			return entry;
	}

	if (!search_tree)
		return nullptr;

	if (auto parent = get_parent(); parent != nullptr)
		return parent->find_type(name, true);

	return nullptr;
}

std::shared_ptr<cminus::storage::object> cminus::storage::object::find_storage(const std::string &name, bool search_tree) const{
	{//Scoped lock
		std::lock_guard<std::mutex> guard(lock_);
		if (auto entry = find_storage_(name); entry != nullptr)
			return entry;
	}

	if (!search_tree)
		return nullptr;

	if (auto parent = get_parent(); parent != nullptr)
		return parent->find_storage(name, true);

	return nullptr;
}

void cminus::storage::object::destroy_entries_(){
	if (entries_.empty())
		return;

	for (auto it = entries_.rbegin(); it != entries_.rend(); ++it)
		*it = nullptr;//Destroy object

	entries_.clear();
	named_entries_.clear();
}

void cminus::storage::object::add_(const std::string &name, std::shared_ptr<memory::reference> entry){
	if (auto it = named_entries_.find(name); it == named_entries_.end()){//Entry was previously undefined
		for (auto entry_it = entries_.begin(); entry_it != entries_.end(); ++entry_it){
			if (entry_it->get() == it->second)
				entries_.erase(entry_it);
		}
	}

	entries_.push_back(entry);
	if (!name.empty())
		named_entries_[name] = entry.get();
}

void cminus::storage::object::add_(std::shared_ptr<declaration::function_base> entry, std::size_t address){
	if (auto function_it = functions_.find(entry->get_name()); function_it == functions_.end()){//New entry
		std::shared_ptr<memory::block> block;
		if (address == 0u)
			block = runtime::object::memory_object->allocate_block(sizeof(void *));
		else
			block = runtime::object::memory_object->find_block(address);

		if (block == nullptr || block->get_address() == 0u)
			throw memory::exception::allocation_failure();
	}
	else//Add to existing
		function_it->second->add(entry);
}

void cminus::storage::object::add_(std::shared_ptr<attribute::object> entry){

}

void cminus::storage::object::add_(std::shared_ptr<type::object> entry){

}

void cminus::storage::object::add_(std::shared_ptr<object> entry){

}

void cminus::storage::object::del_(const std::string &name){
	if (name.empty() || named_entries_.empty())
		throw exception::entry_not_found();

	if (auto it = named_entries_.find(name); it != named_entries_.end()){
		for (auto entry_it = entries_.begin(); entry_it != entries_.end(); ++entry_it){
			if (entry_it->get() == it->second){
				*entry_it = std::make_shared<memory::undefined_reference>(nullptr);
				it->second = entry_it->get();
				break;
			}
		}
	}
	else
		throw exception::entry_not_found();
}

bool cminus::storage::object::exists_(const std::string &name, entry_type type) const{
	return (name.empty() || named_entries_.find(name) != named_entries_.end() || functions_.find(name) != functions_.end() ||
		types_.find(name) != types_.end() || storages_.find(name) != storages_.end());
}

std::shared_ptr<cminus::memory::reference> cminus::storage::object::find_(const std::string &name) const{
	if (name.empty() || named_entries_.empty())
		return nullptr;

	if (auto it = named_entries_.find(name); it != named_entries_.end()){
		for (auto entry : entries_){
			if (entry.get() == it->second)
				return entry;
		}

		return nullptr;
	}
	
	if (auto function_it = functions_.find(name); function_it != functions_.end())
		return std::make_shared<memory::function_reference>(*function_it->second, nullptr);

	return nullptr;
}

std::shared_ptr<cminus::attribute::object> cminus::storage::object::find_attribute_(const std::string &name) const{
	if (name.empty() || attributes_.empty())
		return nullptr;

	if (auto it = attributes_.find(name); it != attributes_.end())
		return it->second;

	return nullptr;
}

std::shared_ptr<cminus::type::object> cminus::storage::object::find_type_(const std::string &name) const{
	if (name.empty() || types_.empty())
		return nullptr;

	if (auto it = types_.find(name); it != types_.end())
		return it->second;

	return nullptr;
}

std::shared_ptr<cminus::storage::object> cminus::storage::object::find_storage_(const std::string &name) const{
	if (name.empty())
		return nullptr;

	if (!storages_.empty()){
		if (auto it = storages_.find(name); it != storages_.end())
			return it->second;
	}

	return std::dynamic_pointer_cast<object>(find_type_(name));
}

cminus::storage::named_object::named_object(const std::string &name, object *parent)
	: name_(name), parent_(parent){}

cminus::storage::named_object::~named_object(){
	destroy_entries_();
}

const std::string &cminus::storage::named_object::get_name() const{
	return name_;
}

cminus::storage::object *cminus::storage::named_object::get_parent() const{
	return parent_;
}
