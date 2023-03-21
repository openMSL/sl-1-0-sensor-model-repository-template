//
// Copyright 2016 -- 2018 PMSF IT Consulting Pierre R. Mai
// Copyright 2023 BMW AG
// SPDX-License-Identifier: MPL-2.0
//

#include "HelloWorldSensor.h"

/*
 * Debug Breaks
 *
 * If you define DEBUG_BREAKS the FMU will automatically break
 * into an attached Debugger on all major computation functions.
 * Note that the FMU is likely to break all environments if no
 * Debugger is actually attached when the breaks are triggered.
 */
#if defined(DEBUG_BREAKS) && !defined(NDEBUG)
#if defined(__has_builtin) && !defined(__ibmxl__)
#if __has_builtin(__builtin_debugtrap)
#define DEBUGBREAK() __builtin_debugtrap()
#elif __has_builtin(__debugbreak)
#define DEBUGBREAK() __debugbreak()
#endif
#endif
#if !defined(DEBUGBREAK)
#if defined(_MSC_VER) || defined(__INTEL_COMPILER)
#include <intrin.h>
#define DEBUGBREAK() __debugbreak()
#else
#include <signal.h>
#if defined(SIGTRAP)
#define DEBUGBREAK() raise(SIGTRAP)
#else
#define DEBUGBREAK() raise(SIGABRT)
#endif
#endif
#endif
#else
#define DEBUGBREAK()
#endif

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <string>

using namespace std;

#ifdef PRIVATE_LOG_PATH
ofstream COSMPDummySensor::private_log_file;
#endif

/*
 * ProtocolBuffer Accessors
 */

void* DecodeIntegerToPointer(fmi2Integer hi, fmi2Integer lo)
{
#if PTRDIFF_MAX == INT64_MAX
    union Addrconv
    {
        struct
        {
            int lo;
            int hi;
        } base;
        unsigned long long address;
    } myaddr;
    myaddr.base.lo = lo;
    myaddr.base.hi = hi;
    return reinterpret_cast<void*>(myaddr.address);
#elif PTRDIFF_MAX == INT32_MAX
    return reinterpret_cast<void*>(lo);
#else
#error "Cannot determine 32bit or 64bit environment!"
#endif
}

void EncodePointerToInteger(const void* ptr, fmi2Integer& hi, fmi2Integer& lo)
{
#if PTRDIFF_MAX == INT64_MAX
    union Addrconv
    {
        struct
        {
            int lo;
            int hi;
        } base;
        unsigned long long address;
    } myaddr;
    myaddr.address = reinterpret_cast<unsigned long long>(ptr);
    hi = myaddr.base.hi;
    lo = myaddr.base.lo;
#elif PTRDIFF_MAX == INT32_MAX
    hi = 0;
    lo = reinterpret_cast<int>(ptr);
#else
#error "Cannot determine 32bit or 64bit environment!"
#endif
}

bool HelloWorldSensor::GetFmiSensorViewConfig(osi3::SensorViewConfiguration& data)
{
    if (integer_vars_[FMI_INTEGER_SENSORVIEW_CONFIG_SIZE_IDX] > 0)
    {
        void* buffer = DecodeIntegerToPointer(integer_vars_[FMI_INTEGER_SENSORVIEW_CONFIG_BASEHI_IDX], integer_vars_[FMI_INTEGER_SENSORVIEW_CONFIG_BASELO_IDX]);
        NormalLog(
            "OSMP", "Got %08X %08X, reading from %p ...", integer_vars_[FMI_INTEGER_SENSORVIEW_CONFIG_BASEHI_IDX], integer_vars_[FMI_INTEGER_SENSORVIEW_CONFIG_BASELO_IDX], buffer);
        data.ParseFromArray(buffer, integer_vars_[FMI_INTEGER_SENSORVIEW_CONFIG_SIZE_IDX]);
        return true;
    }
    return false;
}

void HelloWorldSensor::SetFmiSensorViewConfigRequest(const osi3::SensorViewConfiguration& data)
{
    data.SerializeToString(current_config_request_buffer_);
    EncodePointerToInteger(
        current_config_request_buffer_->data(), integer_vars_[FMI_INTEGER_SENSORVIEW_CONFIG_REQUEST_BASEHI_IDX], integer_vars_[FMI_INTEGER_SENSORVIEW_CONFIG_REQUEST_BASELO_IDX]);
    integer_vars_[FMI_INTEGER_SENSORVIEW_CONFIG_REQUEST_SIZE_IDX] = (fmi2Integer)current_config_request_buffer_->length();
    NormalLog("OSMP",
              "Providing %08X %08X, writing from %p ...",
              integer_vars_[FMI_INTEGER_SENSORVIEW_CONFIG_REQUEST_BASEHI_IDX],
              integer_vars_[FMI_INTEGER_SENSORVIEW_CONFIG_REQUEST_BASELO_IDX],
              current_config_request_buffer_->data());
    swap(current_config_request_buffer_, last_config_request_buffer_);
}

