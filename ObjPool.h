#pragma once

#include <iostream>
#include <vector>
#include "cocos2d.h"
#include <algorithm>

#include "TSingleton.h"
using namespace std;
USING_NS_CC;

#define POOL_INITSIZE 128

class CharcterProtocal
{
public:
	virtual void sleep() = 0;//���ǰ�Ĳ���
	virtual void awake() = 0;//����ǰ�Ĳ���

	virtual void on_create(){}//�ӳ��д���ʱ�Ĳ���new
	virtual void on_destroy(){}//�ӳ����ͷ��ǵĲ���delete
};

class PoolProtocal : public Ref
{
public:
	virtual void garbage() = 0;
};
template <typename T>
class ObjectPool : public PoolProtocal
{
public:

	ObjectPool()
	{
		allocate(POOL_INITSIZE);
		m_initSize = POOL_INITSIZE;
	};
    ObjectPool(int initSize):m_initSize(initSize)
    {
        CCAssert(m_initSize >= 4, "m_initSize should >= 4");
        allocate(initSize);
    };
    virtual ~ObjectPool()
    {
		log("free pool");
        //for_each(m_usedList.begin(), m_usedList.end(), free_object);
        for_each(m_freeList.begin(), m_freeList.end(), free_object);
    }
	static ObjectPool<T>* getInstance()
	{
		/*static ObjectPool<T> _instance(POOL_INITSIZE);
		return &_instance;*/
		return TSingleton<ObjectPool<T>>::getInstance();
	}

	virtual void init(){};
    // ����һ������
    T* request(){
        
        //�ȼ����������Ƿ�Ϊ��
        if (m_freeList.size() == 0) {
            allocate(1);
        }
        
        T* t = m_freeList.front();
		t->awake();
		m_freeList.pop_front();
        //m_freeList.erase(m_freeList.begin());
        //m_usedList.push_back(t);
		//cocos2d::log("%s  size : %d,%d", __FUNCTION__, m_usedList.size(),m_freeList.size());
        return t;
    };
    
    
    // �Ż�һ�����󵽳�����
    void put_back(T* t)
    {
		t->sleep();
		m_freeList.push_back(t);
    //    for (int i = 0; i < m_usedList.size(); ++i) {
    //        if (t == m_usedList[i]) {
    //            //�ҵ��˶���
				//t->sleep();
    //            m_usedList.erase(m_usedList.begin() + i);
    //            m_freeList.push_back(t);return;
    //        }
    //    }
    }
    
    
    //Ϊ�˱����ڴ�Ĳ�����������Ҫ���ֿռ�
    void garbage()
    {
        //��ӡ�´�С
        //log("m_usedList.size:%d",m_usedList.size());
        log("m_freeList.size:%d",m_freeList.size());
        
        //�������Ķ���20��ɾ������ʼ������
        if (m_freeList.size() > m_initSize) {

            typename deque<T*>::iterator it = m_freeList.begin();
            int pIndex = 0;
            while (it != m_freeList.end()) {
                
                if (pIndex > m_initSize) {
					T* t = m_freeList.front();
					m_freeList.pop_front();
                    //m_freeList.erase(it);
                    free_object(t);
                    --it;
                }
                
                ++pIndex;
                ++it;
            }
        }
    }
    
private:
    
    // ����size���ռ�
    void allocate(int size)
    {
        for (int i = 0; i < size; ++i) {
            T* t = new T;
			t->on_create();
            m_freeList.push_back(t);
        }
    }
    
    static void free_object(void* p)
    {
        T* t = (T*)p;
		t->on_destroy();
        delete t;
    }
private:
    // ʹ������listһ���������Ѿ�ʹ�õ�,һ��������δʹ�õ�
    //vector<T*> m_freeList;
    //vector<T*> m_usedList;
	// ���ö���
	deque<T*> m_freeList;
    // ��ʼ����
    int m_initSize;
};

/*
 * PoolMgr
 * Create by Jack
 */
class PoolMgr : public TSingleton<PoolMgr>
{
	CCArray* _pPools;
	class pool_counter{
	public:
		pool_counter(PoolProtocal* pool,CCArray* pools)
		{
			pools->addObject(pool);
		}
	};
public:
	template<class T> 
	T* get_object(T*& obj)
	{
		static pool_counter _pPoolCounter(ObjectPool<T>::getInstance(), _pPools);
		return ObjectPool<T>::getInstance()->request();
	}

	template<class T>
	void free_object(T* obj)
	{
		ObjectPool<T>::getInstance()->put_back(obj);
	}

//Constructor
public:
	PoolMgr()
	{
		_pPools = CCArray::create();
		_pPools->retain();
	}
	virtual ~PoolMgr()
	{
		/*int iCnt = _pPools->count();
		for (int iPoolIdx = 0; iPoolIdx < iCnt; iPoolIdx++)
		{
		auto obj = _pPools->getObjectAtIndex(iPoolIdx);
		delete obj;
		}*/
		while (_pPools->count() > 0)
		{
			auto obj = _pPools->getLastObject();
			_pPools->removeLastObject();
			delete obj;
		}

		_pPools->release();
	}
};

#define GET_Pool PoolMgr::getInstance()

//TEST
//class pooltesta : public CharcterProtocal
//{
//public:
//	pooltesta(){};
//	virtual ~pooltesta(){};
//	virtual void sleep(){};
//	virtual void awake(){};
//
//	virtual void on_create(){}
//	virtual void on_destroy(){}
//};
//
//class pooltestb : public CharcterProtocal
//{
//public:
//	pooltestb(){};
//	virtual ~pooltestb(){};
//	virtual void sleep(){};
//	virtual void awake(){};
//
//	virtual void on_create(){}
//	virtual void on_destroy(){}
//};