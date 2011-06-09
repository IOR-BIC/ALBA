/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIDockManager.h,v $
  Language:  C++
  Date:      $Date: 2011-06-09 09:39:32 $
  Version:   $Revision: 1.1.2.4 $
  Authors:   Benjamin I. Williams
==========================================================================
  Copyright:   (C) Copyright 2005, Kirix Corporation, All Rights Reserved.
  Licence:     wxWindows Library Licence, Version 3.1
=========================================================================*/

#ifndef __mafGUIDockManager__H
#define __mafGUIDockManager__H

#include "mafDefines.h"

enum wxFrameManagerDock
{
    wxAUI_DOCK_NONE = 0,
    wxAUI_DOCK_TOP = 1,
    wxAUI_DOCK_RIGHT = 2,
    wxAUI_DOCK_BOTTOM = 3,
    wxAUI_DOCK_LEFT = 4,
    wxAUI_DOCK_CENTER = 5,
    wxAUI_DOCK_CENTRE = wxAUI_DOCK_CENTER
};

enum wxFrameManagerOption
{
    wxAUI_MGR_ALLOW_FLOATING        = 1 << 0,
    wxAUI_MGR_ALLOW_ACTIVE_PANE     = 1 << 1,
    wxAUI_MGR_TRANSPARENT_DRAG      = 1 << 2,
    wxAUI_MGR_TRANSPARENT_HINT      = 1 << 3,
    wxAUI_MGR_TRANSPARENT_HINT_FADE = 1 << 4,

    wxAUI_MGR_DEFAULT = wxAUI_MGR_ALLOW_FLOATING |
                        wxAUI_MGR_TRANSPARENT_HINT |
                        wxAUI_MGR_TRANSPARENT_HINT_FADE
};

enum wxPaneDockArtSetting
{
    wxAUI_ART_SASH_SIZE = 0,
    wxAUI_ART_CAPTION_SIZE = 1,
    wxAUI_ART_GRIPPER_SIZE = 2,
    wxAUI_ART_PANE_BORDER_SIZE = 3,
    wxAUI_ART_PANE_BUTTON_SIZE = 4,
    wxAUI_ART_BACKGROUND_COLOUR = 5,
    wxAUI_ART_SASH_COLOUR = 6,
    wxAUI_ART_ACTIVE_CAPTION_COLOUR = 7,
    wxAUI_ART_ACTIVE_CAPTION_GRADIENT_COLOUR = 8,
    wxAUI_ART_INACTIVE_CAPTION_COLOUR = 9,
    wxAUI_ART_INACTIVE_CAPTION_GRADIENT_COLOUR = 10,
    wxAUI_ART_ACTIVE_CAPTION_TEXT_COLOUR = 11,
    wxAUI_ART_INACTIVE_CAPTION_TEXT_COLOUR = 12,
    wxAUI_ART_BORDER_COLOUR = 13,
    wxAUI_ART_GRIPPER_COLOUR = 14,
    wxAUI_ART_CAPTION_FONT = 15,
    wxAUI_ART_GRADIENT_TYPE = 16
};

enum wxPaneDockArtGradients
{
    wxAUI_GRADIENT_NONE = 0,
    wxAUI_GRADIENT_VERTICAL = 1,
    wxAUI_GRADIENT_HORIZONTAL = 2
};

enum wxPaneButtonState
{
    wxAUI_BUTTON_STATE_NORMAL = 0,
    wxAUI_BUTTON_STATE_HOVER = 1,
    wxAUI_BUTTON_STATE_PRESSED = 2
};

enum wxPaneInsertLevel
{
    wxAUI_INSERT_PANE = 0,
    wxAUI_INSERT_ROW = 1,
    wxAUI_INSERT_DOCK = 2
};



// forwards and array declarations
class MAF_EXPORT wxDockUIPart;
class MAF_EXPORT wxPaneButton;
class MAF_EXPORT wxPaneInfo;
class MAF_EXPORT wxDockInfo;
class MAF_EXPORT wxDockArt;
class MAF_EXPORT wxFrameManagerEvent;

WX_DECLARE_OBJARRAY_WITH_DECL(wxDockInfo, wxDockInfoArray,class MAF_EXPORT);
WX_DECLARE_OBJARRAY_WITH_DECL(wxDockUIPart, wxDockUIPartArray,class MAF_EXPORT);
WX_DECLARE_OBJARRAY_WITH_DECL(wxPaneButton, wxPaneButtonArray,class MAF_EXPORT);
WX_DECLARE_OBJARRAY_WITH_DECL(wxPaneInfo, wxPaneInfoArray,class MAF_EXPORT);
WX_DEFINE_ARRAY_PTR(wxPaneInfo*, wxPaneInfoPtrArray);
WX_DEFINE_ARRAY_PTR(wxDockInfo*, wxDockInfoPtrArray);

