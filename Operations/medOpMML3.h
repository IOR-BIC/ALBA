/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpMML3.h,v $
Language:  C++
Date:      $Date: 2009-09-18 08:10:33 $
Version:   $Revision: 1.1.2.8 $
Authors:   Mel Krokos, Nigel McFarlane
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medOpMML3_H__
#define __medOpMML3_H__

#include "mafDefines.h"

#include "vtkPolyData.h"
#include "vtkDataSet.h"
#include "vtkTextSource.h"

#include "mafEvent.h"
#include "mafGUI.h"
#include "mafGUIDialog.h"
#include "mafGUILutSlider.h"
#include "mafGUIButton.h"
#include "mafVMEVolumeGray.h"
#include "mafVMESurface.h"
#include "mafVMELandmark.h"
#include "mafTagItem.h"
#include "mafRWI.h"

#include "medOpMML3ModelView.h"
#include "medOpMML3ContourWidget.h"
#include "medOpMML3ParameterView.h"
#include "medOpMML3NonUniformSlicePipe.h"




//------------------------------------------------------------------------------
// TODO 16.9.09
// 
// Widget
// Unify variables m_Operation, m_ScalingMode etc.
// Problems with text display.
// Problems caused by pre-setting the endpoints of the splines.
// Got problems where contour does not respond to T and S, although text is changing.
//
// Preview
// Some components are missing, and widget handles are still visible.
//
// Final registration
// Small problem with ops changing results of previous ops
//
// Dialog
// Add progress bar to show pre-processing of slices.
// Slice processing is rather slow.  One second per slice at 128 resolution.
// Crashes with only 12 slices at 256 resolution.  Out of memory ?
//
// Bent axis
// Restore piecewise axis option
// Test visual pipes with bent axis.
//
// Remove medOpMatrixVectorMath from aneuFuse and lhp.
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
/// medOpMML3. \n
/// Muscle Modelling Lab. \n
/// Manual registration operation (register from template). \n
/// This operation registers a polydata model of a muscle or bone onto a volume image of the feature. \n
/// The inputs are a volume (the patient data) and a muscle surface (the model). \n
/// Global registration is performed by matching a set of 3 or 4 landmarks on patient and model. \n
/// The user can then manually fit the shape of the model to the patient data. \n
/// Optional inputs are landmarks used for registration and defining the slicing axis. \n
/// Two optional axis landmarks can be used to set the slicing direction.
/// The input landmarks must be in VME folders labelled "Atlas" and "Patient", \n
/// with the axis landmarks being part of the Atlas section.
//
// Coordinate systems.
// Atlas space - the coord system of the model before global registration.
// Patient space - the coord system of the patient data.  The 3d view is in patient space.
// Current slice space - the coord system of the current slice.  The 2d view is in current slice space.
// Alpha and zeta - alpha and zeta are the distance (fractional and actual) of the slice along the axis.  
// The splines are functions of zeta, but displayed as a function of slice id.
//------------------------------------------------------------------------------
class medOpMML3: public mafOp
{
public:
  medOpMML3(const wxString &label); ///< constructor
  ~medOpMML3(); ///< destructor

  mafOp* Copy();
  bool Accept(mafNode* vme);
  void OpRun();
  void OpStop(int result);
  void OpDo();
  void OpUndo();


  /// static callback which allows vme selector to select only this type
  static bool AcceptVMELandmark(mafNode *node)
  {
    mafVME* vme = mafVME::SafeDownCast(node);
    return ((vme != NULL) && vme->IsA("mafVMELandmark")) ;
  }

  /// static callback which allows vme selector to select only this type
  static bool AcceptVMESurface(mafNode *node)
  {
    mafVME* vme = mafVME::SafeDownCast(node);
    return ((vme != NULL) && vme->IsA("mafVMESurface")) ;
  }

  /// static constant: number of non-uniform sections
  static const int NumberOfNonUniformSections = 10 ; 


protected:
  //----------------------------------------------------------------------------
  // Dialog methods
  //----------------------------------------------------------------------------

