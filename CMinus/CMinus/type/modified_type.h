#pragma once

#include "type_object.h"

namespace cminus::type{
	class modified : public object{
	public:
		modified(const std::string &name, std::shared_ptr<object> base_type);

		virtual ~modified();

		virtual std::size_t get_size() const override;

		virtual std::shared_ptr<memory::reference> cast(std::shared_ptr<memory::reference> data, std::shared_ptr<object> target_type, cast_type type) const override;

		virtual bool is(query_type type, const object *arg = nullptr) const override;

	protected:
		std::shared_ptr<object> base_type_;
	};

	class constant : public modified{
	public:
		explicit constant(std::shared_ptr<object> base_type);

		virtual ~constant();

		virtual std::string get_qname() const override;

		virtual bool is_constructible(std::shared_ptr<memory::reference> target) const override;

		virtual bool is_exact(const object &target) const override;

		virtual int get_score(const object &target) const override;

		virtual std::shared_ptr<object> convert(conversion_type type, std::shared_ptr<object> self_or_other = nullptr) const override;

		virtual bool is(query_type type, const object *arg = nullptr) const override;
	};

	class ref : public modified{
	public:
		explicit ref(std::shared_ptr<object> base_type);

		virtual ~ref();

		virtual std::string get_qname() const override;

		virtual bool is_exact(const object &target) const override;

		virtual int get_score(const object &target) const override;

		virtual std::shared_ptr<object> convert(conversion_type type, std::shared_ptr<object> self_or_other = nullptr) const override;

		virtual bool is(query_type type, const object *arg = nullptr) const override;
	};
}
