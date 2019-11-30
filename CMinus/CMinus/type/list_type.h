#pragma once

#include "primitive_type.h"

namespace cminus::type{
	class array_primitive : public primitive{
	public:
		array_primitive(std::shared_ptr<object> base_type, std::size_t count);

		virtual ~array_primitive();

		virtual std::string get_qname() const override;

		virtual void print_value(io::writer &writer, std::shared_ptr<memory::reference> data) const override;

		virtual std::size_t get_size() const override;

		virtual std::size_t get_memory_size() const override;

		virtual bool is_exact(const object &target) const override;

		virtual std::shared_ptr<memory::reference> cast(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type, cast_type type) const override;

		virtual evaluator::object::id_type get_evaluator_id() const override;

		virtual std::shared_ptr<object> get_inferred(std::shared_ptr<object> target) const override;

		virtual bool can_be_iterated() const override;

		virtual bool can_be_inferred_from(const object &target) const override;

		virtual bool is_inferred() const override;

		virtual std::shared_ptr<object> get_base_type() const;

		virtual std::size_t get_count() const;

	protected:
		virtual int get_score_(const object &target, bool is_lval, bool is_const) const override;

		std::shared_ptr<object> base_type_;
		std::size_t count_;
	};

	class initializer_list_primitive : public primitive{
	public:
		initializer_list_primitive(std::shared_ptr<object> base_type, std::size_t count);

		virtual ~initializer_list_primitive();

		virtual std::string get_qname() const override;

		virtual void print_value(io::writer &writer, std::shared_ptr<memory::reference> data) const override;

		virtual std::size_t get_size() const override;

		virtual std::size_t get_memory_size() const override;

		virtual bool is_exact(const object &target) const override;

		virtual std::shared_ptr<memory::reference> cast(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type, cast_type type) const override;

		virtual std::shared_ptr<object> get_inferred(std::shared_ptr<object> target) const override;

		virtual bool can_be_inferred_from(const object &target) const override;

		virtual bool is_inferred() const override;

		virtual std::shared_ptr<object> get_base_type() const;

		virtual std::size_t get_count() const;

	protected:
		virtual int get_score_(const object &target, bool is_lval, bool is_const) const override;

		std::shared_ptr<object> base_type_;
		std::size_t count_;
	};
}
