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

// Note: The code in this file are ported from soci

#include "HorizonDatabasePrivatePCH.h"

//#include "AutomationEditorCommon.h"
#include "Horizon/Database/Tests/soci/common-tests.h"
#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>
#include <string>
#if WITH_DEV_AUTOMATION_TESTS


using namespace soci;
using namespace soci::tests;
static std::string connectString = ":memory:";
static soci::backend_factory const &backEnd = *soci::factory_sqlite3();

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHorizonSqlite3RowID, "Horizon.SOCI.Sqlite3.UnitTest.[sqlite][rowid][oid]", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FHorizonSqlite3RowID::RunTest(const FString& Parameters)
{
	bool bResult = true;

	soci::session sql(backEnd, connectString);

	try { sql << "drop table if exists test1"; }
	catch (soci::soci_error const &) {} // ignore if error

	sql <<
		"create table test1 ("
		"    id integer,"
		"    name varchar(100)"
		")";

	sql << "insert into test1(id, name) values(7, \'John\')";

	soci::rowid rid(sql);
	sql << "select oid from test1 where id = 7", into(rid);

	int id;
	std::string name;

	sql << "select id, name from test1 where oid = :rid",
		soci::into(id), soci::into(name), soci::use(rid);

	FString name2;
	sql << "select id, name from test1 where oid = :rid",
		soci::into(id), soci::into(name2), soci::use(rid);


	TestEqual(TEXT("Id should equal to 7"), id, 7);
	TestEqual(TEXT("Name should equal to John"), name, std::string("John"));

	sql << "drop table test1";
	if (ExecutionInfo.GetErrorTotal() > 0)
	{
		bResult = false;

	}
	return bResult;
}


// BLOB test
struct blob_table_creator : public table_creator_base
{
	blob_table_creator(soci::session & sql)
		: table_creator_base(sql)
	{
		sql <<
			"create table soci_test ("
			"    id integer,"
			"    img blob"
			")";
	}
};


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHorizonSqlite3Blob, "Horizon.SOCI.Sqlite3.UnitTest.[sqlite][blob]", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FHorizonSqlite3Blob::RunTest(const FString& Parameters)
{
	bool bResult = true;
	soci::session sql(backEnd, connectString);

	blob_table_creator tableCreator(sql);

	char buf[] = "abcdefghijklmnopqrstuvwxyz";

	sql << "insert into soci_test(id, img) values(7, '')";

	{
		soci::blob b(sql);

		sql << "select img from soci_test where id = 7", soci::into(b);
		TestEqual(TEXT("b.get_len() == 0"), (int)b.get_len(), 0);
		b.write(0, buf, sizeof(buf));
		TestEqual(TEXT("b.get_len() == sizeof(buf)"), b.get_len(), sizeof(buf));
		sql << "update soci_test set img=? where id = 7", soci::use(b);

		b.append(buf, sizeof(buf));
		TestEqual(TEXT("b.get_len() == 2 * sizeof(buf)"), b.get_len(), 2 * sizeof(buf));
		sql << "insert into soci_test(id, img) values(8, ?)", soci::use(b);
	}
	{
		soci::blob b(sql);
		sql << "select img from soci_test where id = 8", soci::into(b);
		TestEqual(TEXT("b.get_len() == 2 * sizeof(buf)"), b.get_len(), 2 * sizeof(buf));
		char buf2[100];
		b.read(0, buf2, 10);
		TestEqual(TEXT("std::strncmp(buf2, \"abcdefghij\", 10) == 0"), std::strncmp(buf2, "abcdefghij", 10), 0);


		sql << "select img from soci_test where id = 7", into(b);
		TestEqual(TEXT("b.get_len() == sizeof(buf)"), b.get_len(), sizeof(buf));

	}

	if (ExecutionInfo.GetErrorTotal() > 0)
	{
		bResult = false;

	}
	return bResult;
}


// This test was put in to fix a problem that occurs when there are both
// into and use elements in the same query and one of them (into) binds
// to a vector object.

struct test3_table_creator : table_creator_base
{
	test3_table_creator(soci::session & sql) : table_creator_base(sql)
	{
		sql << "create table soci_test( id integer, name varchar, subname varchar);";
	}
};


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHorizonSqlite3UseAndVectorInfo, "Horizon.SOCI.Sqlite3.UnitTest.[sqlite][use][into][vector]", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FHorizonSqlite3UseAndVectorInfo::RunTest(const FString& Parameters)
{
	bool bResult = true;


	soci::session sql(backEnd, connectString);

	test3_table_creator tableCreator(sql);

	sql << "insert into soci_test(id,name,subname) values( 1,'john','smith')";
	sql << "insert into soci_test(id,name,subname) values( 2,'george','vals')";
	sql << "insert into soci_test(id,name,subname) values( 3,'ann','smith')";
	sql << "insert into soci_test(id,name,subname) values( 4,'john','grey')";
	sql << "insert into soci_test(id,name,subname) values( 5,'anthony','wall')";

	{
		std::vector<int> v(10);

		statement s(sql.prepare << "Select id from soci_test where name = :name");

		std::string name = "john";

		s.exchange(use(name, "name"));
		s.exchange(into(v));

		s.define_and_bind();
		s.execute(true);
		TestEqual(TEXT("v.size() == 2"), (int)v.size(), 2);
	}

	if (ExecutionInfo.GetErrorTotal() > 0)
	{
		bResult = false;

	}
	return bResult;
}


