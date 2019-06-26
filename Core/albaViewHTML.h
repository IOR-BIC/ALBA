/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaViewHTML
 Authors: Paolo Quadrani    Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaViewHTML_H__
#define __albaViewHTML_H__


#include "albaView.h"

//----------------------------------------------------------------------------
// forward references
//----------------------------------------------------------------------------
class albaRWI;
class albaSceneGraph;
class albaSceneGraph;
class albaGUICheckListBox;
class wxHtmlWindow;
//----------------------------------------------------------------------------
// albaViewHTML :
//----------------------------------------------------------------------------
/**   */
class ALBA_EXPORT albaViewHTML: public albaView
{
public:
	          albaViewHTML(const wxString &label = "vtkViewHTML", int camera_position = CAMERA_PERSPECTIVE, bool show_axes = true, bool show_grid = false, int stereo = 0);
	virtual	 ~albaViewHTML();

  albaTypeMacro(albaViewHTML, albaView);

	albaView				*Copy(albaObserver *Listener = NULL, bool lightCopyEnabled = false);
  void           Create();
	albaSceneGraph *GetSceneGraph();
  albaRWIBase    *GetDefaultRWI();

  void VmeAdd   (albaVME *vme);
	void VmeRemove(albaVME *vme);
	void VmeSelect(albaVME *vme, bool select);
	void VmeShow  (albaVME *vme, bool show);
	void VmeUpdateProperty(albaVME *vme, bool fromTag = false) 	{};
  void VmeCreatePipe(albaVME *vme);
  void VmeDeletePipe(albaVME *vme)                           {};

	void OnEvent(albaEventBase *alba_event);

	virtual void SetBackgroundColor(wxColor color);

    /** IDs for the GUI */
  enum VIEW_HTML_WIDGET_ID
  {
	  ID_LOAD = MINID,
	  ID_FORWARD,
	  ID_BACK,
    ID_URL,
  };

protected:
  albaRWI	      *m_Rwi;
  albaSceneGraph *m_Sg;
  albaVME        *m_ActiveNote;

  void  OnLoad();
  void  OnForward();
  void  OnBack();
  
	albaGUI *CreateGui();

  wxHtmlWindow *m_Html;
  wxString m_Url;
};
#endif
