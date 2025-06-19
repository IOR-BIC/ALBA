/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDefines
 Authors: Marco Petrone, Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaDefines_h
#define __albaDefines_h

/** Standard macros and constants for the ALBA library.
  This file include all the standard macros and constants used inside the ALBA
  library.
*/
//----------------------------------------------------------------------------
// Pragmas: disable the "singned/unsigned mismatch" warning
//----------------------------------------------------------------------------
#pragma warning( disable : 4018 )
#pragma warning( disable : 4251 )



//----------------------------------------------------------------------------
// Includes: albaConfigure should be first, albaIncludeWX second
//----------------------------------------------------------------------------
#include "albaConfigure.h"
#include "albaIncludeWX.h" // must be after albaConfigure.h

#include <string.h>
#include <typeinfo>
#include <iosfwd>
#include <sstream>
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
#ifndef MIN 
	#define MIN( x, y ) ( (x) < (y) ? (x) : (y) )
#endif
#ifndef MAX 
	#define MAX( x, y ) ( (x) > (y) ? (x) : (y) )
#endif
#ifndef round
	#define round(x) (x<0?ceil((x)-0.5):floor((x)+0.5))
#endif

//------------------------------------------------------------------------------
// Typedefs
//------------------------------------------------------------------------------
typedef double albaTimeStamp; ///< type for time varying data timestamps (not for pipelines timestamps!)
typedef long albaID; ///< type for IDs inside ALBA @todo to be changed to support 64bit IDs
typedef std::type_info albaTypeID; ///< type for albaObject's class type IDs

//------------------------------------------------------------------------------
// Constants
//------------------------------------------------------------------------------
enum ALBA_EXPORT ALBA_RETURN_VALUES_ID
{
  ALBA_OK = 0,
  ALBA_ERROR,
  ALBA_WAIT,
  ALBA_USER_RETURN_VALUE
};
#define ID_NO_EVENT 0
/*enum ALBA_NO_EVENT_ID
{
  ID_NO_EVENT = 0
};
*/
#define ALBA_STRING_BUFFER_SIZE 2048
/*enum ALBA_DEFAULT_BUFFER_SIZE
{
  ALBA_STRING_BUFFER_SIZE = 2048
};
*/
//------------------------------------------------------------------------------
// Global Variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Global Functions
//------------------------------------------------------------------------------
/** write a message in the log area */
ALBA_EXPORT void albaLogMessage(const char *format, ...);
/** open a warning dialog and write a message */
ALBA_EXPORT void albaWarningMessage(const char *format, ...);
/** open an error dialog and write a message */
ALBA_EXPORT void albaErrorMessage(const char *format, ...);
/** open a message dialog and write a message */
ALBA_EXPORT void albaMessage(const char *format, ...);
/** 
  reliable comparison test for floating point numbers. Extracted from article:
  "Work Around Floating-Point Accuracy/Comparison Problems" Article ID: Q69333
  of MSDN library*/
ALBA_EXPORT bool albaEquals(double x, double y);
ALBA_EXPORT bool albaFloatEquals(float x, float y);

/** retrieve the double value with the desired precision*/
ALBA_EXPORT double albaRoundToPrecision(double val, unsigned prec);

/** return true if it's little endian*/
ALBA_EXPORT bool albaIsLittleEndian(void);

/** wait for given milliseconds */
ALBA_EXPORT void albaSleep(int msec);

//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------

/** Delete a VTK object */
#define vtkDEL(a) do{if (a) { a->Delete(); a = NULL; }}while(0)

/** Allocate a new VTK object: don't worry, New is a static member function! */
#define vtkNEW(a) a=a->New()

/** Allocate a new ALBA object: don't worry, New is a static member function! */
#define albaNEW(a) do{a=a->New();a->Register(this);}while(0)

/** Delete a ALBA object */
#define albaDEL(a) do{if (a) { a->Delete(); a = NULL;}}while(0)