  void CreateInputsDlg();         ///< Dialog to select the polydata surface
  void CreateRegistrationDlg();   ///< Dialog to perform the model fitting
  void CreateNonUniformSlicesDlg();   ///< Dialog to input non-uniform slice spacing
  void DeleteInputsDlg() ;        ///< Delete inputs dialog
  void DeleteRegistrationDlg() ;  ///< Delete registration dialog
  void DeleteNonUniformSlicesDlg();   ///< Delete slice spacing dialog

  void medOpMML3::Set2DButtonsEnable(bool enable) ; ///< Enable/Disable buttons which only work in 2d display mode


  //----------------------------------------------------------------------------
  // Setting up classes
  //----------------------------------------------------------------------------

  /// Get vtk data for volume and surface with pose matrices \n
  /// Must run inputs dialog first 
  void GetVtkInputs() ;

  /// Set up the main model view
  bool SetUpModelView();       

  /// Set up the parameter views (the xy plots) and initialize spline functions. \n
  /// The ModelView must be initialized first. \n
  /// However, this changes the spline values, so you must update the model view afterwards.
  bool SetUpParameterViews();

  /// Set up the contour widget
  bool SetUpContourWidget();   

  /// Create new render window for parameter view, including
  /// visual pipe for labelling.
  mafRWI* CreateParameterViewmafRWI(vtkTextSource *ts, wxString lab, float r, float g, float b);

  /// Calculate fractional slice positions, alpha[0..n-1] along axis, \n
  /// where the values 0.0 and 1.0 correspond to the min and max of the slicing range. \n
  /// NB The first and last slices might not be exactly at 0.0 and 1.0. \n
  /// NB Run ApplyAxisRangeFactor() before this - not after !
  void CalculateSlicePositionsAlongAxis(double *alpha) ;


  //----------------------------------------------------------------------------
  // Processing
  //----------------------------------------------------------------------------

  /// Apply global registration from landmarks. \n
  /// Creates patient space versions of muscle polydata and landmarks
  void ApplyGlobalRegistration() ;

  void Update();                      ///< Update model view and widget
  void ResetOperation();



  //----------------------------------------------------------------------------
  // Landmark methods
  //----------------------------------------------------------------------------

  /// Get absolute position of landmark. \n
  /// Required because landmark->GetPoint() only returns pos relative to parent.
  void GetAbsPosOfLandmark(mafVMELandmark *landmark, double point[3]) ;

  /// Create default set of landmarks. \n
  /// Landmarks are based on the bounds of the input volume.
  void CreateDefaultRegistrationLandmarks() ;

  /// Create default set of axis landmarks. \n
  /// The axis is x, y or z in patient coords, depending on selection made in inputs dialog.
  void CreateDefaultAxisLandmarks() ;

  /// Apply scaling factor to axis landmarks. \n
  /// Do this before uploading the landmarks to the model view.
  void ApplyAxisRangeFactor() ;

  /// Set up a specific set of landmarks (not currently used)
  void SetUpLandmarks1(wxString AtlasSectionVMEName, wxString PatientSectionVMEName);

  /// Set up a specific set of landmarks
  void SetUpLandmarks2(wxString AtlasSectionVMEName, wxString PatientSectionVMEName);



  //----------------------------------------------------------------------------
  // Event handlers
  //----------------------------------------------------------------------------
  void OnEvent(mafEventBase *e); ///< Event handler

  void OnTextNumberChange(mafID id) ;  ///< non-uniform slice entry changed

  void OnInputsOk() ; ///< Inputs dialog ok - tidy up
  void OnSliceXYZ() ; ///< Action when use reg check box changes

  void OnSOperationButton();
  void OnROperationButton();
  void OnTOperationButton();
  void OnPOperationButton();
  void OnRegistrationCANCEL();
  void OnRegistrationOK();  ///< Registration ok - create output VME

