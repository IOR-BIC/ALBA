/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGizmoHandle.h,v $
  Language:  C++
  Date:      $Date: 2008-07-01 13:47:19 $
  Version:   $Revision: 1.6 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/


#ifndef __mafGizmoHandle_H__
#define __mafGizmoHandle_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafObserver.h"
#include "mafMatrix.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mmiGenericMouse;
class mmiCompositorMouse;
class mafVME;
class vtkCubeSource;
class vtkCylinderSource;
class vtkTransformPolyDataFilter;
class vtkTransform;
class mafVMEGizmo;
class vtkMatrix4x4;
class vtkPlane;
class vtkPlaneSource;

//----------------------------------------------------------------------------
// constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// mafGizmoHandle :
//----------------------------------------------------------------------------
/**           
               ZMAX
               ---
           _  |___|  _
          |           |
         _     ___     _
        | |   |   |   | |           z
    XMIN| |   |   |   | | XMAX      ^
         -     ---     -            |
               YMIN                 |
          |_   ___   _|             X-->x
              |   |                 y
               ---                  
   XZ          ZMIN                
  ---------------------------------
                
               YMAX
               ---
           _  |___|  _
          |           |
         _     ___     _
        | |   |   |   | |
    XMIN| |   |   |   | | XMAX
         -     ---     -
               ZMAX
          |_   ___   _|
              |   |
               ---          
               YMIN
  XY
  ----------------------------------

  Create a gizmo handle from the bounding box of the vme passed in the constructor.
  Handle initial position and moving constrains are set depending on the handle 
  type which can be mafGizmoHandle::XMIN, XMAX, YMIN, YMAX, ZMIN, ZMAX.
  @sa mafGizmoROI mafOpCrop 
*/

class mafGizmoHandle: public mafObserver 
{
public:
           mafGizmoHandle(mafVME *input, mafObserver *listener = NULL, int constraintModality=BOUNDS,mafVME *parent=NULL, bool showShadingPlane = false);
  virtual ~mafGizmoHandle(); 
  
  /** Set the gizmo generating vme; the gizmo will be centered on this vme*/
  void SetInput(mafVME *vme); 
  mafVME *GetInput() {return this->m_InputVme;};

  //----------------------------------------------------------------------------
  // events handling 
  //----------------------------------------------------------------------------
  
  /** Set the event receiver object*/
  void SetListener(mafObserver *Listener) {m_Listener = Listener;};
  
  /** Events handling*/        
  virtual void OnEvent(mafEventBase *maf_event);
    
  //----------------------------------------------------------------------------
  // highlight and show 
  //----------------------------------------------------------------------------

  /** Highlight the gizmo*/
  void Highlight(bool highlight);
    
  /** Show the gizmo */
  void Show(bool show);
  
  /** Set/Get the length of the handle*/
  void   SetLength(double length);
  double GetLength();
   
  /** Set the gizmo abs pose */
  void SetAbsPose(mafMatrix *absPose);
  mafMatrix *GetAbsPose();

  /** Set the gizmo local pose*/
  void SetPose(mafMatrix *pose);
  mafMatrix *GetPose();

  /** Set the constrain ref sys */
  void SetConstrainRefSys(mafMatrix *constrain);
	
	enum CONSTRAINT_MODALITY {BOUNDS = 0, FREE};

  /** Set the pivot matrix */
  void SetPivotMatrix(mafMatrix &matrix) {m_PivotMatrix = matrix;};
  
  /** Get the pivot matrix */
  mafMatrix &GetPivotMatrix() {return m_PivotMatrix;};

  enum GIZMOTYPE {XMIN = 0, XMAX, YMIN, YMAX, ZMIN, ZMAX};
  
  /** Set the gizmo type. 
  Type can be mafGizmoHandle::XMIN, XMAX, YMIN, YMAX, ZMIN, ZMAX.
  Place the gizmo on the bounding box and set its constrain*/
  void SetType(int type);
  
  /** Return the gizmo's type*/
  int GetType() {return m_GizmoType;};

  /** Set the bounding box centers from its bounds*/
  void SetBBCenters(double bounds[6]);
  
  /** Set translation intervals*/
  void SetTranslationIntervals(double bounds[6]);

  /** Return the center of the handle*/
	void GetHandleCenter(int type, double HandleCenter[3]);

  /** Since handles position has changed shading plane bounds must be recomputed */
  void UpdateShadingPlaneDimension(double b[6]);

  void ShowShadingPlane(bool show);

  bool GetShowShadingPlaneOn(){return m_ShowShadingPlane;};

protected:

  mafVME *m_InputVme;///<Register input vme

  vtkCubeSource *m_Cube;///<Cube source

  mafVMEGizmo *m_BoxGizmo; 
  mafVMEGizmo *m_ShadingPlaneGizmo;

  vtkTransformPolyDataFilter *m_TranslateBoxPolyDataFilter;///<translate PDF for box
  vtkTransformPolyDataFilter *m_TranslateShadingPlanePolyDataFilter;///<translate PDF for box

  vtkTransform *m_TranslateBoxTr;///<translation transform for box
  vtkTransform *m_TranslateShadingPlaneTr;///<translation transform for box

	vtkTransformPolyDataFilter *m_TranslateBoxPolyDataFilterEnd;///<translate PDF for box
  vtkTransformPolyDataFilter *m_TranslateShadingPlanePolyDataFilterEnd;///<translate PDF for box

	vtkTransform *m_TranslateBoxTrEnd;///<translation transform for box
  vtkTransform *m_TranslateShadingPlaneTrEnd;///<translation transform for box

  vtkTransformPolyDataFilter *m_RotateBoxPolyDataFilter;///<rotate PDF for box
  vtkTransformPolyDataFilter *m_RotateShadingPlanePolyDataFilter;///<rotate PDF for box

  vtkTransform *m_RotateBoxTr;///<rotation transform for box
  vtkTransform *m_RotateShadingPlaneTr;///<rotation transform for box
  
  /** Create vtk objects needed*/
  void CreatePipeline();

  mmiCompositorMouse *m_IsaComp; ///< Interactor style compositor

  mmiGenericMouse *m_IsaGen; ///< Generic Interactor style

  /** Set the gizmo color */
  void SetColor(double col[3]);
  
  /** Set the gizmo color */
  void SetColor(double colR, double colG, double colB);
  
  mafObserver *m_Listener; ///<Register the event receiver object

  int m_GizmoType; ///<Register the gizmo type

  double m_BBCenters[6][3];
  double m_TranslationIntervals[6][2];

  mafMatrix m_PivotMatrix; ///<The Pivot Matrix

  void Update();

	double m_CubeSize;

	int m_ConstraintModality;

  bool m_ShowShadingPlane;

  vtkPlaneSource *m_PlaneSource;
  vtkPlane *m_ShadingPlane;

  double m_ShadingPlaneDimension[3];

  /** friend test */
  friend class mafGizmoROITest;

};
#endif
