
build: main.cpp blif2verilog.cpp
	g++ -o    blif2verilog main.cpp blif2verilog.cpp

debug: main.cpp blif2verilog.cpp
	g++ -g -o  blif2verilog main.cpp blif2verilog.cpp

run: build
	./blif2verilog

tograph: diregraph.cpp blif2verilog.cpp ml_rcs.cpp
	g++ -o diregraph diregraph.cpp blif2verilog.cpp ml_rcs.cpp

debugtograph: diregraph.cpp blif2verilog.cpp 
	g++ -g  -o  diregraph diregraph.cpp blif2verilog.cpp

clean:
	rm blif2verilog
	rm diregraph