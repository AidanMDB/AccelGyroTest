// ======================================================================
// \title  AccelGyroTestMain.cpp
// \author aidandb
// \brief  cpp file for AccelGyro component test main function
// ======================================================================

#include "AccelGyroTester.hpp"

TEST(Nominal, powerOnOff) {
  Components::AccelGyroTester tester;
  tester.testPowerOnOff();
}

//TEST(Nominal, getAccelerometer) {
//  Components::AccelGyroTester tester;
//  tester.testGetAccelTlm();
//}

TEST(Error, powerError) {
  Components::AccelGyroTester tester;
  tester.testPowerError();
}

TEST(Error, setupError) {
  Components::AccelGyroTester tester;
  tester.testSetupError();
}

TEST(Nominal, accelTelemetry) {
  Components::AccelGyroTester tester;
  tester.testGetAccelTlm();
}

TEST(Nominal, gyroTelemetry) {
  Components::AccelGyroTester tester;
  tester.testGetGyroTlm();
}

TEST(Error, tlmError) {
  Components::AccelGyroTester tester;
  tester.testTlmError();
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
