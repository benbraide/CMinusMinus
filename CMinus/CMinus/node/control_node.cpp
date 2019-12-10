#include "../storage/global_storage.h"
#include "../declaration/variable_declaration.h"

#include "identifier_node.h"
#include "expression_node.h"
#include "declaration_node.h"
#include "control_node.h"

cminus::node::control::control(std::shared_ptr<object> condition, std::shared_ptr<object> statement)
	: condition_(condition), statement_(statement){}

cminus::node::control::~control() = default;

std::shared_ptr<cminus::memory::reference> cminus::node::control::evaluate() const{
	storage::control store;
	init_();

	std::shared_ptr<memory::reference> result;
	if (condition_is_true_(true)){
		do{
			store.init_inner();
			try{
				result = evaluate_();
			}
			catch (const runtime::exception::control_interrupt &e){
				if (e.get_value() == runtime::exception::control_interrupt::value_type::break_)
					break;
			}
			catch (...){
				store.uninit_inner();
				throw;
			}

			store.uninit_inner();
			update_();
		} while (is_iteration_() && condition_is_true_(false));
	}
	else//Evaluate else block, if any
		result = else_(store);

	uninit_();
	return result;
}

bool cminus::node::control::is_iteration_() const{
	return false;
}

void cminus::node::control::init_() const{}

bool cminus::node::control::condition_is_true_(bool is_first) const{
	if (condition_ == nullptr)
		return true;

	auto value = condition_->evaluate();
	if (value == nullptr)
		throw runtime::exception::bad_control_condition();

	auto boolean_value = value->get_type()->cast(value, runtime::object::global_storage->get_boolean_type(), type::cast_type::static_rval);
	if (boolean_value == nullptr)
		throw runtime::exception::bad_control_condition();

	return (boolean_value->read_scalar<type::boolean_constant>() == type::boolean_constant::true_);
}

std::shared_ptr<cminus::memory::reference> cminus::node::control::evaluate_() const{
	return statement_->evaluate();
}

void cminus::node::control::update_() const{}

void cminus::node::control::uninit_() const{}

std::shared_ptr<cminus::memory::reference> cminus::node::control::else_(storage::control &store) const{
	return nullptr;
}

cminus::node::control_with_else::control_with_else(std::shared_ptr<object> condition, std::shared_ptr<object> statement, std::shared_ptr<object> else_value)
	: control(condition, statement), else_value_(else_value){}

cminus::node::control_with_else::~control_with_else() = default;

std::shared_ptr<cminus::memory::reference> cminus::node::control_with_else::else_(storage::control &store) const{
	if (else_value_ == nullptr)
		return nullptr;

	std::shared_ptr<memory::reference> result;
	try{
		store.init_inner();
		result = else_value_->evaluate();
		store.uninit_inner();
	}catch (...){
		store.uninit_inner();
		throw;
	}

	return result;
}

cminus::node::if_control::if_control(std::shared_ptr<object> init_value, std::shared_ptr<object> condition, std::shared_ptr<object> statement, std::shared_ptr<object> else_value)
	: control_with_else(condition, statement, else_value), init_value_(init_value){}

cminus::node::if_control::~if_control() = default;

void cminus::node::if_control::init_() const{
	if (init_value_ != nullptr)
		init_value_->evaluate();
}

cminus::node::while_control::while_control(std::shared_ptr<object> init_value, std::shared_ptr<object> condition, std::shared_ptr<object> statement, std::shared_ptr<object> else_value)
	: control_with_else(condition, statement, else_value), init_value_(init_value){}

cminus::node::while_control::~while_control() = default;

bool cminus::node::while_control::is_iteration_() const{
	return true;
}

void cminus::node::while_control::init_() const{
	if (init_value_ != nullptr)
		init_value_->evaluate();
}

cminus::node::do_while_control::do_while_control(std::shared_ptr<object> condition, std::shared_ptr<object> statement)
	: while_control(nullptr, condition, statement, nullptr){}

cminus::node::do_while_control::~do_while_control() = default;

bool cminus::node::do_while_control::condition_is_true_(bool is_first) const{
	return (is_first || while_control::condition_is_true_(is_first));
}

cminus::node::for_control::for_control(std::shared_ptr<object> init_value, std::shared_ptr<object> update_value, std::shared_ptr<object> condition, std::shared_ptr<object> statement, std::shared_ptr<object> else_value)
	: while_control(init_value, condition, statement, else_value), update_value_(update_value){}

