/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmiInfoImage.h,v $
  Language:  C++
  Date:      $Date: 2012-04-06 08:19:22 $
  Version:   $Revision: 1.1.8.3 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mmiInfoImage_h
#define __mmiInfoImage_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "medCommonDefines.h"
#include "mafInteractorPER.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------

/** Class implementing image info reporting on status bar when not interacting.
*/
class MED_COMMON_EXPORT mmiInfoImage : public mafInteractorPER
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
