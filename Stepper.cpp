#include "Stepper.h"

Stepper::Stepper(PinName step_pin,
                 PinName dir_pin,
                 uint16_t step_per_rev) : m_Step(step_pin)
                                        , m_Dir(dir_pin)
                                        , m_Thread(osPriorityHigh2)
{
    m_steps_per_rev = static_cast<float>(step_per_rev);
    m_time_step_const = 1.0e6f / m_steps_per_rev;

    m_steps_setpoint = 0;
    m_steps = 0;
    m_velocity = 1.0f;

    m_period_mus = 0;

    // start thread
    m_Thread.start(callback(this, &Stepper::threadTask));
}

Stepper::~Stepper()
{
    m_Ticker.detach();
    m_Timeout.detach();
    m_Thread.terminate();
}

void Stepper::setRotation(float rotations, float velocity)
{
    setSteps(static_cast<int>(rotations * m_steps_per_rev + 0.5f), velocity);
}

void Stepper::setRotation(float rotations)
{
    setRotation(rotations, m_velocity);
}

void Stepper::setRotationRelative(float rotations, float velocity)
{
    setSteps(static_cast<int>(rotations * m_steps_per_rev + 0.5f) + m_steps, velocity);
}

void Stepper::setRotationRelative(float rotations)
{
    setSteps(static_cast<int>(rotations * m_steps_per_rev + 0.5f) + m_steps, m_velocity);
}

void Stepper::setVelocity(float velocity)
{
    if (velocity == 0.0f) {
        m_velocity = 0.0f;
        m_period_mus = 0;
        m_Ticker.detach();
        return;
    } else if (velocity > 0.0f)
        setSteps(STEPS_MAX, velocity);
    else
        setSteps(STEPS_MIN, velocity);
}

void Stepper::setSteps(int steps, float velocity)
{
    // return if setpoint is equal to steps or velocity is zero
    m_steps_setpoint = steps;
    if (m_steps_setpoint == m_steps || velocity == 0.0f) {
        m_velocity = 0.0f;
        m_period_mus = 0;
        m_Ticker.detach();
        return;
    }
    
    // write direction
    if (m_steps_setpoint > m_steps)
        m_Dir.write(1);
    else
        m_Dir.write(0);

    // calculate period for ticker in microseconds
    const int period_mus = static_cast<int>(m_time_step_const / fabs(velocity) + 0.5f);
    
    // make sure we only attach the ticker if the period has changed
    if (m_period_mus != period_mus) {

        // update quantizised velocity
        m_velocity = copysignf(m_time_step_const / static_cast<float>(period_mus), velocity);

        // update period
        m_period_mus = period_mus;
    
        // attach sendThreadFlag() to ticker so that sendThreadFlag() is called periodically, which signals the thread to execute
        m_Ticker.attach(callback(this, &Stepper::sendThreadFlag), std::chrono::microseconds{m_period_mus});
    }
}

void Stepper::step()
{
    // send one step via timeout
    enableDigitalOutput();
    m_Timeout.attach(callback(this, &Stepper::disableDigitalOutput), std::chrono::microseconds{PULSE_MUS});

    // increment steps
    if (m_Dir.read() == 0)
        m_steps--;
    else
        m_steps++;
}

void Stepper::enableDigitalOutput()
{
    m_Step = 1; // set the digital output to high
}

void Stepper::disableDigitalOutput()
{
    m_Step = 0; // set the digital output to low
}

void Stepper::threadTask()
{
    while (true) {
        ThisThread::flags_wait_any(m_ThreadFlag);

        if (m_steps_setpoint == m_steps) {
            m_period_mus = 0;
            m_Ticker.detach();
        } else
            step();
    }
}

void Stepper::sendThreadFlag()
{
    // set the thread flag to trigger the thread task
    m_Thread.flags_set(m_ThreadFlag);
}