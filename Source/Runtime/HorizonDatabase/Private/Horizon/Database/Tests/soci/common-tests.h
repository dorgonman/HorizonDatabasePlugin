//
// Copyright (C) 2004-2008 Maciej Sobczak, Stephen Hutton
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef SOCI_COMMON_TESTS_H_INCLUDED
#define SOCI_COMMON_TESTS_H_INCLUDED

#include "soci/soci.h"

#ifdef HAVE_BOOST
// explicitly pull conversions for Boost's optional, tuple and fusion:
#include <boost/version.hpp>
#include "soci/boost-optional.h"
#include "soci/boost-tuple.h"
#include "soci/boost-gregorian-date.h"
#if defined(BOOST_VERSION) && BOOST_VERSION >= 103500
#include "soci/boost-fusion.h"
#endif // BOOST_VERSION
#endif // HAVE_BOOST

//#include "soci-compiler.h"

#define CATCH_CONFIG_RUNNER
//#include <catch.hpp>

#include <algorithm>
#include <cassert>
#include <clocale>
#include <cstdlib>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <typeinfo>

// Although SQL standard mandates right padding CHAR(N) values to their length
// with spaces, some backends don't confirm to it:
//
//  - Firebird does pad the string but to the byte-size (not character size) of
//  the column (i.e. CHAR(10) NONE is padded to 10 bytes but CHAR(10) UTF8 --
//  to 40).
//  - For MySql PAD_CHAR_TO_FULL_LENGTH option must be set, otherwise the value
//  is trimmed.
//  - SQLite never behaves correctly at all.
//
// This method will check result string from column defined as fixed char It
// will check only bytes up to the original string size. If padded string is
// bigger than expected string then all remaining chars must be spaces so if
// any non-space character is found it will fail.
void
checkEqualPadded(const std::string& padded_str, const std::string& expected_str)
{
    size_t const len = expected_str.length();
    std::string const start_str(padded_str, 0, len);

    if (start_str != expected_str)
    {
        throw soci::soci_error(
                "Expected string \"" + expected_str + "\" "
                "is different from the padded string \"" + padded_str + "\""
              );
    }

    if (padded_str.length() > len)
    {
        std::string const end_str(padded_str, len);
        if (end_str != std::string(padded_str.length() - len, ' '))
        {
            throw soci::soci_error(
                  "\"" + padded_str + "\" starts with \"" + padded_str +
                  "\" but non-space characater(s) are found aftewards"
                );
        }
    }
}

#define CHECK_EQUAL_PADDED(padded_str, expected_str) \
    CHECK_NOTHROW(checkEqualPadded(padded_str, expected_str));

// Objects used later in tests 14,15
struct PhonebookEntry
{
    std::string name;
    std::string phone;
};

struct PhonebookEntry2 : public PhonebookEntry
{
};

class PhonebookEntry3
{
public:
    void setName(std::string const & n) { name_ = n; }
    std::string getName() const { return name_; }

    void setPhone(std::string const & p) { phone_ = p; }
    std::string getPhone() const { return phone_; }

public:
    std::string name_;
    std::string phone_;
};

// user-defined object for test26 and test28
class MyInt
{
public:
    MyInt() : i_() {}
    MyInt(int i) : i_(i) {}
    void set(int i) { i_ = i; }
    int get() const { return i_; }
private:
    int i_;
};

namespace soci
{

// basic type conversion for user-defined type with single base value
template<> struct type_conversion<MyInt>
{
    typedef int base_type;

    static void from_base(int i, indicator ind, MyInt &mi)
    {
        if (ind == i_ok)
        {
            mi.set(i);
        }
    }

    static void to_base(MyInt const &mi, int &i, indicator &ind)
    {
        i = mi.get();
        ind = i_ok;
    }
};

// basic type conversion on many values (ORM)
template<> struct type_conversion<PhonebookEntry>
{
    typedef soci::values base_type;

    static void from_base(values const &v, indicator /* ind */, PhonebookEntry &pe)
    {
        // here we ignore the possibility the the whole object might be NULL
        pe.name = v.get<std::string>("NAME");
        pe.phone = v.get<std::string>("PHONE", "<NULL>");
    }

    static void to_base(PhonebookEntry const &pe, values &v, indicator &ind)
    {
        v.set("NAME", pe.name);
        v.set("PHONE", pe.phone, pe.phone.empty() ? i_null : i_ok);
        ind = i_ok;
    }
};

// type conversion which directly calls values::get_indicator()
template<> struct type_conversion<PhonebookEntry2>
{
    typedef soci::values base_type;

    static void from_base(values const &v, indicator /* ind */, PhonebookEntry2 &pe)
    {
        // here we ignore the possibility the the whole object might be NULL

        pe.name = v.get<std::string>("NAME");
        indicator ind = v.get_indicator("PHONE"); //another way to test for null
        pe.phone = ind == i_null ? "<NULL>" : v.get<std::string>("PHONE");
    }

    static void to_base(PhonebookEntry2 const &pe, values &v, indicator &ind)
    {
        v.set("NAME", pe.name);
        v.set("PHONE", pe.phone, pe.phone.empty() ? i_null : i_ok);
        ind = i_ok;
    }
};

template<> struct type_conversion<PhonebookEntry3>
{
    typedef soci::values base_type;

    static void from_base(values const &v, indicator /* ind */, PhonebookEntry3 &pe)
    {
        // here we ignore the possibility the the whole object might be NULL

        pe.setName(v.get<std::string>("NAME"));
        pe.setPhone(v.get<std::string>("PHONE", "<NULL>"));
    }

    static void to_base(PhonebookEntry3 const &pe, values &v, indicator &ind)
    {
        v.set("NAME", pe.getName());
        v.set("PHONE", pe.getPhone(), pe.getPhone().empty() ? i_null : i_ok);
        ind = i_ok;
    }
};

} // namespace soci

