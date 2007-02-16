/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGizmoPath.h,v $
  Language:  C++
  Date:      $Date: 2007-02-16 11:45:40 $
  Version:   $Revision: 1.3 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafGizmoPath_h
#define __mafGizmoPath_h

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
class vtkLineSource;
class mafVME;

/** Gizmo constrained to arbitrary path...

to use this object instanciate it in your client and listen for ID_TRANSFORM
events with mafGizmoPath::ABS_POSE argument; 

REFACTOR NOTE: this object should be the expanded to support mouse interaction
*/

class mafGizmoPath : public mafGizmoInterface
{
public:

	mafGizmoPath(mafNode* inputVme, mafObserver *Listener = NULL, const char *name = "GizmoPath");

  virtual	~mafGizmoPath();
  
  enum SentPoseID {ABS_POSE = mmiGenericMouse::MOUSE_UP+1};

  /** Set position along constraint polyline*/
  void SetCurvilinearAbscissa(double s);

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
  mafVMEGizmo *GetOutput() {return m_VmeGizmoPath;}; 

  mafMatrix *GetAbsPose();
  
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
  void OnEvent(mafEventBase *maf_event); 

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

  void CreateInteractor();

  mafString           m_Name;
  mafVMEGizmo        *m_VmeGizmoPath;
  mmiCompositorMouse *m_GizmoInteractor;
  mmiGenericMouse    *m_LeftMouseInteractor;
  mmgGui             *m_GizmoGui;


  mafVME *m_ConstraintPolyline;

  vtkLineSource *m_LineSource;
  
  
  void FindGizmoAbsPose(double s);
  void FindBoundaryVerticesID(double s, int &idMin, int &idMax, double &sFromIdMin);
  void ComputeLocalPointPositionBetweenVertices(double distP0s, int idP0, int idP1, double pOut[3]);
  void ComputeLocalNormal(int idP0, int idP1, double viewUp[3], double normal[3]);

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
