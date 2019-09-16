#include "attribute_guard.h"

cminus::attribute::guard::~guard(){
	try{
		if (callback_ != nullptr)
			callback_();
	}
	catch (...){}
}

cminus::attribute::write_guard::write_guard(std::shared_ptr<memory::reference> target, std::shared_ptr<memory::reference> arg){
	target->get_attributes().query(stage_type::before_write, target, arg);
	callback_ = [=]{
		target->get_attributes().query(stage_type::after_write, target, arg);
	};
}

cminus::attribute::write_guard::~write_guard() = default;

cminus::attribute::read_guard::read_guard(std::shared_ptr<memory::reference> target, std::shared_ptr<memory::reference> arg){
	target->get_attributes().query(stage_type::before_read, target, arg);
	callback_ = [=]{
		target->get_attributes().query(stage_type::after_read, target, arg);
	};
}

cminus::attribute::read_guard::~read_guard() = default;
