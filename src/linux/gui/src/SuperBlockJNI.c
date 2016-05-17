#include <jni.h>
#include <stdio.h>
#include <string.h>
#include "SuperBlockJNI.h"

extern int init(char *disk);
extern long getPartitionSize();
extern long getInodeTableSize();
extern long getReservedGDTBlocks();
extern int getSingleBlockSize();
extern int getBlocksPerGroup();

JNIEXPORT jboolean JNICALL Java_SuperBlockJNI_init (JNIEnv *env, jobject jObj, jstring jStr) {
	int retVal = 0,i = 0;
	char *disk;
	disk = (*env)->GetStringUTFChars(env, jStr, NULL);
	// bgdGetNumberofBlockGroups(disk);
	printf("%s\n", disk);
	retVal = init(disk);
	return retVal;
}

JNIEXPORT jint JNICALL Java_SuperBlockJNI_getNumberofBlocks(JNIEnv *env, jobject jObj) {
//	char* info = bgdGetBlockGroupInfo(jInt);
	return getBlocksPerGroup();
}

/*
 * Class:     SuperBlockJNI
 * Method:    getPartitionSize
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_SuperBlockJNI_getPartitionSize(JNIEnv *env, jobject jObj) {
	return getPartitionSize();
}

/*
 * Class:     SuperBlockJNI
 * Method:    getInodeTableSize
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_SuperBlockJNI_getInodeTableSize (JNIEnv *env, jobject jObj) {
	return getInodeTableSize();
}

/*
 * Class:     SuperBlockJNI
 * Method:    getReservedGDTBlocks
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_SuperBlockJNI_getReservedGDTBlocks (JNIEnv *env, jobject jObj) {
	return getReservedGDTBlocks();
}

/*
 * Class:     SuperBlockJNI
 * Method:    getBlockSize
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_SuperBlockJNI_getBlockSize (JNIEnv *env, jobject jObj) {
	return getSingleBlockSize();
}
