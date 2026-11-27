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


#include "albaDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

// Do not include albaGUIDockManager.h here: its names clash with the ones of wx/aui
#include <wx/aui/aui.h>
#include <wx/aui/tabartmsw.h>
#include "albaGUIViewArea.h"
#include "albaGUIViewPage.h"
#include "albaDecl.h"
#include "albaEvent.h"
#include "albaObserver.h"
#include "albaServiceLocator.h"
#include "albaAbsLogicManager.h"
#include "albaView.h"
#include <algorithm>

typedef albaGUIViewAreaNode Node;

enum { DROP_NONE = 0, DROP_CENTER = 1 }; // other values: wxLEFT, wxRIGHT, wxTOP, wxBOTTOM

//----------------------------------------------------------------------------
// Tab art: no border around the views, narrower tabs, and a full-width title bar when an area holds a single view
//----------------------------------------------------------------------------
class albaGUIViewTabArt : public wxAuiMSWTabArt
{
public:
	albaGUIViewTabArt(albaGUIViewArea *area) { m_Area = area; }
	wxAuiTabArt *Clone() override { return new albaGUIViewTabArt(*this); }

	static bool IsSingle(wxWindow *wnd)
	{
		wxAuiTabCtrl *tc = dynamic_cast<wxAuiTabCtrl *>(wnd);
		return tc && tc->GetPageCount() == 1;
	}

	void DrawBorder(wxDC &dc, wxWindow *wnd, const wxRect &rect) override {}
	int GetBorderWidth(wxWindow *wnd) override { return 0; }
	int GetAdditionalBorderSpace(wxWindow *wnd) override { return 0; }

	void DrawBackground(wxDC &dc, wxWindow *wnd, const wxRect &rect) override
	{
		if (!IsSingle(wnd))
		{
			wxAuiMSWTabArt::DrawBackground(dc, wnd, rect);
			return;
		}
		dc.SetPen(*wxTRANSPARENT_PEN);
		dc.SetBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
		dc.DrawRectangle(rect);
	}

	wxSize GetTabSize(wxDC &dc, wxWindow *wnd, const wxString &caption, const wxBitmapBundle &bitmap, bool active, int closeButtonState, int *xExtent) override
	{
		wxSize sz = wxAuiMSWTabArt::GetTabSize(dc, wnd, caption, bitmap, active, closeButtonState, xExtent);
		// Lower tabs: the tab bar height is computed from this size too
		sz.y -= wnd->FromDIP(4);
		if (IsSingle(wnd))
		{
			sz.x = *xExtent = wnd->GetClientSize().x;
			return sz;
		}
		sz.x -= wnd->FromDIP(6);
		*xExtent = sz.x;
		return sz;
	}

	void DrawTab(wxDC &dc, wxWindow *wnd, const wxAuiNotebookPage &page, const wxRect &inRect, int closeButtonState, wxRect *outTabRect, wxRect *outButtonRect, int *xExtent) override
	{
		if (!IsSingle(wnd))
		{
			wxAuiMSWTabArt::DrawTab(dc, wnd, page, inRect, closeButtonState, outTabRect, outButtonRect, xExtent);
			return;
		}
		wxRect r(0, 0, wnd->GetClientSize().x, wnd->GetClientSize().y);
		dc.SetPen(*wxTRANSPARENT_PEN);
		dc.SetBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
		dc.DrawRectangle(r);

		// The title of the active area is bold
		wxFont font = wnd->GetFont();
		if (wnd->GetParent() == m_Area->GetActiveNotebook())
			font.MakeBold();
		dc.SetFont(font);
		dc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));
		int tw, th;
		dc.GetTextExtent(page.caption, &tw, &th);
		dc.DrawText(page.caption, r.x + wnd->FromDIP(8), r.y + (r.height - th) / 2);

		int bs = r.height - 4;
		wxRect btn(r.GetRight() - bs - 2, r.y + 2, bs, bs);
		if (closeButtonState != wxAUI_BUTTON_STATE_HIDDEN)
		{
			if (closeButtonState == wxAUI_BUTTON_STATE_HOVER || closeButtonState == wxAUI_BUTTON_STATE_PRESSED)
			{
				dc.SetPen(*wxTRANSPARENT_PEN);
				dc.SetBrush(wxSystemSettings::GetColour(closeButtonState == wxAUI_BUTTON_STATE_PRESSED ? wxSYS_COLOUR_BTNSHADOW : wxSYS_COLOUR_BTNHIGHLIGHT));
				dc.DrawRectangle(btn);
			}
			wxBitmap bmp = m_activeCloseBmp.GetBitmapFor(wnd);
			dc.DrawBitmap(bmp, btn.x + (btn.width - bmp.GetLogicalWidth()) / 2, btn.y + (btn.height - bmp.GetLogicalHeight()) / 2, true);
		}
		*outButtonRect = btn;
		*outTabRect = r;
		*xExtent = r.width;
	}

