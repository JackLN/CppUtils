# CppUtils
Some utils with game develop


## Obj Pool

```

//obj pool
class ObjTest : public CharcterProtocal
{
public:
	ObjTest(){};
	virtual ~ObjTest(){};
	virtual void sleep(){};
	virtual void awake(){};

	virtual void on_create(){}
	virtual void on_destroy(){}
};

//retrieve or recycle obj
void func()
{
	ObjTest* foo = PoolMgr::getInstance()->get_object(foo);
	PoolMgr::getInstance()->free_object(foo);
}

```

## To be continue