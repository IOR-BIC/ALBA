/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: mafDefines.h,v $
  Language:  C++
  Date:      $Date: 2004-10-27 11:24:41 $
  Version:   $Revision: 1.2 $

=========================================================================*/
// .NAME standard macros and constants for the MFL libray
// .SECTION Description
// This file include all the standard macros and constants used inside the MFL
// library.
// .SECTION ToDO
// - Make a test for the VME object factory 
#ifndef __mafDefines_h
#define __mafDefines_h

#include "Core/Configure.h"

typedef double mafTimeStamp;

#define MAF_OK                 0
#define MAF_ERROR              1

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
  Macros used to determine whether a class is the same class or
  a subclass of the named class and to create a new instance of
  the same type of object.
*/
#define mafTypeMacro(thisClass,superclass) \
  private: \
  static mafID TypeId; \
  public: \
  typedef superclass Superclass; \
  static mafID GetTypeId(); \
  virtual mafID GetClassTypeId() const; \
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

/** This macro must be used in the cpp/cxx file to implement the TypeId member */
#define mafCxxTypeMacro(thisClass) \
  mafID thisClass::TypeId = GetNextTypeId(#thisClass); \
  mafID thisClass::GetTypeId() {return thisClass::TypeId;} \
  mafID thisClass::GetClassTypeId() const {return thisClass::TypeId;} \
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

#endif

