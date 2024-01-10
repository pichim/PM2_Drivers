/**
 * @file DCMotor.h
 * @brief This file defines the DCMotor class, which is used for controlling a DC motor.
 *
 * The DCMotor class provides functionality to set and control the velocity and rotation of a DC motor.
 * It uses various components such as an EncoderCounter, FastPWM, Motion control, PID controller,
 * and an IIR Filter for precise control. The class offers methods to set velocity, rotation, control gains,
 * and retrieve the current state of the motor.
 *
 * @dependencies
 * This class relies on external components:
 * - EncoderCounter: For encoding the rotation counts.
 * - FastPWM: For generating high-frequency PWM signals.
 * - Motion: For handling motion control.
 * - PID_Cntrl: For implementing PID control.
 * - IIR_Filter: For filtering the velocity signals.
 *
 * Usage:
 * To use the DCMotor class, create an instance with the required motor parameters.
 * Set the desired velocity or rotation using the setVelocity() or setRotation() methods.
 * Control gains can be adjusted using setVelocityCntrl() and setRotationCntrlGain() methods.
 * Current motor status can be obtained using the get methods like getVelocity(), getRotation(), etc.
 *
 * Example:
 * ```
 * DCMotor motor(PIN_PWM, PIN_ENC_A, PIN_ENC_B, COUNTS_PER_TURN, KN, VOLTAGE_MAX);
 * motor.setVelocity(1.5f); // command velocity to 1.5 rotations per second
 * float currentVelocity = motor.getVelocity(); // read current velocity
 * motor.setRotation(5.0f); // command rotation to 5
 * motor.getRotation(); // read current rotation
 * ```
 *
 * @author M. E. Peter
 * @date 11.12.2023
 */

#ifndef DC_MOTOR_H_
#define DC_MOTOR_H_

#include "EncoderCounter.h"
#include "FastPWM/FastPWM.h"
#include "Motion.h"
#include "ThreadFlag.h"
#include "PID_Cntrl.h"
#include "IIR_Filter.h"

#define PERFORM_GPA_MEAS false

#if PERFORM_GPA_MEAS
#include "GPA.h"
#endif

class DCMotor
{
public:
    /**
     * @brief Construct a new DCMotor object.
     *
     * @param pin_pwm The pin name for PWM control of the motor.
     * @param pin_enc_a The first pin name for the encoder.
     * @param pin_enc_b The second pin name for the encoder.
     * @param gear_ratio The gear ratio of the gear box.
     * @param kn The motor constant.
     * @param voltage_max The maximum voltage for the motor.
     * @param counts_per_turn The number of encoder counts per turn of the motor.
     */
    explicit DCMotor(PinName pin_pwm,
                     PinName pin_enc_a,
                     PinName pin_enc_b,
                     float gear_ratio,
                     float kn,
                     float voltage_max = 12.0f,
                     float counts_per_turn = 20.0f);

    /**
     * @brief Destroy the DCMotor object.
     */
    virtual ~DCMotor();

    /**
     * @brief Set the target velocity of the motor.
     *
     * @param velocity The target velocity in units per second.
     */
    void setVelocity(float velocity);

    /**
     * @brief Set the target rotation of the motor.
     *
     * @param rotation The target rotation in degrees.
     */
    void setRotation(float rotation);

    /**
     * @brief Get the current rotation target of the motor.
     *
     * @return float The current rotation target in degrees.
     */
    float getRotationTarget() const;

    /**
     * @brief Get the current rotation setpoint of the motor.
     *
     * @return float The current rotation setpoint in degrees.
     */
    float getRotationSetpoint() const;

    /**
     * @brief Get the current rotation of the motor.
     *
     * @return float The current rotation in degrees.
     */
    float getRotation() const;

    /**
     * @brief Get the current velocity target of the motor.
     *
     * @return float The current velocity target in units per second.
     */
    float getVelocityTarget() const;

    /**
     * @brief Get the current velocity setpoint of the motor.
     *
     * @return float The current velocity setpoint in units per second.
     */
    float getVelocitySetpoint() const;

    /**
     * @brief Get the current velocity of the motor.
     *
     * @return float The current velocity in units per second.
     */
    float getVelocity() const;

    /**
     * @brief Get the current voltage applied to the motor.
     *
     * @return float The current voltage in volts.
     */
    float getVoltage() const;

    /**
     * @brief Get the current PWM signal applied to the motor.
     *
     * @return float The current PWM signal value.
     */
    float getPWM() const;

    /**
     * @brief Set the control parameters for the velocity PID controller.
     *
     * @param kp The proportional gain.
     * @param ki The integral gain.
     * @param kd The derivative gain.
     */
    void setVelocityCntrl(float kp = KP, float ki = KI, float kd = KD);

    /**
     * @brief Set the gain for the rotation control.
     *
     * @param p The proportional gain for the rotation control.
     */
    void setRotationCntrlGain(float p = P);

    /**
     * @brief Set the maximum velocity for the motor.
     *
     * @param velocity The maximum velocity in rotations per second.
     */
    void setMaxVelocity(float velocity);

    /**
     * @brief Get the maximum velocity for the motor.
     *
     * @return float The maximum velocity in rotations per second.
     */
    float getMaxVelocity();

    /**
     * @brief Set the maximum acceleration for the motor.
     *
     * @param acceleration The maximum acceleration in rotations per second squared.
     */
    void setMaxAcceleration(float acceleration);

    /**
     * @brief Get the maximum acceleration for the motor.
     *
     * @return float The maximum acceleration in rotations per second squared.
     */
    float getMaxAcceleration();

    /**
     * @brief Enable or disable the motion planner.
     *
     * @param enable True to enable the motion planner, false to disable. Enabled by default.
     */
    void setEnableMotionPlanner(bool enable);

private:
    static constexpr int64_t PERIOD_MUS = 500;
    static constexpr float TS = 1.0e-6f * static_cast<float>(PERIOD_MUS);
    static constexpr float PWM_MIN = 0.01f;
    static constexpr float PWM_MAX = 0.99f;
    static constexpr float ROTATION_ERROR_MAX = 3.0e-3f;
    // Default controller parameters where found using a motor with gear ratio 78.125:1
    static constexpr float KP = 4.2f;
    static constexpr float KI = 140.0f;
    static constexpr float KD = 0.0192f;
    static constexpr float P = 16.0f;

    FastPWM m_FastPWM;
    EncoderCounter m_EncoderCounter;
    Motion m_Motion;
    PID_Cntrl m_PID_Cntrl_velocity;
    IIR_Filter m_IIR_Filter_velocity;
#if PERFORM_GPA_MEAS
    GPA m_GPA;
#endif

    Thread m_Thread;
    Ticker m_Ticker;
    ThreadFlag m_ThreadFlag;

    enum CntrlMode {
        Rotation = 0,
        Velocity,
    };
    CntrlMode m_cntrlMode = CntrlMode::Velocity;

    bool m_enable_motion_planner;

    // motor parameters
    float m_counts_per_turn;
    float m_voltage_max;
    float m_velocity_max;
    float m_acceleration_max;

    // rotation controller parameter
    float m_p;

    // signals
    long  m_count;
    short m_count_previous;
    float m_rotation_initial;
    float m_rotation_target;
    float m_rotation_setpoint;
    float m_rotation;
    float m_velocity_target;
    float m_velocity_setpoint;
    float m_velocity;
    float m_voltage;
    float m_pwm;

    void threadTask();
    void sendThreadFlag();
};

#endif /* DC_MOTOR_H_ */