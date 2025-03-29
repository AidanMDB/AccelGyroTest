// ======================================================================
// \title  AccelGyro.hpp
// \author aidandb
// \brief  hpp file for AccelGyro component implementation class
// ======================================================================

#ifndef Components_AccelGyro_HPP
#define Components_AccelGyro_HPP

#include "Components/AccelGyro/AccelGyroComponentAc.hpp"

namespace Components {

  class AccelGyro :
    public AccelGyroComponentBase
  {

    public:

    struct I2cAddr {
      enum T {
        // used when multiple devices connected on one line
        AD0_0 = 0x68, //!< device address with ad0 set to 0
        AD0_1 = 0x69  //!< device address with ad0 set to 1
      };
    };

    static const U8 POWER_MGMT_ADDR = 0x6B;
    static const U8 GYRO_CONFIG_ADDR = 0x1B;
    static const U8 ACCEL_CONFIG_ADDR = 0x1C;
    static const U8 DEVICE_CONFIG_ADDR = 0x1A;
    static const U8 GYRO_RAW_DATA_START = 0x43;
    static const U8 ACCEL_RAW_DATA_START = 0x3B;
    static const U8 POWER_ON = 0x00;
    static const U8 POWER_OFF = 0x40;

    static const U16 MAX_DATA_SIZE = 6;
    static const U16 REG_SIZE_BYTES = 1;

    static constexpr float accelScaleFactor = 16384.0f;
    static constexpr float gyroScaleFactor = 131.072f;
      // ----------------------------------------------------------------------
      // Component construction and destruction
      // ----------------------------------------------------------------------

      //! Construct AccelGyro object
      AccelGyro(
          const char* const compName //!< The component name
      );
      
      //! Initialize object AccelGyro
      void init(const NATIVE_INT_TYPE instance = 0);

      //! Destroy AccelGyro object
      ~AccelGyro();

      void setup(I2cAddr::T devAddress);

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for Run
      //!
      //! Port for sending telemtry to ground
      void Run_handler(
          FwIndexType portNum, //!< The port number
          U32 context //!< The call order
      ) override;

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for commands
      // ----------------------------------------------------------------------

      //! Handler implementation for command POWER_ON_OFF
      //!
      //! Command to turn on or off the accelerometer and gyroscope
      void POWER_ON_OFF_cmdHandler(
          FwOpcodeType opCode, //!< The opcode
          U32 cmdSeq, //!< The command sequence number
          Fw::On powerState //!< Indicates whether the device is on or off
      ) override;
      

      // ----------------------------------------------------------------------
      // Helper Functions
      // ----------------------------------------------------------------------
      /**
       * \brief Turn power on/off of device
       * \param powerState: ON/OFF Type from the framework
       */
      void power(Fw::On powerState);

      /**
       * \brief Read, telemetry, and update accelerometer data
       */
      void updateAccel();

      /**
       * \brief Read, telemetry, and update gyroscope data
       */
      void updateGyro();
      
      /**
       * \brief configures the accelerometer and gyroscope
       */
      void config();

      Drv::I2cStatus readRegisterBlock(U8 startRegisterAddress, Fw::Buffer& buffer);

      Drv::I2cStatus setupReadRegister(U8 registerAddress);

      F32x3 deserializeVector(Fw::Buffer& buffer, F32 scaleFactor);

      // ----------------------------------------------------------------------
      // Member Variables
      // ----------------------------------------------------------------------
      Fw::On m_power = Fw::On::OFF;
      I2cAddr::T m_I2cDevAddress;
  };

}

#endif
