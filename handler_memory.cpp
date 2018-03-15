#include "handler_memory.hpp"

handler_memory::handler_memory() :
  storage_(nullptr),
  in_use_(false),
  current_size_(0)
{

}

handler_memory::~handler_memory()
{
  ::operator delete(storage_);
}

void *handler_memory::allocate(std::size_t size)
{
  if (!in_use_ && size <= current_size_) {
    in_use_ = true;
    return storage_;
  } else {
    current_size_ = size;
    in_use_ = true;
    storage_ = ::operator new(current_size_);
    return storage_;
  }
}

void handler_memory::deallocate(void *ptr)
{
  if (ptr == storage_) {
    in_use_ = false;
  } else {
    ::operator delete(ptr);
  }
}
