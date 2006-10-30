/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoRegisterClusters.h,v $
  Language:  C++
  Date:      $Date: 2006-10-30 09:54:26 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani      - porting Daniele Giunchi
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoRegisterClusters_H__
#define __mmoRegisterClusters_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"
#include "mmgVMEChooserAccept.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMESurface.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mmgGui;
class mafEvent;
class vtkPoints;
class mmgDialog;

//----------------------------------------------------------------------------
// mmoRegisterClusters :
//----------------------------------------------------------------------------
/** */
class mmoRegisterClusters: public mafOp
{
public:
  mmoRegisterClusters(wxString label = "Register Landmark Cloud");
 ~mmoRegisterClusters(); 
  virtual void OnEvent(mafEventBase *maf_event);
  
  mafTypeMacro(mmoRegisterClusters, mafOp);
  
  mafOp* Copy();

	/** Return true for the acceptable vme type. */
  bool Accept(mafNode* node);   

	/** Builds operation's interface. */
  void OpRun();

	/** Execute the operation. */
  void OpDo();

	/** Makes the undo for the operation. */
  void OpUndo();

	class mafClusterAccept : public mmgVMEChooserAccept
	{
		public:
			
			mafClusterAccept() {};
		 ~mafClusterAccept() {};

		bool Validate(mafNode* node) {return (node != NULL && node->IsA("mafVMELandmarkCloud") && !((mafVMELandmarkCloud*)node)->IsOpen());};
	};
  mafClusterAccept *m_cluster_accept;

	class mafClusterSurfaceAccept : public mmgVMEChooserAccept
	{
		public:
			
			mafClusterSurfaceAccept() {};
		 ~mafClusterSurfaceAccept() {};

		bool Validate(mafNode* node) {return (node != NULL && node->IsMAFType(mafVMESurface) /*mafGetBaseType(vme) == VME_SURFACE*/);};
	};
  mafClusterSurfaceAccept *m_cluster_surface_accept;

protected:
  /** Method called to extract matching point between source and target.*/
	int ExtractMatchingPoints(double time = -1);
  
	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop(int result);
	
	/** Register the source  on the target  according 
	to the registration method selected: rigid, similar or affine. */
	void RegisterPoints(double currTime = -1);

	/** Check the correctness of the vme's type. */
	void OnChooseVme(mafNode *vme);

  /** Open and Close source and Target Clouds */
  void CloseClouds();
  void OpenClouds();

	mafVMELandmarkCloud*    m_Source;
	mafVMELandmarkCloud*    m_Target;
	mafVMELandmarkCloud*    m_Registered;

	mafVMELandmarkCloud*    m_CommonPoints;
	double*					m_Weight;			 

	mafVME *m_Follower;
  mmgGui *m_GuiSetWeights;
  mmgDialog *m_Dialog;
  
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
