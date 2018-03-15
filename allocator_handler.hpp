#ifndef ALLOCATOR_HANDLER_HPP
#define ALLOCATOR_HANDLER_HPP

#include "handler_memory.hpp"
#include "handler_allocator.hpp"

template <typename Handler>
class allocator_handler
{
  public:
    using allocator_type = handler_allocator<Handler>;

    allocator_handler(handler_memory& memory, Handler handler) :
      memory_(memory),
      handler_(handler)
    {
    }

    allocator_type get_allocator() const noexcept
    {
      return handler_allocator<Handler>(memory_);
    }

    template <typename ...Args>
    void operator()(Args&&... args)
    {
      handler_(std::forward<Args>(args)...);
    }

  private:
    handler_memory& memory_;
    Handler handler_;
};

template <typename Handler>
inline allocator_handler<Handler> make_allocation_handler(handler_memory& m, Handler h)
{
  return allocator_handler<Handler>(m, h);
}

#endif // ALLOCATOR_HANDLER_HPP
