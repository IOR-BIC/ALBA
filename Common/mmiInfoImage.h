/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmiInfoImage.h,v $
  Language:  C++
  Date:      $Date: 2005-11-02 11:07:18 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mmiInfoImage_h
#define __mmiInfoImage_h

#include "mmiPER.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------

/** Class implementing image info reporting on status bar when not interacting.
*/
class mmiInfoImage : public mmiPER
{
public: 
  mafTypeMacro(mmiInfoImage,mmiPER);

  virtual void OnEvent(mafEventBase *event);

protected:
  mmiInfoImage();
  virtual ~mmiInfoImage();

private:
  mmiInfoImage(const mmiInfoImage&);  // Not implemented.
  void operator=(const mmiInfoImage&);  // Not implemented.
};
#endif 
