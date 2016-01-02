CXX      ?= g++
CXXFLAGS ?= -ggdb -Wall -std=c++11
LDFLAGS  ?=

INSTALL ?= install
DESTDIR ?=
prefix  ?= /usr/local
bindir  ?= $(prefix)/bin

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

install:
	$(INSTALL) $(TARGET) $(DESTDIR)$(bindir)/$(TARGET)

uninstall:
	rm -f $(DESTDIR)$(bindir)/$(TARGET)

clean:
	rm -f *.d *.o $(TARGET)

-include $(DEPS)
