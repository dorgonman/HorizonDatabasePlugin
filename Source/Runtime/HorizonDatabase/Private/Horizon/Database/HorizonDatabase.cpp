// Created by dorgon, All Rights Reserved.
// email: dorgonman@hotmail.com
// blog: dorgon.horizon-studio.net
#include "HorizonDatabase.h"
#include "HorizonDatabasePrivatePCH.h"

#include "soci/soci.h"
#include "soci/empty/soci-empty.h"
#include "soci/sqlite3/soci-sqlite3.h"
#include "Horizon/Database/HorizonDatabaseFunctionLibrary.h"




AHorizonDatabase::AHorizonDatabase()
{


}


void AHorizonDatabase::BeginPlay()
{
	Super::BeginPlay();
	if (bAutoOpen) {Open();}


}

void AHorizonDatabase::EndPlay(const EEndPlayReason::Type EndPlayReason) 
{
	Super::EndPlay(EndPlayReason);
	Close();
}


bool AHorizonDatabase::Open()
{
	bool bResult = false;
	if (!SessionPtr.IsValid())
	{
		try {
			switch (BackEndType) {
			case EHorizonDatabaseBackEnd::Empty:
				SessionPtr = MakeShareable(new soci::session(soci::empty, TCHAR_TO_UTF8(*ConnectString)));
				break;
			case EHorizonDatabaseBackEnd::Sqlite3:
			{
				FString baseDirectory;
				FString dbFileFullPath;
				if (!IsMemoryDB()) {
					baseDirectory = UHorizonDatabaseFunctionLibrary::GetBaseDirectory();
					dbFileFullPath = baseDirectory / ConnectString;

					UHorizonDatabaseFunctionLibrary::EnsureFileDirectory(dbFileFullPath);

					if (FPaths::IsRelative(dbFileFullPath)) {
						dbFileFullPath = FPaths::ConvertRelativePathToFull(MoveTemp(dbFileFullPath));
					}
				}
				else {
					dbFileFullPath = ConnectString;
				}
				SessionPtr = MakeShareable(new soci::session(soci::sqlite3, TCHAR_TO_UTF8(*dbFileFullPath)));
			}
			break;
			default:
			{
				//auto dbType = GetHorizonDatabaseBackEndEnumAsString(BackEndType).ToString();
				ensureMsgf(false, TEXT("backend %d not support"), (int)BackEndType);
				SessionPtr = MakeShareable(new soci::session(soci::empty, TCHAR_TO_UTF8(*ConnectString)));
			}
			break;
			}

			bResult = true;
		}
		catch (const std::exception& e)
		{

			UE_HORIZONDB_ERROR("AHorizonDatabase::Open exception: %s", *FString(e.what()));
			throw e;
		}

	}//if (!SessionPtr.IsValid())

	//case EHorizonDatabaseBackEnd::DB2:
	//	//TODO: add soci_db2.Build.cs
	//	//SessionPtr = MakeShareable(new soci::session(soci::db2, TCHAR_TO_UTF8(*connectString)));
	//	SessionPtr = MakeShareable(new soci::session(soci::empty, TCHAR_TO_UTF8(*connectString)));
	//	break;
	//case EHorizonDatabaseBackEnd::Firebird:
	//	//TODO: add soci_firebird.Build.cs
	//	//SessionPtr = MakeShareable(new soci::session(soci::firebird, TCHAR_TO_UTF8(*connectString)));
	//	SessionPtr = MakeShareable(new soci::session(soci::empty, TCHAR_TO_UTF8(*connectString)));
	//	break;
	//case EHorizonDatabaseBackEnd::MySQL:
	//	//TODO: add soci_mysql.Build.cs
	//	//SessionPtr = MakeShareable(new soci::session(soci::mysql, TCHAR_TO_UTF8(*connectString)));
	//	SessionPtr = MakeShareable(new soci::session(soci::empty, TCHAR_TO_UTF8(*connectString)));
	//	break;
	//case EHorizonDatabaseBackEnd::ODBC:
	//	//TODO: add soci_odbc.Build.cs
	//	//SessionPtr = MakeShareable(new soci::session(soci::odbc, TCHAR_TO_UTF8(*connectString)));
	//	SessionPtr = MakeShareable(new soci::session(soci::empty, TCHAR_TO_UTF8(*connectString)));
	//	break;
	//case EHorizonDatabaseBackEnd::Oracle:
	//	//TODO: add soci_oracle.Build.cs
	//	//SessionPtr = MakeShareable(new soci::session(soci::oracle, TCHAR_TO_UTF8(*connectString)));
	//	SessionPtr = MakeShareable(new soci::session(soci::empty, TCHAR_TO_UTF8(*connectString)));
	//	break;
	//case EHorizonDatabaseBackEnd::PostgreSQL:
	//	//TODO: add soci_postgresql.Build.cs
	//	//SessionPtr = MakeShareable(new soci::session(soci::postgresql, TCHAR_TO_UTF8(*connectString)));
	//	SessionPtr = MakeShareable(new soci::session(soci::empty, TCHAR_TO_UTF8(*connectString)));
	//	break;






	return bResult;
}
bool AHorizonDatabase::IsMemoryDB() {
	bool bResult = false;

	switch (BackEndType) {
		case EHorizonDatabaseBackEnd::Sqlite3:
		{
			if (ConnectString.Equals(":memory:")) 
			{
				bResult = true;
			}
		}
		break;
		case EHorizonDatabaseBackEnd::Empty:
		default:
			bResult = true;
			break;
	}
	return bResult;
}


