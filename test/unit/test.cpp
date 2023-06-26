//
// Copyright 2023 BMW AG
// SPDX-License-Identifier: MPL-2.0
//

#include <cmath>

#include "../../src/MySensorModel.h"
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

    MySensorModel::RotatePointXYZ(x, y, z, yaw, pitch, roll, rx, ry, rz);

    EXPECT_NEAR(rx, -3.0, 0.000001);
    EXPECT_NEAR(ry, 5.0, 0.000001);
    EXPECT_NEAR(rz, 0.0, 0.000001);
}

TEST_F(ISensorModelTest, TranslationToVehicle)
{
    //global object position
    const double obj_x = 25.0;
    const double obj_y = 10.0;
    const double obj_z = 0.5;

    //global ego pose
    const double ego_x = 10.0;
    const double ego_y = 5.0;
    const double ego_z = 0.5;
    const double ego_yaw = 0.0 * M_PI / 180;
    const double ego_pitch = 0.0;
    const double ego_roll = 0.0;

    //ego attributes
    const double ego_bb_center_to_rear_x = -1.0;
    const double ego_bb_center_to_rear_y = 0.0;
    const double ego_bb_center_to_rear_z = -0.3;

    double rel_x = obj_x;
    double rel_y = obj_y;
    double rel_z = obj_z;

    MySensorModel::TransformCoordinateGlobalToVehicle(rel_x, rel_y, rel_z,
                                                         ego_x, ego_y, ego_z,
                                                         ego_yaw, ego_pitch, ego_roll,
                                                         ego_bb_center_to_rear_x, ego_bb_center_to_rear_y, ego_bb_center_to_rear_z);

    EXPECT_NEAR(rel_x, 16.0, 0.000001);
    EXPECT_NEAR(rel_y, 5.0, 0.000001);
    EXPECT_NEAR(rel_z, 0.3, 0.000001);
}

TEST_F(ISensorModelTest, RotationToVehicle)
{
    //global object position
    const double obj_x = 25.0;
    const double obj_y = 25.0;
    const double obj_z = 0.5;

    //global ego pose
    const double ego_x = 10.0;
    const double ego_y = 10.0;
    const double ego_z = 0.5;
    const double ego_yaw = 45.0 * M_PI / 180;
    const double ego_pitch = 0.0;
    const double ego_roll = 0.0;

    //ego attributes
    const double ego_bb_center_to_rear_x = 0.0;
    const double ego_bb_center_to_rear_y = 0.0;
    const double ego_bb_center_to_rear_z = 0.0;

    double rel_x = obj_x;
    double rel_y = obj_y;
    double rel_z = obj_z;

    MySensorModel::TransformCoordinateGlobalToVehicle(rel_x, rel_y, rel_z,
                                                         ego_x, ego_y, ego_z,
                                                         ego_yaw, ego_pitch, ego_roll,
                                                         ego_bb_center_to_rear_x, ego_bb_center_to_rear_y, ego_bb_center_to_rear_z);

    EXPECT_NEAR(rel_x, sqrt(2*pow(15,2)), 0.000001);
    EXPECT_NEAR(rel_y, 0, 0.000001);
    EXPECT_NEAR(rel_z, 0, 0.000001);
}

TEST_F(ISensorModelTest, TranslationToSensor)
{
    //object position in vehicle coordinates
    const double obj_x = 25.0;
    const double obj_y = 10.0;
    const double obj_z = 0.0;

    //sensor mounting position with respect to vehicle coordinates
    const double mounting_position_x = 2.0;
    const double mounting_position_y = 0.2;
    const double mounting_position_z = -0.1;
    const double mounting_position_yaw = 0.0;
    const double mounting_position_pitch = 0.0;
    const double mounting_position_roll = 0.0;

    double rel_x = obj_x;
    double rel_y = obj_y;
    double rel_z = obj_z;

    MySensorModel::TransformCoordinateVehicleToSensor(rel_x, rel_y, rel_z,
                                                         mounting_position_x, mounting_position_y, mounting_position_z,
                                                         mounting_position_yaw, mounting_position_pitch, mounting_position_roll);

    EXPECT_NEAR(rel_x, 23.0, 0.000001);
    EXPECT_NEAR(rel_y, 9.8, 0.000001);
    EXPECT_NEAR(rel_z, 0.1, 0.000001);
}

TEST_F(ISensorModelTest, RotationToSensor)
{
    //object position in vehicle coordinates
    const double obj_x = 25.0;
    const double obj_y = 10.0;
    const double obj_z = 0.0;

    //sensor mounting position with respect to vehicle coordinates
    const double mounting_position_x = 0.0;
    const double mounting_position_y = 0.0;
    const double mounting_position_z = 0.0;
    const double mounting_position_yaw = 90.0 * M_PI / 180;
    const double mounting_position_pitch = 0.0;
    const double mounting_position_roll = 0.0;

    double rel_x = obj_x;
    double rel_y = obj_y;
    double rel_z = obj_z;

    MySensorModel::TransformCoordinateVehicleToSensor(rel_x, rel_y, rel_z,
                                                         mounting_position_x, mounting_position_y, mounting_position_z,
                                                         mounting_position_yaw, mounting_position_pitch, mounting_position_roll);

    EXPECT_NEAR(rel_x, 10.0, 0.000001);
    EXPECT_NEAR(rel_y, -25, 0.000001);
    EXPECT_NEAR(rel_z, 0.0, 0.000001);
}

TEST_F(ISensorModelTest, TransformToSensor)
{
    //object position in vehicle coordinates
    const double obj_x = 25.0;
    const double obj_y = 10.0;
    const double obj_z = 0.0;

    //sensor mounting position with respect to vehicle coordinates
    const double mounting_position_x = 2.0;
    const double mounting_position_y = 0.2;
    const double mounting_position_z = -0.1;
    const double mounting_position_yaw = 90.0 * M_PI / 180;
    const double mounting_position_pitch = 0.0;
    const double mounting_position_roll = 0.0;

    double rel_x = obj_x;
    double rel_y = obj_y;
    double rel_z = obj_z;

    MySensorModel::TransformCoordinateVehicleToSensor(rel_x, rel_y, rel_z,
                                                         mounting_position_x, mounting_position_y, mounting_position_z,
                                                         mounting_position_yaw, mounting_position_pitch, mounting_position_roll);

    EXPECT_NEAR(rel_x, 9.8, 0.000001);
    EXPECT_NEAR(rel_y, -23, 0.000001);
    EXPECT_NEAR(rel_z, 0.1, 0.000001);
}