/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpMML3.h,v $
Language:  C++
Date:      $Date: 2009-05-29 11:05:29 $
Version:   $Revision: 1.1.2.1 $
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
#include "mafTagItem.h"
#include "mafRWI.h"

#include "medOpMML3ModelView.h"
#include "medOpMML3ContourWidget.h"
#include "medOpMML3ParameterView.h"



//----------------------------------------------------------------------------
// TODO 28.5.09
// Replace "grain" in dialog with actual resolution
// Calculation of slice size and position is very poor - sort it out
// Possible mistakes in setting translation matrices.
// Why is the output only transformed by inv(finalm), which doesn't include "transform 1" ?
// Contour widget can be too big, so you lose the scaling handles.
// Restore commented-out sections.
// Is it ok to have only 3 slices ?
//----------------------------------------------------------------------------


//------------------------------------------------------------------------------
/// medOpMML3. \n
/// Muscle Modelling Lab. \n
/// This operation allows the user to deform a polydata model of a muscle or bone
/// to fit a volume image of the feature. \n
/// The inputs are a volume (the patient data) and a surface (the model).
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


  void OnEvent(mafEventBase *e); ///< Event handler
  void OnSOperationButton();
  void OnROperationButton();
  void OnTOperationButton();
  void OnPOperationButton();
  void OnRegistrationCANCEL();
  void OnRegistrationOK();  ///< Registration ok - create ouput VME

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

  void ApplyInverseRegistrationOps();
  void ApplyRegistrationOps();        ///< Apply the deformations to the model

  void Update();

  void    CreateFakeLandmarks();
  bool    SetUpInputs();
  void	  SetUpLandmarks(wxString AtlasSectionVMEName, wxString PatientSectionVMEName);

  void	ResetOperation();



  // dialogs
  mafGUIDialog	*m_ChooseDlg;		///< inputs dialog
  mafGUIDialog	*m_OpDlg;			  ///< operation dialog

  // input vme and vtk data for volume and selected muscle polydata
  mafVMEVolumeGray    *m_VolumeVME;  ///< input volume vme  
  mafVMESurface       *m_SurfaceVME; ///< input surface vme
  vtkDataSet          *m_Volume;     ///< input volume vtk
  vtkPolyData         *m_Muscle;     ///< input surface vtk * pose matrix

  // parameter views
  medOpMML3ParameterView *m_PH; // PH parameter view
  medOpMML3ParameterView *m_PV; // PV parameter view
  medOpMML3ParameterView *m_TH; // TH parameter view
  medOpMML3ParameterView *m_TV; // TV parameter view
  medOpMML3ParameterView *m_RA; // RA parameter view
  medOpMML3ParameterView *m_SN; // SN parameter view
  medOpMML3ParameterView *m_SS; // SS parameter view
  medOpMML3ParameterView *m_SE; // SE parameter view
  medOpMML3ParameterView *m_SW; // SW parameter view

  // parameter views maf RWIs
  mafRWI      *m_PHmafRWI;		// PH parameter view maf RWI
  mafRWI      *m_PVmafRWI;		// PV parameter view maf RWI
  mafRWI      *m_THmafRWI;		// TH parameter view maf RWI
  mafRWI      *m_TVmafRWI;		// TV parameter view maf RWI
  mafRWI      *m_RAmafRWI;		// RA parameter view maf RWI
  mafRWI      *m_SNmafRWI;		// SN parameter view maf RWI
  mafRWI      *m_SSmafRWI;		// SS parameter view maf RWI
  mafRWI      *m_SEmafRWI;		// SE parameter view maf RWI
  mafRWI      *m_SWmafRWI;		// SW parameter view maf RWI

  // model view
  medOpMML3ModelView     *m_Model;

  // model view maf RWI
  mafRWI		*m_ModelmafRWI;

  //
  medOpMML3ContourWidget *m_Widget;  // interactive contour widget


  int             m_Slice;
  mafGUILutSlider   *m_Lut;
  wxRadioBox     *m_Radio;

  int             m_State;
  int             m_ShowAxes;
  int             m_ContourVisibility ;

  wxString             m_VolName;
  wxString             m_SurfaceName;

  // msf file sections
  wxString			   m_AtlasMSFSectionName;
  wxString			   m_PatientMSFSectionName;

  //atlas landmarks
  //names
  wxString             m_L1Name;
  wxString             m_L2Name;
  wxString             m_L3Name;
  wxString             m_L4Name;
  // points
  double			   m_L1Point[3];
  double			   m_L2Point[3];
  double			   m_L3Point[3];
  double			   m_L4Point[3];

  //patient landmarks
  // names
  wxString             m_P1Name;
  wxString             m_P2Name;
  wxString             m_P3Name;
  wxString             m_P4Name;
  // points
  double			   m_P1[3];
  double			   m_P2[3];
  double			   m_P3[3];
  double			   m_P4[3];

  // muscle type
  int m_MuscleType;

  // registration status
  int m_RegistrationStatus;

  // 3d flag
  int m_3DFlag;

  // 4 landmarks flag
  int m_LandmarksFlag;

  // other parameters
  //double m_ScansDistance;
  int	 m_ScansNumber, m_ScansNumber2;
  double m_RegistrationXYScalingFactor, m_RegistrationXYScalingFactor2;
  double m_ScansSize[2];
  double m_ScansResolution[2];
  int m_ScansGrain;

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

  // operation tags
  mafTagItem *m_SliceIdStackTag;
  mafTagItem *m_ZValueStackTag;
  mafTagItem *m_OperationTypeStackTag;
  mafTagItem *m_Parameter1StackTag;
  mafTagItem *m_Parameter2StackTag;

  // coords tags
  mafTagItem *m_CoordsXTag;
  mafTagItem *m_CoordsYTag;
  mafTagItem *m_CoordsZTag;

  wxTextCtrl   *m_ScansNumberTxt;
  wxTextCtrl   *m_RegistrationXYSxalingFactorTxt;

  // text sources used in CreateParameterViewmafRWI()
  vtkTextSource *m_textSource[9] ;

};
#endif














