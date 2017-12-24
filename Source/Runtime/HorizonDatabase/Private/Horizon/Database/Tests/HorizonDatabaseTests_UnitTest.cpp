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



#include "HorizonDatabasePrivatePCH.h"
//#include "UnrealEd.h"
//#include "AutomationEditorCommon.h"
#include "AutomationTest.h"
#include "Horizon/Database/HorizonDatabase.h"
#include "Horizon/Database/Tests/DBTable/HorizonTestDBTable1.h"
#include "Horizon/Database/Tests/soci/common-tests.h"
#include <soci/soci.h>
//#include <soci/sqlite3/soci-sqlite3.h>
//#include <string>
//#include "Runtime/Projects/Public/Interfaces/IPluginManager.h"
#if WITH_DEV_AUTOMATION_TESTS

namespace
{
	UWorld* GetSimpleEngineAutomationTestGameWorld(const int32 TestFlags)
	{
		// Accessing the game world is only valid for game-only 
		//check((TestFlags & EAutomationTestFlags::ApplicationContextMask) == EAutomationTestFlags::ClientContext);
		check(GEngine->GetWorldContexts().Num() == 1);
		//check(GEngine->GetWorldContexts()[0].WorldType == EWorldType::Game);

		return GEngine->GetWorldContexts()[0].World();
	}

}



static FString GConnectString = "Horizon/test1/MyTest.db";
static EHorizonDatabaseBackEnd::Type GBackendType = EHorizonDatabaseBackEnd::Sqlite3;

static void RunTestHorizonDBImplement(FAutomationTestBase* pTestCase,
	const TFunction<void(FAutomationTestBase*, AHorizonDatabase*)>& testImplFunc)
{
	bool bResult = true;

	//virtual uint32 GetTestFlags() const override { return EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter; }
	UWorld* CurrentWorld = GetSimpleEngineAutomationTestGameWorld(pTestCase->GetTestFlags());
		
	//GEngine->CreateNewWorldContext(EWorldType::Game).World();// GEditor->GetEditorWorldContext().World();
	//FAutomationEditorCommonUtils::CreateNewMap();   //only after 4.14
	
	pTestCase->TestNotNull("Failed to get CurrentWorld", CurrentWorld);
	if (nullptr != CurrentWorld) {
		auto pDB = CurrentWorld->SpawnActor<AHorizonDatabase>();
		pTestCase->TestNotNull("Failed to spawan AHorizonDatabase", pDB);
		if (pDB) {
			pDB->BackEndType = GBackendType;
			pDB->ConnectString = GConnectString;

			try {
				pDB->Open();
				testImplFunc(pTestCase, pDB);
			}
			catch (const std::exception& e)
			{
				pTestCase->AddError(FString::Printf(TEXT("catch exception, reason: %s"), *FString(e.what())));
			}
	
			pDB->K2_DestroyActor();
			pDB = nullptr;
		}
	}

	
}

static void SimpleTestImplement(FAutomationTestBase* pTestCase, AHorizonDatabase* pDB) 
{
	pDB->ExecuteSQL("drop table if exists TestCase1");
	pDB->ExecuteSQL("create table TestCase1("
		"id integer,"
		"name varchar(100))");
	pDB->ExecuteSQL("insert into TestCase1(id, name) values(7, \'John\')");
	// fundamental type
	{
		int myId = 0;
		std::string myName;
		pDB->QueryData("TestCase1", "id", myId, "");

		pTestCase->TestEqual(TEXT("myId should equal to 7"), myId, 7);
		pDB->QueryData("TestCase1", "name", myName, "");
		pTestCase->TestEqual(TEXT("myName should equal to John"), myName, std::string("John"));
	}



	//std::tuple
	{
		enum { id, name };
		std::tuple<int, std::string> dataTuple;

		pDB->QueryData("TestCase1", "id, name", dataTuple, "");
		pTestCase->TestEqual(TEXT("id should equal to 7"), std::get<id>(dataTuple), 7);
		pTestCase->TestEqual(TEXT("name should equal to John"), std::get<name>(dataTuple), std::string("John"));
	}

	{
		enum { id, name };
		std::tuple<int, FString> dataTuple;

		pDB->QueryData("TestCase1", "id, name", dataTuple, "");
		pTestCase->TestEqual(TEXT("id should equal to 7"), std::get<id>(dataTuple), 7);
		pTestCase->TestEqual(TEXT("name should equal to John"), std::get<name>(dataTuple), FString("John"));
	}

	pDB->ExecuteSQL("drop table if exists TestCase1");
}


