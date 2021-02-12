# VecMem project, part of the ACTS project (R&D line)
#
# (c) 2021 CERN for the benefit of the ACTS project
#
# Mozilla Public License Version 2.0

# Make sure that the code is not included more than once.
include_guard( GLOBAL )

# CMake include(s).
include( CheckLanguage )

# Teach CMake about VecMem's custom language files.
list( APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/hip" )

# Code mimicking CMake's CheckLanguage.cmake module. But making sure that the
# VecMem specific code is used while looking for the non-standard languages.
macro( vecmem_check_language lang )

   # If CMAKE_${lang}_COMPILER is already set (even to an invalid value),
   # don't look for it again. Also ignore anything but the HIP "language".
   if( NOT DEFINED CMAKE_${lang}_COMPILER )

      # Handle the HIP case.
      if( "${lang}" STREQUAL "HIP" )

         # Greet the user.
         set( _desc "Looking for a ${lang} compiler" )
         message( CHECK_START "${_desc}" )

         # Clean up.
         file( REMOVE_RECURSE
            "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/Check${lang}" )

         # Set up a test project, which will be used to check for the viability
         # of HIP.
         file( WRITE
            "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/Check${lang}/CMakeLists.txt"
            "cmake_minimum_required( VERSION ${CMAKE_VERSION} )\n"
            "project( Check${lang} LANGUAGES CXX )\n"
            "list( INSERT CMAKE_MODULE_PATH 0 "
            "      \"${CMAKE_CURRENT_LIST_DIR}/hip\" )\n"
            "enable_language( HIP )\n"
            "file( WRITE \"\${CMAKE_CURRENT_BINARY_DIR}/result.cmake\"\n"
            "   \"set( CMAKE_${lang}_COMPILER \\\"\${CMAKE_${lang}_COMPILER}\\\" )\" )" )
         execute_process(
            WORKING_DIRECTORY
               "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/Check${lang}"
            COMMAND ${CMAKE_COMMAND} -G ${CMAKE_GENERATOR} .
            OUTPUT_VARIABLE _vcl_output
            ERROR_VARIABLE _vcl_output
            RESULT_VARIABLE _vcl_result )

         # Check if the call succeeded.
         include(
            "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/Check${lang}/result.cmake"
            OPTIONAL )
         if( CMAKE_${lang}_COMPILER AND "${_vcl_result}" STREQUAL "0" )
            set( _CHECK_COMPILER_STATUS CHECK_PASS )
         else()
            set( _CHECK_COMPILER_STATUS CHECK_FAIL )
            set( CMAKE_${lang}_COMPILER NOTFOUND )
         endif()

         # Let the user know what happened.
         message( ${_CHECK_COMPILER_STATUS} "${CMAKE_${lang}_COMPILER}" )
         set( CMAKE_${lang}_COMPILER "${CMAKE_${lang}_COMPILER}" CACHE FILEPATH
            "${lang} compiler" )
         mark_as_advanced( CMAKE_${lang}_COMPILER )

      else()

         # Fall back on CMake's code.
         check_language( ${lang} )

      endif()
   endif()

endmacro( vecmem_check_language )