void HelloWorldSensor::ResetFmiSensorViewConfigRequest()
{
    integer_vars_[FMI_INTEGER_SENSORVIEW_CONFIG_REQUEST_SIZE_IDX] = 0;
    integer_vars_[FMI_INTEGER_SENSORVIEW_CONFIG_REQUEST_BASEHI_IDX] = 0;
    integer_vars_[FMI_INTEGER_SENSORVIEW_CONFIG_REQUEST_BASELO_IDX] = 0;
}

bool HelloWorldSensor::GetFmiSensorViewIn(osi3::SensorView& data)
{
    if (integer_vars_[FMI_INTEGER_SENSORVIEW_IN_SIZE_IDX] > 0)
    {
        void* buffer = DecodeIntegerToPointer(integer_vars_[FMI_INTEGER_SENSORVIEW_IN_BASEHI_IDX], integer_vars_[FMI_INTEGER_SENSORVIEW_IN_BASELO_IDX]);
        NormalLog("OSMP", "Got %08X %08X, reading from %p ...", integer_vars_[FMI_INTEGER_SENSORVIEW_IN_BASEHI_IDX], integer_vars_[FMI_INTEGER_SENSORVIEW_IN_BASELO_IDX], buffer);
        data.ParseFromArray(buffer, integer_vars_[FMI_INTEGER_SENSORVIEW_IN_SIZE_IDX]);
        return true;
    }
    return false;
}

void HelloWorldSensor::SetFmiSensorDataOut(const osi3::SensorData& data)
{
    data.SerializeToString(current_output_buffer_);
    EncodePointerToInteger(current_output_buffer_->data(), integer_vars_[FMI_INTEGER_SENSORDATA_OUT_BASEHI_IDX], integer_vars_[FMI_INTEGER_SENSORDATA_OUT_BASELO_IDX]);
    integer_vars_[FMI_INTEGER_SENSORDATA_OUT_SIZE_IDX] = (fmi2Integer)current_output_buffer_->length();
    NormalLog("OSMP",
              "Providing %08X %08X, writing from %p ...",
              integer_vars_[FMI_INTEGER_SENSORDATA_OUT_BASEHI_IDX],
              integer_vars_[FMI_INTEGER_SENSORDATA_OUT_BASELO_IDX],
              current_output_buffer_->data());
    swap(current_output_buffer_, last_output_buffer_);
}

void HelloWorldSensor::ResetFmiSensorDataOut()
{
    integer_vars_[FMI_INTEGER_SENSORDATA_OUT_SIZE_IDX] = 0;
    integer_vars_[FMI_INTEGER_SENSORDATA_OUT_BASEHI_IDX] = 0;
    integer_vars_[FMI_INTEGER_SENSORDATA_OUT_BASELO_IDX] = 0;
}

void HelloWorldSensor::RefreshFmiSensorViewConfigRequest()
{
    osi3::SensorViewConfiguration config;
    if (GetFmiSensorViewConfig(config))
    {
        SetFmiSensorViewConfigRequest(config);
    }
    else
    {
        config.Clear();
        config.mutable_version()->CopyFrom(osi3::InterfaceVersion::descriptor()->file()->options().GetExtension(osi3::current_interface_version));
        const double field_of_view = 3.14;
        const double range_factor = 1.1;
        const uint32_t update_nanos = 20000000;
        config.set_field_of_view_horizontal(field_of_view);
        config.set_field_of_view_vertical(field_of_view);
        config.set_range(FmiNominalRange() * range_factor);
        config.mutable_update_cycle_time()->set_seconds(0);
        config.mutable_update_cycle_time()->set_nanos(update_nanos);
        config.mutable_update_cycle_offset()->Clear();
        osi3::GenericSensorViewConfiguration* generic = config.add_generic_sensor_view_configuration();
        generic->set_field_of_view_horizontal(field_of_view);
        generic->set_field_of_view_vertical(field_of_view);
        SetFmiSensorViewConfigRequest(config);
    }
}

/*
 * Actual Core Content
 */

fmi2Status HelloWorldSensor::DoInit()
{

    /* Booleans */
    for (int& boolean_var : boolean_vars_)
    {
        boolean_var = fmi2False;
    }

    /* Integers */
    for (int& integer_var : integer_vars_)
    {
        integer_var = 0;
    }

    /* Reals */
    for (double& real_var : real_vars_)
    {
        real_var = 0.0;
    }

    /* Strings */
    for (auto& string_var : string_vars_)
    {
        string_var = "";
    }

    const double nominal_range = 135.0;
    SetFmiNominalRange(nominal_range);
    return fmi2OK;
}

fmi2Status HelloWorldSensor::DoStart(fmi2Boolean tolerance_defined, fmi2Real tolerance, fmi2Real start_time, fmi2Boolean stop_time_defined, fmi2Real stop_time)
{
    return fmi2OK;
}

fmi2Status HelloWorldSensor::DoEnterInitializationMode()
{
    return fmi2OK;
}

