.PHONY: run

CXXFLAGS=-Wall -Wextra -Werror -std=c++17
TARGET=main
run:
	@${CXX} ${CXXFLAGS} ${TARGET}.cpp -lsystemd