#include <jni.h>
#include <stdio.h>
#include <string.h>
#include "GroupBlockJNI.h"

extern int bgdGetNumberofBlockGroups(char* disk);
extern int bgdInit(char* drivename);
extern char *bgdGetBlockGroupInfo(int blockGroupNum);
extern char *bgdReadBlockBitmap(char driveName[], int blockGroupNo);

JNIEXPORT jint JNICALL Java_GroupBlockJNI_getTotalBlockGroups (JNIEnv *env, jobject jObj, jstring jStr) {
	int i = 0;
	char *disk;
	disk = (*env)->GetStringUTFChars(env, jStr, NULL);
	return bgdGetNumberofBlockGroups(disk);
}

JNIEXPORT jint JNICALL Java_GroupBlockJNI_init(JNIEnv *env, jobject jObj, jstring jStr) {
	char *disk;
	disk = (*env)->GetStringUTFChars(env, jStr, NULL);
	return bgdInit(disk);
}

JNIEXPORT jstring JNICALL Java_GroupBlockJNI_getBlockGroupInfo(JNIEnv *env, jobject jObj, jint jInt) {
	char* info = bgdGetBlockGroupInfo(jInt);
	return (*env)->NewStringUTF(env, info);
}

JNIEXPORT jstring JNICALL Java_GroupBlockJNI_getBlockInfo(JNIEnv *env, jobject jObj, jstring jStr, jint jInt) {
	char *disk;
	disk = (*env)->GetStringUTFChars(env, jStr, NULL);
	char* info = bgdReadBlockBitmap(disk, jInt);
	return (*env)->NewStringUTF(env, info);
}
