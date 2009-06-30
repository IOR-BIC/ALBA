/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpMML3.h,v $
Language:  C++
Date:      $Date: 2009-06-30 15:35:59 $
Version:   $Revision: 1.1.2.6 $
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



//------------------------------------------------------------------------------
// TODO 18.6.09
// Is it ok to have only 3 slices ?
// Contour widget not rendered on first slice until you change to another slice and back.
// Does MML still work if the muscle model is scaled ?
// Windowing range might not be right.
// Cross hairs of contour widget not always centred - prone to noise
// Two-part axis: slice is not positioned properly in 2D view, contour not in slice plane in 3D view.
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// MODELVIEW ACTORS (2D + 3D)
// m_SyntheticScansActor[numScans]

// MODELVIEW ACTORS (2D only)
// m_GlobalPosXAxisActor (PosX, NegX, PosY, NegY) (global axes)
// m_ContourPosXAxisActor (PosX, NegX, PosY, NegY) (contour axes)
// m_ContourGlyphActor (not used)
// m_NEContourActor (NE, SE, NW, SW) (contour in 4 quarters)
// m_ScaledTextActorX (X, Y)

// MODELVIEW ACTORS (3D only)
// m_MuscleLODActor (muscle surface)
// m_ContourActor (whole contour for 3d display)
// m_GlobalPosZAxisActor (PosZ, NegZ) (global z axis)
// m_L1L2Actor, m_L2L3Actor (action lines)
// m_Landmark1Actor (1, 2, 3, 4) (landmarks)

// CONTOURWIDGET ACTORS
// m_PlaneActor
// m_Handle[4]
// m_RotationalHandle
// m_CenterHandle
// m_LineActor
// m_ConeActor
// m_LineActor2
// m_ConeActor2
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
/// medOpMML3. \n
/// Muscle Modelling Lab. \n
/// Manual registration operation.
/// This operation registers a polydata model of a muscle or bone onto a volume image of the feature. \n
/// The inputs are a volume (the patient data) and a muscle surface (the model).
/// Optional inputs 
/// Global registration is performed by matching a set of 3 or 4 landmarks on patient and model.
/// The line between landmarks 1 and 2 defines the slicing axis.
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


protected:
  void CreateInputsDlg();         ///< Dialog to select the polydata surface
  void CreateRegistrationDlg();   ///< Dialog to perform the model fitting
  void CreateNonUniformSlicesDlg();   ///< Dialog to input non-uniform slice spacing
  void DeleteInputsDlg() ;        ///< Delete inputs dialog
  void DeleteRegistrationDlg() ;  ///< Delete registration dialog
  void DeleteNonUniformSlicesDlg();   ///< Delete slice spacing dialog

  /// Get vtk data for volume and surface with pose matrices \n
  /// Must run inputs dialog first 
  void GetVtkInputs() ;

  bool SetUpContourWidget();  ///< set up the contour widget
  bool SetUpModelView();  ///< set up the main model view
  bool SetUpParameterViews();  ///< set up the parameter views (ie the xy plots)

  /// Create new render window for parameter view, including
  /// visual pipe for labelling.
  mafRWI* CreateParameterViewmafRWI(vtkTextSource *ts, wxString lab, float r, float g, float b);

  /// Transfer setup parameters from input dialog and landmarks to model view
  void SetUpModelViewInputs();

  void ApplyRegistrationOps();        ///< Apply the deformations to the model
  void ApplyInverseRegistrationOps(); ///< Apply inverse deformations (not implemented)

  void Update();

  void ResetOperation();



  //----------------------------------------------------------------------------
  // Landmark methods
  //----------------------------------------------------------------------------

  /// Get absolute position of landmark. \n
  /// Required because landmark->GetPoint() only returns pos relative to parent.
  void GetAbsPosOfLandmark(mafVMELandmark *landmark, double point[3]) ;

  /// Create default set of landmarks
  void CreateDefaultRegistrationLandmarks() ;

  /// Create default set of landmarks
  void CreateDefaultAxisLandmarks() ;

  /// Apply scaling factor to axis landmarks
  /// Do this before uploading the landmarks to the model view.
  void ApplyAxisRangeFactor() ;

  /// Set up a specific set of landmarks (not currently used)
  void SetUpLandmarks1(wxString AtlasSectionVMEName, wxString PatientSectionVMEName);

  /// Set up a specific set of landmarks (not currently used)
  void SetUpLandmarks2(wxString AtlasSectionVMEName, wxString PatientSectionVMEName);


  //----------------------------------------------------------------------------
  // Event handlers
  //----------------------------------------------------------------------------
  void OnEvent(mafEventBase *e); ///< Event handler

  void OnChooseOk() ; ///< Inputs dialog ok - tidy up
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
  void OnContourLineAxesVisibility();
  void OnContourVisibility();
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
  mafGUIDialog *m_ChooseDlg;		         ///< inputs dialog
  mafGUIDialog *m_NonUniformSlicesDlg ;  ///< non-uniform slices dialog
  mafGUIDialog *m_OpDlg;			           ///< operation dialog

  // input vme and vtk data for volume and selected muscle polydata
  mafVMEVolumeGray    *m_VolumeVME;  ///< input volume vme  
  mafVMESurface       *m_SurfaceVME; ///< input surface vme
  vtkDataSet          *m_Volume;     ///< input volume vtk
  vtkPolyData         *m_Muscle;     ///< input surface vtk * pose matrix

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
  int m_3DFlag;	///< flag indicating if model view is 2D or 3D
  int m_slicexyz ;  ///< validator for default slice direction
  int m_ScansGrain; ///< set by user, used to calculate size and resolution of scans
  double m_AxisRangeFactor ; ///< factor which extends range of axis beyond landmarks

  /// flag to remember how axis landmarks were created. \n
  /// 1 for default (patient space) \n
  /// 2 for selected (from atlas)
  int m_AxisLandmarksFlag ; 

  vtkTextSource *m_textSource[9] ;  ///< text sources used in CreateParameterViewmafRWI()


  //----------------------------------------------------------------------------
  // number of slices
  //----------------------------------------------------------------------------
  int m_NumberOfSlices ;                              ///< total number of slices
  static const int NumberOfNonUniformSections = 10 ;  ///< number of non-uniform sections
  int m_NonUniformSliceSpacing ;                     ///< is slice spacing non-uniform
  int m_SlicesInSection[NumberOfNonUniformSections] ; ///< no. of slices per section, if non-uniform



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



  //----------------------------------------------------------------------------
  // Widgets
  //----------------------------------------------------------------------------

  mafGUILutSlider *m_Lut;

  mafGUIButton *m_ChooseOk;
  mafGUIButton *m_AxesOnOffButton;
  mafGUIButton *m_ContourOnOffButton;

  mafGUIButton *m_PlaceOpButton;
  mafGUIButton *m_TranslateOpButton;
  mafGUIButton *m_RotateOpButton;
  mafGUIButton *m_ScaleOpButton;
  mafGUIButton *m_ResetViewButton;
  mafGUIButton *m_UndoButton;
  mafGUIButton *m_OkButton;
  mafGUIButton *m_CancelButton;

  wxColour m_ButtonBackgroundColour;

  wxTextCtrl *m_NumberOfSlicesTxt;

  wxRadioBox *m_radio_slicexyz ;
};
#endif














