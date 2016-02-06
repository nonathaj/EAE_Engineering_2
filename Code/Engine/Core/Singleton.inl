#include <assert.h>

namespace Lame
{
	template<class T>
	Singleton<T>::~Singleton()
	{
		Release();
	}

	template<class T>
	void Singleton<T>::Touch()
	{
		if (!m_pInstance)
			m_pInstance = new T();
	}

	template<class T>
	T & Singleton<T>::Get()
	{
		Touch();
		return *m_pInstance;
	}

	template<class T>
	bool Singleton<T>::Exists()
	{
		return m_pInstance != nullptr;
	}

	template<class T>
	void Singleton<T>::Release()
	{
		if( m_pInstance )
		{
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}

	template <class T>
	T * Singleton<T>::m_pInstance = nullptr;

} // namespace Lame
