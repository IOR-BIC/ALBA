/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: mafDefines.h,v $
  Language:  C++
  Date:      $Date: 2004-11-08 17:32:34 $
  Version:   $Revision: 1.7 $

=========================================================================*/
// .NAME standard macros and constants for the MFL libray
// .SECTION Description
// This file include all the standard macros and constants used inside the MFL
// library.
// .SECTION ToDO
// - Make a test for the VME object factory 
#ifndef __mafDefines_h
#define __mafDefines_h

#include "Configure.h"
#include "string.h"

typedef double mafTimeStamp;

#define MAF_OK                 0
#define MAF_ERROR              1

enum {ID_NO_EVENT=0};

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
  Macro used by mafObjects for RTTI information. This macor must be placed
  in the class definition puplic section.
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

  
/** Helper macro used for testing */  
#define MAF_TEST(a) if (!(a)) \
{ \
  std::cerr << "Test failed at line " \
  << __LINE__ << " : " << #a << std::endl; \
  return MAF_ERROR; \
}

#endif

