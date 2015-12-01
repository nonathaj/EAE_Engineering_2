#ifndef _ENGINE_TYPEHANDLING_H
#define _ENGINE_TYPEHANDLING_H

#include <cstdint>
#include <cstdio>
#include <memory>

///
/// Replacement for built-in dynamic_cast, which requires runtime type information (RTTI)
///
///	Instead of storing type information for each class, this stores a static int for each class marked
///		with the macro.  So attempting conversions only requires an integer compare.
///
namespace Engine
{
	namespace TypeHandling
	{
		// TypeID backend
		typedef uint32_t typeid_t;

		// Unique ID generator
		// Could also use the __COUNTER__ preprocessor macro, but it may be MS specific
		inline typeid_t GenerateUniqueID();

		// Use some template trickery to create a unique ID per class or type
		template<typename T>
		inline typeid_t GetTypeID();

		// Use even more template trickery to get the classes unique ID from an instance of it.
		template<typename T>
		inline typeid_t GetTypeIDFromInstance(const T *);
	}
}

// A handy macro for adding type ID info to a class
#define ADD_TYPEID() \
public: \
	inline virtual Engine::TypeHandling::typeid_t GetTypeID() const { return Engine::TypeHandling::GetTypeIDFromInstance( this ); } \
private:

// A template for casting from base to derived, if it's possible
template<typename To, typename From>
inline To * attempt_cast(From * i_pFrom);

template<typename To, typename From>
inline const To * attempt_cast(const From * i_pFrom);

template<typename To, typename From>
inline std::shared_ptr<To> attempt_pointer_cast(const std::shared_ptr<From>& i_from);

template<typename To, typename From>
inline std::weak_ptr<To> attempt_pointer_cast(const std::weak_ptr<From>& i_from);

#include "TypeHandling.inl"

#endif //_ENGINE_TYPEHANDLING_H