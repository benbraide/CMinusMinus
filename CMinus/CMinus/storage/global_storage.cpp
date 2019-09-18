#include "../evaluator/initializer.h"

#include "global_storage.h"

cminus::storage::global::global()
	: object("", nullptr){
	default_initializer_ = std::make_shared<evaluator::initializer>();
}

cminus::storage::global::~global(){
	destroy_entries_();
}

std::shared_ptr<cminus::evaluator::object> cminus::storage::global::get_evaluator_for(const type::object &target_type) const{
	return nullptr;
}

std::shared_ptr<cminus::evaluator::initializer> cminus::storage::global::get_default_initializer() const{
	return default_initializer_;
}
