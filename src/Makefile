CXX = g++
CXXFLAGS = -g -O3 -Wall -Wextra -Wpedantic
LDFLAGS = -lPocoNet -lPocoUtil -lPocoFoundation -lPocoNetSSL -lssl -lcrypto -luv
SRC = main.cpp
OUT = ../build/aoi.elf

all: $(OUT)

$(OUT): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(OUT) $(SRC) $(LDFLAGS)

clean:
	rm -f $(OUT)
