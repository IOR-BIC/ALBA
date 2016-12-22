/*=========================================================================

 Program: MAF2
 Module: mafOpAddLandmark
 Authors: Paolo Quadrani, Nicola Vanella
 
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
#include "mafOp.h"

#include "mafEvent.h"
#include "mafString.h"

#include <vector>

#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/dom/DOM.hpp>
#include "mafInteractorPERPicker.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class mafVMELandmark;
class mafVMELandmarkCloud;
class mafInteractor;
class mafInteractorPERPicker;
class mafGUINamedPanel;
class mafGUIDictionaryWidget;

typedef std::vector<wxString> StringVector;

#ifdef MAF_EXPORTS
#include "mafDllMacros.h"
EXPORT_STL_VECTOR(MAF_EXPORT,mafVMELandmark*);
#endif
#include "mmaMaterial.h"
#include "mafInteractorPERPicker.h"
//----------------------------------------------------------------------------
// mafOpAddLandmark :
//----------------------------------------------------------------------------
class MAF_EXPORT mafOpAddLandmark: public mafOp
{
public:

	//----------------------------------------------------------------------------
	//Widgets ID's
	//----------------------------------------------------------------------------
	enum ADD_LANDMARK_ID
	{
		ID_LOAD = MINID,
		ID_SAVE,
		ID_ADD_TO_CURRENT_TIME,
		ID_CHANGE_POSITION,
		ID_CHANGE_NAME,
		ID_ADD_LANDMARK,
		ID_REMOVE_LANDMARK,
		ID_CHANGE_TIME,
		ID_SHOW_GROUP,
	};

	mafOpAddLandmark(const wxString &label = "AddLandmark");
	~mafOpAddLandmark(); 
	virtual void OnEvent(mafEventBase *maf_event);
		
	mafTypeMacro(mafOpAddLandmark, mafOp);

	virtual mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafVME*node);

	/** Builds operation's interface. */
	virtual void OpRun();

	/** Execute the operation. */
	void OpDo();

	/** Makes the undo for the operation. */
	void OpUndo();

	void SetCloudName(mafString name);

  /** Add landmark to the cloud */
  void AddLandmark(double pos[3]);
	
	/** Remove landmark from the cloud and tree */
	void RemoveLandmark();

	/** Select landmark to the cloud */
	void SelectLandmark(mafString selection);

	/** Deselect landmark to the cloud */
	void DeselectLandmark();

	void LoadDictionary(wxString fileName = "");
	int SaveDictionary(wxString fileName ="");

	mafVME* GetPickedVme(){return m_PickedVme;};
  mafVMELandmarkCloud* GetCloud(){return m_Cloud;};

  void SetLandmarkName(mafString name){m_LandmarkName = name;};

	void EnableAddMode(bool mode) { m_AddModeActive = mode; };

	void EnableShowAllMode(bool mode) { m_ShowAllMode = mode; };

	void SetLandmarkRadius(double radius) { m_Radius = radius; }

protected:
	/** Create the AddLandmarks interface. */
	virtual void CreateGui();

	/** Delete the AddLandmarks interface. */
	void DeleteGui();

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop(int result);

	/** Restore landmarks from vector. */
	void RestoreLandmarkVect(std::vector<mafVMELandmark*> &landmarkVect);

	/** Set the color property of the material*/
	void SetMaterialRGBA(mmaMaterial *material, double r, double g, double b, double a);

	int LoadLandmarksDefinitions(wxString fileName);
	int SaveLandmarksDefinitions(const char *landmarksFileName);

	bool CheckNodeElement(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node, const char *elementName);
	mafString GetElementAttribute(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node, const char *attributeName);

	int LoadLandmarksFromVME();
	void ShowLandmarkGroup();

	void CheckEnableOkCondition();

	void PushUniqueItem(wxString item);

	int m_AddToCurrentTime;

  mafVMELandmarkCloud	*m_Cloud;
	mafVME      				*m_PickedVme;

	mafVMELandmarkCloud	*m_SelectedLandmarkCloud;
	mafVMELandmark *m_CurrentLandmark;
	mafVMELandmark *m_SelectedLandmark;

	std::vector<mafVMELandmark *> m_LandmarkUndoVetc;
	std::vector<mafVMELandmark *> m_LandmarkRedoVect;

	std::vector<StringVector> m_LandmarkNameVect;
	StringVector							m_LocalLandmarkNameVect;
	StringVector							m_AllItemsNameVect;

	bool					m_CloudCreatedFlag;
	bool					m_AddModeActive;
	bool					m_AddLandmarkMode;
	bool					m_HasSelection;
	bool					m_FirstOpDo;
	bool					m_DictionaryLoaded;

	mafString			m_CloudName;
	mafString			m_LandmarkName;
	mafString			m_SelectedLandmarkName;

	bool					m_ShowAllMode;
	int						m_LandmarkNameCount;
	int						m_ShowMode;
	wxComboBox		*m_ShowComboBox;

	mafString			m_RemoveMessage;

  mafInteractorPERPicker	*m_LandmarkPicker;
  mafInteractor       *m_OldBehavior;

	double								m_LandmarkPosition[3];
	double								m_OldColorCloud[4];
	double								m_Radius;
  mafGUINamedPanel		*m_GuiPanel;
  mafGUIDictionaryWidget *m_Dict;

	friend class mafOpAddLandmarkTest;
};
#endif