static void ORMTestImplement(FAutomationTestBase* pTestCase, AHorizonDatabase* pDB) 
{
	pTestCase->AddLogItem("ORMTestImplement start");
	auto pStruct = FHorizonTestDBTable1::StaticStruct();


	//============================create database==============================
	pDB->DropTable(FHorizonTestDBTable1::StaticStruct()->GetName());
	UHorizonTestDBTable1FunctionLibrary::CreateTable(pDB);
	pTestCase->AddLogItem("ORMTestImplement CreateTable");
	//============================test data===================================
	FHorizonTestDBTable1BulkData bulkData;
	FHorizonTestDBTable1 a0;
	a0.Id = 0;
	a0.TestString = "test a0";
	a0.TestFloat = 1.333f;
	a0.bTest1 = true;
	FHorizonTestDBTable1 a1;
	a1.Id = 1;
	a1.TestString = "test a1";
	a1.TestFloat = 2.3333f;
	a1.bTest1 = false;
	UHorizonTestDBTable1FunctionLibrary::AddBulkData(bulkData, a0);
	UHorizonTestDBTable1FunctionLibrary::AddBulkData(bulkData, a1);
	//auto insertSQL = AHorizonDatabase::GetInsertSQLUseStmt(pStruct, false);

	pTestCase->AddLogItem("ORMTestImplement2 start insert row");
	//============================insert row==================================

	try {
		UHorizonTestDBTable1FunctionLibrary::InsertBulkData(pDB, bulkData);
	}
	catch (std::exception& e) {
		pTestCase->AddError(FString::Printf(TEXT("insertSQL exception: %s"), *FString(e.what())));
	}
	pTestCase->AddLogItem("end insert row");
	//============================select single row==================================
	{
		auto data0 = UHorizonTestDBTable1FunctionLibrary::QueryData(pDB, "WHERE Id = 0");
		pTestCase->TestEqual(TEXT("data0 == a0"), data0, a0);
		auto data1 = UHorizonTestDBTable1FunctionLibrary::QueryData(pDB, "WHERE Id = 1");
		pTestCase->TestEqual(TEXT("data1 == a1"), data1, a1);
	}
	//=======================================================================
	pTestCase->AddLogItem("end select single row");

	//============================select multi row==================================
	{
		auto rowSet = UHorizonTestDBTable1FunctionLibrary::QueryMultiData(pDB);
		pTestCase->TestEqual(TEXT("rowSet.Num() == 2"), rowSet.Num(), 2);
		pTestCase->TestEqual(TEXT("rowSet[0] == a0"), rowSet[0], a0);
		pTestCase->TestEqual(TEXT("rowSet[1] == a1"), rowSet[1], a1);
	}


	pTestCase->AddLogItem("end select multi row");
	//============================select single tuple: C++ only=======================================

	{
		enum { Id, TestString };
		std::tuple<int, FString> dataTuple;

		pDB->QueryData(FHorizonTestDBTable1::StaticStruct()->GetName(), "Id, TestString", dataTuple, "WHERE Id = 0");
		pTestCase->TestEqual(TEXT("Id should equal to a0.Id"), std::get<Id>(dataTuple), a0.Id);
		pTestCase->TestEqual(TEXT("TestString should equal to a0.TestString"), std::get<TestString>(dataTuple), a0.TestString);
	}

	pTestCase->AddLogItem("end select single tuple");
	//============================select multi tuple: C++ only=======================================

	{
		enum { Id, TestString };
		std::tuple<int, FString> dataTuple;

		auto rowSet = pDB->QueryMultiData<decltype(dataTuple)>(FHorizonTestDBTable1::StaticStruct()->GetName(), "Id, TestString", "");
		auto rowIt = rowSet.begin();
		pTestCase->TestEqual(TEXT("Id should equal to a0.Id"), std::get<Id>(*rowIt), a0.Id);
		pTestCase->TestEqual(TEXT("TestString should equal to a0.TestString"), std::get<TestString>(*rowIt), a0.TestString);
		rowIt++;
		pTestCase->TestEqual(TEXT("Id should equal to a1.Id"), std::get<Id>(*rowIt), a1.Id);
		pTestCase->TestEqual(TEXT("TestString should equal to a1.TestString"), std::get<TestString>(*rowIt), a1.TestString);

	}

	pTestCase->AddLogItem("end select multi tuple");
	//=============================UpdateData===============================================

	{
		UHorizonTestDBTable1FunctionLibrary::UpdateData(pDB, "TestString='abc', TestFloat=4.3333", "WHERE Id = 0");
		auto data0 = UHorizonTestDBTable1FunctionLibrary::QueryData(pDB, "WHERE Id = 0");
		pTestCase->TestNotEqual(TEXT("data0 != a0"), data0, a0);
	}

	pTestCase->AddLogItem("end UpdateData");
	//=================================DeleteData======================================

	{
		UHorizonTestDBTable1FunctionLibrary::DeleteData(pDB, "WHERE Id = 0");
		auto rowSet = UHorizonTestDBTable1FunctionLibrary::QueryMultiData(pDB);
		pTestCase->TestEqual(TEXT("rowSet.Num() == 1"), rowSet.Num(), 1);
	}
	{
		UHorizonTestDBTable1FunctionLibrary::DeleteData(pDB, "WHERE Id = 1");
		auto rowSet = UHorizonTestDBTable1FunctionLibrary::QueryMultiData(pDB);
		pTestCase->TestEqual(TEXT("rowSet.Num() == 0"), rowSet.Num(), 0);
	}

	pTestCase->AddLogItem("ORMTestImplement2 logitem");
	{
		try
		{
			{ //insert test data
				UHorizonTestDBTable1FunctionLibrary::InsertBulkData(pDB, bulkData);
				auto rowSet = UHorizonTestDBTable1FunctionLibrary::QueryMultiData(pDB);
				pTestCase->TestEqual(TEXT("rowSet.Num() == 2"), rowSet.Num(), 2);
			}
			pDB->DeleteData(FHorizonTestDBTable1::StaticStruct()->GetName(), "");
			auto rowSet = UHorizonTestDBTable1FunctionLibrary::QueryMultiData(pDB);
			pTestCase->TestEqual(TEXT("rowSet.Num() == 0"), rowSet.Num(), 0);
		}
		catch (std::exception& e) {
			pTestCase->AddError(FString::Printf(TEXT("insertSQL exception: %s"), *FString(e.what())));
		}
	}
	pTestCase->AddLogItem("end DeleteData");
	//=================================TruncateTable======================================
	{
		{ //insert test data
			UHorizonTestDBTable1FunctionLibrary::InsertBulkData(pDB, bulkData);
			auto rowSet = UHorizonTestDBTable1FunctionLibrary::QueryMultiData(pDB);
			pTestCase->TestEqual(TEXT("rowSet.Num() == 2"), rowSet.Num(), 2);
		}
		pDB->TruncateTable(FHorizonTestDBTable1::StaticStruct()->GetName());
		auto rowSet = UHorizonTestDBTable1FunctionLibrary::QueryMultiData(pDB);
		pTestCase->TestEqual(TEXT("rowSet.Num() == 0"), rowSet.Num(), 0);
	}

	pTestCase->AddLogItem("end TruncateTable");
	//================================DropTable============================================
	{
		try
		{
			pTestCase->TestEqual(TEXT("bTableExists == true"), pDB->IsTableExists(FHorizonTestDBTable1::StaticStruct()->GetName()), true);
			pDB->DropTable(FHorizonTestDBTable1::StaticStruct()->GetName());
			pTestCase->TestEqual(TEXT("bTableExists == false"), pDB->IsTableExists(FHorizonTestDBTable1::StaticStruct()->GetName()), false);
		}
		catch (std::exception& e) {
			pTestCase->AddError(FString::Printf(TEXT("drop table exception: %s"), *FString(e.what())));
		}
	}
	pTestCase->AddLogItem("end DropTable");
}



IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHorizonDatabaseSqlite3SimpleTest, "Horizon.Database.Sqlite3.UnitTest.SimpleTest", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FHorizonDatabaseSqlite3SimpleTest::RunTest(const FString& Parameters)
{
	GBackendType = EHorizonDatabaseBackEnd::Sqlite3;
	bool bResult = true;
	RunTestHorizonDBImplement(this, &SimpleTestImplement);

	if (ExecutionInfo.Errors.Num() > 0)
	{
		bResult = false;
	}

	return bResult;
}




IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHorizonDatabaseSqlite3ORMTest, "Horizon.Database.Sqlite3.UnitTest.ORMTest[CRUD]", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FHorizonDatabaseSqlite3ORMTest::RunTest(const FString& Parameters)
{
	GBackendType = EHorizonDatabaseBackEnd::Sqlite3;
	bool bResult = true;
	RunTestHorizonDBImplement(this, &ORMTestImplement);

	if (ExecutionInfo.Errors.Num() > 0)
	{
		bResult = false;
	}


	

	return bResult;
}




//
//
//IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHorizonDatabaseMySQLORMTest, "Horizon.Database.MySQL.UnitTest.ORMTest[CRUD]", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
//
//bool FHorizonDatabaseMySQLORMTest::RunTest(const FString& Parameters)
//{
//	GBackendType = EHorizonDatabaseBackEnd::MySQL;
//	bool bResult = true;
//	RunTestHorizonDBImplement(this, &ORMTestImplement);
//
//	if (ExecutionInfo.Errors.Num() > 0)
//	{
//		bResult = false;
//	}
//
//
//
//
//	return bResult;
//}


