find_package(PkgConfig REQUIRED)

add_executable(RetroEngine ${RETRO_FILES})

set(RETRO_SUBSYSTEM "CTR" CACHE STRING "The subsystem to use")
if(NOT (RETRO_SUBSYSTEM STREQUAL "CTR"))
    message(FATAL_ERROR "RETRO_SUBSYSTEM must be set to CTR")
endif()
if(NOT GAME_STATIC)
    message(FATAL_ERROR "GAME_STATIC must be on")
endif()

set(RETRO_MOD_LOADER OFF CACHE BOOL "Disable the mod loader" FORCE)

pkg_check_modules(OGG ogg)

if(NOT OGG_FOUND)
    set(COMPILE_OGG TRUE)
    message(NOTICE "libogg not found, attempting to build from source")
else()
    message("found libogg")
    target_link_libraries(RetroEngine ${OGG_STATIC_LIBRARIES})
    target_link_options(RetroEngine PRIVATE ${OGG_STATIC_LDLIBS_OTHER})
    target_compile_options(RetroEngine PRIVATE ${OGG_STATIC_CFLAGS})
endif()

pkg_check_modules(THEORA theora theoradec)

if(NOT THEORA_FOUND)
    message("could not find libtheora, attempting to build manually")
    set(COMPILE_THEORA TRUE)
else()
    message("found libtheora")
    target_link_libraries(RetroEngine ${THEORA_STATIC_LIBRARIES})
    target_link_options(RetroEngine PRIVATE ${THEORA_STATIC_LDLIBS_OTHER})
    target_compile_options(RetroEngine PRIVATE ${THEORA_STATIC_CFLAGS})
endif()


set(SHARED_DEFINES
    SCREEN_XMAX=512 SCREEN_COUNT=1
)
target_compile_definitions(RetroEngine PRIVATE ${SHARED_DEFINES})
target_compile_definitions(${GAME_NAME} PRIVATE ${SHARED_DEFINES})

target_compile_options(RetroEngine PRIVATE -O2 -lcitro2d -lcitro3d)
target_compile_definitions(RetroEngine PRIVATE RETRO_DISABLE_LOG=1)
target_link_libraries(RetroEngine ctru m)

set(PLATFORM 3DS)
ctr_create_3dsx(RetroEngine)
