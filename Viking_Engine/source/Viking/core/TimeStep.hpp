#ifndef TIME_STEP_HPP
#define TIME_STEP_HPP

namespace vi
{
    class TimeStep
    {
    public:
        TimeStep(float p_time = 0.0f): m_time{ p_time } {}

        operator float() const { return m_time; }

        [[nodiscard]] float get_seconds() const { return m_time; }
        [[nodiscard]] float get_millieconds() const { return m_time * 1000.0f; }

    private:
        float m_time{};
    };
}

#endif
