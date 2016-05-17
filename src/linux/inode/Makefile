#include the make.h
include ./make.h

MODULE_OBJECT_LIST =	$(MODULE_OBJ_DIR)/inodeutil.o \
						$(MODULE_OBJ_DIR)/inodeinit.o \
						$(MODULE_OBJ_DIR)/inodedir.o \
						$(MODULE_OBJ_DIR)/inodecmd.o \
						$(MODULE_OBJ_DIR)/inodecli.o \
						$(MODULE_OBJ_DIR)/inodefix.o \
						$(MODULE_OBJ_DIR)/inodemain.o

MODULE_FINAL_EXE = $(MODULE_EXE_DIR)/inode
#MODULE_STATIC_LIBS_DIR = -L$(BASE_DIR)/common/lib/

MODULE_LIBS_LIST = -lm

#MODULE_LIBS_LIST = -lcmn \
					-lm \
					-lrt \
					-lpthread

$(MODULE_FINAL_EXE): $(MODULE_OBJECT_LIST)
	$(CC) -g -O0 $(MODULE_OBJECT_LIST) $(MODULE_STATIC_LIBS_DIR) $(MODULE_LIBS_LIST) -o $(MODULE_FINAL_EXE)

$(MODULE_OBJ_DIR)/inodeutil.o: $(MODULE_SRC_DIR)/inodeutil.c
	$(CC) $(CC_FLAGS) $(MODULE_FINAL_INC_DIRS) $(MODULE_SRC_DIR)/inodeutil.c -o $(MODULE_OBJ_DIR)/inodeutil.o

$(MODULE_OBJ_DIR)/inodedir.o: $(MODULE_SRC_DIR)/inodedir.c
	$(CC) $(CC_FLAGS) $(MODULE_FINAL_INC_DIRS) $(MODULE_SRC_DIR)/inodedir.c -o $(MODULE_OBJ_DIR)/inodedir.o

$(MODULE_OBJ_DIR)/inodecli.o: $(MODULE_SRC_DIR)/inodecli.c
	$(CC) $(CC_FLAGS) $(MODULE_FINAL_INC_DIRS) $(MODULE_SRC_DIR)/inodecli.c -o $(MODULE_OBJ_DIR)/inodecli.o

$(MODULE_OBJ_DIR)/inodecmd.o: $(MODULE_SRC_DIR)/inodecmd.c
	$(CC) $(CC_FLAGS) $(MODULE_FINAL_INC_DIRS) $(MODULE_SRC_DIR)/inodecmd.c -o $(MODULE_OBJ_DIR)/inodecmd.o

$(MODULE_OBJ_DIR)/inodefix.o: $(MODULE_SRC_DIR)/inodefix.c
	$(CC) $(CC_FLAGS) $(MODULE_FINAL_INC_DIRS) $(MODULE_SRC_DIR)/inodefix.c -o $(MODULE_OBJ_DIR)/inodefix.o

$(MODULE_OBJ_DIR)/inodeinit.o: $(MODULE_SRC_DIR)/inodeinit.c
	$(CC) $(CC_FLAGS) $(MODULE_FINAL_INC_DIRS) $(MODULE_SRC_DIR)/inodeinit.c -o $(MODULE_OBJ_DIR)/inodeinit.o

$(MODULE_OBJ_DIR)/inodemain.o: $(MODULE_SRC_DIR)/inodemain.c
	$(CC) $(CC_FLAGS) $(MODULE_FINAL_INC_DIRS) $(MODULE_SRC_DIR)/inodemain.c -o $(MODULE_OBJ_DIR)/inodemain.o

clean:
	rm -f $(MODULE_OBJ_DIR)/*
	rm -f $(MODULE_EXE_DIR)/*						   

doxy:
	(cat docs/doxy.conf ; echo "PROJECT_NAME=Inode"; echo "PROJECT_BRIEF=\"Inode developer guide\"") | doxygen -
#	doxygen docs/doxy.conf
	ln -fs html/index.html docs/documentation.html
