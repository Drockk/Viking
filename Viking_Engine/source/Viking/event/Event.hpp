#ifndef EVENT_HPP
#define EVENT_HPP

namespace vi
{
    enum class EventType
    {
        None = 0,
        WindowClose
    };

    class Event
    {
    public:
        explicit Event(const EventType p_type) : m_type{ p_type }
        {

        }
        virtual ~Event() = default;

        EventType get_type() const {
            return m_type;
        }

    private:
        EventType m_type{ EventType::None };
    };
}

#endif // !EVENT_HPP
