/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoPath
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaGizmoPath_h
#define __albaGizmoPath_h

#include "albaGizmoInterface.h"
#include "albaMatrix.h"
#include "albaInteractorGenericMouse.h"
//---------------------------------------------------------------------------
// forward refs:
//---------------------------------------------------------------------------
class albaInteractorCompositorMouse;
class albaInteractorGenericMouse;
class albaVME;
class albaVMEGizmo;
class vtkPoints;
class vtkDoubleArray;
class albaVMEPolylineSpline;
class albaVMEPolyline;
class vtkLineSource;
class albaVME;

/** Gizmo constrained to arbitrary path...

to use this object instanciate it in your client and listen for ID_TRANSFORM
events with albaGizmoPath::ABS_POSE argument; 

REFACTOR NOTE: this object should be the expanded to support mouse interaction
*/

class ALBA_EXPORT albaGizmoPath : public albaGizmoInterface
{
public:

	albaGizmoPath(albaVME* inputVme, albaObserver *Listener = NULL, const char *name = "GizmoPath", int textVisibility = 0);

  virtual	~albaGizmoPath();
  
  enum ID_TEXT_SIDE
  {
    ID_LEFT_TEXT_SIDE = 0,
    ID_RIGTH_TEXT_SIDE,
  };

  enum SentPoseID {ABS_POSE = albaInteractorGenericMouse::MOUSE_UP+1};

  /** Set position along constraint polyline*/
  void SetCurvilinearAbscissa(double s);
  double GetCurvilinearAbscissa();

  /** Set the constraint polyline: any VME can be provided but its output must be a
  albaVMEOutputPolyline*/
  void SetConstraintPolyline(albaVME* constraintPolyline)
  {
    m_ConstraintPolyline = constraintPolyline;
  }

  /** Show/Hide the gizmos using actor visibility instead of pipe creation/destruction: this is used for faster 
  rendering*/
  void Show(bool show) ;
  
  /** This method is used to change the input: this VME is used only to reparent the gizmo
  to the root*/
  void SetInput(albaVME *vme);

  /** return the gizmo object*/
  albaVMEGizmo *GetOutput() {return m_VmeGizmoPath;}; 

  albaMatrix *GetAbsPose();
  
  /** Set the gizmo color */
  void SetColor(double col[3]);

  /** set the length of the gizmo default representation ie a 2d line
  REFACTOR NOTE: graphical representation should be defined by subclasses...
  */
  void SetLineLength (double lineLength);
  
  /** Set/Get the gizmo moving modality, default is free */
  void SetConstraintModalityToSnapVertex(); // not yet implemented
  void SetConstraintModalityToFree(); 
  void SetConstraintModalityToBounds(); // not yet implemented
  int  GetConstraintModality() {return this->m_ConstraintModality;};
  
  /** Events handling method */
  void OnEvent(albaEventBase *alba_event); 

  void SetSideTextPosition(bool leftRight = ID_LEFT_TEXT_SIDE){m_TextSidePosition = leftRight;};
  bool GetSideTextPosition(){return m_TextSidePosition;};

protected:

  // create gizmo
  void CreateVMEGizmo();
  void DestroyVMEGizmo();
  

  // move to albaMatrix
  void SetVersor(int axis, double versor[3], albaMatrix &matrix);

  static void BuildVector(double p0[3],double p1[3],double vOut[3]);

  // move somewhere else... [coeff * inVector];
  static void BuildVector(double coeff, const double *inVector, double *outVector);

  static void AddVectors(double p0[3],double p1[3],double sum[3]);

  void Constructor(albaVME *imputVme, albaObserver *listener, const char *name);
  void Destructor();

  void CreateInteractor();

  albaString           m_Name;
  albaVMEGizmo        *m_VmeGizmoPath;
  albaInteractorCompositorMouse *m_GizmoInteractor;
  albaInteractorGenericMouse    *m_LeftMouseInteractor;
  albaGUI             *m_GizmoGui;


  albaVME *m_ConstraintPolyline;

  vtkLineSource *m_LineSource;
  
  
  void FindGizmoAbsPose(double s);
  void FindBoundaryVerticesID(double s, int &idMin, int &idMax, double &sFromIdMin);
  void ComputeLocalPointPositionBetweenVertices(double distP0s, int idP0, int idP1, double pOut[3]);
  void ComputeLocalNormal(int idP0, int idP1, double viewUp[3], double normal[3]);

  double m_CurvilinearAbscissa;
  int m_ConstraintModality;

  int m_TextVisibility;
  bool m_TextSidePosition;

  enum CONSTRAINT_MODALITY_ID
  {
    FREE = 0,
    BOUNDS,
    SNAP_VERTEX,
    CONSTRAINT_MODALITY_NUMBERS,
  };
};
#endif
