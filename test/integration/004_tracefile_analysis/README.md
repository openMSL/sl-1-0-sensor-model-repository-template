# 004 Tracefile Analysis

This test checks if an object inside the field of view of the sensor is detected by the model.

## Scenario

One object is placed in front of the sensor.
The x-coordinates of the objects in this scenario are:

- ego: 10 m
- ego bbcenter2rear: -1.146 m
- object 25 m

## Metric

The object is expected to be inside the field of view of the sensor at all times.
In the analyze.py script the average number of detected moving objects over all simulation time steps is calculated.

## Pass/Fail Criterion

The test fails, if it differs from the expectation value of 1.0.
