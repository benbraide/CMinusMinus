#pragma once

#include "../storage/specialized_storage.h"

#include "node_object.h"

namespace cminus::node{
	class variable_declaration;

	class control : public object{
	public:
		control(std::shared_ptr<object> condition, std::shared_ptr<object> statement);

		virtual ~control();

		virtual std::shared_ptr<memory::reference> evaluate() const override;

	protected:
		virtual bool is_iteration_() const;

		virtual void init_() const;

		virtual bool condition_is_true_(bool is_first) const;

		virtual std::shared_ptr<memory::reference> evaluate_() const;

		virtual void update_() const;

		virtual void uninit_() const;

		virtual std::shared_ptr<memory::reference> else_(storage::control &store) const;

		std::shared_ptr<object> condition_;
		std::shared_ptr<object> statement_;
	};

	class control_with_else : public control{
	public:
		control_with_else(std::shared_ptr<object> condition, std::shared_ptr<object> statement, std::shared_ptr<object> else_value);

		virtual ~control_with_else();

	protected:
		virtual std::shared_ptr<memory::reference> else_(storage::control &store) const override;

		std::shared_ptr<object> else_value_;
	};

	class if_control : public control_with_else{
	public:
		if_control(std::shared_ptr<object> init_value, std::shared_ptr<object> condition, std::shared_ptr<object> statement, std::shared_ptr<object> else_value);

		virtual ~if_control();

	protected:
		virtual void init_() const override;

		std::shared_ptr<object> init_value_;
	};

	class while_control : public control_with_else{
	public:
		while_control(std::shared_ptr<object> init_value, std::shared_ptr<object> condition, std::shared_ptr<object> statement, std::shared_ptr<object> else_value);

		virtual ~while_control();

	protected:
		virtual bool is_iteration_() const override;

		virtual void init_() const override;

		std::shared_ptr<object> init_value_;
	};

	class do_while_control : public while_control{
	public:
		do_while_control(std::shared_ptr<object> condition, std::shared_ptr<object> statement);

		virtual ~do_while_control();

	protected:
		virtual bool condition_is_true_(bool is_first) const override;

		std::shared_ptr<object> init_value_;
	};

	class for_control : public while_control{
	public:
		for_control(std::shared_ptr<object> init_value, std::shared_ptr<object> update_value, std::shared_ptr<object> condition, std::shared_ptr<object> statement, std::shared_ptr<object> else_value);

		virtual ~for_control();

	protected:
		virtual void update_() const override;

		std::shared_ptr<object> update_value_;
	};

	class for_each_control : public for_control{
	public:
		for_each_control(std::shared_ptr<variable_declaration> decl, std::shared_ptr<object> target, std::shared_ptr<object> statement, std::shared_ptr<object> else_value);

		virtual ~for_each_control();
	};

	class try_control : public control_with_else{
	public:
		try_control(std::shared_ptr<object> statement, std::shared_ptr<object> catch_value);

		virtual ~try_control();

	protected:
		virtual std::shared_ptr<memory::reference> evaluate_() const override;

		virtual void handle_(std::shared_ptr<memory::reference> e) const;
	};

	class catch_control : public control_with_else{
	public:
		catch_control(std::shared_ptr<object> condition, std::shared_ptr<object> statement, std::shared_ptr<object> catch_value);

		virtual ~catch_control();

	protected:
		virtual bool condition_is_true_(bool is_first) const override;

		virtual std::shared_ptr<memory::reference> evaluate_() const override;
	};

	class finally_control : public control{
	public:
		explicit finally_control(std::shared_ptr<object> statement);

		virtual ~finally_control();

	protected:
		virtual bool condition_is_true_(bool is_first) const override;
	};
}
