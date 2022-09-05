#pragma once

namespace Viking {
    class TimeStep {
    public:
        TimeStep(const float time = 0.0f): m_Time(time) {
            
        }

        operator float() const {
            return m_Time;
        }

        [[nodiscard]] float getSeconds() const {
            return m_Time;
        }

        [[nodiscard]] float getMilliseconds() const {
            return m_Time * 1000.0f;
        }
    private:
        float m_Time;
    };
}
