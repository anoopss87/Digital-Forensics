#include the make.h
include ./make.h

MODULE_LIBRARY_LIST	= $(MODULE_EXE_DIR)/libshared.so
MODULE_MANIFEST_FILE	= $(MODULE_DIR)/manifest.txt
MODULE_IMAGES_DIR	= $(MODULE_JAVA_DIR)/images
MODULE_FINAL_EXE	= $(MODULE_EXE_DIR)/Forensics.jar

ifneq "$(JAVA_HOME)" ""
all: $(MODULE_FINAL_EXE)
else
all:
	@echo 'Error: JAVA not found'
	@echo 'JAVA_HOME needs to be set before compiling GUI'
endif

$(MODULE_FINAL_EXE): $(MODULE_LIBRARY_LIST)
	@echo $(JAVA_HOME)
	jar cvfm $(MODULE_FINAL_EXE) $(MODULE_MANIFEST_FILE) -C $(MODULE_OBJ_DIR) .

$(MODULE_EXE_DIR)/libshared.so: $(MODULE_JNI_DIR)/MBRJNI.h $(MODULE_JNI_DIR)/GroupBlockJNI.h $(MODULE_JNI_DIR)/SuperBlockJNI.h $(MODULE_JNI_DIR)/mbr.o $(MODULE_JNI_DIR)/MBRjni.o $(MODULE_JNI_DIR)/gbd.o $(MODULE_JNI_DIR)/GroupBlockJNI.o $(MODULE_JNI_DIR)/superblock.o $(MODULE_JNI_DIR)/SuperBlockJNI.o
	$(CC) -shared -fPIC -Wl,-soname,libshared.so.1 -o $@ $(MODULE_JNI_DIR)/mbr.o $(MODULE_JNI_DIR)/MBRjni.o $(MODULE_JNI_DIR)/gbd.o $(MODULE_JNI_DIR)/GroupBlockJNI.o $(MODULE_JNI_DIR)/superblock.o $(MODULE_JNI_DIR)/SuperBlockJNI.o -lc

$(MODULE_JNI_DIR)/mbr.o : $(MODULE_SRC_DIR)/mbr.c
	$(CC) -c -fPIC $(MODULE_FINAL_INC_DIRS) -DGRAPHICAL -o $@ $(MODULE_SRC_DIR)/mbr.c

$(MODULE_JNI_DIR)/gbd.o : $(MODULE_SRC_DIR)/Group_Desc.c
	gcc -c -fPIC $(MODULE_FINAL_INC_DIRS) -o $@ $(MODULE_SRC_DIR)/Group_Desc.c

$(MODULE_JNI_DIR)/superblock.o : $(MODULE_SRC_DIR)/superblock.c
	gcc -c -fPIC $(MODULE_FINAL_INC_DIRS) -o $@ $(MODULE_SRC_DIR)/superblock.c

$(MODULE_JNI_DIR)/MBRjni.o : $(MODULE_SRC_DIR)/MBRjni.c
	$(CC) -I "$(JAVA_HOME)/include" -I"$(JAVA_HOME)/include/linux" -c -fPIC $(MODULE_FINAL_INC_DIRS) -o $@ $<

$(MODULE_JNI_DIR)/GroupBlockJNI.o : $(MODULE_SRC_DIR)/GroupBlockJNI.c
	$(CC) -I "$(JAVA_HOME)/include" -I"$(JAVA_HOME)/include/linux" -c -fPIC $(MODULE_FINAL_INC_DIRS) -o $@ $<

$(MODULE_JNI_DIR)/SuperBlockJNI.o : $(MODULE_SRC_DIR)/SuperBlockJNI.c
	$(CC) -I "$(JAVA_HOME)/include" -I"$(JAVA_HOME)/include/linux" -c -fPIC $(MODULE_FINAL_INC_DIRS) -o $@ $<

# $* matches the target filename without the extension
$(MODULE_JNI_DIR)/MBRJNI.h : $(MODULE_OBJ_DIR)/MBRJNI.class
	javah -classpath $(MODULE_OBJ_DIR) -o $@ MBRJNI

$(MODULE_JNI_DIR)/GroupBlockJNI.h : $(MODULE_OBJ_DIR)/GroupBlockJNI.class
	javah -classpath $(MODULE_OBJ_DIR) -o $@ GroupBlockJNI

$(MODULE_JNI_DIR)/SuperBlockJNI.h : $(MODULE_OBJ_DIR)/SuperBlockJNI.class
	javah -classpath $(MODULE_OBJ_DIR) -o $@ SuperBlockJNI

$(MODULE_OBJ_DIR)/MBRJNI.class : $(MODULE_JAVA_DIR)/MBRJNI.java
	javac -sourcepath $(MODULE_JAVA_DIR) $(MODULE_JAVA_DIR)/*.java -d $(MODULE_OBJ_DIR)

$(MODULE_OBJ_DIR)/SuperBlockJNI.class : $(MODULE_JAVA_DIR)/SuperBlockJNI.java
	javac -sourcepath $(MODULE_JAVA_DIR) $(MODULE_JAVA_DIR)/*.java -d $(MODULE_OBJ_DIR)

clean:
	rm -f $(MODULE_OBJ_DIR)/*
	rm -f $(MODULE_EXE_DIR)/*
	rm -f $(MODULE_JNI_DIR)/*