MAF_EXPORT extern wxDockInfo wxNullDockInfo;
MAF_EXPORT extern wxPaneInfo wxNullPaneInfo;



/**
  class name: wxPaneInfo
  This class define properties of a pane. That is a panel that can be floating, dockable with a proper side.
*/
class MAF_EXPORT wxPaneInfo
{
public:

    /** constructor */
    wxPaneInfo()
    {
        window = NULL;
        frame = NULL;
        state = 0;
        dock_direction = wxAUI_DOCK_LEFT;
        dock_layer = 0;
        dock_row = 0;
        dock_pos = 0;
        floating_pos = wxDefaultPosition;
        floating_size = wxDefaultSize;
        best_size = wxDefaultSize;
        min_size = wxDefaultSize;
        max_size = wxDefaultSize;
        dock_proportion = 0;
        
        DefaultPane();
    }

    /** copy constructor */
    wxPaneInfo(const wxPaneInfo& c)
    {
        name = c.name;
        caption = c.caption;
        window = c.window;
        frame = c.frame;
        state = c.state;
        dock_direction = c.dock_direction;
        dock_layer = c.dock_layer;
        dock_row = c.dock_row;
        dock_pos = c.dock_pos;
        best_size = c.best_size;
        min_size = c.min_size;
        max_size = c.max_size;
        floating_pos = c.floating_pos;
        floating_size = c.floating_size;
        dock_proportion = c.dock_proportion;
        buttons = c.buttons;
        rect = c.rect;
    }

    /** assignment operator */
    wxPaneInfo& operator=(const wxPaneInfo& c)
    {
        name = c.name;
        caption = c.caption;
        window = c.window;
        frame = c.frame;
        state = c.state;
        dock_direction = c.dock_direction;
        dock_layer = c.dock_layer;
        dock_row = c.dock_row;
        dock_pos = c.dock_pos;
        best_size = c.best_size;
        min_size = c.min_size;
        max_size = c.max_size;
        floating_pos = c.floating_pos;
        floating_size = c.floating_size;
        dock_proportion = c.dock_proportion;
        buttons = c.buttons;
        rect = c.rect;
        return *this;
    }

    /** retrive if the pane exist */
    bool IsOk() const { return (window != NULL) ? true : false; }
    /** check flag optionResizable */
    bool IsFixed() const { return !HasFlag(optionResizable); }
    /** check flag optionResizable */
    bool IsResizable() const { return HasFlag(optionResizable); }
    /** check flag optionHidden */
    bool IsShown() const { return !HasFlag(optionHidden); }
    /** check flag optionFloating */
    bool IsFloating() const { return HasFlag(optionFloating); }
    /** check flag optionFloating */
    bool IsDocked() const { return !HasFlag(optionFloating); }
    /** check flag optionToolbar */
    bool IsToolbar() const { return HasFlag(optionToolbar); }
    /** check flag optionTopDockable */
    bool IsTopDockable() const { return HasFlag(optionTopDockable); }
    /** check flag optionBottomDockable */
    bool IsBottomDockable() const { return HasFlag(optionBottomDockable); }
    /** check flag optionLeftDockable */
    bool IsLeftDockable() const { return HasFlag(optionLeftDockable); }
    /** check flag optionRightDockable */
    bool IsRightDockable() const { return HasFlag(optionRightDockable); }
    /** check flag optionFloatable */
    bool IsFloatable() const { return HasFlag(optionFloatable); }
    /** check flag optionMovable */
    bool IsMovable() const { return HasFlag(optionMovable); }
    /** check flag optionCaption */
    bool HasCaption() const { return HasFlag(optionCaption); }
    /** check flag optionGripper */
    bool HasGripper() const { return HasFlag(optionGripper); }
    /** check flag optionPaneBorder */
    bool HasBorder() const { return HasFlag(optionPaneBorder); }
    /** check flag buttonClose */
    bool HasCloseButton() const { return HasFlag(buttonClose); }
    /** check flag buttonMaximize */
    bool HasMaximizeButton() const { return HasFlag(buttonMaximize); }
    /** check flag buttonMinimize */
    bool HasMinimizeButton() const { return HasFlag(buttonMinimize); }
    /** check flag buttonPin */
    bool HasPinButton() const { return HasFlag(buttonPin); }
    
