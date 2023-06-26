//
// Copyright 2016 -- 2018 PMSF IT Consulting Pierre R. Mai
// Copyright 2023 BMW AG
// SPDX-License-Identifier: MPL-2.0
// The MPL-2.0 is only an example here. You can choose any other open source license accepted by OpenMSL, or any other license if this template is used elsewhere.
//

#include "MySensorModel.h"

#include <algorithm>
#include <cmath>

#include "osi_sensordata.pb.h"
#include "osi_sensorview.pb.h"

void MySensorModel::Init(double nominal_range_in)
{
    nominal_range_ = nominal_range_in;
}

osi3::SensorData MySensorModel::Step(osi3::SensorView current_in, double time)
{
    osi3::SensorData current_out;

    double ego_x = 0;
    double ego_y = 0;
    double ego_z = 0;
    double ego_yaw = 0;
    double ego_pitch = 0;
    double ego_roll = 0;
    double ego_bb_center_to_rear_x = 0;
    double ego_bb_center_to_rear_y = 0;
    double ego_bb_center_to_rear_z = 0;
    osi3::Identifier ego_id = current_in.global_ground_truth().host_vehicle_id();
    NormalLog("OSI", "Looking for EgoVehicle with ID: %llu", ego_id.value());
    std::for_each(current_in.global_ground_truth().moving_object().begin(),
                  current_in.global_ground_truth().moving_object().end(),
                  [this, ego_id, &ego_x, &ego_y, &ego_z, &ego_yaw, &ego_pitch, &ego_roll, &ego_bb_center_to_rear_x, &ego_bb_center_to_rear_y, &ego_bb_center_to_rear_z](
                      const osi3::MovingObject& obj) {
                      NormalLog("OSI", "MovingObject with ID %llu is EgoVehicle: %d", obj.id().value(), static_cast<int>(obj.id().value() == ego_id.value()));
                      if (obj.id().value() == ego_id.value())
                      {
                          NormalLog("OSI", "Found EgoVehicle with ID: %llu", obj.id().value());
                          ego_x = obj.base().position().x();
                          ego_y = obj.base().position().y();
                          ego_z = obj.base().position().z();
                          ego_yaw = obj.base().orientation().yaw();
                          ego_pitch = obj.base().orientation().pitch();
                          ego_roll = obj.base().orientation().roll();
                          ego_bb_center_to_rear_x = obj.vehicle_attributes().bbcenter_to_rear().x();
                          ego_bb_center_to_rear_y = obj.vehicle_attributes().bbcenter_to_rear().y();
                          ego_bb_center_to_rear_z = obj.vehicle_attributes().bbcenter_to_rear().z();
                      }
                  });
    NormalLog("OSI", "Current Ego Position: %f,%f,%f", ego_x, ego_y, ego_z);

    /* Clear Output */
    current_out.Clear();
    current_out.mutable_version()->CopyFrom(osi3::InterfaceVersion::descriptor()->file()->options().GetExtension(osi3::current_interface_version));
    /* Adjust Timestamps and Ids */
    current_out.mutable_timestamp()->set_seconds((long long int)std::floor(time));
    const double nano_seconds = 1000000000.0;
    current_out.mutable_timestamp()->set_nanos((int)((time - std::floor(time)) * nano_seconds));
    /* Copy of SensorView */
    current_out.add_sensor_view()->CopyFrom(current_in);

    int i = 0;
    const double range_factor = 1.1;
    double actual_range = nominal_range_ * range_factor;
    std::for_each(
        current_in.global_ground_truth().moving_object().begin(),
        current_in.global_ground_truth().moving_object().end(),
        [this,
         &i,
         &current_in,
         &current_out,
         ego_id,
         ego_x,
         ego_y,
         ego_z,
         ego_yaw,
         ego_pitch,
         ego_roll,
         ego_bb_center_to_rear_x,
         ego_bb_center_to_rear_y,
         ego_bb_center_to_rear_z,
         actual_range](const osi3::MovingObject& veh) {
            if (veh.id().value() != ego_id.value())
            {
                double rel_x = veh.base().position().x();
                double rel_y = veh.base().position().y();
                double rel_z = veh.base().position().z();
                /* transform object coordinate to vehicle coordinate system */
                TransformCoordinateGlobalToVehicle(
                    rel_x, rel_y, rel_z, ego_x, ego_y, ego_z, ego_yaw, ego_pitch, ego_roll, ego_bb_center_to_rear_x, ego_bb_center_to_rear_y, ego_bb_center_to_rear_z);
                /* transform vehicle-relative coordinate to (virtual) sensor-relative coordinate */
                TransformCoordinateVehicleToSensor(rel_x,
                                                   rel_y,
                                                   rel_z,
                                                   current_out.mounting_position().position().x(),
                                                   current_out.mounting_position().position().y(),
                                                   current_out.mounting_position().position().z(),
                                                   current_out.mounting_position().orientation().yaw(),
                                                   current_out.mounting_position().orientation().pitch(),
                                                   current_out.mounting_position().orientation().roll());
                double distance = sqrt(rel_x * rel_x + rel_y * rel_y + rel_z * rel_z);
                const double fov_thres = 0.866025;
                if ((distance <= actual_range) && (rel_x / distance > fov_thres))
                {
                    osi3::DetectedMovingObject* obj = current_out.mutable_moving_object()->Add();
                    obj->mutable_header()->add_ground_truth_id()->CopyFrom(veh.id());
                    obj->mutable_header()->mutable_tracking_id()->set_value(i);
                    obj->mutable_header()->set_existence_probability(cos((2 * distance - actual_range) / actual_range));
                    obj->mutable_header()->set_measurement_state(osi3::DetectedItemHeader_MeasurementState_MEASUREMENT_STATE_MEASURED);
                    obj->mutable_header()->add_sensor_id()->CopyFrom(current_in.sensor_id());
                    obj->mutable_base()->mutable_position()->set_x(rel_x);
                    obj->mutable_base()->mutable_position()->set_y(rel_y);
                    obj->mutable_base()->mutable_position()->set_z(rel_z);
                    obj->mutable_base()->mutable_dimension()->set_length(veh.base().dimension().length());
                    obj->mutable_base()->mutable_dimension()->set_width(veh.base().dimension().width());
                    obj->mutable_base()->mutable_dimension()->set_height(veh.base().dimension().height());
                    obj->mutable_base()->mutable_orientation()->set_yaw(veh.base().orientation().yaw() - ego_yaw - current_out.mounting_position().orientation().yaw());
                    obj->mutable_base()->mutable_orientation()->set_pitch(veh.base().orientation().pitch() - ego_pitch - current_out.mounting_position().orientation().pitch());
                    obj->mutable_base()->mutable_orientation()->set_roll(veh.base().orientation().roll() - ego_roll - current_out.mounting_position().orientation().roll());

                    osi3::DetectedMovingObject::CandidateMovingObject* candidate = obj->add_candidate();
                    candidate->set_type(veh.type());
                    candidate->mutable_vehicle_classification()->CopyFrom(veh.vehicle_classification());
                    candidate->set_probability(1);

                    NormalLog("OSI",
                              "Output Vehicle %d[%llu] Probability %f Relative Position: %f,%f,%f (%f,%f,%f)",
                              i,
                              veh.id().value(),
                              obj->header().existence_probability(),
                              rel_x,
                              rel_y,
                              rel_z,
                              obj->base().position().x(),
                              obj->base().position().y(),
                              obj->base().position().z());
                    i++;
                }
                else
                {
                    NormalLog("OSI",
                              "Ignoring Vehicle %d[%llu] Outside Sensor Scope Relative Position: %f,%f,%f (%f,%f,%f)",
                              i,
                              veh.id().value(),
                              veh.base().position().x() - ego_x,
                              veh.base().position().y() - ego_y,
                              veh.base().position().z() - ego_z,
                              veh.base().position().x(),
                              veh.base().position().y(),
                              veh.base().position().z());
                }
            }
            else
            {
                NormalLog("OSI",
                          "Ignoring EGO Vehicle %d[%llu] Relative Position: %f,%f,%f (%f,%f,%f)",
                          i,
                          veh.id().value(),
                          veh.base().position().x() - ego_x,
                          veh.base().position().y() - ego_y,
                          veh.base().position().z() - ego_z,
                          veh.base().position().x(),
                          veh.base().position().y(),
                          veh.base().position().z());
            }
        });
    NormalLog("OSI", "Mapped %d vehicles to output", i);
    return current_out;
}

