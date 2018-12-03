CC = g++
CFLAGS = -std=c++14 -c -g -O2 -Wall
INCLUDES += -I./
LDFLAGS = -L/usr/local/lib

SOURCE=alg6.cpp global.cpp experiment.cpp cost.cpp lcs.cpp print.cpp read_chains.cpp update.cpp
OBJECT=$(SOURCE:.cpp=.o)
TARGET=main

.PHONY: all  clean
all: $(TARGET)

$(TARGET): $(OBJECT)
	$(CC) $^ $(LDFLAGS) -o $@ 
.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@ 

clean:
	\rm -rf $(OBJECT) $(TARGET)

