#ifndef _ENGINE_CORE_ENUMMASK_H
#define _ENGINE_CORE_ENUMMASK_H

#include <cstddef>
#include <bitset>

namespace Lame
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
		inline bool test(const EnumType i_key) const { return mask_.test(i_key); }
		inline void set() { mask_.set(); }
		inline void set(const EnumType i_key, bool i_val = true) { mask_.set(i_key, i_val); }
		inline void reset() { mask_.reset(); }
		inline void reset(const EnumType i_key) { mask_.reset(i_key); }
		inline void flip() { mask_.flip(); }
		inline void flip(const EnumType i_key) { mask_.flip(i_key); }
		inline size_t count() const { return mask_.count(); }				//number of set bits
		inline size_t size() const { return mask_.size(); }					//number of bits in the entire set
		inline bool any() const { return mask_.any(); }
		inline bool none() const { return mask_.none(); }
		inline bool all() const { return mask_.all(); }
		inline std::bitset<EnumType::Count> mask() const { return mask_; }
		inline void mask(const std::bitset<EnumType::Count>& i_mask) { mask_ = i_mask; }

		bool operator==(const EnumMask<EnumType>& i_other) const { return mask_ == i_other.mask_; }
		bool operator!=(const EnumMask<EnumType>& i_other) const { return mask_ != i_other.mask_; }

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

		EnumMask<EnumType> operator&(const EnumMask<EnumType>& i_other) const
		{
			EnumMask<EnumType> newMask;
			newMask.mask_ = mask_ & i_other.mask_;
			return newMask;
		}

		EnumMask<EnumType> operator|(const EnumMask<EnumType>& i_other) const
		{
			EnumMask<EnumType> newMask;
			newMask.mask_ = mask_ | i_other.mask_;
			return newMask;
		}

		EnumMask<EnumType> operator~() const
		{
			EnumMask<EnumType> newMask;
			newMask.mask_ = ~mask_;
			return newMask;
		}

	private:
		std::bitset<EnumType::Count> mask_;
	};

} //namespace Lame

#define ENUM_FLAGS_OPERATORS(Enum_Type) \
inline Enum_Type operator|(Enum_Type a, Enum_Type b) { return static_cast<Enum_Type>(a | b); } \
inline Enum_Type& operator|=(Enum_Type& a, Enum_Type b) { return a = a | b; } \
inline Enum_Type operator&(Enum_Type a, Enum_Type b) { return static_cast<Enum_Type>(a & b); } \
inline Enum_Type& operator&=(Enum_Type& a, Enum_Type b) { return a = a & b; } \
inline Enum_Type operator^(Enum_Type a, Enum_Type b) { return static_cast<Enum_Type>(a ^ b); } \
inline Enum_Type& operator^=(Enum_Type& a, Enum_Type b) { return a = a ^ b; } \
inline Enum_Type operator~(Enum_Type a) { return static_cast<Enum_Type>(~a); }

#endif //_ENGINE_CORE_ENUMMASK_H
