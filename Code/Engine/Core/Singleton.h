#ifndef _ENGINE_CORE_SINGLETON_H
#define _ENGINE_CORE_SINGLETON_H

namespace Engine
{
	template <class T>
	class Singleton
	{
	public:
		Singleton();
		~Singleton();

		static T & Get();
		static void Release();

	private:
		// make non-copyable
		Singleton( Singleton const & );
		Singleton & operator=( Singleton const & );

		static T *m_pInstance;
	};
} // namespace Engine

#include "Singleton.inl"

#endif // _ENGINE_CORE_SINGLETON_H