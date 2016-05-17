include ../make.h
MODULE = gui

MODULE_DIR = ${BASE_DIR}/${MODULE}

#JNI Include files directory
MODULE_INC_DIR = ${MODULE_DIR}/inc

#JNI Source files directory
MODULE_SRC_DIR = ${MODULE_DIR}/src

#Java Class object files directory
MODULE_OBJ_DIR = ${MODULE_DIR}/obj

#Final Library files directory
MODULE_EXE_DIR = ${MODULE_DIR}/exe

#Compiled JNI files directory
MODULE_JNI_DIR = ${MODULE_DIR}/jni

#Java files directory
MODULE_JAVA_DIR = ${MODULE_DIR}/java

#Other include directiories
COMMON_INC_DIR = ${BASE_DIR}/common/inc

MODULE_FINAL_INC_DIRS = -I${MODULE_INC_DIR} \
			-I${MODULE_JNI_DIR} \
			-I${COMMON_INC_DIR}
