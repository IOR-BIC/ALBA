/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpMML3ContourWidget
 Authors: Mel Krokos
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpMML3ContourWidget_h
#define __albaOpMML3ContourWidget_h

#include "albaDefines.h"
#include "albaDecl.h"

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

#include "albaOpMML3ModelView.h"
#include "albaOpMML3ParameterView.h"


#define VTK_PLANE_OFF 0
#define VTK_PLANE_WIREFRAME 1
#define VTK_PLANE_SURFACE 2


//------------------------------------------------------------------------------
/// albaOpMML3ContourWidget. \n
/// Helper class for albaOpMML3. \n
/// This is a vtk polydata widget for manipulating the contour in the view.
//
// Handle 0 is S (green)
//   "    1 is E (blue)
//   "    2 is W (magenta)
//   "    3 is N (red)
//
// The widget should not control the model view's visual pipe components directly.
// It sets the spline values, which are read by the model view when updating.
// The model view uses the spline values to calculate the transforms which control the view.
//
// Transforms applied to widget and model view components (C is translation by original contour centres)
// Rot. handle              CP
// Contour axes             CPR
// Cutting planes transform C T^-1
// Contour segments         CPRST C^-1
// Widget plane             CPR
//
// Note that because T positions the cutting planes, which affect S, 
// the user cannot return to T after an S  has been performed.
//------------------------------------------------------------------------------
class albaOpMML3ContourWidget : public vtkPolyDataSourceWidget
{
public:
  /// Instantiate the object.
  static albaOpMML3ContourWidget *New();

  vtkTypeMacro(albaOpMML3ContourWidget,vtkPolyDataSourceWidget);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Methods that satisfy the superclass' API.
  virtual void SetEnabled(int);
  virtual void PlaceWidget(double bounds[6]);

	virtual vtkPolyDataAlgorithm* GetPolyDataAlgorithm();


  //----------------------------------------------------------------------------
  // Methods which set parameter and model view classes
  //----------------------------------------------------------------------------
  void SetPH(albaOpMML3ParameterView* PH) {m_PH = PH ;} ///< set the place horizontal parameter view
	void SetPV(albaOpMML3ParameterView* PV) {m_PV = PV ;} ///< set the place vertical parameter view
	void SetTH(albaOpMML3ParameterView* TH) {m_TH = TH ;} ///< set the translate horizontal parameter view
	void SetTV(albaOpMML3ParameterView* TV) {m_TV = TV ;} ///< set the translate vertical parameter view
	void SetRA(albaOpMML3ParameterView* RA) {m_RA = RA ;} ///< set the rotate parameter view
	void SetSN(albaOpMML3ParameterView* SN) {m_SN = SN ;} ///< set the scale north parameter view
	void SetSS(albaOpMML3ParameterView* SS) {m_SS = SS ;} ///< set the scale south parameter view
	void SetSE(albaOpMML3ParameterView* SE) {m_SE = SE ;} ///< set the scale east parameter view
	void SetSW(albaOpMML3ParameterView* SW) {m_SW = SW ;} ///< set the scale west parameter view

  void SetModel(albaOpMML3ModelView* Model) {m_Model = Model ;} ///< Set the model view


  //----------------------------------------------------------------------------
  // Update methods
  // These are the only methods which can change the position of the handles
  //----------------------------------------------------------------------------
  void UpdateScalingHandles();  ///< Update position of scaling handles
  void UpdateRotationHandle();  ///< Update position of rotation handle
  void UpdateWidgetTransform(); ///< Update position of plane


  //----------------------------------------------------------------------------
  // Visibility methods
  //----------------------------------------------------------------------------
	void SetRotationHandleVisibility();
	void RotationHandleOff();
	void RotationHandleOn();
  void ScalingHandlesOff();
  void ScalingHandlesOn();


  //----------------------------------------------------------------------------
  // Mode methods
  //----------------------------------------------------------------------------
  void PlaceModeOff(); ///< switch place mode on//off
  void PlaceModeOn(); ///< switch place mode on//off
  void ScalingModeOff(); ///< switch scaling mode on//off
  void ScalingModeOn(); ///< switch scaling mode on//off
  void RotationModeOff(); ///< switch rotation mode on//off
  void RotationModeOn(); ///< switch rotation mode on//off
  void TranslationModeOff(); ///< switch translation mode on//off
  void TranslationModeOn(); ///< switch translation mode on//off