namespace soci
{
namespace tests
{

// TODO: improve cleanup capabilities by subtypes, soci_test name may be omitted --mloskot
//       i.e. optional ctor param accepting custom table name
class table_creator_base
{
public:
    table_creator_base(session& sql)
        : msession(sql) { drop(); }

    virtual ~table_creator_base() { drop();}
private:
    void drop()
    {
        try
        {
            msession << "drop table if exists soci_test";
        }
        catch (soci_error const& e)
        {
            //std::cerr << e.what() << std::endl;
            e.what();
        }
    }
    session& msession;

    SOCI_NOT_COPYABLE(table_creator_base)
};

class procedure_creator_base
{
public:
    procedure_creator_base(session& sql)
        : msession(sql) { drop(); }

    virtual ~procedure_creator_base() { drop();}
private:
    void drop()
    {
        try { msession << "drop procedure soci_test"; } catch (soci_error&) {}
    }
    session& msession;

    SOCI_NOT_COPYABLE(procedure_creator_base)
};

class function_creator_base
{
public:
    function_creator_base(session& sql)
        : msession(sql) { drop(); }

    virtual ~function_creator_base() { drop();}

protected:
    virtual std::string dropstatement()
    {
        return "drop function soci_test";
    }

private:
    void drop()
    {
        try { msession << dropstatement(); } catch (soci_error&) {}
    }
    session& msession;

    SOCI_NOT_COPYABLE(function_creator_base)
};

// This is a singleton class, at any given time there is at most one test
// context alive and common_tests fixture class uses it.
class test_context_base
{
public:
    test_context_base(backend_factory const &backEnd,
                    std::string const &connectString)
        : backEndFactory_(backEnd),
          connectString_(connectString)
    {
        // This can't be a CHECK() because the test context is constructed
        // outside of any test.
        assert(!the_test_context_);

        the_test_context_ = this;

        // To allow running tests in non-default ("C") locale, the following
        // environment variable can be set and then the current default locale
        // (which can itself be changed by setting LC_ALL environment variable)
        // will then be used.
        if (std::getenv("SOCI_TEST_USE_LC_ALL"))
            std::setlocale(LC_ALL, "");
    }

    static test_context_base const& get_instance()
    {
       // REQUIRE(the_test_context_);

        return *the_test_context_;
    }

    backend_factory const & get_backend_factory() const
    {
        return backEndFactory_;
    }

    std::string get_connect_string() const
    {
        return connectString_;
    }

    virtual std::string to_date_time(std::string const &dateTime) const = 0;

    virtual table_creator_base* table_creator_1(session&) const = 0;
    virtual table_creator_base* table_creator_2(session&) const = 0;
    virtual table_creator_base* table_creator_3(session&) const = 0;
    virtual table_creator_base* table_creator_4(session&) const = 0;

    // Override this if the backend doesn't handle floating point values
    // correctly, i.e. writing a value and reading it back doesn't return
    // *exactly* the same value.
    virtual bool has_fp_bug() const { return false; }

    // Override this if the backend doesn't handle multiple active select
    // statements at the same time, i.e. a result set must be entirely consumed
    // before creating a new one (this is the case of MS SQL without MARS).
    virtual bool has_multiple_select_bug() const { return false; }

    // Override this if the backend may not have transactions support.
    virtual bool has_transactions_support(session&) const { return true; }

    // Override this if the backend silently truncates string values too long
    // to fit by default.
    virtual bool has_silent_truncate_bug(session&) const { return false; }