/** delete a new() allocated object */
#define cppDEL(a) do{if (a) { delete a; a = NULL;}}while(0)

/**
  Macro used by albaObjects for RTTI information. This macro must be placed
  in the class definition public section. */
#define albaAbstractTypeMacro(thisClass,superclass) \
  public: \
  /** commodity type representing the parent class type */ \
  typedef superclass Superclass; \
  /** return the class type id for this class type (static function) */ \
  static const albaTypeID &GetStaticTypeId(); \
  /** return the class type id for this albaObject instance */ \
  virtual const albaTypeID &GetTypeId() const; \
  /** Return the name of this type (static function) */ \
  static const char *GetStaticTypeName(); \
  /** Return the class name of this instance */ \
  virtual const char *GetTypeName() const; \
  /** This is used by IsA to check the class name */ \
  static bool IsStaticType(const char *type); \
  /** This is used by IsA to check the class type id */ \
  static bool IsStaticType(const albaTypeID &type); \
  /** Check the class name of this instance */ \
  virtual bool IsA(const char *type) const; \
  /** Check the type id of this instance */ \
  virtual bool IsA(const albaTypeID &type) const; \
  /** Cast with dynamic type checking. This is used for casting from a (albaObject *) */ \
  static thisClass* SafeDownCast(albaObject *o);

/**
  Macro used by albaObjects for RTTI information. This macro must be placed
  in the class definition public section. */
#define albaTypeMacro(thisClass,superclass) \
  albaAbstractTypeMacro(thisClass,superclass); \
  /** return a new instance of the given type (static function) */  \
  static albaObject *NewObject(); \
  /** return a new instance of the albaObject instance */ \
  virtual albaObject *NewObjectInstance() const; \
  /** return a typed new instance of a given object (this calls NewObject and casts) */ \
  thisClass *NewInstance() const; \
  /** return a new instance of the this class type (static function). It can be called with "object_type::New()"  \
      Also this function must be used for creating objects to be used with reference counting in place of the new() \
      operator. */ \
  static thisClass *New();
  
/**
  Macro used by albaObjects for RTTI information. This macor must be placed
  in the .cpp file. */