  BOOL GetCenterMode() const {return m_PlaceMode;} ///< is place mode on
  BOOL GetScalingMode() const {return m_ScalingMode;} ///< is scaling mode on
  BOOL GetRotationMode() const {return m_RotatingMode;} ///< is rotation mode on
  BOOL GetTranslationMode() const {return m_TranslationMode;} ///< is translation mode on


  void SetNextOperationId(int n) {m_OperationID = n ;}
  int GetNextOperationId() const {return m_OperationID ;}


  void SetResolution(int r);  ///< Set the resolution (number of subdivisions) of the plane.
  int GetResolution();  ///< Get the resolution (number of subdivisions) of the plane.

  /// Get the plane properties. \n
  /// The properties of the plane when selected and unselected can be manipulated.
  virtual void Setm_PlaneProperty(vtkProperty*);
  virtual vtkProperty* GetPlaneProperty() const {return m_PlaneProperty;};
  virtual vtkProperty* GetSelectedPlaneProperty(){return m_SelectedPlaneProperty;};



protected:
  /// Operation
  enum WidgetOperation
  {
    NoOp = 0,    ///< no operation
    Placement,   ///< placement (translation of contour and widget together)
    Translation, ///< translation (translation of contour with respect to widget, along widget axes)
    Rotation,    ///< rotation
    NorthScale,  ///< north scale
    SouthScale,  ///< south scale
    EastScale,   ///< east scale
    WestScale    ///< west scale
  };

  /// Manage the state of the widget
  enum WidgetState
  {
    Start=0,    ///< set at start and when mouse button is raised
    Moving,     ///< set when object picked, except for rotation handle
    Rotating,   ///< set when rotation handle picked
    Scaling,    ///< nothing sets this state
    Pushing,    ///< nothing sets this state
    Outside     ///< set when nothing is picked
  };

  /// constructor
  albaOpMML3ContourWidget();

  /// destructor
  ~albaOpMML3ContourWidget();

  /// handles the events
  static void ProcessEvents(vtkObject* object, unsigned long event, void* clientdata, void* calldata);

  /// ProcessEvents() dispatches to these methods.
  void OnLeftButtonDown();
  void OnLeftButtonUp();
  void OnMiddleButtonDown();
  void OnMiddleButtonUp();
  void OnRightButtonDown();
  void OnRightButtonUp();
  void OnMouseMove();



  //----------------------------------------------------------------------------
  /// Methods which set the spline values. \n
  /// Inputs are the previous and current positions of the mouse. \n
  /// The mouse position is projected onto the widget axis and a scaling value calculated. \n
  /// The splines are updated and used to update the widget and the contour. \n
  //----------------------------------------------------------------------------
  void ScaleNorth(double *p1, double *p2); ///< set the north scaling
  void ScaleSouth(double *p1, double *p2); ///< set the south scaling
  void ScaleWest(double *p1, double *p2); ///< set the west scaling
  void ScaleEast(double *p1, double *p2); ///< set the east scaling
  void Place(double *p1, double *p2); ///< set the place vector
  void Translate(double *p1, double *p2); ///< set the translate vector
  void Rotate(double *p1, double *p2); ///< set the rotation value


  //----------------------------------------------------------------------------
  // These methods are only called by widget states which don't occur
  //----------------------------------------------------------------------------
  void Scale(double *p1, double *p2, int X, int Y); ///< Scale the hexahedron
  void Push(double *p1, double *p2);                ///< Push the hexahedron


  float GetHandleRadius() const {return m_HandleRadius;}
  void SetHandleRadius(float r) {m_HandleRadius = r ;}


  //----------------------------------------------------------------------------
  // Methods for positioning the handles
  //----------------------------------------------------------------------------
  void PositionHandles(float X, float Y); ///< Nigel: this method currently does nothing

