/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpMMLContourWidget.h,v $
  Language:  C++
  Date:      $Date: 2008-04-28 08:48:42 $
  Version:   $Revision: 1.1 $
  Authors:   Mel Krokos
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medOpMMLContourWidget_h
#define __medOpMMLContourWidget_h

#include "mafDefines.h"

#include "vtkPolyDataSourceWidget.h"
#include "vtkTubeFilter.h"

#include "medOpMMLModelView.h"
#include "medOpMMLParameterView.h"

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
// medOpMMLContourWidget  :
//----------------------------------------------------------------------------
class medOpMMLContourWidget : public vtkPolyDataSourceWidget
{
public:
	void SetPH(medOpMMLParameterView* PH);
	void SetPV(medOpMMLParameterView* PV);
	void SetTH(medOpMMLParameterView* TH);
	void SetTV(medOpMMLParameterView* TV);
	void SetRA(medOpMMLParameterView* RA);
	void SetSN(medOpMMLParameterView* SN);
	void SetSS(medOpMMLParameterView* SS);
	void SetSE(medOpMMLParameterView* SE);
	void SetSW(medOpMMLParameterView* SW);
	void SetModel(medOpMMLModelView* Model);
	void SetNextOperationId(int n);
	int GetNextOperationId();
	int OperationID;
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
	float HandleRadius;
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
	vtkPlaneSource    *PlaneSource;
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
	BOOL m_bRotatingMode;
	BOOL m_bScalingMode;
	BOOL m_bTranslatingMode;
	BOOL m_bCenterMode;
	float bounds[6];
	void ScalingModeOff();
	void ScalingModeOn();
	void RotationModeOff();
	void RotationModeOn();
	void TranslationModeOff();
	void TranslationModeOn();

  // Description:
  // Instantiate the object.
  static medOpMMLContourWidget *New();

  vtkTypeRevisionMacro(medOpMMLContourWidget,vtkPolyDataSourceWidget);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Methods that satisfy the superclass' API.
  virtual void SetEnabled(int);
  virtual void PlaceWidget(double bounds[6]);
  void PlaceWidget()
    {this->Superclass::PlaceWidget();}

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
  vtkSetClampMacro(Representation,int,VTK_PLANE_OFF,VTK_PLANE_SURFACE);
  vtkGetMacro(Representation,int);
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
  vtkSetMacro(NormalToXAxis,int);
  vtkGetMacro(NormalToXAxis,int);
  vtkBooleanMacro(NormalToXAxis,int);
  vtkSetMacro(NormalToYAxis,int);
  vtkGetMacro(NormalToYAxis,int);
  vtkBooleanMacro(NormalToYAxis,int);
  vtkSetMacro(NormalToZAxis,int);
  vtkGetMacro(NormalToZAxis,int);
  vtkBooleanMacro(NormalToZAxis,int);

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
  // Satisfies superclass API.  This returns a pointer to the underlying
  // PolyData.  Make changes to this before calling the initial PlaceWidget()
  // to have the initial placement follow suit.  Or, make changes after the
  // widget has been initialised and call UpdatePlacement() to realise.
  vtkPolyDataSource* GetPolyDataSource();
   
  // Description:
  // Satisfies superclass API.  This will change the state of the widget to
  // match changes that have been made to the underlying PolyDataSource
  void UpdatePlacement(void);

  // Description:
  // Get the handle properties (the little balls are the handles). The 
  // properties of the handles when selected and normal can be 
  // manipulated.
  vtkGetObjectMacro(HandleProperty,vtkProperty);
  vtkGetObjectMacro(SelectedHandleProperty,vtkProperty);
  
  // Description:
  // Get the plane properties. The properties of the plane when selected 
  // and unselected can be manipulated.
  virtual void SetPlaneProperty(vtkProperty*);
  vtkGetObjectMacro(PlaneProperty,vtkProperty);
  vtkGetObjectMacro(SelectedPlaneProperty,vtkProperty);

  // MK BEGIN
  medOpMMLModelView *M; // model
  
  medOpMMLParameterView *CH; // center horizontal translation
  medOpMMLParameterView *CV; // center vertical translation
  medOpMMLParameterView *T; // twist
  medOpMMLParameterView *H; // horizontal translation
  medOpMMLParameterView *V; // vertical translation
  medOpMMLParameterView *N; // north scaling
  medOpMMLParameterView *S; // south scaling
  medOpMMLParameterView *E; // east scaling
  medOpMMLParameterView *W; // west scaling
  // MK END

protected:
  medOpMMLContourWidget();
  ~medOpMMLContourWidget();

// MK Operation
  int Operation;
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
  int State;
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
  float Motion[3];
  // MK END

  // controlling ivars
  int NormalToXAxis;
  int NormalToYAxis;
  int NormalToZAxis;
  int Representation;
  void SelectRepresentation();

  // the plane
  vtkActor          *PlaneActor;
  vtkPolyDataMapper *PlaneMapper;
  
  vtkTubeFilter		*PlaneOutlineTubes; // MK
  vtkPolyData       *PlaneOutline;
  void HighlightPlane(int highlight);

  // glyphs representing hot spots (e.g., handles)
  vtkActor          **Handle;
  vtkPolyDataMapper **HandleMapper;
  vtkSphereSource   **HandleGeometry;

  vtkActor          *RotationalHandle;
  vtkPolyDataMapper *RotationalHandleMapper;
  vtkSphereSource   *RotationalHandleGeometry;

  vtkActor          *CenterHandle;
  vtkPolyDataMapper *CenterHandleMapper;
  vtkSphereSource   *CenterHandleGeometry;

  void HandlesOn(double length);
  void HandlesOff();
  int HighlightHandle(vtkProp *prop); //returns cell id
  virtual void SizeHandles();
  
  // the normal cone
  vtkActor          *ConeActor;
  vtkPolyDataMapper *ConeMapper;
  vtkConeSource     *ConeSource;
  void HighlightNormal(int highlight);

  // the normal line
  vtkActor          *LineActor;
  vtkPolyDataMapper *LineMapper;
  vtkLineSource     *LineSource;

  // the normal cone
  vtkActor          *ConeActor2;
  vtkPolyDataMapper *ConeMapper2;
  vtkConeSource     *ConeSource2;

  // the normal line
  vtkActor          *LineActor2;
  vtkPolyDataMapper *LineMapper2;
  vtkLineSource     *LineSource2;

  // Do the picking
  vtkCellPicker *HandlePicker;
  vtkCellPicker *PlanePicker;
  vtkActor *CurrentHandle;
  
  // Methods to manipulate the hexahedron.
  void Rotate(double *p1, double *p2);
  void Scale(double *p1, double *p2, int X, int Y);
  void Translate(double *p1, double *p2);
  void Push(double *p1, double *p2);
  
  // Plane normal, normalized
  float Normal[3];

  // Transform the hexahedral points (used for rotations)
  vtkTransform *Transform;
  
  // Properties used to control the appearance of selected objects and
  // the manipulator in general.
  vtkProperty *HandleProperty;
  vtkProperty *SelectedHandleProperty;
  vtkProperty *PlaneProperty;
  vtkProperty *SelectedPlaneProperty;
  void CreateDefaultProperties();
  
  void GeneratePlane();

private:
  medOpMMLContourWidget(const medOpMMLContourWidget&);  //Not implemented
  void operator=(const medOpMMLContourWidget&);  //Not implemented
};

#endif
