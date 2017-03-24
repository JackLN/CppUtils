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
	virtual void sleep() = 0;//归池前的操作
	virtual void awake() = 0;//出池前的操作

	virtual void on_create(){}//从池中创建时的操作new
	virtual void on_destroy(){}//从池中释放是的操作delete
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
    // 请求一个对象
    T* request(){
        
        //先检查空闲里面是否为空
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
    
    
    // 放回一个对象到池里面
    void put_back(T* t)
    {
		t->sleep();
		m_freeList.push_back(t);
    //    for (int i = 0; i < m_usedList.size(); ++i) {
    //        if (t == m_usedList[i]) {
    //            //找到此对象
				//t->sleep();
    //            m_usedList.erase(m_usedList.begin() + i);
    //            m_freeList.push_back(t);return;
    //        }
    //    }
    }
    
    
    //为了避免内存的不断增长，需要保持空间
    void garbage()
    {
        //打印下大小
        //log("m_usedList.size:%d",m_usedList.size());
        log("m_freeList.size:%d",m_freeList.size());
        
        //如果空余的多余20则删除到初始化个数
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
    
    // 分配size个空间
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
    // 使用两个list一个用来放已经使用的,一个用来放未使用的
    //vector<T*> m_freeList;
    //vector<T*> m_usedList;
	// 改用队列
	deque<T*> m_freeList;
    // 初始容量
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