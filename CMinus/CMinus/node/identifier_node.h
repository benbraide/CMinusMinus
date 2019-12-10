#pragma once

#include "node_object.h"

namespace cminus::node{
	class identifier : public object{
	public:
		explicit identifier(const std::string &value);

		virtual ~identifier();

		virtual std::shared_ptr<memory::reference> evaluate() const override;

		virtual std::shared_ptr<memory::reference> lookup(std::shared_ptr<memory::reference> context, bool search_tree) const override;

		virtual std::string evaluate_as_name() const override;

		virtual std::shared_ptr<type::object> evaluate_as_type(bool search_tree) const override;

		virtual storage::object *evaluate_as_storage(bool search_tree) const override;

	protected:
		virtual storage::object *get_storage_(bool search_tree) const;

		virtual bool should_search_tree_() const;

		std::string value_;
	};

	class scoped_identifier : public identifier{
	public:
		scoped_identifier(std::shared_ptr<object> left, const std::string &value);

		virtual ~scoped_identifier();

		virtual std::string evaluate_as_name() const override;

	protected:
		virtual storage::object *get_storage_(bool search_tree) const override;

		virtual bool should_search_tree_() const override;

		std::shared_ptr<object> left_;
	};

	class primtive_type : public object{
	public:
		enum class id_type{
			nil,
			undefined,
			auto_,
			void_,
			bool_,
			byte_,
			char_,
			wchar_,
			small_number,
			number,
			big_number,
			small_integer,
			unsigned_small_integer,
			integer,
			unsigned_integer,
			big_integer,
			unsigned_big_integer,
			small_float,
			float_,
			big_float,
			function,
			nullptr_,
			string,
			exception,
			attribute_stage,
			attribute_result,
			type,
			storage,
		};

		explicit primtive_type(id_type id);

		virtual ~primtive_type();

		virtual std::shared_ptr<memory::reference> evaluate() const override;

		virtual std::shared_ptr<type::object> evaluate_as_type(bool search_tree) const override;

		virtual storage::object *evaluate_as_storage(bool search_tree) const override;

		virtual id_type get_id() const;

	protected:
		id_type id_;
	};

	class modified_type : public object{
	public:
		explicit modified_type(std::shared_ptr<object> value);

		virtual ~modified_type();

		virtual std::shared_ptr<memory::reference> evaluate() const override;

		virtual std::shared_ptr<type::object> evaluate_as_type(bool search_tree) const override;

		virtual std::shared_ptr<object> get_value() const;

	protected:
		virtual std::shared_ptr<type::object> evaluate_as_type_(std::shared_ptr<type::object> value) const = 0;

		std::shared_ptr<object> value_;
	};

	class pointer_type : public modified_type{
	public:
		explicit pointer_type(std::shared_ptr<object> value, std::size_t magnitude = 1u);

		virtual ~pointer_type();

		virtual std::size_t get_magnitude() const;

	protected:
		virtual std::shared_ptr<type::object> evaluate_as_type_(std::shared_ptr<type::object> value) const override;

		std::size_t magnitude_;
	};

	class const_type : public modified_type{
	public:
		using modified_type::modified_type;

		virtual ~const_type();

	protected:
		virtual std::shared_ptr<type::object> evaluate_as_type_(std::shared_ptr<type::object> value) const override;
	};

	class ref_type : public modified_type{
	public:
		using modified_type::modified_type;

		virtual ~ref_type();

	protected:
		virtual std::shared_ptr<type::object> evaluate_as_type_(std::shared_ptr<type::object> value) const override;
	};

	class typename_ : public modified_type{
	public:
		using modified_type::modified_type;

		virtual ~typename_();

	protected:
		virtual std::shared_ptr<type::object> evaluate_as_type_(std::shared_ptr<type::object> value) const override;
	};
}
