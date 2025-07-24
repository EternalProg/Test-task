CXX=g++
CXXFLAGS=-c -std=c++17 -Wall -Wextra
LDFLAGS=-pthread
SOURCES=main.cpp lock_free_queue.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=main

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

.PHONY: all clean