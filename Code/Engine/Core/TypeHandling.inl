
namespace Engine
{
	namespace TypeHandling
	{
		// TypeID backend
		// Unique ID generator
		// Could also use the __COUNTER__ preprocessor macro, but it may be MS specific
		inline typeid_t GenerateUniqueID()
		{
			static typeid_t currentTypeID = 0;
			return ++currentTypeID;
		}

		// Use some template trickery to create a unique ID per class or type
		template<typename T>
		inline typeid_t GetTypeID()
		{
			static const typeid_t typeID = GenerateUniqueID();
			return typeID;
		}

		// Use even more template trickery to get the classes unique ID from an instance of it.
		template<typename T>
		inline typeid_t GetTypeIDFromInstance(const T *)
		{
			return GetTypeID<T>();
		}
	}
}

// A template for casting from base to derived, if it's possible
template<typename To, typename From>
inline To * attempt_cast(From * i_pFrom)
{
	if (i_pFrom && i_pFrom->GetTypeID() == Engine::TypeHandling::GetTypeID<To>())
		return static_cast<To *>(i_pFrom);
	else
		return nullptr;
}

template<typename To, typename From>
inline const To * attempt_cast(const From * i_pFrom)
{
	if (i_pFrom && i_pFrom->GetTypeID() == Engine::TypeHandling::GetTypeID<To>())
		return static_cast<const To *>(i_pFrom);
	else
		return nullptr;
}

template<typename To, typename From>
inline std::shared_ptr<To> attempt_pointer_cast(const std::shared_ptr<From>& i_from)
{
	if (i_from && i_from->GetTypeID() == Engine::TypeHandling::GetTypeID<To>())
		return std::static_pointer_cast<To>(i_from);
	else
		return nullptr;
}

template<typename To, typename From>
inline std::weak_ptr<To> attempt_pointer_cast(const std::weak_ptr<From>& i_from)
{
	if (!i_from.expired())
		return attempt_pointer_cast<To>(i_from.lock());
	else
		return nullptr;
}