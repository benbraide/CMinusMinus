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

	EXPECT_FALSE(object_.is_access_protected(first_block));
	EXPECT_FALSE(object_.is_write_protected(first_block));
	EXPECT_FALSE(object_.is_resizable(first_block));

	EXPECT_EQ(first_block, initial_address_);
	EXPECT_EQ(object_.get_size(first_block), 8);
	EXPECT_EQ(object_.get_next_address(), (initial_address_ + object_.get_size(first_block)));

	auto second_block = object_.allocate_heap_block(16);

	EXPECT_FALSE(object_.is_access_protected(second_block));
	EXPECT_FALSE(object_.is_write_protected(second_block));
	EXPECT_TRUE(object_.is_resizable(second_block));

	EXPECT_EQ(second_block, (initial_address_ + object_.get_size(first_block)));
	EXPECT_EQ(object_.get_size(second_block), 16);
	EXPECT_EQ(object_.get_next_address(), (initial_address_ + object_.get_size(first_block) + object_.get_size(second_block)));

	EXPECT_THROW(object_.allocate_block(0), cminus::memory::exception::invalid_size);
}

TEST_F(MemoryObjectTest, TestEdgeDeallocation){
	auto first_block = object_.allocate_block(8), second_block = object_.allocate_block(16);

	object_.deallocate_block(second_block);
	EXPECT_EQ(object_.get_next_address(), (initial_address_ + object_.get_size(first_block)));

	auto third_block = object_.allocate_block(2), fourth_block = object_.allocate_block(106);

	object_.deallocate_block(fourth_block);
	EXPECT_EQ(object_.get_next_address(), (initial_address_ + object_.get_size(first_block) + object_.get_size(third_block)));

	EXPECT_THROW(object_.deallocate_block(fourth_block), cminus::memory::exception::block_not_found);
}

TEST_F(MemoryObjectTest, TestSplitDeallocation){
	auto first_block = object_.allocate_block(8), second_block = object_.allocate_block(16), third_block = object_.allocate_block(2);

	object_.deallocate_block(second_block);
	EXPECT_EQ(object_.get_next_address(), (initial_address_ + object_.get_size(first_block) + object_.get_size(second_block) + object_.get_size(third_block)));

	auto fourth_block = object_.allocate_block(106);

	object_.deallocate_block(third_block);
	EXPECT_EQ(object_.get_next_address(), (initial_address_ + object_.get_size(first_block) + object_.get_size(second_block) + object_.get_size(third_block) + object_.get_size(fourth_block)));

	object_.deallocate_block(first_block);
	EXPECT_EQ(object_.get_next_address(), (initial_address_ + object_.get_size(first_block) + object_.get_size(second_block) + object_.get_size(third_block) + object_.get_size(fourth_block)));

	object_.deallocate_block(fourth_block);
	EXPECT_EQ(object_.get_next_address(), initial_address_);
}

TEST_F(MemoryObjectTest, TestAllocationAfterDeallocation){
	auto first_block = object_.allocate_block(8), second_block = object_.allocate_block(16), third_block = object_.allocate_block(2);

	object_.deallocate_block(second_block);
	EXPECT_EQ(object_.get_next_address(), (initial_address_ + object_.get_size(first_block) + object_.get_size(second_block) + object_.get_size(third_block)));

	auto fourth_block = object_.allocate_block(10);

	EXPECT_EQ(fourth_block, second_block);
	EXPECT_EQ(object_.get_next_address(), (initial_address_ + object_.get_size(first_block) + 16 + object_.get_size(third_block)));

	auto fifth_block = object_.allocate_block(4);

	EXPECT_EQ(fifth_block, (fourth_block + object_.get_size(fourth_block)));
	EXPECT_EQ(object_.get_next_address(), (initial_address_ + object_.get_size(first_block) + 16 + object_.get_size(third_block)));

	auto sixth_block = object_.allocate_block(8);

	EXPECT_EQ(sixth_block, (third_block + object_.get_size(third_block)));
	EXPECT_EQ(object_.get_next_address(), (initial_address_ + object_.get_size(first_block) + 16 + object_.get_size(third_block) + object_.get_size(sixth_block)));
}

TEST_F(MemoryObjectTest, TestReallocation){
	auto first_block = object_.allocate_heap_block(8), second_block = object_.allocate_heap_block(16), sixth_block = object_.allocate_heap_block(2), seventh_block = object_.allocate_heap_block(6);
	auto third_block = object_.reallocate_heap_block(first_block, 10);

	EXPECT_EQ((third_block + object_.get_size(third_block)), object_.get_next_address());
	EXPECT_EQ(object_.get_size(third_block), 10);

	auto fourth_block = object_.reallocate_heap_block(second_block, 7);

	EXPECT_EQ(fourth_block, second_block);
	EXPECT_EQ(object_.get_size(fourth_block), 7);

	auto fifth_block = object_.allocate_block(9);

	EXPECT_EQ(fifth_block, (fourth_block + object_.get_size(fourth_block)));

	auto eighth_block = object_.reallocate_heap_block(sixth_block, 8);

	EXPECT_EQ(eighth_block, first_block);
	EXPECT_EQ(object_.get_size(eighth_block), 8);

	auto ninth_block = object_.reallocate_heap_block(third_block, 8);

	EXPECT_EQ(ninth_block, third_block);
	EXPECT_EQ((ninth_block + object_.get_size(ninth_block)), object_.get_next_address());
	EXPECT_EQ(object_.get_size(ninth_block), 8);
}