/**
 * @brief Rotate point with following order of rotation:
 * 1. roll (around x-axis) 2. pitch (around y-axis) 3. yaw (around z-axis);
 *
 * Positive rotation is counter clockwise (right-hand rule).
 */
void MySensorModel::RotatePointXYZ(double x, double y, double z, double yaw, double pitch, double roll, double& rx, double& ry, double& rz)
{
    double matrix[3][3];
    double cos_yaw = cos(yaw);
    double cos_pitch = cos(pitch);
    double cos_roll = cos(roll);
    double sin_yaw = sin(yaw);
    double sin_pitch = sin(pitch);
    double sin_roll = sin(roll);

    matrix[0][0] = cos_pitch * cos_yaw;
    matrix[0][1] = -cos_pitch * sin_yaw;
    matrix[0][2] = sin_pitch;
    matrix[1][0] = sin_roll * sin_pitch * cos_yaw + cos_roll * sin_yaw;
    matrix[1][1] = -sin_roll * sin_pitch * sin_yaw + cos_roll * cos_yaw;
    matrix[1][2] = -sin_roll * cos_pitch;
    matrix[2][0] = -cos_roll * sin_pitch * cos_yaw + sin_roll * sin_yaw;
    matrix[2][1] = cos_roll * sin_pitch * sin_yaw + sin_roll * cos_yaw;
    matrix[2][2] = cos_roll * cos_pitch;

    rx = matrix[0][0] * x + matrix[0][1] * y + matrix[0][2] * z;
    ry = matrix[1][0] * x + matrix[1][1] * y + matrix[1][2] * z;
    rz = matrix[2][0] * x + matrix[2][1] * y + matrix[2][2] * z;
}

