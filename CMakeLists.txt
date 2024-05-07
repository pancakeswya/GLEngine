cmake_minimum_required(VERSION 3.22)
project(gl_proj C)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)

set(SRC_DIR src)
set(THIRD_PARTY_DIR third_party)

include_directories(
        "${SRC_DIR}",
        "${THIRD_PARTY_DIR}"
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra -O3")

add_executable(gl_proj
        ${THIRD_PARTY_DIR}/stb/stb_image.h
        ${THIRD_PARTY_DIR}/hashmap/hashmap.h
        ${THIRD_PARTY_DIR}/hashmap/hashmap.c
        ${THIRD_PARTY_DIR}/ini/ini.h
        ${THIRD_PARTY_DIR}/ini/ini.c

        ${SRC_DIR}/base/error.h
        ${SRC_DIR}/base/error.c
        ${SRC_DIR}/base/config.h
        ${SRC_DIR}/base/config.c
        ${SRC_DIR}/base/io.h
        ${SRC_DIR}/base/io.c
        ${SRC_DIR}/base/vector.h
        ${SRC_DIR}/base/vector.c
        ${SRC_DIR}/base/strutil.h
        ${SRC_DIR}/base/strutil.c
        ${SRC_DIR}/engine/render/object.h
        ${SRC_DIR}/engine/render/object.c
        ${SRC_DIR}/engine/render/maps.h
        ${SRC_DIR}/engine/render/maps.c
        ${SRC_DIR}/engine/render/context.h
        ${SRC_DIR}/engine/render/context.c
        ${SRC_DIR}/engine/render/render.h
        ${SRC_DIR}/engine/render/render.c
        ${SRC_DIR}/engine/engine.h
        ${SRC_DIR}/engine/engine.c
        ${SRC_DIR}/engine/window/window.h
        ${SRC_DIR}/engine/window/window.c
        ${SRC_DIR}/log/log.h
        ${SRC_DIR}/log/log.c
        ${SRC_DIR}/main.c
        ${SRC_DIR}/obj/data.h
        ${SRC_DIR}/obj/data.c
        ${SRC_DIR}/obj/parser.h
        ${SRC_DIR}/obj/parser.c
)

target_link_libraries(gl_proj OpenGL::GL GLEW::GLEW glfw m)

add_compile_definitions(INI_STOP_ON_FIRST_ERROR INI_USE_STACK=0)

#add_compile_definitions(DEBUG)