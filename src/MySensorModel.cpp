//
// Copyright 2016 -- 2018 PMSF IT Consulting Pierre R. Mai
// Copyright 2023 BMW AG
// SPDX-License-Identifier: MPL-2.0
// The MPL-2.0 is only an example here. You can choose any other open source license accepted by OpenMSL.
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
    osi3::Identifier ego_id = current_in.global_ground_truth().host_vehicle_id();
    NormalLog("OSI", "Looking for EgoVehicle with ID: %llu", ego_id.value());
    std::for_each(current_in.global_ground_truth().moving_object().begin(),
                  current_in.global_ground_truth().moving_object().end(),
                  [this, ego_id, &ego_x, &ego_y, &ego_z, &ego_yaw, &ego_pitch, &ego_roll](const osi3::MovingObject& obj) {
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
    std::for_each(current_in.global_ground_truth().moving_object().begin(),
                  current_in.global_ground_truth().moving_object().end(),
                  [this, &i, &current_in, &current_out, ego_id, ego_x, ego_y, ego_z, ego_yaw, ego_pitch, ego_roll, actual_range](const osi3::MovingObject& veh) {
                      if (veh.id().value() != ego_id.value())
                      {
                          // NOTE: We currently do not take sensor mounting position into account,
                          // i.e. sensor-relative coordinates are relative to center of bounding box
                          // of ego vehicle currently.
                          double trans_x = veh.base().position().x() - ego_x;
                          double trans_y = veh.base().position().y() - ego_y;
                          double trans_z = veh.base().position().z() - ego_z;
                          double rel_x = NAN;
                          double rel_y = NAN;
                          double rel_z = NAN;
                          RotatePoint(trans_x, trans_y, trans_z, ego_yaw, ego_pitch, ego_roll, rel_x, rel_y, rel_z);
                          double distance = sqrt(rel_x * rel_x + rel_y * rel_y + rel_z * rel_z);
                          const double azimuth_fov = 0.866025;
                          if ((distance <= actual_range) && (rel_x / distance > azimuth_fov))
                          {
                              osi3::DetectedMovingObject* obj = current_out.mutable_moving_object()->Add();
                              obj->mutable_header()->add_ground_truth_id()->CopyFrom(veh.id());
                              obj->mutable_header()->mutable_tracking_id()->set_value(i);
                              obj->mutable_header()->set_existence_probability(cos((2.0 * distance - actual_range) / actual_range));
                              obj->mutable_header()->set_measurement_state(osi3::DetectedItemHeader_MeasurementState_MEASUREMENT_STATE_MEASURED);
                              obj->mutable_header()->add_sensor_id()->CopyFrom(current_in.sensor_id());
                              obj->mutable_base()->mutable_position()->set_x(rel_x);
                              obj->mutable_base()->mutable_position()->set_y(rel_y);
                              obj->mutable_base()->mutable_position()->set_z(rel_z);
                              obj->mutable_base()->mutable_dimension()->set_length(veh.base().dimension().length());
                              obj->mutable_base()->mutable_dimension()->set_width(veh.base().dimension().width());
                              obj->mutable_base()->mutable_dimension()->set_height(veh.base().dimension().height());

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

void MySensorModel::TransposeRotationMatrix(double matrix_in[3][3], double matrix_trans[3][3])
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            matrix_trans[j][i] = matrix_in[i][j];
        }
    }
}

void MySensorModel::RotatePoint(double x, double y, double z, double yaw, double pitch, double roll, double& rx, double& ry, double& rz)
{
    double matrix[3][3];
    double cos_yaw = cos(yaw);
    double cos_pitch = cos(pitch);
    double cos_roll = cos(roll);
    double sin_yaw = sin(yaw);
    double sin_pitch = sin(pitch);
    double sin_roll = sin(roll);

    matrix[0][0] = cos_yaw * cos_pitch;
    matrix[0][1] = cos_yaw * sin_pitch * sin_roll - sin_yaw * cos_roll;
    matrix[0][2] = cos_yaw * sin_pitch * cos_roll + sin_yaw * sin_roll;
    matrix[1][0] = sin_yaw * cos_pitch;
    matrix[1][1] = sin_yaw * sin_pitch * sin_roll + cos_yaw * cos_roll;
    matrix[1][2] = sin_yaw * sin_pitch * cos_roll - cos_yaw * sin_roll;
    matrix[2][0] = -sin_pitch;
    matrix[2][1] = cos_pitch * sin_roll;
    matrix[2][2] = cos_pitch * cos_roll;

    double matrix_trans[3][3];
    TransposeRotationMatrix(matrix, matrix_trans);

    rx = matrix_trans[0][0] * x + matrix_trans[0][1] * y + matrix_trans[0][2] * z;
    ry = matrix_trans[1][0] * x + matrix_trans[1][1] * y + matrix_trans[1][2] * z;
    rz = matrix_trans[2][0] * x + matrix_trans[2][1] * y + matrix_trans[2][2] * z;
}