    /** set window and retrieve this pointer */
    wxPaneInfo& Window(wxWindow* w) { window = w; return *this; }
    /** set name and retrieve this pointer */
    wxPaneInfo& Name(const wxString& n) { name = n; return *this; }
    /** set caption and retrieve this pointer */
    wxPaneInfo& Caption(const wxString& c) { caption = c; return *this; }
    /** set dock direction to left and retrieve this pointer */
    wxPaneInfo& Left() { dock_direction = wxAUI_DOCK_LEFT; return *this; }
    /** set dock direction to right and retrieve this pointer */
    wxPaneInfo& Right() { dock_direction = wxAUI_DOCK_RIGHT; return *this; }
    /** set dock direction to top and retrieve this pointer */
    wxPaneInfo& Top() { dock_direction = wxAUI_DOCK_TOP; return *this; }
    /** set dock direction to bottom and retrieve this pointer */
    wxPaneInfo& Bottom() { dock_direction = wxAUI_DOCK_BOTTOM; return *this; }
    /** set dock direction to center and retrieve this pointer */
    wxPaneInfo& Center() { dock_direction = wxAUI_DOCK_CENTER; return *this; }
    /** set dock direction to center and retrieve this pointer */
    wxPaneInfo& Centre() { dock_direction = wxAUI_DOCK_CENTRE; return *this; }
    /** set dock direction and retrieve this pointer */
    wxPaneInfo& Direction(int direction) { dock_direction = direction; return *this; }
    /** set layer and retrieve this pointer */
    wxPaneInfo& Layer(int layer) { dock_layer = layer; return *this; }
    /** set row and retrieve this pointer */
    wxPaneInfo& Row(int row) { dock_row = row; return *this; }
    /** set position and retrieve this pointer */
    wxPaneInfo& Position(int pos) { dock_pos = pos; return *this; }
    /** set best size and retrieve this pointer */
    wxPaneInfo& BestSize(const wxSize& size) { best_size = size; return *this; }
    /** set min size and retrieve this pointer */
    wxPaneInfo& MinSize(const wxSize& size) { min_size = size; return *this; }
    /** set max size and retrieve this pointer */
    wxPaneInfo& MaxSize(const wxSize& size) { max_size = size; return *this; }
    /** set best size and retrieve this pointer */
    wxPaneInfo& BestSize(int x, int y) { best_size.Set(x,y); return *this; }
    /** set min size and retrieve this pointer */
    wxPaneInfo& MinSize(int x, int y) { min_size.Set(x,y); return *this; }
    /** set max size and retrieve this pointer */
    wxPaneInfo& MaxSize(int x, int y) { max_size.Set(x,y); return *this; }
    /** set floating position and retrieve this pointer */
    wxPaneInfo& FloatingPosition(const wxPoint& pos) { floating_pos = pos; return *this; }
    /** set floating position and retrieve this pointer */
    wxPaneInfo& FloatingPosition(int x, int y) { floating_pos.x = x; floating_pos.y = y; return *this; }
    /** set floating size and retrieve this pointer */
    wxPaneInfo& FloatingSize(const wxSize& size) { floating_size = size; return *this; }
    /** set floating size and retrieve this pointer */
    wxPaneInfo& FloatingSize(int x, int y) { floating_size.Set(x,y); return *this; }
    /** set flag optionResizable and retrieve this pointer */
    wxPaneInfo& Fixed() { return SetFlag(optionResizable, false); }
    /** set flag optionResizable and retrieve this pointer */
    wxPaneInfo& Resizable(bool resizable = true) { return SetFlag(optionResizable, resizable); }
    /** set flag optionFloating and retrieve this pointer */
    wxPaneInfo& Dock() { return SetFlag(optionFloating, false); }
    /** set flag optionFloating and retrieve this pointer */
    wxPaneInfo& Float() { return SetFlag(optionFloating, true); }
    /** set flag optionHidden and retrieve this pointer */
    wxPaneInfo& Hide() { return SetFlag(optionHidden, true); }
    /** set flag optionHidden and retrieve this pointer */
    wxPaneInfo& Show(bool show = true) { return SetFlag(optionHidden, !show); }
    /** set flag optionCaption and retrieve this pointer */
    wxPaneInfo& CaptionVisible(bool visible = true) { return SetFlag(optionCaption, visible); }
    /** set flag optionPaneBorder and retrieve this pointer */
    wxPaneInfo& PaneBorder(bool visible = true) { return SetFlag(optionPaneBorder, visible); }
    /** set flag optionGripper and retrieve this pointer */
    wxPaneInfo& Gripper(bool visible = true) { return SetFlag(optionGripper, visible); }
    /** set flag buttonClose and retrieve this pointer */
    wxPaneInfo& CloseButton(bool visible = true) { return SetFlag(buttonClose, visible); }
    /** set flag buttonMaximize and retrieve this pointer */
    wxPaneInfo& MaximizeButton(bool visible = true) { return SetFlag(buttonMaximize, visible); }
    /** set flag buttonMinimize and retrieve this pointer */
    wxPaneInfo& MinimizeButton(bool visible = true) { return SetFlag(buttonMinimize, visible); }
    /** set flag buttonPin and retrieve this pointer */
    wxPaneInfo& PinButton(bool visible = true) { return SetFlag(buttonPin, visible); }
    /** set flag optionDestroyOnClose and retrieve this pointer */
    wxPaneInfo& DestroyOnClose(bool b = true) { return SetFlag(optionDestroyOnClose, b); }
    /** set flag optionTopDockable and retrieve this pointer */
    wxPaneInfo& TopDockable(bool b = true) { return SetFlag(optionTopDockable, b); }
    /** set flag optionBottomDockable and retrieve this pointer */
    wxPaneInfo& BottomDockable(bool b = true) { return SetFlag(optionBottomDockable, b); }
    /** set flag optionLeftDockable and retrieve this pointer */
    wxPaneInfo& LeftDockable(bool b = true) { return SetFlag(optionLeftDockable, b); }
    /** set flag optionRightDockable and retrieve this pointer */
    wxPaneInfo& RightDockable(bool b = true) { return SetFlag(optionRightDockable, b); } 
    /** set flag optionFloatable and retrieve this pointer */
    wxPaneInfo& Floatable(bool b = true) { return SetFlag(optionFloatable, b); }
    /** set flag optionMovable and retrieve this pointer */
    wxPaneInfo& Movable(bool b = true) { return SetFlag(optionMovable, b); }
    /** set TopDockable BottomDockable LeftDockable RightDockable and retrieve this pointer */
    wxPaneInfo& Dockable(bool b = true)
    {
        return TopDockable(b).BottomDockable(b).LeftDockable(b).RightDockable(b);
    }

