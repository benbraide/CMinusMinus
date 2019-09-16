#include "../type/type_object.h"

#include "attribute_collection.h"

cminus::attribute::collection::collection(std::shared_ptr<type::object> target_type)
	: target_type_(target_type){}

cminus::attribute::collection::collection(const list_type &list, std::shared_ptr<type::object> target_type)
	: target_type_(target_type){
	for (auto entry : list)
		add(entry);
}

cminus::attribute::collection::collection(const map_type &list, std::shared_ptr<type::object> target_type)
	: list_(list), target_type_(target_type){}

cminus::attribute::collection::collection(map_type &&list, std::shared_ptr<type::object> target_type)
	: list_(std::move(list)), target_type_(target_type){}

void cminus::attribute::collection::set_target_type(std::shared_ptr<type::object> value){
	target_type_ = value;
}

std::shared_ptr<cminus::type::object> cminus::attribute::collection::get_target_type() const{
	return target_type_;
}

void cminus::attribute::collection::add(std::shared_ptr<object> value){
	if (value != nullptr && !has(*value))
		list_[value.get()] = value;
}

void cminus::attribute::collection::remove(const object &target){
	if (list_.empty())
		return;

	for (auto it = list_.begin(); it != list_.end(); ++it){
		if (it->first->is_same(target)){
			list_.erase(it);
			break;
		}
	}
}

void cminus::attribute::collection::remove(const std::string &qname){
	if (list_.empty())
		return;

	for (auto it = list_.begin(); it != list_.end(); ++it){
		if (it->first->get_qname() == qname){
			list_.erase(it);
			break;
		}
	}
}

void cminus::attribute::collection::remove_primitive(const std::string &name){
	if (list_.empty())
		return;

	for (auto it = list_.begin(); it != list_.end(); ++it){
		if (it->first->is_primitive() && it->first->get_name() == name){
			list_.erase(it);
			break;
		}
	}
}

void cminus::attribute::collection::clear(){
	list_.clear();
}

bool cminus::attribute::collection::has(const object &target) const{
	if (list_.empty())
		return false;

	for (auto &entry : list_){
		if (entry.first->is_same(target))
			return true;
	}

	return false;
}

bool cminus::attribute::collection::has(const std::string &qname) const{
	if (list_.empty())
		return false;

	for (auto &entry : list_){
		if (entry.first->get_qname() == qname)
			return true;
	}

	return false;
}

bool cminus::attribute::collection::has_primitive(const std::string &name) const{
	if (list_.empty())
		return false;

	for (auto &entry : list_){
		if (entry.first->is_primitive() && entry.first->get_name() == name)
			return true;
	}

	return false;
}

void cminus::attribute::collection::traverse(const std::function<void(std::shared_ptr<object>)> &callback) const{
	if (list_.empty())
		return;

	auto no_type = (target_type_ == nullptr);
	for (auto &entry : list_){
		if (no_type || entry.first->applies_to_type(*target_type_))
			callback(entry.second);
	}
}

void cminus::attribute::collection::traverse_all(const std::function<void(std::shared_ptr<object>)> &callback) const{
	if (list_.empty())
		return;

	for (auto &entry : list_)
		callback(entry.second);
}

cminus::attribute::query_result cminus::attribute::collection::query(stage_type stage, std::shared_ptr<memory::reference> target, std::shared_ptr<memory::reference> arg) const{
	auto result = query_result::nil;
	traverse([&](std::shared_ptr<object> attr){
		if (auto current_result = object::handle_query_result(stage, *attr, target, arg); result == query_result::nil)
			result = current_result;
	});

	return result;
}

const cminus::attribute::collection::map_type &cminus::attribute::collection::get_list() const{
	return list_;
}
