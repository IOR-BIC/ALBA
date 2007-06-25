/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGizmoHandle.h,v $
  Language:  C++
  Date:      $Date: 2007-06-25 10:02:15 $
  Version:   $Revision: 1.2 $
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
  @sa mafGizmoROI mmoCrop 
*/

class mafGizmoHandle: public mafObserver 
{
public:
           mafGizmoHandle(mafVME *input, mafObserver *listener = NULL, int constraintModality=BOUNDS,mafVME *parent=NULL);
  virtual ~mafGizmoHandle(); 
  
  /** 
  Set the gizmo generating vme; the gizmo will be centered on this vme*/
  void SetInput(mafVME *vme); 
  mafVME *GetInput() {return this->InputVme;};

  //----------------------------------------------------------------------------
  // events handling 
  //----------------------------------------------------------------------------
  
  /** Set the event receiver object*/
  void  SetListener(mafObserver *Listener) {m_Listener = Listener;};
  
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
   
  /** 
  Set the gizmo abs pose */
  void SetAbsPose(mafMatrix *absPose);
  mafMatrix *GetAbsPose();

  /**
  Set the gizmo local pose*/
  void SetPose(mafMatrix *pose);
  mafMatrix *GetPose();

  /** 
  Set the constrain ref sys */
  void SetConstrainRefSys(mafMatrix *constrain);
	
	enum CONSTRAINT_MODALITY {BOUNDS = 0, FREE};

  /**
  Set/Get the pivot matrix */
  void SetPivotMatrix(mafMatrix &matrix) {PivotMatrix = matrix;};
  mafMatrix &GetPivotMatrix() {return PivotMatrix;};

  enum GIZMOTYPE {XMIN = 0, XMAX, YMIN, YMAX, ZMIN, ZMAX};
  /**
  Set the gizmo type: type can be mafGizmoHandle::XMIN, XMAX, YMIN, YMAX, ZMIN, ZMAX.
  Place the gizmo on the bounding box and set its constrain*/
  void SetType(int type);
  int GetType() {return GizmoType;};

  /**
  Set the bounding box centers from its bounds*/
  void SetBBCenters(double bounds[6]);
  
  /**
  Set translation intervals*/
  void SetTranslationIntervals(double bounds[6]);

	void GetHandleCenter(int type,double HandleCenter[3]);

protected:

  /** Register input vme*/
  mafVME *InputVme;
  
  /** Cube source*/
  vtkCubeSource *Cube;

  /** Box gizmo */
  mafVMEGizmo *BoxGizmo;

  /** translate PDF for box*/
  vtkTransformPolyDataFilter *TranslateBoxPDF;

  /** translation transform for box*/
  vtkTransform *TranslateBoxTr;

	/** translate PDF for box*/
	vtkTransformPolyDataFilter *TranslateBoxPDFEnd;

	/** translation transform for box*/
	vtkTransform *TranslateBoxTrEnd;

  /** rotate PDF for box*/
  vtkTransformPolyDataFilter *RotateBoxPDF;

  /** rotation transform for box*/
  vtkTransform *RotateBoxTr;
  
  /** Create vtk objects needed*/
  void CreatePipeline();

  /** isa compositor*/
  mmiCompositorMouse *IsaComp;

  /** isa generic*/
  mmiGenericMouse *IsaGen;

  /** Set the gizmo color */
  void SetColor(double col[3]);
  void SetColor(double colR, double colG, double colB);
  
  /**
  Register the event receiver object*/
  mafObserver *m_Listener;

  /**
  Register the gizmo type*/
  int GizmoType;

  double BBCenters[6][3];
  double TranslationIntervals[6][2];

  /**
  The Pivot Matrix */
  mafMatrix PivotMatrix;

  void Update();

	double cubeSize;

	int m_ConstraintModality;
};
#endif
