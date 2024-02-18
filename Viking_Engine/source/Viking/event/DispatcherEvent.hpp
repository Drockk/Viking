#ifndef DISPATCHER_EVENT_HPP
#define DISPATCHER_EVENT_HPP

#include "Event.hpp"

#include <eventpp/eventqueue.h>

#include <memory>

namespace vi
{
    using EventPointer = std::shared_ptr<Event>;

    struct EventPolicy
    {
        static EventType getEvent(const EventPointer& p_event)
        {
            return p_event->get_type();
        }
    };

    class EventDispatcher
    {
        using EventQueue = eventpp::EventQueue<EventType, void(const EventPointer&), EventPolicy>;

    public:
        static void add_listener(EventType p_type, const std::function<void(const EventPointer&)>& p_callback);
        static void send_event(const EventPointer& p_event);
        static void send_event(const EventPointer&& p_event);
        static void dispatch();

    private:
        inline static EventQueue m_event_queue{};
    };
}

#endif // !DISPATCHER_EVENT_HPP
