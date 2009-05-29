/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpMML3ContourWidget.h,v $
  Language:  C++
  Date:      $Date: 2009-05-29 11:05:29 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Mel Krokos
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medOpMML3ContourWidget_h
#define __medOpMML3ContourWidget_h

#include "mafDefines.h"
#include "medDecl.h"

#include "vtkPolyDataSourceWidget.h"
#include "vtkTubeFilter.h"
#include "vtkProperty.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkSphereSource.h"
#include "vtkConeSource.h"
#include "vtkLineSource.h"
#include "vtkCellPicker.h"
#include "vtkTransform.h"
#include "vtkPlane.h"
#include "vtkProp.h"

#include "medOpMML3ModelView.h"
#include "medOpMML3ParameterView.h"


#define VTK_PLANE_OFF 0
#define VTK_PLANE_OUTLINE 1
#define VTK_PLANE_WIREFRAME 2
#define VTK_PLANE_SURFACE 3


//------------------------------------------------------------------------------
/// medOpMML3ContourWidget. \n
/// Helper class for medOpMML3.
/// This is a vtk polydata widget for manipulating the contour in the view.
//------------------------------------------------------------------------------
class medOpMML3ContourWidget : public vtkPolyDataSourceWidget
{
public:
  // Instantiate the object.
  static medOpMML3ContourWidget *New();

  vtkTypeRevisionMacro(medOpMML3ContourWidget,vtkPolyDataSourceWidget);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Methods that satisfy the superclass' API.
  virtual void SetEnabled(int);
  virtual void PlaceWidget(double bounds[6]);
  void PlaceWidget() {this->Superclass::PlaceWidget();}

  void SetPH(medOpMML3ParameterView* PH) {m_CH = PH ;}
	void SetPV(medOpMML3ParameterView* PV) {m_CV = PV ;}
	void SetTH(medOpMML3ParameterView* TH) {m_H = TH ;}
	void SetTV(medOpMML3ParameterView* TV) {m_V = TV ;}
	void SetRA(medOpMML3ParameterView* RA) {m_T = RA ;}
	void SetSN(medOpMML3ParameterView* SN) {m_N = SN ;}
	void SetSS(medOpMML3ParameterView* SS) {m_S = SS ;}
	void SetSE(medOpMML3ParameterView* SE) {m_E = SE ;}
	void SetSW(medOpMML3ParameterView* SW) {m_W = SW ;}
  void SetModel(medOpMML3ModelView* Model) {m_Model = Model ;}

  void SetNextOperationId(int n) {m_OperationID = n ;}
  int GetNextOperationId() const {return m_OperationID ;}

	void SetRotationHandleVisibility();
	void RotationHandleOff();
	void RotationHandleOn();
	void UpdateScalingHandles();
	void UpdateRotationHandle();
	void UpdateWidgetTransform();

  BOOL GetCenterMode() const {return m_BCenterMode;}
  BOOL GetScalingMode() const {return m_BScalingMode;}
  BOOL GetRotationMode() const {return m_BRotatingMode;}
  BOOL GetTranslationMode() const {return m_BTranslatingMode;}

  void CenterModeOff();
  void CenterModeOn();
  void ScalingModeOff();
  void ScalingModeOn();
  void RotationModeOff();
  void RotationModeOn();
  void TranslationModeOff();
  void TranslationModeOn();

  void ScalingHandlesOff();
  void ScalingHandlesOn();

  // Set/Get the resolution (number of subdivisions) of the plane.
  void SetResolution(int r);
  int GetResolution();

  // Description:
  // Get the plane properties. The properties of the plane when selected 
  // and unselected can be manipulated.
  virtual void Setm_PlaneProperty(vtkProperty*);
  virtual vtkProperty* GetPlaneProperty() const {return m_PlaneProperty;};
  virtual vtkProperty* GetSelectedPlaneProperty(){return m_SelectedPlaneProperty;};



protected:
  // Operation
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

  // Manage the state of the widget
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

  /// constructor
  medOpMML3ContourWidget();

  /// destructor
  ~medOpMML3ContourWidget();

  //handles the events
  static void ProcessEvents(vtkObject* object, unsigned long event, void* clientdata, void* calldata);

  // ProcessEvents() dispatches to these methods.
  void OnLeftButtonDown();
  void OnLeftButtonUp();
  void OnMiddleButtonDown();
  void OnMiddleButtonUp();
  void OnRightButtonDown();
  void OnRightButtonUp();
  void OnMouseMove();


  void ComputeCenter(double *c);
  void ComputeBounds(double b[6]);

  void Place(double *p1, double *p2);
  void ScaleWest(double *p1, double *p2);
  void ScaleEast(double *p1, double *p2);
  void ScaleNorth(double *p1, double *p2);
  void ScaleSouth(double *p1, double *p2);

  float GetHandleRadius() const {return m_HandleRadius;}
  void SetHandleRadius(float r) {m_HandleRadius = r ;}

  vtkActor* GetCenterHandleActor() const {return m_CenterHandle;}
  vtkActor* GetRotationalHandleActor() const {return m_RotationalHandle;}
  vtkActor* GetWestScalingHandleActor() const {return m_Handle[2];}
  vtkActor* GetEastScalingHandleActor() const {return m_Handle[1];}
  vtkActor* GetSouthScalingHandleActor() const {return m_Handle[0];}
  vtkActor* GetNorthScalingHandleActor() const {return m_Handle[3];}

  void SetCenterHandle(double *xyz);
  void SetCenterHandle(double x, double y, double z);

