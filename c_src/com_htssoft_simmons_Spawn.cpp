/*
 * com_htssoft_simmons_Spawn.c
 *
 *  Created on: Apr 4, 2012
 *      Author: aubrey
 */

#include "com_htssoft_simmons_Spawn.h"

#ifdef linux
#ifndef LINUX
#define LINUX
#endif
#endif


#ifndef JNIEXPORT //just to get eclipse's crappy indexer to stop bitching.
#define JNIEXPORT
#define JNICALL
#endif

#include <stdlib.h>
#include <string.h>

void JNU_ThrowByName(JNIEnv *env, const char *name, const char *msg) {
     jclass cls = env->FindClass(name);
     /* if cls is NULL, an exception has already been thrown */
     if (cls != NULL) {
         env->ThrowNew(cls, msg);
     }
     /* free the local ref */
     env->DeleteLocalRef(cls);
}


#ifdef LINUX
#include <unistd.h>
#include <errno.h>

void doExec(char *exePath, char **argv){
	setsid();
	execv(exePath, argv);
	exit(1); //if we get here, there was an issue. Just exit.
}

JNIEXPORT void JNICALL Java_com_htssoft_simmons_Spawn_backgroundSpawn(JNIEnv *env,
		jclass clz,
		jstring executablePath,
		jobjectArray arguments){

	jsize programStringLength = env->GetStringUTFLength(executablePath);
	const char *programStringUTF = env->GetStringUTFChars(executablePath, NULL);

	char *programString = (char*) malloc(sizeof(char) * (programStringLength + 1));
	memcpy(programString, programStringUTF, programStringLength + 1);

	env->ReleaseStringUTFChars(executablePath, programStringUTF);

	jsize argsLength = env->GetArrayLength(arguments);
	char **argV = (char**) malloc(sizeof(char *) * (argsLength + 1));

	for (int i = 0; i < argsLength; i++){
		jstring singleArgument = (jstring) env->GetObjectArrayElement(arguments, i);
		jsize singleArgLength = env->GetStringUTFLength(singleArgument);
		const char *argumentStringUTF = env->GetStringUTFChars(singleArgument, NULL);

		argV[i] = (char *) malloc(sizeof(char) * (singleArgLength + 1));
		memcpy(argV[i], argumentStringUTF, singleArgLength);
		argV[i][singleArgLength] = '\0';

		env->ReleaseStringUTFChars(singleArgument, argumentStringUTF);
	}

	argV[argsLength] = NULL;

	int forkResult = fork();

	if (!forkResult){ //this is the child process.
		doExec(programString, argV);
	}

	if (forkResult < 0){ //we're in the parent, but the fork failed for some reason.
		int syserr = errno;
		JNU_ThrowByName(env, "java/lang/Exception", strerror(syserr));
		return;
	}

	for (int i = 0; i < argsLength; i++){
		free(argV[i]);
	}

	free(programString);
}

JNIEXPORT void JNICALL Java_com_htssoft_simmons_Spawn_backgroundWindowsSpawn (JNIEnv *env, jclass clz,
			jstring executablePath, jstring commandLine){
	JNU_ThrowByName(env, "java/lang/UnsupportedOperationException", "Cannot execute Windows spawn on unix platform.");
	return;
}

#elif defined _WIN32

#include <Windows.h>

JNIEXPORT void JNICALL Java_com_htssoft_simmons_Spawn_backgroundSpawn(JNIEnv *env,
		jclass clz,
		jstring executablePath,
		jobjectArray arguments){
	JNU_ThrowByName(env, "java/lang/UnsupportedOperationException", "Cannot execute unix spawn on Windows platform.");
	return;
}

JNIEXPORT void JNICALL Java_com_htssoft_simmons_Spawn_backgroundWindowsSpawn (JNIEnv *env, jclass clz,
			jstring executablePath, jstring commandLine){

	jsize programStringLength = env->GetStringUTFLength(executablePath);
	const char *programStringUTF = env->GetStringUTFChars(executablePath, NULL);

	char *programString = (char*) malloc(sizeof(char) * (programStringLength + 1));
	memcpy(programString, programStringUTF, programStringLength);
	programString[programStringLength] = '\0';

	env->ReleaseStringUTFChars(executablePath, programStringUTF);

	jsize commandLineLength = env->GetStringUTFLength(commandLine);
	const char *commandLineUTF = env->GetStringUTFChars(commandLine, NULL);

	char *commandLineString = (char *) malloc(sizeof(char) * (commandLineLength + 1));
	memcpy(commandLineString, commandLineUTF, commandLineLength);
	commandLineString[commandLineLength] = '\0';

	env->ReleaseStringUTFChars(commandLine, commandLineUTF);

	STARTUPINFO *startupInfo = (STARTUPINFO*) malloc(sizeof(STARTUPINFO));
	PROCESS_INFORMATION *procInfo = (PROCESS_INFORMATION*) malloc(sizeof(PROCESS_INFORMATION));
	memset(startupInfo, 0, sizeof(STARTUPINFO));
	memset(procInfo, 0, sizeof(PROCESS_INFORMATION));

	DWORD cp_res = CreateProcess(programString,
				  	  	  	   commandLineString,
				  	  	  	   NULL,
				  	  	  	   NULL,
				  	  	  	   FALSE,
				  	  	  	   CREATE_DEFAULT_ERROR_MODE,
				  	  	  	   NULL,
				  	  	  	   NULL,
				  	  	  	   startupInfo,
				  	  	  	   procInfo);
	if (!cp_res){

		DWORD errorCode = GetLastError();
		LPVOID lpMsgBuf;

	    FormatMessage(
	        FORMAT_MESSAGE_ALLOCATE_BUFFER |
	        FORMAT_MESSAGE_FROM_SYSTEM |
	        FORMAT_MESSAGE_IGNORE_INSERTS,
	        NULL,
	        errorCode,
	        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
	        (LPTSTR) &lpMsgBuf,
	        0, NULL );

		JNU_ThrowByName(env, "java/lang/Exception", (LPTSTR) lpMsgBuf);
		LocalFree(lpMsgBuf);
		return;
	}
}


#endif


