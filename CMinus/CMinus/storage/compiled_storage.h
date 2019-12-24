#pragma once

#include "storage_object.h"

namespace cminus::storage{
	class compiled : public named_object{
	public:
		using named_object::named_object;

		virtual ~compiled();

	protected:
		virtual void add_variable_(std::shared_ptr<declaration::variable> entry, std::size_t address) override;

		virtual void add_callable_(std::shared_ptr<declaration::callable> entry, std::size_t address) override;

		virtual void compile_();

		std::size_t static_address_ = 0u;
		std::size_t static_size_ = 0u;

		std::size_t function_address_ = 0u;
		std::size_t function_size_ = 0u;

		bool should_compile_ = true;
	};
}
