#pragma once

#include "function_declaration.h"

namespace cminus::declaration::core{
	class core_function : public external_function{
	public:
		core_function(const std::string &name, storage::object &parent, std::shared_ptr<type::object> return_type);

		virtual ~core_function();
	};

	class heap_alloc : public core_function{
	public:
		explicit heap_alloc(storage::object &parent);

		virtual ~heap_alloc();

	protected:
		virtual void evaluate_body_() const override;
	};

	class heap_dealloc : public core_function{
	public:
		explicit heap_dealloc(storage::object &parent);

		virtual ~heap_dealloc();

	protected:
		virtual void evaluate_body_() const override;
	};

	class heap_realloc : public core_function{
	public:
		explicit heap_realloc(storage::object &parent);

		virtual ~heap_realloc();

	protected:
		virtual void evaluate_body_() const override;
	};

	class is_heap : public core_function{
	public:
		explicit is_heap(storage::object &parent);

		virtual ~is_heap();

	protected:
		virtual void evaluate_body_() const override;
	};

	class begin : public core_function{
	public:
		begin(storage::object &parent, bool is_constant_target);

		virtual ~begin();

	protected:
		virtual void evaluate_body_() const override;
	};

	class rbegin : public core_function{
	public:
		rbegin(storage::object &parent, bool is_constant_target);

		virtual ~rbegin();

	protected:
		virtual void evaluate_body_() const override;
	};

	class end : public core_function{
	public:
		end(storage::object &parent, bool is_constant_target);

		virtual ~end();

	protected:
		virtual void evaluate_body_() const override;
	};

	class rend : public core_function{
	public:
		rend(storage::object &parent, bool is_constant_target);

		virtual ~rend();

	protected:
		virtual void evaluate_body_() const override;
	};
}