#define albaCxxAbstractTypeMacro(thisClass) \
  const albaTypeID &thisClass::GetStaticTypeId() {return typeid(thisClass);} \
  const albaTypeID &thisClass::GetTypeId() const {return typeid(thisClass);} \
  const char *thisClass::GetStaticTypeName() {return #thisClass;} \
  const char *thisClass::GetTypeName() const {return #thisClass;} \
  bool thisClass::IsStaticType(const char *type) \
  { return ( strcmp(#thisClass,type)==0 ) ? true : Superclass::IsStaticType(type); } \
  bool thisClass::IsStaticType(const albaTypeID &type) \
  { return ( type==typeid(thisClass) ? true : Superclass::IsStaticType(type) ); } \
  bool thisClass::IsA(const char *type) const {return IsStaticType(type);} \
  bool thisClass::IsA(const albaTypeID &type) const {return IsStaticType(type);} \
  thisClass* thisClass::SafeDownCast(albaObject *o) \
  { try { return dynamic_cast<thisClass *>(o); } catch (std::bad_cast) { return NULL;} }

/**
  Macro used by albaObjects for RTTI information. This macro must be placed
  in the .cpp file. */
#define albaCxxTypeMacro(thisClass) \
  albaCxxAbstractTypeMacro(thisClass); \
  albaObject *thisClass::NewObject() \
  { \
    thisClass *obj = new thisClass; \
    if (obj) obj->m_HeapFlag=true; \
    return obj; \
  } \
  albaObject *thisClass::NewObjectInstance() const \
  { return NewObject(); } \
  thisClass *thisClass::New() \
  { return (thisClass *)NewObject(); } \
  thisClass *thisClass::NewInstance() const \
  { return (thisClass *)NewObjectInstance(); }

/** Shortcut for type checking */
#define IsALBAType(type_name) IsA(type_name::GetStaticTypeId())

/** This macros is used to declare a new Id and should be placed in a .h file*/
#define ALBA_ID_IMP(idname) const albaID idname = mmuIdFactory::GetNextId(#idname);
/** This macros is used to define a new Id and should be placed in a .cpp file*/
#define ALBA_ID_DEC(idname) static const albaID idname;
/** This macros is used to declare a new global Id and should be placed in a .h file*/
#define ALBA_ID_GLOBAL(idname) extern const albaID idname;

/** This macro is used to define a group of IDs and should be placed in a .h file. Ids declaration with ALBA_ID_DEC. */
// Not Used !!
//#define ALBA_ID_GROUP(groupname,num) const albaID groupname = mmuIdFactory::AllocIdGroup(#groupname,num);

/** This macro is used to declare the base ID of a  group of ids. */
#define ALBA_ID_CLASS_DEC(baseClass) static const albaID BaseID;
/** This macro is used to define the base ID of a  group of ids. */
#define ALBA_ID_CLASS_IMP(baseClass,num) const albaID baseClass::BaseID = mmuIdFactory::AllocIdGroup("#baseClass::BaseID",num);

#define ALBA_ID_LOC(name,idname) static const albaID name=mmuIdFactory::GetId(#idname);

/** These macros are used to retrieve the base ID of a group of events */
#define albaGetEventClassId(event,baseClass) (event->GetID()-baseClass::BaseID)
#define albaGetEventGroupId(event,baseID) (event->GetID()-baseID)
#define albaEvalGroupId(baseClass,id) (baseClass::BaseID+id)

/** albaEventMacro is an handy shortcut to send an Event. */
#define albaEventMacro(e)  if (m_Listener) {m_Listener->OnEvent(&e);}
  
/** Helper macro used for testing */  
#define ALBA_TEST(a) if (!(a)) \
{ \
  std::cerr << "Test failed at line " \
  << __LINE__ << " : " << #a << std::endl; \
  return ALBA_ERROR; \
}

/** 
  Macro for printing Warning messages in log area. This macro also
  displays line at which error was printed. */
#define albaWarningMacro(x) \
{ \
  std::stringstream msg; \
  msg << "Warning in: " __FILE__ ", line " << __LINE__ << "\n" x \
    << "\n"; \
  albaLogMessage(msg.str().c_str());\
}

/** 
  Macro for printing Error messages in log area. This macro also
  displays line at which error was printed. */
#define albaErrorMacro(x) \
{ \
  std::stringstream msg; \
  msg << "Error in: " __FILE__ ", line " << __LINE__ << "\n" x \
    << "\n"; \
  albaLogMessage(msg.str().c_str());\
}

/** 
  Macro for displaying Warning messages.*/
#define albaWarningMessageMacro(x) \
{ \
  std::stringstream msg; \
  msg << x << "\n"; \
  albaWarningMessage(msg.str().c_str());\
}

/** 
  Macro for displaying Error messages. */
#define albaErrorMessageMacro(x) \
{ \
  std::stringstream msg; \
  msg << x << "\n"; \
  albaErrorMessage(msg.str().c_str());\
}

/** 
  Macro for displaying messages. */
#define albaMessageMacro(x) \
{ \
  std::stringstream msg; \
  msg << x << "\n"; \
  albaMessage(msg.str().c_str());\
}

/** 
  Macro for formatted printing to string (adapted from wxWidgets IMPLEMENT_LOG_FUNCTION)
  To use it you will need to include <stdio.h>, <stdarg.h> and <varargs.h>
  This is less safe since it can't limit output string size. */
#define ALBA_PRINT_MACRO(format,buffer,size) \
  va_list argptr; \
  va_start(argptr, format); \
  vsnprintf(buffer,(const size_t)size, format, argptr); \
  va_end(argptr);

#endif