protected:
	albaGUIViewArea *m_Area;
};

//----------------------------------------------------------------------------
// Translucent rectangle showing where the dragged view will go, also used as the bar that follows a dragged divider
//----------------------------------------------------------------------------
class albaGUIViewAreaHint : public wxFrame
{
public:
	albaGUIViewAreaHint(wxWindow *parent, wxColour colour, int alpha) : wxFrame(parent, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxFRAME_TOOL_WINDOW | wxFRAME_FLOAT_ON_PARENT | wxFRAME_NO_TASKBAR | wxNO_BORDER)
	{
		SetBackgroundColour(colour);
		SetTransparent(alpha);
		m_Timer.Bind(wxEVT_TIMER, &albaGUIViewAreaHint::OnTimer, this);
	}

	void ShowAt(wxRect r)
	{
		// The drag can end without notice (e.g. Esc or lost capture): hide when the button is released
		if (!m_Timer.IsRunning())
			m_Timer.Start(100);
		if (GetRect() != r)
			SetSize(r);
		if (!IsShown())
			ShowWithoutActivating();
	}

protected:
	void OnTimer(wxTimerEvent &e)
	{
		if (wxGetMouseState().LeftIsDown())
			return;
		Hide();
		m_Timer.Stop();
	}

	wxTimer m_Timer;
};

