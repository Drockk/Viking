#ifndef VIKING_DELETION_QUEUE_HPP
#define VIKING_DELETION_QUEUE_HPP

#include <deque>
#include <functional>

namespace vi
{
    class DeletionQueue
    {
        using DeleteFunction = std::function<void()>;

    public:
        static void push_function(DeleteFunction&& p_function);
        static void flush();

    private:
        inline static std::deque<DeleteFunction> m_deletors;
    };
} // vi

#endif //VIKING_DELETION_QUEUE_HPP
