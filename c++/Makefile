CC = gcc
CXX = g++
CXXFLAGS += -std=c++11 -Werror
CFLAGS += -Werror
RM = rm -f

PROGRAMS	= $(basename $(wildcard *.cpp))

.PHONY: all clean

all: $(PROGRAMS)

clean:
	$(RM) $(PROGRAMS)
