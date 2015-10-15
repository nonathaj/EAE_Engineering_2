#ifndef _ENGINE_CORE_HASHEDSTRING_H
#define _ENGINE_CORE_HASHEDSTRING_H

#include <string>
#include <stdint.h>

namespace Engine
{
	/*
		HashedString Class

		Used for storing a string as an integer.
		Useful for string that are not printed and only compared wth other strings
	*/
	class HashedString
	{
	public:
		inline HashedString();
		inline HashedString(const char * i_string);
		inline HashedString(const HashedString & i_other);
		inline HashedString & operator=(const HashedString & i_other);
		inline ~HashedString();

		inline uint32_t Get() const;

		inline bool operator==(const HashedString & i_other) const;
		inline bool operator!=(const HashedString & i_other) const;

		static uint32_t Hash(const char * i_string);
		static uint32_t Hash(const void * i_bytes, size_t i_count);
	private:
		uint32_t 	m_Hash;

#ifdef DEBUG_KEEP_STRING
		const char * 	m_pString;
#endif
	};
}

#include "HashedString.inl"

namespace std
{
	//Overload the std::hash to allow the hashedstring to be used in std containers, such as unordered_map
	template<>
	struct hash<Engine::HashedString>
	{
		uint32_t operator()(const Engine::HashedString &i_str) const { return static_cast<uint32_t>(hash<uint32_t>()(i_str.Get())); }
	};
}

#endif //_ENGINE_CORE_HASHEDSTRING_H