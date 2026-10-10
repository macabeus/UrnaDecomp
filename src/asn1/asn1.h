/*
 * asn1.h
 *
 * Abstract Syntax Notation 1 Encoding Rules
 *
 * Copyright (c) 1993-1998 Equivalence Pty. Ltd.
 *
 * Copyright (c) 2001 Institute for Information Industry, Taiwan, Republic of China
 * (http://www.iii.org.tw/iiia/ewelcome.htm)
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.0 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 * the License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is Portable Windows Library.
 *
 * The Initial Developer of the Original Code is Equivalence Pty. Ltd.
 *
 * Portions are Copyright (C) 1993 Free Software Foundation, Inc.
 * All Rights Reserved.
 *
 * Contributor(s): Huang-Ming Huang
 *
 * The code is adapted from asner.h of PWLib, but the dependancy on PWLib has
 * been removed.
 *
 * The code is modified by Genesys Telecommunications Labs UK, 2003-2011
 * Contributors: 
 *    Arunas Ruksnaitis <arunas.ruksnaitis@genesyslab.com>
 *    Rustam Mirzaev <rustam.mirzaev@genesyslab.com>
 *
 * - support proper DER encoding
 * - remove dependency from BOOST libraries
 * - ASN1_HAS_IOSTREAM is no longer used
 * - ASN1_STD definition is used across all sources in order to support
 *   compilers without proper std namespace (f.i., on HPUX +AP - HP_aCC_RW has to be defined)
 * - Ability to use longer integers (64-bit) added (ASN_USE_LONG has to be defined)
 */

#ifndef _ASN1_H
#define _ASN1_H

#include "Platform.h"

#if defined(_WIN32)
	#include "Platform_WIN32.h"
#elif defined(__VMS)
	#include "Platform_VMS.h"
#elif defined(ALS_VXWORKS)
	#include "Platform_VX.h"
#elif defined(ALS_OS_FAMILY_UNIX)
	#include "Platform_POSIX.h"
#endif

// Reduce bloat imported by "ASN1/UnWindows.h"
#if defined(_WIN32)
	#if !defined(_WIN32_WINNT)
		#define _WIN32_WINNT 0x0501
	#endif
	#if !defined(WIN32_LEAN_AND_MEAN) && !defined(ASN1_BLOATED_WIN32)
		#define WIN32_LEAN_AND_MEAN
	#endif
#endif


// Unicode Support
#if defined(UNICODE) && !defined(ASN1_WIN32_UTF8)
	#define ASN1_WIN32_UTF8
#endif


//
// Ensure that ASN1_DLL is default unless ASN1_STATIC is defined
//
#if defined(_WIN32) && defined(_DLL)
	#if !defined(ASN1_DLL) && !defined(ASN1_STATIC)
		#define ASN1_DLL
	#endif
#endif

#if defined(_MSC_VER)
	#if defined(ASN1_DLL)
		#if defined(_DEBUG)
			#define ASN1_LIB_SUFFIX "d.lib"
		#else
			#define ASN1_LIB_SUFFIX ".lib"
		#endif
	#elif defined(_DLL)
		#if defined(_DEBUG)
			#define ASN1_LIB_SUFFIX "mdd.lib"
		#else
			#define ASN1_LIB_SUFFIX "md.lib"
		#endif
	#else
		#if defined(_DEBUG)
			#define ASN1_LIB_SUFFIX "mtd.lib"
		#else
			#define ASN1_LIB_SUFFIX "mt.lib"
		#endif
	#endif
#endif

//
// The following block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the ASN1_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// ASN1_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
//
#if defined(_WIN32) && defined(ASN1_DLL)
	#if defined(ASN1_EXPORTS)
		#define ASN1_API __declspec(dllexport)
	#else
		#define ASN1_API __declspec(dllimport)
	#endif
#endif


#if !defined(ASN1_API)
	#define ASN1_API
#endif


//
// Automatically link ASN1 library.
//
#if defined(_MSC_VER)
	#if !defined(ASN1_NO_AUTOMATIC_LIBS) && !defined(ASN1_EXPORTS)
		#pragma comment(lib, "ASN1rts" ASN1_LIB_SUFFIX)
	#endif
#endif


#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#ifndef ASN1_STD
#  ifdef HP_aCC_RW
#    define ASN1_STD ::
#  else
#    define ASN1_STD std::
#  endif
#endif


#ifndef __USE_STD_IOSTREAM
#  define __USE_STD_IOSTREAM
#endif

#include <assert.h>
#include <stdarg.h>
#include <string>
#include <memory>
#include <iterator>
#include <algorithm>
#include <vector>
#include <functional>
#include <time.h>
#include <limits.h>
#include <cstring>
#include <sstream>

#ifdef ASN_USE_LONG

#if defined(__osf__)
  #define  INT_TYPE           long
  #define UINT_TYPE  unsigned long
#elif defined(_WIN32)
  #define  INT_TYPE           __int64
  #define UINT_TYPE  unsigned __int64
#else
  #define  INT_TYPE           long long
  #define UINT_TYPE  unsigned long long
#endif

#else
  #define  INT_TYPE           int
  #define UINT_TYPE  unsigned int
#endif


//----------------------------------------------------------------------------

#if defined(HP_aCC_RW)
#  include <strstream.h>
typedef ios::io_state iostate;

struct strstream_defroster
{
  strstream_defroster(ASN1_STD ostrstream& strm) : m_strm(strm) {}
  ~strstream_defroster() { m_strm.rdbuf()->freeze(0); }
private:
  ASN1_STD ostrstream& m_strm;
};

#define DECLARE_STRSTREAM(strm) \
      ASN1_STD ostrstream strm; \
      strstream_defroster df_##strm(strm)

#else
#  include <sstream>
   typedef ASN1_STD ios::iostate iostate;

#define DECLARE_STRSTREAM(strm)  ASN1_STD ostringstream strm

#endif

#ifdef NEED_WSTRING_TYPEDEF
namespace std {
  typedef basic_string<wchar_t> wstring;
}
#endif

namespace ASN1 {
	ASN1_API unsigned CountBits(UINT_TYPE range);
} 

#include "AssocVector.h"

/////////////////////////////////////////////////////////////////////////////

namespace ASN1 {

  class Visitor;
  class ConstVistor;
  class PrintHelper;

  enum TagClass {
    UniversalTagClass,
    ApplicationTagClass,
    ContextSpecificTagClass,
    PrivateTagClass
  };

  enum UniversalTags {
    InvalidUniversalTag,
    UniversalBoolean,
    UniversalInteger,
    UniversalBitString,
    UniversalOctetString,
    UniversalNull,
    UniversalObjectId,
    UniversalObjectDescriptor,
    UniversalExternalType,
    UniversalReal,
    UniversalEnumeration,
    UniversalEmbeddedPDV,
    UniversalSequence = 16,
    UniversalSet,
    UniversalNumericString,
    UniversalPrintableString,
    UniversalTeletexString,
    UniversalVideotexString,
    UniversalIA5String,
    UniversalUTCTime,
    UniversalGeneralisedTime,
    UniversalGraphicString,
    UniversalVisibleString,
    UniversalGeneralString,
    UniversalUniversalString,
    UniversalBMPString = 30
  };

  enum { InvalidTag = 0 };

  inline bool
    isExplicit(unsigned tag) { return (tag & (1<<29))!=0; }

  inline unsigned
    makePrimitive(unsigned tag) { return tag & ~(1<<29); }

  enum ConstraintType {
    Unconstrained,
    PartiallyConstrained,
    FixedConstraint,
    ExtendableConstraint
  };

  class Visitor;
  class ConstVisitor;
  class AbstractData;

  namespace detail {

    template <UINT_TYPE v>
    struct int_to_type
    {};
  }

  template<class T>
  using asn_ptr = ASN1_STD unique_ptr<T>;

  /**
   * Base class for ASN.1 types.
   *
   * The ASN1::AbstractData class provides an abstract base class from which all ASN.1
   * builtin and user-defined type classes are derived. This class offers polymorphic
   * functionality including copying/cloning, comparison, validity check, and output.
   * The class is abstract and can only be instantiated in derived forms.
   *
   * The ASN1::AbstractData class also plays the role \a Element in \a Visitor Design Pattern, described
   * by Gamma, et al. It has the accept() operations which dispatch the request according to their exact
   * subtype (ConcreteElement). In this implementation, there are two kinds of \a Visitors, \c Visitor
   * and \c ConstVistor. See those two classes for more detail explanation.
   *
   * @warning For all the AbstractData subclass, the assignment operator do not involve the exchange of
   *  constraints and tags. That is to say, the type been assigned will
   *  retain the same constraints and tags as it hasn't been assigned.
   *  However all the copy constructors do copy the  constraints and tags from the object been copied.
   *  It is the programmer's responsibility to prevent using copy constructor on different types.
   */
  class ASN1_API AbstractData
  {
  public:
    virtual ~AbstractData() {};

    /**
     *  Allocate a copy of this object on the freestore.
     *
     *  The object allocated is of
     *  the same actual subclass as the object on which this method is invoked, and
     *  has a copy of all of its values. The allocated object is owned by the caller,
     *  who is responsible for deleting it.
     */
    AbstractData* clone() const { return do_clone(); }

    /**
     * Verify that the object conforms to any type constraints on it.
     *
     * These constraints
     * consist of those inherent to the ASN.1 type (such as NumericString), and any subtype
     * constraints. The isValid() function accepts a value as an acceptable extended value.
     * That is, if the ASN.1 type is defined as being extensible (using the ... notation),
     * then a value that could fit within such an extension is considered valid.
     */
    bool isValid() const ;

    /**
     * Verify that the object conforms to any type constraints on it,
     * without allowing for any extended values.
     *
     * For types that do not have
     * extensibility defined, isStrictlyValid() is equivalent to isValid().
     * For constructed types, isStrictlyValid() returns true when isStrictlyValid()
     * is true for all the components.
     */
    bool isStrictlyValid() const ;

    /**
     * Compare two AbstractData Objects.
     *
     * @return The return value is 0 if this object equals to \c other;
     *  the return value > 0 if this object is greater than \c other.
     *
     * This implementation allows only the comparison between compatible types, e.g.
     * IA5String and VisibleString. The behavior of comparing between incompatible
     * types is undefined.
     */
    INT_TYPE compare(const AbstractData& other) const { return do_compare(other); }

    /*
     * Most operators are defined as member functions rather than friend functions because
     * GCC 2.95.x have problem to resolve template operator functions when  any non-template
     * operator function exists.
     */
    bool operator == (const AbstractData& rhs) const { return compare(rhs) == 0; }
    bool operator != (const AbstractData& rhs) const { return compare(rhs) != 0; }
    bool operator <  (const AbstractData& rhs) const { return compare(rhs) <  0; }
    bool operator >  (const AbstractData& rhs) const { return compare(rhs) >  0; }
    bool operator <= (const AbstractData& rhs) const { return compare(rhs) <= 0; }
    bool operator >= (const AbstractData& rhs) const { return compare(rhs) >= 0; }

    /**
     * Returns the ASN.1 Tag.
     *
     * An ASN.1 tag has two parts, tag class and tag number.
     * The return value of getTag() is (tagClass << 30 | tagNumber).
     */
    unsigned getTag() const { return info()->tag; }

    unsigned getParentTag() const { return parent_info() ? parent_info()->tag : 0; }

    /**
     * Accept a \c Visitor which implement a fragment of algorithm or request for
     * each corresponding class of ASN.1 object structure.
     *
     * @param v The \c Visitor.
     * @return true if the operation has been successfully executed.
     */
    bool accept(Visitor& v) { return do_accept(v) ;}
    /**
     * Accept a \c Visitor which implement a fragment of algorithm or request for
     * each corresponding class of ASN.1 object structure.
     *
     * @param v The \c ConstVisitor.
     * @return true if the operation has been successfully executed.
     */
    bool accept(ConstVisitor& v) const { return do_accept(v); }

    /**
     * Create a AbstractData object based on the \c info structure.
     *
     * This is a factory method for creating an instance of ASN1::AbstractData
     * object. Each concrete ASN1::AbstractData subclass contains a structure which describe
     * the information of the type, constraints, and how it can be instantiate. Using this information,
     * an object can be created by the static member function.
     *
     * @return NULL if the object creation is fail; otherwise a new object is successfully
     *  created and should be deleted by its caller.
     */
    inline static AbstractData* create(const void* info)
    { return info ? static_cast<const InfoType*>(info)->create(info) : 0; }

    typedef AbstractData* (*CreateFun)(const void*);

  private:
    virtual INT_TYPE do_compare(const AbstractData& other) const =0;
    virtual AbstractData* do_clone() const = 0;
    virtual bool do_accept(Visitor&) = 0;
    virtual bool do_accept(ConstVisitor&) const =0;
  public:
    virtual ASN1_STD string getTypeName() const { return "AbstractData"; }
  private:

  protected:
    AbstractData(const void* info);

    struct InfoType
    {
      CreateFun create;
      unsigned tag; /* the tag is represented using the formula (tagClass << 30 | tagNumber) */
      const void* parent_info;
    };
    const void* info_;

  public:
    const InfoType* info() const { return static_cast<const InfoType*>(info_);}
    const InfoType* parent_info() const { return info() ? static_cast<const InfoType*>(info()->parent_info) : 0;}


  public:
    friend ASN1_STD ostream & operator<<(
             ASN1_STD ostream &strm,        // Stream to print the object into.
             const AbstractData & obj       // Object to print to the stream.
             );

    friend ASN1_STD istream & operator >>(
              ASN1_STD istream &strm,       // Stream which stores the object into.
              AbstractData & obj            // Object to retrive from the stream.
              );

    iostate get_from(ASN1_STD istream &);
    iostate print_on(ASN1_STD ostream &) const ;

    void Print(PrintHelper& helper) const;

    /**
     *  Set the AbstractData object from a string containing ASN.1 value notation.
     *
     *  This supports full ASN.1 except for the following limitations:
     *  \li Value references are not allowed. That is, a value can not contain the names of other values.
     *  Note that values are commonly used to provide the high order part of an Object Identifier.
     *  \li Open type is not currently supported.
     */
    bool setFromValueNotation(const ASN1_STD string& valueString);
    /**
     *  Format the AbstractData object using normal ASN.1 value notation into the string.
     */
    ASN1_STD string asValueNotation() const;
  };

  /** Base class for constrained ASN encoding/decoding.
   */
  class ASN1_API ConstrainedObject : public AbstractData
  {
  public:
    ConstrainedObject(const void* info) : AbstractData(info){}
    bool extendable() const { return info()->type == ExtendableConstraint; }
    bool constrained() const { return info()->type != Unconstrained; }
    unsigned getConstraintType() const { return info()->type; }
    INT_TYPE getLowerLimit() const { return info()->lowerLimit; }
    UINT_TYPE getUpperLimit() const { return info()->upperLimit; }

  protected:
    struct InfoType
    {
      CreateFun create;
      unsigned tag;
      const void* parent_info;
      unsigned type;
      INT_TYPE  lowerLimit;
      UINT_TYPE upperLimit;
    };
    const InfoType* info() const  { return static_cast<const InfoType*>(info_);}
  };

  /** Class for ASN Null type.
   */
  class ASN1_API Null : public AbstractData
  {
    Null(const void* info) : AbstractData(info){}
  public:
    Null() : AbstractData(&theInfo){}

    bool isValid() const { return true;}
    bool isStrictlyValid() const { return true;}

    Null * clone() const { return static_cast<Null*>(do_clone()); }
    static AbstractData* create(const void* info);
    void swap(Null& that) { /* do nothing */}

    bool operator == (const Null& ) const { return true;  }
    bool operator != (const Null& ) const { return false; }
    bool operator <  (const Null& ) const { return false; }
    bool operator >  (const Null& ) const { return false; }
    bool operator <= (const Null& ) const { return false; }
    bool operator >= (const Null& ) const { return false; }
    static const InfoType theInfo;
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }

