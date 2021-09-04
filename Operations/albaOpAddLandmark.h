/*=========================================================================
Program:   ALBA (Agile Library for Biomedical Applications)
Module:    albaOpAddLandmark.h
Language:  C++
Date:      $Date: 2021-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Paolo Quadrani, Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2021 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#ifndef __albaOpAddLandmark_H__
#define __albaOpAddLandmark_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaOp.h"

#include "albaEvent.h"
#include "albaInteractorPERPicker.h"
#include "albaString.h"

#include <vector>

#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/dom/DOM.hpp>


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
class ALBA_EXPORT albaOpAddLandmark : public albaOp
{
public:

	//----------------------------------------------------------------------------
	//Widgets ID's
	//----------------------------------------------------------------------------
	enum ADD_LANDMARK_ID
	{
		ID_LOAD_DICTIONARY = MINID,
		ID_SAVE_DICTIONARY,

		ID_LANDMARK_NAME,
		ID_LANDMARK_POSITION,
		ID_LANDMARK_RADIUS,
		ID_LANDMARK_ADD,
		ID_LANDMARK_REMOVE,

		ID_ADD_TO_CURRENT_TIME,
		ID_CHANGE_TIME,
		ID_SHOW_GROUP,

	};

	albaOpAddLandmark(const wxString &label = "AddLandmark  \tCtrl+A");
	~albaOpAddLandmark();
	virtual void OnEvent(albaEventBase *alba_event);

	
	void UpdateGui();

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

	albaVME* GetPickedVme() { return m_PickedVme; };
	albaVMELandmarkCloud* GetCloud() { return m_Cloud; };

	void SetCloudName(albaVMELandmarkCloud *cloud, albaString name);
	void SetCloudColor(albaVMELandmarkCloud *cloud, double r, double g, double b, double a);

	/** Add landmark to the cloud */
	void AddLandmark(double pos[3]);

	/** Remove landmark from the cloud and tree */
	void RemoveLandmark();

	/** Select landmark to the cloud */
	void SelectLandmark(albaString selection);

	/** Deselect landmark to the cloud */
	void DeselectLandmark();
	
	void SetLandmarkName(albaString name);
	void SetLandmarkPosition(double pos[3]);
	void SetLandmarkRadius(double radius);

	void EnableAddMode(bool mode) { m_AddModeFlag = mode; };
	void EnableShowAllMode(bool mode) { m_ShowAllMode = mode; };

	void LoadDictionary(wxString fileName = "");
	int SaveDictionary(wxString fileName = "");
	
protected:
	/** Create the AddLandmarks interface. */
	virtual void CreateGui();

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop(int result);

	/** Restore landmarks from vector. */
	void RestoreLandmarkVect(std::vector<albaVMELandmark*> &landmarkVect);

	int LoadLandmarksDefinitions(wxString fileName);
	int SaveLandmarksDefinitions(const char *landmarksFileName);

	bool CheckNodeElement(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node, const char *elementName);
	albaString GetElementAttribute(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node, const char *attributeName);
	void CheckEnableOkCondition();

	int LoadLandmarksFromVME();

	void ShowLandmarkGroup();

	void PushUniqueItem(wxString item);

	int m_AddToCurrentTime;

	std::vector<albaVMELandmark *> m_LandmarkUndoVetc;
	std::vector<albaVMELandmark *> m_LandmarkRedoVect;

	albaVME *m_PickedVme;

	albaVMELandmarkCloud *m_Cloud;
	albaVMELandmarkCloud *m_SelectedLandmarkCloud;
	albaVMELandmark *m_CurrentLandmark;
	albaVMELandmark *m_SelectedLandmark;

	std::vector<StringVector> m_LandmarkNameVect;
	StringVector m_LocalLandmarkNameVect;
	StringVector m_AllItemsNameVect;
	
	albaString m_CloudName;
	albaString m_LandmarkName;
	albaString m_SelectedLandmarkName;

	double m_LandmarkPosition[3]{ 0,0,0 };
	double m_LandmarkRadius;
	double m_OldColorCloud[4];

	bool m_IsCloudCreated;
	bool m_AddModeFlag;

	bool m_FirstOpDo;
	bool m_DictionaryLoaded;
	bool m_ShowAllMode;
	int m_LandmarkNameCount;
	int m_ShowMode;

	wxComboBox *m_ShowComboBox;

	albaString m_RemoveMessage;

	albaInteractorPERPicker	*m_LandmarkPicker;
	albaInteractor *m_OldBehavior;


	albaGUINamedPanel *m_GuiPanel;
	albaGUIDictionaryWidget *m_Dict;

	friend class albaOpAddLandmarkTest;
};
#endif
