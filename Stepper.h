/*
 * Stepper motor class
 * szar
 * 04.03.2024
 */

#ifndef STEPPER_H_
#define STEPPER_H_

#include "mbed.h"
#include "ThreadFlag.h"
#include <cstdio>
#include <type_traits>

class Stepper
{
public:
    /**
     * @brief Construct a new Stepper motor object.
     * @param step_pin The pin to control step pulses.
     * @param dir_pin The pin to control direction.
     * @param initial_speed The initial speed of the motor in revolutions per second.
     * @param step_per_rev The number of steps per revolution of the motor.
     */
    explicit Stepper(PinName step_pin,
                     PinName dir_pin,
                     float initial_speed = 3.0f,
                     int step_per_rev = 200);

    /**
     * @brief Destroy the Stepper object.
     */
    virtual ~Stepper();

    /**
     * @brief Get the current position of the stepper motor in steps.
     * @return Current position in steps.
     */
    float getPosition();

    /**
     * @brief Get the current position of the stepper motor in rotations.
     * @return Current position in rotations.
     */
    float getRotations();

    /**
     * @brief Get the current speed of the stepper motor.
     * @return Current speed in revolutions per second.
     */
    float getSpeed();

    /**
     * @brief Set the direction of the stepper motor.
     * @param direction The direction to set (true for CW, false for CCW).
     */
    void setDirection(bool direction);

    /**
     * @brief Set the speed of the stepper motor.
     * @param speed The speed to set in revolutions per second.
     */
    void setSpeed(float speed);

    /**
     * @brief Set the current position as the zero position.
     */
    void setAbsoluteZeroPosition();

    /**
     * @brief Set an absolute position for the stepper motor to move to.
     * @param absolute_pos The absolute position to move to in steps.
     */
    void setAbsolutePosition(int absolute_pos);

    /**
     * @brief Set a relative position for the stepper motor to move to.
     * @param relative_pos The relative position to move to in steps.
     */
    void setRelativePositon(int relative_pos);

    /**
     * @brief Set an absolute position for the stepper motor to move to in revolutions.
     * @param absolute_rev The absolute position to move to in revolutions.
     */
    void setAbsoluteRevolutions(float absolute_rev);

    /**
     * @brief Set a relative position for the stepper motor to move to in revolutions.
     * @param relative_rev The relative position to move to in revolutions.
     */
    void setRelativeRevolutions(float relative_rev);

    /**
     * @brief Start the rotation of the stepper motor.
     */
    void startRotation();

    /**
     * @brief Stop the rotation of the stepper motor.
     */
    void stopRotation();

    // Enumerators for directions and motor states
    enum Direction {
        CW = 0,  ///< Clockwise direction
        CCW = 1  ///< Counter-clockwise direction
    } direction = Direction::CW;

    enum MotorMode {
        STEPS,   ///< Step mode
        VELOCITY,///< Velocity mode
        VOID,    ///< Void mode
        STOP     ///< Stop mode
    } motor_mode;

private:
    // Constants
    static constexpr int PULSE_MUS = 30;  ///< Pulse width in microseconds

    // Timing variables
    int interval_time_mus;

    // Motor variables
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

    // Hardware objects
    DigitalOut m_Step;
    DigitalOut m_Dir;

    Thread m_Thread;
    Ticker m_Ticker;
    Timeout m_Timeout;
    ThreadFlag m_ThreadFlag;
};

#endif /* STEPPER_H_ */