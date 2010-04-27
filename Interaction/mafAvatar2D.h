/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafAvatar2D.h,v $
  Language:  C++
  Date:      $Date: 2010-04-27 07:05:34 $
  Version:   $Revision: 1.3.22.2 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
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
