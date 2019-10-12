#include "../storage/global_storage.h"
#include "../declaration/variable_declaration.h"

#include "iterator_type.h"

cminus::type::iterator::iterator(state_type state)
	: class_("", nullptr){
	auto is_const = false;
	switch (state){
	case state_type::reversed:
		name_ = "ReverseIterator";
		evaluator_type_ = evaluator::object::id_type::riter;
		break;
	case state_type::const_:
		name_ = "ConstIterator";
		evaluator_type_ = evaluator::object::id_type::iter;
		is_const = true;
		break;
	case state_type::reversed_const:
		name_ = "ConstReverseIterator";
		evaluator_type_ = evaluator::object::id_type::riter;
		is_const = true;
		break;
	default:
		name_ = "Iterator";
		evaluator_type_ = evaluator::object::id_type::iter;
		break;
	}

	add_variable_(std::make_shared<declaration::variable>(
		"data_",																//Name
		runtime::object::global_storage->get_auto_pointer_type(is_const),		//Type
		attribute::collection::list_type{},										//Attributes
		declaration::flags::private_,											//Flags
		std::shared_ptr<memory::reference>()									//Initialization
	), 0u);
}

cminus::type::iterator::~iterator(){
	destroy_entries_();
}

void cminus::type::iterator::print_value(io::writer &writer, std::shared_ptr<memory::reference> data) const{
	if (auto private_data = find_non_static_member("data_", data); private_data != nullptr)
		private_data->get_type()->print_value(writer, private_data);
	else
		throw runtime::exception::not_supported();
}

std::shared_ptr<cminus::evaluator::object> cminus::type::iterator::get_evaluator() const{
	return runtime::object::global_storage->get_evaluator(evaluator_type_);
}
