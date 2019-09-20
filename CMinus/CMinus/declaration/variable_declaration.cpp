#include "variable_declaration.h"

cminus::declaration::variable::~variable() = default;

std::shared_ptr<cminus::node::object> cminus::declaration::variable::get_initialization() const{
	return initialization_;
}

std::shared_ptr<cminus::memory::reference> cminus::declaration::variable::evaluate(std::size_t address, std::shared_ptr<node::object> initialization) const{
	if (initialization == nullptr && (initialization = initialization_) == nullptr)
		return evaluate_(address, std::vector<std::shared_ptr<memory::reference>>{});

	std::vector<std::shared_ptr<memory::reference>> args;
	args.reserve(initialization->get_list_count());

	initialization->traverse_list([&](const node::object &entry){
		args.push_back(entry.evaluate());
	});

	return evaluate_(address, std::vector<std::shared_ptr<memory::reference>>{ args });
}

std::shared_ptr<cminus::memory::reference> cminus::declaration::variable::evaluate(std::size_t address, const std::vector<std::shared_ptr<memory::reference>> &initialization) const{
	return (initialization.empty() ? evaluate(address, initialization_) : evaluate_(address, initialization));
}

std::shared_ptr<cminus::memory::reference> cminus::declaration::variable::evaluate(std::size_t address, std::shared_ptr<memory::reference> initialization) const{
	return ((initialization == nullptr) ? evaluate(address, initialization_) : evaluate_(address, std::vector<std::shared_ptr<memory::reference>>{ initialization }));
}

std::shared_ptr<cminus::memory::reference> cminus::declaration::variable::evaluate(std::size_t address) const{
	return evaluate(address, initialization_);
}

std::shared_ptr<cminus::memory::reference> cminus::declaration::variable::allocate_memory(std::size_t address, std::shared_ptr<type::object> initialization_type) const{
	if (static_value_ != nullptr)
		return static_value_;

	auto computed_type = ((initialization_type == nullptr) ? nullptr : type_->convert(type::object::conversion_type::infer, initialization_type));
	if (computed_type == nullptr)//No conversion
		computed_type = type_;

	if (computed_type->is(type::object::query_type::inferred))//Conversion required
		throw evaluator::exception::inferred_type();

	std::shared_ptr<memory::reference> reference;
	if (computed_type->is(type::object::query_type::ref)){
		if (address == 0u)//Allocate memory
			reference = std::make_shared<memory::indirect_reference>(computed_type, attributes_.get_list(), nullptr);
		else//Use address
			reference = std::make_shared<memory::indirect_reference>(address, computed_type, attributes_.get_list(), nullptr);
	}
	else if (address == 0u)//Allocate memory
		reference = std::make_shared<memory::reference>(computed_type, attributes_.get_list(), nullptr);
	else//Use address
		reference = std::make_shared<memory::reference>(address, computed_type, attributes_.get_list(), nullptr);

	if (reference == nullptr || reference->get_address() == 0u)
		throw memory::exception::allocation_failure();

	if (is(flags::static_))//Update static value
		static_value_ = reference;

	return reference;
}

void cminus::declaration::variable::initialize_memory(std::shared_ptr<memory::reference> target, std::shared_ptr<node::object> value) const{
	if (value != nullptr || (value = initialization_) != nullptr){
		std::vector<std::shared_ptr<memory::reference>> args;
		args.reserve(value->get_list_count());

		value->traverse_list([&](const node::object &entry){
			args.push_back(entry.evaluate());
		});

		initialize_memory_(target, std::vector<std::shared_ptr<memory::reference>>{ args });
	}
	else//No initialization
		initialize_memory_(target, std::vector<std::shared_ptr<memory::reference>>{});
}

void cminus::declaration::variable::initialize_memory(std::shared_ptr<memory::reference> target, const std::vector<std::shared_ptr<memory::reference>> &value) const{
	if (value.empty())
		initialize_memory(target, initialization_);
	else//Use list
		initialize_memory_(target, value);
}

void cminus::declaration::variable::initialize_memory(std::shared_ptr<memory::reference> target, std::shared_ptr<memory::reference> value) const{
	if (value == nullptr)
		initialize_memory(target, initialization_);
	else
		initialize_memory_(target, std::vector<std::shared_ptr<memory::reference>>{ value });
}

void cminus::declaration::variable::initialize_memory(std::shared_ptr<memory::reference> target) const{
	initialize_memory(target, initialization_);
}

std::shared_ptr<cminus::memory::reference> cminus::declaration::variable::evaluate_(std::size_t address, const std::vector<std::shared_ptr<memory::reference>> &initialization) const{
	if (static_value_ != nullptr)
		return static_value_;

	if (auto reference = allocate_memory(address, (initialization.empty() ? nullptr : (*initialization.rbegin())->get_type())); reference != nullptr){
		initialize_memory_(reference, initialization);
		return reference;
	}

	throw memory::exception::allocation_failure();
	return nullptr;
}

void cminus::declaration::variable::initialize_memory_(std::shared_ptr<memory::reference> target, const std::vector<std::shared_ptr<memory::reference>> &value) const{
	target->get_type()->construct(target, value);
}