    /** initialize pane flags and retrieve this pointer */
    wxPaneInfo& DefaultPane()
    {
        state |= optionTopDockable | optionBottomDockable |
                 optionLeftDockable | optionRightDockable |
                 optionFloatable | optionMovable | optionResizable |
                 optionCaption | optionPaneBorder | buttonClose;
        return *this;
    }
    
    /** set Center PaneBorder Resizable and retrieve this pointer */
    wxPaneInfo& CentrePane() { return CenterPane(); }
    /** set Center PaneBorder Resizable and retrieve this pointer */
    wxPaneInfo& CenterPane()
    {
        state = 0;
        return Center().PaneBorder().Resizable();
    }
    
    /** set optionToolbar optionGripper , remove optionResizable and optionCaption and retrieve this pointer */
    wxPaneInfo& ToolbarPane()
    {
        DefaultPane();
        state |= (optionToolbar | optionGripper);
        state &= ~(optionResizable | optionCaption);
        if (dock_layer == 0)
            dock_layer = 10;
        return *this;
    }

    /** set generic flag and retrieve this pointer */
    wxPaneInfo& SetFlag(unsigned int flag, bool option_state)
    {
        if (option_state)
            state |= flag;
             else
            state &= ~flag;
        return *this;
    }
    
    /** check if flag is active */
    bool HasFlag(unsigned int flag) const
    {
        return (state & flag) ? true:false;
    }

public:

    enum wxPaneState
    {
        optionFloating        = 1 << 0,
        optionHidden          = 1 << 1,
        optionLeftDockable    = 1 << 2,
        optionRightDockable   = 1 << 3,
        optionTopDockable     = 1 << 4,
        optionBottomDockable  = 1 << 5,
        optionFloatable       = 1 << 6,
        optionMovable         = 1 << 7,
        optionResizable       = 1 << 8,
        optionPaneBorder      = 1 << 9,
        optionCaption         = 1 << 10,
        optionGripper         = 1 << 11,
        optionDestroyOnClose  = 1 << 12,
        optionToolbar         = 1 << 13,
        optionActive          = 1 << 14,

        buttonClose           = 1 << 24,
        buttonMaximize        = 1 << 25,
        buttonMinimize        = 1 << 26,
        buttonPin             = 1 << 27,
        buttonCustom1         = 1 << 28,
        buttonCustom2         = 1 << 29,
        buttonCustom3         = 1 << 30,
        actionPane            = 1 << 31  // used internally
    };

public:
    wxString name;        // name of the pane
    wxString caption;     // caption displayed on the window

    wxWindow* window;     // window that is in this pane
    wxWindow* frame;      // floating frame window that holds the pane
    unsigned int state;   // a combination of wxPaneState values

    int dock_direction;   // dock direction (top, bottom, left, right, center)
    int dock_layer;       // layer number (0 = innermost layer)
    int dock_row;         // row number on the docking bar (0 = first row)
    int dock_pos;         // position inside the row (0 = first position)

    wxSize best_size;     // size that the layout engine will prefer
    wxSize min_size;      // minimum size the pane window can tolerate
    wxSize max_size;      // maximum size the pane window can tolerate

