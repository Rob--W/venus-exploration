# Build and run test
.PHONY: test-location test-pathlogic
CXXFLAGS=-Wall -fsanitize=address -DONLY_FOR_TESTING_LOCATION_H
CXX=g++ $(CXXFLAGS)

test-location: test-location.cpp Source/location.cpp Source/location.h
	@\
	$(CXX) Source/location.cpp test-location.cpp -o test-location && \
	./test-location

test-pathlogic:
	@\
	$(CXX) Source/pathlogic.cpp test-pathlogic.cpp -o test-pathlogic &&\
	./test-pathlogic