//----------------------------------------------------------------------------
// Layout helpers. At most one vertical and one horizontal division (see IsValidLayout)
//----------------------------------------------------------------------------
static Node *NodeAt(Node &root, std::vector<int> &path)
{
	Node *n = &root;
	for (int i : path)
		n = &n->m_Kids[i];
	return n;
}
//----------------------------------------------------------------------------
static bool FindPath(Node &n, wxAuiNotebook *nb, std::vector<int> &path)
{
	if (n.IsLeaf())
		return n.m_Notebook == nb;
	for (int i = 0; i < (int)n.m_Kids.size(); i++)
	{
		path.push_back(i);
		if (FindPath(n.m_Kids[i], nb, path))
			return true;
		path.pop_back();
	}
	return false;
}
//----------------------------------------------------------------------------
static void Leaves(Node &n, std::vector<wxAuiNotebook *> &out)
{
	if (n.IsLeaf())
		out.push_back(n.m_Notebook);
	for (Node &k : n.m_Kids)
		Leaves(k, out);
}
//----------------------------------------------------------------------------
static int Depth(Node &n)
{
	int d = 0;
	for (Node &k : n.m_Kids)
		d = wxMax(d, Depth(k) + 1);
	return d;
}
//----------------------------------------------------------------------------
static void ReplaceNotebook(Node &n, wxAuiNotebook *from, wxAuiNotebook *to)
{
	if (n.IsLeaf() && n.m_Notebook == from)
		n.m_Notebook = to;
	for (Node &k : n.m_Kids)
		ReplaceNotebook(k, from, to);
}
//----------------------------------------------------------------------------
// Merge children split in the same direction as their parent and collapse splits left with a single child
static void Normalize(Node &n)
{
	for (Node &k : n.m_Kids)
		Normalize(k);
	std::vector<Node> kids;
	std::vector<double> w;
	for (int i = 0; i < (int)n.m_Kids.size(); i++)
	{
		Node &k = n.m_Kids[i];
		if (!k.IsLeaf() && k.m_Columns == n.m_Columns)
		{
			double sum = 0;
			for (double x : k.m_Weights)
				sum += x;
			for (int j = 0; j < (int)k.m_Kids.size(); j++)
			{
				kids.push_back(k.m_Kids[j]);
				w.push_back(n.m_Weights[i] * k.m_Weights[j] / sum);
			}
		}
		else
		{
			kids.push_back(k);
			w.push_back(n.m_Weights[i]);
		}
	}
	n.m_Kids = kids;
	n.m_Weights = w;
	if (n.m_Kids.size() == 1)
	{
		Node only = n.m_Kids[0];
		n = only;
	}
}
//----------------------------------------------------------------------------
// A 2x2 grid is always stored as two columns of two rows
static void Canonical(Node &root)
{
	if (root.IsLeaf() || root.m_Columns || root.m_Kids.size() != 2)
		return;
	Node &r1 = root.m_Kids[0];
	Node &r2 = root.m_Kids[1];
	if (r1.m_Kids.size() != 2 || r2.m_Kids.size() != 2 || !r1.m_Kids[0].IsLeaf() || !r1.m_Kids[1].IsLeaf() || !r2.m_Kids[0].IsLeaf() || !r2.m_Kids[1].IsLeaf())
		return;
	Node grid;
	grid.m_Columns = true;
	for (int j = 0; j < 2; j++)
	{
		Node col;
		col.m_Columns = false;
		col.m_Kids.push_back(r1.m_Kids[j]);
		col.m_Kids.push_back(r2.m_Kids[j]);
		col.m_Weights.push_back(root.m_Weights[0]);
		col.m_Weights.push_back(root.m_Weights[1]);
		grid.m_Kids.push_back(col);
		grid.m_Weights.push_back(r1.m_Weights[j] / (r1.m_Weights[0] + r1.m_Weights[1]) + r2.m_Weights[j] / (r2.m_Weights[0] + r2.m_Weights[1]));
	}
	root = grid;
}
//----------------------------------------------------------------------------
// At most one vertical and one horizontal division: the whole area splits in two, and each half may split in two the other way
static bool IsValidLayout(Node &root)
{
	if (Depth(root) > 2 || root.m_Kids.size() > 2)
		return false;
	for (Node &k : root.m_Kids)
		if (k.m_Kids.size() > 2)
			return false;
	return true;
}
//----------------------------------------------------------------------------
static bool IsGrid(Node &root)
{
	return root.m_Kids.size() == 2 && root.m_Kids[0].m_Kids.size() == 2 && root.m_Kids[1].m_Kids.size() == 2;
}
//----------------------------------------------------------------------------
// In a 2x2 grid the horizontal division is a single one: both columns use the row sizes of the given column
static void SyncGrid(Node &root, int fromColumn)
{
	if (IsGrid(root))
		root.m_Kids[1 - fromColumn].m_Weights = root.m_Kids[fromColumn].m_Weights;
}
//----------------------------------------------------------------------------
static void RemoveLeaf(Node &root, wxAuiNotebook *nb)
{
	std::vector<int> path;
	if (!FindPath(root, nb, path) || path.empty())
		return;
	int idx = path.back();
	path.pop_back();
	Node *p = NodeAt(root, path);
	p->m_Kids.erase(p->m_Kids.begin() + idx);
	p->m_Weights.erase(p->m_Weights.begin() + idx);
	Normalize(root);
	Canonical(root);
}
//----------------------------------------------------------------------------
// Put a new area next to the target, on the given side; the target gives half of its space
static void SplitLeaf(Node &root, wxAuiNotebook *target, int side, wxAuiNotebook *nb)
{
	std::vector<int> path;
	if (!FindPath(root, target, path))
		return;
	bool cols = side == wxLEFT || side == wxRIGHT;
	bool before = side == wxLEFT || side == wxTOP;
	Node leaf;
	leaf.m_Notebook = nb;
	if (!path.empty())
	{
		int idx = path.back();
		std::vector<int> parentPath(path.begin(), path.end() - 1);
		Node *p = NodeAt(root, parentPath);
		if (p->m_Columns == cols)
		{
			double half = p->m_Weights[idx] / 2;
			p->m_Weights[idx] = half;
			int at = before ? idx : idx + 1;
			p->m_Kids.insert(p->m_Kids.begin() + at, leaf);
			p->m_Weights.insert(p->m_Weights.begin() + at, half);
			Normalize(root);
			Canonical(root);
			return;
		}
	}
	Node *t = NodeAt(root, path);
	Node split;
	split.m_Columns = cols;
	split.m_Kids.push_back(before ? leaf : *t);
	split.m_Kids.push_back(before ? *t : leaf);
	split.m_Weights.push_back(1);
	split.m_Weights.push_back(1);
	*t = split;
	Normalize(root);
	Canonical(root);
	if (IsGrid(root))
	{
		std::vector<int> newPath;
		FindPath(root, nb, newPath);
		SyncGrid(root, 1 - newPath[0]);
	}
}

