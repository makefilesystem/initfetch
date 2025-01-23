CC=gcc
CFLAGS=-o
LDFLAGS=-lX11
SRC_DIR=src
BIN_DIR=.
INSTALL_DIR=/usr/bin

TARGET=initfetch
SRC_FILES=$(SRC_DIR)/initfetch.c $(SRC_DIR)/xlib.h

all: $(TARGET)

$(TARGET): $(SRC_FILES)
	$(CC) $(CFLAGS) $(BIN_DIR)/$(TARGET) $(SRC_FILES) $(LDFLAGS)

install: $(TARGET)
	install -m 755 $(BIN_DIR)/$(TARGET) $(INSTALL_DIR)

clean:
	rm -f $(BIN_DIR)/$(TARGET)

uninstall:
	rm -f $(INSTALL_DIR)/$(TARGET)

