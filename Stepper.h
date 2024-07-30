#ifndef STEPPER_H_
#define STEPPER_H_

#include "mbed.h"
#include "ThreadFlag.h"

class Stepper
{
public:
    explicit Stepper(PinName step_pin,
                     PinName dir_pin,
                     uint16_t step_per_rev = 200 * 16);
    virtual ~Stepper();

    int getStepsSetpoint() const { return m_steps_setpoint; };
    int getSteps() const { return m_steps; };
    float getRotation() const { return static_cast<float>(m_steps) / m_steps_per_rev; };
    float getVelocity() const { return m_velocity; };

    void setInternalVelocity(float velocity = 0.0f) { m_velocity = velocity; };
    void setInternalRotation(float rotations = 0.0f) { m_steps = static_cast<int>(rotations * m_steps_per_rev + 0.5f); };

    void setRotation(float rotations, float velocity = 1.0f);
    void setRotation(float rotations);
    void setRotationRelative(float rotations, float velocity = 1.0f);
    void setRotationRelative(float rotations);
    void setVelocity(float velocity);
    void setSteps(int steps, float velocity);

private:
    static constexpr int PULSE_MUS = 10;
    static constexpr int STEPS_MIN = std::numeric_limits<int>::min();
    static constexpr int STEPS_MAX = std::numeric_limits<int>::max();

    DigitalOut m_Step;
    DigitalOut m_Dir;

    Thread m_Thread;
    Ticker m_Ticker;
    ThreadFlag m_ThreadFlag;
    Timeout m_Timeout;

    float m_steps_per_rev;
    float m_time_step_const;

    int m_steps_setpoint;
    int m_steps;
    float m_velocity;

    int m_period_mus;

    void step();
    void enableDigitalOutput();
    void disableDigitalOutput();

    void threadTask();
    void sendThreadFlag();
};

#endif /* STEPPER_H_ */