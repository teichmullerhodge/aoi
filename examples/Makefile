CXX = g++
CXXFLAGS = -g -O3 -Wall -Wextra -Wpedantic
LDFLAGS = -lPocoNet -lPocoUtil -lPocoFoundation -lPocoNetSSL -lssl -lcrypto -luv
SRC = sleeping_example.cpp
OUT = ../build/examples/sleeping_example.elf

all: $(OUT)

$(OUT): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(OUT) $(SRC) $(LDFLAGS)

clean:
	rm -f $(OUT)
