/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmiInfoImage.h,v $
  Language:  C++
  Date:      $Date: 2009-12-17 12:24:37 $
  Version:   $Revision: 1.1.8.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mmiInfoImage_h
#define __mmiInfoImage_h

#include "mafInteractorPER.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------

/** Class implementing image info reporting on status bar when not interacting.
*/
class mmiInfoImage : public mafInteractorPER
{
public: 
  mafTypeMacro(mmiInfoImage,mafInteractorPER);

  virtual void OnEvent(mafEventBase *event);

protected:
  mmiInfoImage();
  virtual ~mmiInfoImage();

private:
  mmiInfoImage(const mmiInfoImage&);  // Not implemented.
  void operator=(const mmiInfoImage&);  // Not implemented.
};
#endif 