  private:
    virtual INT_TYPE do_compare(const AbstractData& other) const;
    virtual AbstractData* do_clone() const ;
    virtual bool do_accept(Visitor&);
    virtual bool do_accept(ConstVisitor&) const;
  };

  /** Class for ASN Boolean type.
   */
  class ASN1_API BOOLEAN : public AbstractData
  {
  protected:
    BOOLEAN(const void* info);
  public:
    typedef bool value_type;
    typedef bool& reference;
    typedef bool const_reference;
    BOOLEAN() : AbstractData(&theInfo), value(false) {}
    BOOLEAN(bool b , const void* info= &theInfo);

    BOOLEAN(const BOOLEAN& that);
    BOOLEAN& operator = (const BOOLEAN& that) { value = that.value; return *this; }
    BOOLEAN& operator = (bool b) { value = b; return *this; }
    operator bool() const { return value; }

    bool isValid() const { return true;}
    bool isStrictlyValid() const { return true;}

    BOOLEAN * clone() const { return static_cast<BOOLEAN*>(do_clone()); }
    static AbstractData* create(const void*);
    void swap(BOOLEAN& that) { ASN1_STD swap(value, that.value); }

    bool operator == (const BOOLEAN& rhs) const { return value == rhs.value ; }
    bool operator != (const BOOLEAN& rhs) const { return value != rhs.value ; }
    bool operator <  (const BOOLEAN& rhs) const { return value <  rhs.value ; }
    bool operator >  (const BOOLEAN& rhs) const { return value >  rhs.value ; }
    bool operator <= (const BOOLEAN& rhs) const { return value <= rhs.value ; }
    bool operator >= (const BOOLEAN& rhs) const { return value >= rhs.value ; }

#if __GNUC__> 2 || __GNUC_MINOR__ > 95
    friend bool operator == (bool lhs, const BOOLEAN& rhs) { return lhs == rhs.value ; }
    friend bool operator != (bool lhs, const BOOLEAN& rhs) { return lhs != rhs.value ; }
    friend bool operator <  (bool lhs, const BOOLEAN& rhs) { return lhs <  rhs.value ; }
    friend bool operator >  (bool lhs, const BOOLEAN& rhs) { return lhs >  rhs.value ; }
    friend bool operator <= (bool lhs, const BOOLEAN& rhs) { return lhs <= rhs.value ; }
    friend bool operator >= (bool lhs, const BOOLEAN& rhs) { return lhs >= rhs.value ; }
#endif

    bool operator == (bool rhs) const { return value == rhs ; }
    bool operator != (bool rhs) const { return value != rhs ; }
    bool operator <  (bool rhs) const { return value <  rhs ; }
    bool operator >  (bool rhs) const { return value >  rhs ; }
    bool operator <= (bool rhs) const { return value <= rhs ; }
    bool operator >= (bool rhs) const { return value >= rhs ; }
    static const InfoType theInfo;
    static bool equal_type(const ASN1::AbstractData& type)
    {return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo);}

  private:
    virtual INT_TYPE do_compare(const AbstractData& other) const;
    virtual AbstractData* do_clone() const ;
    virtual bool do_accept(Visitor&);
    virtual bool do_accept(ConstVisitor&) const;
    bool value;
  };

  /** Class for ASN Integer type.
   */
  class ASN1_API INTEGER : public ConstrainedObject
  {
  protected:
    INTEGER(const void* info);
  public:
    typedef INT_TYPE int_type;
    typedef INT_TYPE value_type;
    typedef int_type& reference;
    typedef int_type const_reference;

    INTEGER() : ConstrainedObject(&theInfo), value(0){}
    INTEGER(int_type val, const void* info = &theInfo);
    INTEGER(const INTEGER& other);

    INTEGER& operator = (const INTEGER& val) { setValue(val.value) ; return *this;}
    INTEGER& operator = (int_type val) { setValue(val); return *this;}
    int_type getValue() const { return static_cast<int_type>(value);}

    bool isValid() const { return isStrictlyValid() || getConstraintType() == ExtendableConstraint;  }
    bool isStrictlyValid() const;

    INTEGER* clone() const { return static_cast<INTEGER*>(do_clone()); }
    static AbstractData* create(const void*);
    void swap(INTEGER& that) { ASN1_STD swap(value, that.value); }

    bool operator == (const INTEGER& rhs) const { return getValue() == rhs.getValue(); }
    bool operator != (const INTEGER& rhs) const { return getValue() != rhs.getValue(); }
    bool operator <  (const INTEGER& rhs) const { return getValue() <  rhs.getValue(); }
    bool operator >  (const INTEGER& rhs) const { return getValue() >  rhs.getValue(); }
    bool operator <= (const INTEGER& rhs) const { return getValue() <= rhs.getValue(); }
    bool operator >= (const INTEGER& rhs) const { return getValue() >= rhs.getValue(); }

#if __GNUC__> 2 || __GNUC_MINOR__ > 95
    friend bool operator == (int_type lhs, const INTEGER& rhs) { return lhs == rhs.getValue(); }
    friend bool operator != (int_type lhs, const INTEGER& rhs) { return lhs != rhs.getValue(); }
    friend bool operator <  (int_type lhs, const INTEGER& rhs) { return lhs <  rhs.getValue(); }
    friend bool operator >  (int_type lhs, const INTEGER& rhs) { return lhs >  rhs.getValue(); }
    friend bool operator <= (int_type lhs, const INTEGER& rhs) { return lhs <= rhs.getValue(); }
    friend bool operator >= (int_type lhs, const INTEGER& rhs) { return lhs >= rhs.getValue(); }
#endif

    bool operator == (int_type rhs) const { return getValue() == rhs; }
    bool operator != (int_type rhs) const { return getValue() != rhs; }
    bool operator <  (int_type rhs) const { return getValue() <  rhs; }
    bool operator >  (int_type rhs) const { return getValue() >  rhs; }
    bool operator <= (int_type rhs) const { return getValue() <= rhs; }
    bool operator >= (int_type rhs) const { return getValue() >= rhs; }

    INTEGER& operator += (int_type val) { value += val; return *this; }
    INTEGER& operator -= (int_type val) { value -= val; return *this; }
    INTEGER& operator *= (int_type val) { value *= val; return *this; }
    INTEGER& operator /= (int_type val) { value /= val; return *this; }
    INTEGER& operator %= (int_type val) { value %= val; return *this; }

    INTEGER& operator += (const INTEGER& val) { value += val.getValue(); return *this; }
    INTEGER& operator -= (const INTEGER& val) { value -= val.getValue(); return *this; }
    INTEGER& operator *= (const INTEGER& val) { value *= val.getValue(); return *this; }
    INTEGER& operator /= (const INTEGER& val) { value /= val.getValue(); return *this; }
    INTEGER& operator %= (const INTEGER& val) { value %= val.getValue(); return *this; }

    INTEGER& operator ++ ()    { ++ value  ; return *this;}
    INTEGER  operator ++ (int) { INTEGER result(*this); ++value; return result;}
    INTEGER& operator -- ()    { -- value  ; return *this;}
    INTEGER  operator -- (int) { INTEGER result(*this); --value; return result;}

    int_type operator + (const INTEGER& rhs) const { int_type t(getValue()); return t+=rhs.getValue();}
    int_type operator - (const INTEGER& rhs) const { int_type t(getValue()); return t-=rhs.getValue();}
    int_type operator * (const INTEGER& rhs) const { int_type t(getValue()); return t*=rhs.getValue();}
    int_type operator / (const INTEGER& rhs) const { int_type t(getValue()); return t/=rhs.getValue();}

    friend int_type operator + (int_type lhs, const INTEGER& rhs) { int_type t(lhs); return t+=rhs.getValue();}
    friend int_type operator - (int_type lhs, const INTEGER& rhs) { int_type t(lhs); return t-=rhs.getValue();}
    friend int_type operator * (int_type lhs, const INTEGER& rhs) { int_type t(lhs); return t*=rhs.getValue();}
    friend int_type operator / (int_type lhs, const INTEGER& rhs) { int_type t(lhs); return t/=rhs.getValue();}

    int_type operator + (int_type rhs) { int_type t(getValue()); return t+=rhs;}
    int_type operator - (int_type rhs) { int_type t(getValue()); return t-=rhs;}
    int_type operator * (int_type rhs) { int_type t(getValue()); return t*=rhs;}
    int_type operator / (int_type rhs) { int_type t(getValue()); return t/=rhs;}

    static const InfoType theInfo;
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }

  protected:
    void setValue(int_type val) { value = static_cast<UINT_TYPE>(val); }
    UINT_TYPE value;
  private:
    virtual INT_TYPE do_compare(const AbstractData& other) const;
    virtual AbstractData* do_clone() const ;
    virtual bool do_accept(Visitor&);
    virtual bool do_accept(ConstVisitor&) const;
  };

  /**
    */
  class ASN1_API IntegerWithNamedNumber : public INTEGER
  {
  public:
    struct NameEntry
    {
      INT_TYPE value;
      const char* name;
    };

    struct NameEntryCmp
    {
      bool operator () (const NameEntry& lhs, const NameEntry& rhs) const
      { return lhs.value < rhs.value; }
      bool operator () (INT_TYPE lhs, const NameEntry& rhs) const
      { return lhs < rhs.value; }
      bool operator () (const NameEntry& lhs, INT_TYPE rhs) const
      { return lhs.value < rhs; }
    };

  protected:
    IntegerWithNamedNumber(const void* info);
    IntegerWithNamedNumber(INT_TYPE val, const void* info);
    static AbstractData* create(const void* info);

    struct InfoType
    {
      CreateFun create;
      unsigned tag;
      const void* parent_info;
      unsigned type;
      INT_TYPE lowerLimit;
      UINT_TYPE upperLimit;
      const NameEntry* nameEntries;
      unsigned entryCount;
    };
    const InfoType* info() const { return static_cast<const InfoType*>(info_); }

  private:
    virtual bool do_accept(Visitor&);
    virtual bool do_accept(ConstVisitor&) const;

  public:
    bool getName(ASN1_STD string&) const;
    bool setFromName(const ASN1_STD string&);
  };

  namespace detail {
    template <INT_TYPE i>
    struct is_negtive
    {
      enum { yes = (i<0) };
    };

    template <bool isNegtive>
    struct select_integer_type;

    template <>
    struct select_integer_type<false>
    {
      typedef UINT_TYPE value_type;
    };

    template <>
    struct select_integer_type<true>
    {
      typedef INT_TYPE value_type;
    };
  }

  template <ConstraintType contraint, INT_TYPE lower, UINT_TYPE upper >
  class Constrained_INTEGER : public INTEGER
  {
    typedef Constrained_INTEGER<contraint, lower, upper> ThisType;
    typedef INTEGER Inherited;

  protected:
    typedef Inherited::InfoType InfoType;

  public:
    enum {
      UpperLimit = upper,
      LowerLimit = lower
    };

    // select value_type base on lower bound, if lower is negtive then value_type is int, otherwise value_type is unsigned.
    typedef typename detail::select_integer_type<detail::is_negtive<lower>::yes >::value_type value_type;
    typedef value_type& reference;
    typedef value_type const_reference;
    typedef value_type int_type;

    Constrained_INTEGER(value_type val =0)
      : INTEGER(&theInfo)
    {   setValue(val);    }

    // Constrained_INTEGER(const ThisType& other) ; //use default copy constructor
    ThisType& operator = (const INTEGER& val) { value = val.getValue() ; assert(isValid()); return *this;}
    ThisType& operator = (int_type val) { value = val; assert(isValid()); return *this;}
    int_type getValue() const { return (value_type) value; }

    bool operator == (const ThisType& rhs) const { return getValue() == rhs.getValue(); }
    bool operator != (const ThisType& rhs) const { return getValue() != rhs.getValue(); }
    bool operator <  (const ThisType& rhs) const { return getValue() <  rhs.getValue(); }
    bool operator >  (const ThisType& rhs) const { return getValue() >  rhs.getValue(); }
    bool operator <= (const ThisType& rhs) const { return getValue() <= rhs.getValue(); }
    bool operator >= (const ThisType& rhs) const { return getValue() >= rhs.getValue(); }

#if __GNUC__> 2 || __GNUC_MINOR__ > 95
    friend bool operator == (int_type lhs, const ThisType& rhs) { return lhs == rhs.getValue(); }
    friend bool operator != (int_type lhs, const ThisType& rhs) { return lhs != rhs.getValue(); }
    friend bool operator <  (int_type lhs, const ThisType& rhs) { return lhs <  rhs.getValue(); }
    friend bool operator >  (int_type lhs, const ThisType& rhs) { return lhs >  rhs.getValue(); }
    friend bool operator <= (int_type lhs, const ThisType& rhs) { return lhs <= rhs.getValue(); }
    friend bool operator >= (int_type lhs, const ThisType& rhs) { return lhs >= rhs.getValue(); }
#endif

    bool operator == (int_type rhs) const { return getValue() == rhs; }
    bool operator != (int_type rhs) const { return getValue() != rhs; }
    bool operator <  (int_type rhs) const { return getValue() <  rhs; }
    bool operator >  (int_type rhs) const { return getValue() >  rhs; }
    bool operator <= (int_type rhs) const { return getValue() <= rhs; }
    bool operator >= (int_type rhs) const { return getValue() >= rhs; }

    ThisType& operator += (int_type val) { value += val; assert(isValid()); return *this; }
    ThisType& operator -= (int_type val) { value -= val; assert(isValid()); return *this; }
    ThisType& operator *= (int_type val) { value *= val; assert(isValid()); return *this; }
    ThisType& operator /= (int_type val) { value /= val; assert(isValid()); return *this; }
    ThisType& operator %= (int_type val) { value %= val; assert(isValid()); return *this; }

    ThisType& operator += (const ThisType& val) { value += val.getValue(); assert(isValid()); return *this; }
    ThisType& operator -= (const ThisType& val) { value -= val.getValue(); assert(isValid()); return *this; }
    ThisType& operator *= (const ThisType& val) { value *= val.getValue(); assert(isValid()); return *this; }
    ThisType& operator /= (const ThisType& val) { value /= val.getValue(); assert(isValid()); return *this; }
    ThisType& operator %= (const ThisType& val) { value %= val.getValue(); assert(isValid()); return *this; }

    ThisType& operator ++ () { ++ value  ; assert(isValid()); return *this;}
    ThisType  operator ++ (int) { ThisType result(getValue()); ++value; assert(isValid()); return result;}
    ThisType& operator -- () { -- value  ; assert(isValid()); return *this;}
    ThisType  operator -- (int){ ThisType result(getValue()); --value; assert(isValid()); return result;}

    ThisType* clone() const { return static_cast<ThisType*>(INTEGER::clone()); }
    void swap(ThisType& that) { ASN1_STD swap(value, that.value); }

    int_type operator + (const ThisType& rhs) const { int_type t(getValue()); return t+=rhs.getValue();}
    int_type operator - (const ThisType& rhs) const { int_type t(getValue()); return t-=rhs.getValue();}
    int_type operator * (const ThisType& rhs) const { int_type t(getValue()); return t*=rhs.getValue();}
    int_type operator / (const ThisType& rhs) const { int_type t(getValue()); return t/=rhs.getValue();}

    friend int_type operator + (int_type lhs, const ThisType& rhs) { int_type t(lhs); return t+=rhs.getValue();}
    friend int_type operator - (int_type lhs, const ThisType& rhs) { int_type t(lhs); return t-=rhs.getValue();}
    friend int_type operator * (int_type lhs, const ThisType& rhs) { int_type t(lhs); return t*=rhs.getValue();}
    friend int_type operator / (int_type lhs, const ThisType& rhs) { int_type t(lhs); return t/=rhs.getValue();}

    int_type operator + (int_type rhs) const { int_type t(getValue()); return t+=rhs;}
    int_type operator - (int_type rhs) const { int_type t(getValue()); return t-=rhs;}
    int_type operator * (int_type rhs) const { int_type t(getValue()); return t*=rhs;}
    int_type operator / (int_type rhs) const { int_type t(getValue()); return t/=rhs;}

    static const InfoType theInfo;
    static bool equal_type(const ASN1::AbstractData& type)
    {return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo);}

  };

  template <ConstraintType contraint, INT_TYPE lower, UINT_TYPE upper >
#if defined (__IBMCPP__)
  // I don't know, why the else-part doesn't work on VAC++5/AiX...
  const typename ConstrainedObject::InfoType Constrained_INTEGER<contraint, lower, upper>::theInfo =
#else
   const typename Constrained_INTEGER<contraint, lower, upper>::InfoType Constrained_INTEGER<contraint, lower, upper>::theInfo =
