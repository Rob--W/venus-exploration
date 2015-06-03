# Build and run test
.PHONY: test-location test-pathlogic

test-location: test-location.cpp Source/location.cpp Source/location.h
	@\
	g++ Source/location.cpp test-location.cpp -o test-location -Wall && \
	./test-location

test-pathlogic:
	@\
	g++ Source/pathlogic.cpp test-pathlogic.cpp -o test-pathlogic -Wall -fsanitize=address &&\
	./test-pathlogic
