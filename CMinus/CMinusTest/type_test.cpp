#include "pch.h"

#include "../CMinus/runtime/runtime_exception.cpp"
#include "../CMinus/storage/storage_exception.cpp"
#include "../CMinus/declaration/declaration_exception.cpp"
#include "../CMinus/evaluator/evaluator_exception.cpp"

#include "../CMinus/attribute/attribute_object.cpp"
#include "../CMinus/attribute/attribute_collection.cpp"

#include "../CMinus/operators/operator_conversions.cpp"
#include "../CMinus/memory/memory_reference.cpp"

#include "../CMinus/type/type_object.cpp"
#include "../CMinus/type/class_type.cpp"
#include "../CMinus/type/enum_type.cpp"
#include "../CMinus/type/function_type.cpp"
#include "../CMinus/type/iterator_type.cpp"
#include "../CMinus/type/modified_type.cpp"
#include "../CMinus/type/pointer_type.cpp"
#include "../CMinus/type/primitive_type.cpp"
#include "../CMinus/type/proxy_type.cpp"
#include "../CMinus/type/string_type.cpp"
#include "../CMinus/type/system_enum_types.cpp"
#include "../CMinus/type/variadic_type.cpp"

#include "../CMinus/storage/storage_object.cpp"
#include "../CMinus/storage/global_storage.cpp"
#include "../CMinus/storage/specialized_storage.cpp"

#include "../CMinus/evaluator/evaluator_object.cpp"
#include "../CMinus/evaluator/arithmetic.cpp"
#include "../CMinus/evaluator/assignment.cpp"
#include "../CMinus/evaluator/boolean_evaluator.cpp"
#include "../CMinus/evaluator/byte_evaluator.cpp"
#include "../CMinus/evaluator/comparison.cpp"
#include "../CMinus/evaluator/compound_assignment.cpp"
#include "../CMinus/evaluator/enum_evaluator.cpp"
#include "../CMinus/evaluator/explicit_comparison.cpp"
#include "../CMinus/evaluator/initializer.cpp"
#include "../CMinus/evaluator/number_evaluator.cpp"
#include "../CMinus/evaluator/pointer_evaluator.cpp"
#include "../CMinus/evaluator/list_evaluator.cpp"
#include "../CMinus/evaluator/type_evaluator.cpp"
#include "../CMinus/evaluator/class_evaluator.cpp"
#include "../CMinus/evaluator/string_evaluator.cpp"

#include "../CMinus/declaration/declaration_object.cpp"
#include "../CMinus/declaration/callable_declaration.cpp"
#include "../CMinus/declaration/callable_declaration_group.cpp"
#include "../CMinus/declaration/function_declaration.cpp"
#include "../CMinus/declaration/function_declaration_group.cpp"
#include "../CMinus/declaration/special_function_declaration.cpp"
#include "../CMinus/declaration/string_function_declarations.cpp"
#include "../CMinus/declaration/string_function_definitions.cpp"
#include "../CMinus/declaration/variable_declaration.cpp"

#include "../CMinus/node/node_object.cpp"
#include "../CMinus/node/identifier_node.cpp"
#include "../CMinus/node/literal_node.h"
#include "../CMinus/node/member_access_node.cpp"
#include "../CMinus/node/memory_reference_node.cpp"

class TypeTest : public testing::Test{
protected:
	TypeTest(){
		memory_object_ = std::make_shared<cminus::memory::object>();
		cminus::runtime::object::memory_object = memory_object_.get();

		global_storage_ = std::make_shared<cminus::storage::global>();
	}

	std::shared_ptr<cminus::memory::object> memory_object_;
	std::shared_ptr<cminus::storage::global> global_storage_;
};

