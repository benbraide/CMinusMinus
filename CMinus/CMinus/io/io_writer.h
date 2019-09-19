#pragma once

#include <string>
#include <typeinfo>

namespace cminus::io{
	class writer{
	public:
		virtual ~writer() = default;

		virtual std::size_t get_position() const = 0;

		virtual std::size_t get_size() const = 0;

		virtual std::size_t set(std::byte value, std::size_t size) = 0;

		virtual std::size_t write(const std::byte *buffer, std::size_t size) = 0;

		virtual std::size_t write(const std::string &buffer){
			return write_buffer(buffer.data(), buffer.size());
		}

		template <typename target_type>
		std::size_t write_scalar(const target_type &buffer){
			return write_scalar_((const std::byte *)(&buffer), sizeof(target_type), typeid(target_type));
		}

		template <typename target_type>
		std::size_t write_buffer(const target_type *buffer, std::size_t count){
			return write((const std::byte *)buffer, (sizeof(target_type) * count));
		}

		virtual std::size_t write_buffer(const char *buffer, std::size_t count){
			return write(reinterpret_cast<const std::byte *>(buffer), count);
		}

		virtual std::size_t write_buffer(const char *buffer){
			return write_buffer(buffer, std::strlen(buffer));
		}

		virtual std::size_t write_buffer(const wchar_t *buffer, std::size_t count){
			return write(reinterpret_cast<const std::byte *>(buffer), (sizeof(wchar_t) * count));
		}

		virtual std::size_t write_buffer(const wchar_t *buffer){
			return write_buffer(buffer, std::wcslen(buffer));
		}

	protected:
		virtual std::size_t write_scalar_(const std::byte *buffer, std::size_t size, const std::type_info &type_info){
			return write(buffer, size);
		}
	};
}
