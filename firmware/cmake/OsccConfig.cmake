#To be deprecated in favor of using VEHICLE and VEHICLE VALUES so that
#adding new vehicles can be maintained in a single list and the list of
#available values can be printed in help and output.
set(KIA_SOUL OFF CACHE BOOL "Build firmware for the petrol Kia Soul")
set(KIA_SOUL_EV OFF CACHE BOOL "Build firmware for the Kia Soul EV")
set(KIA_NIRO OFF CACHE BOOL "Build firmware for the Kia Niro")

set(VEHICLE "" CACHE STRING
  "VEHICLE chosen by the user at CMake configure time")

set(VEHICLE_VALUES "kia_soul;kia_soul_ev;kia_niro" CACHE STRING
  "List of possible values for the VEHICLE cache variable")

set_property(CACHE VEHICLE PROPERTY STRINGS ${VEHICLE_VALUES})

message(STATUS "VEHICLE='${VEHICLE}'")

if (";${VEHICLE_VALUES};" MATCHES ";${VEHICLE};")
  string(TOUPPER ${VEHICLE} VEHICLE_UPPER_CASE)
  add_definitions(-D${VEHICLE_UPPER_CASE})
elseif(KIA_SOUL)
  add_definitions(-DKIA_SOUL)
  set(VEHICLE kia_soul)
  message(WARNING "-DKIA_SOUL=ON is being deprecated for VEHICLE string assign '-DVEHICLE=kia_soul'")
elseif(KIA_SOUL_EV)
  add_definitions(-DKIA_SOUL_EV)
  set(VEHICLE kia_soul_ev)
  message(WARNING "-DKIA_SOUL_EV=ON is being deprecated for VEHICLE string assign '-DVEHICLE=kia_soul_ev'")
elseif(KIA_NIRO)
  add_definitions(-DKIA_NIRO)
  set(VEHICLE kia_niro)
  message(WARNING "-DKIA_NIRO=ON is being deprecated for VEHICLE string assign '-DVEHICLE=kia_niro'")
else()
  message(FATAL_ERROR "No platform selected, available platforms are: ${VEHICLE_VALUES}")
endif()
