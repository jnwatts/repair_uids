CXX      ?= g++
CXXFLAGS ?= -ggdb -Wall -std=c++11
LDFLAGS  ?=

TARGET = repair_uids
SRCS   = repair_uids.cpp iddb.cpp dig.cpp string_utils.cpp
OBJS   = $(SRCS:.cpp=.o)
DEPS   = $(SRCS:.cpp=.d)

.PHONY: clean all

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJS) -o $(TARGET)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.d: %.cpp
	$(CXX) -M $(CXXFLAGS) $< > $@

clean:
	rm -f *.d *.o $(TARGET)

-include $(DEPS)
