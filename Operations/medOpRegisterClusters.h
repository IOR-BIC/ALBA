/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpRegisterClusters.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 11:14:48 $
  Version:   $Revision: 1.2 $
  Authors:   Paolo Quadrani      - porting Daniele Giunchi
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medOpRegisterClusters_H__
#define __medOpRegisterClusters_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMEGroup.h"
#include "mafVMEInfoText.h"
#include "mafVMESurface.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGUI;
class mafEvent;
class vtkPoints;
class mafGUIDialog;

//----------------------------------------------------------------------------
// medOpRegisterClusters :
//----------------------------------------------------------------------------
/** */
class medOpRegisterClusters: public mafOp
{
public:
  medOpRegisterClusters(wxString label = _("Register Landmark Cloud"));
 ~medOpRegisterClusters(); 
  virtual void OnEvent(mafEventBase *maf_event);
  
  mafTypeMacro(medOpRegisterClusters, mafOp);
  
  mafOp* Copy();

	/** Return true for the acceptable vme type. */
  bool Accept(mafNode* node);   

	/** Builds operation's interface. */
  void OpRun();

	/** Execute the operation. */
  void OpDo();

	/** Makes the undo for the operation. */
  void OpUndo();

	static bool ClosedCloudAccept(mafNode* node) {return (node != NULL && node->IsA("mafVMELandmarkCloud") && !((mafVMELandmarkCloud*)node)->IsOpen());};

	static bool SurfaceAccept(mafNode* node) {return (node != NULL && node->IsMAFType(mafVMESurface));};

protected:
  /** Method called to extract matching point between source and target.*/
	int ExtractMatchingPoints(double time = -1);
  
	/** Register the source  on the target  according 
	to the registration method selected: rigid, similar or affine. */
	double RegisterPoints(double currTime = -1);

	/** Check the correctness of the vme's type. */
	void OnChooseVme(mafNode *vme);

  /** Open and Close source and Target Clouds */
  void CloseClouds();
  void OpenClouds();

	mafVMELandmarkCloud*    m_Source;
	mafVMELandmarkCloud*    m_Target;
	mafVMELandmarkCloud*    m_Registered;
  mafVMEGroup        *    m_Result;
  mafVMEInfoText     *    m_Info;

	mafVMELandmarkCloud*    m_CommonPoints;
	double*					m_Weight;			 

	mafVME *m_Follower;
  mafGUI *m_GuiSetWeights;
  mafGUIDialog *m_Dialog;
  
	mafString   m_SourceName;
	mafString   m_TargetName;
	mafString   m_FollowerName;
  
	vtkPoints *m_PointsSource;
	vtkPoints *m_PointsTarget;
  
	int m_RegistrationMode; 
	int m_MultiTime;           
	int m_Apply;
  int m_SettingsGuiFlag;

 };
#endif