/**
 * @brief Transform global OSI ground truth coordinate to vehicle coordinate
 * system (origin of vehicle coordinate system: center rear axle).
 */
void MySensorModel::TransformCoordinateGlobalToVehicle(double& rx,
                                                       double& ry,
                                                       double& rz,
                                                       double ego_x,
                                                       double ego_y,
                                                       double ego_z,
                                                       double ego_yaw,
                                                       double ego_pitch,
                                                       double ego_roll,
                                                       double ego_bbcenter_to_rear_x,
                                                       double ego_bbcenter_to_rear_y,
                                                       double ego_bbcenter_to_rear_z)
{
    /* subtract global ego vehicle position from global coordinate */
    rx = rx - ego_x;
    ry = ry - ego_y;
    rz = rz - ego_z;

    /* rotate by negative ego vehicle orientation */
    RotatePointXYZ(rx, ry, rz, -ego_yaw, -ego_pitch, -ego_roll, rx, ry, rz);

    /* subtract center of rear axle position */
    rx = rx - ego_bbcenter_to_rear_x;
    ry = ry - ego_bbcenter_to_rear_y;
    rz = rz - ego_bbcenter_to_rear_z;
}

/**
 * @brief Transform coordinate from vehicle coordinate system to
 * virtual/physical sensor coordinate system.
 */
void MySensorModel::TransformCoordinateVehicleToSensor(double& rx,
                                                       double& ry,
                                                       double& rz,
                                                       double mounting_position_x,
                                                       double mounting_position_y,
                                                       double mounting_position_z,
                                                       double mounting_position_yaw,
                                                       double mounting_position_pitch,
                                                       double mounting_position_roll)
{
    /* subtract virtual/physical sensor mounting position */
    rx = rx - mounting_position_x;
    ry = ry - mounting_position_y;
    rz = rz - mounting_position_z;

    /* rotate by negative virtual/physical sensor mounting orientation */
    RotatePointXYZ(rx, ry, rz, -mounting_position_yaw, -mounting_position_pitch, -mounting_position_roll, rx, ry, rz);
}