void AHorizonDatabase::Close()
{
	SessionPtr.Reset();
}





//===========================Begin Blueprint Interface================================

void AHorizonDatabase::CreateTable(UStruct* pSchema)
{
	auto sqlStmt = GetCreateTableSqlStmt(this, pSchema);

	try
	{
		ExecuteSQL(sqlStmt);
	}
	catch (soci::soci_error const& e)
	{
		throw e;

	}

	

}


void AHorizonDatabase::DeleteData(const FString& tableName, const FString& condition)
{
	FString sqlStmt = FString::Printf(TEXT("%s%s %s"), *HORIZON_SQL_DELETE_FROM, *tableName, *condition);
	try
	{
		ExecuteSQL(sqlStmt);
	}
	catch (soci::soci_error const& e)
	{
		throw e;

	}
}


void AHorizonDatabase::TruncateTable(const FString& tableName)
{
	FString sqlStmt;
	switch (BackEndType) {
	case EHorizonDatabaseBackEnd::Sqlite3://sqlite didn't has TRUNCATE TABLE 
		sqlStmt = FString::Printf(TEXT("%s%s"), *HORIZON_SQL_DELETE_FROM, *tableName);
		break;
	default:
		sqlStmt = FString::Printf(TEXT("%s%s"), *HORIZON_SQL_TRANCATE_TABLE_IF_EXISTS, *tableName);
		break;
	}
	
	// FString::Printf(TEXT("%s%s"), *HORIZON_SQL_TRANCATE_TABLE_IF_EXISTS, *tableName);
	try
	{
		ExecuteSQL(sqlStmt);
	}
	catch (soci::soci_error const& e)
	{
		throw e;

	}
}


void AHorizonDatabase::DropTable(const FString& tableName)
{
	FString sqlStmt = FString::Printf(TEXT("%s%s"), *HORIZON_SQL_DROP_TABLE_IF_EXISTS, *tableName);;
	try
	{
		ExecuteSQL(sqlStmt);
	}
	catch (soci::soci_error const& e)
	{
		throw e;

	}
}

bool AHorizonDatabase::IsTableExists(const FString& tableName)
{
	//SELECT count(*) FROM sqlite_master WHERE type = 'table' AND name = 'table_name';
	FString sqlStmt;//
	int count = 0;
	switch (BackEndType) {
		case EHorizonDatabaseBackEnd::Sqlite3://sqlite didn't has TRUNCATE TABLE 
		{
			FString selectFromSqliteMaster = "SELECT count(*) FROM sqlite_master WHERE type = 'table' AND name =";
			sqlStmt = FString::Printf(TEXT("WHERE type = 'table' AND name ='%s'"), *tableName);
			count = GetTableRowCount("sqlite_master", sqlStmt);
		}
		break;
		default:
		{
			ensureMsgf(false, TEXT("BackEndType didn't implemented properly:%d"), (int)BackEndType);
			count = GetTableRowCount(tableName, sqlStmt);
			break;
		}
	}

	return count > 0;
}

void  AHorizonDatabase::UpdateData(const FString& tableName, const FString& updateParam, const FString&  condition)
{
	FString sqlStmt = FString::Printf(TEXT("%s%s%s %s %s"), *HORIZON_SQL_UPDATE, *tableName, *HORIZON_SQL_SET, *updateParam, *condition);

	try
	{
		ExecuteSQL(sqlStmt);
	}
	catch (soci::soci_error const& e)
	{
		throw e;

	}
}

int AHorizonDatabase::GetTableRowCount(const FString& tableName, const FString& condition)
{
	int count = 0;
	FString sqlStmt = FString::Printf(TEXT("%s%s %s"), *HORIZON_SQL_SELECT_COUNT_ALL_FROM, *tableName, *condition);
	try
	{
		*SessionPtr << TCHAR_TO_UTF8(*sqlStmt), soci::into(count);
	}
	catch (soci::soci_error const& e)
	{
		throw e;

	}


	return count;
}

void AHorizonDatabase::ExecuteSQL(const FString& sqlStmt)
{
	if (SessionPtr.IsValid()) 
	{
		try {
			*SessionPtr << TCHAR_TO_UTF8(*sqlStmt);
		}
		catch (const std::exception& e) {
			throw e;
		}
	}


}




