cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

set(CMAKE_SYSTEM_VERSION 10.0 CACHE STRING "" FORCE)

project(FRCScout C CXX)

################################################################################
# Set target arch type if empty. Visual studio solution generator provides it.
################################################################################
if(NOT CMAKE_VS_PLATFORM_NAME)
    set(CMAKE_VS_PLATFORM_NAME "x64")
endif()
message("${CMAKE_VS_PLATFORM_NAME} architecture in use")

if(NOT ("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64"
     OR "${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86"))
    message(FATAL_ERROR "${CMAKE_VS_PLATFORM_NAME} arch is not supported!")
endif()

################################################################################
# Global configuration types
################################################################################
set(CMAKE_CONFIGURATION_TYPES
    "Debug"
    "OldCLI"
    "ReleaseUI"
    CACHE STRING "" FORCE
)

################################################################################
# Global compiler options
################################################################################
if(MSVC)
    # remove default flags provided with CMake for MSVC
    set(CMAKE_C_FLAGS "")
    set(CMAKE_C_FLAGS_DEBUG "")
    set(CMAKE_C_FLAGS_OLDCLI "")
    set(CMAKE_C_FLAGS_RELEASEUI "")
    set(CMAKE_CXX_FLAGS "")
    set(CMAKE_CXX_FLAGS_DEBUG "")
    set(CMAKE_CXX_FLAGS_OLDCLI "")
    set(CMAKE_CXX_FLAGS_RELEASEUI "")
endif()

################################################################################
# Global linker options
################################################################################
if(MSVC)
    # remove default flags provided with CMake for MSVC
    set(CMAKE_EXE_LINKER_FLAGS "")
    set(CMAKE_MODULE_LINKER_FLAGS "")
    set(CMAKE_SHARED_LINKER_FLAGS "")
    set(CMAKE_STATIC_LINKER_FLAGS "")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_DEBUG "${CMAKE_STATIC_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS_OLDCLI "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_OLDCLI "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_OLDCLI "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_OLDCLI "${CMAKE_STATIC_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASEUI "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_RELEASEUI "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASEUI "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_RELEASEUI "${CMAKE_STATIC_LINKER_FLAGS}")
endif()

################################################################################
# Nuget packages function stub.
################################################################################
function(use_package TARGET PACKAGE VERSION)
    message(WARNING "No implementation of use_package. Create yours. "
                    "Package \"${PACKAGE}\" with version \"${VERSION}\" "
                    "for target \"${TARGET}\" is ignored!")
endfunction()

################################################################################
# Common utils
################################################################################
include(CMake/Utils.cmake)

################################################################################
# Additional Global Settings(add specific info there)
################################################################################
include(CMake/GlobalSettingsInclude.cmake OPTIONAL)

################################################################################
# Use solution folders feature
################################################################################
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

################################################################################
# Sub-projects
################################################################################



























set(PROJECT_NAME FRCScout)

################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "out/release/FRCScout.exe"
    "out/release/FRCScout.pdb"
)
source_group("" FILES ${no_group_source_files})

set(Headers
    "api/backend/data.h"
    "api/backend/match.h"
    "api/backend/rfpredict.h"
    "api/backend/team.h"
    "api/frontend/app.h"
    "api/frontend/colours.h"
    "api/frontend/mainframe.h"
    "api/frontend/wxids.h"
    "ext/json.hpp"
    "ext/json_fwd.hpp"
    "ext/qrcodegen.hpp"
    "ext/sqlite3.h"
    "ext/sqlite3ext.h"
    "ext/stb_image_write.h"
)
source_group("Headers" FILES ${Headers})

set(Sources
    "ext/qrcodegen.cpp"
    "ext/shell.c"
    "ext/sqlite3.c"
    "src/backend/data.cpp"
    "src/backend/match.cpp"
    "src/backend/rfpredict.cpp"
    "src/backend/team.cpp"
    "src/frontend/app.cpp"
    "src/frontend/events.cpp"
    "src/frontend/mainframe.cpp"
)
source_group("Sources" FILES ${Sources})

set(ALL_FILES
    ${no_group_source_files}
    ${Headers}
    ${Sources}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE FRCScout)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
################################################################################
# Output directory
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_OLDCLI    "${CMAKE_SOURCE_DIR}//out/release/"
        OUTPUT_DIRECTORY_RELEASEUI "${CMAKE_SOURCE_DIR}//out/release/"
    )
endif()
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_OLDCLI    "TRUE"
        INTERPROCEDURAL_OPTIMIZATION_RELEASEUI "TRUE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_OLDCLI    "TRUE"
        INTERPROCEDURAL_OPTIMIZATION_RELEASEUI "TRUE"
    )
endif()
################################################################################
# Include directories
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "$<$<CONFIG:OldCLI>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../../wxWidgets-3.2.6/include/msvc;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../../wxWidgets-3.2.6/include"
        ">"
        "$<$<CONFIG:ReleaseUI>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../../wxWidgets-3.2.6/include/msvc;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../../wxWidgets-3.2.6/include"
        ">"
    )
endif()

################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:OldCLI>:"
            "NDEBUG;"
            "_CRT_SECURE_NO_WARNINGS"
        ">"
        "$<$<CONFIG:ReleaseUI>:"
            "NDEBUG;"
            "_CRT_SECURE_NO_WARNINGS"
        ">"
        "_CONSOLE;"
        "UNICODE;"
        "_UNICODE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:OldCLI>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:ReleaseUI>:"
            "NDEBUG"
        ">"
        "WIN32;"
        "_CONSOLE;"
        "UNICODE;"
        "_UNICODE"
    )
endif()

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:OldCLI>:
                /MP;
                /Oi;
                /Gy
            >
            $<$<CONFIG:ReleaseUI>:
                /MP;
                /Oi;
                /Gy
            >
            /permissive-;
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:OldCLI>:
                /Oi;
                /Gy
            >
            $<$<CONFIG:ReleaseUI>:
                /Oi;
                /Gy
            >
            /permissive-;
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    endif()
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /SUBSYSTEM:CONSOLE
            >
            $<$<CONFIG:OldCLI>:
                /SUBSYSTEM:CONSOLE;
                /OPT:REF;
                /OPT:ICF
            >
            $<$<CONFIG:ReleaseUI>:
                /SUBSYSTEM:WINDOWS;
                /OPT:REF;
                /OPT:ICF
            >
            /DEBUG
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:OldCLI>:
                /OPT:REF;
                /OPT:ICF
            >
            $<$<CONFIG:ReleaseUI>:
                /OPT:REF;
                /OPT:ICF
            >
            /DEBUG;
            /SUBSYSTEM:CONSOLE
        )
    endif()
endif()

if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_link_directories(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:OldCLI>:"
            "C:/wxWidgets-3.2.6/lib/vc_x64_lib"
        ">"
        "$<$<CONFIG:ReleaseUI>:"
            "C:/wxWidgets-3.2.6/lib/vc_x64_lib"
        ">"
    )
endif()

