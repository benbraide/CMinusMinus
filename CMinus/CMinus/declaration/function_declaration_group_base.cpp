#include "special_function_declaration.h"

#include "function_declaration_group_base.h"

cminus::declaration::function_group_base::~function_group_base() = default;

void cminus::declaration::function_group_base::add(std::shared_ptr<function_base> entry){
	if (dynamic_cast<constructor *>(entry.get()) == nullptr && dynamic_cast<destructor *>(entry.get()) == nullptr)
		add_(entry);
	else
		throw exception::function_expected();
}

cminus::declaration::constructor_group_base::~constructor_group_base() = default;

void cminus::declaration::constructor_group_base::add(std::shared_ptr<function_base> entry){
	if (dynamic_cast<constructor *>(entry.get()) != nullptr)
		add_(entry);
	else
		throw exception::constructor_expected();
}

cminus::declaration::destructor_group_base::~destructor_group_base() = default;

void cminus::declaration::destructor_group_base::add(std::shared_ptr<function_base> entry){
	if (dynamic_cast<destructor *>(entry.get()) != nullptr)
		add_(entry);
	else
		throw exception::destructor_expected();
}
