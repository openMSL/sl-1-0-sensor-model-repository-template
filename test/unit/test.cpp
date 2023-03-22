//
// Copyright 2023 BMW AG
// SPDX-License-Identifier: MPL-2.0
//

#include <cmath>

#include "../../src/HelloWorldSensor.h"
#include "gtest/gtest.h"

class ISensorModelTest : public ::testing::Test
{
};

TEST_F(ISensorModelTest, YawRotation)
{

    const double x = 5.0;
    const double y = 3.0;
    const double z = 0.0;
    const double yaw = 90.0 * M_PI / 180;
    const double pitch = 0.0;
    const double roll = 0.0;

    double rx = 0.0;
    double ry = 0.0;
    double rz = 0.0;

    HelloWorldSensor::RotatePoint(x, y, z, yaw, pitch, roll, rx, ry, rz);

    EXPECT_NEAR(rx, 3.0, 0.000001);
    EXPECT_NEAR(ry, -5.0, 0.000001);
    EXPECT_NEAR(rz, 0.0, 0.000001);
}