TEST_F(TypeTest, TestName){
	EXPECT_STREQ(global_storage_->get_cached_type(cminus::storage::global::cached_type::undefined)->get_name().data(), "UndefinedType");
	EXPECT_STREQ(global_storage_->get_cached_type(cminus::storage::global::cached_type::void_)->get_name().data(), "Void");
	EXPECT_STREQ(global_storage_->get_cached_type(cminus::storage::global::cached_type::auto_)->get_name().data(), "Auto");

	EXPECT_STREQ(global_storage_->get_cached_type(cminus::storage::global::cached_type::bool_)->get_name().data(), "Boolean");
	EXPECT_STREQ(global_storage_->get_cached_type(cminus::storage::global::cached_type::byte_)->get_name().data(), "Byte");
	EXPECT_STREQ(global_storage_->get_cached_type(cminus::storage::global::cached_type::char_)->get_name().data(), "Char");
	EXPECT_STREQ(global_storage_->get_cached_type(cminus::storage::global::cached_type::wchar_)->get_name().data(), "WideChar");

	EXPECT_STREQ(global_storage_->get_cached_type(cminus::storage::global::cached_type::small_number)->get_name().data(), "SmallNumber");
	EXPECT_STREQ(global_storage_->get_cached_type(cminus::storage::global::cached_type::number)->get_name().data(), "Number");
	EXPECT_STREQ(global_storage_->get_cached_type(cminus::storage::global::cached_type::big_number)->get_name().data(), "BigNumber");

	EXPECT_STREQ(global_storage_->get_cached_type(cminus::storage::global::cached_type::small_integer)->get_name().data(), "SmallInteger");
	EXPECT_STREQ(global_storage_->get_cached_type(cminus::storage::global::cached_type::integer)->get_name().data(), "Integer");
	EXPECT_STREQ(global_storage_->get_cached_type(cminus::storage::global::cached_type::big_integer)->get_name().data(), "BigInteger");

	EXPECT_STREQ(global_storage_->get_cached_type(cminus::storage::global::cached_type::unsigned_small_integer)->get_name().data(), "UnsignedSmallInteger");
	EXPECT_STREQ(global_storage_->get_cached_type(cminus::storage::global::cached_type::unsigned_integer)->get_name().data(), "UnsignedInteger");
	EXPECT_STREQ(global_storage_->get_cached_type(cminus::storage::global::cached_type::unsigned_big_integer)->get_name().data(), "UnsignedBigInteger");

	EXPECT_STREQ(global_storage_->get_cached_type(cminus::storage::global::cached_type::small_float)->get_name().data(), "SmallFloat");
	EXPECT_STREQ(global_storage_->get_cached_type(cminus::storage::global::cached_type::float_)->get_name().data(), "Float");
	EXPECT_STREQ(global_storage_->get_cached_type(cminus::storage::global::cached_type::big_float)->get_name().data(), "BigFloat");

	EXPECT_STREQ(global_storage_->get_cached_type(cminus::storage::global::cached_type::function)->get_name().data(), "FunctionType");
	EXPECT_STREQ(global_storage_->get_cached_type(cminus::storage::global::cached_type::nullptr_)->get_name().data(), "NullptrType");
	EXPECT_STREQ(global_storage_->get_cached_type(cminus::storage::global::cached_type::string)->get_name().data(), "String");

	auto char_ref_type = global_storage_->get_char_ref_type(false);
	EXPECT_STREQ(char_ref_type->get_name().data(), "Char&");

	auto char_const_ref_type = global_storage_->get_char_ref_type(true);
	EXPECT_STREQ(char_const_ref_type->get_name().data(), "Const Char&");

	auto char_ptr_type = global_storage_->get_pointer_type(global_storage_->get_char_type(), false);
	EXPECT_STREQ(char_ptr_type->get_name().data(), "Char*");

	auto char_const_ptr_type = global_storage_->get_pointer_type(global_storage_->get_char_type(), true);
	EXPECT_STREQ(char_const_ptr_type->get_name().data(), "Const Char*");

	auto char_ptr_const_type = std::make_shared<cminus::type::constant>(char_ptr_type);
	EXPECT_STREQ(char_ptr_const_type->get_name().data(), "Char*Const");

	auto char_ptr_ref_type = global_storage_->get_ref_type(char_ptr_type, false);
	EXPECT_STREQ(char_ptr_ref_type->get_name().data(), "Char*&");

	auto char_ptr_const_ref_type = global_storage_->get_ref_type(char_ptr_type, true);
	EXPECT_STREQ(char_ptr_const_ref_type->get_name().data(), "Char*&Const");

	auto char_ncr_char_const_ptr_type = char_const_ptr_type->remove_const_ref();
	EXPECT_STREQ(char_ncr_char_const_ptr_type->get_name().data(), "Const Char*");
}

