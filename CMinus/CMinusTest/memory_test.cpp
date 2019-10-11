#include "pch.h"

#include "../CMinus/runtime/runtime_object.cpp"
#include "../CMinus/runtime/runtime_value_guard.h"

#include "../CMinus/memory/memory_exception.cpp"
#include "../CMinus/memory/memory_block.cpp"
#include "../CMinus/memory/memory_object.cpp"

class MemoryBlockTest : public testing::Test{
protected:
	MemoryBlockTest()
		: block_(9u, 16u){}

	cminus::memory::data_block<cminus::memory::block> block_;
};

TEST_F(MemoryBlockTest, TestState){
  EXPECT_EQ(block_.get_address(), 9);
  EXPECT_EQ(block_.get_size(), 16);
  EXPECT_NE(block_.get_data(), nullptr);

  EXPECT_FALSE(block_.is_access_protected());
  EXPECT_FALSE(block_.is_write_protected());
  EXPECT_FALSE(block_.is_resizable());
}

TEST_F(MemoryBlockTest, TestReadWrite){
	EXPECT_EQ(block_.write_scalar(18), sizeof(18));
	EXPECT_EQ(block_.read_scalar<int>(), 18);

	EXPECT_EQ(block_.write_buffer("test write"), std::strlen("test write"));
	EXPECT_EQ(block_.write_buffer("test write that should exceed 16 characters"), block_.get_size());
	EXPECT_EQ(block_.write_buffer(L"WIDE"), (std::strlen("WIDE") * 2));
	
	char buffer[32];
	EXPECT_EQ(block_.read_buffer(buffer, 8), 8);
	EXPECT_EQ(block_.read_buffer(buffer, 27), block_.get_size());
}

TEST_F(MemoryBlockTest, TestOffset){
	auto offset_block = block_.get_offset_block(8);

	EXPECT_EQ(offset_block->get_address(), (block_.get_address() + 8));
	EXPECT_EQ(offset_block->get_size(), (block_.get_size() - 8));
	
	EXPECT_NE(offset_block->get_data(), nullptr);
	EXPECT_EQ(offset_block->get_data(), (block_.get_data() + 8));

	EXPECT_EQ(offset_block->is_access_protected(), block_.is_access_protected());
	EXPECT_EQ(offset_block->is_write_protected(), block_.is_write_protected());
	EXPECT_EQ(offset_block->is_resizable(), block_.is_resizable());

	auto out_of_range_offset_block = block_.get_offset_block(32);
	EXPECT_EQ(out_of_range_offset_block, nullptr);
}

TEST(SpecialMemoryBlockTest, TestWriteProtected){
	cminus::memory::data_block<cminus::memory::write_protected_block> block(9, 4u);

	EXPECT_FALSE(block.is_access_protected());
	EXPECT_TRUE(block.is_write_protected());

	EXPECT_THROW(block.write_buffer("test write"), cminus::memory::exception::write_protected);
	
	{//Scoped
		cminus::runtime::value_guard guard(cminus::runtime::object::state, (cminus::runtime::object::state | cminus::runtime::flags::system));
		EXPECT_EQ(block.write_buffer("test write"), block.get_size());
	}

	char buffer[8];
	EXPECT_EQ(block.read_buffer(buffer, 4), 4);
}

TEST(SpecialMemoryBlockTest, TestAccessProtected){
	cminus::memory::data_block<cminus::memory::access_protected_block> block(9, 4u);

	EXPECT_TRUE(block.is_access_protected());
	EXPECT_FALSE(block.is_write_protected());

	EXPECT_EQ(block.write_buffer("test write"), block.get_size());

	char buffer[8];
	EXPECT_THROW(block.read_buffer(buffer, 4), cminus::memory::exception::access_protected);

	{//Scoped
		cminus::runtime::value_guard guard(cminus::runtime::object::state, (cminus::runtime::object::state | cminus::runtime::flags::system));
		EXPECT_EQ(block.read_buffer(buffer, 4), 4);
	}
}

