/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafDefines.h,v $
  Language:  C++
  Date:      $Date: 2004-11-18 22:35:24 $
  Version:   $Revision: 1.4 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

/** Standard macros and constants for the MAF library.
  This file include all the standard macros and constants used inside the MAF
  library.
*/

#ifndef __mafDefines_h
#define __mafDefines_h

#include "mafConfigure.h"

#include "string.h"

//------------------------------------------------------------------------------
// Typedefs
//------------------------------------------------------------------------------
typedef double mafTimeStamp;

//------------------------------------------------------------------------------
// Constants
//------------------------------------------------------------------------------
enum {MAF_OK=0, MAF_ERROR=1};
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

/** this is the type used for IDs inside the MAF
    @todo: to be changed to support 64bit IDs */
typedef unsigned long mafID;

/** Delete a VTK object */
#define vtkDEL(a) if (a) { a->Delete(); a = NULL; }

/** Allocate a new VTK object: don't worry, New is a static member function! */
#define vtkNEW(a) a=a->New()

/**
  Macro used by mafObjects for RTTI information. This macro must be placed
  in the class definition public section.
*/
#define mafTypeMacro(thisClass,superclass) \
  private: \
  static mafID TypeId; \
  public: \
  typedef superclass Superclass; \
  static mafID GetTypeId(); \
  virtual mafID GetClassId() const; \
  static const char *GetTypeName(); \
  virtual const char *GetClassName() const; \
  static int IsTypeOf(const char *type); \
  static int IsTypeOf(const mafID type); \
  virtual int IsA(const char *type) const; \
  virtual int IsA(const mafID type) const; \
  static thisClass* SafeDownCast(mafObject *o); \
  static mafObject *NewObjectInstance(); \
  virtual mafObject *NewInternalInstance() const; \
  thisClass *NewInstance() const;

/**
  Macro used by mafObjects for RTTI information. This macor must be placed
  in the .cpp file.
*/
#define mafCxxTypeMacro(thisClass) \
  mafID thisClass::TypeId = GetNextTypeId(#thisClass); \
  mafID thisClass::GetTypeId() {return thisClass::TypeId;} \
  mafID thisClass::GetClassId() const {return thisClass::TypeId;} \
  const char *thisClass::GetTypeName() {return #thisClass;} \
  const char *thisClass::GetClassName() const {return #thisClass;} \
  int thisClass::IsTypeOf(const char *type) \
  { \
    if ( !strcmp(#thisClass,type) ) \
      { \
      return 1; \
      } \
    return Superclass::IsTypeOf(type); \
  } \
  int thisClass::IsTypeOf(const mafID type) \
  { \
    if ( type==thisClass::TypeId) \
    { \
      return 1; \
    } \
    return Superclass::IsTypeOf(type); \
  } \
  int thisClass::IsA(const char *type) const {return IsTypeOf(type);} \
  int thisClass::IsA(const mafID type) const {return IsTypeOf(type);} \
  thisClass* thisClass::SafeDownCast(mafObject *o) \
  { \
    if ( o && o->IsA(thisClass::TypeId) ) \
    { \
      return static_cast<thisClass *>(o); \
    } \
    return NULL;\
  } \
  mafObject *thisClass::NewObjectInstance() \
  { \
    return new thisClass; \
  } \
  mafObject *thisClass::NewInternalInstance() const \
  { \
    return NewObjectInstance(); \
  } \
  thisClass *thisClass::NewInstance() const \
  { \
    return thisClass::SafeDownCast(NewInternalInstance()); \
  }


/** Macro used to define Set/GetListener() member functions and a Listener member variable */
#define mafSetGetListenerMacro \
  public: \
  void SetListener(mafObserver *observer) { Listener=observer;} \
  mafObserver *GetListener() { return Listener;}
  
/** mafEventMacro is an handy shortcut to send an Event. */
#define mafEventMacro(e)  (Listener) ? Listener->OnEvent(e) : 0
  
/** Helper macro used for testing */  
#define MAF_TEST(a) if (!(a)) \
{ \
  std::cerr << "Test failed at line " \
  << __LINE__ << " : " << #a << std::endl; \
  return MAF_ERROR; \
}

/** 
  Macro for Print Warning messages in log area. To use this you must
  include <sstream> header file */
#define mafWarningMacro(x) \
{ std::stringstream msg; \
  msg << "Warning in: " __FILE__ ", line " << __LINE__ << "\n" x \
    << "\n"; \
    mafLogMessage(msg.str().c_str());\
}

/** 
  Macro for Print Error messages in log area. To use this you must
  include <sstream> header file */
#define mafErrorMacro(x) \
{ std::stringstream msg; \
  msg << "Error in: " __FILE__ ", line " << __LINE__ << "\n" x \
    << "\n"; \
    mafLogMessage(msg.str().c_str());\
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

#else MAF_USE_WX // this is less safe since it can't limit output string size
/** 
  Macro for formatted printing to string (adapted from wxWidgets IMPLEMENT_LOG_FUNCTION)
  To use it you will need to include <stdio.h>, <stdarg.h> and <varargs.h> */
#define MAF_PRINT_MACRO(format,buffer,size) \
  va_list argptr; \
  va_start(argptr, format); \
  vsprintf(buffer, format, argptr); \
  va_end(argptr);

#endif MAF_USE_WX

#endif

