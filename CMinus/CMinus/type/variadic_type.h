#pragma once

#include "type_object.h"

namespace cminus::type{
	class variadic : public object{
	public:
		explicit variadic(std::shared_ptr<object> base_type);

		virtual ~variadic();

		virtual std::size_t get_size() const override;

		virtual void print_value(io::writer &writer, std::shared_ptr<memory::reference> data) const override;

		virtual int get_score(const object &target, bool is_lval, bool is_const) const override;

		virtual std::shared_ptr<memory::reference> cast(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type, cast_type type) const override;

		virtual std::shared_ptr<object> get_base_type() const;

	protected:
		virtual bool is_exact_(const object &target) const override;

		std::shared_ptr<object> base_type_;
	};

	class in_memory_variadic : public variadic{
	public:
		in_memory_variadic(std::shared_ptr<object> base_type, std::size_t count);

		virtual ~in_memory_variadic();

		virtual void destruct(std::shared_ptr<memory::reference> target) const override;

		virtual std::size_t get_size() const override;

		virtual std::size_t get_memory_size() const override;

		virtual std::size_t get_count() const;

		virtual std::shared_ptr<memory::reference> get_indexed(std::shared_ptr<memory::reference> data, std::size_t index) const;

	protected:
		virtual void construct_(std::shared_ptr<memory::reference> target, const std::vector<std::shared_ptr<memory::reference>> &args) const override;

		std::size_t count_;
	};

	class expansion_variadic : public in_memory_variadic{
	public:
		using in_memory_variadic::in_memory_variadic;

		virtual ~expansion_variadic();

		virtual void extend_argument_list(std::shared_ptr<memory::reference> data, std::vector<std::shared_ptr<memory::reference>> &list) const;

		virtual void print_value(io::writer &writer, std::shared_ptr<memory::reference> data) const override;
	};
}
