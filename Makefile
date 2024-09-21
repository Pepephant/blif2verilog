
build: main.cpp blif2verilog.cpp
	g++ -o blif2verilog main.cpp blif2verilog.cpp

run: build
	./blif2verilog

clean:
	rm blif2verilog
	rm test.v