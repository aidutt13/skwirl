CXX := c++
CXXFLAGS := -std=c++20 -Wall -Wextra -Werror -pedantic

SRCDIR := src
BUILDDIR := build
INCLUDEDIR := include
TARGET := bin/skwirl

SRC := $(wildcard $(SRCDIR)/*.cpp) $(wildcard $(SRCDIR)/*/*.cpp)
OBJ := $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(SRC))

$(TARGET): $(OBJ)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -I$(INCLUDEDIR) -c -o $@ $<

.PHONY: clean debug release run cc

clean:
	rm -f $(BUILDDIR)/*.o $(TARGET)

debug: CXXFLAGS += -g

release: CXXFLAGS += -O3

run: $(TARGET)
	./$(TARGET)

cc:
	./cc.sh