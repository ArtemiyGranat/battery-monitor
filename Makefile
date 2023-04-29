.PHONY: run

CC=g++
CXXFLAGS=-Wall -Wextra -Werror -std=c++17 -lsystemd
TARGET=main
run:
	@${CC} ${CXXFLAGS} ${TARGET}.cpp