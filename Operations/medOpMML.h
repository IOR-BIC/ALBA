/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpMML.h,v $
Language:  C++
Date:      $Date: 2008-04-28 08:48:42 $
Version:   $Revision: 1.1 $
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

  mmgDialog	*m_choose_dlg;		// inputs dialog
  mmgDialog	*m_op_dlg;			  // operation dialog

  // parameter views
  medOpMMLParameterView *PH; // PH parameter view
  medOpMMLParameterView *PV; // PV parameter view
  medOpMMLParameterView *TH; // TH parameter view
  medOpMMLParameterView *TV; // TV parameter view
  medOpMMLParameterView *RA; // RA parameter view
  medOpMMLParameterView *SN; // SN parameter view
  medOpMMLParameterView *SS; // SS parameter view
  medOpMMLParameterView *SE; // SE parameter view
  medOpMMLParameterView *SW; // SW parameter view

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
  medOpMMLModelView     *Model;

  // model view maf RWI
  mafRWI		*m_ModelmafRWI;

  //
  medOpMMLContourWidget *Widget;  // interactive contour widget

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

  int             m_slice;
  mmgLutSlider   *m_lut;
  wxRadioBox     *m_radio;

  int             m_state;
  int             m_show_axes;
  int             m_ContourVisibility ;

  wxString             m_vol_name;
  wxString             m_surface_name;

  // msf file sections
  wxString			   m_AtlasMSFSectionName;
  wxString			   m_PatientMSFSectionName;

  //atlas landmarks
  //names
  wxString             m_l1_name;
  wxString             m_l2_name;
  wxString             m_l3_name;
  wxString             m_l4_name;
  // points
  double			   m_l1_point[3];
  double			   m_l2_point[3];
  double			   m_l3_point[3];
  double			   m_l4_point[3];

  //patient landmarks
  // names
  wxString             m_p1_name;
  wxString             m_p2_name;
  wxString             m_p3_name;
  wxString             m_p4_name;
  // points
  double			   m_p1[3];
  double			   m_p2[3];
  double			   m_p3[3];
  double			   m_p4[3];

  // muscle type
  int m_muscle_type;

  // registration status
  int m_RegistrationStatus;

  // 3d flag
  int m_3dflag;

  // 4 landmarks flag
  int m_Landmarksflag;

  // input vme and vtk data for volume and selected muscle polydata
  mafVMEVolumeGray    *m_vol;
  mafVMESurface       *m_surface;
  vtkDataSet          *Volume;
  vtkPolyData         *Muscle;

  // other parameters
  //double m_ScansDistance;
  int	 m_ScansNumber, m_ScansNumber2;
  double m_RegistrationXYScalingFactor, m_RegistrationXYScalingFactor2;
  double m_ScansSize[2];
  double m_ScansResolution[2];
  int m_ScansGrain;

  mmgButton *m_choose_ok;
  mmgButton *AxesOnOffButton;
  mmgButton *ContourOnOffButton;

  mmgButton *PlaceOpButton;
  mmgButton *TranslateOpButton;
  mmgButton *RotateOpButton;
  mmgButton *ScaleOpButton;
  mmgButton *ResetViewButton;
  mmgButton *UndoButton;
  mmgButton *OkButton;
  mmgButton *CancelButton;

  wxColour m_ButtonBackgroundColour;

  vtkMatrix4x4 *m_RegisteredTransform;

  // operation tags
  mafTagItem *SliceId_StackTag;
  mafTagItem *ZValue_StackTag;
  mafTagItem *OperationType_StackTag;
  mafTagItem *Parameter1_StackTag;
  mafTagItem *Parameter2_StackTag;

  // coords tags
  mafTagItem *CoordsXTag;
  mafTagItem *CoordsYTag;
  mafTagItem *CoordsZTag;

  wxTextCtrl   *ScansNumberTxt;
  wxTextCtrl   *RegistrationXYSxalingFactorTxt;
};
#endif