FString AHorizonDatabase::GetCreateTableSqlStmt(AHorizonDatabase* pDB, UStruct* pSchema)
{
	ensureMsgf(pDB->BackEndType != EHorizonDatabaseBackEnd::Empty, TEXT("Warning: Current database BackEndType is Empty"));
	auto tableName = pSchema->GetName();

	FString sqlStmt = HORIZON_SQL_CREATE_TABLE_IF_NOT_EXISTS + tableName + "(";
	for (TFieldIterator<UProperty> propIt(pSchema, EFieldIteratorFlags::SuperClassFlags::IncludeSuper);
		propIt; ++propIt)
	{
		//ref: http://www.w3schools.com/sql/sql_constraints.asp
		//TODO: how to add columnConstrains syntax at runtime?
		FString columnConstrains = "";
#if WITH_EDITOR || HACK_HEADER_GENERATOR
		//columnConstrains = propIt->GetMetaData("HorizonDBColumnConstraints");
#endif//#if WITH_EDITOR || HACK_HEADER_GENERATOR
		auto fieldType = ConvertToDBDataType(pDB, propIt->GetCPPType());//propIt->GetCPPType();
		auto fieldName = propIt->GetNameCPP();

		FString columnType = FString::Printf(TEXT("%s %s %s"), *fieldName, *fieldType, *columnConstrains);
		sqlStmt += columnType;
		if (nullptr != propIt->Next)
		{
			sqlStmt += ", ";
		}
	}
	sqlStmt += ")";

	return sqlStmt;


}



FString AHorizonDatabase::GetInsertBindingSqlStmt(UStruct* pSchema, bool bReplace)
{
	auto tableName = pSchema->GetName();
	FString insertSqlStmt;
	if (bReplace) {
		insertSqlStmt = HORIZON_SQL_INSERT_OR_REPLACE_INTO;
	}
	else {
		insertSqlStmt = HORIZON_SQL_INSERT_INTO;
	}

	FString sqlStmt = insertSqlStmt + tableName + "(";
	for (TFieldIterator<UProperty> propIt(pSchema, EFieldIteratorFlags::SuperClassFlags::IncludeSuper);
		propIt; ++propIt)
	{
		auto fieldName = propIt->GetNameCPP();
		sqlStmt += fieldName;
		if (nullptr != propIt->Next)
		{
			sqlStmt += ",";
		}
	}
	sqlStmt += ") ";
	sqlStmt += "values (";
	for (TFieldIterator<UProperty> propIt(pSchema, EFieldIteratorFlags::SuperClassFlags::IncludeSuper);
		propIt; ++propIt)
	{
		auto fieldName = propIt->GetNameCPP();
		sqlStmt += ":";
		sqlStmt += fieldName;
		if (nullptr != propIt->Next)
		{
			sqlStmt += ",";
		}
	}
	sqlStmt += ") ";
	return sqlStmt;
}



FString AHorizonDatabase::GetUpdateBindingSqlStmt(UStruct* pSchema) {

	auto tableName = pSchema->GetName();
	FString sqlStmt;

	sqlStmt = HORIZON_SQL_UPDATE + tableName + HORIZON_SQL_SET;
	for (TFieldIterator<UProperty> propIt(pSchema, EFieldIteratorFlags::SuperClassFlags::IncludeSuper);
		propIt; ++propIt)
	{
		auto fieldName = propIt->GetNameCPP();
		sqlStmt += fieldName;
		sqlStmt += " =:";
		sqlStmt += fieldName;
		if (nullptr != propIt->Next)
		{
			sqlStmt += ",";
		}
	}

	sqlStmt += " ";
	return sqlStmt;
}


//===========================End Blueprint Interface================================



//==========================Begin Private Function=================================================

FString AHorizonDatabase::ConvertToDBDataType(AHorizonDatabase* pDB, const FString& typeName)
{
	//TODO: check if we should map data from type name to real db data.

	FString result = typeName;
	switch (pDB->BackEndType) {
		case EHorizonDatabaseBackEnd::Empty:
		case EHorizonDatabaseBackEnd::Sqlite3:
		{
			//CREATE TABLE t1(
			//t  TEXT,     -- text affinity by rule 2
			//nu NUMERIC,  -- numeric affinity by rule 5
			//i  INTEGER,  -- integer affinity by rule 1
			//r  REAL,     -- real affinity by rule 4
			//no BLOB      -- no affinity by rule 3
			//);
			// http://www.sqlite.org/datatype3.html
			// sqlite3 use dynamic type system, no map needed?
			//if (typeName.Contains("int", ESearchCase::IgnoreCase)) {
			//	result = "INTEGER";
			//}
			//if (typeName.Contains("string", ESearchCase::IgnoreCase)) {
			//	result = "TEXT";
			//}

			//if (typeName.Contains("string", ESearchCase::IgnoreCase)) {
			//	result = "TEXT";
			//}
		}
		break;
		default:
		{
		}
		break;
	}

	return result;
}



//==========================End Private Function=================================================
