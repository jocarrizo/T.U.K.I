include settings.mk

################################################################################

filename = $(1).out

define compile_bin
gcc $(CFLAGS) -o "$@" $^ $(IDIRS:%=-I%) $(LIBDIRS:%=-L%) $(RUNDIRS:%=-Wl,-rpath,%) $(LIBS:%=-l%)
endef

define compile_objs
@echo "\ncreando objeto $@..."
gcc $(CFLAGS) -c -o "$@" $< $(IDIRS:%=-I%)
endef

################################################################################

# Check if tests folder exists
ifneq ($(wildcard tests/*),)
TESTS_ENABLED=1
endif

# Set prerrequisites
# shell "find src/ -iname "*.{tipo}"" se traduce como: dictame todo lo que este en src cuyo nombre (i-name) sea ...
SRCS_C += $(shell find src/ -iname "*.c")
SRCS_H += $(shell find $(INCLUDE_DIR)/ -iname "*.h")
DEPS = $(foreach SHL,$(SHARED_LIBPATHS),$(SHL:%=%/bin/lib$(notdir $(SHL)).so)) \
	$(foreach STL,$(STATIC_LIBPATHS),$(STL:%=%/bin/lib$(notdir $(STL)).a))

# Set test prerrequisites
ifeq ($(TESTS_ENABLED),1)
TESTS_C += $(shell find tests/ -iname "*.c")
TESTS_H += $(shell find tests/ -iname "*.h")
endif

# Set header paths to (-I)nclude
IDIRS += $(addsuffix /src,$(SHARED_LIBPATHS) $(STATIC_LIBPATHS) .) $(INCLUDE_DIR)

# Set library paths to (-L)ook
LIBDIRS = $(addsuffix /bin,$(SHARED_LIBPATHS) $(STATIC_LIBPATHS))

# Set shared library paths to be found in runtime (-rpath)
RUNDIRS = $(SHARED_LIBPATHS:%=$(shell cd . && pwd)/%/bin)

# Set intermediate objects
OBJS = $(patsubst src/%.c,obj/%.o,$(SRCS_C))

# Set test intermediate objects
ifeq ($(TESTS_ENABLED),1)
TEST_OBJS = $(TESTS_C) $(filter-out $(TEST_EXCLUDE), $(SRCS_C))
endif

# Set binary targets
BIN = bin/$(call filename,$(shell cd . && pwd | xargs basename))

# Set test binary targets
ifeq ($(TESTS_ENABLED),1)
TEST = bin/$(shell cd . && pwd | xargs basename)_tests.out
endif

.PHONY: all
all: CFLAGS = $(CDEBUG)
all: $(BIN) $(TEST)

.PHONY: release
release: CFLAGS = $(CRELEASE)
release: clean $(BIN) $(TEST)

.PHONY: clean
clean:
	-rm -rfv obj bin

.PHONY: watch
watch:
	@test $(shell which entr) || entr
	while sleep 0.1; do \
		find src/ | entr -d make all --no-print-directory; \
	done

$(BIN): $(OBJS) | $(dir $(BIN))
	@echo "\ncompilando filesystem..."
	$(call compile_bin)

obj/%.o: src/%.c $(SRCS_H) $(DEPS) | $(dir $(OBJS))
	$(call compile_objs)

ifeq ($(TESTS_ENABLED),1)
$(TEST): $(TEST_OBJS) | $(dir $(TEST))
	gcc $(CFLAGS) -o "$@" $^ $(IDIRS:%=-I%) $(LIBDIRS:%=-L%) $(RUNDIRS:%=-Wl,-rpath,%) $(LIBS:%=-l%) -lcspecs
endif

.SECONDEXPANSION:
$(DEPS): $$(shell find $$(patsubst %bin/,%src/,$$(dir $$@)) -iname "*.c") \
	$$(shell find $$(patsubst %bin/,%src/,$$(dir $$@)) -iname "*.h")
	make --no-print-directory -C $(patsubst %bin/,%,$(dir $@))

$(sort $(dir $(BIN) $(OBJS))):
	mkdir -pv $@


################################################################################

.PHONY: valgrind
valgrind: $(BIN)
	valgrind --tool=none ./$(BIN) $(ARGS)

.PHONY: daemon
daemon:
	@test $(shell which entr) || entr
	while sleep 0.1; do \
		find src/ | entr -d make valgrind --no-print-directory; \
	done

.PHONY: memcheck
memcheck: $(BIN)
	valgrind $(MEMCHECK_FLAGS) ./$(BIN) $(ARGS)

.PHONY: helgrind
helgrind: $(BIN)
	valgrind --tool=helgrind $(HELGRIND_FLAGS) ./$(BIN) $(ARGS)

.PHONY: exec
start:	$(BIN)
	./$(BIN) $(ARGS)

################################################################################

.PHONY: prueba_base
prueba_base: $(BIN)
	./$(BIN) $(ARGS_P_BASE)

.PHONY: prueba_error
prueba_error: $(BIN)
	./$(BIN) $(ARGS_P_ERROR)

.PHONY: prueba_fs
prueba_fs: $(BIN)
	./$(BIN) $(ARGS_P_FS)

.PHONY: prueba_memoria
prueba_memoria: $(BIN)
	./$(BIN) $(ARGS_P_MEMORIA)

.PHONY: prueba_deadlock
prueba_deadlock: $(BIN)
	./$(BIN) $(ARGS_P_DEADLOCK)

################################################################################

ifeq ($(TESTS_ENABLED),1)
.PHONY: test
test: all
	valgrind --tool=none ./$(TEST)

.PHONY: test-daemon
test-daemon:
	@test $(shell which entr) || entr
	while sleep 0.1; do \
		find tests/ src/ | entr -d make test --no-print-directory; \
	done

.PHONY: test-memcheck
test-memcheck: all
	valgrind --leak-check=full $(MEMCHECK_FLAGS) ./$(TEST)

.PHONY: test-helgrind
test-helgrind: all
	valgrind --tool=helgrind $(HELGRIND_FLAGS) ./$(TEST)
endif
