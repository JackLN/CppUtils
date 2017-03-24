#pragma once
#ifdef WIN32
#include <Windows.h>
#else
#pragma GCC diagnostic ignored "-Wwrite-strings"
#endif

template <typename T>
class TSingleton
{
public:
	virtual ~TSingleton()
	{
		if (m_Instance)
		{
			m_Instance = 0;
		}
	}
	static T *getInstance() 
	{
		if (!m_Instance)
		{
			m_Instance = new T;
			m_Instance->init();
		}
		return m_Instance;
	}

	virtual void init(){};

protected:
	TSingleton(){}
	TSingleton(const TSingleton<T>&){}
	TSingleton<T>& operator=(const TSingleton<T>&){}

	static T *m_Instance;
};

template<typename T>
T *TSingleton<T>::m_Instance = 0;