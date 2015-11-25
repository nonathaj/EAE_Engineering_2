#ifndef _ENGINE_CORE_ENUMMASK_H
#define _ENGINE_CORE_ENUMMASK_H

#include <cstddef>
#include <bitset>

namespace Engine
{
	//struct for ensuring an enum has a count element.
	//this does NOT validate that the count element is the last element
	template<class T>
	struct has_enum_count {
		typedef char yes;
		typedef yes(&no)[2];

		template<class U> static yes test(decltype(U::Count)*);
		template<class U> static no  test(...);

		//SFINAE magic.
		static bool const value = sizeof(test<T>(0)) == sizeof(yes);
	};

	template<typename EnumType>
	class EnumMask
	{
		static_assert(std::is_enum<EnumType>::value, "Type for EnumMask must be an Enum");
		static_assert(has_enum_count<EnumType>::value, "Enum provided to EnumMask must have a \"Count\" option as the last element in the enum.");

	public:
		inline bool test(const EnumType i_key) { return mask_.test(i_key); }
		inline void set() { mask_.set(); }
		inline void set(const EnumType i_key, bool i_val = true) { mask_.set(i_key, i_val); }
		inline void reset() { mask_.reset(); }
		inline void reset(const EnumType i_key) { mask_.reset(i_key); }
		inline void flip() { mask_.flip(); }
		inline void flip(const EnumType i_key) { mask_.flip(i_key); }
		inline size_t count() { return mask_.count(); }					//number of set bits
		inline size_t size() { return mask_.size(); }					//number of bits in the entire set
		inline bool any() { return mask_.any(); }
		inline bool none() { return mask_.none(); }
		inline bool all() { return mask_.all(); }
		inline std::bitset<EnumType::Count> mask() { return mask_; }

		bool operator==(const EnumMask<EnumType>& i_other) { return mask_ == i_other.mask_; }
		bool operator!=(const EnumMask<EnumType>& i_other) { return mask_ != i_other.mask_; }

		EnumMask<EnumType> operator&=(const EnumMask<EnumType>& i_other)
		{
			mask_ &= i_other.mask_;
			return *this;
		}

		EnumMask<EnumType> operator|=(const EnumMask<EnumType>& i_other)
		{
			mask_ |= i_other.mask_;
			return *this;
		}

		EnumMask<EnumType> operator&(const EnumMask<EnumType>& i_other)
		{
			EnumMask<EnumType> newMask;
			newMask.mask_ = mask_ & i_other.mask_;
			return newMask;
		}

		EnumMask<EnumType> operator|(const EnumMask<EnumType>& i_other)
		{
			EnumMask<EnumType> newMask;
			newMask.mask_ = mask_ | i_other.mask_;
			return newMask;
		}

		EnumMask<EnumType> operator~()
		{
			EnumMask<EnumType> newMask;
			newMask.mask_ = ~mask_;
			return newMask;
		}

	private:
		std::bitset<EnumType::Count> mask_;
	};

} //namespace Engine

#endif //_ENGINE_CORE_ENUMMASK_H
