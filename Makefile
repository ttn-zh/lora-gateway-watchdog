APP_NAME := watchdog

LGW_PATH ?= ../lora_gateway/libloragw
RELEASE_VERSION := $(shell cat VERSION)

CC := gcc
CFLAGS := -O2 -Wall -Wextra -std=c99 -Iinc -I.
VFLAG := -D VERSION_STRING="\"$(RELEASE_VERSION)\""
LIBS := -lloragw -lrt -lpthread

LGW_INC = $(LGW_PATH)/inc/loragw_hal.h


# Build targets

all: $(APP_NAME)

clean:
	rm -f obj/*.o
	rm -f $(APP_NAME)

### Main program compilation and assembly

obj/$(APP_NAME).o: src/$(APP_NAME).c $(LGW_INC)
	$(CC) -c $(CFLAGS) $(VFLAG) -I$(LGW_PATH)/inc $< -o $@

$(APP_NAME): obj/$(APP_NAME).o $(LGW_PATH)/libloragw.a
	$(CC) -L$(LGW_PATH) $< -o $@ $(LIBS)