//----------------------------------------------------------------------------
// albaGUIViewArea
//----------------------------------------------------------------------------
albaGUIViewArea::albaGUIViewArea(wxWindow *parent)
	: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxFULL_REPAINT_ON_RESIZE | wxCLIP_CHILDREN)
{
	m_Listener = NULL;
	m_SelectedPage = NULL;
	m_Maximized = false;
	m_NotifyPending = false;
	m_SashSize = FromDIP(3);
	m_SashDrag = -1;
	m_SashExtent = 0;
	m_SashMoved = false;
	m_Hint = NULL;
	m_SashGhost = NULL;
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
	m_Root.m_Notebook = CreateNotebook();
	m_Active = m_Root.m_Notebook;

	Bind(wxEVT_SIZE, &albaGUIViewArea::OnSize, this);
	Bind(wxEVT_MOTION, &albaGUIViewArea::OnMouseMove, this);
	Bind(wxEVT_LEFT_DOWN, &albaGUIViewArea::OnMouseDown, this);
	Bind(wxEVT_LEFT_UP, &albaGUIViewArea::OnMouseUp, this);
	Bind(wxEVT_MOUSE_CAPTURE_LOST, &albaGUIViewArea::OnCaptureLost, this);
	Bind(wxEVT_LEAVE_WINDOW, &albaGUIViewArea::OnLeave, this);
}
//----------------------------------------------------------------------------
albaGUIViewArea::~albaGUIViewArea()
{
	// The hints are children of the main frame, which destroys them
}

