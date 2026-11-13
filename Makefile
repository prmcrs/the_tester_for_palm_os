# PRC-Tools / Palm OS SDK 5.x (classic 68K)
# Requires m68k-palmos-gcc, m68k-palmos-obj-res and build-prc on PATH.
APP = TheTester
CREATOR = PrTe
SOURCES = src/main.c src/network.c
OBJECTS = $(SOURCES:.c=.o)

CC = m68k-palmos-gcc
CFLAGS = -O2 -Wall -I./src
LDFLAGS =
.PHONY: all clean

all: $(APP).prc

$(APP): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS)

$(APP).prc: $(APP)
	m68k-palmos-obj-res $(APP)
	build-prc $(APP).prc "The Tester" $(CREATOR) $(APP) *.bin

clean:
	$(RM) $(OBJECTS) $(APP) $(APP).prc *.bin
