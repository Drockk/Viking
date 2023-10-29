//
// Created by batzi on 29.10.2023.
//

#ifndef VIKING_DELETIONQUEUE_HPP
#define VIKING_DELETIONQUEUE_HPP

#include <deque>
#include <functional>

namespace vi
{
    class DeletionQueue
    {
        using DeleteFunction = std::function<void()>;

    public:
        static void push_function(DeleteFunction&& t_function);
        static void flush();

    private:
        inline static std::deque<DeleteFunction> m_deletors;
    };
} // vi

#endif //VIKING_DELETIONQUEUE_HPP