    wxPoint floating_pos; // position while floating
    wxSize floating_size; // size while floating
    int dock_proportion;  // proportion while docked

    wxPaneButtonArray buttons; // buttons on the pane

    wxRect rect;              // current rectangle (populated by wxAUI)
};




/**
class name: wxFrameManager
Manager for dockabale panels; it can manage flags, attach/detach panes.
*/
class MAF_EXPORT wxFrameManager : public wxEvtHandler
{
friend class wxFloatingPane;

public:

    /** constructor */
    wxFrameManager(wxFrame* frame = NULL,
                   unsigned int flags = wxAUI_MGR_DEFAULT);
    /** destructor */
    virtual ~wxFrameManager();
    /** remove event handler */
    void UnInit();
    
    /** set flags */
    void SetFlags(unsigned int flags);
    /** retrieve flags */
    unsigned int GetFlags() const;
    
    /** set frame */
    void SetFrame(wxFrame* frame);
    /** retrieve frame */
    wxFrame* GetFrame() const;
    
    /** set art provider */
    void SetArtProvider(wxDockArt* art_provider);
    /** retrieve art provider */
    wxDockArt* GetArtProvider() const;

    /** retrieve pane from window */
    wxPaneInfo& GetPane(wxWindow* window);
    /** retrieve pane from name */
    wxPaneInfo& GetPane(const wxString& name);
    /** retrieve all panes */
    wxPaneInfoArray& GetAllPanes();

    /** add pane */
    virtual bool AddPane(wxWindow* window,
                 const wxPaneInfo& pane_info);
    
    /** add pane */
    virtual bool AddPane(wxWindow* window,
                 int direction = wxLEFT,
                 const wxString& caption = wxEmptyString);
    /** insert pane, shifting the panes around, depending on the insert level*/             
    bool InsertPane(wxWindow* window,
                 const wxPaneInfo& pane_info,
                 int insert_level = wxAUI_INSERT_PANE);
              
    /** detach pane */
    bool DetachPane(wxWindow* window);

    /** saves all pane information as a single string */
    wxString SavePerspective();
    
    /** load perspective from a string*/
    bool LoadPerspective(const wxString& perspective,
                 bool update = true);
    
    /** update */
    virtual void Update();

private:
    /** draws a drop hint rectangle, */
    void DrawHintRect(wxWindow* pane_window,
                       const wxPoint& pt,
                       const wxPoint& offset);
    /** is an internal function which invokes wxSizer::Layout
        on the frame's main sizer, then measures all the various UI items
        and updates their internal rectangles.  This should always be called
        instead of calling m_Frame->Layout() directly
    */
    void DoFrameLayout();

    /** add pane */
    void LayoutAddPane(wxSizer* container,
                       wxDockInfo& dock,
                       wxPaneInfo& pane,
                       wxDockUIPartArray& uiparts,
                       bool spacer_only);
    /** add each pane to the dock */
    void LayoutAddDock(wxSizer* container,
                       wxDockInfo& dock,
                       wxDockUIPartArray& uiparts,
                       bool spacer_only);
    /** after emptying all docks out iterate through all known panes, 
    filing each of them into the appropriate dock.*/
    wxSizer* LayoutAll(wxPaneInfoArray& panes,
                       wxDockInfoArray& docks,
                       wxDockUIPartArray& uiparts,
                       bool spacer_only = false);
    /** determines where the pane's new position would be. */
    bool DoDrop(wxDockInfoArray& docks,
                wxPaneInfoArray& panes,
                wxPaneInfo& drop,
                const wxPoint& pt,
                const wxPoint& action_offset = wxPoint(0,0));

    /** function not implemented */
    wxPaneInfo& LookupPane(wxWindow* window);
    /** function not implemented */
    wxPaneInfo& LookupPane(const wxString& name);
    /** an internal function which determines
    which UI item the specified coordinates are over
    (x,y) specify a position in client coordinates */
    wxDockUIPart* HitTest(int x, int y);
    /** looks up the pane border UI part  */
    wxDockUIPart* GetPanePart(wxWindow* pane);
    /** returns a dock's offset in pixels from the left side of the window
       (for horizontal docks) or from the top of the window (for
        vertical docks)*/
    int GetDockPixelOffset(wxPaneInfo& test);
    /** called when starting to move pane */
    void OnFloatingPaneMoveStart(wxWindow* window);
    /** called when moving pane */
    void OnFloatingPaneMoving(wxWindow* window);
    /** called when pane has been moved */
    void OnFloatingPaneMoved(wxWindow* window);
    /** called when pane has been activated */
    void OnFloatingPaneActivated(wxWindow* window);
    /** called when pane has been closed */
    void OnFloatingPaneClosed(wxWindow* window);
    /** called when pane has been resized */
    void OnFloatingPaneResized(wxWindow* window, const wxSize& size);
    /** renders the entire user interface */
    void Render(wxDC* dc);
    /** calls render */
    void Repaint(wxDC* dc = NULL);
    /** process event */
    void ProcessMgrEvent(wxFrameManagerEvent& event);
    /** update button status */
    void UpdateButtonOnScreen(wxDockUIPart* button_ui_part,
                              const wxMouseEvent& event);
    /** retrieve pane positions and sizes*/
    void GetPanePositionsAndSizes(wxDockInfo& dock,
                              wxArrayInt& positions,
                              wxArrayInt& sizes);
    /** show hint */
    void ShowHint(const wxRect& rect);
    /** hide hint */
    void HideHint();
    /** remove hint */
    void RemoveHint();

private:

