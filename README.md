# SL-1-0 Sensor Model Repository Template

[![Credibility Assessment](../../actions/workflows/credibility_assessment.yml/badge.svg)](https://github.com/openMSL/sl-1-0-sensor-model-repository-template/actions/workflows/credibility_assessment.yml)

Enter a short description of the model.
What is the purpose of the model?
What is the general modeling approach?
What inputs does the model need and what outputs does it generate?
<br><br>

< Eye-catcher Image >
<img src="doc/img/model_video.gif" width="800" />

## Modeling Approach

Put details about the inner workings of the model here.
Describe the modeling approach in detail.
What is the structure of the model?
What modules is it comprised of?
What sensor effects are represented in the model and how are they modeled?

## Parameterization

What parameters are used internally?
Which parameters can be set by the user?
Each parameter should have a short description as shown in the following example.

| Parameter                      | Description                                                         |
| ------------------------------ | ------------------------------------------------------------------- |
| `no_of_beams_vertical`         | Number of beams in vertical direction, e.g. layers, of the lidar    |
| `no_of_beams_horizontal`       | Number of beams per layer of the lidar                              |

## Interface

What interfaces are used as model input and model output?
All required field of the interface shall be named in a list as shown in the following example.

### Input: Required Fields in OSI3::SensorView

* `sensor_view.mounting_position`
* `sensor_view.global_ground_truth.timestamp`
* `sensor_view.global_ground_truth.host_vehicle_id`
* `sensor_view.global_ground_truth.stationary_object.id`
* `sensor_view.global_ground_truth.stationary_object.base.position`
* `sensor_view.global_ground_truth.stationary_object.base.orientation`
* `sensor_view.global_ground_truth.stationary_object.base.dimension`

### Output: Fields in OSI3::SensorData Filled by the Sensor Model

* `sensor_data.timestamp`
* `sensor_data.moving_object.header.ground_truth_id`
* `sensor_data.moving_object.header.tracking_id`
* `sensor_data.moving_object.header.existence_probability`
* `sensor_data.moving_object.header.measurement_state`
* `sensor_data.moving_object.header.sensor_id`
* `sensor_data.moving_object.base.position`
* `sensor_data.moving_object.base.dimension`

## Build Instructions

What are the dependencies for building the model?

Give step-by-step build instructions for supported operating systems.
The following is an example for building a model as an FMU in Ubuntu.

### Build Model in Ubuntu 18.04 / 20.04

1. Clone this repository **with submodules**:

    ```bash
    git clone https://github.com/openMSL/your-model.git --recurse-submodules
    ```

2. Build the model by executing in the extracted project root directory:

    ```bash
    mkdir cmake-build
    cd cmake-build
    # If FMU_INSTALL_DIR is not set, CMAKE_BINARY_DIR is used
    cmake -DCMAKE_BUILD_TYPE=Release -DFMU_INSTALL_DIR:PATH=/tmp ..
    make
    ```

3. Take FMU from `FMU_INSTALL_DIR`

< The final FMU has to be named according to the repository name. In this example sl-1-0-sensor-model-repository-template.fmu >

## Credits

Give credits to funding or third-party contributions.
If the model is further described in a publication, it can also be named here.

## References

Throughout this readme file, references are to be used, e.g. when citing scientific literature while describing the modeling approach.
This can be done by added a number in the text with a reference identifier, like so: [[1](#Rosenberger2020)</sup>, p. 192].
For longer papers or book sections, please also give the page number, as shown in this example.

Then add the full list of authors, title and journal or conference in this section.
The IEEE citation style [[2](#IEEEStyle)</sup>] should be used.
Here is the bibliography from the example above.

<a name="Rosenberger2020">[1]</a>
P. Rosenberger, M. F. Holder, N. Cianciaruso, P. Aust, J. F. Tamm-Morschel, C. Linnhoff, and H. Winner,
“Sequential lidar sensor system simulation: A modular approach for simulation-based safety validation of automated driving,”
Automotive and Engine Technology, vol. 5, no. 3-4, pp. 187–197, Dec. 2020.

<a name="IEEEStyle">[2]</a>
IEEEDataPort,
"How to Cite References: IEEE Documentation Style,"
[Online]. Available:
[https://ieee-dataport.org/sites/default/files/analysis/27/IEEE%20Citation%20Guidelines.pdf](https://ieee-dataport.org/sites/default/files/analysis/27/IEEE%20Citation%20Guidelines.pdf).
[Accessed Jan. 09, 2023]

## State-of-the-Art

This section is a state-of-the-art collection for the sensor model sub-library.
When you clone this template to implement your own model, delete this entire section from the readme.

### Sensor Model Validation and Verification

#### General Simulation Credibility

* [Credible Simulation Process Framework](https://gitlab.setlevel.de/open/processes_and_traceability/credible_simulation_process_framework) from the German research project [SET Level](https://setlevel.de/) of the [PEGASUS](https://pegasus-family.de/)  project family
* [Credibility-Assessment-Framework](https://github.com/virtual-vehicle/Credibility-Assessment-Framework) incl. [Credibility Development Kit](https://github.com/virtual-vehicle/Credibility-Assessment-Framework/tree/main/Credibility-Development-Kit)
from the European research project [UPSIM](https://upsim-project.eu/) incl. corresponding [publication](https://ecp.ep.liu.se/index.php/modelica/article/view/572/542)

#### Sensor Model Test Frameworks

* [PMSF FMI Bench](https://pmsf.eu/products/fmibench/)
* [OSI Validator](https://github.com/OpenSimulationInterface/osi-validation)
* [Eclipse OpenMCx](https://projects.eclipse.org/proposals/eclipse-openmcx) incl. [GitHub repo](https://github.com/eclipse/openmcx) for connecting FMUs (a.k.a. co-simulation)

#### Sensor Model Verification

* SET Level Model Verification
  * Object Based Generic Perception Model
  * Reflection Based Radar Object Model

#### Sensor Model Validation

##### Dedicated Publications

| Authors            | Date       | Title                            | Link / Repo / Paper / DOI                            | Data Set | Modality  | Facility  | Funding |
| ------------------ | ---------- | -------------------------------- | ---------------------------------------------------- | -------- | --------- | --------- | ------- |
| Huch et al.        |       2023 | Quantifying the LiDAR Sim-to-Real Domain Shift: A Detailed Investigation Using Object Detectors and Analyzing Point Clouds at Target-Level | [10.1109/TIV.2023.3251650](doi.org/10.1109/TIV.2023.3251650) | [Sim2Real-DistributionAligned-Dataset](https://github.com/TUMFTM/Sim2RealDistributionAlignedDataset) | Lidar | Technical University of Munich  | / |
| Haider et al.      |       2023 | Performance Evaluation of MEMS-Based Automotive LiDAR Sensor and Its Simulation Model as per ASTM E3125-17 Standard | [10.3390/s23063113](https://doi.org/10.3390/s23063113) | / |Radar | Kempten University of Applied Sciences | VIVID |
| Ngo                |       2023 | A methodology for validation of a radar simulation for virtual testing of autonomous driving | [10.18419/opus-12703](http://dx.doi.org/10.18419/opus-12703) | / | Radar | University of Stuttgart  | Robert Bosch GmbH |
| Rosenberger        |       2023 | Metrics for Specification, Validation, and Uncertainty Prediction for Credibility in Simulation of Active Perception Sensor Systems | [10.26083/tuprints-00023034](https://doi.org/10.26083/tuprints-00023034) | / | Generic (Lidar) | TU Darmstadt   | ENABLE-S3, PEGASUS, SET Level, VVMethods |
| Ngo et al.         |       2021 | A Multi-Layered Approach for Measuring the Simulation-to-Reality Gap of Radar Perception for Autonomous Driving | [10.1109/ITSC48978.2021.9564521](https://doi.org/10.1109/ITSC48978.2021.9564521) | / | Radar | University of Stuttgart  | Robert Bosch GmbH |
| Schaermann         |       2020 | Systematische Bedatung und Bewertung umfelderfassender Sensormodelle | [mediaTUM](https://mediatum.ub.tum.de/doc/1518611/1518611.pdf) | / | Generic (Lidar) | TU München  | BMW AG |
| Rosenberger et al. |       2019 | Towards a Generally Accepted Validation Methodology for Sensor Models - Challenges, Metrics, and First Results | [TUprints](http://tubiblio.ulb.tu-darmstadt.de/113867/) | / | Generic    | TU Darmstadt   | PEGASUS    |

##### Measurement Campaigns for Sensor Model Validation

| Authors            | Date       | Title                            | Chapter(s) | Link / Repo / Paper / DOI                            | Modality?  | Facility  | Funding |
| ------------------ | ---------- | -------------------------------- | ---------------------------------------------------- | ---------- | ---------- | --------- | ------- |
| Linnhoff           |       2023 | Analysis of Environmental Influences for Simulation of Active Perception Sensors | Chapter 5 | [10.26083/tuprints-00023116](https://doi.org/10.26083/tuprints-00023116) | Lidar | TU Darmstadt   | SET Level, VVMethods |
| Holder             |       2021 | Synthetic Generation of Radar Sensor Data for Virtual Validation of Autonomous Driving | Chapters 4.2, 5.2, 6.2-6.4 | [10.26083/tuprints-00017545](https://doi.org/10.26083/tuprints-00017545) | Radar | TU Darmstadt   | SET Level, VVMethods |

##### Presentations on Simulation Credibility and Sensor Model Validation

* [Weber, Rosenberger: Simulation Credibility Layers – Toward a Holistic Assessment, SET Level Final Event (October 2022)](https://setlevel.de/assets/s4v2-simulation-credibility-layers-.pdf)
* [Schunk, Rosenberger: Validation of Test Infrastructure - From Cause Trees to a Validated System Simulation, VVM Half Time Event (March 2022)](https://www.vvm-projekt.de/fileadmin/user_upload/Mid-Term/Presentations/VVM_HZE_S2_P6_20220315_ValidationTestInfrastructure.pdf)

### Collection of Sensor Model Candidates for OpenMSL

#### Lidar Models

| Authors       | Date       | Title                              | Link / Repo / Paper / DOI                            | Standards? | Facility  | Funding |
| ------------- | ---------- | ---------------------------------- | ---------------------------------------------------- | ---------- | --------- | ------- |
| Haider et al. |    10.2022 | Development of High-Fidelity Automotive LiDAR Sensor Model with Standardized Interfaces      | [10.3390/s22197556](http://dx.doi.org/10.3390/s22197556)                   | FMI/OSI    | HS Kempten   | VIVID    |
|Guillard et al.| 22.09.2022 | Learning to Simulate Realistic LiDARs | [10.48550/arXiv.2209.10986](https://doi.org/10.48550/arXiv.2209.10986) | / | Ecole polytechnique federale de Lausanne | Microsoft |
| Rott          | 05.04.2022 | Dynamic Update of Stand-Alone Lidar Model Based on Ray Tracing Using the Nvidia Optix Engine | [10.1109/ICCVE52871.2022.9743000](https://doi.org/10.1109/ICCVE52871.2022.9743000) | OSI, obj   | ViF Graz     | COMET K2 |
| Gusmão et al. | 18.11.2020 | Development and Validation of LiDAR Sensor Simulators Based on Parallel Raycasting | [10.3390/s20247186](https://doi.org/10.3390/s20247186) | (optScan in Unity)   | Pontifical Catholic University of Rio de Janeiro  | Coordenação de Aperfeiçoamento de Pessoal de Nível Superior—Brasil (CAPES)—Finance Code 001 |
| Wang et al. | 07.07.2019 | Automatic Generation of Synthetic LiDAR Point Clouds for 3-D Data Analysis | [10.1109/TIM.2019.2906416](https://doi.org/10.1109/TIM.2019.2906416) | (CARLA) | Dalian University of Technology, Dalian, China  | / |
| Yue et al.    | 06.2018 | A LiDAR Point Cloud Generator: from a Virtual World to Autonomous Driving | [10.1145/3206025.3206080](https://doi.org/10.1145/3206025.3206080) | (DeepGTAV with Script Hook V)   | University of California, Berkeley, Berkeley, CA, USA  | NSF, Award 1645964, together with Berkeley Deep Drive |
| Woods         |       2015 | GLIDAR: a simple OpenGL LIDAR simulator | [GitHub](https://github.com/WVU-ASEL/glidar) | OpenGL | West Virginia University | / |

#### Radar Models

| Authors           | Date       | Title                                                                                              | Link / Repo / Paper / DOI                                                    | Standards? | Facility  | Funding |
| ----------------- | ---------- | -------------------------------------------------------------------------------------------------- | ---------------------------------------------------------------------------- | ---------- | --------- | ------- |
| Lindenmaier et al.|    01.2023 | Object-Level Data-Driven Sensor Simulation for Automotive Environment Perception                   |  [10.1109/TIV.2023.3287278](https://doi.org/10.1109/TIV.2023.3287278)        | /          | Budapest University of Technology and Economics | EU and Ministry of Innovation and Technology of Hungary |
| Schwind et al.    | 02.11.2022 | Virtual Sensor Validation for  Automated and Connected Driving                                     |  [10.1007/s38311-022-1405-7](https://doi.org/10.1007/s38311-022-1405-7)      | /          | TU Ilmenau    | VIVID     |
| Reiter et al.     |    10.2022 | (SET Level Radar Model) || OSI  | FZI Karlsruhe | SET Level |
| Kesury            |    05.2022 | RADAR MODELING FOR AUTONOMOUS VEHICLE SIMULATION ENVIRONMENT USING OPEN SOURCE                     | [10.7912/C2/2924](http://dx.doi.org/10.7912/C2/2924)                         | /          | Purdue University Indianapolis, Indiana | / |
| Aust et al.       | 16.05.2022 | A Data-driven Approach for Stochastic Modeling of Automotive Radar Detections for Extended Objects | [IEEExplore](https://ieeexplore.ieee.org/document/9783497)                   | /          | Mercedes Benz | VIVID     |
| Prinz et al.      |       2021 | Automotive Radar Signal and Interference Simulation for Testing Autonomous Driving                 | [10.1007/978-3-030-71454-3_14](https://doi.org/10.1007/978-3-030-71454-3_14) | /          | BMW           | /         |

#### Frameworks for Modelling

| Authors         | Date       | Title                              | Link / Repo / Paper / DOI                            | Standards? | Facility  | Funding |
| --------------- | ---------- | ---------------------------------- | ---------------------------------------------------- | ---------- | --------- | ------- |
| Linnhoff et al. |    10.2022 |  Modular OSMP Framework            | [GitLab](https://gitlab.com/tuda-fzd/perception-sensor-modeling/modular-osmp-framework)                   | FMI/OSI    | TU Darmstadt   | SET Level    |

#### Other Models

* Camera?
* Ultrasonic?
* Thermal Imaging?
* ???

#### Sources for Already Contained Models

| Authors            | Date       | Title                            | Link / Repo / Paper / DOI                            | Modality?  | Standards? | Facility  | Funding |
| ------------------ | ---------- | -------------------------------- | ---------------------------------------------------- | ---------- | ---------- | --------- | ------- |
| Linnhoff           |       2023 | Analysis of Environmental Influences for Simulation of Active Perception Sensors | [10.26083/tuprints-00023116](https://doi.org/10.26083/tuprints-00023116) | Lidar | OSI, FMI | TU Darmstadt   | SET Level, VVMethods |
| Linnhoff et al.    |       2021 | Refining Object-Based Lidar Sensor Modeling — Challenging Ray Tracing as the Magic Bullet | [10.1109/JSEN.2021.3115589](https://doi.org/10.1109/JSEN.2021.3115589) | Lidar | OSI, FMI | TU Darmstadt   | SET Level, VVMethods |
| Holder             |       2021 | Synthetic Generation of Radar Sensor Data for Virtual Validation of Autonomous Driving | [10.26083/tuprints-00017545](https://doi.org/10.26083/tuprints-00017545) | Radar | OSI, FMI | TU Darmstadt   | SET Level, VVMethods |