  void PositionHandles(float X, float Y);
  void SetRotationalHandle(double *xyz);
  void SetRotationalHandle(double x, double y, double z);
  void SetHandle3(double x, double y, double z);
  void SetHandle2(double x, double y, double z);
  void SetHandle1(double x, double y, double z);
  void SetHandle0(double x, double y, double z);
  void SetHandle3(double* xyz);
  void SetHandle2(double* xyz);
  void SetHandle1(double* xyz);
  void SetHandle0(double* xyz);


  // Set/Get the origin of the plane.
  void SetOrigin(double x, double y, double z);
  void SetOrigin(double x[3]);
  double* GetOrigin();
  void GetOrigin(double xyz[3]);

  // Set/Get the position of the point defining the first axis of the plane.
  void SetPoint1(double x, double y, double z);
  void SetPoint1(double x[3]);
  double* GetPoint1();
  void GetPoint1(double xyz[3]);

  // Set/Get the position of the point defining the second axis of the plane.
  void SetPoint2(double x, double y, double z);
  void SetPoint2(double x[3]);
  double* GetPoint2();
  void GetPoint2(double xyz[3]);

  // Get the center of the plane.
  void SetCenter(double x, double y, double z);
  void SetCenter(double x[3]);
  double* GetCenter();
  void GetCenter(double xyz[3]);

  // Get the normal to the plane.
  void SetNormal(double x, double y, double z);
  void SetNormal(double x[3]);
  double* GetNormal();
  void GetNormal(double xyz[3]);

  // Set/Get the motion vector
  void SetMotionVector(float x, float y, float z);
  void SetMotionVector(float m[3]);
  float* GetMotionVector();
  void GetMotionVector(float xyz[3]);


  // Description:
  // Control how the plane appears when GetPolyData() is invoked.
  // If the mode is "outline", then just the outline of the plane
  // is shown. If the mode is "wireframe" then the plane is drawn
  // with the outline plus the interior mesh (corresponding to the
  // resolution specified). If the mode is "surface" then the plane
  // is drawn as a surface.

  //vtkSetClampMacro(Representation,int,VTK_PLANE_OFF,VTK_PLANE_SURFACE);
  virtual void SetRepresentation(int representation); 
  virtual int GetRepresentationMinValue () {return VTK_PLANE_OFF;}
  virtual int GetRepresentationMaxValue () {return VTK_PLANE_SURFACE;}
  virtual int GetRepresentation () {return m_Representation;}
  //vtkGetMacro(Representation,int)
  void SetRepresentationToOff() {this->SetRepresentation(VTK_PLANE_OFF);}
  void SetRepresentationToOutline() {this->SetRepresentation(VTK_PLANE_OUTLINE);}
  void SetRepresentationToWireframe() {this->SetRepresentation(VTK_PLANE_WIREFRAME);}
  void SetRepresentationToSurface() {this->SetRepresentation(VTK_PLANE_SURFACE);}

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
  // Satisfies superclass API.  This returns a pointer to the underlying
  // PolyData.  Make changes to this before calling the initial PlaceWidget()
  // to have the initial placement follow suit.  Or, make changes after the
  // widget has been initialised and call UpdatePlacement() to realise.
  vtkPolyDataSource* GetPolyDataSource() {return m_PlaneSource ;}


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


  /// Create and set default properties. \n
  /// NB This allocates vtk objects. \n
  /// You must call DeletedefDefaultProperties() when finished.
  void CreateDefaultProperties();

  /// Delete default properties
  void DeleteDefaultProperties() ;

  void SelectRepresentation();

  void GeneratePlane();

  void HighlightPlane(int highlight);

  void HandlesOn(double length);
  void HandlesOff();
  int HighlightHandle(vtkProp *prop); //returns cell id
  virtual void SizeHandles();

  void HighlightNormal(int highlight);

  // Methods to manipulate the hexahedron.
  void Rotate(double *p1, double *p2);
  void Scale(double *p1, double *p2, int X, int Y);
  void Translate(double *p1, double *p2);
  void Push(double *p1, double *p2);



  //----------------------------------------------------------------------------
  // Member variables
  //----------------------------------------------------------------------------
  medOpMML3ModelView *m_Model; ///< model

  medOpMML3ParameterView *m_CH; ///< center horizontal translation
  medOpMML3ParameterView *m_CV; ///< center vertical translation
  medOpMML3ParameterView *m_T; ///< twist
  medOpMML3ParameterView *m_H; ///< horizontal translation
  medOpMML3ParameterView *m_V; ///< vertical translation
  medOpMML3ParameterView *m_N; ///< north scaling
  medOpMML3ParameterView *m_S; ///< south scaling
  medOpMML3ParameterView *m_E; ///< east scaling
  medOpMML3ParameterView *m_W; ///< west scaling

  float m_HandleRadius;

  vtkPlaneSource *m_PlaneSource;

  BOOL m_BRotatingMode;
  BOOL m_BScalingMode;
  BOOL m_BTranslatingMode;
  BOOL m_BCenterMode;
  float m_Bounds[6];

  int m_Operation;
  int m_State;

  int m_OperationID;

  // motion vector
  float m_Motion[3];

  // controlling ivars
  int m_NormalToXAxis;
  int m_NormalToYAxis;
  int m_NormalToZAxis;
  int m_Representation;

  // the plane
  vtkActor          *m_PlaneActor;
  vtkPolyDataMapper *m_PlaneMapper;
  
  vtkTubeFilter		*m_PlaneOutlineTubes; // MK
  vtkPolyData       *m_PlaneOutline;

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

  // the normal cone
  vtkActor          *m_ConeActor;
  vtkPolyDataMapper *m_ConeMapper;
  vtkConeSource     *m_ConeSource;

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

private:
  medOpMML3ContourWidget(const medOpMML3ContourWidget&);  //Not implemented
  void operator=(const medOpMML3ContourWidget&);  //Not implemented

};

#endif
