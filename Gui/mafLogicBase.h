/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafLogicBase.h,v $
  Language:  C++
  Date:      $Date: 2005-04-01 09:01:22 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden, Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mafLogicBase_H__
#define __mafLogicBase_H__

#include "mafDefines.h" //important: mafDefines should always be included as first
#include "mafEvent.h"

#include <wx/notebook.h>
#include <wx/icon.h>
//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class mafView;
class mmgMDIFrame;
class mmgSashPanel;
class mmgNamedPanel;
class mmgTimeBar;
class mafWXLog;
class mafSideBar;
//----------------------------------------------------------------------------
// mafLogicBase :
//----------------------------------------------------------------------------
class mafLogicBase: public mafEventListener
{
public:
               mafLogicBase();
	virtual     ~mafLogicBase(); 

	/** Method that answer at all MAF events. */
	void OnEvent(mafEvent& e);
	/** Visualize all the interface elements of the application.*/
	virtual void Show();
	/** Virtual function for plug views into the application.*/
	//SIL. 27-9-2004: virtual void Plug(mafView* view)						{};
	/** Virtual function for plug operations into the application.*/
	//SIL. 27-9-2004: virtual void Plug(mafOp *op)								{};
	/** Main initalization.*/
	virtual void Init(int argc, char **argv)		{};
	/** Returns the pointer to the main panel of the application.*/
	virtual mmgMDIFrame *GetTopWin()						{return m_win;};
	/** Sets the icon for the application.*/
	virtual void SetAppIcon(char **icon)				{m_AppIcon = icon;};
	/** Sets icon for the child windows.*/
	virtual void SetChildIcon(char **icon)			{m_ChildIcon = icon;};
	/** Sets the flag to know if men showld to be built.*/
	void PlugMenu(bool plug)		{m_PlugMenu			= plug;};
	/** Sets the flag to know if Toolbar showld to be built.*/
	void PlugToolbar(bool plug) {m_PlugToolbar	= plug;};
	/** Sets the flag to know if Side bar showld to be built.*/
	void PlugSidebar(bool plug) {m_PlugSidebar	= plug;};
	/** Sets the flag to know if Time bar showld to be built.*/
	void PlugTimebar(bool plug) {m_PlugTimebar	= plug;};
	/** Sets the flag to know if Log bar showld to be built.*/
	void PlugLogbar(bool plug)	{m_PlugLogbar		= plug;};

protected:
	/** Create the main men.*/
	virtual void CreateMenu();
	/** Virtual method to create the toolbar.*/
	virtual void CreateToolbar()								{};
	/** Virtual method to create the side panel, where visualize the tree representation.*/
	virtual void CreateSidebar()								{};
	/** Virtual method to create the time bar.*/
	virtual void CreateTimebar()								{};
	/** Virtual method to create the log bar.*/
	virtual void CreateLogbar()									{};
	/** Create a null logger. This is used when no log is due. */
	void CreateNullLog();
	/** Virtual method to update the user interface.*/
	virtual void UpdateUI(wxUpdateUIEvent *e)		{};
	/** Virtual method to update the time bounds.*/
	virtual void UpdateTimeBounds()							{};
	/** Virtual method to create data associated to the vme.*/
	// virtual void VmeDataCreate(mafNode *vme)			{};
	/** Virtual method to delete data associated to the vme. */
	// virtual void VmeDataDelete(mafNode *vme)			{};
	/** Called on Quit event. Destroy all the interface elements. */
	virtual void OnQuit();

  mmgMDIFrame       *m_win;
	char						 **m_AppIcon;
	char						 **m_ChildIcon;
  wxToolBar         *m_toolbar;
  mmgSashPanel      *m_log_bar;
  mmgSashPanel      *m_time_bar;
  wxMenuBar         *m_menu_bar;
	mafSideBar				*m_SideBarTabbed;
	wxString					 m_last_selected_panel;
  mmgTimeBar        *m_time_panel;
	wxString					 m_AppTitle;
	bool               m_log_to_file;
	bool               m_log_all_events;
  mafWXLog          *m_logger;

	bool m_PlugMenu;
	bool m_PlugToolbar;
	bool m_PlugSidebar;
	bool m_PlugTimebar;
	bool m_PlugLogbar;
};
#endif