fmi2Status HelloWorldSensor::DoExitInitializationMode()
{
    osi3::SensorViewConfiguration config;
    if (!GetFmiSensorViewConfig(config))
    {
        NormalLog("OSI", "Received no valid SensorViewConfiguration from Simulation Environment, assuming everything checks out.");
    }
    else
    {
        NormalLog("OSI", "Received SensorViewConfiguration for Sensor Id %llu", config.sensor_id().value());
        NormalLog("OSI", "SVC Ground Truth FoV Horizontal %f, FoV Vertical %f, Range %f", config.field_of_view_horizontal(), config.field_of_view_vertical(), config.range());
        NormalLog("OSI",
                  "SVC Mounting Position: (%f, %f, %f)",
                  config.mounting_position().position().x(),
                  config.mounting_position().position().y(),
                  config.mounting_position().position().z());
        NormalLog("OSI",
                  "SVC Mounting Orientation: (%f, %f, %f)",
                  config.mounting_position().orientation().roll(),
                  config.mounting_position().orientation().pitch(),
                  config.mounting_position().orientation().yaw());
    }

    return fmi2OK;
}

void HelloWorldSensor::rotatePointXYZ(double x, double y, double z,
                    double yaw, double pitch, double roll,
                    double &rx, double &ry, double &rz)
{
    double matrix[3][3];
    double cos_yaw = cos(yaw);
    double cos_pitch = cos(pitch);
    double cos_roll = cos(roll);
    double sin_yaw = sin(yaw);
    double sin_pitch = sin(pitch);
    double sin_roll = sin(roll);

    /* order of rotation: roll (x-axis), pitch (y-axis), yaw (z-axis) */
    matrix[0][0] = cos_pitch*cos_yaw;                              matrix[0][1] = -cos_pitch*sin_yaw;                             matrix[0][2] = sin_pitch;
    matrix[1][0] = sin_roll*sin_pitch*cos_yaw + cos_roll*sin_yaw;  matrix[1][1] = -sin_roll*sin_pitch*sin_yaw + cos_roll*cos_yaw; matrix[1][2] = -sin_roll*cos_pitch;
    matrix[2][0] = -cos_roll*sin_pitch*cos_yaw + sin_roll*sin_yaw; matrix[2][1] = cos_roll*sin_pitch*sin_yaw + sin_roll*cos_yaw;  matrix[2][2] = cos_roll*cos_pitch;

    rx = matrix[0][0] * x + matrix[0][1] * y + matrix[0][2] * z;
    ry = matrix[1][0] * x + matrix[1][1] * y + matrix[1][2] * z;
    rz = matrix[2][0] * x + matrix[2][1] * y + matrix[2][2] * z;
}

/**
 * @brief Transform global OSI ground truth coordinate to vehicle coordinate
 * system (origin of vehicle coordinate system: center rear axle).
 */
void HelloWorldSensor::transformCoordinateGlobalToVehicle(double &rx, double &ry, double &rz,
                                        double ego_x, double ego_y, double ego_z,
                                        double ego_yaw, double ego_pitch, double ego_roll,
                                        double ego_bbcenter_to_rear_x, double ego_bbcenter_to_rear_y, double ego_bbcenter_to_rear_z)
{
    /* subtract global ego vehicle position from global coordinate */
    rx = rx-ego_x;
    ry = ry-ego_y;
    rz = rz-ego_z;

    /* rotate by negative ego vehicle orientation */
    rotatePointXYZ(rx, ry, rz,
                   -ego_yaw, -ego_pitch, -ego_roll,
                   rx, ry, rz);

    /* subtract center of rear axle position */
    rx = rx-ego_bbcenter_to_rear_x;
    ry = ry-ego_bbcenter_to_rear_y;
    rz = rz-ego_bbcenter_to_rear_z;
}

/**
 * @brief Transform coordinate from vehicle coordinate system to
 * virtual/physical sensor coordinate system.
 */
void HelloWorldSensor::transformCoordinateVehicleToSensor(double &rx, double &ry, double &rz,
                                        double mounting_position_x, double mounting_position_y, double mounting_position_z,
                                        double mounting_position_yaw, double mounting_position_pitch, double mounting_position_roll)
{
    /* subtract virtual/physical sensor mounting position */
    rx = rx-mounting_position_x;
    ry = ry-mounting_position_y;
    rz = rz-mounting_position_z;

    /* rotate by negative virtual/physical sensor mounting orientation */
    rotatePointXYZ(rx, ry, rz,
                   -mounting_position_yaw, -mounting_position_pitch, -mounting_position_roll,
                   rx, ry, rz);
}

