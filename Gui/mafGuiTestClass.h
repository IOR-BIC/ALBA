/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGuiTestClass.h,v $
  Language:  C++
  Date:      $Date: 2005-02-25 07:25:34 $
  Version:   $Revision: 1.3 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
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
