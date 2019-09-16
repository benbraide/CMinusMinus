#pragma once

namespace cminus::attribute{
	enum class stage_type{
		nil,
		before_declaration,
		after_declaration,
		after_lookup,
		before_address_read,
		after_address_read,
		before_read,
		after_read,
		before_write,
		after_write,
		before_delete,
		after_delete,
		before_inheritance,
		after_inheritance,
	};

	enum class query_result{
		nil,
		skip,
		prohibit,
		require,
		info,
	};
}
