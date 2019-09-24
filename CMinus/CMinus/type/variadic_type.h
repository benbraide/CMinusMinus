#pragma once

#include "type_object.h"

namespace cminus::type{
	class variadic : public object{
	public:
		explicit variadic(std::shared_ptr<object> base_type);

		virtual ~variadic();

		virtual std::size_t get_size() const override;

		virtual bool is_exact(const object &target) const override;

		virtual int get_score(const object &target) const override;

		virtual std::shared_ptr<memory::reference> cast(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type, cast_type type) const override;

		virtual bool is(query_type type, const object *arg = nullptr) const override;

		virtual std::shared_ptr<object> get_base_type() const;

	protected:
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
		virtual void construct_(std::shared_ptr<memory::reference> target, const std::list<std::shared_ptr<memory::reference>> &args) const override;

		std::size_t count_;
	};

	class expansion_variadic : public in_memory_variadic{
	public:
		using in_memory_variadic::in_memory_variadic;

		virtual ~expansion_variadic();

		virtual void extend_argument_list(std::shared_ptr<memory::reference> data, std::list<std::shared_ptr<memory::reference>> &list) const override;
	};
}
