/*=========================================================================
Program:   AssemblerPro
Module:     albaGUIHyperLink.h
Language:  C++
Date:      $Date: 2021-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2021 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#ifndef __albaGUIHyperLink_H__
#define __albaGUIHyperLink_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaObserver.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Class Name: albaGUIHyperLink
// Class that handles a slider for visualizing or changing range.
//----------------------------------------------------------------------------
class albaGUIHyperLink : public wxStaticText
{
public:

  /** Constructor */
  albaGUIHyperLink(wxWindow *parent, wxWindowID id, const wxString& label, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxStaticTextNameStr);

  /** Function for setting the listener of events coming from another object*/
  void SetListener(albaObserver *listener) {m_Listener = listener;}
	
	void SetText(wxString text) { m_ValueString = text; };
	void SetUrl(wxString url) { m_URL = url; };

protected:

	/** Initialize the values for the slider. */
  void Initialize();
	
	/** Move the cursors on mouse event. */
  void OnMouse(wxMouseEvent &event);

	albaObserver *m_Listener;

private:

  wxString m_ValueString;
	wxString m_URL;

  /** declaring event table macro */
  DECLARE_EVENT_TABLE()
};
#endif // _albaGUIHyperLink_H_
