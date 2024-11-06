# Libraries
LIBS=shared commons

# Custom libraries' paths
SHARED_LIBPATHS=
STATIC_LIBPATHS=../shared

# Compiler flags
CDEBUG=-g -Wall -DDEBUG
CRELEASE=-O3 -Wall -DNDEBUG

# Arguments when executing with start, memcheck or helgrind
ARGS=cfg/consola.config cfg/codigo.instr

# Valgrind flags
MEMCHECK_FLAGS= --leak-check=full --track-origins=yes --log-file=valgrind.log
HELGRIND_FLAGS=

# Source files (*.c) to be excluded from tests compilation
TEST_EXCLUDE=src/consola.c

INCLUDE_DIR = include