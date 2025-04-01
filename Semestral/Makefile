CXX      =clang++
CXXFLAGS =-Wall -pedantic -std=c++20 -I./libs/ -O2
LD       =$(CXX)
LDFLAGS  =$(CXXFLAGS)

DEBUGFLAGS =-g -DDEBUG -O0

SRC =  cli/main.cpp

DEBUG_DIR=_debug

OBJ = cli.out debug_cli.out

all: $(OBJ)

cli.out: $(SRC:.cpp=.o)
	$(LD) $(LDFLAGS) -o $@ $^

debug_cli.out: $(addprefix $(DEBUG_DIR)/, $(SRC:.cpp=.o))
	$(LD) $(LDFLAGS) $(DEBUGFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(DEBUG_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(DEBUGFLAGS) -c -o $@ $<

deps:
	$(CXX) -MM $(SRC) > Makefile.d

clean:
	rm *.out $(SRC:.cpp=.o)
	rm -r $(DEBUG_DIR)
	-rm Makefile.d

-include Makefile.d
