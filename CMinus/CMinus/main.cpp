#include <iostream>

#include "type/string_type.h"
#include "declaration/string_function_definitions.h"

int main(){
	auto memory_object = std::make_shared<cminus::memory::object>();
	cminus::runtime::object::memory_object = memory_object.get();

	auto global_storage = std::make_shared<cminus::storage::global>();
	{//Scoped
		auto str = global_storage->create_string("Test String");
		auto cstr = global_storage->get_string_value(str);

		auto decl = std::make_shared<cminus::declaration::variable>(
			"",
			global_storage->get_string_type(),
			cminus::attribute::collection::list_type{},
			cminus::declaration::flags::nil,
			std::make_shared<cminus::memory::scalar_reference<int>>(global_storage->get_int_type(), 702)
		);

		auto val = decl->evaluate(0u);
		auto cval = global_storage->get_string_value(val);

		auto beg = global_storage->get_raw_string_type()->find_function("Begin")->call(val, {});
		auto cbeg = reinterpret_cast<char *>(memory_object->find_block(beg->read_scalar<std::size_t>())->get_data());

		auto end = global_storage->get_raw_string_type()->find_function("End")->call(val, {});
		auto cend = reinterpret_cast<char *>(memory_object->find_block(end->read_scalar<std::size_t>())->get_data());

		auto at = global_storage->get_raw_string_type()->find_function("At")->call(val, { global_storage->get_zero_value(global_storage->get_size_type()) });
		auto cat = at->read_scalar<char>();

		auto tchar = std::make_shared<cminus::memory::scalar_reference<char>>(
			global_storage->get_char_type(),
			't'
		);

		auto fnd = global_storage->get_raw_string_type()->find_function("Find")->call(str, { tchar });
		auto cfnd = fnd->read_scalar<std::size_t>();

		auto rfnd = global_storage->get_raw_string_type()->find_function("FindLast")->call(str, { tchar });
		auto crfnd = rfnd->read_scalar<std::size_t>();

		auto pos5 = std::make_shared<cminus::memory::scalar_reference<std::size_t>>(
			global_storage->get_size_type(),
			5u
		);

		auto sub = global_storage->get_raw_string_type()->find_function("GetSub")->call(str, { pos5 });
		auto csub = global_storage->get_string_value(sub);
	}

	return 0;
}