TEST_F(TypeTest, TestProperties){
	EXPECT_FALSE(global_storage_->get_cached_type(cminus::storage::global::cached_type::void_)->is_inferred());
	EXPECT_TRUE(global_storage_->get_cached_type(cminus::storage::global::cached_type::auto_)->is_inferred());
	EXPECT_TRUE(global_storage_->get_cached_type(cminus::storage::global::cached_type::function)->is_inferred());
	EXPECT_TRUE(global_storage_->get_cached_type(cminus::storage::global::cached_type::small_number)->is_inferred());
	EXPECT_TRUE(global_storage_->get_cached_type(cminus::storage::global::cached_type::number)->is_inferred());
	EXPECT_TRUE(global_storage_->get_cached_type(cminus::storage::global::cached_type::big_number)->is_inferred());

	EXPECT_FALSE(global_storage_->get_pointer_type(cminus::storage::global::cached_type::void_, false)->is_inferred());
	EXPECT_TRUE(global_storage_->get_pointer_type(cminus::storage::global::cached_type::auto_, false)->is_inferred());
	EXPECT_TRUE(global_storage_->get_pointer_type(cminus::storage::global::cached_type::function, false)->is_inferred());
	EXPECT_TRUE(global_storage_->get_pointer_type(cminus::storage::global::cached_type::small_number, false)->is_inferred());
	EXPECT_TRUE(global_storage_->get_pointer_type(cminus::storage::global::cached_type::number, false)->is_inferred());
	EXPECT_TRUE(global_storage_->get_pointer_type(cminus::storage::global::cached_type::big_number, false)->is_inferred());

	EXPECT_FALSE(global_storage_->get_pointer_type(cminus::storage::global::cached_type::void_, true)->is_inferred());
	EXPECT_TRUE(global_storage_->get_pointer_type(cminus::storage::global::cached_type::auto_, true)->is_inferred());
	EXPECT_TRUE(global_storage_->get_pointer_type(cminus::storage::global::cached_type::function, true)->is_inferred());
	EXPECT_TRUE(global_storage_->get_pointer_type(cminus::storage::global::cached_type::small_number, true)->is_inferred());
	EXPECT_TRUE(global_storage_->get_pointer_type(cminus::storage::global::cached_type::number, true)->is_inferred());
	EXPECT_TRUE(global_storage_->get_pointer_type(cminus::storage::global::cached_type::big_number, true)->is_inferred());

	EXPECT_FALSE(global_storage_->get_cached_type(cminus::storage::global::cached_type::char_)->is_const());
	EXPECT_FALSE(global_storage_->get_cached_type(cminus::storage::global::cached_type::char_)->is_ref());

	EXPECT_FALSE(global_storage_->get_pointer_type(cminus::storage::global::cached_type::char_, false)->is_const());
	EXPECT_FALSE(global_storage_->get_pointer_type(cminus::storage::global::cached_type::char_, false)->is_ref());

	EXPECT_FALSE(global_storage_->get_pointer_type(cminus::storage::global::cached_type::char_, true)->is_const());
	EXPECT_FALSE(global_storage_->get_pointer_type(cminus::storage::global::cached_type::char_, true)->is_ref());

	EXPECT_FALSE(global_storage_->get_ref_type(cminus::storage::global::cached_type::char_, false)->is_const());
	EXPECT_TRUE(global_storage_->get_ref_type(cminus::storage::global::cached_type::char_, false)->is_ref());

	EXPECT_TRUE(global_storage_->get_ref_type(cminus::storage::global::cached_type::char_, true)->is_const());
	EXPECT_TRUE(global_storage_->get_ref_type(cminus::storage::global::cached_type::char_, true)->is_ref());

	auto const_char_type = std::make_shared<cminus::type::constant>(global_storage_->get_cached_type(cminus::storage::global::cached_type::char_));
	EXPECT_TRUE(const_char_type->is_const());
	EXPECT_FALSE(const_char_type->is_ref());

	auto inferred0 = global_storage_->get_cached_type(cminus::storage::global::cached_type::auto_)->get_inferred(global_storage_->get_cached_type(cminus::storage::global::cached_type::char_));
	ASSERT_NE(inferred0, nullptr);
	EXPECT_FALSE(inferred0->is_inferred());

	auto inferred1 = global_storage_->get_cached_type(cminus::storage::global::cached_type::auto_)->get_inferred(global_storage_->get_pointer_type(cminus::storage::global::cached_type::char_, false));
	ASSERT_NE(inferred1, nullptr);
	EXPECT_FALSE(inferred1->is_inferred());

	auto inferred2 = global_storage_->get_cached_type(cminus::storage::global::cached_type::auto_)->get_inferred(global_storage_->get_pointer_type(cminus::storage::global::cached_type::char_, true));
	ASSERT_NE(inferred2, nullptr);
	EXPECT_FALSE(inferred2->is_inferred());

	auto inferred3 = global_storage_->get_cached_type(cminus::storage::global::cached_type::auto_)->get_inferred(global_storage_->get_ref_type(cminus::storage::global::cached_type::char_, false));
	ASSERT_NE(inferred3, nullptr);
	EXPECT_FALSE(inferred3->is_inferred());

	auto inferred4 = global_storage_->get_cached_type(cminus::storage::global::cached_type::auto_)->get_inferred(global_storage_->get_ref_type(cminus::storage::global::cached_type::char_, true));
	ASSERT_NE(inferred4, nullptr);
	EXPECT_FALSE(inferred4->is_inferred());
	EXPECT_FALSE(inferred4->is_const());
	EXPECT_FALSE(inferred4->is_ref());

	auto inferred5 = global_storage_->get_ref_type(cminus::storage::global::cached_type::auto_, false)->get_inferred(global_storage_->get_ref_type(cminus::storage::global::cached_type::char_, true));
	ASSERT_NE(inferred5, nullptr);
	EXPECT_FALSE(inferred5->is_inferred());
	EXPECT_TRUE(inferred5->is_const());
	EXPECT_TRUE(inferred5->is_ref());

	auto inferred6 = global_storage_->get_cached_type(cminus::storage::global::cached_type::number)->get_inferred(global_storage_->get_cached_type(cminus::storage::global::cached_type::integer));
	ASSERT_NE(inferred6, nullptr);
	EXPECT_FALSE(inferred6->is_inferred());

	auto inferred7 = global_storage_->get_cached_type(cminus::storage::global::cached_type::number)->get_inferred(global_storage_->get_cached_type(cminus::storage::global::cached_type::small_float));
	ASSERT_NE(inferred7, nullptr);
	EXPECT_FALSE(inferred7->is_inferred());

	auto inferred8 = global_storage_->get_cached_type(cminus::storage::global::cached_type::number)->get_inferred(global_storage_->get_cached_type(cminus::storage::global::cached_type::char_));
	EXPECT_EQ(inferred8, nullptr);

	auto inferred9 = global_storage_->get_cached_type(cminus::storage::global::cached_type::small_number)->get_inferred(global_storage_->get_cached_type(cminus::storage::global::cached_type::big_integer));
	EXPECT_EQ(inferred9, nullptr);

	auto inferred10 = global_storage_->get_auto_pointer_type(false)->get_inferred(global_storage_->get_char_pointer_type(false));
	ASSERT_NE(inferred10, nullptr);
	EXPECT_FALSE(inferred10->is_inferred());

	auto inferred11 = global_storage_->get_auto_pointer_type(false)->get_inferred(global_storage_->get_char_type());
	ASSERT_EQ(inferred11, nullptr);

	EXPECT_TRUE(global_storage_->get_cached_type(cminus::storage::global::cached_type::auto_)->is_exact(*global_storage_->get_cached_type(cminus::storage::global::cached_type::auto_)));
	EXPECT_FALSE(global_storage_->get_cached_type(cminus::storage::global::cached_type::bool_)->is_exact(*global_storage_->get_cached_type(cminus::storage::global::cached_type::auto_)));

	EXPECT_FALSE(global_storage_->get_ref_type(cminus::storage::global::cached_type::auto_, false)->is_exact(*global_storage_->get_cached_type(cminus::storage::global::cached_type::auto_)));
	EXPECT_FALSE(global_storage_->get_ref_type(cminus::storage::global::cached_type::auto_, true)->is_exact(*global_storage_->get_cached_type(cminus::storage::global::cached_type::auto_)));

	EXPECT_FALSE(global_storage_->get_pointer_type(cminus::storage::global::cached_type::auto_, false)->is_exact(*global_storage_->get_cached_type(cminus::storage::global::cached_type::auto_)));
	EXPECT_FALSE(global_storage_->get_pointer_type(cminus::storage::global::cached_type::auto_, true)->is_exact(*global_storage_->get_cached_type(cminus::storage::global::cached_type::auto_)));
}

