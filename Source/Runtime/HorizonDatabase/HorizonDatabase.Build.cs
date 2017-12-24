/******************************************************
* Boost Software License - Version 1.0 - 2016/10/06
*
* Copyright (c) 2016 dorgon chang
* http://dorgon.horizon-studio.net/
*
* Permission is hereby granted, free of charge, to any person or organization
* obtaining a copy of the software and accompanying documentation covered by
* this license (the "Software") to use, reproduce, display, distribute,
* execute, and transmit the Software, and to prepare derivative works of the
* Software, and to permit third-parties to whom the Software is furnished to
* do so, all subject to the following:
*
* The copyright notices in the Software and this entire statement, including
* the above license grant, this restriction and the following disclaimer,
* must be included in all copies of the Software, in whole or in part, and
* all derivative works of the Software, unless such copies or derivative
* works are solely in the form of machine-executable object code generated by
* a source language processor.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
* SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
* FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
**********************************************************/

using System;
using System.IO;
using UnrealBuildTool;

public class HorizonDatabase : ModuleRules
{

   public HorizonDatabase(ReadOnlyTargetRules Target)
        : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicIncludePaths.AddRange(
			new string[] {
                "Runtime/HorizonDatabase/Public",
                ModuleLibSociPublicIncludePath,
                ModuleLibSqlite3PublicIncludePath,
                ModuleLibSociConfigIncludePath
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
                "Runtime/HorizonDatabase/Private",
				// ... add other private include paths required here ...
			}
			);

        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore", "UMG" }); //2D
        PrivateDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" }); //Core
        PublicDependencyModuleNames.AddRange(new string[] { "soci_core", "soci_empty", "soci_sqlite3" });

      


        DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);

        bUseRTTI = true; // turn on RTTI
    }

    private string ModuleRootPath
    {
        get
        {
            string ModuleCSFilename = UnrealBuildTool.RulesCompiler.GetFileNameFromType(GetType());
            string ModuleCSFileDirectory = Path.GetDirectoryName(ModuleCSFilename);
            return Path.Combine(ModuleCSFileDirectory);
        }
    }


    private string ModuleLibSoci
    {
        get
        {
            return Path.GetFullPath(Path.Combine(ModuleRootPath, "..", "..", "ThirdParty", "LibSoci"));
        }
    }

    private string ModuleLibSociConfigIncludePath
    {
        get
        {
            return Path.GetFullPath(Path.Combine(ModuleRootPath, "..", "..", "ThirdParty", "LibSoci", "Include"));
        }
    }

    private string ModuleLibSociPublicIncludePath
    {
        get
        {
            return Path.GetFullPath(Path.Combine(ModuleRootPath, ModuleLibSoci, "soci", "include"));
        }
    }




    private string ModuleLibSqlite3PublicIncludePath
    {
        get
        {
            return Path.GetFullPath(Path.Combine(ModuleRootPath, "..", "..", "ThirdParty", "LibSqlite3", "sqlite3", "src"));
        }
    }
}
