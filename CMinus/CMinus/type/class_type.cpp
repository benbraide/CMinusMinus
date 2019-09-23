#include "../storage/specialized_storage.h"
#include "../declaration/variable_declaration.h"

#include "proxy_type.h"
#include "class_type.h"

cminus::type::class_::class_(const std::string &name, storage_base *parent)
	: type_base(name, parent){
	class_context_ = std::make_shared<memory::rval_reference>(0u, std::make_shared<type::proxy>(*this));
}

cminus::type::class_::~class_(){
	destroy_entries_();
}

const std::string &cminus::type::class_::get_name() const{
	return type_base::get_name();
}

cminus::type::class_::storage_base *cminus::type::class_::get_parent() const{
	return type_base::get_parent();
}

void cminus::type::class_::destruct(std::shared_ptr<memory::reference> target) const{

}

std::shared_ptr<cminus::memory::reference> cminus::type::class_::get_default_value(std::shared_ptr<type_base> self) const{
	return nullptr;
}

std::size_t cminus::type::class_::get_size() const{
	return ((size_ == 0u) ? 1u : size_);
}

bool cminus::type::class_::is_exact(const type_base &target) const{
	return (target.get_non_proxy() == this);
}

int cminus::type::class_::get_score(const type_base &target) const{
	return 0;
}

std::size_t cminus::type::class_::compute_base_offset(const type_base &base_type) const{
	return 0u;
}

std::shared_ptr<cminus::memory::reference> cminus::type::class_::cast(std::shared_ptr<memory::reference> data, std::shared_ptr<type_base> target_type, cast_type type) const{
	return nullptr;
}

std::shared_ptr<cminus::type::class_::type_base> cminus::type::class_::convert(conversion_type type, std::shared_ptr<type_base> self_or_other) const{
	return nullptr;
}

bool cminus::type::class_::is(query_type type, const type_base *arg) const{
	return (type == query_type::class_);
}

bool cminus::type::class_::is_accessible(unsigned int access) const{
	auto class_storage = runtime::object::current_storage->get_first_of<class_>();
	if (class_storage == this)//Private access
		return true;

	if (class_storage == nullptr || !class_storage->is_base_type_(*this, true))//Public access
		return ((access & (declaration::flags::private_ | declaration::flags::protected_)) == 0u);

	return ((access & declaration::flags::private_) == 0u);//Protected access
}

bool cminus::type::class_::add_base(unsigned int access, std::shared_ptr<class_> value){
	return false;
}

std::shared_ptr<cminus::memory::reference> cminus::type::class_::find(const std::string &name, std::shared_ptr<memory::reference> context) const{
	std::lock_guard<std::mutex> guard(lock_);
	return find_(name, context);
}

const cminus::type::class_::member_variable_info *cminus::type::class_::find_non_static_member(const std::string &name) const{
	std::lock_guard<std::mutex> guard(lock_);

	if (auto it = member_variables_map_.find(name); it != member_variables_map_.end())
		return it->second;

	return nullptr;
}

std::shared_ptr<cminus::memory::reference> cminus::type::class_::find_non_static_member(const std::string &name, std::shared_ptr<memory::reference> context) const{
	auto entry = find_non_static_member(name);
	if (entry == nullptr)
		return nullptr;

	return std::make_shared<memory::reference>(
		(context->get_address() + entry->address_offset),
		entry->value->get_type(),
		entry->value->get_attributes().get_list(),
		context
	);
}

std::shared_ptr<cminus::memory::reference> cminus::type::class_::find_static_member(const std::string &name) const{
	std::lock_guard<std::mutex> guard(lock_);

	if (auto it = named_entries_.find(name); it != named_entries_.end())
		return it->second;

	return nullptr;
}

void cminus::type::class_::construct_(std::shared_ptr<memory::reference> target, const std::vector<std::shared_ptr<memory::reference>> &args) const{

}

void cminus::type::class_::add_(std::shared_ptr<declaration::variable> entry, std::size_t address){
	if (!entry->is(declaration::flags::static_)){
		auto &name = entry->get_name();
		auto entry_memory_size = entry->get_type()->get_memory_size();

		if (name.empty())
			throw storage::exception::unnamed_entry();

		member_variables_.push_back(member_variable_info{ member_size_, entry });
		member_variables_map_[name] = &*member_variables_.rbegin();

		member_size_ += entry_memory_size;
		size_ += entry_memory_size;

		for (auto &base_type : base_types_)//Adjust base offsets
			base_type.address_offset += entry_memory_size;
	}
	else//Static declaration
		storage_base::add_(entry, address);
}

bool cminus::type::class_::exists_(const std::string &name, entry_type type) const{
	return (member_variables_map_.find(name) != member_variables_map_.end() || storage_base::exists_(name, type));
}

std::shared_ptr<cminus::memory::reference> cminus::type::class_::find_(const std::string &name) const{
	auto member_storage = runtime::object::current_storage->get_first_of<storage::class_member>();
	return find_(name, ((member_storage == nullptr) ? nullptr : member_storage->get_context()));
}

std::shared_ptr<cminus::memory::reference> cminus::type::class_::find_(const std::string &name, std::shared_ptr<memory::reference> context) const{
	if (auto it = member_variables_map_.find(name); it != member_variables_map_.end()){
		if (context == nullptr)
			throw storage::exception::no_member_context();

		if (!is_accessible(it->second->value->get_flags()))
			throw storage::exception::inaccessible_entry();

		return std::make_shared<memory::reference>(
			(context->get_address() + it->second->address_offset),
			it->second->value->get_type(),
			it->second->value->get_attributes().get_list(),
			context
		);
	}

	if (auto function_it = functions_.find(name); function_it != functions_.end())
		return std::make_shared<memory::function_reference>(*function_it->second, ((context == nullptr) ? class_context_ : context));

	for (auto &base_type : base_types_){
		if (auto entry = base_type.value->find_(name, context); entry != nullptr)
			return entry;
	}

	auto entry = storage_base::find_(name);
	if (entry != nullptr){
		if (auto declaration_entry = declarations_.find(name); declaration_entry != declarations_.end() && !is_accessible(declaration_entry->second->get_flags()))
			throw storage::exception::inaccessible_entry();
	}

	return entry;
}

cminus::type::class_::storage_base *cminus::type::class_::find_storage_(const std::string &name) const{
	if (name == get_name())
		return const_cast<class_ *>(this);

	for (auto &base_type : base_types_){
		if (auto value = base_type.value->find_storage_(name); value != nullptr)
			return value;
	}

	return storage_base::find_storage_(name);
}

cminus::type::class_ *cminus::type::class_::find_base_type_(const std::string &name, bool search_hierarchy) const{
	for (auto &base_type : base_types_){
		if (base_type.value->get_name() == name)
			return base_type.value.get();

		if (auto value = (search_hierarchy ? base_type.value->find_base_type_(name, true) : nullptr); value != nullptr)
			return value;
	}

	return nullptr;
}

bool cminus::type::class_::is_base_type_(const class_ &target, bool search_hierarchy) const{
	return (find_base_type_(target.get_name(), search_hierarchy) != nullptr);
}
