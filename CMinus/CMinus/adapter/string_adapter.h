#pragma once

#include "../type/string_type.h"
#include "../declaration/string_function_definitions.h"

namespace cminus::adapter{
	class string{
	public:
		string();

		string(const string &other);

		explicit string(const std::string_view &value);

		explicit string(const std::string &value);

		explicit string(const char *value);

		string(const char *value, std::size_t size);

		string(std::size_t size, char value);

		string(std::shared_ptr<memory::reference> value);

		template <typename value_type>
		string(value_type value)
			: string(std::make_shared<memory::scalar_reference<value_type>>(storage::get_cached_type<value_type>::template type(), value)){}

		bool empty() const;

		std::size_t size() const;

		const char *data() const;

		const char *begin() const;

		const char *end() const;

		char at(std::size_t position) const;

		std::size_t find(const string &other, std::size_t offset = 0u) const;

		std::size_t find(const std::string_view &value, std::size_t offset = 0u) const;

		std::size_t find(const std::string &value, std::size_t offset = 0u) const;

		std::size_t find(const char *value, std::size_t offset = 0u) const;

		std::size_t find(const char *value, std::size_t size, std::size_t offset) const;

		std::size_t find(std::shared_ptr<cminus::memory::reference> value, std::size_t offset = 0u) const;

		template <typename value_type>
		std::size_t find(value_type value, std::size_t offset = 0u) const{
			return find(std::make_shared<memory::scalar_reference<value_type>>(storage::get_cached_type<value_type>::template type(), value), offset);
		}

		void resize(std::size_t size, char fill = '\0');

		void erase(std::size_t position, std::size_t size);

		void clear();

	private:
		std::shared_ptr<memory::reference> value_;
		const char *data_ = nullptr;
	};
}
