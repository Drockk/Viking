#ifndef APPLICATION_EVENT_HPP
#define APPLICATION_EVENT_HPP

#include "Viking/event/Event.hpp"

namespace vi
{
    class WindowCloseEvent : public Event
    {
    public:
        WindowCloseEvent() : Event(EventType::WindowClose)
        {

        }
        ~WindowCloseEvent() override = default;
    };
}

#endif // !APPLICATION_EVENT_HPP
