// AUTOGENERATED file, created by the tool generate_stub.py, don't edit!
// If you want to add your own functionality, instead edit the stubOperationsDispatcher_custom.h file.

#ifndef STUB_COMMON_OPERATIONSDISPATCHER_H
#define STUB_COMMON_OPERATIONSDISPATCHER_H

#include "common/OperationsDispatcher.h"
#include "stubOperationsDispatcher_custom.h"

#ifndef STUB_OpQueEntry_OpQueEntry
//#define STUB_OpQueEntry_OpQueEntry
  template <typename T>
   OpQueEntry<T>::OpQueEntry(Operation o, T & f)
    : from(nullptr)
  {
    
  }
#endif //STUB_OpQueEntry_OpQueEntry

#ifndef STUB_OpQueEntry_OpQueEntry
//#define STUB_OpQueEntry_OpQueEntry
  template <typename T>
   OpQueEntry<T>::OpQueEntry(const OpQueEntry & o)
    : from(nullptr)
  {
    
  }
#endif //STUB_OpQueEntry_OpQueEntry

#ifndef STUB_OpQueEntry_OpQueEntry_DTOR
//#define STUB_OpQueEntry_OpQueEntry_DTOR
  template <typename T>
   OpQueEntry<T>::~OpQueEntry()
  {
    
  }
#endif //STUB_OpQueEntry_OpQueEntry_DTOR


#ifndef STUB_OperationsDispatcher_OperationsDispatcher
//#define STUB_OperationsDispatcher_OperationsDispatcher
  template <typename T>
   OperationsDispatcher<T>::OperationsDispatcher(const std::function<void(const Operation&, T&)>& operationProcessor, const std::function<double()>& timeProviderFn)
  {
    
  }
#endif //STUB_OperationsDispatcher_OperationsDispatcher

#ifndef STUB_OperationsDispatcher_OperationsDispatcher_DTOR
//#define STUB_OperationsDispatcher_OperationsDispatcher_DTOR
  template <typename T>
   OperationsDispatcher<T>::~OperationsDispatcher()
  {
    
  }
#endif //STUB_OperationsDispatcher_OperationsDispatcher_DTOR

#ifndef STUB_OperationsDispatcher_idle
//#define STUB_OperationsDispatcher_idle
  template <typename T>
  bool OperationsDispatcher<T>::idle()
  {
    return false;
  }
#endif //STUB_OperationsDispatcher_idle

#ifndef STUB_OperationsDispatcher_secondsUntilNextOp
//#define STUB_OperationsDispatcher_secondsUntilNextOp
  template <typename T>
  double OperationsDispatcher<T>::secondsUntilNextOp() const
  {
    return 0;
  }
#endif //STUB_OperationsDispatcher_secondsUntilNextOp

#ifndef STUB_OperationsDispatcher_isQueueDirty
//#define STUB_OperationsDispatcher_isQueueDirty
  template <typename T>
  bool OperationsDispatcher<T>::isQueueDirty() const
  {
    return false;
  }
#endif //STUB_OperationsDispatcher_isQueueDirty

#ifndef STUB_OperationsDispatcher_markQueueAsClean
//#define STUB_OperationsDispatcher_markQueueAsClean
  template <typename T>
  void OperationsDispatcher<T>::markQueueAsClean()
  {
    
  }
#endif //STUB_OperationsDispatcher_markQueueAsClean

#ifndef STUB_OperationsDispatcher_clearQueues
//#define STUB_OperationsDispatcher_clearQueues
  template <typename T>
  void OperationsDispatcher<T>::clearQueues()
  {
    
  }
#endif //STUB_OperationsDispatcher_clearQueues

#ifndef STUB_OperationsDispatcher_addOperationToQueue
//#define STUB_OperationsDispatcher_addOperationToQueue
  template <typename T>
  void OperationsDispatcher<T>::addOperationToQueue(const Operation &, T &)
  {
    
  }
#endif //STUB_OperationsDispatcher_addOperationToQueue

#ifndef STUB_OperationsDispatcher_dispatchOperation
//#define STUB_OperationsDispatcher_dispatchOperation
  template <typename T>
  void OperationsDispatcher<T>::dispatchOperation(const OpQueEntry<T>& opQueueEntry)
  {
    
  }
#endif //STUB_OperationsDispatcher_dispatchOperation

#ifndef STUB_OperationsDispatcher_getTime
//#define STUB_OperationsDispatcher_getTime
  template <typename T>
  double OperationsDispatcher<T>::getTime() const
  {
    return 0;
  }
#endif //STUB_OperationsDispatcher_getTime


#endif