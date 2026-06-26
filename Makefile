APP_TITLE   := SysInfo
APP_AUTHOR  := panquake
APP_VERSION := 1.0.0
APP_ICON    := icon.jpg

TARGET      := sysinfo
BUILD       := build
SOURCES     := source
INCLUDES    := include
DATA        := data

ARCH        := -march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE

CFLAGS      := -g -Wall -O2 -ffunction-sections \
               $(ARCH) \
               -D__SWITCH__

CXXFLAGS    := $(CFLAGS) -fno-rtti -fno-exceptions

ASFLAGS     := -g $(ARCH)

LDFLAGS     := -specs=$(DEVKITPRO)/libnx/switch.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map)

LIBS        := -lnx

ifneq ($(BUILD),$(notdir $(CURDIR)))

export OUTPUT := $(CURDIR)/$(TARGET)
export TOPDIR := $(CURDIR)
export VPATH  := $(foreach d,$(SOURCES),$(CURDIR)/$(d)) \
                 $(foreach d,$(DATA),$(CURDIR)/$(d))

CFILES   := $(foreach d,$(SOURCES),$(notdir $(wildcard $(CURDIR)/$(d)/*.c)))
CXXFILES := $(foreach d,$(SOURCES),$(notdir $(wildcard $(CURDIR)/$(d)/*.cpp)))
SFILES   := $(foreach d,$(SOURCES),$(notdir $(wildcard $(CURDIR)/$(d)/*.s)))
BINFILES := $(foreach d,$(DATA),$(notdir $(wildcard $(CURDIR)/$(d)/*.*)))

export LD := $(CC)

export OFILES_BIN := $(addsuffix .o,$(BINFILES))
export OFILES_SRC := $(CFILES:.c=.o) $(CXXFILES:.cpp=.o) $(SFILES:.s=.o)
export OFILES     := $(OFILES_BIN) $(OFILES_SRC)

export HFILES_BIN := $(addsuffix .h,$(subst .,_,$(BINFILES)))

export INCLUDE := $(foreach d,$(INCLUDES),-I$(CURDIR)/$(d)) \
                  -I$(DEVKITPRO)/libnx/include \
                  -I$(DEVKITPRO)/portlibs/switch/include

export LIBDIRS := $(DEVKITPRO)/libnx \
                  $(DEVKITPRO)/portlibs/switch

.PHONY: $(BUILD) clean all

all: $(BUILD)

$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

clean:
	@echo Cleaning...
	@rm -rf $(BUILD) $(TARGET).nro $(TARGET).nacp $(TARGET).elf

else

DEPENDS := $(OFILES:.o=.d)

.PHONY: all

all: $(OUTPUT).nro

$(OUTPUT).nro: $(OUTPUT).elf

$(OUTPUT).elf: $(OFILES)

$(OFILES_SRC): $(HFILES_BIN)

-include $(DEPENDS)

endif
