#ifndef _INSTANTIATION_STRATEGY_HH_
#define _INSTANTIATION_STRATEGY_HH_

#include "OperationStore.hh"
#include <type_traits>

class Object;
namespace answer{

// enum InstantiationStrategyType {SINGLECALL, SINGLETON, LAZY_SINGLETON};
enum RestPayload { AUTO, FORCE_POST, FORCE_GET };

namespace instantiation{

class Instance{};

class SingleCall: public Instance{};

class Singleton: public Instance{};

class LazySingleton: public Instance{};

template <unsigned MIN_THREADS, unsigned MAX_THREADS>
class WorkerPool: public Instance{
public:
	enum {min_threads = MIN_THREADS};
	enum {max_threads = MAX_THREADS};
};
}

//The default strategy is singleCall
template <typename ObjectT, class InstantiationStrategyType = void>
class InstantiationStrategy{
  mutable ObjectT *_webMethod; //Keep the last (cache -> justifies mutable)
public:
  InstantiationStrategy():_webMethod(0){}
  ~InstantiationStrategy(){if (_webMethod) delete _webMethod;}

  ObjectT& Instance() const{
    if (_webMethod){ delete _webMethod; }
    _webMethod = new ObjectT();
    return *_webMethod;
  }
};

template <typename ObjectT>
class InstantiationStrategy<
  ObjectT,
  typename std::enable_if<
    std::is_base_of<instantiation::Singleton, ObjectT>::value
  >::type
  >
{
	ObjectT *_webMethod;
public:
	InstantiationStrategy():_webMethod( new ObjectT() ){}
	~InstantiationStrategy(){delete _webMethod;}

	ObjectT& Instance() const{
		return *_webMethod;
	}
};

template <typename ObjectT>
class InstantiationStrategy<
  ObjectT,
  typename std::enable_if<
    std::is_base_of<instantiation::LazySingleton, ObjectT>::value
  >::type>
{
public:
	ObjectT& Instance() const{
		static ObjectT webMethod;
		return webMethod;
	}
};

// template <typename ObjectT, unsigned MIN, unsigned MAX>
// class InstantiationStrategy<
//   typename std::enable_if<
//     std::is_base_of<instantiation::WorkerPool<MIN, MAX>, ObjectT>::value
//   >::type, 
//   ObjectT>
// {
// public:
// 	InstantiationStrategy(){
// 		std::cerr << "Min Workers: " << MIN << std::endl;
// 		std::cerr << "Max Workers: " << MAX << std::endl;
// 	}
// 	
// 	ObjectT& Instance() const{ // Incomplete
// 		static ObjectT webMethod;
// 		return webMethod;
// 	}
// };
}

#endif //_INSTANTIATION_STRATEGY_HH_