  void OnSlider();
  void OnMinus10() ;
  void OnMinus1() ;
  void OnPlus1() ;
  void OnPlus10() ;
  void OnLut();
  void OnMuscleSelection();
  void OnResetView();
  void OnUndo();
  void OnDisplayMode();
  void OnPreview();
  void OnLandmark1AtlasPatientSelection();
  void OnLandmark2AtlasPatientSelection();
  void OnLandmark3AtlasPatientSelection();
  void OnLandmark4AtlasPatientSelection();
  void OnLandmarkAxis1AtlasSelection();
  void OnLandmarkAxis2AtlasSelection();
  void OnLandmarkAxis3AtlasSelection();



  //----------------------------------------------------------------------------
  // Member variables
  //----------------------------------------------------------------------------

  // dialogs
  mafGUIDialog *m_InputsDlg;		         ///< inputs dialog
  mafGUIDialog *m_NonUniformSlicesDlg ;  ///< non-uniform slices dialog
  mafGUIDialog *m_OpDlg;			           ///< operation dialog

  // input vme and vtk data for volume and selected muscle polydata
  mafVMEVolumeGray  *m_VolumeVME;         ///< input volume vme  
  mafVMESurface     *m_SurfaceVME;        ///< input surface vme
  vtkDataSet        *m_Volume;            ///< input volume vtk
  vtkPolyData       *m_Muscle;            ///< input surface vtk * pose matrix
  vtkPolyData       *m_MuscleGlobalReg ;  ///< input surface vtk, after global registration from landmarks
  vtkPolyData       *m_MuscleOutput ;     ///< final output surface after deformations applied

  // parameter views
  medOpMML3ParameterView *m_PH; ///< PH parameter view
  medOpMML3ParameterView *m_PV; ///< PV parameter view
  medOpMML3ParameterView *m_TH; ///< TH parameter view
  medOpMML3ParameterView *m_TV; ///< TV parameter view
  medOpMML3ParameterView *m_RA; ///< RA parameter view
  medOpMML3ParameterView *m_SN; ///< SN parameter view
  medOpMML3ParameterView *m_SS; ///< SS parameter view
  medOpMML3ParameterView *m_SE; ///< SE parameter view
  medOpMML3ParameterView *m_SW; ///< SW parameter view

  // parameter views maf RWIs
  mafRWI      *m_PHmafRWI;		///< PH parameter view maf RWI
  mafRWI      *m_PVmafRWI;		///< PV parameter view maf RWI
  mafRWI      *m_THmafRWI;		///< TH parameter view maf RWI
  mafRWI      *m_TVmafRWI;		///< TV parameter view maf RWI
  mafRWI      *m_RAmafRWI;		///< RA parameter view maf RWI
  mafRWI      *m_SNmafRWI;		///< SN parameter view maf RWI
  mafRWI      *m_SSmafRWI;		///< SS parameter view maf RWI
  mafRWI      *m_SEmafRWI;		///< SE parameter view maf RWI
  mafRWI      *m_SWmafRWI;		///< SW parameter view maf RWI

  medOpMML3ModelView *m_Model;	///< model view
  mafRWI *m_ModelmafRWI;		///< model view maf RWI
  medOpMML3ContourWidget *m_Widget;  ///< contour widget which controls contours in model view


  // parameters and flags
  int m_CurrentSlice;	///< id of current slice 
  int m_ShowAxes;		///< show contour axes flag
  int m_ContourVisibility ; ///< show contour
  int m_MuscleType;	///< muscle type (axis is single line or piecewise 2 lines)
  int m_RegistrationStatus;	///< flag indicating if registration has taken place
  int m_3DFlag;	///< display mode for model view: 0 = 2D or 1 = 3D
  int m_PreviewFlag;	///< display mode for preview view: 0 = off, 1 = on
  int m_Slicexyz ;  ///< validator for default slice direction
  int m_ScansGrain; ///< set by user, used to calculate size and resolution of scans
  double m_AxisRangeFactor ; ///< factor which extends range of axis beyond landmarks

  /// flag to remember how axis landmarks were created. \n
  /// 1 for default (patient space) \n
  /// 2 for selected (from atlas)
  int m_AxisLandmarksFlag ; 

