/*=========================================================================

 Program: MAF2
 Module: mafOpMML
 Authors: Mel Krokos
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpMML_H__
#define __mafOpMML_H__

#include "mafDefines.h"

#include <vtkPoints.h>
#include <vtkPolyData.h>

#include "mafEvent.h"
#include "mafGUI.h"
#include "mafGUICheckListBox.h"
#include "mafVME.h"
#include "mafTagItem.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class mafOp;
class mafGUIDialog;
class mafRWI;
class vtkCamera;
class mafGUILutSlider;
class vtkLookupTable;
class mafVMEVolumeGray;
class mafVMESurface;
class mafOpMMLParameterView;
class mafOpMMLModelView;
class mafOpMMLContourWidget;
class vtkDataSet;
class vtkPolyData;
class mafGUIButton;


//----------------------------------------------------------------------------
// mafOpMML :
// Muscle Modelling Lab.
// This operation allows the user to deform a polydata model of a muscle or bone
// to fit a volume image of the feature.
// The inputs are a volume (the patient data) and a surface (the model).
//----------------------------------------------------------------------------

class MAF_EXPORT mafOpMML: public mafOp
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
  mafOpMML(const wxString &label);
  virtual ~mafOpMML(); 
  void   OnEvent(mafEventBase *e);
  mafOp* Copy();

  bool Accept(mafNode* vme);
  void OpRun();
  void OpDo();
  void OpUndo();

protected:

  void    CreateFakeLandmarks();
  void    OnRegistrationCANCEL();
  bool    CreateInputsDlg();        // Dialog to select the polydata surface
  void    CreateRegistrationDlg();  // Dialog to perform the model fitting
  mafRWI* CreateParameterViewmafRWI(wxString lab, float r, float g, float b);
  bool    SetUpInputs();
  bool	  SetUpParameterViews();
  bool	  SetUpModelView();
  void	  SetUpLandmarks(wxString AtlasSectionVMEName, wxString PatientSectionVMEName);

  mafGUIDialog	*m_ChooseDlg;		// inputs dialog
  mafGUIDialog	*m_OpDlg;			  // operation dialog

  // parameter views
  mafOpMMLParameterView *m_PH; // PH parameter view
  mafOpMMLParameterView *m_PV; // PV parameter view
  mafOpMMLParameterView *m_TH; // TH parameter view
  mafOpMMLParameterView *m_TV; // TV parameter view
  mafOpMMLParameterView *m_RA; // RA parameter view
  mafOpMMLParameterView *m_SN; // SN parameter view
  mafOpMMLParameterView *m_SS; // SS parameter view
  mafOpMMLParameterView *m_SE; // SE parameter view
  mafOpMMLParameterView *m_SW; // SW parameter view

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
  mafOpMMLModelView     *m_Model;

  // model view maf RWI
  mafRWI		*m_ModelmafRWI;

  //
  mafOpMMLContourWidget *m_Widget;  // interactive contour widget

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
  int m_Flag3D;

  // 4 landmarks flag
  int m_LandmarksFlag;

  // input vme and vtk data for volume and selected muscle polydata
  mafVMEVolumeGray    *m_Vol;
  mafVMESurface       *m_Surface;
  vtkDataSet          *m_Volume;
  vtkPolyData         *m_Muscle;

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

  vtkMatrix4x4 *m_RegisteredTransform;

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
};
#endif














