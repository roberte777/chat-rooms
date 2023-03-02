CC = g++
CFLAGS = -std=c++20
LIBS = -lcurses

SRC_DIR = ../src
BUILD_DIR = .
OBJ_DIR = $(BUILD_DIR)/obj

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

all: server client

server: $(OBJ_DIR)/server.o
	$(CC) $(CFLAGS) $^ -o $(BUILD_DIR)/$@ $(LIBS)

client: $(OBJ_DIR)/client.o
	$(CC) $(CFLAGS) $^ -o $(BUILD_DIR)/$@ $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)/server $(BUILD_DIR)/client $(OBJ_DIR)/*.o
