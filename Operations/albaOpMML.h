/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpMML
 Authors: Mel Krokos
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpMML_H__
#define __albaOpMML_H__

#include "albaDefines.h"

#include <vtkPoints.h>
#include <vtkPolyData.h>

#include "albaEvent.h"
#include "albaGUI.h"
#include "albaGUICheckListBox.h"
#include "albaVME.h"
#include "albaTagItem.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVME;
class albaOp;
class albaGUIDialog;
class albaRWI;
class vtkCamera;
class albaGUILutSlider;
class vtkLookupTable;
class albaVMEVolumeGray;
class albaVMESurface;
class albaOpMMLParameterView;
class albaOpMMLModelView;
class albaOpMMLContourWidget;
class vtkDataSet;
class vtkPolyData;
class albaGUIButton;


//----------------------------------------------------------------------------
// albaOpMML :
// Muscle Modelling Lab.
// This operation allows the user to deform a polydata model of a muscle or bone
// to fit a volume image of the feature.
// The inputs are a volume (the patient data) and a surface (the model).
//----------------------------------------------------------------------------

class ALBA_EXPORT albaOpMML: public albaOp
{
public:
  void ApplyInverseRegistrationOps();
  void ApplyRegistrationOps();        // Apply the deformations to the model
  void OnSOperationButton();
  void OnROperationButton();
  void OnTOperationButton();
  void OnPOperationButton();
  void Update();
  bool SetUpWidget();

  albaOpMML(const wxString &label);
  virtual ~albaOpMML(); 
  void   OnEvent(albaEventBase *e);
  albaOp* Copy();

  void OpRun();
  void OpDo();
  void OpUndo();

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

  void    CreateFakeLandmarks();
  void    OnRegistrationCANCEL();
  bool    CreateInputsDlg();        // Dialog to select the polydata surface
  void    CreateRegistrationDlg();  // Dialog to perform the model fitting
  albaRWI* CreateParameterViewalbaRWI(wxString lab, float r, float g, float b);
  bool    SetUpInputs();
  bool	  SetUpParameterViews();
  bool	  SetUpModelView();
  void	  SetUpLandmarks(wxString AtlasSectionVMEName, wxString PatientSectionVMEName);

  albaGUIDialog	*m_ChooseDlg;		// inputs dialog
  albaGUIDialog	*m_OpDlg;			  // operation dialog

  // parameter views
  albaOpMMLParameterView *m_PH; // PH parameter view
  albaOpMMLParameterView *m_PV; // PV parameter view
  albaOpMMLParameterView *m_TH; // TH parameter view
  albaOpMMLParameterView *m_TV; // TV parameter view
  albaOpMMLParameterView *m_RA; // RA parameter view
  albaOpMMLParameterView *m_SN; // SN parameter view
  albaOpMMLParameterView *m_SS; // SS parameter view
  albaOpMMLParameterView *m_SE; // SE parameter view
  albaOpMMLParameterView *m_SW; // SW parameter view

  // parameter views alba RWIs
  albaRWI      *m_PHalbaRWI;		// PH parameter view alba RWI
  albaRWI      *m_PValbaRWI;		// PV parameter view alba RWI
  albaRWI      *m_THalbaRWI;		// TH parameter view alba RWI
  albaRWI      *m_TValbaRWI;		// TV parameter view alba RWI
  albaRWI      *m_RAalbaRWI;		// RA parameter view alba RWI
  albaRWI      *m_SNalbaRWI;		// SN parameter view alba RWI
  albaRWI      *m_SSalbaRWI;		// SS parameter view alba RWI
  albaRWI      *m_SEalbaRWI;		// SE parameter view alba RWI
  albaRWI      *m_SWalbaRWI;		// SW parameter view alba RWI

  // model view
  albaOpMMLModelView     *m_Model;

  // model view alba RWI
  albaRWI		*m_ModelalbaRWI;

  //
  albaOpMMLContourWidget *m_Widget;  // interactive contour widget

  void    OpStop(int result);

  void	ResetOperation();
  void	OnRegistrationOK();
  void	OnSlider();
  void	OnLut();
  void	OnMuscleSelection();
  void	OnResetView();
  void	OnUndo();
  void	OnOperation();
  void	OnContourLineAxesVisibility();
  void	OnContourVisibility();
  void	OnLandmark1AtlasPatientSelection();
  void	OnLandmark2AtlasPatientSelection();
  void	OnLandmark3AtlasPatientSelection();
  void	OnLandmark4AtlasPatientSelection();

  int             m_Slice;
  albaGUILutSlider   *m_Lut;
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
  int m_Flag3D;

  // 4 landmarks flag
  int m_LandmarksFlag;

  // input vme and vtk data for volume and selected muscle polydata
  albaVMEVolumeGray    *m_Vol;
  albaVMESurface       *m_Surface;
  vtkDataSet          *m_Volume;
  vtkPolyData         *m_Muscle;

  // other parameters
  //double m_ScansDistance;
  int	 m_ScansNumber, m_ScansNumber2;
  double m_RegistrationXYScalingFactor, m_RegistrationXYScalingFactor2;
  double m_ScansSize[2];
  double m_ScansResolution[2];
  int m_ScansGrain;

  albaGUIButton *m_ChooseOk;
  albaGUIButton *m_AxesOnOffButton;
  albaGUIButton *m_ContourOnOffButton;

  albaGUIButton *m_PlaceOpButton;
  albaGUIButton *m_TranslateOpButton;
  albaGUIButton *m_RotateOpButton;
  albaGUIButton *m_ScaleOpButton;
  albaGUIButton *m_ResetViewButton;
  albaGUIButton *m_UndoButton;
  albaGUIButton *m_OkButton;
  albaGUIButton *m_CancelButton;

  wxColour m_ButtonBackgroundColour;

  vtkMatrix4x4 *m_RegisteredTransform;

  // operation tags
  albaTagItem *m_SliceIdStackTag;
  albaTagItem *m_ZValueStackTag;
  albaTagItem *m_OperationTypeStackTag;
  albaTagItem *m_Parameter1StackTag;
  albaTagItem *m_Parameter2StackTag;

  // coords tags
  albaTagItem *m_CoordsXTag;
  albaTagItem *m_CoordsYTag;
  albaTagItem *m_CoordsZTag;

  wxTextCtrl   *m_ScansNumberTxt;
  wxTextCtrl   *m_RegistrationXYSxalingFactorTxt;
};
#endif














