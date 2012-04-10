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

#ifdef LINUX

#include <unistd.h>

void doExec(char *exePath, char **argv){
	setsid();
	execv(exePath, argv);
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

	if (!fork()){
		doExec(programString, argV);
	}

	for (int i = 0; i < argsLength; i++){
		free(argV[i]);
	}

	free(programString);
}
#elif defined _WIN32

#include <Windows.h>

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

	CreateProcess(programString,
				  commandLineString,
				  NULL,
				  NULL,
				  FALSE,
				  CREATE_DEFAULT_ERROR_MODE,
				  NULL,
				  NULL,
				  startupInfo,
				  procInfo);
}


#endif