cminus::node::for_control::~for_control() = default;

void cminus::node::for_control::update_() const{
	if (update_value_ != nullptr)
		update_value_->evaluate();
}

cminus::node::for_each_control::for_each_control(std::shared_ptr<variable_declaration> decl, std::shared_ptr<object> target, std::shared_ptr<object> statement, std::shared_ptr<object> else_value)
	: for_control(nullptr, nullptr, nullptr, statement, else_value){
	auto core_scope = std::make_shared<scoped_identifier>(nullptr, "Core");								// ::Core

	auto begin = std::make_shared<scoped_identifier>(core_scope, "Begin");								// ::Core::Begin
	auto end = std::make_shared<scoped_identifier>(core_scope, "End");									// ::Core::End

	auto begin_call = std::make_shared<binary_expression>(operators::id::call, begin, target);			// ::Core::Begin(<TARGET>)
	auto end_call = std::make_shared<binary_expression>(operators::id::call, end, target);				// ::Core::End(<TARGET>)

	auto &decl_name = decl->get_decl_value()->get_name();
	auto auto_type = std::make_shared<primtive_type>(primtive_type::id_type::auto_);					// Auto

	auto begin_decl = std::make_shared<variable_declaration>(
		("__" + decl_name + "__BEGIN__"),
		auto_type,
		nullptr,
		declaration::flags::nil,
		begin_call
	);																									// Auto __<NAME>__BEGIN__ = ::Core::Begin(<TARGET>)

	auto end_decl = std::make_shared<variable_declaration>(
		("__" + decl_name + "__END__"),
		auto_type,
		nullptr,
		declaration::flags::nil,
		end_call
	);																									// Auto __<NAME>__END__ = ::Core::End(<TARGET>)
}

cminus::node::for_each_control::~for_each_control() = default;

cminus::node::try_control::try_control(std::shared_ptr<object> statement, std::shared_ptr<object> catch_value)
	: control_with_else(nullptr, statement, catch_value){}

cminus::node::try_control::~try_control() = default;

std::shared_ptr<cminus::memory::reference> cminus::node::try_control::evaluate_() const{
	try{
		control_with_else::evaluate_();
	}
	catch (const runtime::exception::return_interrupt &){
		throw;//Ignore
	}
	catch (const runtime::exception::control_interrupt &){
		throw;//Ignore
	}
	catch (const std::shared_ptr<memory::reference> &e){
		handle_(e);
	}
	catch (const exception_base &e){
		handle_(e.create_value());
	}

	return nullptr;
}

void cminus::node::try_control::handle_(std::shared_ptr<memory::reference> e) const{
	try{
		runtime::object::current_exception = e;
		else_value_->evaluate();

		if (runtime::object::current_exception == nullptr)//Finally
			else_value_->evaluate();
		else//Forward exception
			throw e;
	}
	catch (...){
		runtime::object::current_exception = nullptr;
		else_value_->evaluate();//Finally
		throw;
	}
}

cminus::node::catch_control::catch_control(std::shared_ptr<object> condition, std::shared_ptr<object> statement, std::shared_ptr<object> catch_value)
	: control_with_else(condition, statement, catch_value){}

cminus::node::catch_control::~catch_control() = default;

bool cminus::node::catch_control::condition_is_true_(bool is_first) const{
	if (runtime::object::current_exception == nullptr)
		return false;

	auto var = dynamic_cast<variable_declaration *>(condition_.get());
	if (var == nullptr)
		return false;

	auto target_type = var->get_decl_value()->get_type();
	return (runtime::object::current_exception->get_type()->get_score(
		*target_type,
		runtime::object::current_exception->is_lvalue(),
		runtime::object::current_exception->is_const()
	) != type::object::get_score_value(type::object::score_result_type::nil));
}

std::shared_ptr<cminus::memory::reference> cminus::node::catch_control::evaluate_() const{
	dynamic_cast<variable_declaration *>(condition_.get())->get_decl_value()->evaluate(0u, runtime::object::current_exception);
	control_with_else::evaluate_();
	return nullptr;
}

cminus::node::finally_control::finally_control(std::shared_ptr<object> statement)
	: control(nullptr, statement){}

cminus::node::finally_control::~finally_control() = default;

bool cminus::node::finally_control::condition_is_true_(bool is_first) const{
	return (runtime::object::current_exception == nullptr);
}
