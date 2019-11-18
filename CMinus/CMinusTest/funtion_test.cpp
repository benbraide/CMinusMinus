#include "pch.h"

#include "../CMinus/declaration/declaration_exception.h"
#include "../CMinus/declaration/variable_declaration.h"

#include "../CMinus/type/string_type.h"
#include "../CMinus/storage/global_storage.h"

class FunctionTest : public testing::Test{
protected:
	FunctionTest(){
		memory_object_ = std::make_shared<cminus::memory::object>();
		cminus::runtime::object::memory_object = memory_object_.get();

		global_storage_ = std::make_shared<cminus::storage::global>();
	}

	std::shared_ptr<cminus::memory::object> memory_object_;
	std::shared_ptr<cminus::storage::global> global_storage_;
};

TEST_F(FunctionTest, TestConstruction){
	auto var0 = std::make_shared<cminus::declaration::variable>(
		"",																	//Name
		cminus::runtime::object::global_storage->get_string_type(),			//Type
		cminus::attribute::collection::list_type{},							//Attributes
		cminus::declaration::flags::nil,									//Flags
		std::shared_ptr<cminus::node::object>()								//Initialization
	);

	auto str0 = var0->evaluate(0u);
	ASSERT_NE(str0, nullptr);
	EXPECT_TRUE(str0->is_constructed());

	EXPECT_STREQ(cminus::runtime::object::global_storage->get_string_value(str0).data(), "");
	str0->get_type()->destruct(str0);

	auto str1 = cminus::runtime::object::global_storage->create_string("TestString");
	ASSERT_NE(str1, nullptr);
	EXPECT_TRUE(str1->is_constructed());

	EXPECT_STREQ(cminus::runtime::object::global_storage->get_string_value(str1).data(), "TestString");

	auto str2 = std::make_shared<cminus::memory::rval_reference>(cminus::runtime::object::global_storage->get_string_type());
	ASSERT_NE(str2, nullptr);
	EXPECT_FALSE(str2->is_constructed());

	str2->get_type()->construct(str2, str1);
	str2->set_constructed_state();
	EXPECT_TRUE(str2->is_constructed());

	EXPECT_STREQ(cminus::runtime::object::global_storage->get_string_value(str2).data(), "TestString");
	str2->get_type()->destruct(str2);
	str1->get_type()->destruct(str1);
}
