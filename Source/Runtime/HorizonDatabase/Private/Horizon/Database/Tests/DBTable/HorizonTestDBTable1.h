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

#pragma once
#include "Engine.h"
#include <vector>
#include <soci/soci.h>
#include "Horizon/Database/HorizonDatabase.h"
#include "HorizonTestDBTable1.generated.h"



USTRUCT(BlueprintType)
struct HORIZONDATABASE_API FHorizonTestDBTable1 : public FHorizonDatabaseTable
{
	GENERATED_BODY()

public:
	//TODO: how to add columnConstrains syntax at runtime? meta data only supported at editor mode
	//candidate impl: Id_primary_key?(I didn't prefer this method)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (HorizonDBColumnConstraints = "primary key"))
	int Id = 0; 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bTest1 = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString TestString;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TestFloat = 0.0f;



public://TODO: auto generate

	


	friend  bool operator==(const FHorizonTestDBTable1& lhs, const FHorizonTestDBTable1& rhs);
	friend  bool operator!=(const FHorizonTestDBTable1& lhs, const FHorizonTestDBTable1& rhs);
	friend  bool operator< (const FHorizonTestDBTable1& lhs, const FHorizonTestDBTable1& rhs);
	friend  bool operator> (const FHorizonTestDBTable1& lhs, const FHorizonTestDBTable1& rhs);
	friend  bool operator<=(const FHorizonTestDBTable1& lhs, const FHorizonTestDBTable1& rhs);
	friend  bool operator>=(const FHorizonTestDBTable1& lhs, const FHorizonTestDBTable1& rhs);
};


inline bool operator==(const FHorizonTestDBTable1& lhs, const FHorizonTestDBTable1& rhs)
{
	bool bResult = false;
	if (lhs.Id == rhs.Id && 
		lhs.bTest1 == rhs.bTest1 && 
		lhs.TestFloat == rhs.TestFloat &&
		lhs.TestString == rhs.TestString) 
	{
		bResult = true;
	}
	return bResult;
}
inline bool operator!=(const FHorizonTestDBTable1& lhs, const FHorizonTestDBTable1& rhs) { return !operator==(lhs, rhs); }
inline bool operator< (const FHorizonTestDBTable1& lhs, const FHorizonTestDBTable1& rhs)
{
	bool bResult = false;
	if (lhs.Id < rhs.Id) {bResult = true;}
	return bResult;
}
inline bool operator> (const FHorizonTestDBTable1& lhs, const FHorizonTestDBTable1& rhs) { return  operator< (rhs, lhs); }
inline bool operator<=(const FHorizonTestDBTable1& lhs, const FHorizonTestDBTable1& rhs) { return !operator> (lhs, rhs); }
inline bool operator>=(const FHorizonTestDBTable1& lhs, const FHorizonTestDBTable1& rhs) { return !operator< (lhs, rhs); }





//TODO: Implement Tool(HorizonDatabaseUE4GeneratedCode) that can auto generate following code


//=====================================Start HorizonDatabaseUE4GeneratedCode==========================

namespace soci
{
	template<>
	struct type_conversion<FHorizonTestDBTable1>
	{
		typedef values base_type;
		static void from_base(soci::values const & v, soci::indicator ind, FHorizonTestDBTable1 &p)
		{
			p.Id = v.get<int>("Id");
			p.bTest1 = (v.get<int>("bTest1") != 0) ? true : false;
			p.TestString = v.get<FString>("TestString");
			p.TestFloat = v.get<double>("TestFloat");
		}
		static void to_base(const FHorizonTestDBTable1 &p, soci::values & v, soci::indicator & ind)
		{
			v.set("Id", p.Id);
			v.set("bTest1", p.bTest1 ? 1 : 0);
			v.set("TestString", p.TestString);
			v.set("TestFloat", (double)p.TestFloat);
			ind = i_ok;
		}
	};

};//namespace soci


USTRUCT(BlueprintType)
struct HORIZONDATABASE_API FHorizonTestDBTable1BulkData {
public:
	GENERATED_BODY()
public:
	std::vector<int> IdList;
	std::vector<int> bTest1List;
	std::vector<std::string> TestStringList;
	std::vector<double> TestFloatList;
};

UCLASS()
class HORIZONDATABASE_API UHorizonTestDBTable1FunctionLibrary : public UBlueprintFunctionLibrary
{
private:
	GENERATED_BODY()
public://blueprint

	UFUNCTION(BlueprintCallable, Category = "HorizonTestDBTable1FunctionLibrary")
	static void CreateTable(AHorizonDatabase* pDB)
	{
		if (pDB) {
			//pDB->CreateTable(FHorizonTestDBTable1::StaticStruct());
			pDB->ExecuteSQL(GetCreateTableSqlStmt(pDB));
		}
	};



	UFUNCTION(BlueprintCallable, Category = "HorizonTestDBTable1FunctionLibrary")
	static void DeleteData(AHorizonDatabase* pDB, const FString& condition = "")
	{
		if (pDB) {
			pDB->DeleteData(FHorizonTestDBTable1::StaticStruct()->GetName(), condition);
		}
	};


