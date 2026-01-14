# v2.0.3
.PHONY: test custom lib force clear reset main init update shared
.DEFAULT_GOAL=test

CC:=g++
OPT:=-g
STD:=c++20
WFLAGS:=-Wall -Wextra -Wpedantic -Wsuggest-override -Wnull-dereference
SHARED_CFLAGS:=-fPIC
SRCPATH:=src
BUILD_DIR:=.build
DEP_DIR_PATH:=.deps
SHARED_DIR_PATH:=.
CPP_SRC_EXT:=cpp

# Overrides the basic properties
-include makefile.conf

CFLAGS:=-c $(WFLAGS) -std=$(STD)
LDFLAGS:=

MAIN_CPP:=main.$(CPP_SRC_EXT)
TEST_CPP_PATH:=test.$(CPP_SRC_EXT)
TEST_CUSTOM_CPP_PATH:=mtest.$(CPP_SRC_EXT)

TEST_PATH:=test
TEST_SRCPATH:=$(TEST_PATH)/src
TEST_SRCS:=$(wildcard $(TEST_SRCPATH)/*.$(CPP_SRC_EXT))

TEST_CPP:=$(TEST_PATH)/$(TEST_CPP_PATH)
MTEST_CPP:=$(TEST_PATH)/$(TEST_CUSTOM_CPP_PATH)

HDRS=$(wildcard $(SRCPATH)/*.h)
HDPP=$(wildcard $(SRCPATH)/*.hpp)
SRCS=$(wildcard $(SRCPATH)/*.$(CPP_SRC_EXT)) $(EXTRA_SRCS)

# Overriden through the includede makefile.ext
EXT_INCL=
EXT_LIBS=
EXT_RULES=

# Extra test flags from makefile.conf
EXTRA_TF+=

# Extra src files from makefile.conf
EXTRA_SRCS+=

# Extra flags usually overriden directly in command line
EXTRA_CF+=

OBJS=$(addprefix $(BUILD_DIR)/,$(SRCS:%.$(CPP_SRC_EXT)=%.o))

TEST_O:=$(BUILD_DIR)/test/test.o
MTEST_O:=$(BUILD_DIR)/test/mtest.o
MAIN_O:=$(BUILD_DIR)/main.o

DEP_DIR?=$(abspath $(DEP_DIR_PATH))
SHARED_DIR?=$(abspath $(SHARED_DIR_PATH))

CURRENT_SHARED_LIB=$(call get_shared_lib,$(abspath .))
CURRENT_LIB=liboutput.a

DEPFLAGS=-MT $@ -MMD -MP -MF $(BUILD_DIR)/$*.d

LIBS=$(call get_dep_dir,$(STATIC_LIBS) $(DYNAMIC_LIBS))

LIBS_A=$(addsuffix /$(CURRENT_LIB),$(call get_dep_dir,$(STATIC_LIBS)))
LIBS_SHARED=$(call get_shared_lib,$(DYNAMIC_LIBS))

SHARED_INCL=$(addprefix -l,$(call get_dep_name,$(DYNAMIC_LIBS)))

MOD_INC=$(addsuffix /src,$(LIBS))

INCL=-Isrc -I. -I$(DEP_DIR) $(addprefix -I,$(LIBS_INC)) $(addprefix -I,$(MOD_INC)) $(EXT_INCL)

DEPS=$(STATIC_LIBS) $(DYNAMIC_LIBS)

ALL_DEPS=$(wildcard $(DEP_DIR)/*)

define include_prod
	$(eval STATIC_LIBS:=)
	$(eval DYNAMIC_LIBS:=)
	$(eval TEMP_STATIC_LIBS:=)
	$(eval TEMP_DYNAMIC_LIBS:=)
	$(eval INCLUDE_LIBS:=.)

	$(call include_prod_req)

	$(eval STATIC_LIBS:=$(TEMP_STATIC_LIBS))
	$(eval DYNAMIC_LIBS:=$(TEMP_DYNAMIC_LIBS))
endef

define include_prod_req
	$(foreach lib,$(INCLUDE_LIBS), \
		$(eval -include $(lib)/makefile.deps) \
		$(eval -include $(lib)/makefile.flags) \
	)

	$(eval STATIC_LIBS:=$(sort $(STATIC_LIBS)))
	$(eval DYNAMIC_LIBS:=$(sort $(DYNAMIC_LIBS)))

	$(eval STATIC_LIBS:=$(filter-out $(TEMP_STATIC_LIBS),$(STATIC_LIBS)))
	$(eval DYNAMIC_LIBS:=$(filter-out $(TEMP_DYNAMIC_LIBS),$(DYNAMIC_LIBS)))

	$(eval INCLUDE_LIBS:=$(call get_dep_dir,$(STATIC_LIBS) $(DYNAMIC_LIBS)))
	$(eval INCLUDE_LIBS:=$(sort $(INCLUDE_LIBS)))

	$(eval TEMP_STATIC_LIBS+=$(STATIC_LIBS))
	$(eval TEMP_DYNAMIC_LIBS+=$(DYNAMIC_LIBS))

	$(eval STATIC_LIBS:=)
	$(eval DYNAMIC_LIBS:=)

	$(if $(strip $(INCLUDE_LIBS)),$(call include_prod_req),)
endef

define include_all
	$(eval STATIC_LIBS:=)
	$(eval DYNAMIC_LIBS:=)
	$(eval STATIC_DEV_LIBS:=)
	$(eval DYNAMIC_DEV_LIBS:=)
	$(eval TEMP_STATIC_LIBS:=)
	$(eval TEMP_DYNAMIC_LIBS:=)
	$(eval TEMP_STATIC_DEV_LIBS:=)
	$(eval TEMP_DYNAMIC_DEV_LIBS:=)
	$(eval INCLUDE_LIBS:=.)

	$(call include_all_req)

	$(eval STATIC_LIBS:=$(TEMP_STATIC_LIBS) $(TEMP_STATIC_DEV_LIBS))
	$(eval DYNAMIC_LIBS:=$(TEMP_DYNAMIC_LIBS) $(TEMP_DYNAMIC_DEV_LIBS))
endef

define include_all_req
	$(foreach lib,$(INCLUDE_LIBS), \
		$(eval -include $(lib)/makefile.deps) \
		$(eval -include $(lib)/makefile.flags) \
	)

	$(eval STATIC_LIBS:=$(sort $(STATIC_LIBS)))
	$(eval DYNAMIC_LIBS:=$(sort $(DYNAMIC_LIBS)))
	$(eval STATIC_LIBS:=$(filter-out $(TEMP_STATIC_LIBS),$(STATIC_LIBS)))
	$(eval DYNAMIC_LIBS:=$(filter-out $(TEMP_DYNAMIC_LIBS),$(DYNAMIC_LIBS)))

	$(eval STATIC_DEV_LIBS:=$(sort $(STATIC_DEV_LIBS)))
	$(eval DYNAMIC_DEV_LIBS:=$(sort $(DYNAMIC_DEV_LIBS)))
	$(eval STATIC_DEV_LIBS:=$(filter-out $(TEMP_STATIC_DEV_LIBS),$(STATIC_DEV_LIBS)))
	$(eval DYNAMIC_DEV_LIBS:=$(filter-out $(TEMP_DYNAMIC_DEV_LIBS),$(DYNAMIC_DEV_LIBS)))

	$(eval INCLUDE_LIBS:=$(call get_dep_dir,$(STATIC_LIBS) $(DYNAMIC_LIBS) $(STATIC_DEV_LIBS) $(DYNAMIC_DEV_LIBS)))
	$(eval INCLUDE_LIBS:=$(sort $(INCLUDE_LIBS)))

	$(eval TEMP_STATIC_LIBS+=$(STATIC_LIBS))
	$(eval TEMP_DYNAMIC_LIBS+=$(DYNAMIC_LIBS))
	$(eval TEMP_STATIC_DEV_LIBS+=$(STATIC_DEV_LIBS))
	$(eval TEMP_DYNAMIC_DEV_LIBS+=$(DYNAMIC_DEV_LIBS))

	$(if $(strip $(INCLUDE_LIBS)),$(call include_all_req),)
endef

define get_dep_name
$(basename $(notdir $(1)))
endef

define get_dep_dir
$(addprefix $(DEP_DIR)/,$(basename $(notdir $(1))))
endef

define get_shared_lib
$(patsubst %,$(SHARED_DIR)/lib%.so,$(basename $(notdir $(1))))
endef

define newline


endef

####################################
# Include the correct dependencies #
####################################
-include makefile.ext

ifneq (,$(filter init,$(MAKECMDGOALS)))
-include makefile.deps
$(eval STATIC_LIBS+=$(STATIC_DEV_LIBS))
$(eval DYNAMIC_LIBS+=$(DYNAMIC_DEV_LIBS))
endif

ifneq (,$(filter main lib shared,$(MAKECMDGOALS)))
$(call include_prod)
endif

ifneq (,$(filter test mtest,$(MAKECMDGOALS)))
$(call include_all)
endif
ifeq (,$(strip $(MAKECMDGOALS)))
$(call include_all)
endif

####################################
#           ALL TARGETS            #
####################################

# dir dependencies
$(DEP_DIR):
	mkdir $(DEP_DIR)
$(BUILD_DIR):
	mkdir $(BUILD_DIR)

test: EXTRACFLAGS:=$(EXTRA_TF) $(EXTRA_CF)
test: INCL+=-Itest
test: test.exe

custom: EXTRACFLAGS:=$(EXTRA_CF)
custom: mtest.exe

main: EXTRACFLAGS:=$(EXTRA_CF)
main: main.exe

$(TEST_O): $(TEST_SRCS)

$(BUILD_DIR)/%.o: %.$(CPP_SRC_EXT) | $(BUILD_DIR)
$(BUILD_DIR)/%.o: %.$(CPP_SRC_EXT) $(BUILD_DIR)/%.d | $(BUILD_DIR)
	@mkdir -p $(@D)
	$(CC) $(DEPFLAGS) $(CFLAGS) $(EXTRACFLAGS) $(OPT) $< -o $@ $(INCL)

ALL_O_FILES:=$(SRCS) $(TEST_SRCS) $(TEST_CPP) $(MTEST_CPP) $(MAIN_CPP)
DEPFILES:=$(ALL_O_FILES:%.$(CPP_SRC_EXT)=$(BUILD_DIR)/%.d)
$(DEPFILES):

$(CURRENT_LIB): $(OBJS)
	$(RM) $(CURRENT_LIB)
	ar -qcT $(CURRENT_LIB) $(OBJS)

$(LIBS_A): force
	$(MAKE) -C $(dir $@) CFLAGS="$(CFLAGS)" OPT="$(OPT)" lib

$(CURRENT_SHARED_LIB): $(OBJS)
	$(CC) -shared -o $@ $(OBJS)

$(LIBS_SHARED): force
	$(MAKE) -C "$(DEP_DIR)/$(patsubst lib%.so,%,$(notdir $@))" CFLAGS="$(CFLAGS) $(SHARED_CFLAGS)" OPT="$(OPT)" SHARED_DIR="$(SHARED_DIR)" shared

# Include generated o targets
include $(wildcard $(DEPFILES))

####################################
#          Public targets          #
####################################

# Init all of the dependencies recursively in the $(DEP_DIR) folder
init: force | $(DEP_DIR)
	$(foreach x,$(DEPS),\
		if [ ! -d "$(call get_dep_dir,$(x))" ]; then \
			git clone $(x) "$(call get_dep_dir,$(x))"; \
		fi ${newline} \
		$(MAKE) -C "$(call get_dep_dir,$(x))" DEP_DIR="$(DEP_DIR)" init ${newline} \
	)

# Update all inited dependencies
update: force
	$(foreach x,$(ALL_DEPS),\
		git -C "$(x)" pull ${newline} \
	)

# Create static library
lib: $(CURRENT_LIB)

# Create dynamic library
shared: $(CURRENT_SHARED_LIB)

# Cleans current lib and all dependencies
reset: clear
	$(foreach mod,$(ALL_DEPS),$(MAKE) -C $(mod) clear $(newline))

# Cleans only current lib w/o dependencies
clear:
	$(RM) *.exe $(CURRENT_LIB) *.so
	$(RM) -rf $(BUILD_DIR)

# Main test endpoint
test.exe: $(CURRENT_LIB) $(TEST_O) $(LIBS_A) $(LIBS_SHARED) $(EXT_RULES)
	$(CC) -o test.exe $(TEST_O) $(CURRENT_LIB) $(LDFLAGS) $(LIBS_LD) $(LIBS_A) $(EXT_LIBS) -L$(SHARED_DIR) $(SHARED_INCL)

# Custom test endpoint
mtest.exe: $(CURRENT_LIB) $(MTEST_O) $(LIBS_A) $(LIBS_SHARED) $(EXT_RULES)
	$(CC) -o mtest.exe $(MTEST_O) $(CURRENT_LIB) $(LDFLAGS) $(LIBS_LD) $(LIBS_A) $(EXT_LIBS) -L$(SHARED_DIR) $(SHARED_INCL)

# Program executable
main.exe: $(CURRENT_LIB) $(MAIN_O) $(LIBS_A) $(LIBS_SHARED) $(EXT_RULES)
	$(CC) -o main.exe $(MAIN_O) $(CURRENT_LIB) $(LDFLAGS) $(LIBS_LD) $(LIBS_A) $(EXT_LIBS) -L$(SHARED_DIR) $(SHARED_INCL)