  void SetHandle0(double* xyz);                            ///< set position of handle
  void SetHandle0(double x, double y, double z);           ///< set position of handle 
  void SetHandle1(double* xyz);                            ///< set position of handle
  void SetHandle1(double x, double y, double z);           ///< set position of handle
  void SetHandle2(double* xyz);                            ///< set position of handle
  void SetHandle2(double x, double y, double z);           ///< set position of handle
  void SetHandle3(double* xyz);                            ///< set position of handle
  void SetHandle3(double x, double y, double z);           ///< set position of handle
  void SetRotationalHandle(double *xyz);                   ///< set position of handle
  void SetRotationalHandle(double x, double y, double z);  ///< set position of handle
  void SetCenterHandle(double *xyz);                       ///< set position of handle
  void SetCenterHandle(double x, double y, double z);      ///< set position of handle


  //----------------------------------------------------------------------------
  // Methods for positioning the widget plane
  //----------------------------------------------------------------------------
  void SetOrigin(double x, double y, double z);  ///< Set/Get the origin of the plane.
  void SetOrigin(double x[3]);                   ///< Set/Get the origin of the plane.
  double* GetOrigin();                           ///< Set/Get the origin of the plane.
  void GetOrigin(double xyz[3]);                 ///< Set/Get the origin of the plane.

  void SetPoint1(double x, double y, double z);  ///< Set/Get the position of the point defining the first axis of the plane.
  void SetPoint1(double x[3]);                   ///< Set/Get the position of the point defining the first axis of the plane.
  double* GetPoint1();                           ///< Set/Get the position of the point defining the first axis of the plane.
  void GetPoint1(double xyz[3]);                 ///< Set/Get the position of the point defining the first axis of the plane.

  void SetPoint2(double x, double y, double z);  ///< Set/Get the position of the point defining the second axis of the plane.
  void SetPoint2(double x[3]);                   ///< Set/Get the position of the point defining the second axis of the plane.
  double* GetPoint2();                           ///< Set/Get the position of the point defining the second axis of the plane.
  void GetPoint2(double xyz[3]);                 ///< Set/Get the position of the point defining the second axis of the plane.

  void SetCenter(double x, double y, double z);  ///< Set/Get the center of the plane.
  void SetCenter(double x[3]);                   ///< Set/Get the center of the plane.
  double* GetCenter();                           ///< Set/Get the center of the plane.
  void GetCenter(double xyz[3]);                 ///< Set/Get the center of the plane.

  void SetNormal(double x, double y, double z);  ///< Set/Get the normal to the plane.
  void SetNormal(double x[3]);                   ///< Set/Get the normal to the plane.
  double* GetNormal();                           ///< Set/Get the normal to the plane.
  void GetNormal(double xyz[3]);                 ///< Set/Get the normal to the plane.

  /*
  void SetMotionVector(float x, float y, float z);  ///< Set/Get the motion vector
  void SetMotionVector(float m[3]);                 ///< Set/Get the motion vector
  float* GetMotionVector();                         ///< Set/Get the motion vector
  void GetMotionVector(float xyz[3]);               ///< Set/Get the motion vector
*/


  //----------------------------------------------------------------------------
  // Methods for setting the representation of the widget plane
  //----------------------------------------------------------------------------

  /// Control how the plane appears when GetPolyData() is invoked. \n
  /// If the mode is "outline", then just the outline of the plane \n
  /// is shown. If the mode is "wireframe" then the plane is drawn \n
  /// with the outline plus the interior mesh (corresponding to the \n
  /// resolution specified). If the mode is "surface" then the plane \n
  /// is drawn as a surface.

  //vtkSetClampMacro(Representation,int,VTK_PLANE_OFF,VTK_PLANE_SURFACE);
  virtual void SetRepresentation(int representation); 
  virtual int GetRepresentationMinValue () {return VTK_PLANE_OFF;}
  virtual int GetRepresentationMaxValue () {return VTK_PLANE_SURFACE;}
  virtual int GetRepresentation () {return m_Representation;}
  //vtkGetMacro(Representation,int)
  void SetRepresentationToOff() {this->SetRepresentation(VTK_PLANE_OFF);}
  void SetRepresentationToWireframe() {this->SetRepresentation(VTK_PLANE_WIREFRAME);}
  void SetRepresentationToSurface() {this->SetRepresentation(VTK_PLANE_SURFACE);}



