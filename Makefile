# ==============================================================================
# Cross-Platform Makefile
# ==============================================================================

CC        := gcc
CFLAGS    := -Wall -Wextra -Wpedantic -std=c17
CPPFLAGS  := -Iinclude -MMD -MP
LDFLAGS   :=
LDLIBS    :=

BUILD     ?= release
ifeq ($(BUILD),debug)
    CFLAGS += -g3 -O0 -DDEBUG
else
    CFLAGS += -O3 -DNDEBUG
endif

SRC_DIR   := src
BUILD_DIR := build/$(BUILD)
BIN_DIR   := bin

ifeq ($(OS),Windows_NT)
    TARGET_EXT := .exe
    MKDIR_P    = if not exist "$(subst /,\,$(1))" mkdir "$(subst /,\,$(1))"
    RM_R       = if exist "$(subst /,\,$(1))" rmdir /s /q "$(subst /,\,$(1))"
    RUN_PREFIX = .\$1
else
    TARGET_EXT :=
    MKDIR_P    = mkdir -p $(1)
    RM_R       = rm -rf $(1)
    RUN_PREFIX = ./$1
endif

TARGET    := $(BIN_DIR)/parrallell_batcher$(TARGET_EXT)

# Recursive wildcard pure Make function
rwildcard  = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRCS      := $(call rwildcard,$(SRC_DIR),*.c)
OBJS      := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
DEPS      := $(OBJS:.o=.d)

# ------------------------------------------------------------------------------
# Targets
# ------------------------------------------------------------------------------
.PHONY: all clean distclean run info

all: compile_commands.json $(TARGET)

compile_commands.json: $(SRCS)
	@echo Generating compile_commands.json...
	@powershell -NoProfile -Command "\
		$$dir = (Get-Location).Path.Replace('\', '/'); \
		$$files = -split '$(SRCS)'; \
		$$list = foreach ($$f in $$files) { \
			[PSCustomObject]@{ \
				directory = $$dir; \
				file = $$f; \
				command = 'gcc $(CPPFLAGS) $(CFLAGS) -c ' + $$f \
			} \
		}; \
		($$list | ConvertTo-Json -Depth 3) | Set-Content -Encoding utf8 compile_commands.json"

$(TARGET): $(OBJS) | $(BIN_DIR)
	@echo  [LD] $@
	@$(CC) $(OBJS) $(LDFLAGS) $(LDLIBS) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@$(call MKDIR_P,$(dir $@))
	@echo  [CC] $<
	@$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(BUILD_DIR):
	@$(call MKDIR_P,$@)

run: all
	@$(call RUN_PREFIX,$(TARGET))

clean:
	@echo  Cleaning build artifacts...
	@$(call RM_R,$(BUILD_DIR))

distclean:
	@echo  Cleaning all builds...
	@$(call RM_R,build)
	@$(call RM_R,bin)

info:
	@echo Build mode:  $(BUILD)
	@echo OS:          $(OS)
	@echo Sources:     $(SRCS)
	@echo Objects:     $(OBJS)
	@echo Target:      $(TARGET)

-include $(DEPS)