fmi2Status HelloWorldSensor::DoCalc(fmi2Real current_communication_point, fmi2Real communication_step_size, fmi2Boolean no_set_fmu_state_prior_to_current_pointfmi_2_component)
{

    osi3::SensorView current_in;
    osi3::SensorData current_out;
    double time = current_communication_point + communication_step_size;
    NormalLog("OSI", "Calculating Sensor at %f for %f (step size %f)", current_communication_point, time, communication_step_size);
    if (GetFmiSensorViewIn(current_in))
    {
        double ego_x = 0;
        double ego_y = 0;
        double ego_z = 0;
        double ego_yaw = 0;
        double ego_pitch = 0;
        double ego_roll = 0;
        double ego_bb_center_to_rear_x=0;
        double ego_bb_center_to_rear_y=0;
        double ego_bb_center_to_rear_z=0;
        osi3::Identifier ego_id = current_in.global_ground_truth().host_vehicle_id();
        NormalLog("OSI", "Looking for EgoVehicle with ID: %llu", ego_id.value());
        for_each(current_in.global_ground_truth().moving_object().begin(),
                 current_in.global_ground_truth().moving_object().end(),
                 [this, ego_id, &ego_x, &ego_y, &ego_z, &ego_yaw, &ego_pitch, &ego_roll, &ego_bb_center_to_rear_x, &ego_bb_center_to_rear_y, &ego_bb_center_to_rear_z](const osi3::MovingObject& obj) {
                     NormalLog("OSI","MovingObject with ID %llu is EgoVehicle: %d",obj.id().value(), obj.id().value() == ego_id.value());
                     if (obj.id().value() == ego_id.value()) {
                         NormalLog("OSI","Found EgoVehicle with ID: %llu",obj.id().value());
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
        current_out.mutable_timestamp()->set_seconds((long long int)floor(time));
        const double nano_seconds = 1000000000.0;
        current_out.mutable_timestamp()->set_nanos((int)((time - floor(time)) * nano_seconds));
        /* Copy of SensorView */
        current_out.add_sensor_view()->CopyFrom(current_in);

        int i = 0;
        const double range_factor = 1.1;
        double actual_range = FmiNominalRange() * range_factor;
        for_each(current_in.global_ground_truth().moving_object().begin(),
                 current_in.global_ground_truth().moving_object().end(),
                 [this,&i,&current_in,&current_out,ego_id,ego_x,ego_y,ego_z,ego_yaw,ego_pitch,ego_roll,ego_bb_center_to_rear_x,ego_bb_center_to_rear_y,ego_bb_center_to_rear_z,actual_range](const osi3::MovingObject& veh) {
                     if (veh.id().value() != ego_id.value()) {
                         double rel_x, rel_y, rel_z;
                         rel_x = veh.base().position().x();
                         rel_y = veh.base().position().y();
                         rel_z = veh.base().position().z();
                         /* transform object coordinate to vehicle coordinate system */
                         transformCoordinateGlobalToVehicle(rel_x, rel_y, rel_z,
                                                            ego_x, ego_y, ego_z,
                                                            ego_yaw, ego_pitch, ego_roll,
                                                            ego_bb_center_to_rear_x, ego_bb_center_to_rear_y, ego_bb_center_to_rear_z);
                         /* transform vehicle-relative coordinate to (virtual) sensor-relative coordinate */
                         transformCoordinateVehicleToSensor(rel_x, rel_y, rel_z,
                                                            current_out.mounting_position().position().x(), current_out.mounting_position().position().y(), current_out.mounting_position().position().z(),
                                                            current_out.mounting_position().orientation().yaw(), current_out.mounting_position().orientation().pitch(), current_out.mounting_position().orientation().roll());
                         double distance = sqrt(rel_x*rel_x + rel_y*rel_y + rel_z*rel_z);
                         if ((distance <= actual_range) && (rel_x/distance > 0.866025)) {
                             osi3::DetectedMovingObject *obj = current_out.mutable_moving_object()->Add();
                             obj->mutable_header()->add_ground_truth_id()->CopyFrom(veh.id());
                             obj->mutable_header()->mutable_tracking_id()->set_value(i);
                             obj->mutable_header()->set_existence_probability(cos((2.0*distance-actual_range)/actual_range));
                             obj->mutable_header()->set_measurement_state(osi3::DetectedItemHeader_MeasurementState_MEASUREMENT_STATE_MEASURED);
                             obj->mutable_header()->add_sensor_id()->CopyFrom(current_in.sensor_id());
                             obj->mutable_base()->mutable_position()->set_x(rel_x);
                             obj->mutable_base()->mutable_position()->set_y(rel_y);
                             obj->mutable_base()->mutable_position()->set_z(rel_z);
                             obj->mutable_base()->mutable_dimension()->set_length(veh.base().dimension().length());
                             obj->mutable_base()->mutable_dimension()->set_width(veh.base().dimension().width());
                             obj->mutable_base()->mutable_dimension()->set_height(veh.base().dimension().height());
                             obj->mutable_base()->mutable_orientation()->set_yaw(veh.base().orientation().yaw()-ego_yaw-current_out.mounting_position().orientation().yaw());
                             obj->mutable_base()->mutable_orientation()->set_pitch(veh.base().orientation().pitch()-ego_pitch-current_out.mounting_position().orientation().pitch());
                             obj->mutable_base()->mutable_orientation()->set_roll(veh.base().orientation().roll()-ego_roll-current_out.mounting_position().orientation().roll());

                             osi3::DetectedMovingObject::CandidateMovingObject* candidate = obj->add_candidate();
                             candidate->set_type(veh.type());
                             candidate->mutable_vehicle_classification()->CopyFrom(veh.vehicle_classification());
                             candidate->set_probability(1);

                             NormalLog("OSI","Output Vehicle %d[%llu] Probability %f Relative Position: %f,%f,%f (%f,%f,%f)",i,veh.id().value(),obj->header().existence_probability(),rel_x,rel_y,rel_z,obj->base().position().x(),obj->base().position().y(),obj->base().position().z());
                             i++;
                         } else {
                             NormalLog("OSI","Ignoring Vehicle %d[%llu] Outside Sensor Scope Relative Position: %f,%f,%f (%f,%f,%f)",i,veh.id().value(),veh.base().position().x()-ego_x,veh.base().position().y()-ego_y,veh.base().position().z()-ego_z,veh.base().position().x(),veh.base().position().y(),veh.base().position().z());
                         }
                     }
                     else
                     {
                         NormalLog("OSI","Ignoring EGO Vehicle %d[%llu] Relative Position: %f,%f,%f (%f,%f,%f)",i,veh.id().value(),veh.base().position().x()-ego_x,veh.base().position().y()-ego_y,veh.base().position().z()-ego_z,veh.base().position().x(),veh.base().position().y(),veh.base().position().z());
                     }
                 });
        NormalLog("OSI", "Mapped %d vehicles to output", i);
        /* Serialize */
        SetFmiSensorDataOut(current_out);
        SetFmiValid(1);
        SetFmiCount(current_out.moving_object_size());
    }
    else
    {
        /* We have no valid input, so no valid output */
        NormalLog("OSI", "No valid input, therefore providing no valid output.");
        ResetFmiSensorDataOut();
        SetFmiValid(0);
        SetFmiCount(0);
    }
    return fmi2OK;
}

fmi2Status HelloWorldSensor::DoTerm()
{
    return fmi2OK;
}

void HelloWorldSensor::DoFree() {}

/*
 * Generic C++ Wrapper Code
 */

HelloWorldSensor::HelloWorldSensor(fmi2String theinstance_name,
                                   fmi2Type thefmu_type,
                                   fmi2String thefmu_guid,
                                   fmi2String thefmu_resource_location,
                                   const fmi2CallbackFunctions* thefunctions,
                                   fmi2Boolean thevisible,
                                   fmi2Boolean thelogging_on)
    : instance_name_(theinstance_name),
      fmu_type_(thefmu_type),
      fmu_guid_(thefmu_guid),
      fmu_resource_location_(thefmu_resource_location),
      functions_(*thefunctions),
      visible_(thevisible != 0),
      logging_on_(thelogging_on != 0),
      simulation_started_(false),
      current_output_buffer_(new string()),
      last_output_buffer_(new string()),
      current_config_request_buffer_(new string()),
      last_config_request_buffer_(new string())
{
    logging_categories_.clear();
    logging_categories_.insert("FMI");
    logging_categories_.insert("OSMP");
    logging_categories_.insert("OSI");
}

fmi2Status HelloWorldSensor::SetDebugLogging(fmi2Boolean thelogging_on, size_t n_categories, const fmi2String categories[])
{
    FmiVerboseLog("fmi2SetDebugLogging(%s)", thelogging_on != 0 ? "true" : "false");
    logging_on_ = thelogging_on != 0;
    if ((categories != nullptr) && (n_categories > 0))
    {
        logging_categories_.clear();
        for (size_t i = 0; i < n_categories; i++)
        {
            if (0 == strcmp(categories[i], "FMI"))
            {
                logging_categories_.insert("FMI");
            }
            else if (0 == strcmp(categories[i], "OSMP"))
            {
                logging_categories_.insert("OSMP");
            }
            else if (0 == strcmp(categories[i], "OSI"))
            {
                logging_categories_.insert("OSI");
            }
        }
    }
    else
    {
        logging_categories_.clear();
        logging_categories_.insert("FMI");
        logging_categories_.insert("OSMP");
        logging_categories_.insert("OSI");
    }
    return fmi2OK;
}

fmi2Component HelloWorldSensor::Instantiate(fmi2String instance_name,
                                            fmi2Type fmu_type,
                                            fmi2String fmu_guid,
                                            fmi2String fmu_resource_location,
                                            const fmi2CallbackFunctions* functions,
                                            fmi2Boolean visible,
                                            fmi2Boolean logging_on)
{
    auto* myc = new HelloWorldSensor(instance_name, fmu_type, fmu_guid, fmu_resource_location, functions, visible, logging_on);

    if (myc == nullptr)
    {
        FmiVerboseLogGlobal(R"(fmi2Instantiate("%s",%d,"%s","%s","%s",%d,%d) = NULL (alloc failure))",
                            instance_name,
                            fmu_type,
                            fmu_guid,
                            (fmu_resource_location != nullptr) ? fmu_resource_location : "<NULL>",
                            "FUNCTIONS",
                            visible,
                            logging_on);
        return nullptr;
    }

    if (myc->DoInit() != fmi2OK)
    {
        FmiVerboseLogGlobal(R"(fmi2Instantiate("%s",%d,"%s","%s","%s",%d,%d) = NULL (DoInit failure))",
                            instance_name,
                            fmu_type,
                            fmu_guid,
                            (fmu_resource_location != nullptr) ? fmu_resource_location : "<NULL>",
                            "FUNCTIONS",
                            visible,
                            logging_on);
        delete myc;
        return nullptr;
    }
    FmiVerboseLogGlobal(R"(fmi2Instantiate("%s",%d,"%s","%s","%s",%d,%d) = %p)",
                        instance_name,
                        fmu_type,
                        fmu_guid,
                        (fmu_resource_location != nullptr) ? fmu_resource_location : "<NULL>",
                        "FUNCTIONS",
                        visible,
                        logging_on,
                        myc);
    return (fmi2Component)myc;
}

fmi2Status HelloWorldSensor::SetupExperiment(fmi2Boolean tolerance_defined, fmi2Real tolerance, fmi2Real start_time, fmi2Boolean stop_time_defined, fmi2Real stop_time)
{
    FmiVerboseLog("fmi2SetupExperiment(%d,%g,%g,%d,%g)", tolerance_defined, tolerance, start_time, stop_time_defined, stop_time);
    return DoStart(tolerance_defined, tolerance, start_time, stop_time_defined, stop_time);
}

fmi2Status HelloWorldSensor::EnterInitializationMode()
{
    FmiVerboseLog("fmi2EnterInitializationMode()");
    return DoEnterInitializationMode();
}

fmi2Status HelloWorldSensor::ExitInitializationMode()
{
    FmiVerboseLog("fmi2ExitInitializationMode()");
    simulation_started_ = true;
    return DoExitInitializationMode();
}

fmi2Status HelloWorldSensor::DoStep(fmi2Real current_communication_point, fmi2Real communication_step_size, fmi2Boolean no_set_fmu_state_prior_to_current_pointfmi_2_component)
{
    FmiVerboseLog("fmi2DoStep(%g,%g,%d)", current_communication_point, communication_step_size, no_set_fmu_state_prior_to_current_pointfmi_2_component);
    return DoCalc(current_communication_point, communication_step_size, no_set_fmu_state_prior_to_current_pointfmi_2_component);
}

fmi2Status HelloWorldSensor::Terminate()
{
    FmiVerboseLog("fmi2Terminate()");
    return DoTerm();
}

fmi2Status HelloWorldSensor::Reset()
{
    FmiVerboseLog("fmi2Reset()");

    DoFree();
    simulation_started_ = false;
    return DoInit();
}

void HelloWorldSensor::FreeInstance()
{
    FmiVerboseLog("fmi2FreeInstance()");
    DoFree();
}

fmi2Status HelloWorldSensor::GetReal(const fmi2ValueReference vr[], size_t nvr, fmi2Real value[])
{
    FmiVerboseLog("fmi2GetReal(...)");
    for (size_t i = 0; i < nvr; i++)
    {
        if (vr[i] < FMI_REAL_VARS)
        {
            value[i] = real_vars_[vr[i]];
        }
        else
        {
            return fmi2Error;
        }
    }
    return fmi2OK;
}

fmi2Status HelloWorldSensor::GetInteger(const fmi2ValueReference vr[], size_t nvr, fmi2Integer value[])
{
    FmiVerboseLog("fmi2GetInteger(...)");
    bool need_refresh = !simulation_started_;
    for (size_t i = 0; i < nvr; i++)
    {
        if (vr[i] < FMI_INTEGER_VARS)
        {
            if (need_refresh && (vr[i] == FMI_INTEGER_SENSORVIEW_CONFIG_REQUEST_BASEHI_IDX || vr[i] == FMI_INTEGER_SENSORVIEW_CONFIG_REQUEST_BASELO_IDX ||
                                 vr[i] == FMI_INTEGER_SENSORVIEW_CONFIG_REQUEST_SIZE_IDX))
            {
                RefreshFmiSensorViewConfigRequest();
                need_refresh = false;
            }
            value[i] = integer_vars_[vr[i]];
        }
        else
        {
            return fmi2Error;
        }
    }
    return fmi2OK;
}

fmi2Status HelloWorldSensor::GetBoolean(const fmi2ValueReference vr[], size_t nvr, fmi2Boolean value[])
{
    FmiVerboseLog("fmi2GetBoolean(...)");
    for (size_t i = 0; i < nvr; i++)
    {
        if (vr[i] < FMI_BOOLEAN_VARS)
        {
            value[i] = boolean_vars_[vr[i]];
        }
        else
        {
            return fmi2Error;
        }
    }
    return fmi2OK;
}

fmi2Status HelloWorldSensor::GetString(const fmi2ValueReference vr[], size_t nvr, fmi2String value[])
{
    FmiVerboseLog("fmi2GetString(...)");
    for (size_t i = 0; i < nvr; i++)
    {
        if (vr[i] < FMI_STRING_VARS)
        {
            value[i] = string_vars_[vr[i]].c_str();
        }
        else
        {
            return fmi2Error;
        }
    }
    return fmi2OK;
}

fmi2Status HelloWorldSensor::SetReal(const fmi2ValueReference vr[], size_t nvr, const fmi2Real value[])
{
    FmiVerboseLog("fmi2SetReal(...)");
    for (size_t i = 0; i < nvr; i++)
    {
        if (vr[i] < FMI_REAL_VARS)
        {
            real_vars_[vr[i]] = value[i];
        }
        else
        {
            return fmi2Error;
        }
    }
    return fmi2OK;
}

fmi2Status HelloWorldSensor::SetInteger(const fmi2ValueReference vr[], size_t nvr, const fmi2Integer value[])
{
    FmiVerboseLog("fmi2SetInteger(...)");
    for (size_t i = 0; i < nvr; i++)
    {
        if (vr[i] < FMI_INTEGER_VARS)
        {
            integer_vars_[vr[i]] = value[i];
        }
        else
        {
            return fmi2Error;
        }
    }
    return fmi2OK;
}

fmi2Status HelloWorldSensor::SetBoolean(const fmi2ValueReference vr[], size_t nvr, const fmi2Boolean value[])
{
    FmiVerboseLog("fmi2SetBoolean(...)");
    for (size_t i = 0; i < nvr; i++)
    {
        if (vr[i] < FMI_BOOLEAN_VARS)
        {
            boolean_vars_[vr[i]] = value[i];
        }
        else
        {
            return fmi2Error;
        }
    }
    return fmi2OK;
}

fmi2Status HelloWorldSensor::SetString(const fmi2ValueReference vr[], size_t nvr, const fmi2String value[])
{
    FmiVerboseLog("fmi2SetString(...)");
    for (size_t i = 0; i < nvr; i++)
    {
        if (vr[i] < FMI_STRING_VARS)
        {
            string_vars_[vr[i]] = value[i];
        }
        else
        {
            return fmi2Error;
        }
    }
    return fmi2OK;
}

/*
 * FMI 2.0 Co-Simulation Interface API
 */

extern "C" {

FMI2_Export const char* fmi2GetTypesPlatform()
{
    return fmi2TypesPlatform;
}

FMI2_Export const char* fmi2GetVersion()
{
    return fmi2Version;
}

FMI2_Export fmi2Status fmi2SetDebugLogging(fmi2Component c, fmi2Boolean logging_on, size_t n_categories, const fmi2String categories[])
{
    auto* myc = (HelloWorldSensor*)c;
    return myc->SetDebugLogging(logging_on, n_categories, categories);
}

/*
 * Functions for Co-Simulation
 */
FMI2_Export fmi2Component fmi2Instantiate(fmi2String instance_name,
                                          fmi2Type fmu_type,
                                          fmi2String fmu_guid,
                                          fmi2String fmu_resource_location,
                                          const fmi2CallbackFunctions* functions,
                                          fmi2Boolean visible,
                                          fmi2Boolean logging_on)
{
    return HelloWorldSensor::Instantiate(instance_name, fmu_type, fmu_guid, fmu_resource_location, functions, visible, logging_on);
}

FMI2_Export fmi2Status
fmi2SetupExperiment(fmi2Component c, fmi2Boolean tolerance_defined, fmi2Real tolerance, fmi2Real start_time, fmi2Boolean stop_time_defined, fmi2Real stop_time)
{
    auto* myc = (HelloWorldSensor*)c;
    return myc->SetupExperiment(tolerance_defined, tolerance, start_time, stop_time_defined, stop_time);
}

FMI2_Export fmi2Status fmi2EnterInitializationMode(fmi2Component c)
{
    auto* myc = (HelloWorldSensor*)c;
    return myc->EnterInitializationMode();
}

FMI2_Export fmi2Status fmi2ExitInitializationMode(fmi2Component c)
{
    auto* myc = (HelloWorldSensor*)c;
    return myc->ExitInitializationMode();
}

FMI2_Export fmi2Status fmi2DoStep(fmi2Component c,
                                  fmi2Real current_communication_point,
                                  fmi2Real communication_step_size,
                                  fmi2Boolean no_set_fmu_state_prior_to_current_pointfmi2_component)
{
    auto* myc = (HelloWorldSensor*)c;
    return myc->DoStep(current_communication_point, communication_step_size, no_set_fmu_state_prior_to_current_pointfmi2_component);
}

FMI2_Export fmi2Status fmi2Terminate(fmi2Component c)
{
    auto* myc = (HelloWorldSensor*)c;
    return myc->Terminate();
}

FMI2_Export fmi2Status fmi2Reset(fmi2Component c)
{
    auto* myc = (HelloWorldSensor*)c;
    return myc->Reset();
}

FMI2_Export void fmi2FreeInstance(fmi2Component c)
{
    auto* myc = (HelloWorldSensor*)c;
    myc->FreeInstance();
    delete myc;
}

/*
 * Data Exchange Functions
 */
FMI2_Export fmi2Status fmi2GetReal(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2Real value[])
{
    auto* myc = (HelloWorldSensor*)c;
    return myc->GetReal(vr, nvr, value);
}

FMI2_Export fmi2Status fmi2GetInteger(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2Integer value[])
{
    auto* myc = (HelloWorldSensor*)c;
    return myc->GetInteger(vr, nvr, value);
}

FMI2_Export fmi2Status fmi2GetBoolean(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2Boolean value[])
{
    auto* myc = (HelloWorldSensor*)c;
    return myc->GetBoolean(vr, nvr, value);
}

FMI2_Export fmi2Status fmi2GetString(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2String value[])
{
    auto* myc = (HelloWorldSensor*)c;
    return myc->GetString(vr, nvr, value);
}

FMI2_Export fmi2Status fmi2SetReal(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2Real value[])
{
    auto* myc = (HelloWorldSensor*)c;
    return myc->SetReal(vr, nvr, value);
}

FMI2_Export fmi2Status fmi2SetInteger(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2Integer value[])
{
    auto* myc = (HelloWorldSensor*)c;
    return myc->SetInteger(vr, nvr, value);
}

FMI2_Export fmi2Status fmi2SetBoolean(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2Boolean value[])
{
    auto* myc = (HelloWorldSensor*)c;
    return myc->SetBoolean(vr, nvr, value);
}

FMI2_Export fmi2Status fmi2SetString(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2String value[])
{
    auto* myc = (HelloWorldSensor*)c;
    return myc->SetString(vr, nvr, value);
}

/*
 * Unsupported Features (FMUState, Derivatives, Async DoStep, Status Enquiries)
 */
FMI2_Export fmi2Status fmi2GetFMUstate(fmi2Component c, fmi2FMUstate* fmu_state)
{
    return fmi2Error;
}

FMI2_Export fmi2Status fmi2SetFMUstate(fmi2Component c, fmi2FMUstate fmu_state)
{
    return fmi2Error;
}

FMI2_Export fmi2Status fmi2FreeFMUstate(fmi2Component c, fmi2FMUstate* fmu_state)
{
    return fmi2Error;
}

FMI2_Export fmi2Status fmi2SerializedFMUstateSize(fmi2Component c, fmi2FMUstate fmu_state, size_t* size)
{
    return fmi2Error;
}

FMI2_Export fmi2Status fmi2SerializeFMUstate(fmi2Component c, fmi2FMUstate fmu_state, fmi2Byte serialized_state[], size_t size)
{
    return fmi2Error;
}

FMI2_Export fmi2Status fmi2DeSerializeFMUstate(fmi2Component c, const fmi2Byte serialized_state[], size_t size, fmi2FMUstate* fmu_state)
{
    return fmi2Error;
}

FMI2_Export fmi2Status fmi2GetDirectionalDerivative(fmi2Component c,
                                                    const fmi2ValueReference v_unknown_ref[],
                                                    size_t n_unknown,
                                                    const fmi2ValueReference v_known_ref[],
                                                    size_t n_known,
                                                    const fmi2Real dv_known[],
                                                    fmi2Real dv_unknown[])
{
    return fmi2Error;
}

FMI2_Export fmi2Status fmi2SetRealInputDerivatives(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2Integer order[], const fmi2Real value[])
{
    return fmi2Error;
}

FMI2_Export fmi2Status fmi2GetRealOutputDerivatives(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2Integer order[], fmi2Real value[])
{
    return fmi2Error;
}

FMI2_Export fmi2Status fmi2CancelStep(fmi2Component c)
{
    return fmi2OK;
}

FMI2_Export fmi2Status fmi2GetStatus(fmi2Component c, const fmi2StatusKind s, fmi2Status* value)
{
    return fmi2Discard;
}

FMI2_Export fmi2Status fmi2GetRealStatus(fmi2Component c, const fmi2StatusKind s, fmi2Real* value)
{
    return fmi2Discard;
}

FMI2_Export fmi2Status fmi2GetIntegerStatus(fmi2Component c, const fmi2StatusKind s, fmi2Integer* value)
{
    return fmi2Discard;
}

FMI2_Export fmi2Status fmi2GetBooleanStatus(fmi2Component c, const fmi2StatusKind s, fmi2Boolean* value)
{
    return fmi2Discard;
}

FMI2_Export fmi2Status fmi2GetStringStatus(fmi2Component c, const fmi2StatusKind s, fmi2String* value)
{
    return fmi2Discard;
}
}