  vtkTextSource *m_TextSource[9] ;  ///< text sources used in CreateParameterViewmafRWI()


  //----------------------------------------------------------------------------
  // number of slices
  //----------------------------------------------------------------------------
  int m_NumberOfScans ;                              ///< total number of slices
  int m_NonUniformSliceSpacing ;                     ///< is slice spacing non-uniform
  int m_SlicesInSection[NumberOfNonUniformSections] ; ///< no. of slices per section, if non-uniform

  wxBoxSizer **m_NumSlicesRowBoxSizer ;
  wxStaticText **m_NumSlicesLabel ;
  wxTextCtrl **m_NumSlicesEntry ;



  //----------------------------------------------------------------------------
  // Names and tags
  //----------------------------------------------------------------------------

  // msf file sections
  wxString m_AtlasMSFSectionName;
  wxString m_PatientMSFSectionName;

  // volume and surface names
  wxString m_VolName;
  wxString m_SurfaceName;

  // operation tags
  mafTagItem *m_CurrentSliceIdStackTag;
  mafTagItem *m_ZValueStackTag;
  mafTagItem *m_OperationTypeStackTag;
  mafTagItem *m_Parameter1StackTag;
  mafTagItem *m_Parameter2StackTag;

  // coords tags
  mafTagItem *m_CoordsXTag;
  mafTagItem *m_CoordsYTag;
  mafTagItem *m_CoordsZTag;



  //----------------------------------------------------------------------------
  // Landmarks
  // Registration landmarks: 4 from atlas, 4 from patient
  // Axis landmarks: 2 from the patient
  //----------------------------------------------------------------------------

  // atlas landmarks defined
  bool m_L1Defined ;
  bool m_L2Defined ;
  bool m_L3Defined ;
  bool m_L4Defined ;

  // atlas landmark names
  wxString m_L1Name;
  wxString m_L2Name;
  wxString m_L3Name;
  wxString m_L4Name;

  // atlas landmark points
  double m_L1Point[3];
  double m_L2Point[3];
  double m_L3Point[3];
  double m_L4Point[3];

  // patient landmark names
  wxString m_P1Name;
  wxString m_P2Name;
  wxString m_P3Name;
  wxString m_P4Name;

  // patient landmark points
  double m_P1Point[3];
  double m_P2Point[3];
  double m_P3Point[3];
  double m_P4Point[3];

  // axis landmarks defined
  bool m_Axis1Defined ;
  bool m_Axis2Defined ;
  bool m_Axis3Defined ;

  // axis landmark names
  wxString m_Axis1Name;
  wxString m_Axis2Name;
  wxString m_Axis3Name;

  // axis landmark points
  double m_Axis1Point[3];
  double m_Axis2Point[3];
  double m_Axis3Point[3];

  // axis landmark points transformed to patient coords
  double m_Axis1Point_PatientCoords[3];
  double m_Axis2Point_PatientCoords[3];
  double m_Axis3Point_PatientCoords[3];



  //----------------------------------------------------------------------------
  // Widgets
  //----------------------------------------------------------------------------

  mafGUILutSlider *m_Lut;

  mafGUIButton *m_InputsOk;
  mafGUIButton *m_DisplayModeButton;
  mafGUIButton *m_PreviewButton;

  mafGUIButton *m_PlaceOpButton;
  mafGUIButton *m_TranslateOpButton;
  mafGUIButton *m_RotateOpButton;
  mafGUIButton *m_ScaleOpButton;
  mafGUIButton *m_ResetViewButton;
  mafGUIButton *m_UndoButton;
  mafGUIButton *m_OkButton;
  mafGUIButton *m_CancelButton;

  wxColour m_ButtonBackgroundColour;

  wxTextCtrl *m_NumberOfScansTxt;

  wxRadioBox *m_Radio_slicexyz ;



  mafRWI *m_SectionsViewRWI ;
  medOpMML3NonUniformSlicePipe *m_NonUniformSlicePipe ;


};
#endif














