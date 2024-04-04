/*
Stepper motor class
szar
04.03.2024
*/

#ifndef STEPPER_H_
#define STEPPER_H_

#include "mbed.h"
#include "ThreadFlag.h"

class Stepper
{
public:
    /**
     * @brief Construct a new Stepper motor object.
     *
     */
    explicit Stepper(PinName step_pin,
                     PinName dir_pin,
                     float initial_speed = 1.0f,
                     int step_per_rev = 200);

    /**
     * @brief Destroy the Stepper object.
     */
    virtual ~Stepper();

    // Functions to get values from driver
    float getPosition();
    float getRotations(); 
    float getSpeed();

    // Functions to set parameters in driver
    void setDirection(bool direction);
    void setSpeed(float speed);
    void setAbsoluteZeroPosition();

    // Functions to use position commanding
    void setAbsolutePosition(int absolute_pos);
    void setRelativePositon(int relative_pos);
    void setAbsoluteRevolutions(float absolute_rev);
    void setRelativeRevolutions(float relative_rev);

    // Functions to use velocity commanding 
    void startRotation();
    void stopRotation();


    // Enumerators for directions and motor states
    enum Direction {
        CW=0,
        CCW=1
    } direction = Direction::CW;

    enum MotorMode {
        STEPS,
        VELOCITY,
        VOID,
        STOP,
    } motor_mode;

private:
    // Variables regarding time periods
    static constexpr int PULSE_MUS = 30;
    int interval_time_mus;

    // Variables inside the driver
    float motor_speed;
    int steps_absolute;
    int steps_setpoint;
    float steps_per_rev;
    float time_step_const;
    bool stop_rotation;

    // Private functions
    void sendThreadFlag();
    void handler();
    void step();
    void enableDigitalOutput();
    void disableDigitalOutput();

    // Objects
    DigitalOut m_Step;
    DigitalOut m_Dir;

    Thread m_Thread;
    Ticker m_Ticker;
    Timeout m_Timeout;
    ThreadFlag m_ThreadFlag;
};
#endif /* STEPPER_H_ */