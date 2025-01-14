#
# cn-cbor testing 
#

vpath %.c src
vpath %.c .

BUILD_VER ?= 0.0
BUILD_DIR ?= build
BIN ?= $(BUILD_DIR)_v$(BUILD_VER)/cncbor_test

# It MUST include the suffix "-"
CROSSPREFIX ?= 

GCC = $(CROSSPREFIX)gcc 	#c compiler
G++ = $(CROSSPREFIX)g++ 	#c++ compiler

LD = $(CROSSPREFIX)ld
READELF = $(CROSSPREFIX)readelf
OBDUMP = $(CROSSPREFIX)objdump

OBJDIR = obj
DEPDIR = dep

SOURCES := $(wildcard ./*.c) 
SOURCES += $(wildcard src/*.c)

SOURCES := $(strip $(notdir $(SOURCES)))


INCLUDES := -I. \
			-I./include -I./src

INCLUDES := $(strip $(INCLUDES))

LIBDIR :=  

LIBDIR := $(strip $(LIBDIR))

LIBS :=  

LIBS := $(strip $(LIBS))

DEFMACRO := 

DEFMACRO := $(strip $(DEFMACRO))

OBJS = $(patsubst %.c,$(OBJDIR)/%.o,$(SOURCES))



CFLAGS = $(strip $(INCLUDES) $(DEFMACRO))
LDFLAGS = $(LIBDIR) $(LIBS)

.SUFFIXES: .o .c
$(OBJDIR)/%.o : %.c
	@if [ ! -d $(OBJDIR) ]; then mkdir -p $(OBJDIR) ; fi
	$(GCC) $(CFLAGS) -c $< -o $@

.PHONY: all clean distclean checkvalue
BIN ?= $(BUILD_DIR)_v$(BUILD_VER)/cncbor_test
all: $(BIN)
$(BIN): $(OBJS)
	@if [ ! -d $(BUILD_DIR)_v$(BUILD_VER) ]; then mkdir -p $(BUILD_DIR)_v$(BUILD_VER) ; fi
	$(GCC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)

clean:
	@rm -rf $(BIN)

distclean:
	@rm -rf $(BUILD_DIR)_v$(BUILD_VER)
	@rm -rf $(OBJDIR) 

checkvalue:
	@echo "Source List : $(SOURCES) "
	@echo "Object List : $(OBJS) "
	@echo "$(GCC) $(G++) $(LD) $(READELF) $(OBJDUMP)"