//----------------------------------------------------------------------------
wxAuiNotebook *albaGUIViewArea::CreateNotebook()
{
	wxAuiNotebook *nb = new wxAuiNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TOP | wxAUI_NB_TAB_MOVE | wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_CLOSE_ON_ACTIVE_TAB | wxAUI_NB_MIDDLE_CLICK_CLOSE | wxNO_BORDER);
	nb->SetArtProvider(new albaGUIViewTabArt(this));
	nb->Bind(wxEVT_AUINOTEBOOK_PAGE_CHANGED, [this, nb](wxAuiNotebookEvent &e) { SetActive(nb); ActivePageChanged(); e.Skip(); });
	nb->Bind(wxEVT_CHILD_FOCUS, [this, nb](wxChildFocusEvent &e) { SetActive(nb); ActivePageChanged(); e.Skip(); });
	nb->Bind(wxEVT_AUINOTEBOOK_DRAG_MOTION, [this, nb](wxAuiNotebookEvent &e) { OnDragMotion(nb); e.Skip(); });
	nb->Bind(wxEVT_AUINOTEBOOK_END_DRAG, [this, nb](wxAuiNotebookEvent &e) { OnEndDrag(nb, e.GetSelection()); });
	nb->Bind(wxEVT_AUINOTEBOOK_PAGE_CLOSE, [this, nb](wxAuiNotebookEvent &e)
	{
		// The page closes itself (it deletes the view), after the tab bar has finished handling the click
		e.Veto();
		albaGUIViewPage *page = (albaGUIViewPage *)nb->GetPage(e.GetSelection());
		CallAfter([this, page]
		{
			if (FindNotebook(page))
				page->Close();
		});
	});
	return nb;
}
//----------------------------------------------------------------------------
std::vector<wxAuiNotebook *> albaGUIViewArea::GetNotebooks()
{
	std::vector<wxAuiNotebook *> nbs;
	Leaves(m_Root, nbs);
	return nbs;
}
//----------------------------------------------------------------------------
wxAuiNotebook *albaGUIViewArea::FindNotebook(albaGUIViewPage *page)
{
	for (wxAuiNotebook *nb : GetNotebooks())
		if (nb->GetPageIndex(page) != wxNOT_FOUND)
			return nb;
	return NULL;
}
//----------------------------------------------------------------------------
std::vector<albaGUIViewPage *> albaGUIViewArea::GetPages()
{
	std::vector<albaGUIViewPage *> pages;
	for (wxAuiNotebook *nb : GetNotebooks())
		for (int i = 0; i < (int)nb->GetPageCount(); i++)
			pages.push_back((albaGUIViewPage *)nb->GetPage(i));
	return pages;
}
//----------------------------------------------------------------------------
albaGUIViewPage *albaGUIViewArea::GetActivePage()
{
	int sel = m_Active->GetSelection();
	return sel == wxNOT_FOUND ? NULL : (albaGUIViewPage *)m_Active->GetPage(sel);
}
//----------------------------------------------------------------------------
void albaGUIViewArea::AddView(albaGUIViewPage *page)
{
	page->Reparent(m_Active);
	m_Active->AddPage(page, page->GetView() ? wxStripMenuCodes(page->GetView()->GetLabel()) : wxString(), true);
	DoLayout();
	ActivePageChanged();
	Notify();
}
//----------------------------------------------------------------------------
void albaGUIViewArea::RemoveView(albaGUIViewPage *page)
{
	if (page == m_SelectedPage)
		m_SelectedPage = NULL;
	wxAuiNotebook *nb = FindNotebook(page);
	if (nb)
		nb->RemovePage(nb->GetPageIndex(page));
	ActivePageChanged();
	// The page is the one being closed: destroy it later, out of its own event handler; as a child of the view area it
	// does not depend on the notebook, which may be destroyed before
	page->Hide();
	page->Reparent(this);
	CallAfter([page] { page->Destroy(); });
	if (nb)
		CallAfter([this, nb] { RemoveIfEmpty(nb); });
	Notify();
}
//----------------------------------------------------------------------------
void albaGUIViewArea::ActivateView(albaGUIViewPage *page)
{
	wxAuiNotebook *nb = FindNotebook(page);
	if (!nb)
		return;
	SetActive(nb);
	int idx = nb->GetPageIndex(page);
	if (nb->GetSelection() != idx)
		nb->SetSelection(idx);
	DoLayout();
	ActivePageChanged();
	page->SetFocus();
}
//----------------------------------------------------------------------------
void albaGUIViewArea::SetViewCaption(albaGUIViewPage *page, wxString caption)
{
	wxAuiNotebook *nb = FindNotebook(page);
	if (!nb)
		return;
	nb->SetPageText(nb->GetPageIndex(page), caption);
	Notify();
}
//----------------------------------------------------------------------------
wxString albaGUIViewArea::GetViewCaption(albaGUIViewPage *page)
{
	wxAuiNotebook *nb = FindNotebook(page);
	return nb ? nb->GetPageText(nb->GetPageIndex(page)) : wxString();
}
//----------------------------------------------------------------------------
void albaGUIViewArea::SetMaximized(bool maximize)
{
	if (m_Maximized == maximize)
		return;
	m_Maximized = maximize;
	DoLayout();
	Notify();
}
//----------------------------------------------------------------------------
void albaGUIViewArea::SetActive(wxAuiNotebook *nb)
{
	if (m_Active == nb)
		return;
	m_Active = nb;
	// The title bar of the active area is bold; in maximized mode the shown view follows the active one
	if (m_Maximized)
		DoLayout();
	for (wxAuiNotebook *n : GetNotebooks())
		n->Refresh();
	Notify();
}
//----------------------------------------------------------------------------
void albaGUIViewArea::ActivePageChanged()
{
	albaGUIViewPage *page = GetActivePage();
	if (page == m_SelectedPage)
		return;
	m_SelectedPage = page;
	Notify();
	if (page && page->GetView() && !albaGUIViewPage::IsQuitting() && albaServiceLocator::GetLogicManager())
		albaServiceLocator::GetLogicManager()->SelectView(page->GetView());
}
//----------------------------------------------------------------------------
void albaGUIViewArea::Notify()
{
	// Several changes in a row are notified once
	if (m_NotifyPending || !m_Listener)
		return;
	m_NotifyPending = true;
	CallAfter([this]
	{
		m_NotifyPending = false;
		if (!m_Listener || albaGUIViewPage::IsQuitting())
			return;
		albaEvent e(this, VIEW_AREA_CHANGED);
		m_Listener->OnEvent(&e);
	});
}
//----------------------------------------------------------------------------
// An area left without views disappears, unless it is the last one
void albaGUIViewArea::RemoveIfEmpty(wxAuiNotebook *nb)
{
	std::vector<wxAuiNotebook *> nbs = GetNotebooks();
	if (std::find(nbs.begin(), nbs.end(), nb) == nbs.end())
		return;
	if (nb->GetPageCount() == 0 && nbs.size() > 1)
	{
		RemoveLeaf(m_Root, nb);
		if (m_Active == nb)
			SetActive(GetNotebooks().front());
		nb->Destroy();
	}
	DoLayout();
	ActivePageChanged();
}

