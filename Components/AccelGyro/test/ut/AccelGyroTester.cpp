// ======================================================================
// \title  AccelGyroTester.cpp
// \author aidandb
// \brief  cpp file for AccelGyro component test harness implementation class
// ======================================================================

#include "AccelGyroTester.hpp"

// Testing framework provided by Fprime gives 
#include "STest/STest/Pick/Pick.hpp"

#define INSTANCE 0
#define ADDRESS_TEST Components::AccelGyro::I2cAddr::AD0_0

namespace Components {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  AccelGyroTester ::
    AccelGyroTester() :
      AccelGyroGTestBase("AccelGyroTester", AccelGyroTester::MAX_HISTORY_SIZE),
      component("AccelGyro"),
      addrBuf(0),
      accelSerBuf(this->accelBuf, sizeof this->accelBuf),
      gyroSerBuf(this->gyroBuf, sizeof this->gyroBuf)
  {
    memset(this->accelBuf, 0, sizeof this->accelBuf);
    memset(this->gyroBuf, 0, sizeof this->gyroBuf);
    this->initComponents();
    this->connectPorts();
    this->component.setup(ADDRESS_TEST);
  }

  AccelGyroTester ::
    ~AccelGyroTester()
  {

  }

  // ----------------------------------------------------------------------
  // Tests
  // ----------------------------------------------------------------------
  
  void AccelGyroTester ::
    testGetAccelTlm()
  {
    // turn on I2C device
    this->sendCmd_POWER_ON_OFF(0, 0, Fw::On::ON);
    ASSERT_EVENTS_PowerModeError_SIZE(0);
    this->invoke_to_Run(0, 0);

    // create array for storing accel values
    Components::F32x3 expectedVect;
    
    // loop through and collects x, y, and z outputs
    for (U32 j = 0; j < 3; j++) {
      I16 coords = 0;
      const auto status = this->accelSerBuf.deserialize(coords);
      EXPECT_EQ(status, Fw::FW_SERIALIZE_OK);
      const F32 f32Coord = static_cast<F32>(coords) / AccelGyro::accelScaleFactor;
      expectedVect[j] = f32Coord;

    }
    ASSERT_TLM_accelerometer_SIZE(1);
    ASSERT_TLM_accelerometer(0, expectedVect);
  }


  void AccelGyroTester ::
    testGetGyroTlm()
  {
    // turn on I2C device
    this->sendCmd_POWER_ON_OFF(0, 0, Fw::On::ON);
    ASSERT_EVENTS_PowerModeError_SIZE(0);
    this->invoke_to_Run(0, 0);

    // create array for storing accel values
    Components::F32x3 expectedVect;
    
    // loop through and collects x, y, and z outputs
    for (U32 j = 0; j < 3; j++) {
      I16 coords = 0;
      const auto status = this->gyroSerBuf.deserialize(coords);
      EXPECT_EQ(status, Fw::FW_SERIALIZE_OK);
      const F32 f32Coord = static_cast<F32>(coords) / AccelGyro::gyroScaleFactor;
      expectedVect[j] = f32Coord;

    }
    ASSERT_TLM_gyroscope_SIZE(1);
    ASSERT_TLM_gyroscope(0, expectedVect);
  }


  void AccelGyroTester ::
    testPowerOnOff()
  {
    // power ON
    this->sendCmd_POWER_ON_OFF(0, 0, Fw::On::ON);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, AccelGyro::OPCODE_POWER_ON_OFF, 0, Fw::CmdResponse::OK);

