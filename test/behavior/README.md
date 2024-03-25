# *behavior* Folder

As described in the overall [OSMP Test Architecture](https://openmsl.github.io/doc/OpenMSL/test-architecture/index.html), behavior tests are similar to integration tests, but additionally check the output of the model under test.
Every test folder contains a System Structure Definition file (SSD) describing the test case, a README according to the OSMP [Test Architecture Readme Template](https://github.com/openMSL/.github/blob/main/doc/integration_test_readme_template.md) and all auxiliary data needed for the test.
All tests in this folder are automatically simulated in the GitHub Action pipeline for every commit.
This folder contains some examples for different test cases, as listed below.

| File                   | Description                                                                                                             |
|------------------------|-------------------------------------------------------------------------------------------------------------------------|
| 001_detected_object    | This behavior test checks, if the sensor model detects an object inside the sensor's specified field of view.           |
| 002_object_outside_fov | This behavior test checks, if the sensor model does not detects an object outside the sensor's specified field of view. |
