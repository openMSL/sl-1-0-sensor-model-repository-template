set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(LINK_WITH_SHARED_OSI OFF CACHE BOOL "Link FMU with shared OSI library instead of statically linking")
set(PUBLIC_LOGGING OFF CACHE BOOL "Enable logging via FMI logger")
set(PRIVATE_LOGGING OFF CACHE BOOL "Enable private logging to file")

string(TIMESTAMP FMUTIMESTAMP UTC)
string(MD5 FMUGUID modelDescription.in.xml)
configure_file(modelDescription.in.xml modelDescription.xml @ONLY)

find_package(Protobuf 2.6.1 REQUIRED)
add_library(sl-1-0-sensor-model-repository-template SHARED
		OSMP.cpp
		OSMP.h
		MySensorModel.cpp
		MySensorModel.h)
set_target_properties(sl-1-0-sensor-model-repository-template PROPERTIES PREFIX "")
target_compile_definitions(sl-1-0-sensor-model-repository-template PRIVATE "FMU_SHARED_OBJECT")
if(LINK_WITH_SHARED_OSI)
	target_link_libraries(sl-1-0-sensor-model-repository-template open_simulation_interface)
else()
	target_link_libraries(sl-1-0-sensor-model-repository-template open_simulation_interface_pic)
endif()

if(WIN32)
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(FMI_BINARIES_PLATFORM "win64")
	else()
		set(FMI_BINARIES_PLATFORM "win32")
	endif()
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(FMI_BINARIES_PLATFORM "linux64")
	else()
		set(FMI_BINARIES_PLATFORM "linux32")
	endif()
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(FMI_BINARIES_PLATFORM "darwin64")
	else()
		set(FMI_BINARIES_PLATFORM "darwin32")
	endif()
endif()

add_custom_command(TARGET sl-1-0-sensor-model-repository-template
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_CURRENT_BINARY_DIR}/buildfmu"
		COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/buildfmu/sources"
		COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/buildfmu/binaries/${FMI_BINARIES_PLATFORM}"
		COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/buildfmu/extra/net.pmsf.ssp.srmd"
		COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/modelDescription.xml" "${CMAKE_CURRENT_BINARY_DIR}/buildfmu"
		COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/OSMP.cpp" "${CMAKE_CURRENT_BINARY_DIR}/buildfmu/sources/"
		COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/OSMP.h" "${CMAKE_CURRENT_BINARY_DIR}/buildfmu/sources/"
		COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/MySensorModel.cpp" "${CMAKE_CURRENT_BINARY_DIR}/buildfmu/sources/"
		COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/MySensorModel.h" "${CMAKE_CURRENT_BINARY_DIR}/buildfmu/sources/"
		COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/../sl-1-0-sensor-model-repository-template.srmd" "${CMAKE_CURRENT_BINARY_DIR}/buildfmu/extra/net.pmsf.ssp.srmd/resourceMetaData.srmd"
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:sl-1-0-sensor-model-repository-template> $<$<PLATFORM_ID:Windows>:$<$<CONFIG:Debug>:$<TARGET_PDB_FILE:sl-1-0-sensor-model-repository-template>>> "${CMAKE_CURRENT_BINARY_DIR}/buildfmu/binaries/${FMI_BINARIES_PLATFORM}"
		COMMAND ${CMAKE_COMMAND} -E chdir "${CMAKE_CURRENT_BINARY_DIR}/buildfmu" ${CMAKE_COMMAND} -E tar "cfv" "${FMU_INSTALL_DIR}/sl-1-0-sensor-model-repository-template.fmu" --format=zip "modelDescription.xml" "${CMAKE_CURRENT_BINARY_DIR}/buildfmu/sources" "${CMAKE_CURRENT_BINARY_DIR}/buildfmu/binaries/${FMI_BINARIES_PLATFORM}")