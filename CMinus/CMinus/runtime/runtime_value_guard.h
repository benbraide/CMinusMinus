#pragma once

namespace cminus::runtime{
	template <class value_type>
	class value_guard{
	public:
		value_guard(value_type &target, const value_type &value)
			: target_(target), value_(target){
			target_ = value;
		}

		~value_guard(){
			target_ = value_;
		}

	private:
		value_type &target_;
		value_type value_;
	};
}
