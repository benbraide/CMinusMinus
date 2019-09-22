#include "../declaration/variable_declaration.h"

#include "class_type.h"

cminus::type::class_::class_(const std::string &name, storage_base *parent)
	: type_base(name, parent){}

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

bool cminus::type::class_::add_base(unsigned int access, std::shared_ptr<class_> value){
	return false;
}

std::shared_ptr<cminus::memory::reference> cminus::type::class_::find(const std::string &name, std::shared_ptr<memory::reference> context) const{
	std::lock_guard<std::mutex> guard(lock_);
	return find_(name, context);
}

const cminus::type::class_::member_variable_info *cminus::type::class_::find_member(const std::string &name) const{
	std::lock_guard<std::mutex> guard(lock_);
	return find_member_(name);
}

void cminus::type::class_::construct_(std::shared_ptr<memory::reference> target, const std::vector<std::shared_ptr<memory::reference>> &args) const{

}

void cminus::type::class_::add_(std::shared_ptr<declaration::variable> entry, std::size_t address){
	if (!entry->is(declaration::flags::static_)){
		auto entry_memory_size = entry->get_type()->get_memory_size();

		member_variables_.push_back(member_variable_info{ member_size_, entry });
		member_variables_map_[entry->get_name()] = &*member_variables_.rbegin();

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
	return find_(name, nullptr);
}

std::shared_ptr<cminus::memory::reference> cminus::type::class_::find_(const std::string &name, std::shared_ptr<memory::reference> context) const{
	if (auto it = member_variables_map_.find(name); it != member_variables_map_.end()){
		if (context == nullptr)
			return nullptr;//#TODO: Throw

		return std::make_shared<memory::reference>(
			(context->get_address() + it->second->address_offset),
			it->second->value->get_type(),
			it->second->value->get_attributes().get_list(),
			context
		);
	}

	if (auto function_it = functions_.find(name); function_it != functions_.end())
		return std::make_shared<memory::function_reference>(*function_it->second, context);

	for (auto &base_type : base_types_){
		if (auto entry = base_type.value->find_(name, context); entry != nullptr)
			return entry;
	}

	return storage_base::find_(name);
}

const cminus::type::class_::member_variable_info *cminus::type::class_::find_member_(const std::string &name) const{
	if (auto it = member_variables_map_.find(name); it != member_variables_map_.end())
		return it->second;
	return nullptr;
}
