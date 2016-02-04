/*=========================================================================

 Program: MAF2
 Module: mafOpAddLandmark
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpAddLandmark_H__
#define __mafOpAddLandmark_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafOp.h"
#include "mafString.h"

#include <vector>

#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/dom/DOM.hpp>

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class mafVMELandmark;
class mafVMELandmarkCloud;
class mafInteractor;
class mafInteractorPicker;
class mafGUINamedPanel;
class mafGUIDictionaryWidget;

typedef std::vector<wxString> StringVector;

#ifdef MAF_EXPORTS
#include "mafDllMacros.h"
EXPORT_STL_VECTOR(MAF_EXPORT,mafVMELandmark*);
#endif
#include "mmaMaterial.h"
//----------------------------------------------------------------------------
// mafOpAddLandmark :
//----------------------------------------------------------------------------
class MAF_EXPORT mafOpAddLandmark: public mafOp
{
public:
	mafOpAddLandmark(const wxString &label = "AddLandmark");
	~mafOpAddLandmark(); 
	virtual void OnEvent(mafEventBase *maf_event);

  mafTypeMacro(mafOpAddLandmark, mafOp);

	mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode *node);

	/** Builds operation's interface. */
	void OpRun();

	/** Execute the operation. */
	void OpDo();

	/** Makes the undo for the operation. */
	void OpUndo();

  /** Add landmark to the cloud */
  void AddLandmark(double pos[3]);
	
	/** Remove landmark from the cloud and tree */
	void RemoveLandmark(mafString selection);

	/** Remove landmark to the cloud */
	void SelectLandmark(mafString selection);

	void SetPickingActiveFlag(bool picking) { m_PickingActiveFlag = picking; }
  bool GetPickingActiveFlag(){return m_PickingActiveFlag;}

  mafVME* GetPickedVme(){return m_PickedVme;};
  mafVMELandmarkCloud* GetCloud(){return m_Cloud;};

  void SetLandmarkName(mafString name){m_LandmarkName = name;};

protected:
	/** Create the AddLandmarks interface. */
	void CreateGui();

	/** Delete the AddLandmarks interface. */
	void DeleteGui();

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop(int result);

  /** Used in redo to add the landmarks to the tree*/
  void AddLandmarksToTree();

  /** Used in undo to remove the added landmarks from the tree*/
  void RemoveLandmarksFromTree();

	/** Set the color property of the material*/
	void SetMaterialRGBA(mmaMaterial *material, double r, double g, double b, double a);

	int LoadLandmarksGroups(wxString fileName);

	bool CheckNodeElement(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node, const char *elementName);
	mafString GetElementAttribute(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node, const char *attributeName);

	int LoadLocalLandmarks();
	void ShowLandmarkGroup();

	/** Show a message box to inform the user that the added landmark already exists in the tree.*/
  void ExistingLandmarkMessage();

	int m_AddToCurrentTime;

  mafVMELandmarkCloud	*m_Cloud;
	mafVME      				*m_PickedVme;

  std::vector<mafVMELandmark *> m_LandmarkAdded;	 
	std::vector<StringVector> m_LandmarkNameVect;
	StringVector m_LocalLandmarkNameVect;

	bool					m_CloudCreatedFlag;
	bool					m_PickingActiveFlag;

	mafString			m_LandmarkName;
	mafString			m_LandmarkSelected;
	mafString			m_PrevLandmarkSelected;

	int						m_ShowMode;
	wxComboBox		*m_ShowComboBox;

  mafInteractorPicker	*m_LandmarkPicker;
  mafInteractor       *m_OldBehavior;

	double							 m_LandmarkPosition[3];

  mafGUINamedPanel		*m_GuiPanel;
  mafGUIDictionaryWidget *m_Dict;
};
#endif
