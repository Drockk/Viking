//
// Created by batzi on 29.10.2023.
//

#include "DeletionQueue.hpp"

#include <algorithm>

namespace vi
{
    void DeletionQueue::push_function(DeletionQueue::DeleteFunction &&t_function)
    {
        m_deletors.push_back(t_function);
    }

    void DeletionQueue::flush()
    {
        std::for_each(m_deletors.rbegin(), m_deletors.rend(), [](DeleteFunction& t_function){
            if (t_function) {
                t_function();
            }
        });
    }
} // vi