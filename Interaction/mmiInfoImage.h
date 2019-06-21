/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: mmiInfoImage
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mmiInfoImage_h
#define __mmiInfoImage_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaInteractorPER.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------

/** Class implementing image info reporting on status bar when not interacting.
*/
class ALBA_EXPORT mmiInfoImage : public albaInteractorPER
{
public: 
  albaTypeMacro(mmiInfoImage,albaInteractorPER);

  virtual void OnEvent(albaEventBase *event);

protected:
  mmiInfoImage();
  virtual ~mmiInfoImage();

private:
  mmiInfoImage(const mmiInfoImage&);  // Not implemented.
  void operator=(const mmiInfoImage&);  // Not implemented.
};
#endif 
