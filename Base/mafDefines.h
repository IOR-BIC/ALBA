/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafDefines.h,v $
  Language:  C++
  Date:      $Date: 2005-04-01 09:48:07 $
  Version:   $Revision: 1.15 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafDefines_h
#define __mafDefines_h

/** Standard macros and constants for the MAF library.
  This file include all the standard macros and constants used inside the MAF
  library.
*/
//----------------------------------------------------------------------------
// Pragmas: disable the "singned/unsigned mismatch" warning
//----------------------------------------------------------------------------
#pragma warning( disable : 4018 )

//----------------------------------------------------------------------------
// Includes: mafConfigure should be first, mafIncludeWX second
//----------------------------------------------------------------------------
#include "mafConfigure.h"
#include "mafIncludeWX.h" // must be after mafConfigure.h

#include <string.h>
#include <typeinfo>
#include <iosfwd>
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Typedefs
//------------------------------------------------------------------------------
typedef double mafTimeStamp; ///< type for time varying data timestamps (not for pipelines timestamps!)
typedef long mafID; ///< type for IDs inside MAF @todo to be changed to support 64bit IDs
typedef std::type_info mafTypeID; ///< type for mafObject's class type IDs

//------------------------------------------------------------------------------
// Constants
//------------------------------------------------------------------------------
enum {MAF_OK=0, MAF_ERROR,MAF_USER_RETURN_VALUE};
enum {ID_NO_EVENT=0};
enum {MAF_STRING_BUFFER_SIZE=2048};

//------------------------------------------------------------------------------
// Global Variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Global Functions
//------------------------------------------------------------------------------
/** write a message in the log area */
void mafLogMessage(const char *format, ...);
/** open a warning dialog and write a message */
void mafWarningMessage(const char *format, ...);
/** open an error dialog and write a message */
void mafErrorMessage(const char *format, ...);
/** open a message dialog and write a message */
void mafMessage(const char *format, ...);

//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------

/** Delete a VTK object */
#define vtkDEL(a) if (a) { a->Delete(); a = NULL; }

/** Allocate a new VTK object: don't worry, New is a static member function! */
#define vtkNEW(a) a=a->New()

/** Allocate a new MAF object: don't worry, New is a static member function! */
#define mafNEW(a) a=a->New();a->Register(this)

/** Delete a MAF object */
#define mafDEL(a) if (a) { a->Delete(); a = NULL;}

#define cppNEW(a) a=a->New()

/** delete a new() allocated object */
#define cppDEL(a) if (a) { delete a; a = NULL;}

/**
  Macro used by mafObjects for RTTI information. This macro must be placed
  in the class definition public section. */
#define mafAbstractTypeMacro(thisClass,superclass) \
  public: \
  /** commodity type representing the parent class type */ \
  typedef superclass Superclass; \
  /** return the class type id for this class type (static function) */ \
  static const mafTypeID &GetStaticTypeId(); \
  /** return the class type id for this mafObject instance */ \
  virtual const mafTypeID &GetTypeId() const; \
  /** Return the name of this type (static function) */ \
  static const char *GetStaticTypeName(); \
  /** Return the class name of this instance */ \
  virtual const char *GetTypeName() const; \
  /** This is used by IsA to check the class name */ \
  static bool IsStaticType(const char *type); \
  /** This is used by IsA to check the class type id */ \
  static bool IsStaticType(const mafTypeID &type); \
  /** Check the class name of this instance */ \
  virtual bool IsA(const char *type) const; \
  /** Check the type id of this instance */ \
  virtual bool IsA(const mafTypeID &type) const; \
  /** Cast with dynamic type checking. This is used for casting from a (mafObject *) */ \
  static thisClass* SafeDownCast(mafObject *o);

/**
  Macro used by mafObjects for RTTI information. This macro must be placed
  in the class definition public section. */
#define mafTypeMacro(thisClass,superclass) \
  mafAbstractTypeMacro(thisClass,superclass); \
  /** return a new instance of the given type (static function) */  \
  static mafObject *NewObject(); \
  /** return a new instance of the mafObject instance */ \
  virtual mafObject *NewObjectInstance() const; \
  /** return a typed new instance of a given object (this calls NewObject and casts) */ \
  thisClass *NewInstance() const; \
  /** return a new instance of the this class type (static function). It can be called with "object_type::New()"  \
      Also this function must be used for creating objects to be used with reference counting in place of the new() \
      operator. */ \
  static thisClass *New();
  
/**
  Macro used by mafObjects for RTTI information. This macor must be placed
  in the .cpp file. */
