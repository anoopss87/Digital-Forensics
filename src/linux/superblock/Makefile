#include the make.h
include ./make.h

MODULE_OBJECT_LIST = $(MODULE_OBJ_DIR)/superblock.o \
						$(MODULE_OBJ_DIR)/main.o

MODULE_FINAL_EXE = $(MODULE_EXE_DIR)/superblock

MODULE_LIBS_LIST = -lm

$(MODULE_FINAL_EXE): $(MODULE_OBJECT_LIST)
	@echo $(MODULE_LIB_DIR)
	$(CC) -g -O2 $(MODULE_OBJECT_LIST) $(MODULE_STATIC_LIBS_DIR) $(MODULE_LIBS_LIST) -o $(MODULE_FINAL_EXE)

$(MODULE_OBJ_DIR)/superblock.o: $(MODULE_SRC_DIR)/superblock.c
	$(CC) $(CC_FLAGS) $(MODULE_FINAL_INC_DIRS) -o $(MODULE_OBJ_DIR)/superblock.o $(MODULE_SRC_DIR)/superblock.c

$(MODULE_OBJ_DIR)/main.o: $(MODULE_SRC_DIR)/main.c
	$(CC) $(CC_FLAGS) $(MODULE_FINAL_INC_DIRS) -o $(MODULE_OBJ_DIR)/main.o $(MODULE_SRC_DIR)/main.c

clean:
	rm -f $(MODULE_OBJ_DIR)/*
	rm -f $(MODULE_EXE_DIR)/*

