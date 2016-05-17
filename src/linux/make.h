#Base Directory
BASE_DIR = ${PWD}

#Compiler configuration
CC = gcc

# Compilation flags/switches
#DEBUG_FLAGS = -DTRACE_WANTED

CC_FLAGS = -c -g -O -gdwarf-3 -Wunused \
           $(DEBUG_FLAGS) \
