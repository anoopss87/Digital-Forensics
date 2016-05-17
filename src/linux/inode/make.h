include ../make.h
#Module name (directory name )
MODULE = inode

MODULE_DIR = ${BASE_DIR}/${MODULE}

#Include files directory
MODULE_INC_DIR = ${MODULE_DIR}/inc

#Source files directory
MODULE_SRC_DIR = ${MODULE_DIR}/src

#Object files directory
MODULE_OBJ_DIR = ${MODULE_DIR}/obj

#Library files directory
MODULE_EXE_DIR = ${MODULE_DIR}/exe

#Library files directory
MODULE_LIB_DIR = 

#Other include directiories
COMMON_INC_DIR = ${BASE_DIR}/common/inc

MODULE_FINAL_INC_DIRS = -I${MODULE_INC_DIR} \
                        -I${COMMON_INC_DIR}
