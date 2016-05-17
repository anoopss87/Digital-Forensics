#include <jni.h>
#include <stdio.h>
#include <string.h>
#include "MBRJNI.h"

extern int loadMBRArray(char** data, char* disk);
//extern char** diskInfo();
char mbrData[5][200];

/*
 * Class:     MBRjni
 * Method:    loadMBR
 * Signature: ()V
 */
JNIEXPORT jboolean JNICALL Java_MBRJNI_loadMBR (JNIEnv *env, jobject jObj, jstring jStr) {
	strcpy(mbrData[0], "4");
	strcpy(mbrData[1], "0 1 2 3 4 5 6 7 8 9 10 11");
	strcpy(mbrData[2], "100 101 102 103 104 105 106 107 108 109 110 111");
	strcpy(mbrData[3], "200 201 202 203 204 205 206 207 208 209 210 211");
	strcpy(mbrData[4], "300 301 302 303 304 305 306 307 308 309 310 311");
	char *disk;
	disk = (*env)->GetStringUTFChars(env, jStr, NULL);
	if(loadMBRArray(mbrData, disk) == 0)
		return 1;
	else
		return 0;
}

/*
 * Class:     MBRjni
 * Method:    getPartitionInfo
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_MBRJNI_getPartitionInfo (JNIEnv *env, jobject jObj, jint jInt) {
	int i = jInt;
	char outString[128] = "";
	if(i>=4 && i<0) {
		return (*env)->NewStringUTF(env, outString);
	}
	return (*env)->NewStringUTF(env, mbrData[i]);
}
