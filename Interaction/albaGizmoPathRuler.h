/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoPathRuler
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaGizmoPathRuler_H__
#define __albaGizmoPathRuler_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaEvent.h"
#include "albaGizmoInterface.h"
#include "albaGUIGizmoTranslate.h"
#include "albaGizmoPath.h"
#include <vector>
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class albaMatrix;

/** A gizmo representing a ruler that can move on a polyline

REFACTOR NOTE: this object should be expanded to support mouse interaction

usage example:

  m_GizmoPathRuler = new albaGizmoPathRuler(m_ConstraintVME, this,10, 5, 30, 5);
  m_GizmoPathRuler->SetColor(red);

  // these should be removed since Show should be enough...
  // anyway for the moment Show is not working as expected so...
  for (int i = 0;i < m_GizmoPathRuler->GetTicksNumber(); i++)
  {
     view->VmeShow(m_GizmoPathRuler->GetOutput(i), true);
  }

  m_GizmoPathRuler->Show(true);

  // set the new constraint to the gizmo
  m_GizmoPathRuler->SetConstraintPolyline(m_ConstraintVME);

  // create the gizmo at s = 0
  m_GizmoPathRuler->SetCurvilinearAbscissa(0);
*/

class ALBA_EXPORT albaGizmoPathRuler : public albaGizmoInterface 
{
public:
  
  albaGizmoPathRuler(albaVME *input, albaObserver* listener = NULL, int ticksNumber = 1, \
      int originTickId = 0, double ticksHeigth = 50, double ticksDistance = 20, bool enableShorterTicks = true);
  virtual ~albaGizmoPathRuler(); 

  /** Set position of the origin tick along constraint polyline; other ticks will follow*/
  void SetCurvilinearAbscissa(double s);
  double GetCurvilinearAbscissa() {return m_CurvilinearAbscissa;};
  double GetGizmoPathCurvilinearAbscissa(int index) 
  {
    if(index >= m_GizmoPathVector.size())
    {
      index = m_GizmoPathVector.size() - 1;
    }
    return m_GizmoPathVector[index]->GetCurvilinearAbscissa();
  };

  /** Set the constraint polyline: any VME can be provided but its output must be a
  albaVMEOutputPolyline*/
  void SetConstraintPolyline(albaVME* constraintPolyline);

  void SetTicksHeigth(double height);
  double GetTicksHeigth() {return m_TicksHeigth;};

  void SetTicksDistance(double distance);
  double GetTicksDistance() {return m_TicksDistance;};

  int GetTicksNumber() {return m_TicksNumber;};

  /** Set the gizmo color */
  void SetColor(double col[3]);
	void SetColor(int idGizmo,double col[3]);

  void SetColor(double abscissa,double col[3]);

  void HighlightExtremes(double col[3], int bound1, int bound2, int center, bool inside = false);
  void HighlightExtremes(double col[3], double bound1, double bound2, double center, bool inside = false);
  

  /** This method is used to change the input: this VME is used only to reparent the gizmo
  to the root*/
  void SetInput(albaVME *vme);

  /**
  Events handling*/        
  void OnEvent(albaEventBase *alba_event);

  /**
  Show the gizmo*/
  void Show(bool show);

  /** return the gizmo object*/
  albaVMEGizmo *GetOutput(int tickId) {return m_GizmoPathVector[tickId]->GetOutput();}; 

  /**
  Set the gizmo pose; not yet implemented!!!*/
  void SetAbsPose(albaMatrix *absPose);

  /** get the abs pose for each tick */
  albaMatrix *GetAbsPose(int tickId);

  /** not yet available... */
  albaGUI *GetGui() {return NULL;};

  void SetGizmoLabelsVisibility(bool value);

  void ResetLabelsVisibility();

  void CustomIndexLabelVisibility(int index, int flag);

protected:

  void BuildGizmos();
  void DestroyGizmos();

  int m_TicksNumber;
  int m_OriginTickID;
  double m_TicksHeigth;
  double m_TicksDistance;
  double m_CurvilinearAbscissa;
  bool m_EnableShorterTicks;

  /** Gizmo gui events handling; not yet implemented... */
  void OnEventGizmoGui(albaEventBase *alba_event);

  /** Gizmo components events handling */
  void OnEventGizmoComponents(albaEventBase *alba_event);
	
  std::vector<albaGizmoPath *> m_GizmoPathVector;


};

#endif
