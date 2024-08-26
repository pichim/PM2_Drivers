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

#ifndef STEPPER_H_
#define STEPPER_H_

#include "mbed.h"
#include "ThreadFlag.h"

class Stepper
{
public:
    /**
     * @brief Constructs a new Stepper motor object.
     * 
     * Initializes the stepper motor with specified step and direction pins. 
     * Optionally, the steps per revolution can be set, defaulting to 200 * 16 steps.
     * 
     * @param step_pin Pin to control the stepping pulses.
     * @param dir_pin Pin to control the direction of rotation.
     * @param step_per_rev Number of steps per revolution (default is 3200).
     */
    explicit Stepper(PinName step_pin, PinName dir_pin, uint16_t step_per_rev = 200 * 16);

    /**
     * @brief Destroys the Stepper object, cleaning up resources.
     */
    virtual ~Stepper();

    /**
     * @brief Gets the current step setpoint.
     * 
     * Returns the target step position that the motor is set to reach.
     * 
     * @return Current step setpoint.
     */
    int getStepsSetpoint() const { return m_steps_setpoint; };

    /**
     * @brief Gets the current step count.
     * 
     * Returns the current step position of the motor.
     * 
     * @return Current step position.
     */
    int getSteps() const { return m_steps; };

    /**
     * @brief Gets the current rotation count.
     * 
     * Returns the current rotation count, calculated as steps divided by steps per revolution.
     * 
     * @return Current rotation count.
     */
    float getRotation() const { return static_cast<float>(m_steps) / m_steps_per_rev; };

    /**
     * @brief Gets the current velocity of the motor.
     * 
     * Returns the motor's velocity in rotations per second.
     * 
     * @return Current velocity.
     */
    float getVelocity() const { return m_velocity; };

    /**
     * @brief Sets the internal velocity of the motor.
     * 
     * This function sets the internal velocity without moving the motor.
     * 
     * @param velocity The velocity to set (default is 0.0f).
     */
    void setInternalVelocity(float velocity = 0.0f) { m_velocity = velocity; };

    /**
     * @brief Sets the internal rotation of the motor.
     * 
     * This function sets the internal rotation position without moving the motor.
     * 
     * @param rotations The rotation count to set (default is 0.0f).
     */
    void setInternalRotation(float rotations = 0.0f) { m_steps = static_cast<int>(rotations * m_steps_per_rev + 0.5f); };

    /**
     * @brief Sets the motor's rotation to a specified position at a given velocity.
     * 
     * Moves the motor to the specified rotation count at the provided velocity.
     * 
     * @param rotations Target rotation position.
     * @param velocity Speed at which to reach the target rotation (default is 1.0f).
     */
    void setRotation(float rotations, float velocity = 1.0f);

    /**
     * @brief Sets the motor's rotation to a specified position.
     * 
     * Moves the motor to the specified rotation count at the current velocity.
     * 
     * @param rotations Target rotation position.
     */
    void setRotation(float rotations);

    /**
     * @brief Sets the motor's relative rotation at a given velocity.
     * 
     * Moves the motor by a specified relative rotation amount at the provided velocity.
     * 
     * @param rotations Relative rotation amount.
     * @param velocity Speed at which to reach the target rotation (default is 1.0f).
     */
    void setRotationRelative(float rotations, float velocity = 1.0f);

    /**
     * @brief Sets the motor's relative rotation.
     * 
     * Moves the motor by a specified relative rotation amount at the current velocity.
     * 
     * @param rotations Relative rotation amount.
     */
    void setRotationRelative(float rotations);

    /**
     * @brief Sets the motor's velocity.
     * 
     * Adjusts the speed of the motor's movement.
     * 
     * @param velocity The new velocity to set.
     */
    void setVelocity(float velocity);

    /**
     * @brief Sets the motor's steps to a specific value at a given velocity.
     * 
     * Moves the motor to a specific step count at the provided speed.
     * 
     * @param steps Target step position.
     * @param velocity Speed at which to reach the target step position.
     */
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

    int m_period_mus;  ///< Period of the step signal in microseconds.

    void step();
    void enableDigitalOutput();
    void disableDigitalOutput();
    void threadTask();
    void sendThreadFlag();
};

#endif /* STEPPER_H_ */