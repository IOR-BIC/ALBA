/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGuiTestClass.h,v $
  Language:  C++
  Date:      $Date: 2004-11-09 15:31:43 $
  Version:   $Revision: 1.2 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafGuiTestClass_h
#define __mafGuiTestClass_h

#include "mafObject.h"

//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// mafGuiTestClass
//------------------------------------------------------------------------------
/** Test class to test inclusion paths.
  mafGuiTestClass is a class to test inclusion paths.

  @sa mafObject
*/
class MAF_EXPORT mafGuiTestClass: public mafObject
{
public:
  mafGuiTestClass();
  virtual ~mafGuiTestClass();

  mafTypeMacro(mafGuiTestClass,mafObject);
 
  /** copy constructor, this makes a copy of the event */
  mafGuiTestClass(const mafGuiTestClass& c);

  void Print();

protected:

};

#endif /* __mafGuiTestClass_h */