//----------------------------------------------------------------------------
void albaGUIViewArea::ComputeRects(Node &n, wxRect r, std::vector<int> &path, std::map<wxAuiNotebook *, wxRect> &rects, std::vector<Sash> *sashes)
{
	if (n.IsLeaf())
	{
		rects[n.m_Notebook] = r;
		return;
	}
	int count = n.m_Kids.size();
	int size = n.m_Columns ? r.width : r.height;
	int avail = size - m_SashSize * (count - 1);
	double sum = 0;
	for (double x : n.m_Weights)
		sum += x;
	std::vector<int> ext(count);
	int used = 0;
	for (int i = 0; i < count; i++)
	{
		ext[i] = i == count - 1 ? avail - used : (int)(avail * n.m_Weights[i] / sum + 0.5);
		used += ext[i];
	}
	int pos = n.m_Columns ? r.x : r.y;
	for (int i = 0; i < count; i++)
	{
		wxRect cr = n.m_Columns ? wxRect(pos, r.y, ext[i], r.height) : wxRect(r.x, pos, r.width, ext[i]);
		path.push_back(i);
		ComputeRects(n.m_Kids[i], cr, path, rects, sashes);
		path.pop_back();
		if (i < count - 1 && sashes)
		{
			Sash s;
			s.m_Rect = n.m_Columns ? wxRect(pos + ext[i], r.y, m_SashSize, r.height) : wxRect(r.x, pos + ext[i], r.width, m_SashSize);
			s.m_Path = path;
			s.m_Index = i;
			s.m_Columns = n.m_Columns;
			s.m_Extent = ext[i];
			s.m_Total = ext[i] + ext[i + 1];
			sashes->push_back(s);
		}
		pos += ext[i] + m_SashSize;
	}
}
//----------------------------------------------------------------------------
void albaGUIViewArea::DoLayout()
{
	std::map<wxAuiNotebook *, wxRect> rects;
	std::vector<int> path;
	m_Sashes.clear();
	ComputeRects(m_Root, GetClientRect(), path, rects, &m_Sashes);
	// A single view always fills the whole view area without tab bar, as in maximized mode
	bool maximized = m_Maximized || GetPages().size() == 1;
	for (auto &r : rects)
	{
		wxAuiNotebook *nb = r.first;
		nb->SetTabCtrlHeight(maximized && nb == m_Active ? 0 : -1);
		nb->SetSize(maximized ? GetClientRect() : r.second);
		nb->Show(!maximized || nb == m_Active);
		nb->Refresh();
	}
	if (maximized)
		m_Sashes.clear();
}
//----------------------------------------------------------------------------
void albaGUIViewArea::OnSize(wxSizeEvent &e)
{
	DoLayout();
}

