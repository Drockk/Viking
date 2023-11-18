//
// Created by batzi on 29.10.2023.
//

#include "DeletionQueue.hpp"

#include <algorithm>

namespace vi
{
    void DeletionQueue::push_function(DeleteFunction &&p_function)
    {
        m_deletors.emplace_back(p_function);
    }

    void DeletionQueue::flush()
    {
        std::for_each(m_deletors.rbegin(), m_deletors.rend(), [](const DeleteFunction& p_function){
            if (p_function) {
                p_function();
            }
        });
    }
} // vi