    /** OnPaint event */
    void OnPaint(wxPaintEvent& event);
    /** OnEraseBackground event */
    void OnEraseBackground(wxEraseEvent& event);
    /** OnSize event */
    void OnSize(wxSizeEvent& event);
    /** OnSetCursor event */
    void OnSetCursor(wxSetCursorEvent& event);
    /** OnLeftDown event */
    void OnLeftDown(wxMouseEvent& event);
    /** OnLeftUp event */
    void OnLeftUp(wxMouseEvent& event);
    /** OnMotion event */
    void OnMotion(wxMouseEvent& event);
    /** OnLeaveWindow event */
    void OnLeaveWindow(wxMouseEvent& event);
    /** OnPaneButton event */
    virtual void OnPaneButton(wxFrameManagerEvent& event);
    /** OnChildFocus event */
    void OnChildFocus(wxChildFocusEvent& event);
    /** OnHintFadeTimer event */
    void OnHintFadeTimer(wxTimerEvent& event);

private:

    enum
    {
        actionNone = 0,
        actionResize,
        actionClickButton,
        actionClickCaption,
        actionDragToolbarPane,
        actionDragFloatingPane
    };

protected:

    wxFrame* m_Frame;            // the frame being managed
    wxDockArt* m_Art;            // dock art object which does all drawing
    unsigned int m_Flags;        // manager flags wxAUI_MGR_*

    wxPaneInfoArray m_PanesArray;     // array of panes structures
    wxDockInfoArray m_Docks;     // array of docks structures
    wxDockUIPartArray m_UIParts; // array of UI parts (captions, buttons, etc)

    int m_Action;                // current mouse action
    wxPoint m_ActionStart;      // position where the action click started
    wxPoint m_ActionOffset;     // offset from upper left of the item clicked
    wxDockUIPart* m_ActionPart; // ptr to the part the action happened to
    wxWindow* m_ActionWindow;   // action frame or window (NULL if none)
    wxRect m_ActionHintRect;    // hint rectangle for the action
    wxDockUIPart* m_HoverButton;// button ui part being hovered over
    wxRect m_LastHint;          // last hint rectangle
    wxPoint m_LastMouseMove;   // last mouse move position (see OnMotion)

    wxWindow* m_HintWnd;        // transparent hint window (for now, only msw)
    wxTimer m_HintFadeTimer;    // transparent fade timer (for now, only msw)
    int m_HintFadeAmount;          // transparent fade amount (for now, only msw)
    
    /** event table declaration macro */
    DECLARE_EVENT_TABLE()
};


/**
  class name: wxFrameManagerEvent
  Event handled by frame manager
*/
class MAF_EXPORT wxFrameManagerEvent : public wxEvent
{
public:
    /** constructor */
    wxFrameManagerEvent(wxEventType type) : wxEvent(0, type)
    {
        m_Pane = NULL;
        m_Button = 0;
    }

    /** constructor */
    wxFrameManagerEvent(const wxFrameManagerEvent& c) : wxEvent(c)
    {
        m_Pane = c.m_Pane;
        m_Button = c.m_Button;
    }

    /** clone the event and retrieve the new instance */
    wxEvent *Clone() const { return new wxFrameManagerEvent(*this); }

    /** set pane member */
    void SetPane(wxPaneInfo* p) { m_Pane = p; }
    /** set button member */
    void SetButton(int b) { m_Button = b; }
    /** retrieve pane*/
    wxPaneInfo* GetPane() { return m_Pane; }
    /** retrieve button */
    int GetButton() { return m_Button; }

public:
    wxPaneInfo* m_Pane;
    int m_Button;
};




/**
  class name: wxDockArt
  dock art provider code - a dock provider provides all drawing
  functionality to the wxAui dock manager.  This allows the dock
  manager to have pluggable look-and-feels.
*/
class MAF_EXPORT wxDockArt
{
public:
    /** constructor */
    wxDockArt() { }
    /** destructor */
    virtual ~wxDockArt() { }

