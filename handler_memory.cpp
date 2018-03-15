#include "handler_memory.hpp"
#include <iostream>

handler_memory::handler_memory() :
  storage_(nullptr),
  in_use_(false),
  current_size_(0)
{

}

void *handler_memory::allocate(std::size_t size)
{
  std::cout << in_use_  << " " << size  << " " << current_size_ << std::endl;
  if (!in_use_ && size <= current_size_) {
    in_use_ = true;
    return storage_;
  } else {
    std::cout << "alloc " << std::endl;
    current_size_ = size;
    storage_ = ::operator new(current_size_);
    return storage_;
  }
}

void handler_memory::deallocate(void *ptr)
{
  std::cout << ptr << " " << (void*)storage_ << std::endl;
  if (ptr == storage_) {
    in_use_ = false;
  } else {
    std::cout << "dealloc" << std::endl;
    ::operator delete(ptr);
  }
}
