#ifndef HANDLER_ALLOCATOR_HPP
#define HANDLER_ALLOCATOR_HPP

#include "handler_memory.hpp"

template<typename T>
class handler_allocator
{
  private:
    handler_memory& memory_;
    template <typename> friend class handler_allocator;

  public:
    using value_type = T;

    explicit handler_allocator(handler_memory& memory_) :
      memory_(memory_)
    {
    }

    template <typename U>
    handler_allocator(const handler_allocator<U>& other) noexcept :
      memory_(other.memory_)
    {
    }

    bool operator==(const handler_allocator& other) const noexcept
    {
      return &memory_ == &other.memory_;
    }

    bool operator!=(const handler_allocator& other) const noexcept
    {
      return &memory_ != &other.memory_;
    }

    T* allocate(size_t n) const
    {
      return static_cast<T*>(memory_.allocate(n * sizeof(T)));
    }

    void deallocate(T* ptr, size_t) const
    {
      memory_.deallocate(ptr);
    }
};

#endif // HANDLER_ALLOCATOR_HPP
