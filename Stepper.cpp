#include "Stepper.h"

Stepper::Stepper(PinName step_pin,
                 PinName dir_pin,
                 float initial_speed,
                 int step_rev) : m_Step(step_pin), m_Dir(dir_pin), m_Thread(osPriorityHigh2)
{
    motor_mode = MotorMode::VOID;
    steps_per_rev = step_rev;
    time_step_const = 1000000.0f/steps_per_rev;
    setDirection(direction);
    setSpeed(initial_speed);
    m_Ticker.attach(callback(this, &Stepper::sendThreadFlag), std::chrono::microseconds{interval_time_mus});
    // start thread
    m_Thread.start(callback(this, &Stepper::handler));
}

Stepper::~Stepper()
{
    m_Ticker.detach();
    m_Thread.terminate();
}

// Functions to get values from driver
float Stepper::getPosition() 
{
    return float(steps_absolute);
}

float Stepper::getRotations()
{
    return (float(steps_absolute))/steps_per_rev;
}

float Stepper::getSpeed()
{
    return motor_speed;
}

// Functions to set parameters in driver
void Stepper::setDirection(bool direction)
{
    m_Dir.write(direction);
}

void Stepper::setSpeed(float speed)
{
    if (speed <= 0.0f) {
        m_Dir.write(CCW);
        motor_speed = abs(speed); 
    } else {
        m_Dir.write(CW);
        motor_speed = speed;
    }
    
    interval_time_mus = int(time_step_const/motor_speed);

    motor_mode = MotorMode::STOP;
    //interval_time_mus = speed;
    motor_mode = MotorMode::VOID;
    m_Ticker.attach(callback(this, &Stepper::sendThreadFlag), std::chrono::microseconds{interval_time_mus});
}

void Stepper::setAbsoluteZeroPosition()
{
    steps_absolute = 0;
}


// Functions to use position commanding
void Stepper::setAbsolutePosition(int absolute_pos)
{   
    stop_rotation = false;
    steps_setpoint = absolute_pos;
    if (steps_absolute <= steps_setpoint) {
        m_Dir.write(CW);

    } else {
        m_Dir.write(CCW);
    }
    motor_mode = MotorMode::STEPS;
}

// Functions to use position commanding
void Stepper::setRelativePositon(int relative_pos)
{   
    stop_rotation = false;
    steps_setpoint = steps_absolute + relative_pos;
    if (relative_pos >= 0) {
        m_Dir.write(CW);

    } else {
        m_Dir.write(CCW);
    }
    motor_mode = MotorMode::STEPS;
}

void Stepper::setAbsoluteRevolutions(float absolute_rev)
{
    steps_setpoint = int((absolute_rev * steps_per_rev) + 0.5f);
    if (steps_absolute <= steps_setpoint) {
        m_Dir.write(CW);
    } else {
        m_Dir.write(CCW);
    }
    motor_mode = MotorMode::STEPS;
}

void Stepper::setRelativeRevolutions(float relative_rev)
{
    steps_setpoint = steps_absolute + int((relative_rev * steps_per_rev) + 0.5f);
    if (relative_rev >= 0) {
        m_Dir.write(CW);

    } else {
        m_Dir.write(CCW);
    }
    motor_mode = MotorMode::STEPS;
}


// Functions to use velocity commanding
void Stepper::startRotation()
{   
    stop_rotation = false;
    motor_mode = MotorMode::VELOCITY;
}

void Stepper::stopRotation()
{
    stop_rotation = true;
}

// Private functions
void Stepper::handler()
{
    while (true) {
        ThisThread::flags_wait_any(m_ThreadFlag);
        switch (motor_mode) {
            case MotorMode::STEPS:
                if (steps_absolute == steps_setpoint) {
                    motor_mode = MotorMode::VOID;
                } else {
                    step();
                }
                break;

            case MotorMode::VELOCITY:
                step();
                if (stop_rotation == true) {
                    motor_mode = MotorMode::VOID;
                }
                break;

            case MotorMode::VOID:
                disableDigitalOutput();
                break;

            case MotorMode::STOP:
                m_Ticker.detach();
                break;

            default:

                break; // should not happen
        }
    }
}

void Stepper::step()
{
    enableDigitalOutput();
    m_Timeout.attach(callback(this, &Stepper::disableDigitalOutput), std::chrono::microseconds{PULSE_MUS});
    if (m_Dir.read() == 0) {
        steps_absolute++;
    } else {
        steps_absolute--;
    }
}

void Stepper::enableDigitalOutput()
{
    // set the digital output to high
    m_Step = 1;
}

void Stepper::disableDigitalOutput()
{
    // set the digital output to low
    m_Step = 0;
}

void Stepper::sendThreadFlag()
{
    // set the thread flag to trigger the thread task
    m_Thread.flags_set(m_ThreadFlag);
}