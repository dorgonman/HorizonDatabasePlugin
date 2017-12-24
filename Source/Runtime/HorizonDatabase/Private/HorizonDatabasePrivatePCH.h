// Created by dorgon, All Rights Reserved.
// email: dorgonman@hotmail.com
// blog: dorgon.horizon-studio.net

#pragma once



/* Public dependencies
*****************************************************************************/

#include "Engine.h"



/* Public includes
*****************************************************************************/





/* Log define
*****************************************************************************/

//General Log
//CORE_API DECLARE_LOG_CATEGORY_EXTERN(LogTemp, Log, All);
//HORIZONLIBRARY_API 
DECLARE_LOG_CATEGORY_EXTERN(HorizonDatabase, Log, All);
#define UE_HORIZONDB_FATAL(msg, ...) UE_LOG(HorizonDatabase, Fatal, TEXT(msg), ##__VA_ARGS__)
#define UE_HORIZONDB_ERROR(msg, ...) UE_LOG(HorizonDatabase, Error, TEXT(msg), ##__VA_ARGS__)
#define UE_HORIZONDB_WARNING(msg, ...) UE_LOG(HorizonDatabase, Warning, TEXT(msg), ##__VA_ARGS__)
#define UE_HORIZONDB_DISPLAY(msg, ...) UE_LOG(HorizonDatabase, Display, TEXT(msg), ##__VA_ARGS__)
#define UE_HORIZONDB_LOG(msg, ...) UE_LOG(HorizonDatabase, Log, TEXT(msg), ##__VA_ARGS__)
#define UE_HORIZONDB_VERBOSE(msg, ...) UE_LOG(HorizonDatabase, Verbose, TEXT(msg), ##__VA_ARGS__)
#define UE_HORIZONDB_VERY_VERBOSE(msg, ...) UE_LOG(HorizonDatabase, VeryVerbose, TEXT(msg), ##__VA_ARGS__)


#include "HorizonDatabaseModule.h"