#include "proxy_type.h"
#include "enum_type.h"

cminus::type::enum_::enum_(const std::string &name, storage_base *parent)
	: type_base(name, parent){}

cminus::type::enum_::~enum_() = default;

const std::string &cminus::type::enum_::get_name() const{
	return type_base::get_name();
}

cminus::storage::object *cminus::type::enum_::get_parent() const{
	return type_base::get_parent();
}

std::size_t cminus::type::enum_::get_size() const{
	return sizeof(std::size_t);
}

bool cminus::type::enum_::is_exact(const type_base &target) const{
	return (target.get_non_proxy() == this);
}

int cminus::type::enum_::get_score(const type_base &target) const{
	if (target.is(query_type::explicit_auto))
		return get_score_value(score_result_type::auto_assignable);
	return get_score_value(is_exact(target) ? score_result_type::exact : score_result_type::nil);
}

std::shared_ptr<cminus::memory::reference> cminus::type::enum_::cast(std::shared_ptr<memory::reference> data, std::shared_ptr<type_base> target_type, cast_type type) const{
	if (type != cast_type::static_ && type != cast_type::rval_static)
		return nullptr;

	auto is_ref = target_type->is(query_type::ref);
	auto is_lval = data->is_lvalue();

	if (is_ref && !target_type->is(query_type::const_) && !is_lval)
		return nullptr;

	if (type == cast_type::rval_static || !is_lval || is_ref)
		return data;//No copy needed

	return std::make_shared<memory::scalar_reference<std::size_t>>(target_type, data->read_scalar<std::size_t>());
}

bool cminus::type::enum_::is(query_type type, const type_base *arg) const{
	return (type == query_type::enum_ || type == query_type::primitive || type_base::is(type, arg));
}

void cminus::type::enum_::add(std::shared_ptr<declaration::object> entry, std::size_t address){
	throw declaration::exception::bad_declaration();
}

void cminus::type::enum_::add(const std::string &name){
	if (name.empty())
		throw storage::exception::unnamed_entry();

	std::lock_guard<std::mutex> guard(lock_);
	if (!exists_(name, entry_type::nil))
		items_.push_back(name);
	else
		throw storage::exception::duplicate_entry();
}

void cminus::type::enum_::add_entry(const std::string &name, std::shared_ptr<memory::reference> value, bool check_existing){
	throw declaration::exception::bad_declaration();
}

std::shared_ptr<cminus::memory::reference> cminus::type::enum_::get_item(std::size_t index) const{
	std::lock_guard<std::mutex> guard(lock_);
	if (index < items_.size())
		return std::make_shared<memory::scalar_reference<std::size_t>>(std::make_shared<proxy>(*const_cast<enum_ *>(this)), index);
	return nullptr;
}

std::size_t cminus::type::enum_::get_item_index(const std::string &name) const{
	std::lock_guard<std::mutex> guard(lock_);
	std::size_t index = 0u;
	for (auto &item : items_){
		if (item == name)
			return index;
		++index;
	}

	return static_cast<std::size_t>(-1);
}

void cminus::type::enum_::del_(const std::string &name){
	throw declaration::exception::bad_declaration();
}

bool cminus::type::enum_::exists_(const std::string &name, entry_type type) const{
	for (auto &item : items_){
		if (item == name)
			return true;
	}

	return false;
}

std::shared_ptr<cminus::memory::reference> cminus::type::enum_::find_(const std::string &name) const{
	std::size_t index = 0u;
	for (auto &item : items_){
		if (item == name)
			return std::make_shared<memory::scalar_reference<std::size_t>>(std::make_shared<proxy>(*const_cast<enum_ *>(this)), index);
		++index;
	}

	return nullptr;
}
