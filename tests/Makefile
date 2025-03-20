CXX = g++
CXXFLAGS = -g -O3 -Wall -Wextra -Wpedantic
LDFLAGS = -lPocoNet -lPocoUtil -lPocoFoundation -lPocoNetSSL -lssl -lcrypto -luv
SRC = tests.cpp
OUT = ../build/tests/tests.elf

all: $(OUT)

$(OUT): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(OUT) $(SRC) $(LDFLAGS)

clean:
	rm -f $(OUT)