    /** get metric */
    virtual int GetMetric(int id) = 0;
    /** set metric */
    virtual void SetMetric(int id, int new_val) = 0;
    /** set font */
    virtual void SetFont(int id, const wxFont& font) = 0;
    /** get font */
    virtual wxFont GetFont(int id) = 0;
    /** get colour */
    virtual wxColour GetColour(int id) = 0;
    /** set colour */
    virtual void SetColour(int id, const wxColor& colour) = 0;
    /** get colour */
    wxColor GetColor(int id) { return GetColour(id); }
    /** set colour */
    void SetColor(int id, const wxColor& color) { SetColour(id, color); }
    
    /** draw sash */
    virtual void DrawSash(wxDC& dc,
                          int orientation,
                          const wxRect& rect) = 0;
    /** draw background */
    virtual void DrawBackground(wxDC& dc,
                          int orientation,
                          const wxRect& rect) = 0;
    /** draw caption */
    virtual void DrawCaption(wxDC& dc,
                          const wxString& text,
                          const wxRect& rect,
                          wxPaneInfo& pane) = 0;
    /** draw gripper */
    virtual void DrawGripper(wxDC& dc,
                          const wxRect& rect,
                          wxPaneInfo& pane) = 0;
    /** draw border */
    virtual void DrawBorder(wxDC& dc,
                          const wxRect& rect,
                          wxPaneInfo& pane) = 0;

    /** draw pane button */
    virtual void DrawPaneButton(wxDC& dc,
                          int button,
                          int button_state,
                          const wxRect& rect,
                          wxPaneInfo& pane) = 0;
};


/**
class name: wxDefaultDockArt
  this is the default art provider for wxFrameManager.  Dock art
  can be customized by creating a class derived from this one,
  or replacing this class entirely
*/
class MAF_EXPORT wxDefaultDockArt : public wxDockArt
{
public:
    /** constructor */
    wxDefaultDockArt();

    /** get metric */
    int GetMetric(int metric_id);
    /** set metric */
    void SetMetric(int metric_id, int new_val);
    /** get colour */
    wxColour GetColour(int id);
    /** set colour */
    void SetColour(int id, const wxColor& colour);
    /** set font */
    void SetFont(int id, const wxFont& font);
    /** get font */
    wxFont GetFont(int id);

    /** Draw Sash*/
    void DrawSash(wxDC& dc,
                  int orientation,
                  const wxRect& rect);
    /** Draw Background*/
    void DrawBackground(wxDC& dc,
                  int orientation,
                  const wxRect& rect);
    /** Draw Caption*/
    void DrawCaption(wxDC& dc,
                  const wxString& text,
                  const wxRect& rect,
                  wxPaneInfo& pane);
    /** Draw Gripper*/
    void DrawGripper(wxDC& dc,
                  const wxRect& rect,
                  wxPaneInfo& pane);

    /** Draw Border*/
    void DrawBorder(wxDC& dc,
                  const wxRect& rect,
                  wxPaneInfo& pane);
    /** Draw Pane Button*/
    void DrawPaneButton(wxDC& dc,
                  int button,
                  int button_state,
                  const wxRect& rect,
                  wxPaneInfo& pane);

protected:
    /** Draw Caption Background*/
    void DrawCaptionBackground(wxDC& dc, const wxRect& rect, bool active);

protected:

    wxPen m_BorderPen;
    wxBrush m_SashBrush;
    wxBrush m_BackgroundBrush;
    wxBrush m_GripperBrush;
    wxFont m_CaptionFont;
    wxBitmap m_InactiveCloseBitmap;
    wxBitmap m_InactivePinBitmap;
    wxBitmap m_ActiveCloseBitmap;
    wxBitmap m_ActivePinBitmap;
    wxPen m_GripperPen1;
    wxPen m_GripperPen2;
    wxPen m_GripperPen3;
    wxColour m_ActiveCaptionColour;
    wxColour m_ActiveCaptionGradientColour;
    wxColour m_ActiveCaptionTextColour;
    wxColour m_InactiveCaptionColour;
    wxColour m_InactiveCaptionGradientColour;
    wxColour m_InactiveCaptionTextColour;
    int m_BorderSize;
    int m_CaptionSize;
    int m_SashSize;
    int m_ButtonSize;
    int m_GripperSize;
    int m_GradientType;
};