//----------------------------------------------------------------------------
// Dividers between the areas
//----------------------------------------------------------------------------
int albaGUIViewArea::SashAt(wxPoint p)
{
	for (int i = 0; i < (int)m_Sashes.size(); i++)
		if (m_Sashes[i].m_Rect.Contains(p))
			return i;
	return -1;
}
//----------------------------------------------------------------------------
void albaGUIViewArea::OnMouseDown(wxMouseEvent &e)
{
	m_SashDrag = SashAt(e.GetPosition());
	if (m_SashDrag < 0)
		return;
	m_DragStart = e.GetPosition();
	m_SashMoved = false;
	m_SashInfo = m_Sashes[m_SashDrag];
	CaptureMouse();
}
//----------------------------------------------------------------------------
void albaGUIViewArea::OnMouseMove(wxMouseEvent &e)
{
	// While dragging a divider only a bar follows the mouse; the areas are resized on release, which avoids continuous rendering
	if (m_SashDrag >= 0 && e.LeftIsDown())
	{
		Sash &s = m_SashInfo;
		int delta = s.m_Columns ? e.GetX() - m_DragStart.x : e.GetY() - m_DragStart.y;
		int minExt = FromDIP(60);
		m_SashExtent = wxMax(minExt, wxMin(s.m_Total - minExt, s.m_Extent + delta));
		wxRect bar = s.m_Rect;
		if (s.m_Columns)
			bar.x += m_SashExtent - s.m_Extent;
		else
			bar.y += m_SashExtent - s.m_Extent;
		// The horizontal division of a 2x2 grid is a single one, so its bar spans the whole width
		if (!s.m_Columns && s.m_Path.size() == 1 && IsGrid(m_Root))
		{
			bar.x = 0;
			bar.width = GetClientSize().x;
		}
		bar.SetPosition(ClientToScreen(bar.GetPosition()));
		if (!m_SashGhost)
			m_SashGhost = new albaGUIViewAreaHint(wxGetTopLevelParent(this), wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW), 200);
		m_SashGhost->ShowAt(bar);
		m_SashMoved = true;
		return;
	}
	int i = SashAt(e.GetPosition());
	SetCursor(i < 0 ? wxNullCursor : wxCursor(m_Sashes[i].m_Columns ? wxCURSOR_SIZEWE : wxCURSOR_SIZENS));
}
//----------------------------------------------------------------------------
void albaGUIViewArea::OnMouseUp(wxMouseEvent &e)
{
	if (HasCapture())
		ReleaseMouse();
	if (m_SashDrag < 0)
		return;
	// The bar hides itself when the button is released, so do not rely on its visibility
	if (m_SashMoved)
	{
		m_SashGhost->Hide();
		Sash &s = m_SashInfo;
		Node *n = NodeAt(m_Root, s.m_Path);
		double ws = n->m_Weights[s.m_Index] + n->m_Weights[s.m_Index + 1];
		n->m_Weights[s.m_Index] = ws * m_SashExtent / s.m_Total;
		n->m_Weights[s.m_Index + 1] = ws - n->m_Weights[s.m_Index];
		if (s.m_Path.size() == 1)
			SyncGrid(m_Root, s.m_Path[0]);
		DoLayout();
	}
	m_SashDrag = -1;
}
//----------------------------------------------------------------------------
void albaGUIViewArea::OnCaptureLost(wxMouseCaptureLostEvent &e)
{
	m_SashDrag = -1;
	if (m_SashGhost)
		m_SashGhost->Hide();
}
//----------------------------------------------------------------------------
void albaGUIViewArea::OnLeave(wxMouseEvent &e)
{
	if (m_SashDrag < 0)
		SetCursor(wxNullCursor);
}

