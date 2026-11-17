/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: alba.h,v $
Language:  C++
Date:      $Date: 2011-05-27 07:52:12 $
Version:   $Revision: 1.1.2.2 $
Authors:   Gianluigi Crimi, Nicola Vanella
==========================================================================
Copyright (c) 2010
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __albaGenericMouseFreeTransformBehavior_h
#define __albaGenericMouseFreeTransformBehavior_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaGenericMouseBehavior.h"

//----------------------------------------------------------------------------
//forward ref
//----------------------------------------------------------------------------
class albaDeviceButtonsPadMouse;


class ALBA_EXPORT albaGenericMouseFreeTransformBehavior : public albaGenericMouseBehavior
{
public:
  albaTypeMacro(albaGenericMouseFreeTransformBehavior, albaGenericMouseBehavior);
  
  /** Set the current camera */
  //virtual void SetCurrentCamera(vtkCamera *camera) {m_CurrentCamera=camera;}

	void Set2DModality(bool mod2d) { m_Modality2D = mod2d; }

protected:

  albaGenericMouseFreeTransformBehavior();
  ~albaGenericMouseFreeTransformBehavior();

  //----------------------------------------------------------------------------
  // trackball interaction style stuff 
  //----------------------------------------------------------------------------

  virtual void TrackballRotate();
  //void TrackballTranslate();
	virtual void TrackballRoll();

	bool m_Modality2D;

private:

};
#endif
