import sys
from lib.osi_trace import OSITrace

def main():
    if len(sys.argv) == 2:
        trace_file_name = str(sys.argv[1])
        print("Analyzing " + trace_file_name)
    else:
        print("Usage: python3 analyze <trace_file>")
        exit(1)

    # Load trace file
    trace_file = OSITrace(0)
    trace_file.from_file(trace_file_name, "SensorData")

    # Parameter to check
    avg_number_of_detected_objects = 0
    # Expectation value
    exp_number_of_detected_objects = 0

    # Iterate over all time steps
    for current_time_step in range(0, trace_file.timestep_count):
        current_message = trace_file.get_message_by_index(current_time_step)
        current_sensor_data = current_message.value.to_dict()

        # Calculate parameter to check
        if "movingObject" in current_sensor_data:
            avg_number_of_detected_objects = avg_number_of_detected_objects + len(current_sensor_data["movingObject"])

    # Calculate parameter to check
    avg_number_of_detected_objects = avg_number_of_detected_objects / trace_file.timestep_count

    # Check parameter against expectation value
    if avg_number_of_detected_objects != exp_number_of_detected_objects:
        print("Error: Average number of detected moving objects is " + str(avg_number_of_detected_objects) +
              ", expected value is " + str(exp_number_of_detected_objects))

    trace_file.trace_file.close()


if __name__ == "__main__":
    main()
