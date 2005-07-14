/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmiPicker.h,v $
  Language:  C++
  Date:      $Date: 2005-07-14 08:32:01 $
  Version:   $Revision: 1.3 $
  Authors:   Marco Petrone, originally by Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mmiPicker_h
#define __mmiPicker_h

#include "mafInteractor.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class mafObserver;

/** interactor implenting a picking operation */
class mmiPicker : public mafInteractor
{
public:
  mafTypeMacro(mmiPicker,mafInteractor);

protected:
  mmiPicker();
  virtual ~mmiPicker();
  
  virtual void OnButtonDown   (mafEventInteraction *e);
  virtual void OnButtonUp     (mafEventInteraction *e);
  
private:
  mmiPicker(const mmiPicker&);  // Not implemented.
  void operator=(const mmiPicker&);  // Not implemented.
};

#endif 
