#include "DispatcherEvent.hpp"

namespace vi
{
    void EventDispatcher::add_listener(const EventType p_type, const std::function<void(const EventPointer&)>& p_callback)
    {
        m_event_queue.appendListener(p_type, p_callback);
    }

    void EventDispatcher::send_event(const EventPointer& p_event)
    {
        m_event_queue.enqueue(p_event);
    }

    void EventDispatcher::send_event(const EventPointer&& p_event)
    {
        m_event_queue.enqueue(p_event);
    }

    void EventDispatcher::dispatch()
    {
        m_event_queue.process();
    }
}
