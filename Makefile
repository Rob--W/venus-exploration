# Build and run test
test: test-location.cpp Source/location.cpp Source/location.h
	@\
	g++ Source/location.cpp test-location.cpp -o test-location -Wall && \
	./test-location
