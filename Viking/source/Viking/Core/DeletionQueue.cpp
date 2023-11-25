#include "DeletionQueue.hpp"

#include <algorithm>

#include "Debug/Profiler.hpp"

namespace vi
{
    void DeletionQueue::push_function(DeleteFunction &&p_function)
    {
        PROFILER_EVENT();
        m_deletors.emplace_back(p_function);
    }

    void DeletionQueue::flush()
    {
        PROFILER_EVENT();
        std::for_each(m_deletors.rbegin(), m_deletors.rend(), [](const DeleteFunction& p_function){
            if (p_function) {
                p_function();
            }
        });
    }
} // vi