#endif //!defined (__IBMCPP__)
  {
    &INTEGER::create,
    2,
    0,
    contraint,
    lower,
    upper
  };


  /** Class for ASN Enumerated type.
   */
  class ASN1_API ENUMERATED : public AbstractData
  {
  protected:
    ENUMERATED(const void* info);
  public:
    ENUMERATED(const ENUMERATED& that);
    ENUMERATED& operator = (const ENUMERATED& that);

    // ENUMERATED specific methods
    int asInt() const { return value; }
    void setFromInt(int val) { value = val; }

    bool isValid() const { return extendable() || isStrictlyValid() ;}
    bool isStrictlyValid() const { return value <= getMaximum();}

    int getMaximum() const { return info()->maxEnumValue; }

    bool operator == (int rhs) const { return value == rhs; }
    bool operator != (int rhs) const { return value != rhs; }
    bool operator <  (int rhs) const { return value <  rhs; }
    bool operator >  (int rhs) const { return value >  rhs; }
    bool operator <= (int rhs) const { return value <= rhs; }
    bool operator >= (int rhs) const { return value >= rhs; }

    static AbstractData* create(const void* info);
    bool extendable() const { return info()->extendableFlag; }
  protected:
    struct InfoType
    {
      CreateFun create;
      unsigned tag;
      const void* parent_info;
      bool extendableFlag;
      unsigned maxEnumValue;
      const char** names;
    };

    void swap(ENUMERATED& other) { ASN1_STD swap(value, other.value); }
    int value;

  private:
    virtual INT_TYPE do_compare(const AbstractData& other) const;
    virtual AbstractData* do_clone() const ;
    virtual bool do_accept(Visitor&);
    virtual bool do_accept(ConstVisitor&) const;
    const InfoType* info() const { return static_cast<const InfoType*>(info_); }

  public:
    const char* getName() const;
    bool setFromName(const ASN1_STD string&);
    const char** names() { return info()->names; }
  };


  /** Class for ASN Object Identifier type.
   */
  class ASN1_API OBJECT_IDENTIFIER : public AbstractData
  {
  protected:
    OBJECT_IDENTIFIER(const void* info) : AbstractData(info) {}
  public:
    static const InfoType theInfo;

    OBJECT_IDENTIFIER() : AbstractData(&theInfo) {}

    template <class InputIterator>
    OBJECT_IDENTIFIER(InputIterator first, InputIterator last, const void* info = &theInfo)
      : AbstractData(info),value(first, last)
      { }

    OBJECT_IDENTIFIER(const OBJECT_IDENTIFIER & other);
    OBJECT_IDENTIFIER(unsigned nelem, .../*list of unsigned*/);
    OBJECT_IDENTIFIER & operator=(const OBJECT_IDENTIFIER & other)
    {
      // extensibility and tags are not to be assigned,
      // therefore the parent assignment operator is not called
      value = other.value;
      return *this;
    }


    OBJECT_IDENTIFIER * clone() const { return static_cast<OBJECT_IDENTIFIER*>(do_clone());}
    static AbstractData* create(const void* info);
    void swap(OBJECT_IDENTIFIER& that) { value.swap(that.value); }

    bool isValid() const { return value.size() != 0;}
    bool isStrictlyValid() const { return value.size() != 0;}

    unsigned levels() const { return (unsigned)value.size(); }
    const unsigned operator[](unsigned idx) const { return value[idx]; }
    void append(unsigned arcValue) { value.push_back(arcValue); }
    void trim(unsigned levels = 1) { value.erase(value.end()-levels, value.end()); }

    void assign(unsigned nelem, ... /*list of unsigned*/);
    void assign(unsigned nelem, va_list lst);

    template <class InputIterator>
    void assign(InputIterator first, InputIterator last)
    { value.assign(first, last); }

    bool decodeCommon(const char* data, unsigned dataLen);
    void encodeCommon(ASN1_STD vector<char> & eObjId) const;

    // comparison operators
    bool operator == (const OBJECT_IDENTIFIER& rhs) const { return value == rhs.value; }
    bool operator != (const OBJECT_IDENTIFIER& rhs) const { return value != rhs.value; }
    bool operator <  (const OBJECT_IDENTIFIER& rhs) const { return value <  rhs.value; }
    bool operator >  (const OBJECT_IDENTIFIER& rhs) const { return value >  rhs.value; }
    bool operator <= (const OBJECT_IDENTIFIER& rhs) const { return value <= rhs.value; }
    bool operator >= (const OBJECT_IDENTIFIER& rhs) const { return value >= rhs.value; }

    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }

  private:
    virtual INT_TYPE do_compare(const AbstractData& other) const;
    virtual AbstractData * do_clone() const;
    virtual bool do_accept(Visitor&);
    virtual bool do_accept(ConstVisitor&) const;
    ASN1_STD vector<unsigned> value;
  };


  /** Class for ASN Bit String type.
   */
  class ASN1_API BIT_STRING : public ConstrainedObject
  {
  protected:
    BIT_STRING(const void* info);
  public:
    BIT_STRING() : ConstrainedObject(&theInfo) {}
    BIT_STRING(const BIT_STRING & other);
    BIT_STRING & operator=(const BIT_STRING & other) {
      bitData = other.bitData;
      totalBits = other.totalBits;
      return *this;
    }

    bool isValid() const { return size() >= (unsigned)getLowerLimit() && (getConstraintType() != FixedConstraint || size() <= getUpperLimit()); }
    bool isStrictlyValid() const { return size() >= (unsigned)getLowerLimit() && size() <= getUpperLimit(); }

    BIT_STRING * clone() const { return static_cast<BIT_STRING*>(do_clone());}
    static AbstractData* create(const void* info);
    void swap(BIT_STRING& other) {
      bitData.swap(other.bitData);
      ASN1_STD swap(totalBits,other.totalBits);
    }

    unsigned size() const { return totalBits; }
    void resize(unsigned nBits) {
      bitData.resize((nBits+7)/8);
      totalBits = nBits;
    }


    bool operator[](unsigned bit) const {
      if ((unsigned)bit < totalBits)
        return (bitData[bit>>3] & (1 << (7 - (bit&7)))) != 0;
      return false;
    }

    void setData(unsigned nBits, const char * buf){
      int size = (nBits-1)/8 + 1;
      bitData.assign(buf,buf+size);
      totalBits = nBits;
    }

    const ASN1_STD vector<char>& getData() const{
      return bitData;
    }

    void set(unsigned bit) {
      if (bit < totalBits)
        bitData[(unsigned)(bit>>3)] |= 1 << (7 - (bit&7));
    }
    void clear(unsigned bit) {
      if (bit < totalBits)
        bitData[(unsigned)(bit>>3)] &= ~(1 << (7 - (bit&7)));
    }
    void invert(unsigned bit) {
      if (bit < totalBits)
        bitData[(unsigned)(bit>>3)] ^= 1 << (7 - (bit&7));
    }

    size_t getNamesSize() const { return info()->cntNames; }
    const char * const * getNamesList() const { return info()->names; }
    bool hasNames() const { return info()->names!=0; }
    const char * getName(unsigned num) const { return hasNames() && num<getNamesSize() ? getNamesList()[num] : 0; }

    bool operator == (const BIT_STRING& rhs) const { return compare(rhs) == 0; }
    bool operator != (const BIT_STRING& rhs) const { return compare(rhs) != 0; }
    bool operator <  (const BIT_STRING& rhs) const { return compare(rhs) <  0; }
    bool operator >  (const BIT_STRING& rhs) const { return compare(rhs) >  0; }
    bool operator <= (const BIT_STRING& rhs) const { return compare(rhs) <= 0; }
    bool operator >= (const BIT_STRING& rhs) const { return compare(rhs) >= 0; }

    struct InfoType
    {
      CreateFun create;
      unsigned tag;
      const void* parent_info;
      unsigned type;
      INT_TYPE lowerLimit;
      UINT_TYPE upperLimit;
      unsigned cntNames;
      const char** names;
    };
    const InfoType* info() const  { return static_cast<const InfoType*>(info_);}

    static bool equal_type(const ASN1::AbstractData& type)
    {return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo);}
    static const InfoType theInfo;

  private:
    friend class BERDecoder;
    friend class BERDecoderPartial;
    friend class PERDecoder;
    friend class AVNDecoder;
    virtual INT_TYPE do_compare(const AbstractData& other) const;
    virtual AbstractData* do_clone() const ;
    virtual bool do_accept(Visitor&);
    virtual bool do_accept(ConstVisitor&) const;

    unsigned totalBits;
    ASN1_STD vector<char> bitData;
  };

  struct EmptyConstraint {
    enum {
      constraint_type = Unconstrained,
      lower_bound = 0,
      upper_bound = UINT_MAX
    };
  };

  template <unsigned TYPE, int LOWERBOUND, unsigned UPPERBOUND>
  struct SizeConstraint
  {
    enum {
      constraint_type = TYPE,
      lower_bound = LOWERBOUND,
      upper_bound = UPPERBOUND
    };
  };

  template <class Constraint>
  class Constrained_BIT_STRING : public BIT_STRING
  {
    typedef BIT_STRING Inherited;
  protected:
    typedef Inherited::InfoType InfoType;
    Constrained_BIT_STRING(const void* info) : BIT_STRING(info) {}

  public:
    Constrained_BIT_STRING() : BIT_STRING(&theInfo) {}
//    Constrained_BIT_STRING(const Constrained_BIT_STRING & other);  // use default copy constructor

    Constrained_BIT_STRING & operator=(const BIT_STRING & other) {
      BIT_STRING::operator = (other);
      return *this;
    }

    Constrained_BIT_STRING * clone() const { return static_cast<Constrained_BIT_STRING*>(BIT_STRING::clone());}
    static const InfoType theInfo;
    static bool equal_type(const ASN1::AbstractData& type)
    { return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo); }
  };

  template <class Constraint>
  const typename Constrained_BIT_STRING<Constraint>::InfoType Constrained_BIT_STRING<Constraint>::theInfo = {
    BIT_STRING::create,
    3,
    0,
    Constraint::constraint_type,
    Constraint::lower_bound,
    Constraint::upper_bound
  };

  /** Class for ASN Octet String type.
   */
  class ASN1_API OCTET_STRING : public ConstrainedObject, public ASN1_STD vector<char>
  {
    typedef ASN1_STD vector<char> ContainerType;
  protected:
    OCTET_STRING(const void* info) : ConstrainedObject(info) {}
  public:
    typedef ContainerType::value_type value_type;
    typedef ContainerType::size_type size_type;
    typedef ContainerType::difference_type difference_type;
    typedef ContainerType::reference reference;
    typedef ContainerType::const_reference const_reference;

    OCTET_STRING() : ConstrainedObject(&theInfo) {}
    OCTET_STRING(size_type n, char v, const void* info = &theInfo)
       : ConstrainedObject(info), ContainerType(n, v) {}

    template <class Itr>
    OCTET_STRING(Itr first, Itr last)
       : ConstrainedObject(&theInfo),ContainerType(first, last) {}
    OCTET_STRING(const ASN1_STD vector<char>& other, const void* info = &theInfo)
       : ConstrainedObject(info), ContainerType(other) {}

    OCTET_STRING(const OCTET_STRING & other)
       : ConstrainedObject(other), ContainerType(other) {}

    OCTET_STRING & operator=(const OCTET_STRING & other)  {
      assign(other.begin(), other.end());
      return *this;
    }

    OCTET_STRING & operator = (const ASN1_STD vector<char>& other) {
      assign(other.begin(), other.end());
      return *this;
    }

    OCTET_STRING & operator=(const ASN1_STD string & str) {
      assign(str.begin(), str.end());
      return *this;
    }

    OCTET_STRING & operator=(const char* str) {
      assign(str, str+strlen(str));
      return *this;
    }

    bool isValid() const { return size() >= (unsigned)getLowerLimit() && (getConstraintType() != FixedConstraint || size() <= getUpperLimit());}
    bool isStrictlyValid() const { return size() >= (unsigned)getLowerLimit() && size() <= getUpperLimit();}

    OCTET_STRING * clone() const { return static_cast<OCTET_STRING*>(do_clone());}
    static AbstractData* create(const void* info);
    void swap(OCTET_STRING& other) { ContainerType::swap(other); }

    operator ASN1_STD string () const { return ASN1_STD string(begin(), end()); }

    static const InfoType theInfo;
    static bool equal_type(const ASN1::AbstractData& type)
    {return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo);}

  private:
    virtual INT_TYPE do_compare(const AbstractData& other) const;
    virtual AbstractData* do_clone() const ;
    virtual bool do_accept(Visitor&);
    virtual bool do_accept(ConstVisitor&) const;
  };

  template <class Constraint>
  class Constrained_OCTET_STRING : public OCTET_STRING
  {
    typedef OCTET_STRING Inherited;
  protected:
    typedef Inherited::InfoType InfoType;
  public:
    Constrained_OCTET_STRING(const void* info = &theInfo): OCTET_STRING(info) {}

    Constrained_OCTET_STRING(size_type n, char v) : OCTET_STRING(n, v, &theInfo)  { }

    template <class Itr>
    Constrained_OCTET_STRING(Itr first, Itr last) :
        OCTET_STRING(&theInfo)  { assign(first, last); }

    Constrained_OCTET_STRING(const ASN1_STD vector<char>& other) : OCTET_STRING(other, &theInfo) {}

//    Constrained_OCTET_STRING(const Constrained_OCTET_STRING & other) ;

    Constrained_OCTET_STRING & operator = (const ASN1_STD vector<char>& other) {
      assign(other.begin(), other.end());
      return *this;
    }

    Constrained_OCTET_STRING & operator=(const ASN1_STD string & str) {
      assign(str.data(), str.data() + str.size());
      return *this;
    }

    Constrained_OCTET_STRING & operator=(const char* str) {
      assign(str, str+strlen(str));
      return *this;
    }

    Constrained_OCTET_STRING * clone() const { return static_cast<Constrained_OCTET_STRING*>(OCTET_STRING::clone());}
    static AbstractData* create();
    void swap(Constrained_OCTET_STRING& other) { OCTET_STRING::swap(other); }

    static const InfoType theInfo;
    static bool equal_type(const ASN1::AbstractData& type)
    {return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo);}
  };

  template <class Constraint>
  const typename Constrained_OCTET_STRING<Constraint>::InfoType Constrained_OCTET_STRING<Constraint>::theInfo = {
    OCTET_STRING::create,
    4,
    0,
    Constraint::constraint_type,
    Constraint::lower_bound,
    Constraint::upper_bound
  };

  /** Base class for ASN String types.
   */
  class ASN1_API AbstractString : public ConstrainedObject, public ASN1_STD string
  {
  protected:
    typedef ASN1_STD string base_string;
  public:
    AbstractString(const AbstractString& other);
    typedef base_string::value_type value_type;
    typedef base_string::size_type size_type;
    typedef base_string::difference_type difference_type;
    typedef base_string::reference reference;
    typedef base_string::const_reference const_reference;


    AbstractString& operator=(const char * str) { assign(str);  return *this;}
    AbstractString& operator=(const ASN1_STD string & str) { assign(str);  return *this;}
    AbstractString& operator=(char c) { assign(1, c); return *this;}
    AbstractString& operator+=(const ASN1_STD string& rhs) { append(rhs);  return *this;}
    AbstractString& operator+=(const char *s) { append(s);  return *this;}
    AbstractString& operator+=(char c) { append(1, c);  return *this;}
    AbstractString& append(const ASN1_STD string& str) { base_string::append(str); return *this;}
    AbstractString& append(const ASN1_STD string& str, size_type pos, size_type n) { base_string::append(str,pos, n); return *this;}
    AbstractString& append(const char *s, size_type n) { base_string::append(s,n); return *this;}
    AbstractString& append(const char *s) { base_string::append(s); return *this;}
    AbstractString& append(size_type n, char c) { base_string::append(n,c); return *this;}
    AbstractString& append(const_iterator first, const_iterator last) { base_string::append(first, last); return *this;}
    AbstractString& assign(const base_string& str) { base_string::assign(str); return *this;}
    AbstractString& assign(const base_string& str,size_type pos, size_type n) { base_string::assign(str,pos,n); return *this;}
    AbstractString& assign(const char *s, size_type n) { base_string::assign(s,n); return *this;}
    AbstractString& assign(const char *s) { base_string::assign(s); return *this;}
    AbstractString& assign(size_type n, char c) { base_string::assign(n,c); return *this;}
    AbstractString& assign(const_iterator first, const_iterator last) { base_string::assign(first,last); return *this;}
    AbstractString& insert(size_type p0, const base_string& str) { base_string::insert(p0, str); return *this;}
    AbstractString& insert(size_type p0, const base_string& str, size_type pos, size_type n) { base_string::insert(p0, str, pos, n); return *this;}
    AbstractString& insert(size_type p0, const char *s, size_type n) { base_string::insert(p0, s,n); return *this;}
    AbstractString& insert(size_type p0, const char *s) { base_string::insert(p0, s); return *this;}
    AbstractString& insert(size_type p0, size_type n, char c) { base_string::insert(p0, n,c); return *this;}

    INT_TYPE compare(const AbstractString& other) const { return base_string::compare(other); }

    bool isValid() const;
    bool isStrictlyValid() const;
    size_type find_first_invalid() const {
      return info()->characterSetSize ? find_first_not_of(info()->characterSet) : ASN1_STD string::npos;
    }

    AbstractString*  clone() const { return static_cast<AbstractString*>(do_clone()); }

    friend ASN1_STD ostream& operator << (ASN1_STD ostream& os, const AbstractString& data)
    { return os << static_cast<const ASN1_STD string&>(data); }

    const char* getCharacterSet() const  { return info()->characterSet; }
    unsigned getCharacterSetSize() const { return info()->characterSetSize; }
    unsigned getCanonicalSetBits() const { return info()->canonicalSetBits; }
    unsigned getNumBits(bool align) const { return align ? info()->charSetAlignedBits : info()->charSetUnalignedBits; }

    static AbstractData* create(const void*);
  private:
    virtual INT_TYPE do_compare(const AbstractData& other) const;
    virtual AbstractData* do_clone() const ;
    virtual bool do_accept(Visitor&);
    virtual bool do_accept(ConstVisitor&) const;

  protected:
    AbstractString(const void* info);
    AbstractString(const void* info, const ASN1_STD string& str);
    AbstractString(const void* info, const char* str);

    struct InfoType
    {
      CreateFun create;
      unsigned tag;
      const void* parent_info;
      unsigned type;
      INT_TYPE lowerLimit;
      UINT_TYPE upperLimit;
      const char* characterSet;
      unsigned characterSetSize;
      unsigned canonicalSetBits;
      unsigned charSetUnalignedBits;
      unsigned charSetAlignedBits;
    };
    const InfoType* info() const { return static_cast<const InfoType*>(info_);}
  };

  class ASN1_API NumericString : public AbstractString {
  protected:
    NumericString(const void* info) : AbstractString(info) { }
  public:
    NumericString() : AbstractString(&theInfo) { }
    NumericString(const ASN1_STD string& str, const void* info = &theInfo) : AbstractString(info, str) { }
    NumericString(const char* str, const void* info = &theInfo) : AbstractString(info,str) { }

    NumericString& operator = (const NumericString& other) { assign(other); return *this;}
    NumericString& operator=(const char * str) { assign(str);  return *this;}
    NumericString& operator=(const ASN1_STD string & str) { assign(str);  return *this;}
    NumericString& operator=(char c) { assign(1, c); return *this;}

    NumericString * clone() const { return static_cast<NumericString *>(AbstractString::clone()); }
    static AbstractData* create();
    void swap(NumericString& other) { base_string::swap(other); }

    static const InfoType theInfo;
    static bool equal_type(const ASN1::AbstractData& type)
    {return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo);}
  };

  class ASN1_API PrintableString : public AbstractString {
  protected:
    PrintableString(const void* info) : AbstractString(info) { }
  public:
    PrintableString() : AbstractString(&theInfo) { }
    PrintableString(const ASN1_STD string& str, const void* info = &theInfo) : AbstractString(info, str) { }
    PrintableString(const char* str, const void* info = &theInfo) : AbstractString(info,str) { }

    PrintableString& operator = (const PrintableString& other) { assign(other); return *this;}
    PrintableString& operator=(const char * str) { assign(str);  return *this;}
    PrintableString& operator=(const ASN1_STD string & str) { assign(str);  return *this;}
    PrintableString& operator=(char c) { assign(1, c); return *this;}
    PrintableString * clone() const { return static_cast<PrintableString *>(AbstractString::clone()); }
    static AbstractData* create();
    void swap(PrintableString& other) { base_string::swap(other); }
    static const InfoType theInfo;
    static bool equal_type(const ASN1::AbstractData& type)
    {return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo);}
  };

  class ASN1_API GraphicString : public AbstractString {
  protected:
    GraphicString(const void* info) : AbstractString(info) { }
  public:
    GraphicString() : AbstractString(&theInfo) { }
    GraphicString(const ASN1_STD string& str, const void* info = &theInfo) : AbstractString(info, str) { }
    GraphicString(const char* str, const void* info = &theInfo) : AbstractString(info,str) { }

    GraphicString& operator = (const GraphicString& other) { assign(other); return *this;}
    GraphicString& operator=(const char * str) { assign(str);  return *this;}
    GraphicString& operator=(const ASN1_STD string & str) { assign(str);  return *this;}
    GraphicString& operator=(char c) { assign(1, c); return *this;}
    GraphicString * clone() const { return static_cast<GraphicString *>(AbstractString::clone()); }
    static AbstractData* create();
    void swap(GraphicString& other) { base_string::swap(other); }
    static const InfoType theInfo;
    static bool equal_type(const ASN1::AbstractData& type)
    {return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo);}
  };

  class ASN1_API ObjectDescriptor : public GraphicString
  {
  protected:
    ObjectDescriptor(const void* info) : GraphicString(info) { }
  public:
    ObjectDescriptor() : GraphicString() {}
    ObjectDescriptor(const ObjectDescriptor& that) : GraphicString(that) {}
    ObjectDescriptor& operator = (const ObjectDescriptor& that) { assign(that); return *this; }
    ObjectDescriptor(const ASN1_STD string& str, const void* info = &theInfo) : GraphicString(str, info) { }

    ObjectDescriptor& operator= (const ASN1_STD string& str) { assign(str); return *this; }
    //
    // Authorized conversions.
    // Provide cast to string (value).
    //
//    operator ASN1_STD string() const;
    //
    // AbstractData polymorphic behavior.
    // virtual - is implementation determined
    //
//    virtual bool isValid(const bool raise=false) const;
//    virtual bool isStrictlyValid(const bool raise=false) const;

#ifdef FIXME
	virtual void setFromValueNotation(const ASN1_STD string& valueString);
    virtual ASN1_STD string asValueNotation() const;
    virtual const class AbstractType* typeDynamic() const;
#endif
    //
    // AbstractData standard friend functions
    //
    friend ASN1_STD ostream& operator << (ASN1_STD ostream& os, const ObjectDescriptor& value);
    //
    // Standard operators. These operators must be provided by
    // some means which supports the context specified below.
    // They may be implemented as friend functions, member
    // functions, template functions, or by whatever means
    // will support ASN1::AbstractData values being able to appear in
    // the required contexts.
    //
    friend bool operator == (const ObjectDescriptor& valA, const ObjectDescriptor& valB);
    friend bool operator < (const ObjectDescriptor& valA, const ObjectDescriptor& valB);
    //
    // AbstractData standard derived class methods.
    //
#ifdef FIXME
	static const AbstractType* typeStatic();
#else
	static const AbstractData* typeStatic();
#endif
	//
    // Specialized access: same methods as ANSI Standard Library String.
    //
  }; // end class ASN1::ObjectDescriptor

  /**
   */
  class ASN1_API VisibleString : public AbstractString {
  protected:
    VisibleString(const void* info) : AbstractString(info) { }
  public:
    VisibleString() : AbstractString(&theInfo) { }
    VisibleString(const ASN1_STD string& str, const void* info = &theInfo) : AbstractString(info, str) { }
    VisibleString(const char* str, const void* info = &theInfo) : AbstractString(info,str) { }

    VisibleString& operator = (const VisibleString& other) { assign(other); return *this;}
    VisibleString& operator=(const char * str) { assign(str);  return *this;}
    VisibleString& operator=(const ASN1_STD string & str) { assign(str);  return *this;}
    VisibleString& operator=(char c) { assign(1, c); return *this;}
    VisibleString * clone() const { return static_cast<VisibleString *>(AbstractString::clone()); }
    static AbstractData* create();
    void swap(VisibleString& other) { base_string::swap(other); }

    static const InfoType theInfo;
    static bool equal_type(const ASN1::AbstractData& type)
    {return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo);}
  };

  class ASN1_API IA5String : public AbstractString {
  protected:
    IA5String(const void* info) : AbstractString(info) { }
  public:
    IA5String() : AbstractString(&theInfo) { }
    IA5String(const ASN1_STD string& str, const void* info = &theInfo) : AbstractString(info, str) { }
    IA5String(const char* str, const void* info = &theInfo) : AbstractString(info,str) { }

    IA5String& operator = (const IA5String& other) { assign(other); return *this;}
    IA5String& operator=(const char * str) { assign(str);  return *this;}
    IA5String& operator=(const ASN1_STD string & str) { assign(str);  return *this;}
    IA5String& operator=(char c) { assign(1, c); return *this;}
    IA5String * clone() const { return static_cast<IA5String *>(AbstractString::clone()); }
    static AbstractData* create();
    void swap(IA5String& other) { base_string::swap(other); }

    static const InfoType theInfo;
    static bool equal_type(const ASN1::AbstractData& type)
    {return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo);}
  };

  class ASN1_API GeneralString : public AbstractString {
  protected:
    GeneralString(const void* info) : AbstractString(info) { }
  public:
    GeneralString() : AbstractString(&theInfo) { }
    GeneralString(const ASN1_STD string& str, const void* info = &theInfo) : AbstractString(info, str) { }
    GeneralString(const char* str, const void* info = &theInfo) : AbstractString(info,str) { }

    GeneralString& operator = (const GeneralString& other) { assign(other); return *this;}
    GeneralString& operator=(const char * str) { assign(str);  return *this;}
    GeneralString& operator=(const ASN1_STD string & str) { assign(str);  return *this;}
    GeneralString& operator=(char c) { assign(1, c); return *this;}
    GeneralString * clone() const { return static_cast<GeneralString *>(AbstractString::clone()); }
    static AbstractData* create();
    void swap(GeneralString& other) { base_string::swap(other); }

    static const InfoType theInfo;
    static bool equal_type(const ASN1::AbstractData& type)
    {return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo);}
  };

  /**
   * Class for ASN BMP (16 bit) String type.
   */
  class ASN1_API BMPString : public ConstrainedObject, public ASN1_STD wstring
  {
  protected:
    typedef ASN1_STD wstring base_string;
    BMPString(const void* info);
    struct InfoType
    {
      CreateFun create;
      unsigned tag;
      const void* parent_info;
      unsigned type;
      INT_TYPE lowerLimit;
      UINT_TYPE upperLimit;
      wchar_t firstChar, lastChar;
      unsigned charSetUnalignedBits;
      unsigned charSetAlignedBits;
    };
  public:
    typedef base_string::value_type value_type;
    typedef base_string::size_type size_type;
    typedef base_string::difference_type difference_type;
    typedef base_string::reference reference;
    typedef base_string::const_reference const_reference;

    BMPString();
    BMPString(const base_string& str, const void* info = &theInfo);
    BMPString(const value_type* str, const void* info = &theInfo);
    BMPString(const BMPString & other);
    BMPString & operator=(const value_type * str) { return assign(str);}
    BMPString & operator=(const base_string & str) { return  assign(str);}
    BMPString & operator=(value_type c) { return  assign(1,c);}
    BMPString& operator+=(const base_string& rhs) { return append(rhs);}
    BMPString& operator+=(const value_type *s) { return append(s);}
    BMPString& operator+=(value_type c) { return append(1, c);}
    BMPString& append(const base_string& str) { base_string::append(str); return *this;}
    BMPString& append(const base_string& str, size_type pos, size_type n) { base_string::append(str,pos, n); return *this;}
    BMPString& append(const value_type *s, size_type n) { base_string::append(s,n); return *this;}
    BMPString& append(const value_type *s) { base_string::append(s); return *this;}
    BMPString& append(size_type n, value_type c) { base_string::append(n,c); return *this;}
    BMPString& append(const_iterator first, const_iterator last) { base_string::append(first, last); return *this;}
    BMPString& assign(const base_string& str) { base_string::assign(str); return *this;}
    BMPString& assign(const base_string& str,size_type pos, size_type n) { base_string::assign(str,pos,n); return *this;}
    BMPString& assign(const value_type *s, size_type n) { base_string::assign(s,n); return *this;}
    BMPString& assign(const value_type *s) { base_string::assign(s); return *this;}
    BMPString& assign(size_type n, value_type c) { base_string::assign(n,c); return *this;}
    BMPString& assign(const_iterator first, const_iterator last) { base_string::assign(first,last); return *this;}
    BMPString& insert(size_type p0, const base_string& str) { base_string::insert(p0, str); return *this;}
    BMPString& insert(size_type p0, const base_string& str, size_type pos, size_type n) { base_string::insert(p0,str,pos,n); return *this;}
    BMPString& insert(size_type p0, const value_type *s, size_type n) { base_string::insert(p0,s,n); return *this;}
    BMPString& insert(size_type p0, const value_type *s) { base_string::insert(p0,s); return *this;}
    BMPString& insert(size_type p0, size_type n, value_type c) { base_string::insert(p0,n,c); return *this;}

    INT_TYPE compare(const BMPString& other) const { return base_string::compare(other); }

    BMPString * clone() const { return static_cast<BMPString *>(do_clone()); }
    static AbstractData* create(const void* info);
    void swap(BMPString& other) { base_string::swap(other); }

    bool isValid() const;
    bool isStrictlyValid() const;

    friend ASN1_STD ostream& operator << (ASN1_STD ostream& os, const BMPString& data)
    { return os << static_cast<const AbstractData&>(data); }

    wchar_t getFirstChar() const { return info()->firstChar; }
    wchar_t getLastChar() const { return info()->lastChar; }
    unsigned getNumBits(bool align) const {
      return align ? info()->charSetAlignedBits : info()->charSetUnalignedBits;
    }

    static const InfoType theInfo;
    static bool equal_type(const ASN1::AbstractData& type)
    {return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo);}
    size_type first_illegal_at() const;

  private:
    bool legalCharacter(wchar_t ch) const;
    const InfoType* info() const { return static_cast<const InfoType*>(info_);}

    INT_TYPE do_compare(const AbstractData& other) const;
    virtual AbstractData * do_clone() const;
    virtual bool do_accept(Visitor&);
    virtual bool do_accept(ConstVisitor&) const;
  };

  /**
   */
  class ASN1_API UTCTime : public AbstractData
  {
  protected:
    UTCTime(const void* info);
  public:
    UTCTime();
    UTCTime(const char* value);
    UTCTime(int year, int month, int day,
        int hour = 0, int minute=0, int second=0,
        int mindiff = 0, bool utc = false);

    UTCTime(const UTCTime& other) ;
    UTCTime& operator = (const UTCTime& other ) ;

    /**
     * Returns the character string format of this object. Unlike asValueNototation(),
     * the string returned by get() does not contain double quote marks (").
     */
    ASN1_STD string get() const;
    /**
     * Set the value of this object using character string format. Unlike fromValueNototation(),
     * the string used by set() shall not contain double quote marks (").
     */
    void set(const char*);

    time_t get_time_t() const;
    void set_time_t(time_t gmt);

    int get_year() const { return year; }
    int get_month() const { return month; }
    int get_day() const { return day; }
    int get_hour() const { return hour; }
    int get_minute() const { return minute;}
    int get_second() const { return second; }
    int get_mindiff() const { return mindiff; }
    bool get_utc() const { return utc;}

    void set_year(int yr) { year = yr; }
    void set_month(int mon) { month = mon; }
    void set_day(int dy) { day = dy; }
    void set_hour(int hr) { hour = hr; }
    void set_minute(int min) { minute = min;}
    void set_second(int sec) { second = sec; }
    void set_mindiff(int mdiff) { utc = false; mindiff = mdiff; }
    void set_utc(bool tc =true) { if(tc) mindiff = 0; utc = tc; }

    void swap(UTCTime& other) ;
    UTCTime* clone() const { return static_cast<UTCTime*>(do_clone()); }

    bool isValid() const { return isStrictlyValid();}
    static AbstractData* create(const void* info);
    bool isStrictlyValid() const;

    static const InfoType theInfo;
    static bool equal_type(const ASN1::AbstractData& type)
    {return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo);}

  private:
    virtual INT_TYPE do_compare(const AbstractData& other) const;
    virtual AbstractData* do_clone() const ;
    virtual bool do_accept(Visitor&);
    virtual bool do_accept(ConstVisitor&) const;

  protected:
    int year, month, day, hour, minute, second, mindiff;
    bool utc;
  };

  class ASN1_API GeneralizedTime : public UTCTime
  {
  public:
  protected:
    GeneralizedTime(const void* info);
  public:
    GeneralizedTime();
    GeneralizedTime(const char* value);
    GeneralizedTime(int year, int month, int day,
        int hour = 0, int minute=0, int second=0,
        int millisec = 0, int mindiff = 0, bool utc = false);

    GeneralizedTime(const GeneralizedTime& other) ;
    GeneralizedTime& operator = (const GeneralizedTime& other ) ;

    /**
     * Returns the character string format of this object. Unlike asValueNototation(),
     * the string returned by get() does not contain double quote marks (").
     */
    ASN1_STD string get() const;
    /**
     * Set the value of this object using character string format. Unlike fromValueNototation(),
     * the string used by set() shall not contain double quote marks (").
     */
    void set(const char*);

    time_t get_time_t() const;
    void set_time_t(time_t gmt);

    int get_millisec() const { return millisec; }
    void set_millisec(int milsec) { millisec = milsec ;}

    void swap(GeneralizedTime& other) ;
    GeneralizedTime* clone() const { return static_cast<GeneralizedTime*>(do_clone()); }

    bool isValid() const { return isStrictlyValid();}
    static AbstractData* create(const void* info);
    bool isStrictlyValid() const;

    static const InfoType theInfo;
    static bool equal_type(const ASN1::AbstractData& type)
    {return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo);}

  private:
    virtual INT_TYPE do_compare(const AbstractData& other) const;
    virtual AbstractData* do_clone() const ;
    virtual bool do_accept(Visitor&);
    virtual bool do_accept(ConstVisitor&) const;

    int millisec;
  }; // end class ASN1::GeneralizedTime

  //
  // AbstractData polymorphic behavior.

  /** Class for ASN Choice type.
   */
  class ASN1_API CHOICE : public AbstractData
  {
  public:
    enum Id {
      unknownSelection_ = -2,
      unselected_ = -1
    };

    /**
     * Returns the index of the currently selected alternatives.
     */
    int currentSelection() const { return choiceID; }
    bool isSelected(int sel) const { return choiceID == sel; }
    bool isUnknownSelection() const { return choiceID == unknownSelection_; }
    bool isUnselected() const { return choiceID == unselected_; }

    bool isValid() const;
    bool isStrictlyValid() const;

    /**
     * Returns the object of the currently selected alternatives.
     */
    AbstractData * getSelection() { return choice.get(); }
    const AbstractData * getSelection() const { return choice.get(); }
    /**
     * Set the value by alternative index number.
     */
    bool select(int selection) {
      if (choiceID != selection)
      {
        choiceID = selection;
        return createSelection();
      }
      return true;
    }

    /**
     * Set the value by index number and the pointer to the selected alternative.
     *
     * This member funtion will take the ownership of the object pointed by \c obj.
     *
     * @param id The index number of the selected alternative.
     * @param obj The pointer to the selected alternative.
     */
    AbstractData* setSelection(int id, AbstractData* obj);

    /**
     * Returns the number of alternatives in the extension root.
     */
    unsigned getNumChoices() const { return info()->numChoices; }
    bool extendable() const { return info()->extendableFlag; }
    unsigned getSelectionTag() const {
      assert(choiceID >= 0);
      return info()->tags == 0
        ? (ContextSpecificTagClass<<30) | choiceID
        : ( info()->tags[choiceID]
          ? info()->tags[choiceID]
          : static_cast<const InfoType*>(info()->selectionInfos[choiceID])->tag );
    }

    unsigned getSelectionTagInternal() const {
      assert(choiceID >= 0);
      return info()->tags == 0
        ? (ContextSpecificTagClass<<30) | choiceID
        : info()->tags[choiceID];
    }
    bool isSelectionExplicit() const;
    unsigned getSelectionInfoTag() const;

    static AbstractData* create(const void* info);

  protected:
    CHOICE(const void* info , int id = -1, AbstractData* value= 0);
    ~CHOICE();

    CHOICE(const CHOICE & other);
    CHOICE & operator=(const CHOICE & other);
    void swap(CHOICE& other);

    asn_ptr<AbstractData> choice;
    int choiceID;

    struct InfoType
    {
      CreateFun create;
      unsigned tag;
      const void* parent_info;
      bool extendableFlag;
      const void** selectionInfos;
      unsigned numChoices;
      unsigned totalChoices;
      unsigned* tags;
      const char** names;
    };
  private:
    virtual INT_TYPE do_compare(const AbstractData& other) const;
    virtual bool do_accept(Visitor&);
    virtual bool do_accept(ConstVisitor&) const;
    virtual AbstractData* do_clone() const;
    bool createSelection();
    const InfoType* info() const { return static_cast<const InfoType*>(info_);}
  public:
    /**
     * Set the value by \c tag number and \c tag class.
     */
    bool setID(unsigned tagNum, unsigned tagClass);

  public:
    const char* getSelectionName() const { assert(choiceID < static_cast<int>(info()->totalChoices)); return info()->names[choiceID]; }
    friend class AVNDecoder;
  };

  class PEREncoder;
  class PERDecoder;

  /** Class for ASN Sequence type.
   */
  class ASN1_API SEQUENCE : public AbstractData
  {
  public:
    enum {
      AUTOMATIC_TAG,
      EXPLICIT_TAG,
      IMPLICIT_TAG
    };

    SEQUENCE(const SEQUENCE & other);
    ~SEQUENCE();
    SEQUENCE & operator=(const SEQUENCE & other);
    SEQUENCE * clone() const { return static_cast<SEQUENCE*>(do_clone()); }
    void swap(SEQUENCE& other);

    /**
     *  Returns the pointer to the component of the SEQUENCE at position \c pos.
     */
    AbstractData* getField(unsigned pos) {
      assert(pos < fields.size());
      return fields[pos];
    }
    const AbstractData* getField(unsigned pos) const {
      assert(pos < fields.size());
      return fields[pos];
    }

    unsigned getFieldTag(unsigned pos) const {
      assert(pos < fields.size());
      if(info()->tags == 0) // Automatic Tags
        return unsigned(ContextSpecificTagClass<<30) |  pos;

#ifndef old_code
      return (info()->tags != static_cast<const void*>(&defaultTag)) ? info()->tags[pos] : 0;
#else
      return static_cast<const AbstractData::InfoType*>(info()->fieldInfos[pos])->tag;
#endif
    }

    bool extendable() const { return info()->extendableFlag; }
    /**
     * Makes an OPTIONAL field present.
     *
     * @param opt The index to the OPTIONAL field
     * @param pos The position of the OPTIONAL field
     */
    void includeOptionalField(unsigned opt, unsigned pos);
    /**
     * Tests the presence of an OPTIONAL field.
     *
     * @param opt The index to the OPTIONAL field
     */
    bool hasOptionalField(unsigned opt) const;
    /**
     * Makes an OPTIONAL field absence.
     *
     * @param opt The index to the OPTIONAL field
     */
    void removeOptionalField(unsigned opt);

    static AbstractData* create(const void*);

    class ASN1_API FieldVector : public ASN1_STD vector<AbstractData*>
    {
    public:
      FieldVector(){};
      ~FieldVector();
      FieldVector(const FieldVector& other);
    private:
      FieldVector& operator = (const FieldVector& other);
    };

  protected:
    SEQUENCE(const void* info);

    enum
    {
      mandatory_ = -1
    };
    struct ASN1_API BitMap
    {
      BitMap() : totalBits(0) {}
      unsigned size() const { return totalBits;}
      void resize(unsigned nBits);
      bool operator [] (unsigned bit) const;
      void set(unsigned bit);
      void clear(unsigned bit);
      void swap(BitMap& other);
      unsigned totalBits;
      ASN1_STD vector<char> bitData;
    };

    FieldVector fields;
    BitMap optionMap;
    BitMap extensionMap;

    static const unsigned defaultTag;

    struct InfoType
    {
      CreateFun create;
      unsigned tag;
      const void* parent_info;
      bool extendableFlag;
      const void** fieldInfos;
      int* ids;
      unsigned numFields;
      unsigned knownExtensions;
      unsigned numOptional;
      const char* nonOptionalExtensions;
      const unsigned* tags;
      const char** names;
      const char* typeName;
    };

  private:
    friend class Visitor;
    friend class ConstVisitor;
    friend class PEREncoder;
    friend class PERDecoder;

    virtual INT_TYPE do_compare(const AbstractData& other) const;
    virtual AbstractData* do_clone() const ;
    virtual bool do_accept(ConstVisitor&) const;

    const InfoType* info() const { return static_cast<const InfoType*>(info_);}
  protected:
    virtual bool do_accept(Visitor&);
  public:
    const char* getFieldName(int i) const { return info()->names[i]; }
    virtual ASN1_STD string getTypeName() const { return info()->typeName; }
  };

  /** Class for ASN set type.
   */
  class ASN1_API SET : public SEQUENCE
  {
  public:
    SET * clone() const { return static_cast<SET*>(SEQUENCE::clone()); }
  protected:
    SET(const void* info) : SEQUENCE(info) {  }
  };

  /** Class for ASN SEQUENCE type.
   */
  class ASN1_API SEQUENCE_OF_Base : public ConstrainedObject
  {
  public:
    typedef ASN1_STD vector<AbstractData*> Container;

  protected:
    SEQUENCE_OF_Base(const void*);
  public:
    ~SEQUENCE_OF_Base() { clear();}
    SEQUENCE_OF_Base(const SEQUENCE_OF_Base & other);
    SEQUENCE_OF_Base & operator=(const SEQUENCE_OF_Base & other);

    typedef Container::iterator iterator;
    typedef Container::const_iterator const_iterator;

    iterator begin()  { return container.begin(); }
    iterator end()  { return container.end(); }
    const_iterator begin() const { return container.begin(); }
    const_iterator end() const { return container.end(); }
    void resize(Container::size_type size);
    void push_back(AbstractData* obj) { container.push_back(obj);}
    void erase(iterator first, iterator last);
    Container::size_type  size() const { return container.size(); }
    Container::size_type  max_size() const { return container.max_size(); }
    Container::size_type  capacity() const { return container.capacity(); }
    void reserve(Container::size_type n) { container.reserve(n); }
    bool empty() const { return container.empty(); }
    void clear();

    bool isValid() const;
    bool isStrictlyValid() const;

    AbstractData * createElement() const {
      const AbstractData::InfoType* elementInfo =
        static_cast<const AbstractData::InfoType*>(static_cast<const InfoType*>(info_)->elementInfo);
      return elementInfo->create(elementInfo);
    }

    static AbstractData* create(const void* info);
    struct InfoType
    {
      CreateFun create;
      unsigned tag;
      const void* parent_info;
      unsigned type;
      INT_TYPE lowerLimit;
      UINT_TYPE upperLimit;
      const void* elementInfo;
      const char* typeName;
    };
    virtual ASN1_STD string getTypeName() const
    {
      const char* name = static_cast<const InfoType*>(info_)->typeName;
      return name ? ASN1_STD string(name) : AbstractData::getTypeName();
    }

  protected:
    void insert(iterator position, Container::size_type n, const AbstractData& x);
    void insert(iterator position, const_iterator first, const_iterator last);

    Container container;
    virtual AbstractData* do_clone() const;

    struct create_from_ptr
    {
      AbstractData* operator() (const AbstractData* obj) { return obj->clone(); }
    };

  private:
    virtual INT_TYPE do_compare(const AbstractData& other) const;
    virtual bool do_accept(Visitor&);
    virtual bool do_accept(ConstVisitor&) const;

    struct create_from0
    {
      const AbstractData& object;
      create_from0(const AbstractData& obj) : object(obj){}
      AbstractData* operator ()() const { return object.clone(); }
    };
  };

  template <class T, class Constraint = EmptyConstraint>
  class SEQUENCE_OF : public SEQUENCE_OF_Base
  {
    typedef SEQUENCE_OF_Base Inherited;
  protected:
    typedef Inherited::InfoType InfoType;
  public:
    typedef T&                          reference;
    typedef const T&                    const_reference;
    typedef T                           value_type;
    typedef value_type*                 pointer;
    typedef const value_type*           const_pointer;
    typedef Container::size_type        size_type;
    typedef Container::difference_type  difference_type;

#if defined(HP_aCC_RW)
    typedef ASN1_STD random_access_iterator<value_type> my_iterator_traits;
    typedef ASN1_STD random_access_iterator<const value_type> my_const_iterator_traits;
#else
    typedef ASN1_STD iterator<ASN1_STD random_access_iterator_tag, value_type> my_iterator_traits;
    typedef ASN1_STD iterator<ASN1_STD random_access_iterator_tag, const value_type> my_const_iterator_traits;
#endif

  public:
    class iterator : public my_iterator_traits
    {
    public:
      typedef T           value_type;
      typedef T&          reference;
      typedef value_type* pointer;
      typedef Container::difference_type difference_type;

      iterator() {}
      iterator(Container::iterator i) : itsIter(i) {}
      Container::iterator base() const { return itsIter; }

      reference operator*() const {return *static_cast<pointer>(*itsIter);}
      pointer operator->() const {return static_cast<pointer>(*itsIter);}
      reference operator[](difference_type n) const {return *static_cast<pointer>(*itsIter[n]);}

      iterator& operator++()          {++itsIter; return *this;}
      iterator& operator--()          {--itsIter; return *this;}
      iterator operator++(int)        {iterator t(*this); ++itsIter; return t;}
      iterator operator--(int)        {iterator t(*this); --itsIter; return t;}
      iterator& operator+=(difference_type n)     {itsIter+=n; return *this;}
      iterator& operator-=(difference_type n)     {itsIter-=n; return *this;}
      iterator operator+(difference_type n) const {return iterator(itsIter+n);}
      iterator operator-(difference_type n) const {return iterator(itsIter-n);}
//      friend iterator operator + (difference_type n, const iterator& it) { return it+n; }
      difference_type operator - (const iterator& rhs) const { return base() - rhs.base(); }

      bool operator==(const iterator& r) const    {return itsIter == r.itsIter;}
      bool operator!=(const iterator& r) const    {return itsIter != r.itsIter;}
      bool operator<(const iterator& r) const     {return itsIter < r.itsIter;}
    private:
      Container::iterator itsIter;
    };

    class const_iterator : public my_const_iterator_traits
    {
    public:
      typedef T                  value_type;
      typedef const T&           reference;
      typedef const value_type*  pointer;
      typedef Container::difference_type  difference_type;

      const_iterator() {}
      const_iterator(Container::const_iterator i) : itsIter(i) {}
      const_iterator(Container::iterator i) : itsIter(i) {}
      const_iterator(typename SEQUENCE_OF<T>::iterator i) : itsIter(i.base()) {}
//      const_iterator(iterator i) : itsIter(i.base()) {}
      Container::const_iterator base() const {return itsIter;}

      reference operator*() const {return *static_cast<pointer>(*itsIter);}
      pointer operator->() const {return static_cast<pointer>(*itsIter);}
      reference operator[](difference_type n) const {return *static_cast<pointer>(*itsIter[n]);}

      const_iterator& operator++()          {++itsIter; return *this;}
      const_iterator& operator--()          {--itsIter; return *this;}
      const_iterator  operator++(int)        {const_iterator t(*this); ++itsIter; return t;}
      const_iterator  operator--(int)        {const_iterator t(*this); --itsIter; return t;}
      const_iterator& operator+=(difference_type n)     {itsIter+=n; return *this;}
      const_iterator& operator-=(difference_type n)     {itsIter-=n; return *this;}
      const_iterator  operator+(difference_type n) const {return const_iterator(itsIter+n);}
      const_iterator  operator-(difference_type n) const {return const_iterator(itsIter-n);}

//      friend const_iterator  operator + (difference_type n, const const_iterator& it) { return it+n; }
      difference_type operator - (const const_iterator& rhs) const { return base() - rhs.base(); }

      bool operator==(const const_iterator& r) const    {return itsIter == r.itsIter;}
      bool operator!=(const const_iterator& r) const    {return itsIter != r.itsIter;}
      bool operator< (const const_iterator& r) const    {return itsIter < r.itsIter;}
    private:
      Container::const_iterator itsIter;
    };

    class reverse_iterator : public my_iterator_traits
    {
    typedef typename SEQUENCE_OF<T>::iterator base_iterator;
    public:
      typedef T   value_type;
      typedef T&    reference;
      typedef value_type* pointer;
      typedef Container::difference_type  difference_type;

      reverse_iterator() {}
      reverse_iterator(base_iterator i) : itsIter(i) {}
      iterator base() const {return itsIter;}

      reference operator*() const { return *(itsIter-1); }
      pointer operator->() const  { return &(**this); }
      reference operator[](difference_type n) const {return *((*this)+n);}

      reverse_iterator& operator++()          {--itsIter; return *this;}
      reverse_iterator& operator--()          {++itsIter; return *this;}
      reverse_iterator operator++(int)        {reverse_iterator t(*this); --itsIter; return t;}
      reverse_iterator operator--(int)        {reverse_iterator t(*this); ++itsIter; return t;}
      reverse_iterator& operator+=(difference_type n)     {itsIter-=n; return *this;}
      reverse_iterator& operator-=(difference_type n)     {itsIter+=n; return *this;}
      reverse_iterator operator+(difference_type n) const {return reverse_iterator(itsIter-n);}
      reverse_iterator operator-(difference_type n) const {return reverse_iterator(itsIter+n);}
//      friend reverse_iterator operator + (difference_type n, const reverse_iterator& it) { return it-n; }
      difference_type operator - (const reverse_iterator& rhs) const { return rhs.base() - base(); }

      bool operator==(const reverse_iterator& r) const    {return itsIter == r.itsIter;}
      bool operator!=(const reverse_iterator& r) const    {return itsIter != r.itsIter;}
      bool operator<(const reverse_iterator& r) const     {return itsIter > r.itsIter;}
    private:
      base_iterator itsIter;
    };

    class const_reverse_iterator : public my_const_iterator_traits
    {
    public:
      typedef const T value_type;
      typedef const T&  reference;
      typedef const value_type* pointer;
      typedef Container::difference_type  difference_type;

      const_reverse_iterator() {}
      const_reverse_iterator(Container::reverse_iterator i) : itsIter(i.base()) {}
      const_reverse_iterator(Container::const_iterator i) : itsIter(i) {}
      const_iterator base() const {return itsIter;}

      reference operator*() const { return *(itsIter-1); }
      pointer operator->() const  { return &(**this); }
      reference operator[](difference_type n) const {return *((*this)+n);}

      const_reverse_iterator& operator++()          {--itsIter; return *this;}
      const_reverse_iterator& operator--()          {++itsIter; return *this;}
      const_reverse_iterator operator++(int)        {const_reverse_iterator t(*this); --itsIter; return t;}
      const_reverse_iterator operator--(int)        {const_reverse_iterator t(*this); ++itsIter; return t;}
      const_reverse_iterator& operator+=(difference_type n)     {itsIter-n; return *this;}
      const_reverse_iterator& operator-=(difference_type n)     {itsIter+=n; return *this;}
      const_reverse_iterator operator+(difference_type n) const {return const_reverse_iterator(itsIter-n);}
      const_reverse_iterator operator-(difference_type n) const {return const_reverse_iterator(itsIter+n);}
//      friend const_reverse_iterator operator + (difference_type n, const const_reverse_iterator& it) { return it-n; }
      difference_type operator - (const const_reverse_iterator& rhs) const { return rhs.base() - base(); }

      bool operator==(const const_reverse_iterator& r) const    {return itsIter == r.itsIter;}
      bool operator!=(const const_reverse_iterator& r) const    {return itsIter != r.itsIter;}
      bool operator< (const const_reverse_iterator& r) const    {return itsIter >  r.itsIter;}
    private:
      Container::const_iterator itsIter;
    };

    SEQUENCE_OF(const void* info = &theInfo) : SEQUENCE_OF_Base(info) {}
    SEQUENCE_OF(size_type n, const_reference val = T())
      : SEQUENCE_OF_Base(&theInfo)
    {
      insert(begin(),n, val);
    }

    template <class InputIterator>
    SEQUENCE_OF(InputIterator first, InputIterator last)
      : SEQUENCE_OF_Base(&theInfo)
    {
      insert(begin(), first, last);
    }

    SEQUENCE_OF(const SEQUENCE_OF<T, Constraint>& other) : SEQUENCE_OF_Base(other) {}

    SEQUENCE_OF<T, Constraint>& operator = (const SEQUENCE_OF<T, Constraint>& x)
    {
      SEQUENCE_OF<T, Constraint> temp(x.begin(), x.end());
      swap(temp);
      return *this;
    }
    void assign(size_type n)
    {
      SEQUENCE_OF<T, Constraint> temp(n);
      swap(temp);
    }
    void assign(size_type n, const T& value)
    {
      SEQUENCE_OF<T, Constraint> temp(n, value);
      swap(temp);
    }

    template <class InputIterator>
    void assign(InputIterator first, InputIterator last)
    {
      clear();
      insert(begin(), first, last);
    }
    iterator                begin() { return iterator(container.begin());   }
    const_iterator          begin() const { return const_iterator(container.begin());}
    iterator                end() { return iterator(container.end());}
    const_iterator          end() const { return const_iterator(container.end());}

    reverse_iterator        rbegin() { return reverse_iterator(end());}
    const_reverse_iterator  rbegin() const { return const_reverse_iterator(end());}
    reverse_iterator        rend()   { return reverse_iterator(begin());}
    const_reverse_iterator  rend()   const { return const_reverse_iterator(begin());}

    void resize(size_type sz) { Inherited::resize(sz); }

    void resize(size_type sz, const T& c ) {
      if (sz < size()) container.resize(sz, c);
      else insert(end(), sz-size(), c);
    }
    reference         operator[] (size_type n) { return *static_cast<T*>(container.operator[](n));}
    const_reference   operator[] (size_type n)  const{ return *static_cast<const T*>(container.operator[](n));}
    reference         at(size_type n) { return *static_cast<T*>(container.at(n));}
    const_reference   at(size_type n) const { return *static_cast<const T*>(container.at(n));}
    reference         front() { return *static_cast<T*>(container.front());}
    const_reference   front() const { return *static_cast<const T*>(container.front());}
    reference         back() { return *static_cast<T*>(container.back());}
    const_reference   back() const { return *static_cast<const T*>(container.back());}
    void push_back(const T& x) { container.push_back( x.clone() );}
    /**
     * Takes the ownership of the object pointed by \c x,
     * and insert it to the back of this object.
     */
    void push_back(pointer x) { container.push_back(x);}
    void pop_back() { clean(--end()); container.pop_back();}
#ifdef has_push_front
    void push_front(const T& x) { container.push_front(x.clone());}
    /**
     * Takes the ownership of the object pointed by \c x,
     * and insert it to the front of this object.
     */
    void push_front(pointer x) { container.push_front(x);}
    void pop_front() { clean(begin()); container.pop_front();}
#endif
    iterator  insert(iterator position, const T& x) { return iterator(container.insert(position.base(), x.clone()));}
    /**
     * Takes the ownership of the object pointed by \c x,
     * and insert it before the element pointed by \c position.
     */
    iterator    insert(iterator position, pointer x) { return iterator(container.insert(position.base(), x));}
    void insert(iterator position, size_type n, const T& x) { SEQUENCE_OF_Base::insert(position.base(), n, x);}
//#if !defined(_MSC_VER)
//    void insert(iterator position, const_iterator first, const_iterator last)
//    {
//      SEQUENCE_OF_Base::insert(position.base(), first.base(), last.base());
//    }
//#endif
    template <class InputIterator>
    void insert(iterator position, InputIterator first, InputIterator last) {

      // Both VC6.0 SP4 and GNU C 2.95.2 has problem with finding the  operator - (const_iterator, const_iterator)
      //   used by ASN1_STD distance()
      ASN1_STD transform(first, last, ASN1_STD inserter(container, position.base()), create_from1());
//      for (; first != last; ++first) {
//        container.push_back((*first)->clone());
//      }
    }
    iterator  erase(iterator position) { clean(position); return iterator(container.erase(position.base()));}
    iterator  erase(iterator first, iterator last) { clean(first, last); return iterator(container.erase(first.base(), last.base()));}
    void    swap(SEQUENCE_OF<T, Constraint>& x) { container.swap(x.container);}
    /**
     * Removes the last element from this object, and returns the ownership of the removed element
     * to the caller.
     */
    pointer release_back() { pointer x = static_cast<pointer>(container.back()); container.pop_back(); return x;}
#ifdef has_push_front
    /**
     * Removes the first element from this object, and returns the ownership of the removed element
     * to the caller.
     */
    pointer release_front() { pointer x = static_cast<pointer>(container.front()); container.pop_front(); return x;}
#endif
    /**
     * Removes the element pointed by \c position from this object, and returns the ownership of the removed element
     * to the caller.
     */
    pointer release(iterator position) {
      pointer result = static_cast<pointer>(*(position.base()));
      container.erase(position.base());
      return result;
    }
    /**
     * Removes the elements in [first, last) from this object; furthermore, the elements been removed would not be
     * deleted. It is the caller's responsibility to obtain the ownerships of those elements before this operation
     * is executed.
     */
    void release(iterator first, iterator last) {
      container.erase(first.base(), last.base());
    }
    SEQUENCE_OF<T, Constraint>* clone() const { return static_cast<SEQUENCE_OF<T, Constraint>*>(do_clone()); }

    void remove(const_reference x)
    { remove_if([&x](const T& y) { return y == x; });  }

    template <class Predicate>
    void remove_if(Predicate pred)
    {
      Container::iterator k = ASN1_STD stable_partition(begin(), end(), [&pred](AbstractData* x) { return !pred(*static_cast<pointer>(x)); });
      erase(k, end());
    }

    void unique()
    { unique_if(ASN1_STD equal_to<T>()); }

    template <class BinPred>
    void unique_if(BinPred pred)
    {
      if (size() < 2) return;
      BinaryPredicate<BinPred> pr(pred);
      iterator k, first, last = end();
      k = first = begin();
      while (++first != last)
        if (!pr(*k, *first))
          if (++k != first)
            ASN1_STD swap(*first, *k);
      erase(iterator(++k), end());
    }

    void sort() { sort(ASN1_STD less<T>());}

    template <class Compare>
    void sort(Compare comp)
    { ASN1_STD sort(begin(), end(), ComparePredicate<Compare>(comp)); }

    void reverse() { ASN1_STD reverse(container.begin(), container.end());}

    static const InfoType theInfo;
    static bool equal_type(const ASN1::AbstractData& type)
    {return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo);}

  private:
    void clean(iterator i)  { delete &*i; }
    void clean(iterator first, iterator last)      { while (first != last) clean(first++); }

    struct create_from1
    {
      AbstractData* operator() (const T& obj) const { return new T(obj); }
    };

    template <class OriginalPredicate>
    struct UnaryPredicate {
      UnaryPredicate(OriginalPredicate& o) : pred(o){}
      bool operator() (AbstractData* x) const { return pred(*static_cast<pointer>(x));}
      OriginalPredicate& pred;
    };

    template <class OriginalPredicate>
    struct BinaryPredicate {
      BinaryPredicate(OriginalPredicate& o) : pred(o){}
      bool operator() (AbstractData* x, AbstractData* y) const { return pred(*static_cast<pointer>(x), *static_cast<pointer>(y));}
      OriginalPredicate& pred;
    };

    template <class OriginalPredicate>
    struct ComparePredicate {
      ComparePredicate(OriginalPredicate& o) : pred(o){}
      int operator() (AbstractData* x, AbstractData* y) const { return pred(*static_cast<pointer>(x), *static_cast<pointer>(y));}
      OriginalPredicate& pred;
    };
  };

  template <class T, class Constraint>
	const typename SEQUENCE_OF<T, Constraint>::InfoType SEQUENCE_OF<T, Constraint>::theInfo = {
    SEQUENCE_OF_Base::create,
    0x10,
    0,
    Constraint::constraint_type,
    Constraint::lower_bound,
    Constraint::upper_bound,
    &T::theInfo
  };

  template <class T, class Constraint>
  inline typename SEQUENCE_OF<T, Constraint>::iterator operator +
      (typename SEQUENCE_OF<T, Constraint>::difference_type i,
       const typename SEQUENCE_OF<T, Constraint>::iterator& it)
  {
    return it+i;
  }

  template <class T, class Constraint>
  inline typename SEQUENCE_OF<T, Constraint>::const_iterator operator +
      (typename SEQUENCE_OF<T, Constraint>::difference_type i,
       const typename SEQUENCE_OF<T, Constraint>::const_iterator& it)
  {
    return it+i;
  }

  template <class T, class Constraint>
  inline typename SEQUENCE_OF<T, Constraint>::reverse_iterator operator +
      (typename SEQUENCE_OF<T, Constraint>::difference_type i,
       const typename SEQUENCE_OF<T, Constraint>::reverse_iterator& it)
  {
    return it+i;
  }

  template <class T, class Constraint>
  inline typename SEQUENCE_OF<T, Constraint>::const_reverse_iterator operator +
      (typename SEQUENCE_OF<T, Constraint>::difference_type i,
       const typename SEQUENCE_OF<T, Constraint>::const_reverse_iterator& it)
  {
    return it+i;
  }

  template <class T, class Constraint = EmptyConstraint>
  class SET_OF : public SEQUENCE_OF<T, Constraint>
  {
    typedef SEQUENCE_OF<T, Constraint>  Inherited;

  public:
    typedef typename Inherited::size_type size_type;
    typedef typename Inherited::const_reference const_reference;

  protected:
    SET_OF(const void* info) : Inherited(info){  }
    typedef typename Inherited::InfoType InfoType;

  public:
    SET_OF() {  }
    SET_OF(size_type n, const_reference val = T()) : Inherited(&theInfo)
    {   insert(Inherited::begin(),n, val);     }

    template <class InputIterator>
    SET_OF(InputIterator first, InputIterator last) : Inherited(&theInfo)
    {   this->insert(Inherited::begin(), first, last);  }

    SET_OF(const SET_OF<T, Constraint>& other) : Inherited(other) {}
    SET_OF<T, Constraint>& operator = (const SET_OF<T, Constraint>& x)  {
      SET_OF<T, Constraint> temp(x.begin(), x.end());
      Inherited::swap(temp);
      return *this;
    }
    SET_OF<T, Constraint>* clone() const { return static_cast<SET_OF<T, Constraint>*>(Inherited::clone()); }

    static const InfoType theInfo;
    static bool equal_type(const ASN1::AbstractData& type)
    {return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo);}
  };

  template <class T, class Constraint>
  const typename SET_OF<T, Constraint>::InfoType SET_OF<T, Constraint>::theInfo = {
    SEQUENCE_OF_Base::create,
    0x11,
    0,
    Constraint::constraint_type,
    Constraint::lower_bound,
    Constraint::upper_bound,
    &T::theInfo
  };

  typedef ASN1_STD vector<char> OpenBuf;

  class ASN1_API OpenData : public AbstractData
  {
  public:
    typedef AbstractData data_type;

    OpenData(const void* info = &theInfo) : AbstractData(info){}
    OpenData(AbstractData* pData,const void* info = &theInfo)
      : AbstractData(info), data(pData) {}
    OpenData(OpenBuf* pBuf)
      :  AbstractData(&theInfo),buf(pBuf) {}

    OpenData(const AbstractData& aData, const void* info = &theInfo)
      : AbstractData(info), data(aData.clone()) {}
    OpenData(const OpenBuf& aBuf)
      :  AbstractData(&theInfo),buf( new OpenBuf(aBuf)) {}

    OpenData(const OpenData& that);

    OpenData& operator = (const OpenData& that) { OpenData tmp(that); swap(tmp); return *this; }
    OpenData& operator = (const AbstractData& aData) { data.reset(aData.clone()); return *this; }
    OpenData& operator = (const OpenBuf& aBuf) { buf.reset(new OpenBuf(aBuf)); return *this; }

    void grab(AbstractData* aData) { data.reset(aData); }
    void grab(OpenBuf* aBuf) { buf.reset(aBuf); }

    AbstractData* release_data() { return data.release();}
    OpenBuf* release_buf() { return buf.release();}

    bool isEmpty() const { return !has_data() && !has_buf(); }
    bool has_data() const {return data.get() != 0;}
    bool has_buf() const { return buf.get() != 0; }

    AbstractData& get_data() { return *data;}
    const AbstractData& get_data() const { return *data;}
    OpenBuf& get_buf() { return *buf; }
    const OpenBuf& get_buf() const { return *buf; }

    bool isValid() const;
    bool isStrictlyValid() const;

    OpenData* clone() const { return static_cast<OpenData*>(do_clone()); }
    static AbstractData* create(const void* info);
    void swap(OpenData& other);

    bool operator == (const OpenData& rhs) const { return do_compare(rhs) == 0; }
    bool operator != (const OpenData& rhs) const { return do_compare(rhs) != 0; }
    bool operator <  (const OpenData& rhs) const { return do_compare(rhs) <  0; }
    bool operator >  (const OpenData& rhs) const { return do_compare(rhs) >  0; }
    bool operator <= (const OpenData& rhs) const { return do_compare(rhs) <= 0; }
    bool operator >= (const OpenData& rhs) const { return do_compare(rhs) >= 0; }

    static const InfoType theInfo;
    static bool equal_type(const ASN1::AbstractData& type)
    {return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo);}

  protected:
    asn_ptr<AbstractData> data;
    asn_ptr<OpenBuf> buf;
  private:
    virtual INT_TYPE do_compare(const AbstractData& other) const;
    virtual AbstractData* do_clone() const ;
    virtual bool do_accept(ConstVisitor&) const;
    virtual bool do_accept(Visitor&);
  };

  class ASN1_API TypeConstrainedOpenData : public OpenData
  {
  private:
    virtual bool do_accept(Visitor& v);
    // adding dummy to satisfy compilers (cannot use using - private members)
    virtual bool do_accept(ConstVisitor&) const { return false; }
  protected:
    TypeConstrainedOpenData(AbstractData* pData, const void* info) :  OpenData(pData, info) {}
    TypeConstrainedOpenData(const AbstractData& aData, const void* info) : OpenData(aData, info) {}
    static AbstractData* create(const void* info);

    struct InfoType
    {
      CreateFun create;
      unsigned tag;
      const void* parent_info;
      const void* typeInfo;
    };
  };

  template <class T>
  class Constrained_OpenData : public TypeConstrainedOpenData
  {
    typedef T data_type;
    typedef TypeConstrainedOpenData Inherited;
  protected:
    typedef Inherited::InfoType InfoType;
  public:
    Constrained_OpenData(const void* info = &TypeConstrainedOpenData::theInfo)
      : TypeConstrainedOpenData(T::create(&T::theInfo), info)
    {
      assert(static_cast<const InfoType*>(info_)->typeInfo != info);
    }
    Constrained_OpenData(const T& t)
      : TypeConstrainedOpenData(t, &TypeConstrainedOpenData::theInfo){}

    Constrained_OpenData(const Constrained_OpenData<T>& that): TypeConstrainedOpenData(that){}

    Constrained_OpenData<T>& operator = (const data_type& aData) { data.reset(aData.clone()); return *this; }
    Constrained_OpenData<T>& operator = (const Constrained_OpenData<T>& that) { Constrained_OpenData<T> tmp(that);  swap(tmp); return *this; }
    Constrained_OpenData<T>& operator = (const OpenBuf& aBuf) { buf.reset(new OpenBuf(aBuf)); return *this;}

    void grab(T* aData) { data.reset(aData); }
    T& get_data() { return static_cast<T&>(OpenData::get_data()); }
    const T& get_data() const { return static_cast<const T&>(OpenData::get_data());}

    Constrained_OpenData<T>* clone() const { return static_cast<Constrained_OpenData<T>*>(OpenData::clone()); }
    void swap(Constrained_OpenData<T>& other) { OpenData::swap(other); }

    static const InfoType theInfo;
    static bool equal_type(const ASN1::AbstractData& type)
    {return type.info() == reinterpret_cast<const ASN1::AbstractData::InfoType*>(&theInfo);}

  private:
    const InfoType* info() const { return static_cast<const InfoType*>(info_); }
  };

  template <class T>
  const typename Constrained_OpenData<T>::InfoType Constrained_OpenData<T>::theInfo = {
    TypeConstrainedOpenData::create,
    0,
    0,
    &T::theInfo
  };

  //////////////////////////////////////////////////////////////////////////////
  class CoderEnv;

  /**
   * Visitor is base class for defining a new mutable operation for ASN.1 types without
   * changing the classes of the ASN.1 types it operates.
   *
   * This class is the role "Visitor" in the Visitor design pattern (Gamma et al. 1995).
   * It allows us to add a new operation to the ASN.1 types without changing the classes
   * of the ASN.1 types; furthermore, it avoids linking the object code of the new operation
   * when it is not used, which is why this library can be so small and lean.
   *
   * This class \c Visitor differs from the class \c ConstVistor in that a \c Visitor operates on mutable
   * objects; in other words, it will change the value of the ASN.1 objects it operates on. For
   * example, \c BERDecoder and \c PERDecoder ,which both inherit from \c Visitor, are used to decode
   * a byte stream into a ASN.1 value; that is to say, they modify the ASN.1 objects they visit to
   * some specific values.
   *
   * All classes inherit from shall override the do_visit() member functions, which are called
   * by their corresponding visit() member functions. The do_visit() member functions are all
   * declared private for they should not be called by their derivatives.
   *
   * @see BERDecoder, PERDecoder, AVNDecoder
   */

  class ASN1_API Visitor
  {
  public:
    virtual ~Visitor(){}
    bool visit(Null& value) { return do_visit(value); }
    bool visit(BOOLEAN& value) { return do_visit(value); }
    bool visit(INTEGER& value) { return do_visit(value); }
    bool visit(IntegerWithNamedNumber& value) { return do_visit(value); }
    bool visit(ENUMERATED& value) { return do_visit(value); }
    bool visit(OBJECT_IDENTIFIER& value) { return do_visit(value); }
    bool visit(OCTET_STRING& value)  { return do_visit(value); }
    bool visit(BIT_STRING& value) { return do_visit(value); }
    bool visit(AbstractString& value) { return do_visit(value); }
    bool visit(BMPString& value) { return do_visit(value); }
    bool visit(CHOICE& value) { return do_visit(value); }
    bool visit(SEQUENCE_OF_Base& value) { return do_visit(value); }
    bool visit(OpenData& value) { return do_visit(value); }
    bool revisit(OpenData& value) { return do_revisit(value); }
    bool visit(TypeConstrainedOpenData& value) { return do_visit(value); }
    bool visit(UTCTime& value) { return do_visit(value); }
    bool visit(GeneralizedTime& value) { return do_visit(value); }
    bool visit(SEQUENCE& value) ;

    CoderEnv* get_env() { return env;}

    enum VISIT_SEQ_RESULT
    {
      FAIL,
      STOP,
      NO_EXTENSION,
      CONTINUE
    };
  protected:
    Visitor() {}
    Visitor(CoderEnv* coder) : env(coder) {}

  protected:
    virtual bool do_visit(Null& value)=0;
    virtual bool do_visit(BOOLEAN& value)=0;
    virtual bool do_visit(INTEGER& value)=0;
    virtual bool do_visit(IntegerWithNamedNumber& value) { return visit(static_cast<INTEGER&>(value));}
    virtual bool do_visit(ENUMERATED& value)=0;
    virtual bool do_visit(OBJECT_IDENTIFIER& value)=0;
    virtual bool do_visit(OCTET_STRING& value) =0;
    virtual bool do_visit(BIT_STRING& value)=0;
    virtual bool do_visit(AbstractString& value)=0;
    virtual bool do_visit(BMPString& value)=0;
    virtual bool do_visit(CHOICE& value)=0;
    virtual bool do_visit(SEQUENCE_OF_Base& value)=0;
    virtual bool do_visit(OpenData& value)=0;
    virtual bool do_revisit(OpenData& value)=0;
    virtual bool do_visit(TypeConstrainedOpenData& value)=0;
    virtual bool do_visit(UTCTime& value)=0;
    virtual bool do_visit(GeneralizedTime& value)=0;

    /**
     * Called by \c visit() before visiting any component in the \c SEQUENCE.
     *
     * @param value The \c SEQUENCE been visited.
     * @return If the return value is FAIL, it indicates the operation is fail and stop visiting
     *         subsequent components; otherwise, it means the operation is successful. However,
     *         different return values indicate how the sebsequent components are traversed. STOP means
     *         no sebsequent components should be visited; NO_EXTENSION means only extension roots
     *         should be visited; CONTINUE means all components should be visited.
     */
    virtual VISIT_SEQ_RESULT preVisitExtensionRoots(SEQUENCE& value) { return CONTINUE; }
    /**
     * Called by \c visit() while visiting a component in the extension root of
     * the \c SEQUENCE.
     *
     * @param value The \c SEQUENCE been visited.
     * @param pos The position of the component in the SEQUENCE.
     * @param optional_id The optional index of the component in the SEQUENCE, the value is
     *        -1 when it is a mandatory component.
     * @return If the return value is FAIL, it indicates the operation is fail and stop visiting
     *         subsequent components; otherwise, it means the operation is successful. However,
     *         different return values indicate how the sebsequent components are traversed. STOP means
     *         no sebsequent components should be visited; CONTINUE means all components should be visited.
     */
    virtual VISIT_SEQ_RESULT visitExtensionRoot(SEQUENCE& value, int pos, int optional_id) { return FAIL;}
    /**
     * Called by \c visit() before visiting any extension component of the \c SEQUENCE.
     *
     * @param value The \c SEQUENCE been visited.
     * @return If the return value is FAIL, it indicates the operation is fail and stop visiting
     *         subsequent components; otherwise, it means the operation is successful. However,
     *         different return values indicate how the sebsequent components are traversed. STOP means
     *         no sebsequent components should be visited; CONTINUE means all components should be visited.
     */
    virtual VISIT_SEQ_RESULT preVisitExtensions(SEQUENCE& value) { return CONTINUE;}
    /**
     * Called by \c visit() when visiting a component which locates after the extension mark of
     * a \c SEQUENCE.
     *
     * @param value The \c SEQUENCE been visited.
     * @param pos The position of the component in the SEQUENCE.
     * @param optional_id The optional index of the component in the SEQUENCE, the value is
     *        -1 when it is a madatory component.
     * @return If the return value is FAIL, it indicates the operation is fail and stop visiting
     *         subsequent components; otherwise, it means the operation is successful. However,
     *         different return values indicate how the sebsequent components are traversed. STOP means
     *         no sebsequent components should be visited; CONTINUE means all components should be visited.
     */
    virtual VISIT_SEQ_RESULT visitKnownExtension(SEQUENCE& value, int pos, int optional_id){ return FAIL;}
    /**
     * Called by \c visit() after visiting all known extension components of the \c SEQUENCE.
     *
     * @param value The \c SEQUENCE been visited.
     * @return true if the operation is successful.
     */
    virtual bool visitUnknownExtensions(SEQUENCE& value) { return true;}
    CoderEnv* env;
  };

  /**
   * ConstVisitor is base class for defining a new non-mutable operation for ASN.1 types without
   * changing the classes of the ASN.1 types it operates.
   *
   * This class is the role "Visitor" in the Visitor design pattern (Gamma et al. 1995).
   * It allows us to add a new operation to the ASN.1 types without changing the classes
   * of the ASN.1 types; furthermore, it avoids linking the object code of the new operation
   * when it is not used, which is why this library can be so small and lean.
   *
   * This class \c ConstVistor differs from the class \c Visitor in that a \c ConstVistor operates
   * on non-mutable objects; in other words, it cannot change the value of the ASN.1 objects it
   * operates on. For example, \c BEREncoder and \c PEREncoder ,which both inherit from
   * \c ConstVisitor, are used to encode an ASN.1 value into a byte stream; that is to say, the
   * \c BEREncoder and PEREncoder do not modify the ASN.1 objects they visit.
   *
   * All classes inherit from shall override the do_visit() member functions, which are called
   * by their corresponding visit() member functions. The do_visit() member functions are all
   * declared private for they should not be called by their derivatives.
   *
   *@see BEREncoder, PEREncoder, AVNEncoder
   */
  class ASN1_API ConstVisitor
  {
  public:
    virtual ~ConstVisitor(){}
    bool visit(const Null& value) { return do_visit(value); }
    bool visit(const BOOLEAN& value) { return do_visit(value);}
    bool visit(const INTEGER& value) { return do_visit(value); }
    bool visit(const IntegerWithNamedNumber& value) { return do_visit(value);}
    bool visit(const ENUMERATED& value) { return do_visit(value); }
    bool visit(const OBJECT_IDENTIFIER& value) { return do_visit(value); }
    bool visit(const BIT_STRING& value) { return do_visit(value); }
    bool visit(const OCTET_STRING& value) { return do_visit(value); }
    bool visit(const AbstractString& value) { return do_visit(value); }
    bool visit(const BMPString& value) { return do_visit(value); }
    bool visit(const CHOICE& value) { return do_visit(value); }
    bool visit(const OpenData& value) { return do_visit(value); }
    bool visit(const UTCTime& value) { return do_visit(value); }
    bool visit(const GeneralizedTime& value) { return do_visit(value); }
    bool visit(const SEQUENCE_OF_Base& value) {return do_visit(value); }
    bool visit(const SEQUENCE& value) ;
  protected:
    virtual bool do_visit(const AbstractData& value) { return true; }
    virtual bool do_visit(const Null& value) { return do_visit(static_cast<const AbstractData&>(value)); }
    virtual bool do_visit(const BOOLEAN& value) { return do_visit(static_cast<const AbstractData&>(value)); }
    virtual bool do_visit(const INTEGER& value) { return do_visit(static_cast<const AbstractData&>(value)); }
    virtual bool do_visit(const IntegerWithNamedNumber& value) { return do_visit(static_cast<const INTEGER&>(value));}
    virtual bool do_visit(const ENUMERATED& value) { return do_visit(static_cast<const AbstractData&>(value)); }
    virtual bool do_visit(const OBJECT_IDENTIFIER& value) { return do_visit(static_cast<const AbstractData&>(value)); }
    virtual bool do_visit(const BIT_STRING& value) { return do_visit(static_cast<const AbstractData&>(value)); }
    virtual bool do_visit(const OCTET_STRING& value) { return do_visit(static_cast<const AbstractData&>(value)); }
    virtual bool do_visit(const AbstractString& value) { return do_visit(static_cast<const AbstractData&>(value)); }
    virtual bool do_visit(const BMPString& value) { return do_visit(static_cast<const AbstractData&>(value)); }
    virtual bool do_visit(const CHOICE& value) { return do_visit(static_cast<const AbstractData&>(value)); }
    virtual bool do_visit(const OpenData& value) { return do_visit(static_cast<const AbstractData&>(value)); }
    virtual bool do_visit(const UTCTime& value) { return do_visit(static_cast<const AbstractData&>(value)); }
    virtual bool do_visit(const GeneralizedTime& value) { return do_visit(static_cast<const AbstractData&>(value)); }
    virtual bool do_visit(const SEQUENCE_OF_Base& value) { return do_visit(static_cast<const AbstractData&>(value)); }

    virtual bool preVisitExtensionRoots(const SEQUENCE& value) { return do_visit(static_cast<const AbstractData&>(value)); }
    virtual bool visitExtensionRoot(const SEQUENCE& value, int index) { return false; }
    virtual bool preVisitExtensions(const SEQUENCE& ) { return true;}
    virtual bool visitKnownExtension(const SEQUENCE& value, int index) { return false; }
    virtual bool afterVisitSequence(const SEQUENCE&) { return true;}
  };

  /////////////////////////////////////////////////////////////////////////////////////

  /**
   * This class is used to encode an ASN.1 object into a byte stream using Basic Encoding Rule (BER),
   * defined in ITU X.690.
   *
   * The following example shows how to use this class.
   * \code
   *    ASN1::INGEGER obj(5);
   *    ASN1::OpenBuf buf; // the encoded buffer
   *    ASN1::BEREncoder encoder(buf);
   *
   *    obj.accept(encoder); // now buf contains the encoded BER stream.
   * \endcode
   *
   * @see BERDecoder
   */
  class ASN1_API BEREncoder : public ConstVisitor
  {
  public:
    /**
     * Constructor.
     *
     * @param buf The buffer used to holds the encoded BER stream.
     */
    BEREncoder(OpenBuf& buf) : encodedBuffer(buf), tag(InvalidTag)
    {
      encodedBuffer.clear();
      encodedBuffer.reserve(256);
    }

    void encodeHeader(const AbstractData & obj);
    void encodeTag(unsigned tag);
    void encodeLength(unsigned len);

  private:
    using ConstVisitor::do_visit;
    virtual bool do_visit(const Null& value);
    virtual bool do_visit(const BOOLEAN& value);
    virtual bool do_visit(const INTEGER& value);
    virtual bool do_visit(const ENUMERATED& value);
    virtual bool do_visit(const OBJECT_IDENTIFIER& value);
    virtual bool do_visit(const BIT_STRING& value);
    virtual bool do_visit(const OCTET_STRING& value);
    virtual bool do_visit(const AbstractString& value);
    virtual bool do_visit(const BMPString& value);
    virtual bool do_visit(const CHOICE& value);
    virtual bool do_visit(const SEQUENCE_OF_Base& value);
    virtual bool do_visit(const OpenData& value);
    virtual bool do_visit(const UTCTime& value);
    virtual bool do_visit(const GeneralizedTime& value);

    virtual bool preVisitExtensionRoots(const SEQUENCE& value) ;
    virtual bool visitExtensionRoot(const SEQUENCE& value, int index);
    virtual bool visitKnownExtension(const SEQUENCE& value, int index);

    void encodeByte(unsigned value);
    void encodeBlock(const char * bufptr, unsigned nBytes);
    OpenBuf& encodedBuffer;
    unsigned tag;
  };

  /**
   * This class is used to decode a byte stream encoded using Basic Encoding Rule (BER) back
   * to an ASN.1 object.
   *
   * The following example shows how to use this class.
   * \code
   *    ASN1::INGEGER obj;
   *    ASN1::BERDecoder decoder(buf, buf+5);
   *
   *    obj.accept(decoder); // now obj holds the value which the buf represents.
   * \endcode
   *
   * @see BEREncoder
   */
  class ASN1_API BERDecoder  : public Visitor
  {
  public:
    /**
     * Constructor
     *
     * @param first The start position of the encoded BER stream.
     * @param last  The end position of the encoded BER stream.
     * @param coder The CoderEnv object which the decoder used to find the information objects
     *  it needs to decode the ASN.1 object. If this parameter is not NULL, the decoder will
     *  decode the open type based on the information objects which are inserted to the CoderEnv
     *  objects.
     */
    BERDecoder(const char* first, size_t size, CoderEnv* coder = 0);
    ~BERDecoder();

    typedef const char* memento_type;
    memento_type get_memento() const { return beginPosition; }
    void rollback(memento_type memento) { beginPosition = memento;}
    bool decodeChoicePreamle(CHOICE& value, memento_type& nextPostion);

    bool decodeHeader(unsigned & tag, bool & primitive, unsigned & len);
    bool decodeHeader(AbstractData & obj, unsigned & len);

    virtual VISIT_SEQ_RESULT preVisitExtensionRoots(SEQUENCE& value);
    virtual VISIT_SEQ_RESULT visitExtensionRoot(SEQUENCE& value, int index, int optional_id);
    virtual VISIT_SEQ_RESULT visitKnownExtension(SEQUENCE& value, int index, int optional_id);
    virtual bool visitUnknownExtensions(SEQUENCE& value);

  private:
    using Visitor::do_visit;
    virtual bool do_visit(Null& value);
    virtual bool do_visit(BOOLEAN& value);
    virtual bool do_visit(INTEGER& value);
    virtual bool do_visit(ENUMERATED& value);
    virtual bool do_visit(OBJECT_IDENTIFIER& value);
    virtual bool do_visit(BIT_STRING& value);
    virtual bool do_visit(OCTET_STRING& value);
    virtual bool do_visit(AbstractString& value);
    virtual bool do_visit(BMPString& value);
    virtual bool do_visit(CHOICE& value);
    virtual bool do_visit(SEQUENCE_OF_Base& value);
    virtual bool do_visit(OpenData& value);
    virtual bool do_revisit(OpenData& value);
    virtual bool do_visit(TypeConstrainedOpenData& value);
    virtual bool do_visit(UTCTime& value);
    virtual bool do_visit(GeneralizedTime& value);

    bool atEnd();
    unsigned char decodeByte();
    unsigned decodeBlock(char * bufptr, unsigned nBytes);

    const char* beginPosition;
    const char* endPosition;
    ASN1_STD vector<const char*> endSEQUENCEPositions;
    int dontCheckTag;

    class BER_Element;
    BER_Element *m_parsed;
    bool decode(BER_Element&, memento_type end_pos);

  private:
    BERDecoder(const BERDecoder &);
    BERDecoder &operator=(const BERDecoder &);
  };

  /**
   * This class is used to encode an ASN.1 object into a byte stream using Packed Encoding Rule (PER),
   * defined in ITU X.691.
   *
   * The following example shows how to use this class.
   * \code
   *    ASN1::INGEGER obj(5);
   *    ASN1::OpenBuf buf; // the encoded buffer
   *    ASN1::PEREncoder encoder(buf);
   *
   *    obj.accept(encoder); // now buf contains the encoded PER stream.
   * \endcode
   *
   * @see PERDecoder
   */
  class ASN1_API PEREncoder : public ConstVisitor
  {
  public:
    /**
     * Constructor
     *
     * @param buf The buffer used to holds the encoded PER stream.
     * @param isAligned Indicates whether using the aligned PER.
     *
     * @warning The unaligned PER has never been fully tested in current version.
     */
    PEREncoder(OpenBuf& buf, bool isAligned = true)
      : encodedBuffer(buf)
        , bitOffset (8)
        , alignedFlag(isAligned)
    {
      encodedBuffer.clear();
      encodedBuffer.reserve(256);
    }


    /**
     * Returns true if using the aligned PER.
     */
    bool aligned() const { return alignedFlag; }

  private:
    using ConstVisitor::do_visit;
    virtual bool do_visit(const Null& value);
    virtual bool do_visit(const BOOLEAN& value);
    virtual bool do_visit(const INTEGER& value);
    virtual bool do_visit(const ENUMERATED& value);
    virtual bool do_visit(const OBJECT_IDENTIFIER& value);
    virtual bool do_visit(const BIT_STRING& value);
    virtual bool do_visit(const OCTET_STRING& value);
    virtual bool do_visit(const AbstractString& value);
    virtual bool do_visit(const BMPString& value);
    virtual bool do_visit(const CHOICE& value);
    virtual bool do_visit(const OpenData& value);
    virtual bool do_visit(const UTCTime& value);
    virtual bool do_visit(const GeneralizedTime& value);
    virtual bool do_visit(const SEQUENCE_OF_Base& value);

    virtual bool preVisitExtensionRoots(const SEQUENCE& value) ;
    virtual bool visitExtensionRoot(const SEQUENCE& value, int index);
    virtual bool preVisitExtensions(const SEQUENCE& value) ;
    virtual bool visitKnownExtension(const SEQUENCE& value, int index);

    void encodeBitMap(const ASN1_STD vector<char>& bitData, unsigned nBits);

    template <typename uns_int>
    void encodeMultiBit(uns_int value, unsigned nBits)
    {
      if(!nBits) return;

      if(bitOffset == 8) encodedBuffer.push_back(0);

      // Make sure value is in bounds of bit available.
      if(nBits < sizeof(int)*8)
        value &= ((1 << nBits) - 1);

      if(nBits < bitOffset) {
        bitOffset -= nBits;
        encodedBuffer.back() |= value << bitOffset;
        return;
      }

      nBits -= bitOffset;
      encodedBuffer.back() |= (char)(value >> nBits);
      bitOffset = 8;

      while(nBits >= 8) {
        nBits -= 8;
        encodedBuffer.push_back(value >> nBits);
      }

      if(nBits > 0) {
        bitOffset = 8 - nBits;
        encodedBuffer.push_back((value & ((1 << nBits)-1)) << bitOffset);
      }
    }

    bool encodeConstrainedLength(const ConstrainedObject & obj, unsigned length) ;
    bool encodeConstraint(const ConstrainedObject & obj, UINT_TYPE value) ;
    void encodeSingleBit(bool value);
    void encodeSmallUnsigned(unsigned value);
    bool encodeLength(unsigned len, unsigned lower, unsigned upper);

    template <typename uns_int>
    bool encodeUnsigned(uns_int value, uns_int lower, uns_int upper)
    {
      if(value < lower || value > upper)
        return false;

      // X.691 section 10.5
      if(lower == upper) // 10.5.4
         return true;

      uns_int range = (upper - lower) + 1;
      unsigned nBits = CountBits(range);
      uns_int adjusted_value = value - lower;

      if (alignedFlag && (range == 0 || range > 255)) { // not 10.5.6 and not 10.5.7.1
        if (nBits > 16) {                           // not 10.5.7.4
          int numBytes = adjusted_value == 0 ? 1 : (((CountBits(adjusted_value + 1))+7)/8);
          if (!encodeLength(numBytes, 1, (nBits+7)/8))    // 12.2.6
            return false;
          nBits = numBytes*8;
        }
        else if (nBits > 8)      // not 10.5.7.2
          nBits = 16;            // 10.5.7.3
        byteAlign();             // 10.7.5.2 - 10.7.5.4
      }

      encodeMultiBit(adjusted_value, nBits);
      return true;
    }

    bool encodeAnyType(const AbstractData*);

    void byteAlign();
    void encodeByte(unsigned value);
    void encodeBlock(const char * bufptr, unsigned nBytes);
    OpenBuf& encodedBuffer;
    unsigned short bitOffset;
    bool alignedFlag;
  };

  /**
   * This class is used to decode a byte stream encoded using Packed Encoding Rule (PER) back
   * to an ASN.1 object.
   *
   * The following example shows how to use this class.
   * \code
   *    ASN1::INGEGER obj;
   *    ASN1::PERDecoder encoder(buf, buf+5);
   *
   *    obj.accept(encoder); // now obj holds the value which the buf represents.
   * \endcode
   *
   * @see PEREncoder
   */
  /** Callback of the partial BER decoder: told about every decoded node whose path is registered. */
  class ASN1_API IAbstractVisitor
  {
  public:
    virtual ~IAbstractVisitor() {}
    virtual int visit(int index, const ASN1_STD string& path, AbstractData* data, unsigned position, unsigned length) = 0;
  };

  /** Byte source of the partial BER decoder (BufferSeeder.h holds the in-memory one). */
  class ASN1_API ISeeder
  {
  public:
    virtual ~ISeeder() {}
    virtual bool eof() const = 0;
    virtual unsigned char read() = 0;
    virtual unsigned read(char* buffer, unsigned size) = 0;
    virtual void seek(unsigned position) = 0;
    virtual void skip(unsigned size) = 0;
    virtual unsigned size() const = 0;
    virtual unsigned remaining() const = 0;
    virtual unsigned tell() const = 0;
    virtual void resize(unsigned size) = 0;
  };

  /** BER decoder over an ISeeder that reports every decoded node to the registered visitors and logs errors. */
  class ASN1_API BERDecoderPartial : public Visitor
  {
  public:
    class Visitante
    {
    public:
      Visitante(const ASN1_STD string& name, const ASN1_STD shared_ptr<IAbstractVisitor>& visitor) : m_name(name), m_visitor(visitor) {}
      ASN1_STD string getName() const { return m_name; }
      ASN1_STD shared_ptr<IAbstractVisitor> getVisitor() const { return m_visitor; }
    private:
      ASN1_STD string m_name;
      ASN1_STD shared_ptr<IAbstractVisitor> m_visitor;
    };
    typedef ASN1_STD vector<Visitante> Visitantes;

    BERDecoderPartial(ASN1_STD shared_ptr<ISeeder> seeder, CoderEnv* coder = 0);
    ~BERDecoderPartial();

    bool decodeChoicePreamle(CHOICE& value, unsigned& nextPosition);
    bool decodeHeader(unsigned& tag, bool& primitive, unsigned& len);
    bool decodeHeader(AbstractData& obj, unsigned& len);

    virtual VISIT_SEQ_RESULT preVisitExtensionRoots(SEQUENCE& value);
    virtual VISIT_SEQ_RESULT visitExtensionRoot(SEQUENCE& value, int index, int optional_id);
    virtual VISIT_SEQ_RESULT visitKnownExtension(SEQUENCE& value, int index, int optional_id);
    virtual bool visitUnknownExtensions(SEQUENCE& value);

    virtual void setVisitors(const Visitantes& visitors) { m_visitors.assign(visitors.begin(), visitors.end()); }
    virtual void addVisitor(const ASN1_STD string& name, const ASN1_STD shared_ptr<IAbstractVisitor>& visitor)
    { m_visitors.push_back(Visitante(name, visitor)); }
    virtual void addVisitor(const ASN1_STD string& name, IAbstractVisitor* visitor)
    {
      ASN1_STD shared_ptr<IAbstractVisitor> shared;
      shared.reset(visitor);
      m_visitors.push_back(Visitante(name, shared));
    }
    virtual void clearVisitors() { m_visitors.clear(); }

    ASN1_STD ostringstream& getLog() { return m_log; }

  private:
    using Visitor::do_visit;
    virtual bool do_visit(Null& value);
    virtual bool do_visit(BOOLEAN& value);
    virtual bool do_visit(INTEGER& value);
    virtual bool do_visit(ENUMERATED& value);
    virtual bool do_visit(OBJECT_IDENTIFIER& value);
    virtual bool do_visit(BIT_STRING& value);
    virtual bool do_visit(OCTET_STRING& value);
    virtual bool do_visit(AbstractString& value);
    virtual bool do_visit(BMPString& value);
    virtual bool do_visit(CHOICE& value);
    virtual bool do_visit(SEQUENCE_OF_Base& value);
    virtual bool do_visit(OpenData& value);
    virtual bool do_revisit(OpenData& value);
    virtual bool do_visit(TypeConstrainedOpenData& value);
    virtual bool do_visit(UTCTime& value);
    virtual bool do_visit(GeneralizedTime& value);

    void notify(int index, const ASN1_STD string& name, AbstractData* data, unsigned position, unsigned length, bool mayFree);
    template <class TIME> bool decodeTime(TIME& value, unsigned& len);

    bool m_checkTag;
    ASN1_STD vector<unsigned> endSEQUENCEPositions;
    ASN1_STD vector<ASN1_STD string> m_path;
    ASN1_STD shared_ptr<ISeeder> m_seeder;
    Visitantes m_visitors;
    ASN1_STD ostringstream m_log;

  private:
    BERDecoderPartial(const BERDecoderPartial &);
    BERDecoderPartial &operator=(const BERDecoderPartial &);
  };

  /** Visitor that empties the data it visits. */
  class ASN1_API DataFree : public Visitor
  {
  public:
    DataFree() {}
  private:
    using Visitor::do_visit;
    virtual bool do_visit(Null& value);
    virtual bool do_visit(BOOLEAN& value);
    virtual bool do_visit(INTEGER& value);
    virtual bool do_visit(IntegerWithNamedNumber& value);
    virtual bool do_visit(ENUMERATED& value);
    virtual bool do_visit(OBJECT_IDENTIFIER& value);
    virtual bool do_visit(BIT_STRING& value);
    virtual bool do_visit(OCTET_STRING& value);
    virtual bool do_visit(AbstractString& value);
    virtual bool do_visit(BMPString& value);
    virtual bool do_visit(CHOICE& value);
    virtual bool do_visit(SEQUENCE_OF_Base& value);
    virtual bool do_visit(OpenData& value);
    virtual bool do_revisit(OpenData& value);
    virtual bool do_visit(TypeConstrainedOpenData& value);
    virtual bool do_visit(UTCTime& value);
    virtual bool do_visit(GeneralizedTime& value);
  };

  class ASN1_API PERDecoder  : public Visitor
  {
  public:
    /**
     * Constructor
     *
     * @param first The start position of the encoded BER stream.
     * @param last  The end position of the encoded BER stream.
     * @param coder The CoderEnv object which the decoder used to find the information objects
     *  it needs to decode the ASN.1 object. If this parameter is not NULL, the decoder will
     *  decode the open type based on the information objects which are inserted to the CoderEnv
     *  objects.
     * @param isAligned Indicates whether using the aligned PER.
     *
     * @warning The unaligned PER has never been fully tested in current version.
     */
    PERDecoder(const char* first, size_t size, CoderEnv* coder = 0, bool isAligned = true)
      : Visitor(coder)
        , beginPosition(first)
        , endPosition(first+size)
        , bitOffset (8)
        , alignedFlag(isAligned){}

    struct memento_type
    {
      memento_type(const char* bytePos=0, unsigned bitPos=0)
        : bytePosition(bytePos), bitPosition(bitPos){}

      const char* bytePosition;
      unsigned bitPosition;
    };

    memento_type get_memento() const { return memento_type(beginPosition,bitOffset); }
    void rollback(memento_type memento) {
      if (memento.bytePosition != 0)
      {
        beginPosition = (memento.bytePosition > endPosition ?
                             endPosition : memento.bytePosition);
        bitOffset   = memento.bitPosition;
      }
    }

    bool aligned() const { return alignedFlag; }

    bool decodeChoicePreamle(CHOICE& value, memento_type& nextPostion);
    const char* getPosition() const { return beginPosition; }
    const char* getNextPosition() const { return beginPosition + (bitOffset != 8 ? 1 : 0); }
    void setPosition(const char* newPos);
    //! decode constrained length
    //! \param obj Constrained object
    //! \param length reference to decoded length
    //! \return true if decoded correctly
    bool decodeConstrainedLength(ConstrainedObject & obj, unsigned & length);
    //! decode length
    //! \param lower Lower default value
    //! \param upper Upper default value
    //! \param length reference to decoded length
    //! \return true if decoded correctly
    bool decodeLength(unsigned lower, unsigned upper, unsigned & len);
    //dts
    void reset(const char* first, const char* last, CoderEnv* coder = NULL, bool isAligned = true) {
        beginPosition = first;
        endPosition = last;
        bitOffset = 8;
        alignedFlag = isAligned;
    }

    virtual VISIT_SEQ_RESULT preVisitExtensionRoots(SEQUENCE& value);
    virtual VISIT_SEQ_RESULT visitExtensionRoot(SEQUENCE& value, int index, int optional_id);
    virtual VISIT_SEQ_RESULT preVisitExtensions(SEQUENCE& value);
    virtual VISIT_SEQ_RESULT visitKnownExtension(SEQUENCE& value, int index, int optional_id);
    virtual bool visitUnknownExtensions(SEQUENCE& value);

  private:
    using Visitor::do_visit;
    virtual bool do_visit(Null& value);
    virtual bool do_visit(BOOLEAN& value);
    virtual bool do_visit(INTEGER& value);
    virtual bool do_visit(ENUMERATED& value);
    virtual bool do_visit(OBJECT_IDENTIFIER& value);
    virtual bool do_visit(BIT_STRING& value);
    virtual bool do_visit(OCTET_STRING& value);
    virtual bool do_visit(AbstractString& value);
    virtual bool do_visit(BMPString& value);
    virtual bool do_visit(CHOICE& value);
    virtual bool do_visit(SEQUENCE_OF_Base& value);
    virtual bool do_visit(OpenData& value);
    virtual bool do_revisit(OpenData& value);
    virtual bool do_visit(TypeConstrainedOpenData& value);
    virtual bool do_visit(UTCTime& value);
    virtual bool do_visit(GeneralizedTime& value);

    void byteAlign();
    bool atEnd() ;
    unsigned getBitsLeft() const;
    unsigned getBytesLeft() const;
    bool decodeSingleBit();

    template <typename uns_int>
    bool decodeMultiBit(unsigned nBits, uns_int & value)
    {
      if(nBits > sizeof(value)*8 || nBits > getBitsLeft())
        return false;

      if (nBits == 0) {
        value = 0;
        return true;
      }

      if (nBits < bitOffset) {
        bitOffset -= nBits;
        value = (static_cast<unsigned char>(*beginPosition) >> bitOffset) & ((1 << nBits) - 1);
        return true;
      }

      value = static_cast<unsigned char>(*beginPosition++) & ((1 << bitOffset) - 1);
      nBits -= bitOffset;
      bitOffset = 8;

      while (nBits >= 8) {
        value = (value << 8) | static_cast<unsigned char>(*beginPosition++);
        nBits -= 8;
      }

      if (nBits > 0) {
        bitOffset = 8 - nBits;
        value = (value << nBits) | (static_cast<unsigned char>(*beginPosition) >> bitOffset);
      }

      return true;
    }

    bool decodeSmallUnsigned(unsigned & value);

    //! decode (un)signed integer value
    //! \param lower Lower default value
    //! \param upper Upper default value
    //! \param value reference to decoded value
    //! \return true if decoded correctly
    template <typename uns_int>
    bool decodeUnsigned(uns_int lower, uns_int upper, uns_int & value)
    {
      // X.691 section 10.5
      if (lower == upper) { // 10.5.4
        value = lower;
        return true;
      }

      if(atEnd())
        return false;

      uns_int range = (upper - lower) + 1;
      unsigned nBits = CountBits(range);

      if (alignedFlag && (range == 0 || range > 255)) { // not 10.5.6 and not 10.5.7.1
        if (nBits > 16) {                           // not 10.5.7.4
          decodeLength(1, (nBits+7)/8, nBits);      // 12.2.6
          nBits *= 8;
        }
        else if (nBits > 8)    // not 10.5.7.2
          nBits = 16;          // 10.5.7.3
        byteAlign();           // 10.7.5.2 - 10.7.5.4
      }

      if(decodeMultiBit(nBits, value))
      {
        value += lower;
        return true;
      }
      return false;
    }

    unsigned decodeBlock(char * bufptr, unsigned nBytes);
    bool decodeBitMap(ASN1_STD vector<char>& bitData, unsigned nBit);

    const char* beginPosition;
    const char* endPosition;
    unsigned short bitOffset;
    bool alignedFlag;
  };

  /**
   * This class is used to encode an ASN.1 object into a byte stream using Abstract Value Notation,
   * defined in ITU X.680.
   *
   * The following example shows how to use this class.
   * \code
   *    ASN1::INGEGER obj(5);
   *    ASN1::AVNEncoder encoder(ASN1_STD cout);
   *
   *    obj.accept(encoder); // now the object is printed to ASN1_STD cout.
   * \endcode
   *
   * @see AVNDecoder
   */
  class ASN1_API AVNEncoder : public ConstVisitor
  {
  public:
    AVNEncoder(ASN1_STD ostream& os)
      : strm(os), indent(0) {}

  private:
    using ConstVisitor::do_visit;
    virtual bool do_visit(const Null& value);
    virtual bool do_visit(const BOOLEAN& value);
    virtual bool do_visit(const INTEGER& value);
    virtual bool do_visit(const IntegerWithNamedNumber& value);
    virtual bool do_visit(const ENUMERATED& value);
    virtual bool do_visit(const OBJECT_IDENTIFIER& value);
    virtual bool do_visit(const BIT_STRING& value);
    virtual bool do_visit(const OCTET_STRING& value);
    virtual bool do_visit(const AbstractString& value);
    virtual bool do_visit(const BMPString& value);
    virtual bool do_visit(const CHOICE& value);
    virtual bool do_visit(const OpenData& value);
    virtual bool do_visit(const UTCTime& value);
    virtual bool do_visit(const GeneralizedTime& value);
    virtual bool do_visit(const SEQUENCE_OF_Base& value);

    virtual bool preVisitExtensionRoots(const SEQUENCE& value) ;
    virtual bool visitExtensionRoot(const SEQUENCE& value, int index);
    virtual bool visitKnownExtension(const SEQUENCE& value, int index);
    virtual bool afterVisitSequence(const SEQUENCE& value);

    ASN1_STD ostream& strm;
    int indent;
    ASN1_STD vector<bool> outputSeparators; // used to indicate whether to output separator while parsing SEQUENCE
  };

  /**
   * This class is used to decode an Abstract Value Notation encoded byte stream
   * to an ASN.1 object.
   *
   * The following example shows how to use this class.
   * \code
   *    ASN1::INGEGER obj;
   *    ASN1::AVNDecoder encoder(ASN1_STD cin);
   *
   *    obj.accept(encoder); // now obj holds the value which it read from ASN1_STD cin.
   * \endcode
   *
   * @see PEREncoder
   */
  class ASN1_API AVNDecoder  : public Visitor
  {
  public:
    AVNDecoder(ASN1_STD istream& is, CoderEnv* coder = 0)
      : Visitor(coder), strm(is) {}

  private:
    using Visitor::do_visit;
    virtual bool do_visit(Null& value);
    virtual bool do_visit(BOOLEAN& value);
    virtual bool do_visit(INTEGER& value);
    virtual bool do_visit(IntegerWithNamedNumber& value);
    virtual bool do_visit(ENUMERATED& value);
    virtual bool do_visit(OBJECT_IDENTIFIER& value);
    virtual bool do_visit(BIT_STRING& value);
    virtual bool do_visit(OCTET_STRING& value);
    virtual bool do_visit(AbstractString& value);
    virtual bool do_visit(BMPString& value);
    virtual bool do_visit(CHOICE& value);
    virtual bool do_visit(SEQUENCE_OF_Base& value);
    virtual bool do_visit(OpenData& value);
    virtual bool do_revisit(OpenData& value);
    virtual bool do_visit(TypeConstrainedOpenData& value);
    virtual bool do_visit(UTCTime& value);
    virtual bool do_visit(GeneralizedTime& value);

    virtual VISIT_SEQ_RESULT preVisitExtensionRoots(SEQUENCE& value);
    virtual VISIT_SEQ_RESULT visitExtensionRoot(SEQUENCE& value, int index, int optional_id);
    virtual VISIT_SEQ_RESULT visitKnownExtension(SEQUENCE& value, int index, int optional_id);
    virtual bool visitUnknownExtensions(SEQUENCE& value);

    ASN1_STD istream& strm;
    ASN1_STD vector<ASN1_STD string> identifiers; // used to indicate the last parsed idetifier while parsing SEQUENCE.
  };

  /**
   * The trace_invalid() is used to find the first component of a complex
   * ASN1::AbstactData object which doesn't satisfy its own constraint. It
   * output a string which describe where the AbstractData object is invalid
   * to the \c os.
   *
   * If the ASN1::AbstactData object is entirely valid, no string is outputted to \c os.
   *
   * @param os The stream where the result is outputted.
   * @param str The string to be inserted in the front of the result when the first
   *       invalid component is found.
   * @param data The ASN1::AbstactData object to be check for validity.
   * @return true if the ASN1::AbstactData object is valid.
   */
  bool trace_invalid(ASN1_STD ostream& os, const char* str, const AbstractData& data);

    /**
     * This class is used to help print an ASN.1 object using Abstract Value Notation,
     * defined in ITU X.680.
     *
     */

    class ASN1_API PrintHelper
    {
    public:

      PrintHelper() {}
      ~PrintHelper() {}

      virtual int Print(const char * fmt, ... ) = 0;

    };

    /**
     * This class is used to print an ASN.1 object using Abstract Value Notation,
     * defined in ITU X.680.
     *
     * The following example shows how to use this class.
     * \code
     *    ASN1::INGEGER obj(5);
     *    ASN1::AVNPrinter printer(PrintHelper& helper);
     *
     *    obj.accept(printer); // now the object is printed via helper object.
     * \endcode
     *
     */

    class ASN1_API AVNPrinter : public ConstVisitor
    {
    public:
      AVNPrinter(PrintHelper& helper_object) : helper(helper_object), indent(0) {}
    private:
      using ConstVisitor::do_visit;
      virtual bool do_visit(const Null& value);
      virtual bool do_visit(const BOOLEAN& value);
      virtual bool do_visit(const INTEGER& value);
      virtual bool do_visit(const IntegerWithNamedNumber& value);
      virtual bool do_visit(const ENUMERATED& value);
      virtual bool do_visit(const OBJECT_IDENTIFIER& value);
      virtual bool do_visit(const BIT_STRING& value);
      virtual bool do_visit(const OCTET_STRING& value);
      virtual bool do_visit(const AbstractString& value);
      virtual bool do_visit(const BMPString& value);
      virtual bool do_visit(const CHOICE& value);
      virtual bool do_visit(const OpenData& value);
      virtual bool do_visit(const UTCTime& value);
      virtual bool do_visit(const GeneralizedTime& value);
      virtual bool do_visit(const SEQUENCE_OF_Base& value);

      virtual bool preVisitExtensionRoots(const SEQUENCE& value) ;
      virtual bool visitExtensionRoot(const SEQUENCE& value, int index);
      virtual bool visitKnownExtension(const SEQUENCE& value, int index);
      virtual bool afterVisitSequence(const SEQUENCE& value);

      PrintHelper& helper;
      int indent;
      ASN1_STD vector<bool> outputSeparators; // used to indicate whether to output separator while parsing SEQUENCE
    };

  ///////////////////////////////////////////////////////////////////////////

  class ASN1_API Module
  {
  public:
    virtual ~Module(){};
    const char* name() const { return moduleName; }
  protected:
    const char* moduleName;
  };

  /////////////////////////////////////////////////////////////////////////////

  class ASN1_API CoderEnv
  {
  public:
    Module* find(const char* moduleName)
    {
      Modules::iterator i = modules.find(moduleName);
      return i != modules.end() ? i->second : 0;
    }
    void insert(Module* module) { assert(module!=0); modules[module->name()] = module;}
    void erase(Module* module) { assert(module!=0); modules.erase(module->name()); }
    void erase(const char* moduleName) { modules.erase(moduleName); }

    enum EncodingRules {
      avn,
      ber,
      per_Basic_Aligned
    };

    EncodingRules get_encodingRule() const { return encodingRule;}
    void set_encodingRule(EncodingRules rule) { encodingRule = rule; }
    void set_avn() {set_encodingRule(avn);}
    bool is_avn() const { return encodingRule == avn;}
    void set_ber() {set_encodingRule(ber);}
    bool is_ber() const { return encodingRule == ber;}
    void set_per_Basic_Aligned() {set_encodingRule(per_Basic_Aligned);}
    bool is_per_Basic_Aligned() const { return encodingRule == per_Basic_Aligned;}

    template <class OutputIterator>
    bool encode(const AbstractData& val, OutputIterator begin)
    {
      if (get_encodingRule() == per_Basic_Aligned)
        return encodePER(val, begin);
      if (get_encodingRule() == ber)
        return encodeBER(val, begin);
      if (get_encodingRule() == avn)
        return encodeAVN(val, begin);
      return false;
    }

    bool decode(const char* first, size_t size, AbstractData& val, bool defered);
    bool decode(const unsigned char* first, size_t size, AbstractData& val, bool defered)
    {
      return decode(reinterpret_cast<const char*>(first), size, val, defered);
    }

#if !defined(_MSC_VER) || (_MSC_VER > 1200)
    template <class InputIterator>
    bool decode(InputIterator first, InputIterator last, AbstractData& val, bool defered)
    {
      if (get_encodingRule() == avn)
      {
#if !defined(HP_aCC_RW)
        ASN1_STD istringstream strm(ASN1_STD string(first, last));
#else
        ASN1_STD string strng(first, last);
        ASN1_STD istrstream strm(strng.data(), strng.size());
#endif

        AVNDecoder decoder(strm);
        return val.accept(decoder);
      }
      OpenBuf buf(first, last);
      return decode((const char*)&buf[0], buf.size(), val, defered);
    }
#endif

  protected:
    EncodingRules encodingRule;
    struct StringLiteralCmp
    {
      bool operator() (const char* lhs, const char* rhs) const
      { return strcmp(lhs, rhs) < 0 ; }
    };

    typedef AssocVector<const char*, Module*, StringLiteralCmp> Modules;

    Modules modules;

    template <class OutputIterator>
    bool encodePER(const AbstractData& val, OutputIterator begin)
    {
      OpenBuf buf;
      PEREncoder encoder(buf);
      if (val.accept(encoder))
      {
        ASN1_STD copy(buf.begin(), buf.end(), begin);
        return true;
      }
      return false;
    }

    template <class OutputIterator>
    bool encodeBER(const AbstractData& val, OutputIterator begin)
    {
      OpenBuf buf;
      BEREncoder encoder(buf);
      if (val.accept(encoder))
      {
        ASN1_STD copy(buf.begin() , buf.end(), begin);
        return true;
      }
      return false;
    }

    template <class OutputIterator>
    bool encodeAVN(const AbstractData& val, OutputIterator begin)
    {
      DECLARE_STRSTREAM(strm);

      AVNEncoder encoder(strm);
      if (val.accept(encoder))
      {
        const ASN1_STD string str (strm.str());
        ASN1_STD copy(str.begin(), str.end(), begin);
        return true;
      }
      return false;
    }
  };

  template <class OutputIterator>
  bool encode(const AbstractData& val, CoderEnv* env, OutputIterator begin)
  {
    return env->encode(val, begin);
  }

  template <class InputIterator>
  bool decode(InputIterator first, InputIterator last, CoderEnv* env, AbstractData& val, bool defered = false)
  {
#if !defined(_MSC_VER) || (_MSC_VER > 1200)
    return env->decode(first, last, val, defered);
#else
    OpenBuf buf(first, last);
    return env->decode((const char*)&buf[0], buf.size(), val, defered);
#endif
  }

  ////////////////////////////////////////////////////////////////////////////

} // namespace ASN1

#include "asn1_useful.h"

#endif // _ASN1_H
