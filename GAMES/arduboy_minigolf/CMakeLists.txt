cmake_minimum_required(VERSION 3.15)

cmake_policy(SET CMP0091 NEW)
cmake_policy(SET CMP0117 NEW)

project(minigolf)
if(EMSCRIPTEN)
    option(EMSCRIPTEN_OPTIMIZED "Produce size-optimized emscripten build" ON)
    if(EMSCRIPTEN_OPTIMIZED)
        set(EXTRA_FLAGS "${EXTRA_FLAGS} -Oz")
        set(EXTRA_FLAGS "${EXTRA_FLAGS} -flto")
        set(EXTRA_FLAGS "${EXTRA_FLAGS} --closure 1")
        set(EXTRA_FLAGS "${EXTRA_FLAGS} -fno-rtti -fno-exceptions")
        add_link_options("-s MODULARIZE=1")
        add_link_options("-s ALLOW_UNIMPLEMENTED_SYSCALLS=0")
        add_link_options("-flto")
    endif()
    set(EXTRA_FLAGS "${EXTRA_FLAGS} -Wall")
    set(EXTRA_FLAGS "${EXTRA_FLAGS} -Wno-unused-command-line-argument")
    set(EXTRA_FLAGS "${EXTRA_FLAGS} -Wno-unused-function")
    set(EXTRA_FLAGS "${EXTRA_FLAGS} -s ASYNCIFY")
    set(EXTRA_FLAGS "${EXTRA_FLAGS} -sEXPORTED_FUNCTIONS=_main,_initpers")
    set(EXTRA_FLAGS "${EXTRA_FLAGS} -sEXPORTED_RUNTIME_METHODS=ccall,FS")
    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   ${EXTRA_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_FLAGS}")
    add_link_options("-lidbfs.js")
endif()

set(PRE_CONFIGURE_FILE "git.hpp.in")
# write to source folder cause arduino ide :(
set(POST_CONFIGURE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/git.hpp")
include(git_watcher.cmake)
add_library(git INTERFACE ${POST_CONFIGURE_FILE})
target_include_directories(git INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(git check_git)

set(WINFLAG)
if(WIN32)
    set(WINFLAG WIN32)
endif()

set(MINIGOLF_SOURCES
    game.hpp
    camera.cpp
    div.cpp
    mul.cpp
    game.hpp
    draw.cpp
    graphics.cpp
    physics.cpp
    sincos.cpp
    render_scene.cpp
    mat.cpp
    game.cpp
    levels.cpp
)

add_executable(convert_obj tools/convert_obj.cpp)
set(LEVELN 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17)
foreach(N ${LEVELN})
    set(LEVEL_OBJ "${CMAKE_CURRENT_SOURCE_DIR}/levels/level_${N}.obj")
    set(LEVEL_HPP "${CMAKE_CURRENT_SOURCE_DIR}/levels/level_${N}.hpp")
    set(LEVEL_INFO_HPP "${CMAKE_CURRENT_SOURCE_DIR}/levels/level_${N}_info.hpp")
    add_custom_command(
        OUTPUT "${LEVEL_HPP}"
        COMMAND convert_obj "${LEVEL_OBJ}" "${LEVEL_HPP}" "${N}"
        DEPENDS convert_obj "${LEVEL_OBJ}"
    )
    list(APPEND MINIGOLF_SOURCES "${LEVEL_HPP}")
    list(APPEND MINIGOLF_SOURCES "${LEVEL_INFO_HPP}")
endforeach()

add_executable(
    ${PROJECT_NAME} ${WINFLAG}
    README.md
    .editorconfig
    main_emscripten.cpp
    main_win32.cpp
    ${MINIGOLF_SOURCES}
)

if(WIN32)
    add_executable(
        level_editor
        tools/level_editor.cpp
        tools/imgui/backends/imgui_impl_win32.cpp
        tools/imgui/backends/imgui_impl_win32.h
        tools/imgui/backends/imgui_impl_dx9.h
        tools/imgui/backends/imgui_impl_dx9.cpp
        ${MINIGOLF_SOURCES}
    )
    target_include_directories(level_editor PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/tools/imgui"
        "${CMAKE_CURRENT_SOURCE_DIR}/tools/fmt"
    )
    target_compile_definitions(level_editor PUBLIC
        -DBASE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC git)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED 11)

if(NOT EMSCRIPTEN)
    add_executable(gen_math_tables tools/gen_math_tables.cpp)
    add_executable(test_math tools/test_math.cpp ${MINIGOLF_SOURCES})
endif()

if(MSVC)
    set_property(TARGET ${PROJECT_NAME} PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    target_link_options(${PROJECT_NAME} PUBLIC
        /DEBUG
        /MAP
        /EMITPOGOPHASEINFO
        /MANIFEST:NO
        /DYNAMICBASE:NO
        /FIXED
        /INCREMENTAL:NO
        /OPT:REF
        /OPT:ICF=16
        /SAFESEH:NO
        $<$<NOT:$<CONFIG:Debug>>:/ENTRY:main>
        $<$<NOT:$<CONFIG:Debug>>:/NODEFAULTLIB>
    )
    target_compile_definitions(${PROJECT_NAME} PUBLIC -D_HAS_EXCEPTIONS=0)
    target_compile_options(${PROJECT_NAME} PUBLIC /GR- /GS-)
    #if(CMAKE_CXX_FLAGS MATCHES "/EHsc ")
    #    string(REPLACE "/EHsc" "/EHs-c-" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    #endif()
    #if(CMAKE_CXX_FLAGS MATCHES "/EHsc$")
    #    string(REPLACE "/EHsc" "/EHs-c-" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    #endif()
endif()