TEST(SpecialMemoryBlockTest, TestProtected){
	cminus::memory::data_block<cminus::memory::protected_block> block(9, 4u);

	EXPECT_TRUE(block.is_access_protected());
	EXPECT_TRUE(block.is_write_protected());

	EXPECT_THROW(block.write_buffer("test write"), cminus::memory::exception::write_protected);

	char buffer[8];
	EXPECT_THROW(block.read_buffer(buffer, 4), cminus::memory::exception::access_protected);

	{//Scoped
		cminus::runtime::value_guard guard(cminus::runtime::object::state, (cminus::runtime::object::state | cminus::runtime::flags::system));
		EXPECT_EQ(block.write_buffer("test write"), block.get_size());
		EXPECT_EQ(block.read_buffer(buffer, 4), 4);
	}
}

TEST(SpecialMemoryBlockTest, TestFree){
	cminus::memory::free_block block(9, 4u);

	EXPECT_TRUE(block.is_access_protected());
	EXPECT_TRUE(block.is_write_protected());

	EXPECT_THROW(block.write_buffer("test write"), cminus::memory::exception::write_protected);

	char buffer[8];
	EXPECT_THROW(block.read_buffer(buffer, 4), cminus::memory::exception::access_protected);

	{//Scoped
		cminus::runtime::value_guard guard(cminus::runtime::object::state, (cminus::runtime::object::state | cminus::runtime::flags::system));
		EXPECT_THROW(block.write_buffer("test write"), cminus::memory::exception::write_protected);
		EXPECT_THROW(block.read_buffer(buffer, 4), cminus::memory::exception::access_protected);
	}
}

class MemoryObjectTest : public testing::Test{
protected:
	virtual void SetUp() override{
		initial_address_ = object_.get_next_address();
	}

	cminus::memory::object object_;
	std::size_t initial_address_ = 0;
};

TEST_F(MemoryObjectTest, TestAllocation){
	auto first_block = object_.allocate_block(8);

	ASSERT_NE(first_block, nullptr);

	EXPECT_FALSE(first_block->is_access_protected());
	EXPECT_FALSE(first_block->is_write_protected());
	EXPECT_FALSE(first_block->is_resizable());

	EXPECT_EQ(first_block->get_address(), initial_address_);
	EXPECT_EQ(first_block->get_size(), 8);
	EXPECT_EQ(object_.get_next_address(), (initial_address_ + first_block->get_size()));

	auto second_block = object_.allocate_heap_block(16);

	ASSERT_NE(second_block, nullptr);

	EXPECT_FALSE(second_block->is_access_protected());
	EXPECT_FALSE(second_block->is_write_protected());
	EXPECT_TRUE(second_block->is_resizable());

	EXPECT_EQ(second_block->get_address(), (initial_address_ + first_block->get_size()));
	EXPECT_EQ(second_block->get_size(), 16);
	EXPECT_EQ(object_.get_next_address(), (initial_address_ + first_block->get_size() + second_block->get_size()));

	EXPECT_THROW(object_.allocate_block(0), cminus::memory::exception::invalid_size);
}

TEST_F(MemoryObjectTest, TestEdgeDeallocation){
	auto first_block = object_.allocate_block(8);

	ASSERT_NE(first_block, nullptr);

	auto second_block = object_.allocate_block(16);

	ASSERT_NE(second_block, nullptr);

	object_.deallocate_block(second_block->get_address());
	EXPECT_EQ(object_.get_next_address(), (initial_address_ + first_block->get_size()));

	auto third_block = object_.allocate_block(2);

	ASSERT_NE(third_block, nullptr);

	auto fourth_block = object_.allocate_block(106);

	ASSERT_NE(fourth_block, nullptr);

	object_.deallocate_block(fourth_block->get_address());
	EXPECT_EQ(object_.get_next_address(), (initial_address_ + first_block->get_size() + third_block->get_size()));

	EXPECT_THROW(object_.deallocate_block(fourth_block->get_address()), cminus::memory::exception::block_not_found);
}

