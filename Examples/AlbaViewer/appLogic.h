/*=========================================================================
Program:   AlbaViewer
Module:    appLogic.h
Language:  C++
Date:      $Date: 2019-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2019 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#ifndef __appLogic_H__
#define __appLogic_H__

//----------------------------------------------------------------------------
// Includes :
//----------------------------------------------------------------------------
#include "albaLogicWithManagers.h"

//----------------------------------------------------------------------------
// Forward reference :
//----------------------------------------------------------------------------
class albaOpImporterDicom;
class albaVTKLog;

//----------------------------------------------------------------------------
// Class Name: appLogic
//----------------------------------------------------------------------------
class appLogic: public albaLogicWithManagers
{
public:

  /** Constructor. */
	appLogic();

  /** Destructor. */
	virtual ~appLogic(); 

	/*virtual*/ void OnEvent(albaEventBase *alba_event);
		
	/** Program Initialization */
	/*virtual*/ void Init(int argc, char **argv);

	void RunOp(albaOp *op);
	
	/** Called by the VME manager for each VME added to the tree: tell it to SideBar, ViewManager, ecc.. */
	virtual void VmeAdded(albaVME *vme) override;

protected:
  
	/** FILE NEW evt. handler */
	virtual int FileNew() override;

	/** FILE OPEN evt. handler.*/
	virtual int FileOpen(const char *file_to_open = NULL) override;

	/** Called on Quit event. */
	virtual void OnQuit();
	  	
  /** Redefined to add Meter controls*/
  virtual void CreateToolbar();
  
	/** Enable/Disable menu items when an operation start/end running */
	virtual void EnableMenuAndToolbar();

  /** Show arbitrary view vertical tiled with blend ct view */
  void ShowVMEOnView();
	
	/** A new View has Been Created - Logic must create the View Frame */
	virtual void ViewCreate(int viewId);

	/** Redefined to add View,Op, Import, Export menu */
	virtual void CreateMenu();

	/** Show contextual menu for views when right mouse click arrives */
	virtual void ShowViewContextualMenu(bool vmeMenu) override {};

	/* Initialize About Dialog */
	void InitAboutDialog();

  	// Operations
	albaOpImporterDicom	*m_OpImporterDicom;

private:
	//
};
#endif