  /// Grab the polydata (including points) that defines the plane.  The \n
  /// polydata consists of (res+1)*(res+1) points, and res*res quadrilateral \n
  /// polygons, where res is the resolution of the plane. These point values \n
  /// are guaranteed to be up-to-date when either the InteractionEvent or \n
  /// EndInteraction events are invoked. The user provides the vtkPolyData and \n
  /// the points and polyplane are added to it.
  void GetPolyData(vtkPolyData *pd);

  /// Get the planes describing the implicit function defined by the plane \n
  /// widget. The user must provide the instance of the class vtkPlane. Note \n
  /// that vtkPlane is a subclass of vtkImplicitFunction, meaning that it can \n
  /// be used by a variety of filters to perform clipping, cutting, and \n
  /// selection of data.
  void GetPlane(vtkPlane *plane);

  /// Satisfies superclass API.  This returns a pointer to the underlying \n
  /// PolyData.  Make changes to this before calling the initial PlaceWidget() \n
  /// to have the initial placement follow suit.  Or, make changes after the \n
  /// widget has been initialised and call UpdatePlacement() to realise.
  vtkPolyDataAlgorithm* GetPolyDataSource() {return m_PlaneSource ;}

  /// Satisfies superclass API.  This will change the state of the widget to \n
  /// match changes that have been made to the underlying PolyDataSource
  void UpdatePlacement(void);


  /// Create and set default properties. \n
  /// NB This allocates vtk objects. \n
  /// You must call DeleteDefaultProperties() when finished.
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



  //----------------------------------------------------------------------------
  // Member variables
  //----------------------------------------------------------------------------
  albaOpMML3ModelView *m_Model; ///< model

  albaOpMML3ParameterView *m_PH; ///< horizontal place center
  albaOpMML3ParameterView *m_PV; ///< vertical place center
  albaOpMML3ParameterView *m_RA; ///< rotation
  albaOpMML3ParameterView *m_TH; ///< horizontal translation
  albaOpMML3ParameterView *m_TV; ///< vertical translation
  albaOpMML3ParameterView *m_SN; ///< north scaling
  albaOpMML3ParameterView *m_SS; ///< south scaling
  albaOpMML3ParameterView *m_SE; ///< east scaling
  albaOpMML3ParameterView *m_SW; ///< west scaling

  float m_HandleRadius;

  BOOL m_RotatingMode;
  BOOL m_ScalingMode;
  BOOL m_TranslationMode;
  BOOL m_PlaceMode;
  float m_Bounds[6];

  int m_Operation;
  int m_State;

  int m_OperationID;

  // motion vector
  //float m_Motion[3];

  // controlling ivars
  int m_NormalToXAxis;
  int m_NormalToYAxis;
  int m_NormalToZAxis;
  int m_Representation;

  // the widget plane
  vtkPlaneSource    *m_PlaneSource;
  vtkActor          *m_PlaneActor;
  vtkPolyDataMapper *m_PlaneMapper;

  // scaling handles
  vtkActor          **m_HandleActor;
  vtkPolyDataMapper **m_HandleMapper;
  vtkSphereSource   **m_HandleGeometry;

  // rotational handle
  vtkActor          *m_RotationalHandleActor;
  vtkPolyDataMapper *m_RotationalHandleMapper;
  vtkSphereSource   *m_RotationalHandleGeometry;

  // center handle (does not seem to be used)
  vtkActor          *m_CenterHandleActor;
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

   
  // Properties used to control the appearance of selected objects and
  // the manipulator in general.
  vtkProperty *m_HandleProperty;
  vtkProperty *m_SelectedHandleProperty;
  vtkProperty *m_PlaneProperty;
  vtkProperty *m_SelectedPlaneProperty;

private:
  albaOpMML3ContourWidget(const albaOpMML3ContourWidget&);  //Not implemented
  void operator=(const albaOpMML3ContourWidget&);  //Not implemented

};

#endif
