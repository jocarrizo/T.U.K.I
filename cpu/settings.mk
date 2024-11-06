# Libraries
LIBS=shared commons pthread

# Custom libraries' paths
SHARED_LIBPATHS=
STATIC_LIBPATHS=../shared

# Compiler flags
CDEBUG=-g -Wall -DDEBUG
CRELEASE=-O3 -Wall -DNDEBUG

# Arguments when executing with start, memcheck or helgrind
ARGS=cfg/cpu.config
ARGS_P_BASE = cfg/cpu_prueba_base.config
ARGS_P_ERROR = cfg/cpu_prueba_error.config
ARGS_P_FS = cfg/cpu_prueba_filesystem.config
ARGS_P_MEMORIA = cfg/cpu_prueba_memoria.config
ARGS_P_DEADLOCK = cfg/cpu_prueba_deadlock.config

# Valgrind flags
MEMCHECK_FLAGS= --leak-check=full --track-origins=yes --log-file=valgrind.log
HELGRIND_FLAGS=

# Source files (*.c) to be excluded from tests compilation
TEST_EXCLUDE=src/cpu.c

INCLUDE_DIR = include
