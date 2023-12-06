
cmake_minimum_required(VERSION 3.26)

project(cpp-transport-catalogue CXX)
set(CMAKE_CXX_STANDARD 17 -O3 -fsanitize=address,undefined -Wall -Wextra -Werror -Wpedantic)

# Эта команда найдёт собранный нами пакет Protobuf.
# REQUIRED означает, что библиотека обязательна.
# Путь для поиска укажем в параметрах команды cmake.
find_package(Protobuf REQUIRED)
find_package(Threads REQUIRED)

# Команда вызова protoc.
# Ей переданы названиия переменных, в которые будут сохранены
# списки сгенерированных файлов, а также сам proto-файл.
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS
        transport-catalogue/transport_catalogue.proto
        transport-catalogue/map_renderer.proto
        transport-catalogue/svg.proto
        transport-catalogue/transport_router.proto
        transport-catalogue/graph.proto
)

# добавляем цель - transport_catalogue
add_executable(
        cpp-transport-catalogue
        ${PROTO_SRCS}
        ${PROTO_HDRS}
        transport-catalogue/main.cpp
        transport-catalogue/domain.cpp
        transport-catalogue/geo.cpp
        transport-catalogue/json_builder.cpp
        transport-catalogue/json.cpp
        transport-catalogue/json_reader.cpp
        transport-catalogue/map_renderer.cpp
        transport-catalogue/request_handler.cpp
        transport-catalogue/serialization.cpp
        transport-catalogue/svg.cpp
        transport-catalogue/transport_catalogue.cpp
        transport-catalogue/transport_router.cpp
)

# find_package определила переменную Protobuf_INCLUDE_DIRS,
# которую нужно использовать как include-путь.
# Также нужно добавить как include-путь директорию, куда
# protoc положит сгенерированные файлы.
target_include_directories(cpp-transport-catalogue PUBLIC ${Protobuf_INCLUDE_DIRS})
target_include_directories(cpp-transport-catalogue PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

# Также find_package определила Protobuf_LIBRARY и Protobuf_LIBRARY_DEBUG.
# Исправим некорректное значение Protobuf_LIBRARY_DEBUG операцией замены:
string(REPLACE "protobuf.lib" "protobufd.lib" "Protobuf_LIBRARY_DEBUG" "${Protobuf_LIBRARY_DEBUG}")
string(REPLACE "protobuf.a" "protobufd.a" "Protobuf_LIBRARY_DEBUG" "${Protobuf_LIBRARY_DEBUG}")

target_link_libraries(cpp-transport-catalogue $<IF:$<CONFIG:Debug>,${Protobuf_LIBRARY_DEBUG},${Protobuf_LIBRARY}> Threads::Threads)


