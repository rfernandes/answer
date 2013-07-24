#ifndef _INSTANTIATION_STRATEGY_HH_
#define _INSTANTIATION_STRATEGY_HH_

#include "OperationStore.hh"

namespace answer{

// enum InstantiationStrategyType {SINGLECALL, SINGLETON, LAZY_SINGLETON};
enum RestPayload { AUTO, FORCE_POST, FORCE_GET };

namespace instantiation{
class SingleCall{
};

class Singleton{
};

class LazySingleton{
};

template <unsigned MIN_THREADS, unsigned MAX_THREADS>
class WorkerPool{
public:
	enum {min_threads = MIN_THREADS};
	enum {max_threads = MAX_THREADS};
};
}

template <class InstantiationStrategyType, typename ObjectT>
// No default implementation
class InstantiationStrategy{
public:
};

template <typename ObjectT>
class InstantiationStrategy<instantiation::SingleCall, ObjectT>{
	mutable ObjectT *_webMethod; //Keep the last (cache -> justifies mutable)
public:
	InstantiationStrategy():_webMethod(0){}
	~InstantiationStrategy(){if (_webMethod) delete _webMethod;}
	
	ObjectT& getInstance() const{
		if (_webMethod){ delete _webMethod; }
		_webMethod = new ObjectT();
		return *_webMethod;
	}
};

template <typename ObjectT>
class InstantiationStrategy<instantiation::Singleton, ObjectT>{
	ObjectT *_webMethod;
public:
	InstantiationStrategy():_webMethod( new ObjectT() ){}
	~InstantiationStrategy(){delete _webMethod;}

	ObjectT& getInstance() const{
		return *_webMethod;
	}
};

template <typename ObjectT>
class InstantiationStrategy<instantiation::LazySingleton, ObjectT>{
public:
	ObjectT& getInstance() const{
		static ObjectT webMethod;
		return webMethod;
	}
};

template <typename ObjectT, unsigned MIN, unsigned MAX>
class InstantiationStrategy<instantiation::WorkerPool<MIN, MAX>, ObjectT>{
public:
	InstantiationStrategy(){
		std::cerr << "Min Workers: " << MIN << std::endl;
		std::cerr << "Max Workers: " << MAX << std::endl;
	}
	
	ObjectT& getInstance() const{ // Incomplete
		static ObjectT webMethod;
		return webMethod;
	}
};
}

#endif //_INSTANTIATION_STRATEGY_HH_
