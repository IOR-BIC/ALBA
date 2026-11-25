/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module:  albaFakeLogicForTest.h
 Authors: Nicola Vanella
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef _albaFakeLogicForTest_H
#define _albaFakeLogicForTest_H

#include "albaConfigure.h"
#include "albaAbsLogicManager.h"

#include <vector>

class  albaFakeLogicForTest : public albaAbsLogicManager
{

public:

	enum testFunctions
	{
		FKL_VME_SELECT,
		FKL_VME_ADD,
		FKL_VME_REMOVE,
		FKL_VME_MODIFIED,
		FKL_VME_SHOW,
		FKL_VME_VISUAL_MODE_CHANGED,
		FKL_VME_REMOVED,
		FKL_PRINT_IMAGE,
		FKL_SHOW_VIEW_CONTEXTUAL_MENU
	};

	struct calls
	{
		testFunctions testFunction;
		albaVME *vme;
	};
	
	albaFakeLogicForTest();
	~albaFakeLogicForTest();

	// VME
	virtual void VmeSelect(albaVME *vme);

	virtual void VmeSelected(albaVME *vme);

	virtual void VmeShow(albaVME *vme, bool visibility);

	virtual void VmeAdd(albaVME *vme);

	virtual void VmeRemove(albaVME *vme);

	virtual void VmeModified(albaVME* vme);

	virtual void VmeVisualModeChanged(albaVME * vme);

	virtual void VmeAdded(albaVME *vme) override {}

	virtual void VmeRemoving(albaVME *vme) override {}

	/** Delete oldStorage and return a new storage with an initialized root */
	virtual albaVMEStorage *CreateStorage(albaVMEStorage *oldStorage) override;

	/** Return the first candidate accepted by acceptFunction (see SetChooseCandidates), NULL if none: no dialog is shown */
	virtual albaVME *ChooseVME(albaString title = "Choose Node", albaVMEAcceptFunction acceptFunction = NULL, albaVME *selected = NULL) override;

	/** Return all the candidates accepted by acceptFunction (see SetChooseCandidates): no dialog is shown */
	virtual std::vector<albaVME*> ChooseVMEs(albaString title = "Choose Nodes", albaVMEAcceptFunction acceptFunction = NULL, albaVME *selected = NULL) override;

	/** Set the VMEs offered to ChooseVME and ChooseVMEs, in the order the user would find them */
	void SetChooseCandidates(std::vector<albaVME*> candidates) { m_ChooseCandidates = candidates; }

	/** Return the number of ChooseVME and ChooseVMEs calls since the last ClearCalls */
	int GetNumberOfChooseCalls() { return m_ChooseCalls; }

	void ClearCalls();

	calls GetCall(int callNum);

	/** Return the number of calls stored since the last ClearCalls */
	int GetNumberOfCalls() { return m_Calls.size(); }

	/** Return the number of calls of func stored since the last ClearCalls */
	int GetNumberOfCalls(testFunctions func);
	
	void AddCall(testFunctions func, albaVME* vme);

	virtual bool IsOperationRunning();

	virtual void OpRunStarting() override {}

	virtual void OpRunTerminated(bool runOk) override {}

	virtual void OpShowGui(albaGUIPanel *panel) override {}

	virtual void ShowTreeContextualMenu(albaGUIVMETree *tree, albaVME *vme, bool vmeMenu) override {}

	/** Store a FKL_SHOW_VIEW_CONTEXTUAL_MENU call (see GetViewContextualMenuWithVME) */
	virtual void ShowViewContextualMenu(bool vmeMenu) override { AddCall(FKL_SHOW_VIEW_CONTEXTUAL_MENU, NULL); m_ViewContextualMenuWithVME = vmeMenu; }

	/** Return the vmeMenu of the last ShowViewContextualMenu call */
	bool GetViewContextualMenuWithVME() { return m_ViewContextualMenuWithVME; }

	virtual void TimeSet(double t) override {}

	virtual void OpHideGui() override {}

	virtual void CreateViewFrame(albaView *view) override {}

	virtual void ViewDeleted(albaView *view) override {}

	virtual void TileViewFrames(bool horizontally) override {}

	virtual void CascadeViewFrames() override {}

	virtual void UpdateFrameTitle() override {}

	virtual int FileNew() override { return ALBA_OK; }

	virtual int FileOpen(const char *file = NULL) override { return ALBA_OK; }

	virtual int FileSave() override { return ALBA_OK; }

	virtual int FileSaveAs(bool legacy = false) override { return ALBA_OK; }

	virtual int FileReload() override { return ALBA_OK; }

	virtual int CreateSnapshot() override { return ALBA_OK; }

	virtual void WizardRunStarting() override {}

	virtual void WizardRunTerminated() override {}

	virtual void WizardRunOp(const char *opName) override {}

	virtual void WizardRunOp(albaOp *op) override {}

	virtual void WizardRunOp(int opId) override {}
	
	virtual const char* GetMsfFileExtension() override;
	
	virtual void VmeRemoved() override;
	
	virtual void PrintImage(albaVMEImage *img) override;

	virtual albaProsthesesDBManager * GetProsthesesDBManager() override;

	virtual bool IsVMEExpanded(albaVME *vme) override;

	virtual void CollapseVME(albaVME *vme) override;

	virtual void ExpandVME(albaVME *vme) override;

	const char* GetAppBuildNum() override;

	void SetAppBuildNum(char* buildNum) override;

	const char* GetAlbaBuildNum() override;

	void SetAlbaBuildNum(char *buildNum) override;

	const char *GetCitationPaper() override;

	void SetCitationPaper(char *link) override;

	/** Store the selected view */
	void SelectView(albaView *view) override { m_SelectedView = view; };

	/** Return the selected view */
	albaView *GetSelectedView() override { return m_SelectedView; };

	bool IsTimeVaringEnabled() override;

protected:
	
	std::vector<calls> m_Calls;
	albaView *m_SelectedView = NULL;
	std::vector<albaVME*> m_ChooseCandidates;
	int m_ChooseCalls = 0;
	bool m_ViewContextualMenuWithVME = false;
};
#endif