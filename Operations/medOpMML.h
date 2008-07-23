/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpMML.h,v $
Language:  C++
Date:      $Date: 2008-07-23 12:11:42 $
Version:   $Revision: 1.12 $
Authors:   Mel Krokos
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medOpMML_H__
#define __medOpMML_H__

#include "mafDefines.h"

#include <vtkPoints.h>
#include <vtkPolyData.h>

#include "mafEvent.h"
#include "mmgGui.h"
#include "mmgCheckListBox.h"
#include "mafVME.h"
#include "mafTagItem.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class mafOp;
class mmgDialog;
class mafRWI;
class vtkCamera;
class mmgLutSlider;
class vtkLookupTable;
class mafVMEVolumeGray;
class mafVMESurface;
class medOpMMLParameterView;
class medOpMMLModelView;
class medOpMMLContourWidget;
class vtkDataSet;
class vtkPolyData;
class mmgButton;


//----------------------------------------------------------------------------
// medOpMML :
// Muscle Modelling Lab.
// This operation allows the user to deform a polydata model of a muscle or bone
// to fit a volume image of the feature.
// The inputs are a volume (the patient data) and a surface (the model).
//----------------------------------------------------------------------------

class medOpMML: public mafOp
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
  medOpMML(const wxString &label);
  virtual ~medOpMML(); 
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

  mmgDialog	*m_ChooseDlg;		// inputs dialog
  mmgDialog	*m_OpDlg;			  // operation dialog

  // parameter views
  medOpMMLParameterView *m_PH; // PH parameter view
  medOpMMLParameterView *m_PV; // PV parameter view
  medOpMMLParameterView *m_TH; // TH parameter view
  medOpMMLParameterView *m_TV; // TV parameter view
  medOpMMLParameterView *m_RA; // RA parameter view
  medOpMMLParameterView *m_SN; // SN parameter view
  medOpMMLParameterView *m_SS; // SS parameter view
  medOpMMLParameterView *m_SE; // SE parameter view
  medOpMMLParameterView *m_SW; // SW parameter view

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
  medOpMMLModelView     *m_Model;

  // model view maf RWI
  mafRWI		*m_ModelmafRWI;

  //
  medOpMMLContourWidget *m_Widget;  // interactive contour widget

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
  mmgLutSlider   *m_Lut;
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

  mmgButton *m_ChooseOk;
  mmgButton *m_AxesOnOffButton;
  mmgButton *m_ContourOnOffButton;

  mmgButton *m_PlaceOpButton;
  mmgButton *m_TranslateOpButton;
  mmgButton *m_RotateOpButton;
  mmgButton *m_ScaleOpButton;
  mmgButton *m_ResetViewButton;
  mmgButton *m_UndoButton;
  mmgButton *m_OkButton;
  mmgButton *m_CancelButton;

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