//pDB->DeleteData();


//for (TObjectIterator<UStruct> it; it; ++it)
//{
//	UStruct* myStruct = *it;
//	bool isDBTable = myStruct->IsChildOf(FHorizonDatabaseTable::StaticStruct());
//	if (isDBTable) {
//		auto pluginDir = FPaths::GamePluginsDir();
//		auto myPluginDir = IPluginManager::Get().FindPlugin(TEXT("HorizonDatabasePlugin"))->GetBaseDir();
//		auto relatePath = myStruct->GetMetaData("ModuleRelativePath");
//		auto fullPath = FPaths::ConvertRelativePathToFull(relatePath);
//		UE_HORIZONDBEDITOR_LOG("ModuleRelativePath: %s", *relatePath);
//		UE_HORIZONDBEDITOR_LOG("fullPath: %s", *fullPath);
//		UE_HORIZONDBEDITOR_LOG("IsChildOf DBTable: %s", *myStruct->GetName());
//		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

//		FString gameName = FApp::GetGameName();
//		UE_HORIZONDBEDITOR_LOG("FApp::GetGameName(): %s", *gameName);
//		auto test = PlatformFile.GetFilenameOnDisk(*relatePath);
//		//PlatformFile.CreateDirectoryTree();
//		UE_HORIZONDBEDITOR_LOG("test: %s", *test);

//		
//	}
//}


#endif //WITH_DEV_AUTOMATION_TESTS