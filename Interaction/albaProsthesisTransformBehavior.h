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

#ifndef __albaProsthesisTransformBehavior_h
#define __albaProsthesisTransformBehavior_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaTransformBehavior.h"

//----------------------------------------------------------------------------
//forward ref
//----------------------------------------------------------------------------
class albaDeviceButtonsPadMouse;


class ALBA_EXPORT albaProsthesisTransformBehavior : public albaTransformBehavior
{
public:
  albaTypeMacro(albaProsthesisTransformBehavior, albaTransformBehavior);
  
  /** Set the current camera */
  //virtual void SetCurrentCamera(vtkCamera *camera) {m_CurrentCamera=camera;}

	void Set2DModality(bool mod2d) { m_Modality2D = mod2d; }

protected:

  albaProsthesisTransformBehavior();
  ~albaProsthesisTransformBehavior();

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