#define mafCxxAbstractTypeMacro(thisClass) \
  const mafTypeID &thisClass::GetStaticTypeId() {return typeid(thisClass);} \
  const mafTypeID &thisClass::GetTypeId() const {return typeid(thisClass);} \
  const char *thisClass::GetStaticTypeName() {return #thisClass;} \
  const char *thisClass::GetTypeName() const {return #thisClass;} \
  bool thisClass::IsStaticType(const char *type) \
  { return ( strcmp(#thisClass,type)==0 ) ? true : Superclass::IsStaticType(type); } \
  bool thisClass::IsStaticType(const mafTypeID &type) \
  { return ( type==typeid(thisClass) ? true : Superclass::IsStaticType(type) ); } \
  bool thisClass::IsA(const char *type) const {return IsStaticType(type);} \
  bool thisClass::IsA(const mafTypeID &type) const {return IsStaticType(type);} \
  thisClass* thisClass::SafeDownCast(mafObject *o) \
  { try { return dynamic_cast<thisClass *>(o); } catch (std::bad_cast) { return NULL;} }

/**
  Macro used by mafObjects for RTTI information. This macro must be placed
  in the .cpp file. */
#define mafCxxTypeMacro(thisClass) \
  mafCxxAbstractTypeMacro(thisClass); \
  mafObject *thisClass::NewObject() \
  { \
    thisClass *obj = new thisClass; \
    if (obj) obj->m_HeapFlag=true; \
    return obj; \
  } \
  mafObject *thisClass::NewObjectInstance() const \
  { return NewObject(); } \
  thisClass *thisClass::New() \
  { return (thisClass *)NewObject(); } \
  thisClass *thisClass::NewInstance() const \
  { return (thisClass *)NewObjectInstance(); }

/** Shortcut for type checking */
#define IsMAFType(type_name) IsA(type_name::GetStaticTypeId())

/** This macros is used to declare a new Id and should be placed in a .h file*/
#define MAF_ID_IMP(idname) const mafID idname = mmuIdFactory::GetNextId(#idname);
/** This macros is used to define a new Id and should be placed in a .cpp file*/
#define MAF_ID_DEC(idname) static const mafID idname;
/** This macros is used to declare a new global Id and should be placed in a .h file*/
#define MAF_ID_GLOBAL(idname) extern const mafID idname;

/** This macro is used to define a group of IDs and should be placed in a .h file. Ids declaration with MAF_ID_DEC. */
#define MAF_ID_GROUP(groupname,num) const mafID groupname = mmuIdFactory::AllocIdGroup(#groupname,num);

/** This macro is used to declare the base ID of a  group of ids. */
#define MAF_ID_CLASS_DEC(baseClass) static const mafID BaseID;
/** This macro is used to define the base ID of a  group of ids. */
#define MAF_ID_CLASS_IMP(baseClass,num) const mafID baseClass::BaseID = mmuIdFactory::AllocIdGroup("#baseClass::BaseID",num);

#define MAF_ID_LOC(name,idname) static const mafID name=mmuIdFactory::GetId(#idname);

/** These macros are used to retrieve the base ID of a group of events */
#define mafGetEventClassId(event,baseClass) (event->GetID()-baseClass::BaseID)
#define mafGetEventGroupId(event,baseID) (event->GetID()-baseID)
#define mafEvalGroupId(baseClass,id) (baseClass::BaseID+id)

/** mafEventMacro is an handy shortcut to send an Event. */
#define mafEventMacro(e)  if (m_Listener) {m_Listener->OnEvent(&e);}
  
/** Helper macro used for testing */  
#define MAF_TEST(a) if (!(a)) \
{ \
  std::cerr << "Test failed at line " \
  << __LINE__ << " : " << #a << std::endl; \
  return MAF_ERROR; \
}

/** 
  Macro for printing Warning messages in log area. This macro also
  displays line at which error was printed. To use this you must
  include <sstream> header file */
#define mafWarningMacro(x) \
{ \
  std::stringstream msg; \
  msg << "Warning in: " __FILE__ ", line " << __LINE__ << "\n" x \
    << "\n"; \
  mafLogMessage(msg.str().c_str());\
}

/** 
  Macro for printing Error messages in log area. This macro also
  displays line at which error was printed. To use this you must
  include <sstream> header file */
#define mafErrorMacro(x) \
{ \
  std::stringstream msg; \
  msg << "Error in: " __FILE__ ", line " << __LINE__ << "\n" x \
    << "\n"; \
  mafLogMessage(msg.str().c_str());\
}

/** 
  Macro for displaying Warning messages. To use this you must
  include <sstream> header file */
#define mafWarningMessageMacro(x) \
{ \
  std::stringstream msg; \
  msg << x << "\n"; \
  mafWarningMessage(msg.str().c_str());\
}

/** 
  Macro for displaying Error messages. To use this you must
  include <sstream> header file */
#define mafErrorMessageMacro(x) \
{ \
  std::stringstream msg; \
  msg << x << "\n"; \
  mafErrorMessage(msg.str().c_str());\
}

/** 
  Macro for displaying messages. To use this you must
  include <sstream> header file */
#define mafMessageMacro(x) \
{ \
  std::stringstream msg; \
  msg << x << "\n"; \
  mafMessage(msg.str().c_str());\
}

#ifdef MAF_USE_WX
/** 
  Macro for formatted printing to string (adapted from wxWidgets IMPLEMENT_LOG_FUNCTION)
  To use it you will need to include wx/wx.h */
#define MAF_PRINT_MACRO(format,buffer,size) \
  va_list argptr; \
  va_start(argptr, format); \
  wxVsnprintf(buffer, size, format, argptr); \
  va_end(argptr);

#else
/** 
  Macro for formatted printing to string (adapted from wxWidgets IMPLEMENT_LOG_FUNCTION)
  To use it you will need to include <stdio.h>, <stdarg.h> and <varargs.h>
  This is less safe since it can't limit output string size. */
#define MAF_PRINT_MACRO(format,buffer,size) \
  va_list argptr; \
  va_start(argptr, format); \
  vsprintf(buffer, format, argptr); \
  va_end(argptr);

#endif

#endif

