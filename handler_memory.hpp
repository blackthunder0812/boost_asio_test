#ifndef HANDLER_MEMORY_HPP
#define HANDLER_MEMORY_HPP

#include <memory>

class handler_memory
{
  private:
    void *storage_;
    bool in_use_;
    size_t current_size_;

  public:
    handler_memory();
    handler_memory(const handler_memory&) = delete;             // non-copyable
    handler_memory& operator=(const handler_memory&) = delete;  // non-copyable

    void* allocate(size_t size);
    void  deallocate(void *ptr);
};

#endif // HANDLER_MEMORY_HPP
