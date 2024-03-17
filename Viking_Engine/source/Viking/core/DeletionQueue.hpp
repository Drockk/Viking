#ifndef DELETION_QUEUE_HPP
#define DELETION_QUEUE_HPP
#include <deque>
#include <functional>

namespace vi
{
    class DeletionQueue
    {
        using deletors_function = std::function<void()>;

    public:
        void push_function(const deletors_function& p_function)
        {
            m_deletors.push_back(p_function);
        }

        void flush()
        {
            std::for_each(m_deletors.rbegin(), m_deletors.rend(), [](const deletors_function& p_function) {
                p_function();
            });
        }

    private:
        std::deque<deletors_function> m_deletors;
    };
}
#endif // DELETION_QUEUE_HPP
