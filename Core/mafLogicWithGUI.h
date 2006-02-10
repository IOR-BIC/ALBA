/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafLogicWithGUI.h,v $
  Language:  C++
  Date:      $Date: 2006-02-10 16:16:29 $
  Version:   $Revision: 1.13 $
  Authors:   Silvano Imboden, Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mafLogicWithGUI_H__
#define __mafLogicWithGUI_H__
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafDefines.h" //important: mafDefines should always be included as first
#include "mafEvent.h"
#include "mafObserver.h"
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
class mafVTKLog;
class mafSideBar;
//----------------------------------------------------------------------------
// mafLogicWithGUI :
//----------------------------------------------------------------------------
/**
mafLogicWithGUI 'can' be the base class for an Application Logic,
and provide the most common user-interface set-up.
Features:
- MDI Main Frame
- Menu    (can be avoided by calling PlugMenu(false); )
- ToolBar (can be avoided by calling PlugToolbarBar(false); )
- LogBar  (can be avoided by calling PlugLogBar(false); )
- TimeBar (can be avoided by calling PlugTimeBar(false); )
- SideBar (can be avoided by calling PlugSideBar(false); )

The PlugXXX functions must be called before the first Show().
In derived classes it is recommended to pass to mafLogicWithGUI::OnEvent()
the unhandled events.


Tech NOTE: 
Proper Initialization must follow this order
1- CTOR:
2- Configure: (create the GUI el. - cant be in the CTOR because calls virtual functions)
   in sub classes also create the manager
3- Plug Op, Plug View , ...
4- Show:
5- Init: Calls FileOpen or FileNew
*/
class mafLogicWithGUI: public mafObserver
{
public:
               mafLogicWithGUI();
	virtual     ~mafLogicWithGUI(); 

	/** Method that answer at all MAF events. */
	void OnEvent(mafEventBase *maf_event);
	/** Visualize all the interface elements of the application.*/
	virtual void Show();
	/** Virtual function for plug views into the application.*/
	//SIL. 27-9-2004: virtual void Plug(mafView* view)						{};
	/** Virtual function for plug operations into the application.*/
	//SIL. 27-9-2004: virtual void Plug(mafOp *op)								{};

  /** Configure */
  virtual void Configure();

  /** Main initialisation.*/
  virtual void Init(int argc, char **argv) {};
	/** Returns the pointer to the main panel of the application.*/
	virtual mmgMDIFrame *GetTopWin()						{return m_Win;};
	/** Sets the icon for the application.*/
	//virtual void SetAppIcon(char **icon)				{m_AppIcon = icon;};
	/** Sets icon for the child windows.*/
	//virtual void SetChildIcon(char **icon)			{m_ChildIcon = icon;};

  /** Sets the flag to know if menu should be built.*/
	void PlugMenu(bool plug)		{m_PlugMenu			= plug;};
	/** Sets the flag to know if Toolbar should be built.*/
	void PlugToolbar(bool plug) {m_PlugToolbar	= plug;};
	/** Sets the flag to know if Side bar should be built.*/
	void PlugSidebar(bool plug) {m_PlugSidebar	= plug;};
	/** Sets the flag to know if Time bar should be built.*/
	void PlugTimebar(bool plug) {m_PlugTimebar	= plug;};
	/** Sets the flag to know if Log bar should be built.*/
	void PlugLogbar(bool plug)	{m_PlugLogbar		= plug;};

  /** 
  Show the splash screen for the application. To define your own splash screen image simply
  overwrite the SPLASH_SCREEN image into the picture factory by plugging your .xpm image. \sa mafPictureFactory*/
  virtual void ShowSplashScreen();

  /** 
  Used to give a splash screen image directly from the bitmap without converting it in xpm.*/
  virtual void ShowSplashScreen(wxBitmap &splashImage);

protected:
	/** Create the main men.*/
	virtual void CreateMenu();
	/** Virtual method to create the toolbar.*/
	virtual void CreateToolbar();
	/** Virtual method to create the side panel, where visualize the tree representation.*/
	virtual void CreateSidebar();
	/** Virtual method to create the time bar.*/
	virtual void CreateTimebar();
	/** Virtual method to create the log bar.*/
	virtual void CreateLogbar();
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

  /** Enable/disable a Toolbar or Menu Item */
  void EnableItem(int item, bool enable);

  /** Initialize language used into the application.*/
  void InitializeLanguage();

  mmgMDIFrame       *m_Win;
  wxToolBar         *m_ToolBar;
  mmgSashPanel      *m_LogSash;
  mmgSashPanel      *m_TimeSash;
  mmgSashPanel			*m_SideSash;
  wxMenuBar         *m_MenuBar;
	wxString					 m_LastSelectedPanel;
  mmgTimeBar        *m_TimePanel;
	mafString					 m_AppTitle;
	bool               m_LogToFile;
	bool               m_LogAllEvents;
  mafWXLog          *m_Logger;
  mafVTKLog         *m_VtkLog;
  wxLocale           m_Locale;
  wxLanguage         m_Language;
  mafString          m_LanguageDictionary;

	bool m_PlugMenu;    ///< Flag to plug or not the Menu into the application. Default is true.
	bool m_PlugToolbar; ///< Flag to plug or not the Toolbar into the application. Default is true.
	bool m_PlugSidebar; ///< Flag to plug or not the Side-bar into the application. Default is true.
	bool m_PlugTimebar; ///< Flag to plug or not the Time-bar into the application. Default is true.
	bool m_PlugLogbar;  ///< Flag to plug or not the Log area into the application. Default is true.
};
#endif
