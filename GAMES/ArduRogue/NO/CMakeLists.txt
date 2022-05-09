cmake_minimum_required(VERSION 3.15)

cmake_policy(SET CMP0091 NEW)
cmake_policy(SET CMP0117 NEW)

project(ardurogue)

option(PLAYER_INVULNERABLE "Make the player invulnerable" OFF)

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

set(ARDUROGUE_SOURCES
    game.hpp
    ctstr.hpp
    draw.cpp
    entity.cpp
    font.cpp
    game.cpp
    generate.cpp
    light.cpp
    menus.cpp
    monsters.cpp
    save.cpp
    sprintf.cpp
    stack.cpp
    status.cpp
    strings.cpp
    use.cpp
)

add_executable(
    ${PROJECT_NAME} ${WINFLAG}
    README.md
    guide.md
    .editorconfig
    main_emscripten.cpp
    main_win32.cpp
    ${ARDUROGUE_SOURCES}
)

target_link_libraries(${PROJECT_NAME} PUBLIC git)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED 11)

if(PLAYER_INVULNERABLE)
    target_compile_definitions(${PROJECT_NAME} PUBLIC -DPLAYER_INVULNERABLE=1)
endif()

if(NOT EMSCRIPTEN)
    add_executable(tables tools/tables.cpp ${ARDUROGUE_SOURCES})
    set_property(TARGET tables PROPERTY CXX_STANDARD_REQUIRED 11)
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