	UFUNCTION(BlueprintCallable, Category = "HorizonTestDBTable1FunctionLibrary")
	static void TruncateTable(AHorizonDatabase* pDB)
	{
		if (pDB) {
			pDB->TruncateTable(FHorizonTestDBTable1::StaticStruct()->GetName());
		}
	};




	UFUNCTION(BlueprintCallable, Category = "HorizonTestDBTable1FunctionLibrary")
	static void DropTable(AHorizonDatabase* pDB)
	{
		if (pDB) {
			pDB->DropTable(FHorizonTestDBTable1::StaticStruct()->GetName());
		}
	};

	UFUNCTION(BlueprintCallable, Category = "HorizonTestDBTable1FunctionLibrary")
	static bool IsTableExists(AHorizonDatabase* pDB)
	{
		bool bResult = false;
		if (pDB) {
			bResult = pDB->IsTableExists(FHorizonTestDBTable1::StaticStruct()->GetName());
		}

		return bResult;
	};

	UFUNCTION(BlueprintCallable, Category = "HorizonTestDBTable1FunctionLibrary")
	static void UpdateData(AHorizonDatabase* pDB, const FString& updateParam, const FString&  condition = "")
	{
		if (pDB) {
			pDB->UpdateData(FHorizonTestDBTable1::StaticStruct()->GetName(), updateParam, condition);
		}
	};



	UFUNCTION(BlueprintCallable, Category = "HorizonTestDBTable1FunctionLibrary")
	static int GetTableRowCount(AHorizonDatabase* pDB, const FString& condition)
	{
		int result = 0;
		if (pDB) {
			result = pDB->GetTableRowCount(FHorizonTestDBTable1::StaticStruct()->GetName(), condition);
		}
		return result;
	};









	UFUNCTION(BlueprintCallable, Category = "HorizonTestDBTable1FunctionLibrary")
	static void AddBulkData(UPARAM(ref) FHorizonTestDBTable1BulkData& bulkData, const FHorizonTestDBTable1& data) 
	{
		bulkData.IdList.push_back(data.Id);
		bulkData.bTest1List.push_back(data.bTest1 ? 1 : 0);
		bulkData.TestStringList.push_back(TCHAR_TO_UTF8(*data.TestString));
		bulkData.TestFloatList.push_back(data.TestFloat);
	};
	
	

	UFUNCTION(BlueprintCallable, Category = "HorizonTestDBTable1FunctionLibrary")
	static void InsertBulkData(AHorizonDatabase* pDB, const FHorizonTestDBTable1BulkData& bulkData, bool bReplace = true) 
	{
		if (pDB) {
			UHorizonTestDBTable1FunctionLibrary::BulkExec(pDB, pDB->GetInsertBindingSqlStmt(FHorizonTestDBTable1::StaticStruct(), bReplace), bulkData);
		}
	};


	
	UFUNCTION(BlueprintCallable, Category = "HorizonTestDBTable1FunctionLibrary")
	static void UpdateBulkData(AHorizonDatabase* pDB, const FHorizonTestDBTable1BulkData& bulkData)
	{
		if (pDB) {
			UHorizonTestDBTable1FunctionLibrary::BulkExec(pDB, pDB->GetUpdateBindingSqlStmt(FHorizonTestDBTable1::StaticStruct()), bulkData);
		}
	};
	UFUNCTION(BlueprintCallable, Category = "HorizonTestDBTable1FunctionLibrary")
	static FHorizonTestDBTable1 QueryData(AHorizonDatabase* pDB, const FString& condition = "")
	{

		FHorizonTestDBTable1 outData;
		try {
			pDB->QueryData(FHorizonTestDBTable1::StaticStruct()->GetName(), outData, condition);
		}
		catch (const std::exception& e) {
			throw e;
		}

		return outData;
	};

	UFUNCTION(BlueprintCallable, Category = "HorizonTestDBTable1FunctionLibrary")
	static TArray<FHorizonTestDBTable1> QueryMultiData(AHorizonDatabase* pDB, const FString& condition = "")
	{
		TArray<FHorizonTestDBTable1> result;

		try {
			auto rowset = pDB->QueryMultiData<FHorizonTestDBTable1>(FHorizonTestDBTable1::StaticStruct()->GetName(), condition);

			for (auto& it : rowset) {
				result.Add(it);
			}
		}
		catch (const std::exception& e) {
			throw e;
		}
	
		return result;
	};

public://C++

	static FString GetCreateTableSqlStmt(AHorizonDatabase* pDB)
	{
		//pDB->GetCreateTableSqlStmt(pDB, FHorizonTestDBTable1::StaticStruct());
		return "CREATE TABLE IF NOT EXISTS HorizonTestDBTable1(Id int32 primary key, bTest1 bool, TestString FString, TestFloat float)";
	};



	static void BulkExec(AHorizonDatabase* pDB, const FString& sqlString, const FHorizonTestDBTable1BulkData& bulkData)
	{
		soci::transaction tr(pDB->GetSession());
		try {
			pDB->GetSession() << TCHAR_TO_UTF8(*sqlString), soci::use(bulkData.IdList), soci::use(bulkData.bTest1List)
				, soci::use(bulkData.TestStringList), soci::use(bulkData.TestFloatList);
		}
		catch (const std::exception& e) {
			throw e;
		}

		tr.commit();
	};


};




//=====================================End HorizonDatabaseUE4GeneratedCode==========================