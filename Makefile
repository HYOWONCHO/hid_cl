#
#
#


#if [ -a ./cross_set.inc ] ; 	\
#then 							\
#	include ./cross_set.inc 	\
#fi; 							\

ifeq (,"./cross_set.inc")
	include ./cross_set.inc
endif

CC = $(CROSS_PREFIX)gcc

TARGET := hidcl
TARGET_BIN := $(TARGET)_bin
OBJDIR := obj
#OBJS := $(addprefix $(OBJDIR)/, )


vpath %.h ./inc
vpath %.c ./src
vpath %.c .


# Source 
HID_SOURCES := $(wildcard src/*.c)
MAIN_SOURCES := $(wildcard *.c)
SOURCES := $(MAIN_SOURCES) \
		   $(HID_SOURCES)

SOURCES := $(strip $(notdir $(SOURCES)))

#Header
HEADERS := -I. \
		  -I ./inc

HEADERS := $(strip $(HEADERS))

OBJS  = $(patsubst %.c, $(OBJDIR)/%.o,$(SOURCES))

LIBPATH := 
LIBS := -lhidapi-libusb

LDFLAGS := \
		   $(LIBPATH) \
		   $(LIBS)


.SUFFIXES: .o .c
$(OBJDIR)/%.o : %.c
	@if [ ! -d $(OBJDIR) ]; then mkdir -p $(OBJDIR) ; fi
	$(CC) $(CFLAGS) -c $< -o $@


all: $(TARGET_BIN)

$(TARGET_BIN) : $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)

.PHONY : testv
testv:
	@echo "SOURCES : $(SOURCES)"
	@echo "OBJS: $(OBJS)"


.PHONY : clean
clean:
	-rm -rf  $(OBJDIR)
	-rm -f 	$(TARGET_BIN) 
