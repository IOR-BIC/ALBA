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
#include "albaVME.h"
#include "albaVMELandmarkCloud.h"

#include <vector>
#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/dom/DOM.hpp>


//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaGUIDictionaryWidget;
class albaInteractor;
class albaInteractorPERPicker;
class albaVME;
class albaVMELandmark;
class albaVMELandmarkCloud;

typedef std::vector<wxString> StringVector;

#ifdef ALBA_EXPORTS
#include "albaDllMacros.h"
EXPORT_STL_VECTOR(ALBA_EXPORT,albaVMELandmark*);
#endif

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
		ID_LOAD_DICTIONARY_FROM_CLOUD,
		ID_SAVE_DICTIONARY,

		ID_LANDMARK_NAME,
		ID_LANDMARK_POSITION,
		ID_LANDMARK_RADIUS,
		ID_LANDMARK_ADD,
		ID_LANDMARK_REMOVE,

		ID_ADD_TO_CURRENT_TIME,
		ID_CHANGE_TIME,
		ID_SHOW_LANDMARK_GROUP,
	};

	albaOpAddLandmark(const wxString &label = "AddLandmark  \tCtrl+A");
	~albaOpAddLandmark();

	virtual void OnEvent(albaEventBase *alba_event);
		
	void UpdateGui(bool rebuildDict = false);

	albaTypeMacro(albaOpAddLandmark, albaOp);

	virtual albaOp* Copy();

	/** Return an xpm-icon that can be used to represent this operation */
	virtual char** GetIcon();

	/** Builds operation's interface. */
	virtual void OpRun();

	/** Execute the operation. */
	void OpDo();

	/** Makes the undo for the operation. */
	void OpUndo();

	albaVME* GetPickedVme() { return m_PickedVme; };
	albaVMELandmarkCloud* GetCloud() { return m_Cloud; };

	void SetCloudColor(albaVMELandmarkCloud *cloud, double r, double g, double b, double a);

	void SelectGroup(int index, int item = 0);

	/** Add landmark to the cloud */
	void AddLandmark(double pos[3]);

	/** Remove landmark from the cloud and tree */
	void RemoveLandmark();

	/** Select landmark to the cloud */
	void SelectLandmarkByName(albaString name);

	void FindDefinition(albaString &name);

	void SetLandmarkName(albaString name);
	void SetLandmarkPosition(double pos[3]);
	void SetLandmarkRadius(double radius);

	void EnableAddMode(bool mode) { m_AddModeFlag = mode; };

	void LoadDictionary(wxString fileName = "");
	void LoaDictionaryFromCloud(albaVMELandmarkCloud *cloud = NULL);
	int SaveDictionary(wxString fileName = "");

protected:
	/** Create the AddLandmarks interface. */
	virtual void CreateGui();

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop(int result);

	int LoadLandmarksFromVME();

	/** Restore landmarks from vector. */
	void RestoreLandmarkVect(std::vector<albaVMELandmark*> &landmarkVect);

	int LoadLandmarksDefinitions(wxString fileName);
	int SaveLandmarksDefinitions(const char *landmarksFileName);

	bool CheckNodeElement(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node, const char *elementName);
	albaString GetElementAttribute(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node, const char *attributeName);
	
	static bool LandmarkCloudAccept(albaVME* node) { return(node != NULL && node->IsALBAType(albaVMELandmarkCloud) /*&& node != m_Cloud && node =!m_AuxLandmarkCloud*/); };

	void PushUniqueItem(wxString item);
	void ReplaceItem(wxString oldItem, wxString newItem);
	void RemoveItem(wxString item);
	
	std::vector<albaVMELandmark *> m_LandmarkUndoVetc;
	std::vector<albaVMELandmark *> m_LandmarkRedoVect;

	albaVME *m_PickedVme;

	albaVMELandmarkCloud *m_Cloud;
	albaVMELandmark *m_SelectedLandmark;

	albaVMELandmarkCloud *m_AuxLandmarkCloud;
	albaVMELandmark *m_AuxLandmark;

	StringVector m_GroupsNameVect;
	std::vector<StringVector> m_LandmarkGroupVect;
	StringVector m_LandmarkNameVect;
	albaString m_CloudName;
	int m_SelectedGroup;
	
	albaString m_LandmarkName;
	double m_LandmarkPosition[3]{ 0,0,0 };
	double m_LandmarkRadius;
	double m_OldColorCloud[4];

	int m_AddToCurrentTime;
	bool m_IsCloudCreated;
	bool m_AddModeFlag;

	bool m_AddLandmarkFromDef;
	wxString m_LandmarkNameFromDef;

	bool m_FirstOpDo;
	bool m_DictionaryLoaded;

	albaString m_RemoveMessage;
	albaString m_DictMessage;

	albaInteractorPERPicker	*m_LandmarkPicker;

	wxComboBox *m_GroupComboBox;
	albaGUIDictionaryWidget *m_LandmarkGuiDict;
	int m_SelectedItem;

	friend class albaOpAddLandmarkTest;
};
#endif
