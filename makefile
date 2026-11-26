# @author Adam T Koehler, PhD
# date October 12, 2026
#
# This makefile file provides insights on how to compile and 
# and execute your program. 
#
# We also replaced the run buttons commands to be the appropiate
# combination of compiling and executing from below.
#

#
# to execute in terminal type "make build"
# or replace "build" with another target name from below 
#

# If you create your own files, to add them to the commands,
# you can add your .cpp files to the space separated
# GAME_SRCS list after the game.cpp file name.

# --- Variables ---
CXX = g++
CXXFLAGS = -I includes -I rayincludes -Wall -fconcepts -std=c++23 -Llib -lraylib -Wl,-rpath=./lib -l:libGL.so.1 -lm -lpthread -ldl -lrt -lX11

# --- Source Files ---
# STUDENTS: Add your group's .cpp files to this list, separated by spaces.
# Example: GROUP_SRCS = src/player.cpp src/level.cpp src/menu.cpp
GROUP_SRCS = button.cpp
SHARED_SRCS = src/cs141random.cpp
GAME_SRCS = $(SHARED_SRCS) $(GROUP_SRCS) game.cpp
EXAMPLE_SRCS = $(SHARED_SRCS) examples.cpp

# --- Phony Targets Declaration ---
.PHONY: build run_game examples run_examples clean

# --- Targets ---
build:
	rm -f game.out
	$(CXX) $(GAME_SRCS) -o game.out $(CXXFLAGS) 

run_game:
	clear;./game.out

examples:
	rm -f examples.out
	$(CXX) $(EXAMPLE_SRCS) -o examples.out $(CXXFLAGS) 

run_examples:
	clear;./examples.out

clean:
	rm -f game.out
	rm -f examples.out