    // power OFF
    this->sendCmd_POWER_ON_OFF(0, 0, Fw::On::ON);
    ASSERT_CMD_RESPONSE_SIZE(2);
    ASSERT_CMD_RESPONSE(0, AccelGyro::OPCODE_POWER_ON_OFF, 0, Fw::CmdResponse::OK);
  }


  void AccelGyroTester ::
    testPowerError()
  {
    // It's off, turning it off again shouldn't generate anything
    this->sendCmd_POWER_ON_OFF(0, 0, Fw::On::OFF);
    ASSERT_EVENTS_PowerModeError_SIZE(0);
    
    // Turn it on and should receive a power mode Error
    this->m_writeStatus = Drv::I2cStatus::I2C_WRITE_ERR;
    this->sendCmd_POWER_ON_OFF(0, 0, Fw::On::ON);
    ASSERT_EVENTS_PowerModeError_SIZE(1);
    ASSERT_EVENTS_ConfigError_SIZE(0);
    ASSERT_EVENTS_PowerModeError(0, this->m_writeStatus);
  }


  void AccelGyroTester ::
    testSetupError()
  {
    this->m_writeStatus = Drv::I2cStatus::I2C_ADDRESS_ERR;
    this->sendCmd_POWER_ON_OFF(0, 0, Fw::On::ON);
    ASSERT_EVENTS_ConfigError_SIZE(2);
    ASSERT_EVENTS_ConfigError(0, this->m_writeStatus);
  }




  void AccelGyroTester ::
    testTlmError()
  {
    this->sendCmd_POWER_ON_OFF(0, 0, Fw::On::ON);
    ASSERT_EVENTS_PowerModeError_SIZE(0);

    this->m_readStatus = Drv::I2cStatus::I2C_OTHER_ERR;
    this->m_writeStatus = Drv::I2cStatus::I2C_OTHER_ERR;
    this->invoke_to_Run(0, 0);
    ASSERT_EVENTS_TelemetryError_SIZE(2);
    ASSERT_EVENTS_TelemetryError(0, this->m_readStatus);
    ASSERT_EVENTS_TelemetryError(0, this->m_readStatus);
  }

  // ----------------------------------------------------------------------
  // Handlers for typed from ports
  // ----------------------------------------------------------------------

  Drv::I2cStatus AccelGyroTester::
    from_read_handler(const NATIVE_INT_TYPE portNum, U32 addr, Fw::Buffer& serBuffer) 
  {
    this->pushFromPortEntry_read(addr, serBuffer);
    EXPECT_EQ(addr, ADDRESS_TEST);

    if (this->m_readStatus == Drv::I2cStatus::I2C_OK) {
      // fill buffer with random data
      U8* const data = serBuffer.getData();
      const U32 size = serBuffer.getSize();
      const U32 accelGyro_max_data_size = AccelGyro::MAX_DATA_SIZE;

      EXPECT_LE(size, accelGyro_max_data_size);

      // fill each byte with random data
      for (int i = 0; i < size; i++) {
        const U8 byte = STest::Pick::any();
        data[i] = byte;
      }

      if (this->addrBuf == AccelGyro::ACCEL_RAW_DATA_START) {
        // Address write is the accelerometer register
        // so copy data into that buffer
        this->accelSerBuf.resetSer();
        const auto status = this->accelSerBuf.pushBytes(&data[0], size);
        EXPECT_EQ(status, Fw::FW_SERIALIZE_OK);
      }

      if (this->addrBuf == AccelGyro::GYRO_RAW_DATA_START) {
        // Address write was the gyroscope register
        // so copy data into that buffer
        this->gyroSerBuf.resetSer();
        const auto status = this->gyroSerBuf.pushBytes(&data[0], size);
        EXPECT_EQ(status, Fw::FW_SERIALIZE_OK);
      }
    }

    return this->m_readStatus;
  }


  Drv::I2cStatus AccelGyroTester::
    from_write_handler(const NATIVE_INT_TYPE portNum, U32 addr, Fw::Buffer& serBuffer)
  {
    this->pushFromPortEntry_write(addr, serBuffer);
    EXPECT_EQ(addr, ADDRESS_TEST);

    const U32 size = serBuffer.getSize();
    EXPECT_GT(size, 0);

    U8* const data = (U8*)serBuffer.getData();
    Drv::I2cStatus status = Drv::I2cStatus::I2C_OK;

    if (size == 1) {
      // address write so store the address in the address buffer
      this->addrBuf = data[0];
    }

    if (this->m_writeStatus == Drv::I2cStatus::I2C_ADDRESS_ERR) {
      // If the write status indicates an address error, then return
      // OK this time and setup the write status for a write error next time
      this->m_writeStatus = Drv::I2cStatus::I2C_WRITE_ERR;
    }
    else {
      // Otherwise return the write status
      status = this->m_writeStatus;
    }
    return status;
  }
}
