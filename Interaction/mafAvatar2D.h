/*=========================================================================

 Program: MAF2
 Module: mafAvatar2D
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafAvatar2D_h
#define __mafAvatar2D_h

#include "mafInteractor.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafDeviceButtonsPadMouse;

/**
  Avatars are entities moving in the virtual world according to user's 
  movements, and thus representing their presence in the virtual world.
  Currently Avatar2D are empty classes used only to store the renderer.
 */
class MAF_EXPORT mafAvatar2D : public mafInteractor
{
public:
  mafTypeMacro(mafAvatar2D,mafInteractor);

  /**  Process events coming from tracker */
  virtual void OnEvent(mafEventBase *event);

  /** return the Mouse device associated to this avatar */
  mafDeviceButtonsPadMouse *GetMouse();

  /** set the mouse to be used for this avatar2D */
  void SetMouse(mafDeviceButtonsPadMouse *mouse);

protected:
  mafAvatar2D();
  virtual ~mafAvatar2D();

private:
  mafAvatar2D(const mafAvatar2D&) {}  // Not implemented.
  void operator=(const mafAvatar2D&) {}  // Not implemented.

  /**
  Test friend */
  friend class mafAvatar2DTest;

};

#endif 
