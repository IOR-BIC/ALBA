/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpMML3.h,v $
Language:  C++
Date:      $Date: 2009-06-11 17:20:08 $
Version:   $Revision: 1.1.2.3 $
Authors:   Mel Krokos
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



//----------------------------------------------------------------------------
// TODO 11.6.09
// Restore commented-out sections.
// Is it ok to have only 3 slices ?
// Range covered by slices only goes between landmarks - might not reach ends of bone.
// Too many options in input dialog - need to explain, simplify and remove unwanted items.
//   What is XY scale ?
//   What is Slice distance ?
// Contour widget not rendered on first slice until you change to another slice and back.
//----------------------------------------------------------------------------


//------------------------------------------------------------------------------
/// medOpMML3. \n
/// Muscle Modelling Lab. \n
/// This operation allows the user to deform a polydata model of a muscle or bone
/// to fit a volume image of the feature. \n
/// The inputs are a volume (the patient data) and a muscle surface (the model).
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


protected:
  void CreateInputsDlg();         ///< Dialog to select the polydata surface
  void CreateRegistrationDlg();   ///< Dialog to perform the model fitting
  void DeleteInputsDlg() ;        ///< Delete inputs dialog
  void DeleteRegistrationDlg() ;  ///< Delete registration dialog

  /// Get vtk data for volume and surface with pose matrices \n
  /// Must run inputs dialog first 
  void GetVtkInputs() ;

  bool SetUpContourWidget();  ///< set up the contour widget
  bool SetUpModelView();  ///< set up the main model view
  bool SetUpParameterViews();  ///< set up the parameter views (ie the xy plots)

  /// Create new render window for parameter view, including
  /// visual pipe for labelling.
  mafRWI* CreateParameterViewmafRWI(vtkTextSource *ts, wxString lab, float r, float g, float b);

  /// Get absolute position of landmark. \n
  /// Required because landmark->GetPoint() only returns pos relative to parent.
  void GetAbsPosOfLandmark(mafVMELandmark *landmark, double point[3]) ;

  /// Create default set of landmarks
  void CreateFakeLandmarks() ;

  /// Set inputs to model view, including landmarks
  bool SetUpModelViewInputs();


  void ApplyRegistrationOps();        ///< Apply the deformations to the model
  void ApplyInverseRegistrationOps(); ///< Apply inverse deformations (not implemented)

  void Update();

  void ResetOperation();

  /// Set up a specific set of landmarks (not currently used)
  void SetUpLandmarks1(wxString AtlasSectionVMEName, wxString PatientSectionVMEName);


  //----------------------------------------------------------------------------
  // Event handlers
  //----------------------------------------------------------------------------
  void OnEvent(mafEventBase *e); ///< Event handler
  void OnSOperationButton();
  void OnROperationButton();
  void OnTOperationButton();
  void OnPOperationButton();
  void OnRegistrationCANCEL();
  void OnRegistrationOK();  ///< Registration ok - create output VME

  void OnSlider();
  void OnLut();
  void OnMuscleSelection();
  void OnResetView();
  void OnUndo();
  void OnOperation();
  void OnContourLineAxesVisibility();
  void OnContourVisibility();
  void OnLandmark1AtlasPatientSelection();
  void OnLandmark2AtlasPatientSelection();
  void OnLandmark3AtlasPatientSelection();
  void OnLandmark4AtlasPatientSelection();



  //----------------------------------------------------------------------------
  // Member variables
  //----------------------------------------------------------------------------

  // dialogs
  mafGUIDialog	*m_ChooseDlg;		///< inputs dialog
  mafGUIDialog	*m_OpDlg;			  ///< operation dialog

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


  // flags etc
  int m_CurrentSlice;	///< id of current slice 
  int m_State;			///< current operation (used to validate a radio button, now defunct)
  int m_ShowAxes;		///< show contour axes flag
  int m_ContourVisibility ; ///< show contour
  int m_MuscleType;	///< muscle type (axis is single line or piecewise 2 lines)
  int m_RegistrationStatus;	///< flag indicating if registration has taken place
  int m_3DFlag;	///< flag indicating if model view is 2D or 3D

  // parameters of scans (slices)
  //double m_ScansDistance;
  int m_ScansNumber ; ///< number of scans
  int m_ScansGrain;
  double m_ScansSize[2]; ///< size of scans (does this class need its own copy ?)
  double m_ScansResolution[2]; ///< res of scans
  double m_RegistrationXYScalingFactor ; ///< factor used in model
  double m_ScansDistance ;  ///< spacing of scans (Nigel 9.6.09)

  vtkTextSource *m_textSource[9] ;  ///< text sources used in CreateParameterViewmafRWI()



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



  //----------------------------------------------------------------------------
  // Widgets
  //----------------------------------------------------------------------------

  mafGUILutSlider *m_Lut;
  wxRadioBox *m_Radio;

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

  wxTextCtrl *m_ScansNumberTxt;
  wxTextCtrl *m_RegistrationXYSxalingFactorTxt;

};
#endif