    // Override this to call commit() if it's necessary for the DDL statements
    // to be taken into account (currently this is only the case for Firebird).
    virtual void on_after_ddl(session&) const { }

    // Put the database in SQL-complient mode for CHAR(N) values, return false
    // if it's impossible, i.e. if the database doesn't behave correctly
    // whatever we do.
    virtual bool enable_std_char_padding(session&) const { return true; }

    virtual ~test_context_base()
    {
        the_test_context_ = NULL;
    }

private:
    backend_factory const &backEndFactory_;
    std::string const connectString_;

    static test_context_base* the_test_context_;

    SOCI_NOT_COPYABLE(test_context_base)
};

// Currently all tests consist of just a single source file, so we can define
// this member here because this header is included exactly once.
tests::test_context_base* tests::test_context_base::the_test_context_ = NULL;


// Compare doubles for approximate equality. This has to be used everywhere
// where we write "3.14" (or "6.28") to the database as a string and then
// compare the value read back with the literal 3.14 floating point constant
// because they are not the same.
//
// It is also used for the backends which currently don't handle doubles
// correctly.
//
// Notice that this function is normally not used directly but rather from the
// macro below.
inline bool are_doubles_approx_equal(double const a, double const b)
{
    // The formula taken from CATCH test framework
    // https://github.com/philsquared/Catch/
    // Thanks to Richard Harris for his help refining this formula
    double const epsilon(std::numeric_limits<float>::epsilon() * 100);
    double const scale(1.0);
    return std::fabs(a - b) < epsilon * (scale + (std::max)(std::fabs(a), std::fabs(b)));
}

// This is a macro to ensure we use the correct line numbers. The weird
// do/while construction is used to make this a statement and the even weirder
// condition in while ensures that the loop is executed exactly once without
// triggering warnings from MSVC about the condition being always false.
#define ASSERT_EQUAL_APPROX(a, b) \
    do { \
      if (!are_doubles_approx_equal((a), (b))) { \
        FAIL( "Approximate equality check failed: " \
                  << std::fixed \
                  << std::setprecision(std::numeric_limits<double>::digits10 + 1) \
                  << (a) << " != " << (b) ); \
      } \
    } while ( (void)0, 0 )


// Exact double comparison function. We need one, instead of writing "a == b",
// only in order to have some place to put the pragmas disabling gcc warnings.
inline bool
are_doubles_exactly_equal(double a, double b)
{
    // Avoid g++ warnings: we do really want the exact equality here.
   // GCC_WARNING_SUPPRESS(float-equal)

    return a == b;

   // GCC_WARNING_RESTORE(float-equal)
}

#define ASSERT_EQUAL_EXACT(a, b) \
    do { \
      if (!are_doubles_exactly_equal((a), (b))) { \
        FAIL( "Exact equality check failed: " \
                  << std::fixed \
                  << std::setprecision(std::numeric_limits<double>::digits10 + 1) \
                  << (a) << " != " << (b) ); \
      } \
    } while ( (void)0, 0 )


// Compare two floating point numbers either exactly or approximately depending
// on test_context::has_fp_bug() return value.
inline bool
are_doubles_equal(test_context_base const& tc, double a, double b)
{
    return tc.has_fp_bug()
                ? are_doubles_approx_equal(a, b)
                : are_doubles_exactly_equal(a, b);
}

// This macro should be used when where we don't have any problems with string
// literals vs floating point literals mismatches described above and would
// ideally compare the numbers exactly but, unfortunately, currently can't do
// this unconditionally because at least some backends are currently buggy and
// don't handle the floating point values correctly.
//
// This can be only used from inside the common_tests class as it relies on
// having an accessible "tc_" variable to determine whether exact or
// approximate comparison should be used.
#define ASSERT_EQUAL(a, b) \
    do { \
      if (!are_doubles_equal(tc_, (a), (b))) { \
        FAIL( "Equality check failed: " \
                  << std::fixed \
                  << std::setprecision(std::numeric_limits<double>::digits10 + 1) \
                  << (a) << " != " << (b) ); \
      } \
    } while ( (void)0, 0 )


class common_tests
{
public:
    common_tests()
    : tc_(test_context_base::get_instance()),
      backEndFactory_(tc_.get_backend_factory()),
      connectString_(tc_.get_connect_string())
    {}

protected:
    test_context_base const & tc_;
    backend_factory const &backEndFactory_;
    std::string const connectString_;

    SOCI_NOT_COPYABLE(common_tests)
};

typedef std::auto_ptr<table_creator_base> auto_table_creator;



} // namespace tests

} // namespace soci

#endif // SOCI_COMMON_TESTS_H_INCLUDED
