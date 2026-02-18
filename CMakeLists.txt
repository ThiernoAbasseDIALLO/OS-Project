cmake_minimum_required(VERSION 3.10)
project(Simulateur_OS C)

set(CMAKE_C_STANDARD 11)

include_directories(include)

add_executable(simulateur
    src/main.c
    src/process.c
    src/queue.c
    src/scheduler.c
    src/simulateur.c
    src/csv.c
)
