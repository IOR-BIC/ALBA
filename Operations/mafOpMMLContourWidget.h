/*=========================================================================

 Program: MAF2
 Module: mafOpMMLContourWidget
 Authors: Mel Krokos
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpMMLContourWidget_h
#define __mafOpMMLContourWidget_h

#include "mafDefines.h"
#include "mafDecl.h"

#include "vtkPolyDataSourceWidget.h"
#include "vtkTubeFilter.h"
#include "vtkProperty.h"

#include "mafOpMMLModelView.h"
#include "mafOpMMLParameterView.h"

//----------------------------------------------------------------------------
// forward reference :
//----------------------------------------------------------------------------
class vtkActor;
class vtkCellPicker;
class vtkConeSource;
class vtkLineSource;
class vtkPlaneSource;
class vtkPoints;
class vtkPolyData;
class vtkPolyDataMapper;
class vtkProp;
class vtkProperty;
class vtkSphereSource;
class vtkTransform;
class vtkPlane;

#define VTK_PLANE_OFF 0
#define VTK_PLANE_OUTLINE 1
#define VTK_PLANE_WIREFRAME 2
#define VTK_PLANE_SURFACE 3

//----------------------------------------------------------------------------
// mafOpMMLContourWidget  :
//----------------------------------------------------------------------------
class mafOpMMLContourWidget : public vtkPolyDataSourceWidget
{
public:
	void SetPH(mafOpMMLParameterView* PH);
	void SetPV(mafOpMMLParameterView* PV);
	void SetTH(mafOpMMLParameterView* TH);
	void SetTV(mafOpMMLParameterView* TV);
	void SetRA(mafOpMMLParameterView* RA);
	void SetSN(mafOpMMLParameterView* SN);
	void SetSS(mafOpMMLParameterView* SS);
	void SetSE(mafOpMMLParameterView* SE);
	void SetSW(mafOpMMLParameterView* SW);
	void SetModel(mafOpMMLModelView* Model);
	void SetNextOperationId(int n);
	int GetNextOperationId();
	int m_OperationID;
	void SetRotationHandleVisibility();
	void RotationHandleOff();
	void RotationHandleOn();
	void UpdateScalingHandles();
	void UpdateRotationHandle();
	void UpdateWidgetTransform();
	void ComputeCenter(double *c);
	void ComputeBounds(double b[6]);
	void Place(double *p1, double *p2);
	void ScaleWest(double *p1, double *p2);
	void ScaleEast(double *p1, double *p2);
	void ScaleNorth(double *p1, double *p2);
	void ScaleSouth(double *p1, double *p2);
	float GetHandleRadius();
	void SetHandleRadius(float r);
	float m_HandleRadius;
	vtkActor* GetWestScalingHandleActor();
	vtkActor* GetEastScalingHandleActor();
	vtkActor* GetSouthScalingHandleActor();
	vtkActor* GetNorthScalingHandleActor();
	vtkActor* GetRotationalHandleActor();
	vtkActor* GetCenterHandleActor();
	void SetCenterHandle(double *xyz);
	void SetCenterHandle(double x, double y, double z);
	void CenterModeOff();
	void CenterModeOn();
	BOOL GetCenterMode();
	vtkPlaneSource    *m_PlaneSource;
	void PositionHandles(float X, float Y);
	void ScalingHandlesOff();
	void ScalingHandlesOn();
	void SetRotationalHandle(double *xyz);
	void SetRotationalHandle(double x, double y, double z);
	BOOL GetScalingMode();
	BOOL GetRotationMode();
	BOOL GetTranslationMode();
	void SetHandle3(double x, double y, double z);
	void SetHandle2(double x, double y, double z);
	void SetHandle1(double x, double y, double z);
	void SetHandle0(double x, double y, double z);
	void SetHandle3(double* xyz);
	void SetHandle2(double* xyz);
	void SetHandle1(double* xyz);
	void SetHandle0(double* xyz);
	BOOL m_BRotatingMode;
	BOOL m_BScalingMode;
	BOOL m_BTranslatingMode;
	BOOL m_BCenterMode;
	float m_Bounds[6];
	void ScalingModeOff();
	void ScalingModeOn();
	void RotationModeOff();
	void RotationModeOn();
	void TranslationModeOff();
	void TranslationModeOn();

  // Description:
  // Instantiate the object.
  static mafOpMMLContourWidget *New();

  vtkTypeMacro(mafOpMMLContourWidget,vtkPolyDataSourceWidget);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Methods that satisfy the superclass' API.
  virtual void SetEnabled(int);
  virtual void PlaceWidget(double bounds[6]);
  void PlaceWidget()
    {this->Superclass::PlaceWidget();}

	virtual vtkPolyDataAlgorithm* GetPolyDataAlgorithm();

  // Description:
  // Set/Get the resolution (number of subdivisions) of the plane.
  void SetResolution(int r);
  int GetResolution();

  // Description:
  // Set/Get the origin of the plane.
  void SetOrigin(double x, double y, double z);
  void SetOrigin(double x[3]);
  double* GetOrigin();
  void GetOrigin(double xyz[3]);

  // Description:
  // Set/Get the position of the point defining the first axis of the plane.
  void SetPoint1(double x, double y, double z);
  void SetPoint1(double x[3]);
  double* GetPoint1();
  void GetPoint1(double xyz[3]);
  
  // Description:
  // Set/Get the position of the point defining the second axis of the plane.
  void SetPoint2(double x, double y, double z);
  void SetPoint2(double x[3]);
  double* GetPoint2();
  void GetPoint2(double xyz[3]);

  // Description:
  // Get the center of the plane.
  void SetCenter(double x, double y, double z);
  void SetCenter(double x[3]);
  double* GetCenter();
  void GetCenter(double xyz[3]);

  // Description:
  // Get the normal to the plane.
  void SetNormal(double x, double y, double z);
  void SetNormal(double x[3]);
  double* GetNormal();
  void GetNormal(double xyz[3]);

  // MK BEGIN 26/05/04
  // Description:
  // Set/Get the motion vector
  void SetMotionVector(float x, float y, float z);
  void SetMotionVector(float m[3]);
  float* GetMotionVector();
  void GetMotionVector(float xyz[3]);
  // MK END
  

  // Description:
  // Control how the plane appears when GetPolyData() is invoked.
  // If the mode is "outline", then just the outline of the plane
  // is shown. If the mode is "wireframe" then the plane is drawn
  // with the outline plus the interior mesh (corresponding to the
  // resolution specified). If the mode is "surface" then the plane
  // is drawn as a surface.

  //vtkSetClampMacro(Representation,int,VTK_PLANE_OFF,VTK_PLANE_SURFACE);
  virtual void SetRepresentation(int representation); 
  virtual int GetRepresentationMinValue () {return VTK_PLANE_OFF;};
  virtual int GetRepresentationMaxValue () {return VTK_PLANE_SURFACE;};
  virtual int GetRepresentation () {return m_Representation;};
  //vtkGetMacro(Representation,int);
  void SetRepresentationToOff()
    {this->SetRepresentation(VTK_PLANE_OFF);}
  void SetRepresentationToOutline()
    {this->SetRepresentation(VTK_PLANE_OUTLINE);}
  void SetRepresentationToWireframe()
    {this->SetRepresentation(VTK_PLANE_WIREFRAME);}
  void SetRepresentationToSurface()
    {this->SetRepresentation(VTK_PLANE_SURFACE);}

  // Description:
  // Force the plane widget to be aligned with one of the x-y-z axes.
  // Remember that when the state changes, a ModifiedEvent is invoked.
  // This can be used to snap the plane to the axes if it is orginally
  // not aligned.

//   vtkSetMacro(m_NormalToXAxis,int);
//   vtkGetMacro(m_NormalToXAxis,int);
//   vtkBooleanMacro(m_NormalToXAxis,int);
//   vtkSetMacro(NormalToYAxis,int);
//   vtkGetMacro(NormalToYAxis,int);
//   vtkBooleanMacro(NormalToYAxis,int);
//   vtkSetMacro(NormalToZAxis,int);
//   vtkGetMacro(NormalToZAxis,int);
//   vtkBooleanMacro(NormalToZAxis,int);

  // Description:
  // Grab the polydata (including points) that defines the plane.  The
  // polydata consists of (res+1)*(res+1) points, and res*res quadrilateral
  // polygons, where res is the resolution of the plane. These point values
  // are guaranteed to be up-to-date when either the InteractionEvent or
  // EndInteraction events are invoked. The user provides the vtkPolyData and
  // the points and polyplane are added to it.
  void GetPolyData(vtkPolyData *pd);

  // Description:
  // Get the planes describing the implicit function defined by the plane
  // widget. The user must provide the instance of the class vtkPlane. Note
  // that vtkPlane is a subclass of vtkImplicitFunction, meaning that it can
  // be used by a variety of filters to perform clipping, cutting, and
  // selection of data.
  void GetPlane(vtkPlane *plane);

   
  // Description:
  // Satisfies superclass API.  This will change the state of the widget to
  // match changes that have been made to the underlying PolyDataSource
  void UpdatePlacement(void);

  // Description:
  // Get the handle properties (the little balls are the handles). The 
  // properties of the handles when selected and normal can be 
  // manipulated.
//   vtkGetObjectMacro(HandleProperty,vtkProperty);
//   vtkGetObjectMacro(SelectedHandleProperty,vtkProperty);
  
  // Description:
  // Get the plane properties. The properties of the plane when selected 
  // and unselected can be manipulated.
  virtual void Setm_PlaneProperty(vtkProperty*);
  virtual vtkProperty* GetPlaneProperty(){return m_PlaneProperty;};
  virtual vtkProperty* GetSelectedPlaneProperty(){return m_SelectedPlaneProperty;};
  //vtkGetObjectMacro(PlaneProperty,vtkProperty);
  //vtkGetObjectMacro(SelectedPlaneProperty,vtkProperty);

  // MK BEGIN
  mafOpMMLModelView *m_M; // model
  
  mafOpMMLParameterView *m_CH; // center horizontal translation
  mafOpMMLParameterView *m_CV; // center vertical translation
  mafOpMMLParameterView *m_T; // twist
  mafOpMMLParameterView *m_H; // horizontal translation
  mafOpMMLParameterView *m_V; // vertical translation
  mafOpMMLParameterView *m_N; // north scaling
  mafOpMMLParameterView *m_S; // south scaling
  mafOpMMLParameterView *m_E; // east scaling
  mafOpMMLParameterView *m_W; // west scaling
  // MK END

protected:
  mafOpMMLContourWidget();
  ~mafOpMMLContourWidget();

// MK Operation
  int m_Operation;
  enum WidgetOperation
  {
    Placement, // placement
    Translation, // translation
    Rotation, // rotation
    NorthScale, // north scale
    SouthScale, // south scale
	EastScale, // east scale
    WestScale  // west scale
  };
//

//BTX - manage the state of the widget
  int m_State;
  enum WidgetState
  {
    Start=0,
    Moving,
    Scaling,
    Pushing,
    Rotating,
	Centralising,
    Outside
  };
//ETX
    
  //handles the events
  static void ProcessEvents(vtkObject* object, 
                            unsigned long event,
                            void* clientdata, 
                            void* calldata);

  // ProcessEvents() dispatches to these methods.
  void OnLeftButtonDown();
  void OnLeftButtonUp();
  void OnMiddleButtonDown();
  void OnMiddleButtonUp();
  void OnRightButtonDown();
  void OnRightButtonUp();
  void OnMouseMove();

  // MK BEGIN 26/05/04
  // motion vector
  float m_Motion[3];
  // MK END

  // controlling ivars
  int m_NormalToXAxis;
  int m_NormalToYAxis;
  int m_NormalToZAxis;
  int m_Representation;
  void SelectRepresentation();

  // the plane
  vtkActor          *m_PlaneActor;
  vtkPolyDataMapper *m_PlaneMapper;
  
  vtkTubeFilter		*m_PlaneOutlineTubes; // MK
  vtkPolyData       *m_PlaneOutline;
  void HighlightPlane(int highlight);

  // glyphs representing hot spots (e.g., handles)
  vtkActor          **m_Handle;
  vtkPolyDataMapper **m_HandleMapper;
  vtkSphereSource   **m_HandleGeometry;

  vtkActor          *m_RotationalHandle;
  vtkPolyDataMapper *m_RotationalHandleMapper;
  vtkSphereSource   *m_RotationalHandleGeometry;

  vtkActor          *m_CenterHandle;
  vtkPolyDataMapper *m_CenterHandleMapper;
  vtkSphereSource   *m_CenterHandleGeometry;

  void HandlesOn(double length);
  void HandlesOff();
  int HighlightHandle(vtkProp *prop); //returns cell id
  virtual void SizeHandles();
  
  // the normal cone
  vtkActor          *m_ConeActor;
  vtkPolyDataMapper *m_ConeMapper;
  vtkConeSource     *m_ConeSource;
  void HighlightNormal(int highlight);

  // the normal line
  vtkActor          *m_LineActor;
  vtkPolyDataMapper *m_LineMapper;
  vtkLineSource     *m_LineSource;

  // the normal cone
  vtkActor          *m_ConeActor2;
  vtkPolyDataMapper *m_ConeMapper2;
  vtkConeSource     *m_ConeSource2;

  // the normal line
  vtkActor          *m_LineActor2;
  vtkPolyDataMapper *m_LineMapper2;
  vtkLineSource     *m_LineSource2;

  // Do the picking
  vtkCellPicker *m_HandlePicker;
  vtkCellPicker *m_PlanePicker;
  vtkActor *m_CurrentHandle;
  
  // Methods to manipulate the hexahedron.
  void Rotate(double *p1, double *p2);
  void Scale(double *p1, double *p2, int X, int Y);
  void Translate(double *p1, double *p2);
  void Push(double *p1, double *p2);
  
  // Plane normal, normalized
  float m_Normal[3];

  // Transform the hexahedral points (used for rotations)
  vtkTransform *m_Transform;
  
  // Properties used to control the appearance of selected objects and
  // the manipulator in general.
  vtkProperty *m_HandleProperty;
  vtkProperty *m_SelectedHandleProperty;
  vtkProperty *m_PlaneProperty;
  vtkProperty *m_SelectedPlaneProperty;
  void CreateDefaultProperties();
  
  void GeneratePlane();

private:
  mafOpMMLContourWidget(const mafOpMMLContourWidget&);  //Not implemented
  void operator=(const mafOpMMLContourWidget&);  //Not implemented
};

#endif