TEST_F(MemoryObjectTest, TestSplitDeallocation){
	auto first_block = object_.allocate_block(8);

	ASSERT_NE(first_block, nullptr);

	auto second_block = object_.allocate_block(16);

	ASSERT_NE(second_block, nullptr);

	auto third_block = object_.allocate_block(2);

	ASSERT_NE(third_block, nullptr);

	object_.deallocate_block(second_block->get_address());
	EXPECT_EQ(object_.get_next_address(), (initial_address_ + first_block->get_size() + second_block->get_size() + third_block->get_size()));

	auto fourth_block = object_.allocate_block(106);

	ASSERT_NE(fourth_block, nullptr);

	object_.deallocate_block(third_block->get_address());
	EXPECT_EQ(object_.get_next_address(), (initial_address_ + first_block->get_size() + second_block->get_size() + third_block->get_size() + fourth_block->get_size()));

	object_.deallocate_block(first_block->get_address());
	EXPECT_EQ(object_.get_next_address(), (initial_address_ + first_block->get_size() + second_block->get_size() + third_block->get_size() + fourth_block->get_size()));

	object_.deallocate_block(fourth_block->get_address());
	EXPECT_EQ(object_.get_next_address(), initial_address_);
}

TEST_F(MemoryObjectTest, TestAllocationAfterDeallocation){
	auto first_block = object_.allocate_block(8);

	ASSERT_NE(first_block, nullptr);

	auto second_block = object_.allocate_block(16);

	ASSERT_NE(second_block, nullptr);

	auto third_block = object_.allocate_block(2);

	ASSERT_NE(third_block, nullptr);

	object_.deallocate_block(second_block->get_address());
	EXPECT_EQ(object_.get_next_address(), (initial_address_ + first_block->get_size() + second_block->get_size() + third_block->get_size()));

	auto fourth_block = object_.allocate_block(10);

	ASSERT_NE(fourth_block, nullptr);

	EXPECT_EQ(fourth_block->get_address(), second_block->get_address());
	EXPECT_EQ(object_.get_next_address(), (initial_address_ + first_block->get_size() + second_block->get_size() + third_block->get_size()));

	auto fifth_block = object_.allocate_block(4);

	ASSERT_NE(fifth_block, nullptr);

	EXPECT_EQ(fifth_block->get_address(), (fourth_block->get_address() + fourth_block->get_size()));
	EXPECT_EQ(object_.get_next_address(), (initial_address_ + first_block->get_size() + second_block->get_size() + third_block->get_size()));

	auto sixth_block = object_.allocate_block(8);

	ASSERT_NE(sixth_block, nullptr);

	EXPECT_EQ(sixth_block->get_address(), (third_block->get_address() + third_block->get_size()));
	EXPECT_EQ(object_.get_next_address(), (initial_address_ + first_block->get_size() + second_block->get_size() + third_block->get_size() + sixth_block->get_size()));
}

TEST_F(MemoryObjectTest, TestReallocation){
	auto first_block = object_.allocate_heap_block(8);

	ASSERT_NE(first_block, nullptr);

	auto second_block = object_.allocate_heap_block(16);

	ASSERT_NE(second_block, nullptr);

	auto sixth_block = object_.allocate_heap_block(2);

	ASSERT_NE(sixth_block, nullptr);

	auto seventh_block = object_.allocate_heap_block(6);

	auto third_block = object_.reallocate_heap_block(first_block->get_address(), 10);

	ASSERT_NE(third_block, nullptr);
	EXPECT_EQ((third_block->get_address() + third_block->get_size()), object_.get_next_address());
	EXPECT_EQ(third_block->get_size(), 10);

	auto fourth_block = object_.reallocate_heap_block(second_block->get_address(), 7);

	ASSERT_NE(fourth_block, nullptr);
	EXPECT_EQ(fourth_block->get_address(), second_block->get_address());
	EXPECT_EQ(fourth_block->get_size(), 7);

	auto fifth_block = object_.allocate_block(9);

	ASSERT_NE(fifth_block, nullptr);

	EXPECT_EQ(fifth_block->get_address(), (fourth_block->get_address() + fourth_block->get_size()));

	auto eighth_block = object_.reallocate_heap_block(sixth_block->get_address(), 8);

	ASSERT_NE(eighth_block, nullptr);
	EXPECT_EQ(eighth_block->get_address(), first_block->get_address());
	EXPECT_EQ(eighth_block->get_size(), 8);

	auto ninth_block = object_.reallocate_heap_block(third_block->get_address(), 8);

	ASSERT_NE(ninth_block, nullptr);
	EXPECT_EQ(ninth_block->get_address(), third_block->get_address());
	EXPECT_EQ((ninth_block->get_address() + ninth_block->get_size()), object_.get_next_address());
	EXPECT_EQ(ninth_block->get_size(), 8);
}
