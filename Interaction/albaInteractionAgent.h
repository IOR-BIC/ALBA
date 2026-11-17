/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractionAgent
 Authors: Gianluigi Crimi

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaInteractionAgent_h
#define __albaInteractionAgent_h

#include "albaAgent.h"
#include "vtkALBASmartPointer.h"
#include "vtkType.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaVME;
class albaView;
class vtkRenderer;

/** Common base class of interactors and interaction tools.
  It holds the renderer and the view the object is working on and the helpers
  that do not depend on devices or buttons: the device and button state belong
  to albaInteractionBehavior, the tools receive the events already interpreted by the PER. */
class ALBA_EXPORT albaInteractionAgent : public albaAgent
{
public:
  albaInteractionAgent();
  virtual ~albaInteractionAgent();

  albaAbstractTypeMacro(albaInteractionAgent, albaAgent);

  /**
    Set the renderer and the view this object is attached to. This method is redefined
    in subclasses to perform specialized actions when the renderer is changed */
  virtual void SetRendererAndView(vtkRenderer *ren, albaView *view);
  vtkRenderer *GetRenderer() {return m_Renderer;}
  albaView *GetView() {return m_View;}

protected:
  /** Convert a display point to world coordinates using the current renderer */
  void ComputeDisplayToWorld(double x, double y, double z, double worldPt[4]);

  /** Convert a world point to display coordinates using the current renderer */
  void ComputeWorldToDisplay(double x, double y, double z, double displayPt[3]);

  /**
    Send to the listener the event eventId carrying the picked position as a vtkPoints with one point (GetVtkObj),
    the id of the picked point or cell (GetArg), a scalar value (GetDouble) and the picked VME (GetVme). */
  void SendPickedPointEvent(int eventId, double pos[3], vtkIdType id = 0, double scalar = 0, albaVME *vme = NULL);

  albaView *m_View; ///< the view this object is working on
  vtkALBASmartPointer<vtkRenderer> m_Renderer; ///< the renderer this object is working on

private:
  albaInteractionAgent(const albaInteractionAgent&);  // Not implemented.
  void operator=(const albaInteractionAgent&);  // Not implemented.
};

#endif