// Test case from Amnon David 11/1/2007
// I've noticed that table schemas in SQLite3 can sometimes have typeless
// columns. One (and only?) example is the sqlite_sequence that sqlite
// creates for autoincrement . Attempting to traverse this table caused
// SOCI to crash. I've made the following code change in statement.cpp to
// create a workaround:

struct test4_table_creator : table_creator_base
{
	test4_table_creator(soci::session & sql) : table_creator_base(sql)
	{
		sql << "create table soci_test (col INTEGER PRIMARY KEY AUTOINCREMENT, name char)";
	}
};


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHorizonSqlite3Sequence, "Horizon.SOCI.Sqlite3.UnitTest.[sqlite][sequence]", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FHorizonSqlite3Sequence::RunTest(const FString& Parameters)
{
	bool bResult = true;
	// we need to have an table that uses autoincrement to test this.
	soci::session sql(backEnd, connectString);

	test4_table_creator tableCreator(sql);

	sql << "insert into soci_test(name) values('john')";
	sql << "insert into soci_test(name) values('james')";

	{
		int key;
		std::string name;
		sql << "select * from soci_test", into(key), into(name);

		TestEqual(TEXT("name == john"), name, std::string("john"));
		rowset<row> rs = (sql.prepare << "select * from sqlite_sequence");
		rowset<row>::const_iterator it = rs.begin();
		row const& r1 = (*it);

		TestEqual(TEXT("r1.get<std::string>(0) == \"soci_test\""), r1.get<std::string>(0), std::string("soci_test"));
		TestEqual(TEXT("r1.get<std::string>(1) == \"2\""), r1.get<std::string>(1), std::string("2"));
	}
	if (ExecutionInfo.GetErrorTotal() > 0)
	{
		bResult = false;

	}
	return bResult;
}

struct longlong_table_creator : table_creator_base
{
	longlong_table_creator(soci::session & sql)
		: table_creator_base(sql)
	{
		sql << "create table soci_test(val number(20))";
	}
};

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHorizonSqlite3LongLong, "Horizon.SOCI.Sqlite3.UnitTest.[sqlite][longlong]", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FHorizonSqlite3LongLong::RunTest(const FString& Parameters)
{
	bool bResult = true;
	soci::session sql(backEnd, connectString);

	longlong_table_creator tableCreator(sql);

	long long v1 = 1000000000000LL;
	sql << "insert into soci_test(val) values(:val)", use(v1);

	long long v2 = 0LL;
	sql << "select val from soci_test", into(v2);
	TestEqual(TEXT("v2 == v1"), v2, v1);
	if (ExecutionInfo.GetErrorTotal() > 0)
	{
		bResult = false;

	}
	return bResult;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHorizonSqlite3VectorLongLong, "Horizon.SOCI.Sqlite3.UnitTest.[sqlite][vector][longlong]", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FHorizonSqlite3VectorLongLong::RunTest(const FString& Parameters)
{
	bool bResult = true;
	soci::session sql(backEnd, connectString);

	longlong_table_creator tableCreator(sql);

	std::vector<long long> v1;
	v1.push_back(1000000000000LL);
	v1.push_back(1000000000001LL);
	v1.push_back(1000000000002LL);
	v1.push_back(1000000000003LL);
	v1.push_back(1000000000004LL);

	sql << "insert into soci_test(val) values(:val)", use(v1);

	std::vector<long long> v2(10);
	sql << "select val from soci_test order by val desc", into(v2);



	TestEqual(TEXT("v2.size() == 5"), (int)v2.size(), 5);


	TestEqual(TEXT("v2[0] == 1000000000004LL"), v2[0], 1000000000004LL);
	TestEqual(TEXT("v2[1] == 1000000000003LL"), v2[1], 1000000000003LL);
	TestEqual(TEXT("v2[2] == 1000000000002LL"), v2[2], 1000000000002LL);
	TestEqual(TEXT("v2[3] == 1000000000001LL"), v2[3], 1000000000001LL);
	TestEqual(TEXT("v2[4] == 1000000000000LL"), v2[4], 1000000000000LL);
	if (ExecutionInfo.GetErrorTotal() > 0)
	{
		bResult = false;

	}
	return bResult;
}

struct table_creator_for_get_last_insert_id : table_creator_base
{
	table_creator_for_get_last_insert_id(soci::session & sql)
		: table_creator_base(sql)
	{
		sql << "create table soci_test(id integer primary key autoincrement)";
		sql << "insert into soci_test (id) values (41)";
		sql << "delete from soci_test where id = 41";
	}
};

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHorizonSqlite3LastInsert, "Horizon.SOCI.Sqlite3.UnitTest.[sqlite][last-insert-id]", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FHorizonSqlite3LastInsert::RunTest(const FString& Parameters)
{
	bool bResult = true;
	soci::session sql(backEnd, connectString);
	table_creator_for_get_last_insert_id tableCreator(sql);
	sql << "insert into soci_test default values";
	long id;
	bool result = sql.get_last_insert_id("soci_test", id);

	TestEqual(TEXT("result == true"), result, true);
	TestEqual(TEXT("id == 42"), id, (long)42);

	if (ExecutionInfo.GetErrorTotal() > 0)
	{
		bResult = false;

	}
	return bResult;
}






#endif //WITH_DEV_AUTOMATION_TESTS