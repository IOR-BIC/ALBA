/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoHandle
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#ifndef __albaGizmoHandle_H__
#define __albaGizmoHandle_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaEvent.h"
#include "albaObserver.h"
#include "albaServiceClient.h"
#include "albaAbsLogicManager.h"
#include "albaMatrix.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaInteractorGenericMouse;
class albaInteractorCompositorMouse;
class albaVME;
class vtkCubeSource;
class vtkCylinderSource;
class vtkTransformPolyDataFilter;
class vtkTransform;
class albaVMEGizmo;
class vtkMatrix4x4;
class vtkPlane;
class vtkPlaneSource;

//----------------------------------------------------------------------------
// constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// albaGizmoHandle :
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
  type which can be albaGizmoHandle::XMIN, XMAX, YMIN, YMAX, ZMIN, ZMAX.
  @sa albaGizmoROI albaOpCrop 
*/

class ALBA_EXPORT albaGizmoHandle: public albaObserver, public albaServiceClient
{
public:

  /** Create a handle, input vme must be different from NULL and default handle type
  is XMIN */
  albaGizmoHandle(albaVME *input, albaObserver *listener = NULL,\
    int constraintModality=BOUNDS,albaVME *parent=NULL, bool showShadingPlane = false);
  virtual ~albaGizmoHandle(); 
  
  /** Set the gizmo generating vme; the gizmo will be centered on this vme*/
  void SetInput(albaVME *vme); 
  albaVME *GetInput() {return this->m_InputVme;};

  //----------------------------------------------------------------------------
  // events handling 
  //----------------------------------------------------------------------------
  
  /** Set the event receiver object*/
  void SetListener(albaObserver *Listener) {m_Listener = Listener;};

  /** Get the event receiver object*/
  albaObserver *GetListener() {return m_Listener;};

  /** Events handling*/        
  virtual void OnEvent(albaEventBase *alba_event);
    
  //----------------------------------------------------------------------------
  // highlight and show 
  //----------------------------------------------------------------------------

  /** Highlight the gizmo: this change its color to yellow*/
  void Highlight(bool highlight);
    
  /** Show the gizmo */
  void Show(bool show);
  void ShowOn() {this->Show(true);};
  void ShowOff() {this->Show(false);};
  bool GetShow() {return m_Show;};

  /** Set/Get the length of the handle 
  The handle dimensions are: (Length) x (Length) ax (Length/2) */
  void   SetLength(double length);
  double GetLength();
   
  /** Set the gizmo abs pose */
  void SetAbsPose(albaMatrix *absPose);
  /** Get the gizmo abs pose */
  albaMatrix *GetAbsPose();

  /** Set the gizmo local pose*/
  void SetPose(albaMatrix *pose);
  /** Get the gizmo abs pose */
  albaMatrix *GetPose();

  /** Set the constrain ref sys */
  void SetConstrainRefSys(albaMatrix *constrain);
  /** Get the constrain ref sys */
  albaMatrix *GetConstrainRefSys();

	enum CONSTRAINT_MODALITY {BOUNDS = 0, FREE};


  /** DEPRECATED: To be removed!  
  Set the pivot matrix */
  void SetPivotMatrix(albaMatrix &matrix) {m_PivotMatrix = matrix;};
  
  /** DEPRECATED: To be removed! 
  Get the pivot matrix */
  albaMatrix &GetPivotMatrix() {return m_PivotMatrix;};

  enum GIZMOTYPE {XMIN = 0, XMAX, YMIN, YMAX, ZMIN, ZMAX};
  
  /** Set the gizmo type. 
  Type can be albaGizmoHandle::XMIN, XMAX, YMIN, YMAX, ZMIN, ZMAX.
  Place the gizmo on the bounding box and set its constrain*/
  void SetType(int type);
  
  /** Return the gizmo's type*/
  int GetType() {return m_GizmoType;};
  
  /** Show shading plane */
  void ShowShadingPlane(bool show);
  void ShowShadingPlaneOn() {this->ShowShadingPlane(true);};
  void ShowShadingPlaneOff() {this->ShowShadingPlane(false);};
  bool GetShowShadingPlaneOn(){return m_ShowShadingPlane;};
  
  /** Since handles position has changed shading plane bounds must be recomputed */
  void UpdateShadingPlaneDimension(double b[6]);

  /** Update the handle position given the bounding box */
  void SetBounds(double bounds[6]);
  void GetBounds(double bounds[6]);

  /** Return the center of the handle*/
  void GetHandleCenter(int type, double HandleCenter[3]);
  
  /** DEPRECATED: To be removed, use SetBounds instead*/
  void SetBBCenters(double bounds[6]);

protected:
  
  /** Set translation intervals*/
  void SetTranslationIntervals(double bounds[6]);

  albaVME *m_InputVme;///<Register input vme

  vtkCubeSource *m_Cube;///<Cube source

  albaVMEGizmo *m_BoxGizmo; 
  albaVMEGizmo *m_ShadingPlaneGizmo;

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

  albaInteractorCompositorMouse *m_IsaComp; ///< Interactor style compositor

  albaInteractorGenericMouse *m_IsaGen; ///< Generic Interactor style

  /** Set the gizmo color */
  void SetColor(double col[3]);
  
  /** Set the gizmo color */
  void SetColor(double colR, double colG, double colB);
  
  /** Get the gizmo color */
  void GetColor(double color[3]);

  albaObserver *m_Listener; ///<Register the event receiver object

  int m_GizmoType; ///<Register the gizmo type

  double m_BBCenters[6][3];
  double m_TranslationIntervals[6][2];

  albaMatrix m_PivotMatrix; ///<The Pivot Matrix

  void Update();

	double m_CubeSize;

	int m_ConstraintModality;

  bool m_ShowShadingPlane;

  vtkPlaneSource *m_PlaneSource;
  vtkPlane *m_ShadingPlane;

  double m_ShadingPlaneDimension[3];
  
  double m_Color[3];
  
  bool m_Show;
  
  double m_Bounds[6];


  /** friend test */
  friend class albaGizmoROITest;
  friend class albaGizmoHandleTest;

};
#endif
