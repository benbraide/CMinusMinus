#pragma once

#include "type_object.h"

namespace cminus::type{
	class modified : public object{
	public:
		modified(const std::string &name, std::shared_ptr<object> base_type);

		virtual ~modified();

		virtual void print_value(io::writer &writer, std::shared_ptr<memory::reference> data) const override;

		virtual std::size_t get_size() const override;

		virtual int get_score(const object &target, bool is_lval, bool is_const) const override;

		virtual std::shared_ptr<memory::reference> cast(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type, cast_type type) const override;

		virtual const object *remove_const_ref() const;

		virtual std::shared_ptr<object> remove_const_ref(std::shared_ptr<object> self) const;

		virtual bool can_be_inferred_from(const object &target) const override;

		virtual bool is_inferred() const override;

		virtual bool is_const() const override;

		virtual bool is_ref() const override;

	protected:
		std::shared_ptr<object> base_type_;
	};

	class constant : public modified{
	public:
		explicit constant(std::shared_ptr<object> base_type);

		virtual ~constant();

		virtual std::string get_qname() const override;

		virtual bool is_exact(const object &target) const override;

		virtual std::shared_ptr<object> get_inferred(std::shared_ptr<object> target) const override;

		virtual bool is_const() const override;
	};

	class ref : public modified{
	public:
		explicit ref(std::shared_ptr<object> base_type);

		virtual ~ref();

		virtual std::string get_qname() const override;

		virtual bool is_exact(const object &target) const override;

		virtual std::shared_ptr<object> get_inferred(std::shared_ptr<object> target) const override;

		virtual bool is_ref() const override;
	};
}
