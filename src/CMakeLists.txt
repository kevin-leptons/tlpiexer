file(GLOB TLPIEXER_SRC_FILES lib/*.c)
add_library(tlpiexer-slave SHARED ${TLPIEXER_SRC_FILES})

file(GLOB EXE_SRC_FILES imp/*.c)
file(GLOB EXE_SRC_RFILES imp/42-01*.c)
list(REMOVE_ITEM EXE_SRC_FILES ${EXE_SRC_RFILES})
foreach(EXE_SRC_FILE ${EXE_SRC_FILES})
    get_filename_component(EXE_DEST_FILE ${EXE_SRC_FILE} NAME_WE)
    add_executable(${EXE_DEST_FILE} ${EXE_SRC_FILE})
    target_link_libraries(${EXE_DEST_FILE} tlpiexer-slave)
endforeach()

# exception for 42-01
add_executable(42-01 imp/42-01.c)
add_library(tlpi421a SHARED imp/42-01.liba.c)
add_library(tlpi421b SHARED imp/42-01.libb.c)
target_link_libraries(42-01 tlpiexer-slave)
target_link_libraries(tlpi421b tlpi421a)
