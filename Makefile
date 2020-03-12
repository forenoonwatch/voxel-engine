TARGET_EXEC := VoxelEngine

BUILD_DIR := bin
SRC_DIRS := src

rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))
MKDIR_P := mkdir -p

SRCS := $(call rwildcard, $(SRC_DIRS)/, *.cpp *.c)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

UNAME := $(shell uname -s)

ifeq ($(UNAME), Linux)
	LDLIBS := $(shell pkg-config glfw3 --static --libs) $(shell pkg-config glew --static --libs) $(shell pkg-config assimp --static --libs)
	LDFLAGS := -Wall -fuse-ld=gold

	CXXFLAGS := -std=c++17 -g -ggdb -I$(CURDIR)/src
else
	LDLIBS := -lglu32 -lopengl32 -lglew32 -lglfw3 -lassimp.dll 
	LDFLAGS := -Wall

	CXXFLAGS := -std=c++17 -g -ggdb -I$(CURDIR)/src
endif

all: game

game: $(BUILD_DIR)/$(TARGET_EXEC)

run:
#	@echo "Running $(TARGET_EXEC)..."
	@"./$(BUILD_DIR)/$(TARGET_EXEC)"

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
#	@echo "Building $(TARGET_EXEC)..."
	$(CXX) $^ -o $@ $(LDFLAGS) $(LDLIBS)

$(BUILD_DIR)/%.cpp.o: %.cpp
#	@echo "Building $@..."
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: all run game 
