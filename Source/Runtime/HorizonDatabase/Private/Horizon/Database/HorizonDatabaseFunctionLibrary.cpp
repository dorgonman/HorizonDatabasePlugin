// Created by dorgon, All Rights Reserved.
// email: dorgonman@hotmail.com
// blog: dorgon.horizon-studio.net

#include "HorizonDatabasePrivatePCH.h"

#include "Horizon/Database/HorizonDatabaseFunctionLibrary.h"



void UHorizonDatabaseFunctionLibrary::EnsureFileDirectory(const FString& filePath)
{
	int found = INDEX_NONE;
	filePath.FindLastChar('/', found);

	if (found != INDEX_NONE) {
		FString folderPath = filePath.Mid(0, found);

		CreateDirectoryRecursive(folderPath);
	}
	else {
		UE_HORIZONDB_WARNING("UHorizonDatabaseFunctionLibrary::EnsureFileDirectory(%s) can not FindLastChar('/') to extract folder from filePath", *filePath);
	}
}


void UHorizonDatabaseFunctionLibrary::CreateDirectoryRecursive(const FString& folderPath)
{

	if (!FPaths::DirectoryExists(folderPath))
	{
		FString left;
		FString right = folderPath / "/";
		do {
			int found = INDEX_NONE;
			right.FindChar('/', found);
			if (found != INDEX_NONE) {
				left /= right.Mid(0, found);
				if (!FPaths::DirectoryExists(right))
				{
					FPlatformFileManager::Get().GetPlatformFile().CreateDirectory(*left);
				}
				right = right.Mid(found + 1);
			}
			else {
				UE_HORIZONDB_WARNING("UHorizonDatabaseFunctionLibrary::CreateDirectoryRecursive(%s) can not find / to create folder %s/\"%s\"", *folderPath, *left, *right);
				break;
			}
		} while (right.Len() != 0);
	}

}



FString UHorizonDatabaseFunctionLibrary::GetBaseDirectory()
{
	FString baseDirectory;
#if (PLATFORM_ANDROID)
	extern FString GExternalFilePath;
	baseDirectory = GExternalFilePath;//androidPlatformFile.FileRootPath(TEXT("")) + FPaths::GamePersistentDownloadDir() 

									  //remove relate path because FPaths::ConvertRelativePathToFull didn't work on android
	while (baseDirectory.StartsWith(TEXT("../")))
	{
		baseDirectory = baseDirectory.RightChop(3);
	}
	if (baseDirectory.Equals(TEXT("..")))
	{
		baseDirectory = TEXT("");
	}
#else
	baseDirectory = FPaths::GamePersistentDownloadDir();
#endif
	FPaths::NormalizeDirectoryName(baseDirectory);
	if (FPaths::IsRelative(baseDirectory)) {
		baseDirectory = FPaths::ConvertRelativePathToFull(MoveTemp(baseDirectory));
	}
	return baseDirectory;

}