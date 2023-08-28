# <id> Test Name

_Short description of the test. E.g.:_

This test checks if an object inside the field of view of the sensor is detected by the model.

## System Structure Definition

_Short description of the individual components of the test run. Also include a diagram of the system structure. E.g.:_

The system consists of an OSMP binary trace file player, the model itself and a trace file writer.
The trace file player will read the given SensorView trace file as an input for the sensor model.
The output of the sensor model is written into a binary SensorData trace file for the subsequent analysis.

<img alt="System Structure" src="004_tracefile_analysis/system_structure.png" width="600">

## Scenario

_Verbal description of the test scenario containing physical parameter values for distances, velocities, timing etc. E.g.:_

The scenario contained in the fiven trace file consists of a vehicle placed on the x-axis (y = 0) in front of the ego vehicle in the sensor's field of view.
The x-coordinates of the objects in this scenario are:

- ego: 10 m
- ego bbcenter2rear: -1.146 m
- object: 25 m

## Metric

_Description of the metric that is calculated to determine if the test passes. E.g.:_

The object is expected to be inside the field of view of the sensor at all times.
In the analyze.py script the average number of detected moving objects over all simulation time steps is calculated.

## Pass/Fail Criterion

_Description of the pass/fail criterion based on the evaluation of the previously defined metric. E.g.:_

The test fails, if it differs from the expectation value of 1.0.