//----------------------------------------------------------------------------
// Dragging a view
//----------------------------------------------------------------------------
// Where would the dragged view go: area under the pointer and side (or center: as a tab of that area)
int albaGUIViewArea::ComputeDrop(wxAuiNotebook *src, wxAuiNotebook *&target, Node &result, wxRect &hint)
{
	target = NULL;
	if (m_Maximized)
		return DROP_NONE;
	wxPoint p = ScreenToClient(::wxGetMousePosition());
	std::map<wxAuiNotebook *, wxRect> rects;
	std::vector<int> path;
	ComputeRects(m_Root, GetClientRect(), path, rects, NULL);
	for (auto &r : rects)
		if (r.second.Contains(p))
			target = r.first;
	if (!target)
		return DROP_NONE;
	wxRect r = rects[target];
	int tabBar = target->GetTabCtrlHeight();
	bool overTabBar = p.y < r.y + tabBar;
	if (target == src && overTabBar)
		return DROP_NONE; // reordering the tabs, handled by wx

	// Edge bands: a quarter of the area, at least 40 pixels, at most a third
	int side = DROP_CENTER;
	if (!overTabBar)
	{
		int bx = wxMin(r.width / 3, wxMax(FromDIP(40), r.width / 4));
		int by = wxMin(r.height / 3, wxMax(FromDIP(40), r.height / 4));
		double best = 1;
		double d[4] = { double(p.x - r.x) / bx, double(r.GetRight() - p.x) / bx, double(p.y - r.y - tabBar) / by, double(r.GetBottom() - p.y) / by };
		int sides[4] = { wxLEFT, wxRIGHT, wxTOP, wxBOTTOM };
		for (int i = 0; i < 4; i++)
			if (d[i] < best)
			{
				best = d[i];
				side = sides[i];
			}
	}

	if (side != DROP_CENTER)
	{
		// Moving out the only view of the source area removes that area; a split that breaks the limits becomes a tab drop
		wxAuiNotebook *newArea = (wxAuiNotebook *)this; // placeholder for the area to be created
		result = m_Root;
		bool valid = !(target == src && src->GetPageCount() == 1);
		if (valid)
		{
			if (src->GetPageCount() == 1)
				RemoveLeaf(result, src);
			SplitLeaf(result, target, side, newArea);
			valid = IsValidLayout(result);
		}
		if (valid)
		{
			std::map<wxAuiNotebook *, wxRect> newRects;
			std::vector<int> newPath;
			ComputeRects(result, GetClientRect(), newPath, newRects, NULL);
			hint = newRects[newArea];
			hint.SetPosition(ClientToScreen(hint.GetPosition()));
			return side;
		}
	}
	if (target == src)
		return DROP_NONE;
	hint = r;
	hint.SetPosition(ClientToScreen(hint.GetPosition()));
	return DROP_CENTER;
}
//----------------------------------------------------------------------------
void albaGUIViewArea::OnDragMotion(wxAuiNotebook *src)
{
	wxAuiNotebook *target;
	Node result;
	wxRect hint;
	if (ComputeDrop(src, target, result, hint) == DROP_NONE)
	{
		HideHint();
		return;
	}
	if (!m_Hint)
		m_Hint = new albaGUIViewAreaHint(wxGetTopLevelParent(this), wxSystemSettings::GetColour(wxSYS_COLOUR_HOTLIGHT), 90);
	m_Hint->ShowAt(hint);
}
//----------------------------------------------------------------------------
void albaGUIViewArea::OnEndDrag(wxAuiNotebook *src, int pageIndex)
{
	HideHint();
	wxAuiNotebook *target;
	Node result;
	wxRect hint;
	int drop = ComputeDrop(src, target, result, hint);
	if (drop == DROP_NONE)
		return;
	albaGUIViewPage *page = (albaGUIViewPage *)src->GetPage(pageIndex);
	// Act after the tab bar has finished handling the mouse release
	CallAfter([this, src, page, target, drop, result]
	{
		wxAuiNotebook *dest = target;
		if (drop != DROP_CENTER)
		{
			dest = CreateNotebook();
			m_Root = result;
			ReplaceNotebook(m_Root, (wxAuiNotebook *)this, dest);
		}
		int idx = src->GetPageIndex(page);
		wxString caption = src->GetPageText(idx);
		src->RemovePage(idx);
		page->Reparent(dest);
		dest->AddPage(page, caption, true);
		SetActive(dest);
		if (src->GetPageCount() == 0)
		{
			std::vector<int> path;
			if (FindPath(m_Root, src, path))
				RemoveLeaf(m_Root, src);
			src->Destroy();
		}
		DoLayout();
		ActivePageChanged();
		Notify();
	});
}
//----------------------------------------------------------------------------
void albaGUIViewArea::HideHint()
{
	if (m_Hint && m_Hint->IsShown())
		m_Hint->Hide();
}
