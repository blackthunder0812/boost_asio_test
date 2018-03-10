#ifndef DEMANGLE_H
#define DEMANGLE_H

#include <cxxabi.h>
#include <memory>

std::shared_ptr<char> cpp_demangle (const char* abi_name) {
  int status;
  char *ret = abi::__cxa_demangle(abi_name, 0, 0, &status);
  std::shared_ptr<char> retval;
  retval.reset((char*)ret, [](char *mem){
    if (mem) {
      free((void*)mem);
    }});
  return retval;
}

#define CLASS_NAME(pointer) ((const char*)cpp_demangle(typeid(*pointer).name()).get())

#endif // DEMANGLE_H
