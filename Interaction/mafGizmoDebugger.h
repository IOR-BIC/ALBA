/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGizmoDebugger.h,v $
  Language:  C++
  Date:      $Date: 2008-04-09 14:18:48 $
  Version:   $Revision: 1.1 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafGizmoDebugger_h
#define __mafGizmoDebugger_h

#include "mafGizmoInterface.h"
#include "mafMatrix.h"
#include "mmiGenericMouse.h"
//---------------------------------------------------------------------------
// forward refs:
//---------------------------------------------------------------------------
class mmiCompositorMouse;
class mmiGenericMouse;
class mafVME;
class mafVMEGizmo;
class vtkPoints;
class vtkDoubleArray;
class mafVMEPolylineSpline;
class mafVMEPolyline;
class mafVME;

class vtkLineSource;
class vtkSphereSource;
class vtkAppendPolyData;

/** Gizmo constrained to arbitrary path...

to use this object instanciate it in your client and listen for ID_TRANSFORM
events with mafGizmoDebugger::ABS_POSE argument; 

REFACTOR NOTE: this object should be the expanded to support mouse interaction
*/

class mafGizmoDebugger : public mafGizmoInterface
{
public:

	mafGizmoDebugger(mafNode* inputVme, mafObserver *Listener = NULL, const char *name = "GizmoPath");

  virtual	~mafGizmoDebugger();
  
  enum SentPoseID {ABS_POSE = mmiGenericMouse::MOUSE_UP+1};

  /** Set position along constraint polyline*/
  void SetCurvilinearAbscissa(double s);
  double GetCurvilinearAbscissa();

  /** Set the constraint polyline: any VME can be provided but its output must be a
  mafVMEOutputPolyline*/
  void SetConstraintPolyline(mafVME* constraintPolyline)
  {
    m_ConstraintPolyline = constraintPolyline;
  }

  /** Show/Hide the gizmos using actor visibility instead of pipe creation/destruction: this is used for faster 
  rendering*/
  void Show(bool show) ;
  
  /** This method is used to change the input: this VME is used only to reparent the gizmo
  to the root*/
  void SetInput(mafVME *vme);

  /** return the gizmo object*/
  mafVMEGizmo *GetOutput() {return m_VmeGizmoDebugger;}; 

  mafMatrix *GetAbsPose();
  
  /** Set the gizmo color */
  void SetColor(double col[3]);

  /** set the length of the gizmo default representation ie a 2d line
  REFACTOR NOTE: graphical representation should be defined by subclasses...
  */
  void SetLength (double lineLength);
  
  /** Set/Get the gizmo moving modality, default is free */
  void SetConstraintModalityToSnapVertex(); // not yet implemented
  void SetConstraintModalityToFree(); 
  void SetConstraintModalityToBounds(); // not yet implemented
  int  GetConstraintModality() {return this->m_ConstraintModality;};
  
  /** Events handling method */
  void OnEvent(mafEventBase *maf_event); 

void Move( mafEvent * e );
protected:

  // create gizmo
  void CreateVMEGizmo();
  void DestroyVMEGizmo();
  

  // move to mafMatrix
  void SetVersor(int axis, double versor[3], mafMatrix &matrix);

  static void BuildVector(double p0[3],double p1[3],double vOut[3]);

  // move somewhere else... [coeff * inVector];
  static void BuildVector(double coeff, const double *inVector, double *outVector);

  static void AddVectors(double p0[3],double p1[3],double sum[3]);

  void Constructor(mafNode *imputVme, mafObserver *listener, const char *name);
  void Destructor();

  void CreateInteractor1();
  void CreateInteractor2();

  mafString           m_Name;
  mafVMEGizmo        *m_VmeGizmoDebugger;
  mmiCompositorMouse *m_GizmoInteractor;
  mmiGenericMouse    *m_LeftMouseInteractor;
  mmgGui             *m_GizmoGui;


  mafVME *m_ConstraintPolyline;

  vtkLineSource *m_LineSource;
  vtkSphereSource  *m_SphereSource;
  vtkAppendPolyData *m_AppendPolyData;
  
  // REFACTOR THIS: these functions are available also on mafGizmoPath, extract methods...
  void FindGizmoAbsPose(double s);

  /** given the constrain vme and the curvilinear abscissa return the mover abs pose to be 
  perpendicular to the constrain */
  void GetConstrainedAbsPose( mafVME *inputConstrain, double s, mafMatrix &moverOutputAbsPose );
  void FindBoundaryVerticesID(double s, int &idMin, int &idMax, double &sFromIdMin);
  void ComputeLocalPointPositionBetweenVertices(double distP0s, int idP0, int idP1, double pOut[3]);
  void ComputeLocalNormal(int idP0, int idP1, double viewUp[3], double normal[3]);
	void LogTransformEvent(mafEvent  *e) ;
	void LogVector3( double *vector );
	
  double m_CurvilinearAbscissa;
  int m_ConstraintModality;

  enum CONSTRAINT_MODALITY_ID
  {
    FREE = 0,
    BOUNDS,
    SNAP_VERTEX,
    CONSTRAINT_MODALITY_NUMBERS,
  };
};
#endif
