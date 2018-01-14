#ifndef SMART_LIB_GIT_HEAP_POINTER_TEMPLATE_HPP
#define SMART_LIB_GIT_HEAP_POINTER_TEMPLATE_HPP

#include <memory>

template<typename T>
using uniquePtr = std::unique_ptr<T, void(*)(T*)>;

#endif //SMART_LIB_GIT_HEAP_POINTER_TEMPLATE_HPP