TEST_F(TypeTest, TestScore){
	auto score = global_storage_->get_cached_type(cminus::storage::global::cached_type::auto_)->get_score(
		*global_storage_->get_cached_type(cminus::storage::global::cached_type::auto_),
		false,
		false
	);
	EXPECT_EQ(score, cminus::type::object::get_score_value(cminus::type::object::score_result_type::nil));

	score = global_storage_->get_cached_type(cminus::storage::global::cached_type::integer)->get_score(
		*global_storage_->get_cached_type(cminus::storage::global::cached_type::auto_),
		false,
		false
	);
	EXPECT_EQ(score, cminus::type::object::get_score_value(cminus::type::object::score_result_type::inferable));

	score = global_storage_->get_cached_type(cminus::storage::global::cached_type::integer)->get_score(
		*global_storage_->get_cached_type(cminus::storage::global::cached_type::auto_),
		false,
		true
	);
	EXPECT_EQ(score, cminus::type::object::get_score_value(cminus::type::object::score_result_type::inferable, -1));

	score = global_storage_->get_cached_type(cminus::storage::global::cached_type::integer)->get_score(
		*global_storage_->get_cached_type(cminus::storage::global::cached_type::auto_),
		true,
		false
	);
	EXPECT_EQ(score, cminus::type::object::get_score_value(cminus::type::object::score_result_type::inferable));

	score = global_storage_->get_cached_type(cminus::storage::global::cached_type::integer)->get_score(
		*global_storage_->get_ref_type(cminus::storage::global::cached_type::auto_, false),
		false,
		false
	);
	EXPECT_EQ(score, cminus::type::object::get_score_value(cminus::type::object::score_result_type::nil));

	score = global_storage_->get_cached_type(cminus::storage::global::cached_type::integer)->get_score(
		*global_storage_->get_ref_type(cminus::storage::global::cached_type::auto_, true),
		false,
		false
	);
	EXPECT_EQ(score, cminus::type::object::get_score_value(cminus::type::object::score_result_type::inferable, -1));

	score = global_storage_->get_cached_type(cminus::storage::global::cached_type::integer)->get_score(
		*global_storage_->get_ref_type(cminus::storage::global::cached_type::auto_, false),
		true,
		false
	);
	EXPECT_EQ(score, cminus::type::object::get_score_value(cminus::type::object::score_result_type::inferable));

	score = global_storage_->get_cached_type(cminus::storage::global::cached_type::integer)->get_score(
		*global_storage_->get_ref_type(cminus::storage::global::cached_type::auto_, false),
		true,
		true
	);
	EXPECT_EQ(score, cminus::type::object::get_score_value(cminus::type::object::score_result_type::nil));

	score = global_storage_->get_cached_type(cminus::storage::global::cached_type::integer)->get_score(
		*global_storage_->get_ref_type(cminus::storage::global::cached_type::auto_, true),
		true,
		true
	);
	EXPECT_EQ(score, cminus::type::object::get_score_value(cminus::type::object::score_result_type::inferable));

	score = global_storage_->get_cached_type(cminus::storage::global::cached_type::integer)->get_score(
		*global_storage_->get_cached_type(cminus::storage::global::cached_type::integer),
		false,
		false
	);
	EXPECT_EQ(score, cminus::type::object::get_score_value(cminus::type::object::score_result_type::exact));

	score = global_storage_->get_cached_type(cminus::storage::global::cached_type::integer)->get_score(
		*global_storage_->get_cached_type(cminus::storage::global::cached_type::big_float),
		false,
		false
	);
	EXPECT_EQ(score, cminus::type::object::get_score_value(cminus::type::object::score_result_type::assignable));

	score = global_storage_->get_cached_type(cminus::storage::global::cached_type::integer)->get_score(
		*global_storage_->get_cached_type(cminus::storage::global::cached_type::string),
		false,
		false
	);
	EXPECT_EQ(score, cminus::type::object::get_score_value(cminus::type::object::score_result_type::class_compatible));

	score = global_storage_->get_pointer_type(cminus::storage::global::cached_type::integer, false)->get_score(
		*global_storage_->get_cached_type(cminus::storage::global::cached_type::string),
		false,
		false
	);
	EXPECT_EQ(score, cminus::type::object::get_score_value(cminus::type::object::score_result_type::nil));

	score = global_storage_->get_pointer_type(cminus::storage::global::cached_type::integer, true)->get_score(
		*global_storage_->get_cached_type(cminus::storage::global::cached_type::string),
		false,
		false
	);
	EXPECT_EQ(score, cminus::type::object::get_score_value(cminus::type::object::score_result_type::nil));

	score = global_storage_->get_pointer_type(cminus::storage::global::cached_type::integer, false)->get_score(
		*global_storage_->get_pointer_type(cminus::storage::global::cached_type::string, false),
		false,
		false
	);
	EXPECT_EQ(score, cminus::type::object::get_score_value(cminus::type::object::score_result_type::nil));

	score = global_storage_->get_pointer_type(cminus::storage::global::cached_type::integer, false)->get_score(
		*global_storage_->get_pointer_type(cminus::storage::global::cached_type::void_, false),
		false,
		false
	);
	EXPECT_EQ(score, cminus::type::object::get_score_value(cminus::type::object::score_result_type::assignable));

	score = global_storage_->get_pointer_type(cminus::storage::global::cached_type::void_, false)->get_score(
		*global_storage_->get_pointer_type(cminus::storage::global::cached_type::integer, false),
		false,
		false
	);
	EXPECT_EQ(score, cminus::type::object::get_score_value(cminus::type::object::score_result_type::nil));

	score = global_storage_->get_pointer_type(cminus::storage::global::cached_type::small_float, false)->get_score(
		*global_storage_->get_pointer_type(cminus::storage::global::cached_type::integer, false),
		false,
		false
	);
	EXPECT_EQ(score, cminus::type::object::get_score_value(cminus::type::object::score_result_type::nil));

	score = global_storage_->get_pointer_type(cminus::storage::global::cached_type::integer, false)->get_score(
		*global_storage_->get_pointer_type(cminus::storage::global::cached_type::integer, false),
		false,
		false
	);
	EXPECT_EQ(score, cminus::type::object::get_score_value(cminus::type::object::score_result_type::exact));

	score = global_storage_->get_pointer_type(cminus::storage::global::cached_type::integer, true)->get_score(
		*global_storage_->get_pointer_type(cminus::storage::global::cached_type::integer, false),
		false,
		false
	);
	EXPECT_EQ(score, cminus::type::object::get_score_value(cminus::type::object::score_result_type::nil));

	score = global_storage_->get_pointer_type(cminus::storage::global::cached_type::integer, false)->get_score(
		*global_storage_->get_pointer_type(cminus::storage::global::cached_type::integer, true),
		false,
		false
	);
	EXPECT_EQ(score, cminus::type::object::get_score_value(cminus::type::object::score_result_type::exact, -1));
}
