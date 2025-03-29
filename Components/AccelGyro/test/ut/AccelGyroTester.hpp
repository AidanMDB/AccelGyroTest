// ======================================================================
// \title  AccelGyroTester.hpp
// \author aidandb
// \brief  hpp file for AccelGyro component test harness implementation class
// ======================================================================

#ifndef Components_AccelGyroTester_HPP
#define Components_AccelGyroTester_HPP

#include "Components/AccelGyro/AccelGyroGTestBase.hpp"
#include "Components/AccelGyro/AccelGyro.hpp"
#include "Fw/Types/SerialBuffer.hpp"


namespace Components {

  class AccelGyroTester :
    public AccelGyroGTestBase
  {

    public:

      // ----------------------------------------------------------------------
      // Constants
      // ----------------------------------------------------------------------

      static constexpr U16 READ_BUF_SIZE_BYTES = AccelGyro::MAX_DATA_SIZE;

      // Maximum size of histories storing events, telemetry, and port outputs
      static const FwSizeType MAX_HISTORY_SIZE = 10;

      // Instance ID supplied to the component instance under test
      static const FwEnumStoreType TEST_INSTANCE_ID = 0;

    public:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object AccelGyroTester
      AccelGyroTester();

      //! Destroy object AccelGyroTester
      ~AccelGyroTester();

    public:

      // ----------------------------------------------------------------------
      // Tests
      // ----------------------------------------------------------------------

      //! To do
      void testPowerOnOff();

      void testPowerError();

      void testSetupError();

      void testGetAccelTlm();

      void testTlmError();

      void testGetGyroTlm();


    private:

      // ----------------------------------------------------------------------
      // Handler for typed from ports
      // ----------------------------------------------------------------------

      // Handler for from_read
      Drv::I2cStatus from_read_handler (const NATIVE_INT_TYPE portNum,    // The port number
                                        U32 addr,                         // I2c slave device address
                                        Fw::Buffer& serBuffer             // Buffer with data to read/write from
      );

      // Handler for from_write
      Drv::I2cStatus from_write_handler (const NATIVE_INT_TYPE portNum,    // The port number
                                        U32 addr,                         // I2c slave device address
                                        Fw::Buffer& serBuffer             // Buffer with data to read/write from
      );

    private:

      // ----------------------------------------------------------------------
      // Helper functions
      // ----------------------------------------------------------------------

      //! Connect ports
      void connectPorts();

      //! Initialize components
      void initComponents();

    private:

      // ----------------------------------------------------------------------
      // Member variables
      // ----------------------------------------------------------------------
      
      //! The component under test
      AccelGyro component;

      // read status from driver
      Drv::I2cStatus m_readStatus;

      // write status from driver
      Drv::I2cStatus m_writeStatus;

      // buffer for storing address written
      U8 addrBuf;

      // buffer for storing accel data 
      U8 accelBuf[READ_BUF_SIZE_BYTES];

      // buffer for storing gyro data 
      U8 gyroBuf[READ_BUF_SIZE_BYTES];

      // serial buffer wrapping accelBuf
      Fw::SerialBuffer accelSerBuf;

      // serial buffer wrapping gyroBuf
      Fw::SerialBuffer gyroSerBuf;

  };

}

#endif