/**
class name: wxDockInfo
This class define properties of a dock. That contains an array of panes.
*/
class MAF_EXPORT wxDockInfo
{
public:
    /** constructor */
    wxDockInfo()
    {
        dock_direction = 0;
        dock_layer = 0;
        dock_row = 0;
        size = 0;
        min_size = 0;
        resizable = true;
        fixed = false;
        toolbar = false;
    }
    /** copy constructor */
    wxDockInfo(const wxDockInfo& c)
    {
        dock_direction = c.dock_direction;
        dock_layer = c.dock_layer;
        dock_row = c.dock_row;
        size = c.size;
        min_size = c.min_size;
        resizable = c.resizable;
        fixed = c.fixed;
        toolbar = c.toolbar;
        panes = c.panes;
        rect = c.rect;
    }
    /** assignment operator */
    wxDockInfo& operator=(const wxDockInfo& c)
    {
        dock_direction = c.dock_direction;
        dock_layer = c.dock_layer;
        dock_row = c.dock_row;
        size = c.size;
        min_size = c.min_size;
        resizable = c.resizable;
        fixed = c.fixed;
        toolbar = c.toolbar;
        panes = c.panes;
        rect = c.rect;
        return *this;
    }
    /** retrieve if dock direction is active */
    bool IsOk() const { return (dock_direction != 0) ? true : false; }
    /** retrieve if dock direction is top or bottom */
    bool IsHorizontal() const { return (dock_direction == wxAUI_DOCK_TOP ||
                             dock_direction == wxAUI_DOCK_BOTTOM) ? true:false; }
    /** retrieve if dock direction is left , rigth or center */
    bool IsVertical() const { return (dock_direction == wxAUI_DOCK_LEFT ||
                             dock_direction == wxAUI_DOCK_RIGHT ||
                             dock_direction == wxAUI_DOCK_CENTER) ? true:false; }
public:
    wxPaneInfoPtrArray panes; // array of panes
    wxRect rect;              // current rectangle
    int dock_direction;       // dock direction (top, bottom, left, right, center)
    int dock_layer;           // layer number (0 = innermost layer)
    int dock_row;             // row number on the docking bar (0 = first row)
    int size;                 // size of the dock
    int min_size;             // minimum size of a dock (0 if there is no min)
    bool resizable;           // flag indicating whether the dock is resizable
    bool toolbar;             // flag indicating dock contains only toolbars
    bool fixed;               // flag indicating that the dock operates on
                              // absolute coordinates as opposed to proportional
};

/**
class name: wxDockUIPart
This class define a UI part of a dock.
*/
class MAF_EXPORT wxDockUIPart
{
public:
    enum
    {
        typeCaption,
        typeGripper,
        typeDock,
        typeDockSizer,
        typePane,
        typePaneSizer,
        typeBackground,
        typePaneBorder,
        typePaneButton,
    };

    int type;                // ui part type (see enum above)
    int orientation;         // orientation (either wxHORIZONTAL or wxVERTICAL)
    wxDockInfo* dock;        // which dock the item is associated with
    wxPaneInfo* pane;        // which pane the item is associated with
    wxPaneButton* button;    // which pane button the item is associated with
    wxSizer* cont_sizer;     // the part's containing sizer
    wxSizerItem* sizer_item; // the sizer item of the part
    wxRect rect;             // client coord rectangle of the part itself
};

/**
class name: wxPaneButton
This class define a button of a pane.
*/
class MAF_EXPORT wxPaneButton
{
public:
    int m_ButtonId;        // id of the button (e.g. buttonClose)
};




// wx event machinery


// right now the only event that works is wxEVT_AUI_PANEBUTTON. A full
// spectrum of events will be implemented in the next incremental version

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EXPORTED_EVENT_TYPE(MAF_EXPORT,wxEVT_AUI_PANEBUTTON, 0)
END_DECLARE_EVENT_TYPES()

typedef void (wxEvtHandler::*wxFrameManagerEventFunction)(wxFrameManagerEvent&);

#define wxFrameManagerEventHandler(func) \
    (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxFrameManagerEventFunction, &func)

#define EVT_AUI_PANEBUTTON(func) \
   wx__DECLARE_EVT0(wxEVT_AUI_PANEBUTTON, wxFrameManagerEventHandler(func))





/**
  class name: mafGUIDockManager
  Represent a frame manager for gui dock elements.
*/
class MAF_EXPORT mafGUIDockManager : public wxFrameManager 
{
public:
  /** add pane */
  virtual bool AddPane(wxWindow* window,const wxPaneInfo& pane_info, const wxString &menu = _("&View"), const wxString &subMenu = wxEmptyString);
  /** add pane overload*/
  virtual bool AddPane(wxWindow* window,int direction = wxLEFT,const wxString& caption = wxEmptyString);	
  /** update */
  virtual void Update();
protected:
  /** add menu item */
	void AddMenuItem(wxWindow* window,const wxString& caption = wxEmptyString, const wxString &menu = _("&View"), const wxString &subMenu = wxEmptyString );
  /** update menu items */
  void UpdateMenuItems();
};  


#endif  //__mafGUIDockManager__
