/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpAddLandmark
 Authors: Paolo Quadrani, Nicola Vanella
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpAddLandmark_H__
#define __albaOpAddLandmark_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaOp.h"

#include "albaEvent.h"
#include "albaString.h"

#include <vector>

#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/dom/DOM.hpp>
#include "albaInteractorPERPicker.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVME;
class albaVMELandmark;
class albaVMELandmarkCloud;
class albaInteractor;
class albaInteractorPERPicker;
class albaGUINamedPanel;
class albaGUIDictionaryWidget;

typedef std::vector<wxString> StringVector;

#ifdef ALBA_EXPORTS
#include "albaDllMacros.h"
EXPORT_STL_VECTOR(ALBA_EXPORT,albaVMELandmark*);
#endif
#include "mmaMaterial.h"
#include "albaInteractorPERPicker.h"
//----------------------------------------------------------------------------
// albaOpAddLandmark :
//----------------------------------------------------------------------------
class ALBA_EXPORT albaOpAddLandmark: public albaOp
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
		ID_RADIUS,
	};

	albaOpAddLandmark(const wxString &label = "AddLandmark  \tCtrl+A");
	~albaOpAddLandmark(); 
	virtual void OnEvent(albaEventBase *alba_event);
		
	albaTypeMacro(albaOpAddLandmark, albaOp);

	virtual albaOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(albaVME*node);

	/** Builds operation's interface. */
	virtual void OpRun();

	/** Execute the operation. */
	void OpDo();

	/** Makes the undo for the operation. */
	void OpUndo();

	void SetCloudName(albaString name);

  /** Add landmark to the cloud */
  void AddLandmark(double pos[3]);
	
	/** Remove landmark from the cloud and tree */
	void RemoveLandmark();

	/** Select landmark to the cloud */
	void SelectLandmark(albaString selection);

	/** Deselect landmark to the cloud */
	void DeselectLandmark();

	void LoadDictionary(wxString fileName = "");
	int SaveDictionary(wxString fileName ="");

	albaVME* GetPickedVme(){return m_PickedVme;};
  albaVMELandmarkCloud* GetCloud(){return m_Cloud;};

  void SetLandmarkName(albaString name){m_LandmarkName = name;};

	void EnableAddMode(bool mode) { m_AddModeActive = mode; };

	void EnableShowAllMode(bool mode) { m_ShowAllMode = mode; };

	void SetLandmarkRadius(double radius);

protected:
	/** Create the AddLandmarks interface. */
	virtual void CreateGui();

	/** Delete the AddLandmarks interface. */
	void DeleteGui();

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop(int result);

	/** Restore landmarks from vector. */
	void RestoreLandmarkVect(std::vector<albaVMELandmark*> &landmarkVect);

	/** Set the color property of the material*/
	void SetMaterialRGBA(mmaMaterial *material, double r, double g, double b, double a);

	int LoadLandmarksDefinitions(wxString fileName);
	int SaveLandmarksDefinitions(const char *landmarksFileName);

	bool CheckNodeElement(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node, const char *elementName);
	albaString GetElementAttribute(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node, const char *attributeName);

	int LoadLandmarksFromVME();
	void ShowLandmarkGroup();

	void CheckEnableOkCondition();

	void PushUniqueItem(wxString item);

	int m_AddToCurrentTime;

  albaVMELandmarkCloud	*m_Cloud;
	albaVME      				*m_PickedVme;

	albaVMELandmarkCloud	*m_SelectedLandmarkCloud;
	albaVMELandmark *m_CurrentLandmark;
	albaVMELandmark *m_SelectedLandmark;

	std::vector<albaVMELandmark *> m_LandmarkUndoVetc;
	std::vector<albaVMELandmark *> m_LandmarkRedoVect;

	std::vector<StringVector> m_LandmarkNameVect;
	StringVector							m_LocalLandmarkNameVect;
	StringVector							m_AllItemsNameVect;

	bool					m_CloudCreatedFlag;
	bool					m_AddModeActive;
	bool					m_AddLandmarkMode;
	bool					m_HasSelection;
	bool					m_FirstOpDo;
	bool					m_DictionaryLoaded;

	albaString			m_CloudName;
	albaString			m_LandmarkName;
	albaString			m_SelectedLandmarkName;

	bool					m_ShowAllMode;
	int						m_LandmarkNameCount;
	int						m_ShowMode;
	wxComboBox		*m_ShowComboBox;

	albaString			m_RemoveMessage;

  albaInteractorPERPicker	*m_LandmarkPicker;
  albaInteractor       *m_OldBehavior;

	double								m_LandmarkPosition[3];
	double								m_OldColorCloud[4];
	double								m_Radius;
  albaGUINamedPanel		*m_GuiPanel;
  albaGUIDictionaryWidget *m_Dict;

	friend class albaOpAddLandmarkTest;
};
#endif
