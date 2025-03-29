// ======================================================================
// \title  AccelGyro.cpp
// \author aidandb
// \brief  cpp file for AccelGyro component implementation class
// ======================================================================

#include "Components/AccelGyro/AccelGyro.hpp"

namespace Components {

  // ----------------------------------------------------------------------
  // Component construction and destruction
  // ----------------------------------------------------------------------

  AccelGyro ::
    AccelGyro(const char* const compName) :
      AccelGyroComponentBase(compName)
  {

  }

  void AccelGyro ::
    init(const NATIVE_INT_TYPE instance) 
  {
    AccelGyroComponentBase::init(instance);
  }

  void AccelGyro ::
    setup(I2cAddr::T devAddress)
  {
    m_I2cDevAddress = devAddress;
  }

  AccelGyro ::
    ~AccelGyro()
  {

  }


  // ----------------------------------------------------------------------
  // Handler implementations for typed input ports
  // ----------------------------------------------------------------------

  void AccelGyro ::
    Run_handler(
        FwIndexType portNum,
        U32 context
    )
  {
    if (this->m_power == Fw::On::ON) {
      updateAccel();
      updateGyro();
    }
    // TODO
  }

  // ----------------------------------------------------------------------
  // Handler implementations for commands
  // ----------------------------------------------------------------------

  void AccelGyro ::
    POWER_ON_OFF_cmdHandler(
        FwOpcodeType opCode,
        U32 cmdSeq,
        Fw::On powerState
    )
  {
    // TODO
    power(powerState);
    this->log_ACTIVITY_HI_PowerState(powerState);
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }

  // ----------------------------------------------------------------------
  // Helper Functions
  // ----------------------------------------------------------------------

  Drv::I2cStatus AccelGyro ::
    setupReadRegister(U8 registerAddress)
  {
    Fw::Buffer buffer(&registerAddress, sizeof registerAddress);
    return this->write_out(0, this->m_I2cDevAddress, buffer);
  }

  Drv::I2cStatus AccelGyro ::
    readRegisterBlock(U8 startRegisterAddress, Fw::Buffer& buffer)
  {
    Drv::I2cStatus status;
    status = setupReadRegister(startRegisterAddress);

    if (status == Drv::I2cStatus::I2C_OK) {
      status = this->read_out(0, m_I2cDevAddress, buffer);
    }

    return status;
  }

  F32x3 AccelGyro ::
    deserializeVector(Fw::Buffer& buffer, F32 scaleFactor)
  {
    Components::F32x3 vect;
    I16 value;
    FW_ASSERT(buffer.getSize() == 6);
    FW_ASSERT(buffer.getData() != nullptr);

    Fw::SerializeBufferBase& deserializeHelper = buffer.getSerializeRepr();
    deserializeHelper.setBuffLen(buffer.getSize());
    FW_ASSERT(deserializeHelper.deserialize(value) == Fw::FW_SERIALIZE_OK);
    vect[0] = static_cast<F32>(value) / scaleFactor;

    FW_ASSERT(deserializeHelper.deserialize(value) == Fw::FW_SERIALIZE_OK);
    vect[1] = static_cast<F32>(value) / scaleFactor;

    FW_ASSERT(deserializeHelper.deserialize(value) == Fw::FW_SERIALIZE_OK);
    vect[2] = static_cast<F32>(value) / scaleFactor;

    return vect;
  }

  void AccelGyro ::
    config()
  {
    U8 data[REG_SIZE_BYTES * 2];
    Fw::Buffer buffer(data, sizeof data);

    data[0] = ACCEL_CONFIG_ADDR;
    data[1] = 0;      // sets the accel sensitivity to +-2g

    Drv::I2cStatus status = this->write_out(0, this->m_I2cDevAddress, buffer);
    if (status != Drv::I2cStatus::I2C_OK) {
      this->log_WARNING_HI_ConfigError(status);
    }


    data[0] = GYRO_CONFIG_ADDR;
    data[1] = 0;      // sets the gyro sensitivity to +-250 deg/

    status = this->write_out(0, this->m_I2cDevAddress, buffer);
    if (status != Drv::I2cStatus::I2C_OK) {
      this->log_WARNING_HI_ConfigError(status);
    }
  }

  void AccelGyro ::
    power(Fw::On powerState)
  {
    // Create a buffer/array of 2 elements each of which is 1 byte (for sending data over I2C)
    U8 data[REG_SIZE_BYTES * 2];  
    Fw::Buffer buffer(data, sizeof data);

    // Check if already on or already off
    if (powerState == this->m_power) {
      return;
    }
  
    // [0] = hardware register address
    // [1] = bits to set the register
    data[0] = POWER_MGMT_ADDR;
    data[1] = (powerState == Fw::On::ON) ? POWER_ON : POWER_OFF;

    // send power commands to device over I2C
    Drv::I2cStatus powerStatus = this->write_out(0, this->m_I2cDevAddress, buffer);
    if (powerStatus != Drv::I2cStatus::I2C_OK) {          // check success
      this->log_WARNING_HI_PowerModeError(powerStatus);
    }
    else {
      this->m_power = powerState;

      if (powerState == Fw::On::ON) {
        config();
      }
    }
  }

  void AccelGyro ::
    updateAccel()
  {
    U8 data[MAX_DATA_SIZE];
    Fw::Buffer buffer(data, sizeof data);
    
    // reads the block of 6 registers from the MPU 6050 at accel's address
    Drv::I2cStatus status = readRegisterBlock(ACCEL_RAW_DATA_START, buffer);

    // verify successful read before processing data
    if ((status == Drv::I2cStatus::I2C_OK) && (buffer.getSize() == 6) && (buffer.getData() != nullptr)) {
      F32x3 vect = deserializeVector(buffer, accelScaleFactor);
      this->tlmWrite_accelerometer(vect);
    }
    else {
      this->log_WARNING_HI_TelemetryError(status);
    }

  }

  void AccelGyro ::
    updateGyro()
  {
    U8 data[MAX_DATA_SIZE];
    Fw::Buffer buffer(data, sizeof data);

    Drv::I2cStatus status = readRegisterBlock(GYRO_RAW_DATA_START, buffer);

    // verify successful read
    if ((status == Drv::I2cStatus::I2C_OK) && (buffer.getSize() == 6) && (buffer.getData() != nullptr)) {
      F32x3 vect = deserializeVector(buffer, gyroScaleFactor);
      this->tlmWrite_gyroscope(vect);
    }
    else {
      this->log_WARNING_HI_TelemetryError(status);
    }
  }

}
