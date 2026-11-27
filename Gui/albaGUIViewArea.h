/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIViewArea
 Authors: Gianluigi Crimi

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaGUIViewArea_H__
#define __albaGUIViewArea_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include <wx/panel.h>
#include <vector>
#include <map>

// wx/aui is included only by albaGUIViewArea.cpp: its names clash with the ones of albaGUIDockManager.h
class wxAuiNotebook;
class albaGUIViewPage;
class albaGUIViewAreaHint;
class albaObserver;

/**
  Layout of the view areas: a leaf holds a notebook, a split holds its children side by side (columns) or stacked (rows),
  with their relative sizes. Nested splits always alternate direction, so each layout has a single representation.
*/
struct albaGUIViewAreaNode
{
	albaGUIViewAreaNode() { m_Notebook = NULL; m_Columns = false; }
	bool IsLeaf() { return m_Kids.empty(); }

	wxAuiNotebook *m_Notebook;
	bool m_Columns;
	std::vector<albaGUIViewAreaNode> m_Kids;
	std::vector<double> m_Weights;
};

/**
  Class Name: albaGUIViewArea.
  The area of the main frame that shows the views. It is split into at most four areas (at most one vertical and one
  horizontal division), each one a notebook whose pages are the views (albaGUIViewPage). Views are moved between the areas
  and new areas are created by dragging a tab; the drop zones and the layout are computed here, not by wx.
  In maximized mode only the active view is shown, on the whole view area, without tab bar; a single view is always
  shown this way.
  The listener receives VIEW_AREA_CHANGED when the active view, the list of the views or the maximized mode change.
*/
class ALBA_EXPORT albaGUIViewArea : public wxPanel
{
public:
	albaGUIViewArea(wxWindow *parent);
	~albaGUIViewArea();

	void SetListener(albaObserver *listener) { m_Listener = listener; }

	/** Add a view to the active area and make it the active view */
	void AddView(albaGUIViewPage *page);

	/** Remove a view from its area and destroy it; an area left empty disappears. Called by the page when it is closed */
	void RemoveView(albaGUIViewPage *page);

	/** Show the view and make it the active one */
	void ActivateView(albaGUIViewPage *page);

	/** Return the view shown in the active area, NULL if there are no views */
	albaGUIViewPage *GetActivePage();

	/** Return all the views, area by area (left to right, top to bottom) */
	std::vector<albaGUIViewPage *> GetPages();

	/** Set the title of the view on its tab */
	void SetViewCaption(albaGUIViewPage *page, wxString caption);

	/** Return the title of the view */
	wxString GetViewCaption(albaGUIViewPage *page);

	/** Maximized mode: only the active view is shown, without tab bar */
	void SetMaximized(bool maximize);
	bool IsMaximized() { return m_Maximized; }

	/** Return the notebook of the active area */
	wxAuiNotebook *GetActiveNotebook() { return m_Active; }

protected:
	struct Sash
	{
		wxRect m_Rect;
		std::vector<int> m_Path;
		int m_Index;
		bool m_Columns;
		int m_Extent;
		int m_Total;
	};

	wxAuiNotebook *CreateNotebook();
	void SetActive(wxAuiNotebook *nb);
	void RemoveIfEmpty(wxAuiNotebook *nb);
	wxAuiNotebook *FindNotebook(albaGUIViewPage *page);
	std::vector<wxAuiNotebook *> GetNotebooks();

	/** Select the active view in the application if it changed, and notify the listener */
	void ActivePageChanged();
	void Notify();

	void ComputeRects(albaGUIViewAreaNode &n, wxRect r, std::vector<int> &path, std::map<wxAuiNotebook *, wxRect> &rects, std::vector<Sash> *sashes);
	void DoLayout();

	int SashAt(wxPoint p);
	void OnMouseDown(wxMouseEvent &e);
	void OnMouseMove(wxMouseEvent &e);
	void OnMouseUp(wxMouseEvent &e);
	void OnCaptureLost(wxMouseCaptureLostEvent &e);
	void OnLeave(wxMouseEvent &e);
	void OnSize(wxSizeEvent &e);

	int ComputeDrop(wxAuiNotebook *src, wxAuiNotebook *&target, albaGUIViewAreaNode &result, wxRect &hint);
	void OnDragMotion(wxAuiNotebook *src);
	void OnEndDrag(wxAuiNotebook *src, int page);
	void HideHint();

	albaObserver *m_Listener;
	albaGUIViewAreaNode m_Root;
	wxAuiNotebook *m_Active;
	albaGUIViewPage *m_SelectedPage;
	bool m_Maximized;
	bool m_NotifyPending;
	int m_SashSize;
	std::vector<Sash> m_Sashes;
	int m_SashDrag;
	Sash m_SashInfo;
	wxPoint m_DragStart;
	int m_SashExtent;
	bool m_SashMoved;
	albaGUIViewAreaHint *m_Hint;
	albaGUIViewAreaHint *m_SashGhost;
};
#endif
