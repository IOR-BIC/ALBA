/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafDefines.h,v $
  Language:  C++
  Date:      $Date: 2004-12-20 20:47:07 $
  Version:   $Revision: 1.17 $
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
#include <string.h>
#include <typeinfo>

//------------------------------------------------------------------------------
// Typedefs
//------------------------------------------------------------------------------
typedef double mafTimeStamp; ///< type for time varying data timestamps (not for pipelines timestamps!)
typedef unsigned long mafID; ///< type for IDs inside MAF @todo to be changed to support 64bit IDs
typedef std::type_info mafTypeID; ///< type for mafObject's class type IDs

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

/** Delete a VTK object */
#define vtkDEL(a) if (a) { a->Delete(); a = NULL; }

/** Allocate a new VTK object: don't worry, New is a static member function! */
#define vtkNEW(a) a=a->New()

/** Delete a MAF object */
//#define mafDEL(a) if (a) {delete a; a = NULL;}


/**
  Macro used by mafObjects for RTTI information. This macro must be placed
  in the class definition public section. */
#define mafAbstractTypeMacro(thisClass,superclass) \
  public: \
  typedef superclass Superclass; \
  static const mafTypeID &GetTypeId(); \
  virtual const mafTypeID &GetClassId() const; \
  static const char *GetTypeName(); \
  virtual const char *GetClassName() const; \
  static bool IsTypeOf(const char *type); \
  static bool IsTypeOf(const mafTypeID &type); \
  virtual bool IsA(const char *type) const; \
  virtual bool IsA(const mafTypeID &type) const; \
  static thisClass* SafeDownCast(mafObject *o);

/**
  Macro used by mafObjects for RTTI information. This macro must be placed
  in the class definition public section. */
#define mafTypeMacro(thisClass,superclass) \
  mafAbstractTypeMacro(thisClass,superclass); \
  static mafObject *NewObject(); \
  virtual mafObject *NewInternalInstance() const; \
  thisClass *NewInstance() const; \
  static thisClass *New();
  

/**
  Macro used by mafObjects for RTTI information. This macor must be placed
  in the .cpp file. */
#define mafCxxAbstractTypeMacro(thisClass) \
  const mafTypeID &thisClass::GetTypeId() {return typeid(thisClass);} \
  const mafTypeID &thisClass::GetClassId() const {return typeid(thisClass);} \
  const char *thisClass::GetTypeName() {return #thisClass;} \
  const char *thisClass::GetClassName() const {return #thisClass;} \
  bool thisClass::IsTypeOf(const char *type) \
  { return ( strcmp(#thisClass,type)==0 ) ? true : Superclass::IsTypeOf(type); } \
  bool thisClass::IsTypeOf(const mafTypeID &type) \
  { return ( type==typeid(thisClass) ? true : Superclass::IsTypeOf(type) ); } \
  bool thisClass::IsA(const char *type) const {return IsTypeOf(type);} \
  bool thisClass::IsA(const mafTypeID &type) const {return IsTypeOf(type);} \
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
    if (obj) obj->HeapFlag=true; \
    return obj; \
  } \
  mafObject *thisClass::NewInternalInstance() const \
  { return NewObject(); } \
  thisClass *thisClass::New() \
  { return (thisClass *)NewObject(); } \
  thisClass *thisClass::NewInstance() const \
  { return (thisClass *)NewInternalInstance(); }


/** Macro used to define Set/GetListener() member functions and a Listener member variable */
#define mafListenerMacro \
  public: \
  void SetListener(mafObserver *observer) { m_Listener=observer;} \
  mafObserver *GetListener() { return m_Listener;}
  
/** mafEventMacro is an handy shortcut to send an Event. */
#define mafEventMacro(e)  if (m_Listener) {m_Listener->OnEvent(e);}
  
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

