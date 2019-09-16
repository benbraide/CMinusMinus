#include "../type/type_object.h"
#include "../memory/memory_reference.h"
#include "../storage/storage_object.h"

#include "attribute_object.h"

cminus::attribute::object::object(const std::string &name, storage::object *parent)
	: name_(name), parent_(parent){}

cminus::attribute::object::~object() = default;

const std::string &cminus::attribute::object::get_name() const{
	return name_;
}

std::string cminus::attribute::object::get_qname() const{
	return ((parent_ == nullptr) ? name_ : (parent_->get_qname() + "::" + name_));
}

cminus::storage::object *cminus::attribute::object::get_parent() const{
	return parent_;
}

bool cminus::attribute::object::is_primitive() const{
	return true;
}

bool cminus::attribute::object::is_same(const object &other) const{
	if (&other == this)
		return true;

	if (is_primitive())
		return (other.is_primitive() && other.name_ == name_);

	return (!other.is_primitive() && other.get_qname() == get_qname());
}

bool cminus::attribute::object::applies_to_type(const type::object &target_type) const{
	return true;
}

std::string cminus::attribute::object::get_message() const{
	return "Unknown error";
}

cminus::attribute::query_result cminus::attribute::object::handle_query_result(stage_type stage, const object &attr, std::shared_ptr<memory::reference> target, std::shared_ptr<memory::reference> arg){
	return handle_query_result(attr.query(stage, target, arg), attr, arg);
}

cminus::attribute::query_result cminus::attribute::object::handle_query_result(query_result result, const object &target, std::shared_ptr<memory::reference> arg){
	switch (result){
	case query_result::prohibit:
		return query_result::nil;
	case query_result::info:
		return query_result::nil;
	case query_result::require://Argument must contain target attribute
		return query_result::nil;
	default:
		break;
	}

	return result;
}
