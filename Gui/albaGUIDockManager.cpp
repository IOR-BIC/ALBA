/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIDockManager
 Authors: Benjamin I. Williams
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include <wx/wx.h>
#include <wx/minifram.h>
#include <wx/image.h>
#include "albaGUIDockManager.h"
#include "albaGUIMDIFrame.h"

// -- various array and event implementations --

#include <wx/arrimpl.cpp>
WX_DECLARE_OBJARRAY(wxRect, wxAuiRectArray);
WX_DEFINE_OBJARRAY(wxAuiRectArray)
WX_DEFINE_OBJARRAY(wxDockUIPartArray)
WX_DEFINE_OBJARRAY(wxDockInfoArray)
WX_DEFINE_OBJARRAY(wxPaneButtonArray)
WX_DEFINE_OBJARRAY(wxPaneInfoArray)

wxPaneInfo wxNullPaneInfo;
wxDockInfo wxNullDockInfo;
DEFINE_EVENT_TYPE(wxEVT_AUI_PANEBUTTON)

#ifdef __WXMAC__
    // a few defines to avoid name clashes
    #define __MAC_OS_X_MEMORY_MANAGER_CLEAN__ 1
    #define __AIFF__
    #include "wx/mac/private.h"
#endif


// -- wxDefaultDockArt class implementation --

// wxDefaultDockArt is an art provider class which does all of the drawing for
// wxFrameManager.  This allows the library caller to customize the dock art
// (probably by deriving from this class), or to completely replace all drawing
// with custom dock art (probably by writing a new stand-alone class derived
// from the wxDockArt base class). The active dock art class can be set via
// wxFrameManager::SetDockArt()


// StepColour() it a utility function that simply darkens
// or lightens a color, based on the specified percentage
static wxColor StepColour(const wxColor& c, int percent)
{
    int r = c.Red(), g = c.Green(), b = c.Blue();
    return wxColour(wxMin((r*percent)/100,255),
                    wxMin((g*percent)/100,255),
                    wxMin((b*percent)/100,255));
}

static wxColor LightContrastColour(const wxColour& c)
{
    int amount = 120;

    // if the color is especially dark, then
    // make the contrast even lighter
    if (c.Red() < 128 && c.Green() < 128 && c.Blue() < 128)
        amount = 160;

    return StepColour(c, amount);
}

// BitmapFromBits() is a utility function that creates a
// masked bitmap from raw bits (XBM format)
static wxBitmap BitmapFromBits(const unsigned char bits[], int w, int h,
                               const wxColour& color)
{
    wxImage img = wxBitmap((const char*)bits, w, h).ConvertToImage();
    img.Replace(255,255,255,123,123,123);
    img.Replace(0,0,0,color.Red(),color.Green(),color.Blue());
    img.SetMaskColour(123,123,123);
    return wxBitmap(img);
}


static void DrawGradientRectangle(wxDC& dc,
                                  const wxRect& rect,
                                  const wxColour& start_color,
                                  const wxColour& end_color,
                                  int direction)
{
    int rd, gd, bd, high = 0;
    rd = end_color.Red() - start_color.Red();
    gd = end_color.Green() - start_color.Green();
    bd = end_color.Blue() - start_color.Blue();

    if (direction == wxAUI_GRADIENT_VERTICAL)
        high = rect.GetHeight()-1;
         else
        high = rect.GetWidth()-1;

    for (int i = 0; i <= high; ++i)
    {
        int r = start_color.Red() +  ((i*rd*100)/high)/100;
        int g = start_color.Green() + ((i*gd*100)/high)/100;
        int b = start_color.Blue() + ((i*bd*100)/high)/100;

        wxPen p(wxColor(r,g,b));
        dc.SetPen(p);

        if (direction == wxAUI_GRADIENT_VERTICAL)
            dc.DrawLine(rect.x, rect.y+i, rect.x+rect.width, rect.y+i);
             else
            dc.DrawLine(rect.x+i, rect.y, rect.x+i, rect.y+rect.height);
    }

}

wxDefaultDockArt::wxDefaultDockArt()
{
#ifdef __WXMAC__
    wxBrush toolbarbrush;
    toolbarbrush.MacSetTheme( kThemeBrushToolbarBackground );
    wxColor base_color = toolbarbrush.GetColour();
#else
    wxColor base_color = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
#endif
    
    wxColor darker1_color = StepColour(base_color, 85);
    wxColor darker2_color = StepColour(base_color, 70);
    wxColor darker3_color = StepColour(base_color, 60);
    wxColor darker4_color = StepColour(base_color, 50);
    wxColor darker5_color = StepColour(base_color, 40);

    m_ActiveCaptionColour = LightContrastColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
    m_ActiveCaptionGradientColour = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
    m_ActiveCaptionTextColour = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT);
    m_InactiveCaptionColour = StepColour(darker1_color, 80);
    m_InactiveCaptionGradientColour = darker1_color;
    m_InactiveCaptionTextColour = *wxBLACK;

#ifdef __WXMAC__
    m_SashBrush = toolbarbrush;
    m_BackgroundBrush = toolbarbrush;
    m_GripperBrush = toolbarbrush;
#else
    m_SashBrush = wxBrush(base_color);
    m_BackgroundBrush = wxBrush(base_color);
    m_GripperBrush = wxBrush(base_color);
#endif
    m_BorderPen = wxPen(darker2_color);
    m_GripperPen1 = wxPen(darker5_color);
    m_GripperPen2 = wxPen(darker3_color);
    m_GripperPen3 = *wxWHITE_PEN;

#ifdef __WXMAC__
    m_CaptionFont = *wxSMALL_FONT;
#else
    m_CaptionFont = wxFont(8, wxDEFAULT, wxNORMAL, wxNORMAL, FALSE);
#endif

    // some built in bitmaps
#ifdef __WXMAC__
     static unsigned char close_bits[]={
         0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0xFE, 0x03, 0xF8, 0x01, 0xF0, 0x19, 0xF3,
         0xB8, 0xE3, 0xF0, 0xE1, 0xE0, 0xE0, 0xF0, 0xE1, 0xB8, 0xE3, 0x19, 0xF3,
         0x01, 0xF0, 0x03, 0xF8, 0x0F, 0xFE, 0xFF, 0xFF };
#else
    static unsigned char close_bits[]={
        0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xef,0xfb,0xcf,0xf9,
        0x9f,0xfc,0x3f,0xfe,0x3f,0xfe,0x9f,0xfc,0xcf,0xf9,0xef,0xfb,
        0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
#endif

    static unsigned char pin_bits[]={
        0xff,0xff,0xff,0xff,0xff,0xff,0x1f,0xfc,0xdf,0xfc,0xdf,0xfc,
        0xdf,0xfc,0xdf,0xfc,0xdf,0xfc,0x0f,0xf8,0x7f,0xff,0x7f,0xff,
        0x7f,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
          
#ifdef __WXMAC__
    m_InactiveCloseBitmap = BitmapFromBits(close_bits, 16, 16, *wxWHITE);
#else
    m_InactiveCloseBitmap = BitmapFromBits(close_bits, 16, 16, m_InactiveCaptionTextColour);
#endif
    m_InactivePinBitmap = BitmapFromBits(pin_bits, 16, 16, m_InactiveCaptionTextColour);
#ifdef __WXMAC__
    m_ActiveCloseBitmap = BitmapFromBits(close_bits, 16, 16, *wxWHITE );
#else
    m_ActiveCloseBitmap = BitmapFromBits(close_bits, 16, 16, m_ActiveCaptionTextColour);
#endif
    m_ActivePinBitmap = BitmapFromBits(pin_bits, 16, 16, m_ActiveCaptionTextColour);

    // default metric values
#ifdef __WXMAC__
    SInt32 height;
    GetThemeMetric( kThemeMetricSmallPaneSplitterHeight , &height );
    m_SashSize = height;
#else
    m_SashSize = 4;
#endif
    m_CaptionSize = 17;
    m_BorderSize = 1;
    m_ButtonSize = 14;
    m_GripperSize = 9;
    m_GradientType = wxAUI_GRADIENT_VERTICAL;
}

int wxDefaultDockArt::GetMetric(int id)
{
    switch (id)
    {
        case wxAUI_ART_SASH_SIZE:          return m_SashSize;
        case wxAUI_ART_CAPTION_SIZE:       return m_CaptionSize;
        case wxAUI_ART_GRIPPER_SIZE:       return m_GripperSize;
        case wxAUI_ART_PANE_BORDER_SIZE:   return m_BorderSize;
        case wxAUI_ART_PANE_BUTTON_SIZE:   return m_ButtonSize;
        case wxAUI_ART_GRADIENT_TYPE:      return m_GradientType;
        default: wxFAIL_MSG(wxT("Invalid Metric Ordinal")); break;
    }

    return 0;
}

void wxDefaultDockArt::SetMetric(int id, int new_val)
{
    switch (id)
    {
        case wxAUI_ART_SASH_SIZE:          m_SashSize = new_val; break;
        case wxAUI_ART_CAPTION_SIZE:       m_CaptionSize = new_val; break;
        case wxAUI_ART_GRIPPER_SIZE:       m_GripperSize = new_val; break;
        case wxAUI_ART_PANE_BORDER_SIZE:   m_BorderSize = new_val; break;
        case wxAUI_ART_PANE_BUTTON_SIZE:   m_ButtonSize = new_val; break;
        case wxAUI_ART_GRADIENT_TYPE:      m_GradientType = new_val; break;
        default: wxFAIL_MSG(wxT("Invalid Metric Ordinal")); break;
    }
}

wxColour wxDefaultDockArt::GetColour(int id)
{
    switch (id)
    {
        case wxAUI_ART_BACKGROUND_COLOUR:                return m_BackgroundBrush.GetColour(); break;
        case wxAUI_ART_SASH_COLOUR:                      return m_SashBrush.GetColour(); break;
        case wxAUI_ART_INACTIVE_CAPTION_COLOUR:          return m_InactiveCaptionColour; break;
        case wxAUI_ART_INACTIVE_CAPTION_GRADIENT_COLOUR: return m_InactiveCaptionGradientColour; break;
        case wxAUI_ART_INACTIVE_CAPTION_TEXT_COLOUR:     return m_InactiveCaptionTextColour; break;
        case wxAUI_ART_ACTIVE_CAPTION_COLOUR:            return m_ActiveCaptionColour; break;
        case wxAUI_ART_ACTIVE_CAPTION_GRADIENT_COLOUR:   return m_ActiveCaptionGradientColour; break;
        case wxAUI_ART_ACTIVE_CAPTION_TEXT_COLOUR:       return m_ActiveCaptionTextColour; break;
        case wxAUI_ART_BORDER_COLOUR:                    return m_BorderPen.GetColour(); break;
        case wxAUI_ART_GRIPPER_COLOUR:                   return m_GripperBrush.GetColour(); break;
        default: wxFAIL_MSG(wxT("Invalid Metric Ordinal")); break;
    }

    return wxColour();
}

void wxDefaultDockArt::SetColour(int id, const wxColor& colour)
{
    switch (id)
    {
        case wxAUI_ART_BACKGROUND_COLOUR:                m_BackgroundBrush.SetColour(colour); break;
        case wxAUI_ART_SASH_COLOUR:                      m_SashBrush.SetColour(colour); break;
        case wxAUI_ART_INACTIVE_CAPTION_COLOUR:          m_InactiveCaptionColour = colour; break;
        case wxAUI_ART_INACTIVE_CAPTION_GRADIENT_COLOUR: m_InactiveCaptionGradientColour = colour; break;
        case wxAUI_ART_INACTIVE_CAPTION_TEXT_COLOUR:     m_InactiveCaptionTextColour = colour; break;
        case wxAUI_ART_ACTIVE_CAPTION_COLOUR:            m_ActiveCaptionColour = colour; break;
        case wxAUI_ART_ACTIVE_CAPTION_GRADIENT_COLOUR:   m_ActiveCaptionGradientColour = colour; break;
        case wxAUI_ART_ACTIVE_CAPTION_TEXT_COLOUR:       m_ActiveCaptionTextColour = colour; break;
        case wxAUI_ART_BORDER_COLOUR:                    m_BorderPen.SetColour(colour); break;
        case wxAUI_ART_GRIPPER_COLOUR:
            m_GripperBrush.SetColour(colour);
            m_GripperPen1.SetColour(StepColour(colour, 40));
            m_GripperPen2.SetColour(StepColour(colour, 60));
            break;
        default: wxFAIL_MSG(wxT("Invalid Metric Ordinal")); break;
    }
}

void wxDefaultDockArt::SetFont(int id, const wxFont& font)
{
    if (id == wxAUI_ART_CAPTION_FONT)
        m_CaptionFont = font;
}

wxFont wxDefaultDockArt::GetFont(int id)
{
    if (id == wxAUI_ART_CAPTION_FONT)
        return m_CaptionFont;
    return wxNullFont;
}

void wxDefaultDockArt::DrawSash(wxDC& dc, int, const wxRect& rect)
{
#ifdef __WXMAC__
    HIRect splitterRect = CGRectMake( rect.x , rect.y , rect.width , rect.height );
    CGContextRef cgContext ;
#if wxMAC_USE_CORE_GRAPHICS
    cgContext = ((wxMacCGContext*)(dc.GetGraphicContext()))->GetNativeContext() ;
#else
    Rect bounds ;
    GetPortBounds( (CGrafPtr) dc.m_macPort , &bounds ) ;
    QDBeginCGContext( (CGrafPtr) dc.m_macPort , &cgContext ) ;
    CGContextTranslateCTM( cgContext , 0 , bounds.bottom - bounds.top ) ;
    CGContextScaleCTM( cgContext , 1 , -1 ) ;
#endif
    
    HIThemeSplitterDrawInfo drawInfo ;
    drawInfo.version = 0 ;
    drawInfo.state = kThemeStateActive ;
    drawInfo.adornment = kHIThemeSplitterAdornmentNone ;
    HIThemeDrawPaneSplitter( &splitterRect , &drawInfo , cgContext , kHIThemeOrientationNormal ) ;    
    
#if wxMAC_USE_CORE_GRAPHICS
#else
    QDEndCGContext( (CGrafPtr) dc.m_macPort , &cgContext ) ;
#endif

#else
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(m_SashBrush);
    dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);
#endif
}


void wxDefaultDockArt::DrawBackground(wxDC& dc, int, const wxRect& rect)
{
    dc.SetPen(*wxTRANSPARENT_PEN);
#ifdef __WXMAC__
    // we have to clear first, otherwise we are drawing a light striped pattern
    // over an already darker striped background
    dc.SetBrush(*wxWHITE_BRUSH) ;
    dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);
#endif
    dc.SetBrush(m_BackgroundBrush);
    dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);
}

void wxDefaultDockArt::DrawBorder(wxDC& dc, const wxRect& _rect,
                                  wxPaneInfo& pane)
{
    dc.SetPen(m_BorderPen);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);

    wxRect rect = _rect;
    int i, border_width = GetMetric(wxAUI_ART_PANE_BORDER_SIZE);

    if (pane.IsToolbar())
    {
        for (i = 0; i < border_width; ++i)
        {
            dc.SetPen(*wxWHITE_PEN);
            dc.DrawLine(rect.x, rect.y, rect.x+rect.width, rect.y);
            dc.DrawLine(rect.x, rect.y, rect.x, rect.y+rect.height);
            dc.SetPen(m_BorderPen);       
            dc.DrawLine(rect.x, rect.y+rect.height-1,
                        rect.x+rect.width, rect.y+rect.height-1);
            dc.DrawLine(rect.x+rect.width-1, rect.y,
                        rect.x+rect.width-1, rect.y+rect.height);
            rect.Deflate(1);
        }
    }
     else
    {
        for (i = 0; i < border_width; ++i)
        {
            dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);
            rect.Deflate(1);
        }
    }
}


void wxDefaultDockArt::DrawCaptionBackground(wxDC& dc, const wxRect& rect, bool active)
{
    if (m_GradientType == wxAUI_GRADIENT_NONE)
    {
        if (active)
            dc.SetBrush(wxBrush(m_ActiveCaptionColour));
             else
            dc.SetBrush(wxBrush(m_InactiveCaptionColour));

        dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);
    }
     else
    {
        if (active)
        {
            // on mac the gradients are expected to become darker from the top
#ifdef __WXMAC__
            DrawGradientRectangle(dc, rect,
                                 m_ActiveCaptionGradientColour,
                                 m_ActiveCaptionColour,
                                 m_GradientType);
#else
            DrawGradientRectangle(dc, rect,
                                 m_ActiveCaptionColour,
                                 m_ActiveCaptionGradientColour,
                                 m_GradientType);
#endif
        }
         else
        {
            // on mac the gradients are expected to become darker from the top
#ifdef __WXMAC__
            DrawGradientRectangle(dc, rect,
                                 m_InactiveCaptionGradientColour,
                                 m_InactiveCaptionColour,
                                 m_GradientType);
#else
            DrawGradientRectangle(dc, rect,
                                 m_InactiveCaptionColour,
                                 m_InactiveCaptionGradientColour,
                                 m_GradientType);
#endif
        }
    }
}


void wxDefaultDockArt::DrawCaption(wxDC& dc,
                                   const wxString& text,
                                   const wxRect& rect,
                                   wxPaneInfo& pane)
{
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetFont(m_CaptionFont);

    DrawCaptionBackground(dc, rect,
                          (pane.m_State & wxPaneInfo::optionActive)?true:false);

    if (pane.m_State & wxPaneInfo::optionActive)
        dc.SetTextForeground(m_ActiveCaptionTextColour);
     else
        dc.SetTextForeground(m_InactiveCaptionTextColour);


    wxCoord w,h;
    dc.GetTextExtent(wxT("ABCDEFHXfgkj"), &w, &h);

    dc.SetClippingRegion(rect);
    dc.DrawText(text, rect.x+3, rect.y+(rect.height/2)-(h/2)-1);
    dc.DestroyClippingRegion();
}

void wxDefaultDockArt::DrawGripper(wxDC& dc,
                                   const wxRect& rect,
                                   wxPaneInfo& pane)
{
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(m_GripperBrush);

    dc.DrawRectangle(rect.x, rect.y, rect.width,rect.height);

    int y = 5;
    while (1)
    {
        dc.SetPen(m_GripperPen1);
        dc.DrawPoint(rect.x+3, rect.y+y);
        dc.SetPen(m_GripperPen2);
        dc.DrawPoint(rect.x+3, rect.y+y+1);
        dc.DrawPoint(rect.x+4, rect.y+y);        
        dc.SetPen(m_GripperPen3);
        dc.DrawPoint(rect.x+5, rect.y+y+1);
        dc.DrawPoint(rect.x+5, rect.y+y+2);        
        dc.DrawPoint(rect.x+4, rect.y+y+2);        
        
        y += 4;
        if (y > rect.GetHeight()-5)
            break;
    }
}

void wxDefaultDockArt::DrawPaneButton(wxDC& dc,
                                      int button,
                                      int button_state,
                                      const wxRect& _rect,
                                      wxPaneInfo& pane)
{
    wxRect rect = _rect;

    if (button_state == wxAUI_BUTTON_STATE_PRESSED)
    {
        rect.x++;
        rect.y++;
    }

    if (button_state == wxAUI_BUTTON_STATE_HOVER ||
        button_state == wxAUI_BUTTON_STATE_PRESSED)
    {
        if (pane.m_State & wxPaneInfo::optionActive)
        {
            dc.SetBrush(wxBrush(StepColour(m_ActiveCaptionColour, 120)));
            dc.SetPen(wxPen(StepColour(m_ActiveCaptionColour, 70)));
        }
         else
        {
            dc.SetBrush(wxBrush(StepColour(m_InactiveCaptionColour, 120)));
            dc.SetPen(wxPen(StepColour(m_InactiveCaptionColour, 70)));
        }

        // draw the background behind the button
        dc.DrawRectangle(rect.x, rect.y, 15, 15);
    }

    wxBitmap bmp;
    switch (button)
    {
        default:
        case wxPaneInfo::buttonClose:
            if (pane.m_State & wxPaneInfo::optionActive)
                bmp = m_ActiveCloseBitmap;
                 else
                bmp = m_InactiveCloseBitmap;
            break;
        case wxPaneInfo::buttonPin:
            if (pane.m_State & wxPaneInfo::optionActive)
                bmp = m_ActivePinBitmap;
                 else
                bmp = m_InactivePinBitmap;
            break;
    }

    // draw the button itself
    dc.DrawBitmap(bmp, rect.x, rect.y, true);
}


// -- wxFloatingPane class implementation --


// wxFloatingPane implements a frame class with some special functionality
// which allows the library to sense when the frame move starts, is active,
// and completes.  Note that it contains it's own wxFrameManager instance,
// which, in the future, would allow for nested managed frames.
// For now, with wxMSW, the wxMiniFrame window is used, but on wxGTK, wxFrame

#if defined( __WXMSW__ ) || defined( __WXMAC__ )
#define wxFloatingPaneBaseClass wxMiniFrame
#else
#define wxFloatingPaneBaseClass wxFrame
#endif

#ifdef __WXGTK__
extern "C" { void* gdk_window_get_pointer(void*, int*, int*, unsigned int*); }
#endif

class wxFloatingPane : public wxFloatingPaneBaseClass
{
public:
    wxFloatingPane(wxWindow* parent,
                   wxFrameManager* owner_mgr,
                   wxWindowID id = -1,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize)
                    : wxFloatingPaneBaseClass(parent, id, wxT(""), pos, size,
                            wxRESIZE_BORDER | wxSYSTEM_MENU | wxCAPTION |
                            wxCLOSE_BOX | wxFRAME_NO_TASKBAR |
                            wxFRAME_FLOAT_ON_PARENT | wxCLIP_CHILDREN)
    {
        m_OwnerMgr = owner_mgr;
        m_Moving = false;
        m_LastRect = wxRect();
        m_Mgr.SetFrame(this);
        SetExtraStyle(wxWS_EX_PROCESS_IDLE);
    }

    ~wxFloatingPane()
    {
        m_Mgr.UnInit();
    }

    void SetPaneWindow(const wxPaneInfo& pane)
    {
        m_PaneWindow = pane.m_Window;
        m_PaneWindow->Reparent(this);
        
        wxPaneInfo contained_pane = pane;
        contained_pane.Dock().Center().Show().
                       CaptionVisible(false).
                       PaneBorder(false).
                       Layer(0).Row(0).Position(0);
                                              
        m_Mgr.AddPane(m_PaneWindow, contained_pane);
        m_Mgr.Update();           

        if (pane.m_MinSize.IsFullySpecified())
        {
            // because SetSizeHints() calls Fit() too (which sets the window
            // size to its minimum allowed), we keep the size before calling
            // SetSizeHints() and reset it afterwards...
            wxSize tmp = GetSize();
            GetSizer()->SetSizeHints(this);
            SetSize(tmp);
        }
        
        SetTitle(pane.m_Caption);

        if (contained_pane.IsFixed())
            SetWindowStyle(GetWindowStyle() & ~wxRESIZE_BORDER);

        if (pane.m_FloatingSize != wxDefaultSize)
        {
            SetSize(pane.m_FloatingSize);
        }
         else
        {
            wxSize size = pane.m_BestSize;
            if (size == wxDefaultSize)
                size = pane.m_MinSize;
            if (size == wxDefaultSize)
                size = m_PaneWindow->GetSize();
            if (pane.HasGripper())
                size.x += m_OwnerMgr->m_Art->GetMetric(wxAUI_ART_GRIPPER_SIZE);
                
            SetClientSize(size);
        }
    }

private:

    void OnSize(wxSizeEvent& event)
    {
        m_OwnerMgr->OnFloatingPaneResized(m_PaneWindow, event.GetSize());
    }
    
    void OnClose(wxCloseEvent& event)
    {
        m_OwnerMgr->OnFloatingPaneClosed(m_PaneWindow);
        Destroy();
    }

    void OnMoveEvent(wxMoveEvent& event)
    {
        wxRect win_rect = GetRect();

        // skip the first move event
        if (m_LastRect.IsEmpty())
        {
            m_LastRect = win_rect;
            return;
        }

        // prevent frame redocking during resize
        if (m_LastRect.GetSize() != win_rect.GetSize())
        {
            m_LastRect = win_rect;
            return;
        }

        m_LastRect = win_rect;
        
        if (!isMouseDown())
            return;

        if (!m_Moving)
        {
            OnMoveStart();
            m_Moving = true;
        }

        OnMoving(event.GetRect());
    }

    void OnIdle(wxIdleEvent& event)
    {
        if (m_Moving)
        {
            if (!isMouseDown())
            {
                m_Moving = false;
                OnMoveFinished();
            }
             else
            {
                event.RequestMore();
            }
        }
    }

    void OnMoveStart()
    {
        // notify the owner manager that the pane has started to move
        m_OwnerMgr->OnFloatingPaneMoveStart(m_PaneWindow);
    }

    void OnMoving(const wxRect& window_rect)
    {
        // notify the owner manager that the pane is moving
        m_OwnerMgr->OnFloatingPaneMoving(m_PaneWindow);
    }

    void OnMoveFinished()
    {
        // notify the owner manager that the pane has finished moving
        m_OwnerMgr->OnFloatingPaneMoved(m_PaneWindow);
    }

    void OnActivate(wxActivateEvent& event)
    {
        if (event.GetActive())
        {
            m_OwnerMgr->OnFloatingPaneActivated(m_PaneWindow);
        }
    }

    // utility function which determines the state of the mouse button
    // (independant of having a wxMouseEvent handy) - utimately a better
    // mechanism for this should be found (possibly by adding the
    // functionality to wxWidgets itself)
    static bool isMouseDown()
    {
        #ifdef __WXMSW__
        return (GetKeyState( VK_LBUTTON ) & (1<<15)) ? true : false;
        #endif

        #ifdef __WXGTK__
        int x, y;
        unsigned int m;
        gdk_window_get_pointer(NULL, &x, &y, &m);
        return (m & 0x100) ? true : false;
        //return (m & 0x1F00) ? true : false;
        #endif
        
        #ifdef __WXMAC__
        return GetCurrentEventButtonState() & 0x01 ;
        #endif
    }

private:
    wxWindow* m_PaneWindow;    // pane window being managed
    bool m_Moving;
    wxRect m_LastRect;
    wxSize m_LastSize;

    wxFrameManager* m_OwnerMgr;
    wxFrameManager m_Mgr;

    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxFloatingPane, wxFloatingPaneBaseClass)
    EVT_SIZE(wxFloatingPane::OnSize)
    EVT_MOVE(wxFloatingPane::OnMoveEvent)
    EVT_MOVING(wxFloatingPane::OnMoveEvent)
    EVT_CLOSE(wxFloatingPane::OnClose)
    EVT_IDLE(wxFloatingPane::OnIdle)
    EVT_ACTIVATE(wxFloatingPane::OnActivate)
END_EVENT_TABLE()





// -- static utility functions --

static wxBitmap wxPaneCreateStippleBitmap()
{
    unsigned char data[] = { 0,0,0,192,192,192, 192,192,192,0,0,0 };
    wxImage img(2,2,data,true);
    return wxBitmap(img);
}

static void DrawResizeHint(wxDC& dc, const wxRect& rect)
{
    wxBitmap stipple = wxPaneCreateStippleBitmap();
    wxBrush brush(stipple);
    dc.SetBrush(brush);    
    dc.SetPen(*wxTRANSPARENT_PEN);

    dc.SetLogicalFunction(wxXOR);
    dc.DrawRectangle(rect);
}

#ifdef __WXMSW__

// on supported windows systems (Win2000 and greater), this function
// will make a frame window transparent by a certain amount
static void MakeWindowTransparent(wxWindow* wnd, int amount)
{
    // this API call is not in all SDKs, only the newer ones, so
    // we will runtime bind this
    typedef DWORD (WINAPI *PSETLAYEREDWINDOWATTR)(HWND, DWORD, BYTE, DWORD);
    static PSETLAYEREDWINDOWATTR pSetLayeredWindowAttributes = NULL;
    static HMODULE h = NULL;
    HWND hwnd = (HWND)wnd->GetHWND();
    
    if (!h)
        h = LoadLibrary(_T("user32"));
        
    if (!pSetLayeredWindowAttributes)
    {
        pSetLayeredWindowAttributes =
         (PSETLAYEREDWINDOWATTR)GetProcAddress(h,"SetLayeredWindowAttributes");
    }
    
    if (pSetLayeredWindowAttributes == NULL)
        return;
        
    LONG exstyle = GetWindowLong(hwnd, GWL_EXSTYLE);
    if (0 == (exstyle & 0x80000) /*WS_EX_LAYERED*/)
        SetWindowLong(hwnd, GWL_EXSTYLE, exstyle | 0x80000 /*WS_EX_LAYERED*/);   
             
    pSetLayeredWindowAttributes(hwnd, 0, amount, 2 /*LWA_ALPHA*/);
}

#endif


// CopyDocksAndPanes() - this utility function creates copies of
// the dock and pane info.  wxDockInfo's usually contain pointers
// to wxPaneInfo classes, thus this function is necessary to reliably
// reconstruct that relationship in the new dock info and pane info arrays

static void CopyDocksAndPanes(wxDockInfoArray& dest_docks,
                              wxPaneInfoArray& dest_panes,
                              const wxDockInfoArray& src_docks,
                              const wxPaneInfoArray& src_panes)
{
    dest_docks = src_docks;
    dest_panes = src_panes;
    int i, j, k, dock_count, pc1, pc2;
    for (i = 0, dock_count = dest_docks.GetCount(); i < dock_count; ++i)
    {
        wxDockInfo& dock = dest_docks.Item(i);
        for (j = 0, pc1 = dock.m_Panes.GetCount(); j < pc1; ++j)
            for (k = 0, pc2 = src_panes.GetCount(); k < pc2; ++k)
                if (dock.m_Panes.Item(j) == &src_panes.Item(k))
                    dock.m_Panes.Item(j) = &dest_panes.Item(k);
    }
}

// GetMaxLayer() is an internal function which returns
// the highest layer inside the specified dock
static int GetMaxLayer(const wxDockInfoArray& docks, int dock_direction)
{
    int i, dock_count, max_layer = 0;
    for (i = 0, dock_count = docks.GetCount(); i < dock_count; ++i)
    {
        wxDockInfo& dock = docks.Item(i);
        if (dock.m_DockDirection == dock_direction &&
            dock.m_DockLayer > max_layer && !dock.m_Fixed)
                max_layer = dock.m_DockLayer;
    }
    return max_layer;
}


// GetMaxRow() is an internal function which returns
// the highest layer inside the specified dock
static int GetMaxRow(const wxPaneInfoArray& panes, int direction, int layer)
{
    int i, pane_count, max_row = 0;
    for (i = 0, pane_count = panes.GetCount(); i < pane_count; ++i)
    {
        wxPaneInfo& pane = panes.Item(i);
        if (pane.m_DockDirection == direction &&
            pane.m_DockLayer == layer && 
            pane.m_DockRow > max_row)
                max_row = pane.m_DockRow;
    }
    return max_row;
}



// DoInsertDockLayer() is an internal function that inserts a new dock
// layer by incrementing all existing dock layer values by one
static void DoInsertDockLayer(wxPaneInfoArray& panes,
                              int dock_direction,
                              int dock_layer)
{
    int i, pane_count;
    for (i = 0, pane_count = panes.GetCount(); i < pane_count; ++i)
    {
        wxPaneInfo& pane = panes.Item(i);
        if (!pane.IsFloating() &&
            pane.m_DockDirection == dock_direction &&
            pane.m_DockLayer >= dock_layer)
                pane.m_DockLayer++;
    }
}

// DoInsertDockLayer() is an internal function that inserts a new dock
// row by incrementing all existing dock row values by one
static void DoInsertDockRow(wxPaneInfoArray& panes,
                            int dock_direction,
                            int dock_layer,
                            int dock_row)
{
    int i, pane_count;
    for (i = 0, pane_count = panes.GetCount(); i < pane_count; ++i)
    {
        wxPaneInfo& pane = panes.Item(i);
        if (!pane.IsFloating() &&
            pane.m_DockDirection == dock_direction &&
            pane.m_DockLayer == dock_layer &&
            pane.m_DockRow >= dock_row)
                pane.m_DockRow++;
    }
}

// DoInsertDockLayer() is an internal function that inserts a space for 
// another dock pane by incrementing all existing dock row values by one
static void DoInsertPane(wxPaneInfoArray& panes,
                         int dock_direction,
                         int dock_layer,
                         int dock_row,
                         int dock_pos)
{
    int i, pane_count;
    for (i = 0, pane_count = panes.GetCount(); i < pane_count; ++i)
    {
        wxPaneInfo& pane = panes.Item(i);
        if (!pane.IsFloating() &&
            pane.m_DockDirection == dock_direction &&
            pane.m_DockLayer == dock_layer &&
            pane.m_DockRow == dock_row &&
            pane.m_DockPos >= dock_pos)
                pane.m_DockPos++;
    }
}

// FindDocks() is an internal function that returns a list of docks which meet
// the specified conditions in the parameters and returns a sorted array
// (sorted by layer and then row)
static void FindDocks(wxDockInfoArray& docks,
                      int dock_direction,
                      int dock_layer,
                      int dock_row,
                      wxDockInfoPtrArray& arr)
{
    int begin_layer = dock_layer;
    int end_layer = dock_layer;
    int begin_row = dock_row;
    int end_row = dock_row;
    int dock_count = docks.GetCount();
    int layer, row, i, max_row = 0, max_layer = 0;

    // discover the maximum dock layer and the max row
    for (i = 0; i < dock_count; ++i)
    {
        max_row = wxMax(max_row, docks.Item(i).m_DockRow);
        max_layer = wxMax(max_layer, docks.Item(i).m_DockLayer);
    }
    
    // if no dock layer was specified, search all dock layers
    if (dock_layer == -1)
    {
        begin_layer = 0;
        end_layer = max_layer;
    }
    
    // if no dock row was specified, search all dock row
    if (dock_row == -1)
    {
        begin_row = 0;
        end_row = max_row;
    }

    arr.Clear();

    for (layer = begin_layer; layer <= end_layer; ++layer)
        for (row = begin_row; row <= end_row; ++row)
            for (i = 0; i < dock_count; ++i)
            {
                wxDockInfo& d = docks.Item(i);
                if (dock_direction == -1 || dock_direction == d.m_DockDirection)
                {
                    if (d.m_DockLayer == layer && d.m_DockRow == row)
                        arr.Add(&d);
                }
            }
}

// FindPaneInDock() looks up a specified window pointer inside a dock.
// If found, the corresponding wxPaneInfo pointer is returned, otherwise NULL.
static wxPaneInfo* FindPaneInDock(const wxDockInfo& dock, wxWindow* window)
{
    int i, count = dock.m_Panes.GetCount();
    for (i = 0; i < count; ++i)
    {
        wxPaneInfo* p = dock.m_Panes.Item(i);
        if (p->m_Window == window)
            return p;
    }
    return NULL;
}

// RemovePaneFromDocks() removes a pane window from all docks
// with a possible exception specified by parameter "except"
static void RemovePaneFromDocks(wxDockInfoArray& docks,
                                wxPaneInfo& pane,
                                wxDockInfo* except = NULL)
{
    int i, dock_count;
    for (i = 0, dock_count = docks.GetCount(); i < dock_count; ++i)
    {
        wxDockInfo& d = docks.Item(i);
        if (&d == except)
            continue;
        wxPaneInfo* pi = FindPaneInDock(d, pane.m_Window);
        if (pi)
            d.m_Panes.Remove(pi);
    }
}

// RenumberDockRows() takes a dock and assigns sequential numbers
// to existing rows.  Basically it takes out the gaps; so if a
// dock has rows with numbers 0,2,5, they will become 0,1,2
static void RenumberDockRows(wxDockInfoPtrArray& docks)
{
    int i, dock_count, j, pane_count;
    for (i = 0, dock_count = docks.GetCount(); i < dock_count; ++i)
    {
        wxDockInfo& dock = *docks.Item(i);
        dock.m_DockRow = i;
        for (j = 0, pane_count = dock.m_Panes.GetCount(); j < pane_count; ++j)
            dock.m_Panes.Item(j)->m_DockRow = i;
    }
}



static void SetActivePane(wxPaneInfoArray& panes, wxWindow* active_pane)
{
    int i, pane_count;
    for (i = 0, pane_count = panes.GetCount(); i < pane_count; ++i)
    {
        wxPaneInfo& pane = panes.Item(i);
        pane.m_State &= ~wxPaneInfo::optionActive;
        if (pane.m_Window == active_pane)
            pane.m_State |= wxPaneInfo::optionActive;
    }
}


// this function is used to sort panes by dock position
static int PaneSortFunc(wxPaneInfo** p1, wxPaneInfo** p2)
{
    return ((*p1)->m_DockPos < (*p2)->m_DockPos) ? -1 : 1;
}


// -- wxFrameManager class implementation --


BEGIN_EVENT_TABLE(wxFrameManager, wxEvtHandler)
    EVT_AUI_PANEBUTTON(wxFrameManager::OnPaneButton)
    EVT_PAINT(wxFrameManager::OnPaint)
    EVT_ERASE_BACKGROUND(wxFrameManager::OnEraseBackground)
    EVT_SIZE(wxFrameManager::OnSize)
    EVT_SET_CURSOR(wxFrameManager::OnSetCursor)
    EVT_LEFT_DOWN(wxFrameManager::OnLeftDown)
    EVT_LEFT_UP(wxFrameManager::OnLeftUp)
    EVT_MOTION(wxFrameManager::OnMotion)
    EVT_LEAVE_WINDOW(wxFrameManager::OnLeaveWindow)
    EVT_CHILD_FOCUS(wxFrameManager::OnChildFocus)
    EVT_TIMER(101, wxFrameManager::OnHintFadeTimer)
END_EVENT_TABLE()


wxFrameManager::wxFrameManager(wxFrame* frame, unsigned int flags)
{
    m_Action = actionNone;
    m_LastMouseMove = wxPoint();
    m_HoverButton = NULL;
    m_Art = new wxDefaultDockArt;
    m_HintWnd = NULL;
    m_Flags = flags;

    if (frame)
    {
        SetFrame(frame);
    }
}

wxFrameManager::~wxFrameManager()
{
    delete m_Art;
}

// GetPane() looks up a wxPaneInfo structure based
// on the supplied window pointer.  Upon failure, GetPane()
// returns an empty wxPaneInfo, a condition which can be checked
// by calling wxPaneInfo::IsOk().
//
// The pane info's structure may then be modified.  Once a pane's
// info is modified, wxFrameManager::Update() must be called to
// realize the changes in the UI.

wxPaneInfo& wxFrameManager::GetPane(wxWindow* window)
{
    int i, pane_count;
    for (i = 0, pane_count = m_PanesArray.GetCount(); i < pane_count; ++i)
    {
        wxPaneInfo& p = m_PanesArray.Item(i);
        if (p.m_Window == window)
            return p;
    }
    return wxNullPaneInfo;
}

// this version of GetPane() looks up a pane based on a
// 'pane name', see above comment for more info
wxPaneInfo& wxFrameManager::GetPane(const wxString& name)
{
    int i, pane_count;
    for (i = 0, pane_count = m_PanesArray.GetCount(); i < pane_count; ++i)
    {
        wxPaneInfo& p = m_PanesArray.Item(i);
        if (p.m_Name == name)
            return p;
    }
    return wxNullPaneInfo;
}

// GetAllPanes() returns a reference to all the pane info structures
wxPaneInfoArray& wxFrameManager::GetAllPanes()
{
    return m_PanesArray;
}

// HitTest() is an internal function which determines
// which UI item the specified coordinates are over
// (x,y) specify a position in client coordinates
wxDockUIPart* wxFrameManager::HitTest(int x, int y)
{
    wxDockUIPart* result = NULL;

    int i, part_count;
    for (i = 0, part_count = m_UIParts.GetCount(); i < part_count; ++i)
    {
        wxDockUIPart* item = &m_UIParts.Item(i);
        
        // we are not interested in typeDock, because this space 
        // isn't used to draw anything, just for measurements;
        // besides, the entire dock area is covered with other
        // rectangles, which we are interested in.
        if (item->m_Type == wxDockUIPart::typeDock)
            continue;
        
        // if we already have a hit on a more specific item, we are not
        // interested in a pane hit.  If, however, we don't already have
        // a hit, returning a pane hit is necessary for some operations
        if ((item->m_Type == wxDockUIPart::typePane ||
            item->m_Type == wxDockUIPart::typePaneBorder) && result)
            continue;
           
        // if the point is inside the rectangle, we have a hit
        if (item->m_Rect.Contains(x,y))
            result = item;
    }
    
    return result;
}


// SetFlags() and GetFlags() allow the owner to set various
// options which are global to wxFrameManager
void wxFrameManager::SetFlags(unsigned int flags)
{
    m_Flags = flags;
}

unsigned int wxFrameManager::GetFlags() const
{
    return m_Flags;
}


// SetFrame() is usually called once when the frame
// manager class is being initialized.  "frame" specifies
// the frame which should be managed by the frame mananger
void wxFrameManager::SetFrame(wxFrame* frame)
{
    wxASSERT_MSG(frame, wxT("specified frame must be non-NULL"));

    m_Frame = frame;
    m_Frame->PushEventHandler(this);

#if wxUSE_MDI
    // if the owner is going to manage an MDI parent frame,
    // we need to add the MDI client window as the default
    // center pane

    if (frame->IsKindOf(CLASSINFO(wxMDIParentFrame)))
    {
        wxMDIParentFrame* mdi_frame = (wxMDIParentFrame*)frame;
				wxMDIClientWindow* client_window = dynamic_cast<wxMDIClientWindow*>(mdi_frame->GetClientWindow());

        wxASSERT_MSG(client_window, wxT("Client window is NULL!"));

        AddPane(client_window,
                wxPaneInfo().Name(wxT("mdiclient")).
                CenterPane().PaneBorder(false));
    }
#endif
}


// UnInit() must be called, usually in the destructor
// of the frame class.   If it is not called, usually this
// will result in a crash upon program exit
void wxFrameManager::UnInit()
{
    m_Frame->RemoveEventHandler(this);
}

// GetFrame() returns the frame pointer being managed by wxFrameManager
wxFrame* wxFrameManager::GetFrame() const
{
    return m_Frame;
}

wxDockArt* wxFrameManager::GetArtProvider() const
{
    return m_Art;
}

void wxFrameManager::ProcessMgrEvent(wxFrameManagerEvent& event)
{
    // first, give the owner frame a chance to override
    if (m_Frame)
    {
        if (m_Frame->ProcessWindowEvent(event))
            return;
    }

    ProcessEvent(event);
}

// SetArtProvider() instructs wxFrameManager to use the
// specified art provider for all drawing calls.  This allows
// plugable look-and-feel features
void wxFrameManager::SetArtProvider(wxDockArt* art_provider)
{
    // delete the last art provider, if any
    delete m_Art;
    
    // assign the new art provider
    m_Art = art_provider;
}

bool wxFrameManager::AddPane(wxWindow* window, const wxPaneInfo& pane_info)
{
    // check if the pane has a valid window
    if (!window)
        return false;

    // check if the pane already exists
    if (GetPane(pane_info.m_Window).IsOk())
        return false;

    m_PanesArray.Add(pane_info);

    wxPaneInfo& pinfo = m_PanesArray.Last();

    // set the pane window
    pinfo.m_Window = window;
    
    // if the pane's name identifier is blank, create a random string
    if (pinfo.m_Name.IsEmpty())
    {
        pinfo.m_Name.Printf(wxT("%08x%08x%08x%08x"),
             ((unsigned long)pinfo.m_Window) & 0xffffffff,
             (unsigned int)time(NULL),
             (unsigned int)clock(), m_PanesArray.GetCount());
    }
    
    // set initial proportion (if not already set)
    if (pinfo.m_DockProportion == 0)
        pinfo.m_DockProportion = 100000;

    if (pinfo.HasCloseButton() &&
        pinfo.m_Buttons.size() == 0)
    {
        wxPaneButton button;
        button.m_ButtonId = wxPaneInfo::buttonClose;
        pinfo.m_Buttons.Add(button);
    }
    
    if (pinfo.m_BestSize == wxDefaultSize &&
        pinfo.m_Window)
    {
        pinfo.m_BestSize = pinfo.m_Window->GetClientSize();

        if (pinfo.m_Window->IsKindOf(CLASSINFO(wxToolBar)))
        {
            // GetClientSize() doesn't get the best size for
            // a toolbar under some newer versions of wxWidgets,
            // so use GetBestSize()
            pinfo.m_BestSize = pinfo.m_Window->GetBestSize();
                    
            // for some reason, wxToolBar::GetBestSize() is returning
            // a size that is a pixel shy of the correct amount.
            // I believe this to be the correct action, until
            // wxToolBar::GetBestSize() is fixed.  Is this assumption
            // correct?
            pinfo.m_BestSize.y++;
        }
        
        if (pinfo.m_MinSize != wxDefaultSize)
        {
            if (pinfo.m_BestSize.x < pinfo.m_MinSize.x)
                pinfo.m_BestSize.x = pinfo.m_MinSize.x;
            if (pinfo.m_BestSize.y < pinfo.m_MinSize.y)
                pinfo.m_BestSize.y = pinfo.m_MinSize.y;
        }
    }

    return true;
}

bool wxFrameManager::AddPane(wxWindow* window,
                             int direction,
                             const wxString& caption)
{
    wxPaneInfo pinfo;
    pinfo.Caption(caption);
    switch (direction)
    {
        case wxTOP:    pinfo.Top(); break;
        case wxBOTTOM: pinfo.Bottom(); break;
        case wxLEFT:   pinfo.Left(); break;
        case wxRIGHT:  pinfo.Right(); break;
        case wxCENTER: pinfo.CenterPane(); break;
    }
    return AddPane(window, pinfo);
}

bool wxFrameManager::InsertPane(wxWindow* window, const wxPaneInfo& pane_info,
                                int insert_level)
{
    // shift the panes around, depending on the insert level
    switch (insert_level)
    {
        case wxAUI_INSERT_PANE:
            DoInsertPane(m_PanesArray,
                 pane_info.m_DockDirection,
                 pane_info.m_DockLayer,
                 pane_info.m_DockRow,
                 pane_info.m_DockPos);
            break;
        case wxAUI_INSERT_ROW:
            DoInsertDockRow(m_PanesArray,
                 pane_info.m_DockDirection,
                 pane_info.m_DockLayer,
                 pane_info.m_DockRow);
            break;
        case wxAUI_INSERT_DOCK:
            DoInsertDockLayer(m_PanesArray,
                 pane_info.m_DockDirection,
                 pane_info.m_DockLayer);
            break;
    }
    
    // if the window already exists, we are basically just moving/inserting the
    // existing window.  If it doesn't exist, we need to add it and insert it
    wxPaneInfo& existing_pane = GetPane(window);
    if (!existing_pane.IsOk())
    {
        return AddPane(window, pane_info);
    }
     else
    {
        if (pane_info.IsFloating())
        {
            existing_pane.Float();
            if (pane_info.m_FloatingPos != wxDefaultPosition)
                existing_pane.FloatingPosition(pane_info.m_FloatingPos);
            if (pane_info.m_FloatingSize != wxDefaultSize)
                existing_pane.FloatingSize(pane_info.m_FloatingSize);
        }
         else
        {
            existing_pane.Direction(pane_info.m_DockDirection);
            existing_pane.Layer(pane_info.m_DockLayer);
            existing_pane.Row(pane_info.m_DockRow);
            existing_pane.Position(pane_info.m_DockPos);
        }
    }

    return true;
}

    
bool wxFrameManager::DetachPane(wxWindow* window)
{
    int i, count;
    for (i = 0, count = m_PanesArray.GetCount(); i < count; ++i)
    {
        wxPaneInfo& p = m_PanesArray.Item(i);    
        if (p.m_Window == window)
        {
            if (p.m_Frame)
            {
                // we have a floating frame which is being detached. We need to
                // reparent it to m_Frame and destroy the floating frame

                // reduce flicker
                p.m_Window->SetSize(1,1);
                p.m_Frame->Show(false);

                // reparent to m_Frame and destroy the pane
                p.m_Window->Reparent(m_Frame);
                p.m_Frame->SetSizer(NULL);
                p.m_Frame->Destroy();
                p.m_Frame = NULL;
            }
            m_PanesArray.RemoveAt(i);
            return true;
        }
    }
    return false;
}


// EscapeDelimiters() changes ";" into "\;" and "|" into "\|"
// in the input string.  This is an internal functions which is
// used for saving perspectives
static wxString EscapeDelimiters(const wxString& s)
{
    wxString result;
    result.Alloc(s.Length());
    const wxChar* ch = s.c_str();
    while (*ch)
    {
        if (*ch == wxT(';') || *ch == wxT('|'))
            result += wxT('\\');
        result += *ch;
        ++ch;
    }
    return result;
}


// SavePerspective() saves all pane information as a single string.
// This string may later be fed into LoadPerspective() to restore
// all pane settings.  This save and load mechanism allows an
// exact pane configuration to be saved and restored at a later time

wxString wxFrameManager::SavePerspective()
{
    wxString result;
    result.Alloc(500);
    result = wxT("layout1|");

    int pane_i, pane_count = m_PanesArray.GetCount();
    for (pane_i = 0; pane_i < pane_count; ++pane_i)
    {
        wxPaneInfo& pane = m_PanesArray.Item(pane_i);
        
        result += wxT("name=");
        result += EscapeDelimiters(pane.m_Name);
        result += wxT(";");
        
        result += wxT("caption=");
        result += EscapeDelimiters(pane.m_Caption);
        result += wxT(";");
        
        result += wxString::Format(wxT("state=%u;"), pane.m_State);
        result += wxString::Format(wxT("dir=%d;"), pane.m_DockDirection);
        result += wxString::Format(wxT("layer=%d;"), pane.m_DockLayer);
        result += wxString::Format(wxT("row=%d;"), pane.m_DockRow);
        result += wxString::Format(wxT("pos=%d;"), pane.m_DockPos);
        result += wxString::Format(wxT("prop=%d;"), pane.m_DockProportion);
        result += wxString::Format(wxT("bestw=%d;"), pane.m_BestSize.x);
        result += wxString::Format(wxT("besth=%d;"), pane.m_BestSize.y);
        result += wxString::Format(wxT("minw=%d;"), pane.m_MinSize.x);
        result += wxString::Format(wxT("minh=%d;"), pane.m_MinSize.y);
        result += wxString::Format(wxT("maxw=%d;"), pane.m_MaxSize.x);
        result += wxString::Format(wxT("maxh=%d;"), pane.m_MaxSize.y);
        result += wxString::Format(wxT("floatx=%d;"), pane.m_FloatingPos.x);
        result += wxString::Format(wxT("floaty=%d;"), pane.m_FloatingPos.y);
        result += wxString::Format(wxT("floatw=%d;"), pane.m_FloatingSize.x);
        result += wxString::Format(wxT("floath=%d"), pane.m_FloatingSize.y);
        result += wxT("|");
    }
    
    int dock_i, dock_count = m_Docks.GetCount();
    for (dock_i = 0; dock_i < dock_count; ++dock_i)
    {
        wxDockInfo& dock = m_Docks.Item(dock_i);
        
        result += wxString::Format(wxT("dock_size(%d,%d,%d)=%d|"),
                                   dock.m_DockDirection, dock.m_DockLayer,
                                   dock.m_DockRow, dock.m_Size);
    }
    
    return result;
}

// LoadPerspective() loads a layout which was saved with SavePerspective()
// If the "update" flag parameter is true, the GUI will immediately be updated

bool wxFrameManager::LoadPerspective(const wxString& layout, bool update)
{
    wxString input = layout;
    wxString part;
    
    // check layout string version
    part = input.BeforeFirst(wxT('|'));
    input = input.AfterFirst(wxT('|'));
    part.Trim(true);
    part.Trim(false);
    if (part != wxT("layout1"))
        return false;
    
    
    // mark all panes currently managed as docked and hidden
    int pane_i, pane_count = m_PanesArray.GetCount();
    for (pane_i = 0; pane_i < pane_count; ++pane_i)
        m_PanesArray.Item(pane_i).Dock().Hide();

    // clear out the dock array; this will be reconstructed
    m_Docks.Clear();
    
    // replace escaped characters so we can
    // split up the string easily
    input.Replace(wxT("\\|"), wxT("\a"));
    input.Replace(wxT("\\;"), wxT("\b"));
    
    while (1)
    {
        wxPaneInfo pane;

        wxString pane_part = input.BeforeFirst(wxT('|'));
        input = input.AfterFirst(wxT('|'));
        pane_part.Trim(true);

        // if the string is empty, we're done parsing
        if (pane_part.IsEmpty())
            break;


        if (pane_part.Left(9) == wxT("dock_size"))
        {
            wxString val_name = pane_part.BeforeFirst(wxT('='));
            wxString value = pane_part.AfterFirst(wxT('='));
                  
            long dir, layer, row, size;
            wxString piece = val_name.AfterFirst(wxT('('));
            piece = piece.BeforeLast(wxT(')'));
            piece.BeforeFirst(wxT(',')).ToLong(&dir);
            piece = piece.AfterFirst(wxT(','));
            piece.BeforeFirst(wxT(',')).ToLong(&layer);
            piece.AfterFirst(wxT(',')).ToLong(&row);
            value.ToLong(&size);
            
            wxDockInfo dock;
            dock.m_DockDirection = dir;
            dock.m_DockLayer = layer;
            dock.m_DockRow = row;
            dock.m_Size = size;
            m_Docks.Add(dock);
            continue;
        }

        while (1)
        {
            wxString val_part = pane_part.BeforeFirst(wxT(';'));
            pane_part = pane_part.AfterFirst(wxT(';'));
            wxString val_name = val_part.BeforeFirst(wxT('='));
            wxString value = val_part.AfterFirst(wxT('='));
            val_name.MakeLower();
            val_name.Trim(true);
            val_name.Trim(false);
            value.Trim(true);
            value.Trim(false);
        
            if (val_name.IsEmpty())
                break;

            if (val_name == wxT("name"))
                pane.m_Name = value;
            else if (val_name == wxT("caption"))
                pane.m_Caption = value;
            else if (val_name == wxT("state"))
                pane.m_State = (unsigned int)wxAtoi(value.c_str());
            else if (val_name == wxT("dir"))
                pane.m_DockDirection = wxAtoi(value.c_str());
            else if (val_name == wxT("layer"))
                pane.m_DockLayer = wxAtoi(value.c_str());
            else if (val_name == wxT("row"))
                pane.m_DockRow = wxAtoi(value.c_str());
            else if (val_name == wxT("pos"))
                pane.m_DockPos = wxAtoi(value.c_str());
            else if (val_name == wxT("prop"))
                pane.m_DockProportion = wxAtoi(value.c_str());
            else if (val_name == wxT("bestw"))
                pane.m_BestSize.x = wxAtoi(value.c_str());
            else if (val_name == wxT("besth"))
                pane.m_BestSize.y = wxAtoi(value.c_str());
            else if (val_name == wxT("minw"))
                pane.m_MinSize.x = wxAtoi(value.c_str());
            else if (val_name == wxT("minh"))
                pane.m_MinSize.y = wxAtoi(value.c_str());
            else if (val_name == wxT("maxw"))
                pane.m_MaxSize.x = wxAtoi(value.c_str());
            else if (val_name == wxT("maxh"))
                pane.m_MaxSize.y = wxAtoi(value.c_str());
            else if (val_name == wxT("floatx"))
                pane.m_FloatingPos.x = wxAtoi(value.c_str());
            else if (val_name == wxT("floaty"))
                pane.m_FloatingPos.y = wxAtoi(value.c_str());
            else if (val_name == wxT("floatw"))
                pane.m_FloatingSize.x = wxAtoi(value.c_str());
            else if (val_name == wxT("floath"))
                pane.m_FloatingSize.y = wxAtoi(value.c_str());
            else {
                wxFAIL_MSG(wxT("Bad Perspective String"));
            }
        }
        
        // replace escaped characters so we can
        // split up the string easily
        pane.m_Name.Replace(wxT("\a"), wxT("|"));
        pane.m_Name.Replace(wxT("\b"), wxT(";"));
        pane.m_Caption.Replace(wxT("\a"), wxT("|"));
        pane.m_Caption.Replace(wxT("\b"), wxT(";"));
        
        wxPaneInfo& p = GetPane(pane.m_Name);
        if (!p.IsOk())
        {
            // the pane window couldn't be found
            // in the existing layout
            return false;
        }
        
        pane.m_Window = p.m_Window;
        pane.m_Frame = p.m_Frame;
        pane.m_Buttons = p.m_Buttons;
        p = pane;
    }
    
    if (update)
        Update();

    return true;
}


void wxFrameManager::GetPanePositionsAndSizes(wxDockInfo& dock,
                                              wxArrayInt& positions,
                                              wxArrayInt& sizes)
{
    int caption_size = m_Art->GetMetric(wxAUI_ART_CAPTION_SIZE);
    int pane_border_size = m_Art->GetMetric(wxAUI_ART_PANE_BORDER_SIZE);
    int gripper_size = m_Art->GetMetric(wxAUI_ART_GRIPPER_SIZE);

    positions.Empty();
    sizes.Empty();

    int offset, action_pane = -1;
    int pane_i, pane_count = dock.m_Panes.GetCount();

    // find the pane marked as our action pane
    for (pane_i = 0; pane_i < pane_count; ++pane_i)
    {
        wxPaneInfo& pane = *(dock.m_Panes.Item(pane_i));

        if (pane.m_State & wxPaneInfo::actionPane)
        {
            wxASSERT_MSG(action_pane==-1, wxT("Too many fixed action panes"));
            action_pane = pane_i;
        }
    }
    
    // set up each panes default position, and
    // determine the size (width or height, depending
    // on the dock's orientation) of each pane
    for (pane_i = 0; pane_i < pane_count; ++pane_i)
    {
        wxPaneInfo& pane = *(dock.m_Panes.Item(pane_i));
        positions.Add(pane.m_DockPos);
        int size = 0;
        
        if (pane.HasBorder())
            size += (pane_border_size*2);
        if (pane.HasGripper())
            size += gripper_size;
                
        if (dock.IsHorizontal())
        {
            size += pane.m_BestSize.x;
        }
         else
        {
            if (pane.HasCaption())
                size += caption_size; 
            size += pane.m_BestSize.y;
        }
   
        sizes.Add(size);
    }

    // if there is no action pane, just return the default
    // positions (as specified in pane.pane_pos)
    if (action_pane == -1)
        return;

    offset = 0;
    for (pane_i = action_pane-1; pane_i >= 0; --pane_i)
    {
        int amount = positions[pane_i+1] - (positions[pane_i] + sizes[pane_i]);

        if (amount >= 0)
            offset += amount;
             else
            positions[pane_i] -= -amount;

        offset += sizes[pane_i];
    }
    
    // if the dock mode is fixed, make sure none of the panes
    // overlap; we will bump panes that overlap
    offset = 0;
    for (pane_i = action_pane; pane_i < pane_count; ++pane_i)
    {
        int amount = positions[pane_i] - offset;
        if (amount >= 0)
            offset += amount;
             else
            positions[pane_i] += -amount;

        offset += sizes[pane_i];
    }
}


void wxFrameManager::LayoutAddPane(wxSizer* cont,
                                   wxDockInfo& dock,
                                   wxPaneInfo& pane,
                                   wxDockUIPartArray& uiparts,
                                   bool spacer_only)
{        
    wxDockUIPart part;
    wxSizerItem* sizer_item;

    int caption_size = m_Art->GetMetric(wxAUI_ART_CAPTION_SIZE);
    int gripper_size = m_Art->GetMetric(wxAUI_ART_GRIPPER_SIZE);
    int pane_border_size = m_Art->GetMetric(wxAUI_ART_PANE_BORDER_SIZE);
    int pane_button_size = m_Art->GetMetric(wxAUI_ART_PANE_BUTTON_SIZE);

    // find out the orientation of the item (orientation for panes
    // is the same as the dock's orientation)
    int orientation;
    if (dock.IsHorizontal())
        orientation = wxHORIZONTAL;
         else
        orientation = wxVERTICAL;

    // this variable will store the proportion
    // value that the pane will receive
    int pane_proportion = pane.m_DockProportion;

    wxBoxSizer* horz_pane_sizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* vert_pane_sizer = new wxBoxSizer(wxVERTICAL);

    if (pane.HasGripper())
    {
        sizer_item = horz_pane_sizer->Add(gripper_size, 1, 0, wxEXPAND);

        part.m_Type = wxDockUIPart::typeGripper;
        part.m_Dock = &dock;
        part.m_Pane = &pane;
        part.m_Button = NULL;
        part.m_Orientation = orientation;
        part.m_ContSizer = horz_pane_sizer;
        part.m_SizerItem = sizer_item;
        uiparts.Add(part);
    }

    if (pane.HasCaption())
    {
        // create the caption sizer
        wxBoxSizer* caption_sizer = new wxBoxSizer(wxHORIZONTAL);

        sizer_item = caption_sizer->Add(1, caption_size, 1, wxEXPAND);

        part.m_Type = wxDockUIPart::typeCaption;
        part.m_Dock = &dock;
        part.m_Pane = &pane;
        part.m_Button = NULL;
        part.m_Orientation = orientation;
        part.m_ContSizer = vert_pane_sizer;
        part.m_SizerItem = sizer_item;
        int caption_part_idx = uiparts.GetCount();
        uiparts.Add(part);

        // add pane buttons to the caption
        int i, button_count;
        for (i = 0, button_count = pane.m_Buttons.GetCount();
             i < button_count; ++i)
        {
            wxPaneButton& button = pane.m_Buttons.Item(i);

            sizer_item = caption_sizer->Add(pane_button_size,
                                            caption_size,
                                            0, wxEXPAND);

            part.m_Type = wxDockUIPart::typePaneButton;
            part.m_Dock = &dock;
            part.m_Pane = &pane;
            part.m_Button = &button;
            part.m_Orientation = orientation;
            part.m_ContSizer = caption_sizer;
            part.m_SizerItem = sizer_item;
            uiparts.Add(part);
        }

        // add the caption sizer
        sizer_item = vert_pane_sizer->Add(caption_sizer, 0, wxEXPAND);

        uiparts.Item(caption_part_idx).m_SizerItem = sizer_item;
    }

    // add the pane window itself
    if (spacer_only)
    {
        sizer_item = vert_pane_sizer->Add(1, 1, 1, wxEXPAND);
    }
     else
    {
        sizer_item = vert_pane_sizer->Add(pane.m_Window, 1, wxEXPAND);
        vert_pane_sizer->SetItemMinSize(pane.m_Window, 1, 1);
    }

    part.m_Type = wxDockUIPart::typePane;
    part.m_Dock = &dock;
    part.m_Pane = &pane;
    part.m_Button = NULL;
    part.m_Orientation = orientation;
    part.m_ContSizer = vert_pane_sizer;
    part.m_SizerItem = sizer_item;
    uiparts.Add(part);


    // determine if the pane should have a minimum size; if the pane is
    // non-resizable (fixed) then we must set a minimum size. Alternitavely,
    // if the pane.min_size is set, we must use that value as well
    
    wxSize min_size = pane.m_MinSize;
    if (pane.IsFixed())
    {
        if (min_size == wxDefaultSize)
        {
            min_size = pane.m_BestSize;
            pane_proportion = 0;
        }
    }
    
    if (min_size != wxDefaultSize)
    {
        vert_pane_sizer->SetItemMinSize(
                        vert_pane_sizer->GetChildren().GetCount()-1,
                        min_size.x, min_size.y);
    }


    // add the verticle sizer (caption, pane window) to the
    // horizontal sizer (gripper, verticle sizer)
    horz_pane_sizer->Add(vert_pane_sizer, 1, wxEXPAND);

    // finally, add the pane sizer to the dock sizer

    if (pane.HasBorder())
    {
        // allowing space for the pane's border
        sizer_item = cont->Add(horz_pane_sizer, pane_proportion,
                               wxEXPAND | wxALL, pane_border_size);

        part.m_Type = wxDockUIPart::typePaneBorder;
        part.m_Dock = &dock;
        part.m_Pane = &pane;
        part.m_Button = NULL;
        part.m_Orientation = orientation;
        part.m_ContSizer = cont;
        part.m_SizerItem = sizer_item;
        uiparts.Add(part);
    }
     else
    {
        sizer_item = cont->Add(horz_pane_sizer, pane_proportion, wxEXPAND);
    }
}

void wxFrameManager::LayoutAddDock(wxSizer* cont,
                                   wxDockInfo& dock,
                                   wxDockUIPartArray& uiparts,
                                   bool spacer_only)
{
    wxSizerItem* sizer_item;
    wxDockUIPart part;

    int sash_size = m_Art->GetMetric(wxAUI_ART_SASH_SIZE);
    int orientation = dock.IsHorizontal() ? wxHORIZONTAL : wxVERTICAL;

    // resizable bottom and right docks have a sash before them
    if (!dock.m_Fixed && (dock.m_DockDirection == wxAUI_DOCK_BOTTOM ||
                        dock.m_DockDirection == wxAUI_DOCK_RIGHT))
    {
        sizer_item = cont->Add(sash_size, sash_size, 0, wxEXPAND);

        part.m_Type = wxDockUIPart::typeDockSizer;
        part.m_Orientation = orientation;
        part.m_Dock = &dock;
        part.m_Pane = NULL;
        part.m_Button = NULL;
        part.m_ContSizer = cont;
        part.m_SizerItem = sizer_item;
        uiparts.Add(part);
    }

    // create the sizer for the dock
    wxSizer* dock_sizer = new wxBoxSizer(orientation);

    // add each pane to the dock
    int pane_i, pane_count = dock.m_Panes.GetCount();

    if (dock.m_Fixed)
    {
        wxArrayInt pane_positions, pane_sizes;
        
        // figure out the real pane positions we will
        // use, without modifying the each pane's pane_pos member
        GetPanePositionsAndSizes(dock, pane_positions, pane_sizes);

        int offset = 0;
        for (pane_i = 0; pane_i < pane_count; ++pane_i)
        {
            wxPaneInfo& pane = *(dock.m_Panes.Item(pane_i));
            int pane_pos = pane_positions.Item(pane_i);

            int amount = pane_pos - offset;
            if (amount > 0)
            {
                if (dock.IsVertical())
                    sizer_item = dock_sizer->Add(1, amount, 0, wxEXPAND);
                     else
                    sizer_item = dock_sizer->Add(amount, 1, 0, wxEXPAND);

                part.m_Type = wxDockUIPart::typeBackground;
                part.m_Dock = &dock;
                part.m_Pane = NULL;
                part.m_Button = NULL;
                part.m_Orientation = (orientation==wxHORIZONTAL) ? wxVERTICAL:wxHORIZONTAL;
                part.m_ContSizer = dock_sizer;
                part.m_SizerItem = sizer_item;
                uiparts.Add(part);

                offset += amount;
            }

            LayoutAddPane(dock_sizer, dock, pane, uiparts, spacer_only);

            offset += pane_sizes.Item(pane_i);
        }

        // at the end add a very small stretchable background area
        sizer_item = dock_sizer->Add(1,1, 1, wxEXPAND);

        part.m_Type = wxDockUIPart::typeBackground;
        part.m_Dock = &dock;
        part.m_Pane = NULL;
        part.m_Button = NULL;
        part.m_Orientation = orientation;
        part.m_ContSizer = dock_sizer;
        part.m_SizerItem = sizer_item;
        uiparts.Add(part);
    }
     else
    {
        for (pane_i = 0; pane_i < pane_count; ++pane_i)
        {
            wxPaneInfo& pane = *(dock.m_Panes.Item(pane_i));

            // if this is not the first pane being added,
            // we need to add a pane sizer
            if (pane_i > 0)
            {
                sizer_item = dock_sizer->Add(sash_size, sash_size, 0, wxEXPAND);

                part.m_Type = wxDockUIPart::typePaneSizer;
                part.m_Dock = &dock;
                part.m_Pane = dock.m_Panes.Item(pane_i-1);
                part.m_Button = NULL;
                part.m_Orientation = (orientation==wxHORIZONTAL) ? wxVERTICAL:wxHORIZONTAL;
                part.m_ContSizer = dock_sizer;
                part.m_SizerItem = sizer_item;
                uiparts.Add(part);
            }

            LayoutAddPane(dock_sizer, dock, pane, uiparts, spacer_only);
        }
    }

    if (dock.m_DockDirection == wxAUI_DOCK_CENTER)
        sizer_item = cont->Add(dock_sizer, 1, wxEXPAND);
         else
        sizer_item = cont->Add(dock_sizer, 0, wxEXPAND);

    part.m_Type = wxDockUIPart::typeDock;
    part.m_Dock = &dock;
    part.m_Pane = NULL;
    part.m_Button = NULL;
    part.m_Orientation = orientation;
    part.m_ContSizer = cont;
    part.m_SizerItem = sizer_item;
    uiparts.Add(part);

    if (dock.IsHorizontal())
        cont->SetItemMinSize(dock_sizer, 0, dock.m_Size);
         else
        cont->SetItemMinSize(dock_sizer, dock.m_Size, 0);

    //  top and left docks have a sash after them
    if (!dock.m_Fixed && (dock.m_DockDirection == wxAUI_DOCK_TOP ||
                        dock.m_DockDirection == wxAUI_DOCK_LEFT))
    {
        sizer_item = cont->Add(sash_size, sash_size, 0, wxEXPAND);

        part.m_Type = wxDockUIPart::typeDockSizer;
        part.m_Dock = &dock;
        part.m_Pane = NULL;
        part.m_Button = NULL;
        part.m_Orientation = orientation;
        part.m_ContSizer = cont;
        part.m_SizerItem = sizer_item;
        uiparts.Add(part);
    }
}

wxSizer* wxFrameManager::LayoutAll(wxPaneInfoArray& panes,
                                   wxDockInfoArray& docks,
                                   wxDockUIPartArray& uiparts,
                                   bool spacer_only)
{
    wxBoxSizer* container = new wxBoxSizer(wxVERTICAL);

    int pane_border_size = m_Art->GetMetric(wxAUI_ART_PANE_BORDER_SIZE);
    int caption_size = m_Art->GetMetric(wxAUI_ART_CAPTION_SIZE);
    wxSize cli_size = m_Frame->GetClientSize();
    int i, dock_count, pane_count;
    

    // empty all docks out
    for (i = 0, dock_count = docks.GetCount(); i < dock_count; ++i)
        docks.Item(i).m_Panes.Empty();
    
    // iterate through all known panes, filing each
    // of them into the appropriate dock. If the
    // pane does not exist in the dock, add it
    for (i = 0, pane_count = panes.GetCount(); i < pane_count; ++i)
    {
        wxPaneInfo& p = panes.Item(i);

        // find any docks in this layer
        wxDockInfo* dock;
        wxDockInfoPtrArray arr;
        FindDocks(docks, p.m_DockDirection, p.m_DockLayer, p.m_DockRow, arr);

        if (arr.GetCount() > 0)
        {
            dock = arr.Item(0);
        }
         else
        {
            // dock was not found, so we need to create a new one
            wxDockInfo d;
            d.m_DockDirection = p.m_DockDirection;
            d.m_DockLayer = p.m_DockLayer;
            d.m_DockRow = p.m_DockRow;
            docks.Add(d);
            dock = &docks.Last();
        }


        if (p.IsDocked() && p.IsShown())
        {
            // remove the pane from any existing docks except this one
            RemovePaneFromDocks(docks, p, dock);

            // pane needs to be added to the dock,
            // if it doesn't already exist 
            if (!FindPaneInDock(*dock, p.m_Window))
                dock->m_Panes.Add(&p);
        }
         else
        {
            // remove the pane from any existing docks
            RemovePaneFromDocks(docks, p);
        }

    }

    // remove any empty docks
    for (i = docks.GetCount()-1; i >= 0; --i)
    {
        if (docks.Item(i).m_Panes.GetCount() == 0)
            docks.RemoveAt(i);
    }

    // configure the docks further
    for (i = 0, dock_count = docks.GetCount(); i < dock_count; ++i)
    {
        wxDockInfo& dock = docks.Item(i);
        int j, dock_pane_count = dock.m_Panes.GetCount();
        
        // sort the dock pane array by the pane's
        // dock position (dock_pos), in ascending order
        dock.m_Panes.Sort(PaneSortFunc);

        // for newly created docks, set up their initial size
        if (dock.m_Size == 0)
        {
            int size = 0;

            for (j = 0; j < dock_pane_count; ++j)
            {
                wxPaneInfo& pane = *dock.m_Panes.Item(j);
                wxSize pane_size = pane.m_BestSize;
                if (pane_size == wxDefaultSize)
                    pane_size = pane.m_MinSize;
                if (pane_size == wxDefaultSize)
                    pane_size = pane.m_Window->GetSize();
                
                if (dock.IsHorizontal())
                    size = wxMax(pane_size.y, size);
                     else
                    size = wxMax(pane_size.x, size);
            }
            
            // add space for the border (two times), but only
            // if at least one pane inside the dock has a pane border
            for (j = 0; j < dock_pane_count; ++j)
            {
                if (dock.m_Panes.Item(j)->HasBorder())
                {
                    size += (pane_border_size*2);
                    break;
                }
            }

            // if pane is on the top or bottom, add the caption height,
            // but only if at least one pane inside the dock has a caption
            if (dock.IsHorizontal())
            {
                for (j = 0; j < dock_pane_count; ++j)
                {
                    if (dock.m_Panes.Item(j)->HasCaption())
                    {
                        size += caption_size;
                        break;
                    }
                }
            }

            // new dock's size may not be more than 1/3 of the frame size
            if (dock.IsHorizontal())
                size = wxMin(size, cli_size.y/3);
                 else
                size = wxMin(size, cli_size.x/3);

            if (size < 10)
                size = 10;
            dock.m_Size = size;
        }


        // determine the dock's minimum size
        bool plus_border = false;
        bool plus_caption = false;
        int dock_min_size = 0;
        for (j = 0; j < dock_pane_count; ++j)
        {
            wxPaneInfo& pane = *dock.m_Panes.Item(j);
            if (pane.m_MinSize != wxDefaultSize)
            {
                if (pane.HasBorder())
                    plus_border = true;
                if (pane.HasCaption())
                    plus_caption = true;
                if (dock.IsHorizontal())
                {
                    if (pane.m_MinSize.y > dock_min_size)
                        dock_min_size = pane.m_MinSize.y;
                }
                 else
                {
                    if (pane.m_MinSize.x > dock_min_size)
                        dock_min_size = pane.m_MinSize.x;
                }
            }
        }
        
        if (plus_border)
            dock_min_size += (pane_border_size*2);
        if (plus_caption && dock.IsHorizontal())
            dock_min_size += (caption_size);
           
        dock.m_MinSize = dock_min_size;
        
        
        // if the pane's current size is less than it's
        // minimum, increase the dock's size to it's minimum
        if (dock.m_Size < dock.m_MinSize)
            dock.m_Size = dock.m_MinSize;


        // determine the dock's mode (fixed or proportional);
        // determine whether the dock has only toolbars
        bool action_pane_marked = false;
        dock.m_Fixed = true;
        dock.m_Toolbar = true;
        for (j = 0; j < dock_pane_count; ++j)
        {
            wxPaneInfo& pane = *dock.m_Panes.Item(j);
            if (!pane.IsFixed())
                dock.m_Fixed = false;
            if (!pane.IsToolbar())
                dock.m_Toolbar = false;
            if (pane.m_State & wxPaneInfo::actionPane)
                action_pane_marked = true;
        }


        // if the dock mode is proportional and not fixed-pixel,
        // reassign the dock_pos to the sequential 0, 1, 2, 3;
        // e.g. remove gaps like 1, 2, 30, 500
        if (!dock.m_Fixed)
        {
            for (j = 0; j < dock_pane_count; ++j)
            {
                wxPaneInfo& pane = *dock.m_Panes.Item(j);
                pane.m_DockPos = j;
            }
        }

        // if the dock mode is fixed, and none of the panes
        // are being moved right now, make sure the panes
        // do not overlap each other.  If they do, we will
        // adjust the panes' positions
        if (dock.m_Fixed && !action_pane_marked)
        {
            wxArrayInt pane_positions, pane_sizes;
            GetPanePositionsAndSizes(dock, pane_positions, pane_sizes);
            
            int offset = 0;
            for (j = 0; j < dock_pane_count; ++j)
            {
                wxPaneInfo& pane = *(dock.m_Panes.Item(j));
                pane.m_DockPos = pane_positions[j];

                int amount = pane.m_DockPos - offset;
                if (amount >= 0)
                    offset += amount;
                     else
                    pane.m_DockPos += -amount;

                offset += pane_sizes[j];
            }
        }
    }
    
    // discover the maximum dock layer
    int max_layer = 0;
    for (i = 0; i < dock_count; ++i)
        max_layer = wxMax(max_layer, docks.Item(i).m_DockLayer);
 

    // clear out uiparts
    uiparts.Empty();

    // create a bunch of box sizers,
    // from the innermost level outwards.
    wxSizer* cont = NULL;
    wxSizer* middle = NULL;
    int layer = 0;
    int row, row_count;

    for (layer = 0; layer <= max_layer; ++layer)
    {
        wxDockInfoPtrArray arr;

        // find any docks in this layer
        FindDocks(docks, -1, layer, -1, arr);

        // if there aren't any, skip to the next layer
        if (arr.IsEmpty())
            continue;

        wxSizer* old_cont = cont;

        // create a container which will hold this layer's
        // docks (top, bottom, left, right)
        cont = new wxBoxSizer(wxVERTICAL);


        // find any top docks in this layer
        FindDocks(docks, wxAUI_DOCK_TOP, layer, -1, arr);
        RenumberDockRows(arr);
        if (!arr.IsEmpty())
        {
            for (row = 0, row_count = arr.GetCount(); row < row_count; ++row)
                LayoutAddDock(cont, *arr.Item(row), uiparts, spacer_only);
        }

        
        // fill out the middle layer (which consists
        // of left docks, content area and right docks)
        
        middle = new wxBoxSizer(wxHORIZONTAL);

        // find any left docks in this layer
        FindDocks(docks, wxAUI_DOCK_LEFT, layer, -1, arr);
        RenumberDockRows(arr);
        if (!arr.IsEmpty())
        {
            for (row = 0, row_count = arr.GetCount(); row < row_count; ++row)
                LayoutAddDock(middle, *arr.Item(row), uiparts, spacer_only);
        }

        // add content dock (or previous layer's sizer
        // to the middle
        if (!old_cont)
        {
            // find any center docks
            FindDocks(docks, wxAUI_DOCK_CENTER, -1, -1, arr);
            if (!arr.IsEmpty())
            {
                for (row = 0,row_count = arr.GetCount(); row<row_count; ++row)
                   LayoutAddDock(middle, *arr.Item(row), uiparts, spacer_only);
            }
             else
            {
                // there are no center docks, add a background area
                wxSizerItem* sizer_item = middle->Add(1,1, 1, wxEXPAND);
                wxDockUIPart part;
                part.m_Type = wxDockUIPart::typeBackground;
                part.m_Pane = NULL;
                part.m_Dock = NULL;
                part.m_Button = NULL;
                part.m_ContSizer = middle;
                part.m_SizerItem = sizer_item;
                uiparts.Add(part);
            }
        }
         else
        {
            middle->Add(old_cont, 1, wxEXPAND);
        }

        // find any right docks in this layer
        FindDocks(docks, wxAUI_DOCK_RIGHT, layer, -1, arr);
        RenumberDockRows(arr);
        if (!arr.IsEmpty())
        {
            for (row = arr.GetCount()-1; row >= 0; --row)
                LayoutAddDock(middle, *arr.Item(row), uiparts, spacer_only);
        }

        cont->Add(middle, 1, wxEXPAND);



        // find any bottom docks in this layer
        FindDocks(docks, wxAUI_DOCK_BOTTOM, layer, -1, arr);
        RenumberDockRows(arr);
        if (!arr.IsEmpty())
        {
            for (row = arr.GetCount()-1; row >= 0; --row)
                LayoutAddDock(cont, *arr.Item(row), uiparts, spacer_only);
        }

    }

    if (!cont)
    {
        // no sizer available, because there are no docks,
        // therefore we will create a simple background area
        cont = new wxBoxSizer(wxVERTICAL);
        wxSizerItem* sizer_item = cont->Add(1,1, 1, wxEXPAND);
        wxDockUIPart part;
        part.m_Type = wxDockUIPart::typeBackground;
        part.m_Pane = NULL;
        part.m_Dock = NULL;
        part.m_Button = NULL;
        part.m_ContSizer = middle;
        part.m_SizerItem = sizer_item;
        uiparts.Add(part);
    }

    container->Add(cont, 1, wxEXPAND);
    return container;
}


// Update() updates the layout.  Whenever changes are made to
// one or more panes, this function should be called.  It is the
// external entry point for running the layout engine.

void wxFrameManager::Update()
{
    wxSizer* sizer;
    int i, pane_count = m_PanesArray.GetCount();

    // delete old sizer first
    m_Frame->SetSizer(NULL);

    // destroy floating panes which have been
    // redocked or are becoming non-floating
    for (i = 0; i < pane_count; ++i)
    {
        wxPaneInfo& p = m_PanesArray.Item(i);

        if (!p.IsFloating() && p.m_Frame)
        {
            // because the pane is no longer in a floating, we need to
            // reparent it to m_Frame and destroy the floating frame
            
            // reduce flicker
            p.m_Window->SetSize(1,1);
            p.m_Frame->Show(false);
                       
            // reparent to m_Frame and destroy the pane
            p.m_Window->Reparent(m_Frame);
            p.m_Frame->SetSizer(NULL);
            p.m_Frame->Destroy();
            p.m_Frame = NULL;
        }
    }


    // create a layout for all of the panes
    sizer = LayoutAll(m_PanesArray, m_Docks, m_UIParts, false);

    // hide or show panes as necessary,
    // and float panes as necessary
    for (i = 0; i < pane_count; ++i)
    {
        wxPaneInfo& p = m_PanesArray.Item(i);

        if (p.IsFloating())
        {
            if (p.m_Frame == NULL)
            {
                // we need to create a frame for this
                // pane, which has recently been floated
                wxFloatingPane* frame = new wxFloatingPane(m_Frame,
                                                  this, -1,
                                                  p.m_FloatingPos,
                                                  p.m_FloatingSize);
                                
                // on MSW, if the owner desires transparent dragging, and
                // the dragging is happening right now, then the floating
                // window should have this style by default                  
                #ifdef __WXMSW__
                if (m_Action == actionDragFloatingPane &&
                    (m_Flags & wxAUI_MGR_TRANSPARENT_DRAG))
                        MakeWindowTransparent(frame, 150);
                #endif
                
                frame->SetPaneWindow(p);
                p.m_Frame = frame;

                if (p.IsShown())
                {
                    frame->Show();
                }
            }
             else
            {
                // frame already exists, make sure it's position
                // and size reflect the information in wxPaneInfo
                if (p.m_Frame->GetPosition() != p.m_FloatingPos)
                {
                    p.m_Frame->SetSize(p.m_FloatingPos.x, p.m_FloatingPos.y,
                                     -1, -1, wxSIZE_USE_EXISTING);
                    //p.frame->Move(p.floating_pos.x, p.floating_pos.y);
                }

                 p.m_Frame->Show(p.IsShown());
            }
        }
         else
        {
            p.m_Window->Show(p.IsShown());
        }

        // if "active panes" are no longer allowed, clear
        // any optionActive values from the pane states
        if ((m_Flags & wxAUI_MGR_ALLOW_ACTIVE_PANE) == 0)
        {
            p.m_State &= ~wxPaneInfo::optionActive;
        }
    }


    // keep track of the old window rectangles so we can
    // refresh those windows whose rect has changed
    wxAuiRectArray old_pane_rects;
    for (i = 0; i < pane_count; ++i)
    {
        wxRect r;
        wxPaneInfo& p = m_PanesArray.Item(i);

        if (p.m_Window && p.IsShown() && p.IsDocked())
            r = p.m_Rect;

        old_pane_rects.Add(r);
    }




    // apply the new sizer
    m_Frame->SetSizer(sizer);
    m_Frame->SetAutoLayout(false);
    DoFrameLayout();



    // now that the frame layout is done, we need to check
    // the new pane rectangles against the old rectangles that
    // we saved a few lines above here.  If the rectangles have
    // changed, the corresponding panes must also be updated
    for (i = 0; i < pane_count; ++i)
    {
        wxPaneInfo& p = m_PanesArray.Item(i);
        if (p.m_Window && p.m_Window->IsShown() && p.IsDocked())
        {
            if (p.m_Rect != old_pane_rects[i])
            {
                p.m_Window->Refresh();
                p.m_Window->Update();
            }
        }
    }


    Repaint();
    
    // set frame's minimum size
    
/*
    // N.B. More work needs to be done on frame minimum sizes;
    // this is some intresting code that imposes the minimum size,
    // but we may want to include a more flexible mechanism or
    // options for multiple minimum-size modes, e.g. strict or lax
    wxSize min_size = sizer->GetMinSize();
    wxSize frame_size = m_Frame->GetSize();
    wxSize client_size = m_Frame->GetClientSize();

    wxSize minframe_size(min_size.x+frame_size.x-client_size.x,
                         min_size.y+frame_size.y-client_size.y );
                         
    m_Frame->SetMinSize(minframe_size);
 
    if (frame_size.x < minframe_size.x ||
        frame_size.y < minframe_size.y)
            sizer->Fit(m_Frame);
*/
}


// DoFrameLayout() is an internal function which invokes wxSizer::Layout
// on the frame's main sizer, then measures all the various UI items
// and updates their internal rectangles.  This should always be called
// instead of calling m_Frame->Layout() directly

void wxFrameManager::DoFrameLayout()
{
    m_Frame->Layout();
    
    int i, part_count;
    for (i = 0, part_count = m_UIParts.GetCount(); i < part_count; ++i)
    {
        wxDockUIPart& part = m_UIParts.Item(i);

        // get the rectangle of the UI part
        // originally, this code looked like this:
        //    part.rect = wxRect(part.sizer_item->GetPosition(),
        //                       part.sizer_item->GetSize());
        // this worked quite well, with one exception: the mdi
        // client window had a "deferred" size variable 
        // that returned the wrong size.  It looks like
        // a bug in wx, because the former size of the window
        // was being returned.  So, we will retrieve the part's
        // rectangle via other means


        part.m_Rect = part.m_SizerItem->GetRect();
        int flag = part.m_SizerItem->GetFlag();
        int border = part.m_SizerItem->GetBorder();
        if (flag & wxTOP)
        {
            part.m_Rect.y -= border;
            part.m_Rect.height += border;
        }
        if (flag & wxLEFT)
        {
            part.m_Rect.x -= border;
            part.m_Rect.width += border;
        }
        if (flag & wxBOTTOM)
            part.m_Rect.height += border;
        if (flag & wxRIGHT)
            part.m_Rect.width += border;


        if (part.m_Type == wxDockUIPart::typeDock)
            part.m_Dock->m_Rect = part.m_Rect;
        if (part.m_Type == wxDockUIPart::typePane)
            part.m_Pane->m_Rect = part.m_Rect;
    }
}

// GetPanePart() looks up the pane the pane border UI part (or the regular
// pane part if there is no border). This allows the caller to get the exact
// rectangle of the pane in question, including decorations like
// caption and border (if any).

wxDockUIPart* wxFrameManager::GetPanePart(wxWindow* wnd)
{
    int i, part_count;
    for (i = 0, part_count = m_UIParts.GetCount(); i < part_count; ++i)
    {
        wxDockUIPart& part = m_UIParts.Item(i);
        if (part.m_Type == wxDockUIPart::typePaneBorder &&
            part.m_Pane && part.m_Pane->m_Window == wnd)
                return &part;
    }
    for (i = 0, part_count = m_UIParts.GetCount(); i < part_count; ++i)
    {
        wxDockUIPart& part = m_UIParts.Item(i);
        if (part.m_Type == wxDockUIPart::typePane &&
            part.m_Pane && part.m_Pane->m_Window == wnd)
                return &part;
    }
    return NULL;
}



// GetDockPixelOffset() is an internal function which returns
// a dock's offset in pixels from the left side of the window
// (for horizontal docks) or from the top of the window (for
// vertical docks).  This value is necessary for calculating
// fixel-pane/toolbar offsets when they are dragged.

int wxFrameManager::GetDockPixelOffset(wxPaneInfo& test)
{
    // the only way to accurately calculate the dock's
    // offset is to actually run a theoretical layout
    
    int i, part_count, dock_count;
    wxDockInfoArray docks;
    wxPaneInfoArray panes;
    wxDockUIPartArray uiparts;
    CopyDocksAndPanes(docks, panes, m_Docks, m_PanesArray);
    panes.Add(test);

    wxSizer* sizer = LayoutAll(panes, docks, uiparts, true);
    wxSize client_size = m_Frame->GetClientSize();
    sizer->SetDimension(0, 0, client_size.x, client_size.y);
    sizer->Layout();

    for (i = 0, part_count = uiparts.GetCount(); i < part_count; ++i)
    {
        wxDockUIPart& part = uiparts.Item(i);
        part.m_Rect = wxRect(part.m_SizerItem->GetPosition(),
                           part.m_SizerItem->GetSize());
        if (part.m_Type == wxDockUIPart::typeDock)
            part.m_Dock->m_Rect = part.m_Rect;
    }
    
    delete sizer;
    
    for (i = 0, dock_count = docks.GetCount(); i < dock_count; ++i)
    {
        wxDockInfo& dock = docks.Item(i);
        if (test.m_DockDirection == dock.m_DockDirection &&
            test.m_DockLayer==dock.m_DockLayer && test.m_DockRow==dock.m_DockRow)
        {
            if (dock.IsVertical())
                return dock.m_Rect.y;
                 else
                return dock.m_Rect.x;
        }
    }

    return 0;
}



// ProcessDockResult() is a utility function used by DoDrop() - it checks
// if a dock operation is allowed, the new dock position is copied into
// the target info.  If the operation was allowed, the function returns true.

static bool ProcessDockResult(wxPaneInfo& target,
                              const wxPaneInfo& new_pos)
{
    bool allowed = false;
    switch (new_pos.m_DockDirection)
    {
        case wxAUI_DOCK_TOP:    allowed = target.IsTopDockable();    break;
        case wxAUI_DOCK_BOTTOM: allowed = target.IsBottomDockable(); break;
        case wxAUI_DOCK_LEFT:   allowed = target.IsLeftDockable();   break;
        case wxAUI_DOCK_RIGHT:  allowed = target.IsRightDockable();  break;
    }

    if (allowed)
        target = new_pos;

    return allowed;
}


// DoDrop() is an important function.  It basically takes a mouse position,
// and determines where the pane's new position would be.  If the pane is to be
// dropped, it performs the drop operation using the specified dock and pane
// arrays.  By specifying copied dock and pane arrays when calling, a "what-if"
// scenario can be performed, giving precise coordinates for drop hints.
// If, however, wxFrameManager:m_Docks and wxFrameManager::m_PanesArray are specified
// as parameters, the changes will be made to the main state arrays

const int auiInsertRowPixels = 10;
const int auiNewRowPixels = 40;
const int auiLayerInsertPixels = 40;
const int auiLayerInsertOffset = 5;

bool wxFrameManager::DoDrop(wxDockInfoArray& docks,
                            wxPaneInfoArray& panes,
                            wxPaneInfo& target,
                            const wxPoint& pt,
                            const wxPoint& offset)
{
    wxSize cli_size = m_Frame->GetClientSize();

    wxPaneInfo drop = target;


    // The result should always be shown
    drop.Show();
    

    // Check to see if the pane has been dragged outside of the window
    // (or near to the outside of the window), if so, dock it along the edge


    int layer_insert_offset = auiLayerInsertOffset;
    if (target.IsToolbar())
        layer_insert_offset = 0;
    
    if (pt.x < layer_insert_offset &&
        pt.x > layer_insert_offset-auiLayerInsertPixels)
    {
        int new_layer = wxMax(wxMax(GetMaxLayer(docks, wxAUI_DOCK_LEFT),
                                    GetMaxLayer(docks, wxAUI_DOCK_BOTTOM)),
                                    GetMaxLayer(docks, wxAUI_DOCK_TOP)) + 1;       
        drop.Dock().Left().
             Layer(new_layer).
             Row(0).
             Position(pt.y - GetDockPixelOffset(drop) - offset.y);
        return ProcessDockResult(target, drop);
    }
     else if (pt.y < layer_insert_offset &&
              pt.y > layer_insert_offset-auiLayerInsertPixels)
    {
        int new_layer = wxMax(wxMax(GetMaxLayer(docks, wxAUI_DOCK_TOP),
                                    GetMaxLayer(docks, wxAUI_DOCK_LEFT)),
                                    GetMaxLayer(docks, wxAUI_DOCK_RIGHT)) + 1;
        drop.Dock().Top().
             Layer(new_layer).
             Row(0).
             Position(pt.x - GetDockPixelOffset(drop) - offset.x);
        return ProcessDockResult(target, drop);
    }
     else if (pt.x >= cli_size.x - layer_insert_offset &&
              pt.x < cli_size.x - layer_insert_offset + auiLayerInsertPixels)
    {
        int new_layer = wxMax(wxMax(GetMaxLayer(docks, wxAUI_DOCK_RIGHT),
                                    GetMaxLayer(docks, wxAUI_DOCK_TOP)),
                                    GetMaxLayer(docks, wxAUI_DOCK_BOTTOM)) + 1;    
        drop.Dock().Right().
             Layer(new_layer).
             Row(0).
             Position(pt.y - GetDockPixelOffset(drop) - offset.y);
        return ProcessDockResult(target, drop);
    }
     else if (pt.y >= cli_size.y - layer_insert_offset &&
              pt.y < cli_size.y - layer_insert_offset + auiLayerInsertPixels)
    {
        int new_layer = wxMax(wxMax(GetMaxLayer(docks, wxAUI_DOCK_BOTTOM),
                                    GetMaxLayer(docks, wxAUI_DOCK_LEFT)),
                                    GetMaxLayer(docks, wxAUI_DOCK_RIGHT)) + 1;
        drop.Dock().Bottom().
             Layer(new_layer).
             Row(0).
             Position(pt.x - GetDockPixelOffset(drop) - offset.x);
        return ProcessDockResult(target, drop);
    }


    wxDockUIPart* part = HitTest(pt.x, pt.y);


    if (drop.IsToolbar())
    {
        if (!part || !part->m_Dock)
            return false;
        

        // calculate the offset from where the dock begins
        // to the point where the user dropped the pane
        int dock_drop_offset = 0;
        if (part->m_Dock->IsHorizontal())
            dock_drop_offset = pt.x - part->m_Dock->m_Rect.x - offset.x;
             else
            dock_drop_offset = pt.y - part->m_Dock->m_Rect.y - offset.y;


        // toolbars may only be moved in and to fixed-pane docks,
        // otherwise we will try to float the pane.  Also, the pane
        // should float if being dragged over center pane windows
        if (!part->m_Dock->m_Fixed || part->m_Dock->m_DockDirection == wxAUI_DOCK_CENTER)
        {
            if ((m_Flags & wxAUI_MGR_ALLOW_FLOATING) &&
                   (drop.IsFloatable() ||
                    (part->m_Dock->m_DockDirection != wxAUI_DOCK_CENTER &&
                     part->m_Dock->m_DockDirection != wxAUI_DOCK_NONE)))
            {
                drop.Float();
            }
            
            return ProcessDockResult(target, drop);
        }
        
        drop.Dock().
             Direction(part->m_Dock->m_DockDirection).
             Layer(part->m_Dock->m_DockLayer).
             Row(part->m_Dock->m_DockRow).
             Position(dock_drop_offset);

        if (pt.y < part->m_Dock->m_Rect.y + 2 &&
            part->m_Dock->m_Panes.GetCount() > 1)
        {
            int row = drop.m_DockRow;
            DoInsertDockRow(panes, part->m_Dock->m_DockDirection,
                            part->m_Dock->m_DockLayer,
                            part->m_Dock->m_DockRow);
            drop.m_DockRow = row;
        }
        
        if (pt.y > part->m_Dock->m_Rect.y + part->m_Dock->m_Rect.height - 2 &&
            part->m_Dock->m_Panes.GetCount() > 1)
        {
            DoInsertDockRow(panes, part->m_Dock->m_DockDirection,
                            part->m_Dock->m_DockLayer,
                            part->m_Dock->m_DockRow+1);
            drop.m_DockRow = part->m_Dock->m_DockRow+1;
        }

        return ProcessDockResult(target, drop);
    }



    
    if (!part)
        return false;

    if (part->m_Type == wxDockUIPart::typePaneBorder ||
        part->m_Type == wxDockUIPart::typeCaption ||
        part->m_Type == wxDockUIPart::typeGripper ||
        part->m_Type == wxDockUIPart::typePaneButton ||
        part->m_Type == wxDockUIPart::typePane ||
        part->m_Type == wxDockUIPart::typePaneSizer ||
        part->m_Type == wxDockUIPart::typeDockSizer ||
        part->m_Type == wxDockUIPart::typeBackground)
    {
        if (part->m_Type == wxDockUIPart::typeDockSizer)
        {
            if (part->m_Dock->m_Panes.GetCount() != 1)
                return false;
            part = GetPanePart(part->m_Dock->m_Panes.Item(0)->m_Window);
            if (!part)
                return false;
        }



        // If a normal frame is being dragged over a toolbar, insert it
        // along the edge under the toolbar, but over all other panes.
        // (this could be done much better, but somehow factoring this
        // calculation with the one at the beginning of this function)
        if (part->m_Dock && part->m_Dock->m_Toolbar)
        {
            int layer = 0;

            switch (part->m_Dock->m_DockDirection)
            {
                case wxAUI_DOCK_LEFT:
                    layer = wxMax(wxMax(GetMaxLayer(docks, wxAUI_DOCK_LEFT),
                                      GetMaxLayer(docks, wxAUI_DOCK_BOTTOM)),
                                      GetMaxLayer(docks, wxAUI_DOCK_TOP));
                    break;
                case wxAUI_DOCK_TOP:
                    layer = wxMax(wxMax(GetMaxLayer(docks, wxAUI_DOCK_TOP),
                                      GetMaxLayer(docks, wxAUI_DOCK_LEFT)),
                                      GetMaxLayer(docks, wxAUI_DOCK_RIGHT));
                    break;
                case wxAUI_DOCK_RIGHT:
                    layer = wxMax(wxMax(GetMaxLayer(docks, wxAUI_DOCK_RIGHT),
                                      GetMaxLayer(docks, wxAUI_DOCK_TOP)),
                                      GetMaxLayer(docks, wxAUI_DOCK_BOTTOM));
                    break;
                case wxAUI_DOCK_BOTTOM:
                    layer = wxMax(wxMax(GetMaxLayer(docks, wxAUI_DOCK_BOTTOM),
                                      GetMaxLayer(docks, wxAUI_DOCK_LEFT)),
                                      GetMaxLayer(docks, wxAUI_DOCK_RIGHT));
                    break;
            }

            DoInsertDockRow(panes, part->m_Dock->m_DockDirection,
                            layer, 0);
            drop.Dock().
                 Direction(part->m_Dock->m_DockDirection).
                 Layer(layer).Row(0).Position(0);
            return ProcessDockResult(target, drop);
        }


        if (!part->m_Pane)
            return false;

        part = GetPanePart(part->m_Pane->m_Window);
        if (!part)
            return false;
            
        bool insert_dock_row = false;
        int insert_row = part->m_Pane->m_DockRow;
        int insert_dir = part->m_Pane->m_DockDirection;
        int insert_layer = part->m_Pane->m_DockLayer;
        
        switch (part->m_Pane->m_DockDirection)
        {
            case wxAUI_DOCK_TOP:
                if (pt.y >= part->m_Rect.y &&
                    pt.y < part->m_Rect.y+auiInsertRowPixels)
                        insert_dock_row = true;
                break;
            case wxAUI_DOCK_BOTTOM:
                if (pt.y > part->m_Rect.y+part->m_Rect.height-auiInsertRowPixels &&
                    pt.y <= part->m_Rect.y + part->m_Rect.height)
                        insert_dock_row = true;
                break;
            case wxAUI_DOCK_LEFT:
                if (pt.x >= part->m_Rect.x &&
                    pt.x < part->m_Rect.x+auiInsertRowPixels)
                        insert_dock_row = true;
                break;
            case wxAUI_DOCK_RIGHT:
                if (pt.x > part->m_Rect.x+part->m_Rect.width-auiInsertRowPixels &&
                    pt.x <= part->m_Rect.x+part->m_Rect.width)
                        insert_dock_row = true;
                break;
            case wxAUI_DOCK_CENTER:
            {
                // "new row pixels" will be set to the default, but
                // must never exceed 20% of the window size
                int new_row_pixels_x = auiNewRowPixels;
                int new_row_pixels_y = auiNewRowPixels;

                if (new_row_pixels_x > (part->m_Rect.width*20)/100)
                    new_row_pixels_x = (part->m_Rect.width*20)/100;

                if (new_row_pixels_y > (part->m_Rect.height*20)/100)
                    new_row_pixels_y = (part->m_Rect.height*20)/100;

                
                // determine if the mouse pointer is in a location that
                // will cause a new row to be inserted.  The hot spot positions
                // are along the borders of the center pane

                insert_layer = 0;
                insert_dock_row = true;
                if (pt.x >= part->m_Rect.x &&
                    pt.x < part->m_Rect.x+new_row_pixels_x)
                        insert_dir = wxAUI_DOCK_LEFT;
                 else
                if (pt.y >= part->m_Rect.y &&
                    pt.y < part->m_Rect.y+new_row_pixels_y)
                        insert_dir = wxAUI_DOCK_TOP;
                 else
                if (pt.x >= part->m_Rect.x + part->m_Rect.width-new_row_pixels_x &&
                    pt.x < part->m_Rect.x + part->m_Rect.width)
                        insert_dir = wxAUI_DOCK_RIGHT;
                 else
                if (pt.y >= part->m_Rect.y+ part->m_Rect.height-new_row_pixels_y &&
                    pt.y < part->m_Rect.y + part->m_Rect.height)
                        insert_dir = wxAUI_DOCK_BOTTOM;
                 else
                return false;

                insert_row = GetMaxRow(panes, insert_dir, insert_layer) + 1;
            }
        }

        if (insert_dock_row)
        { 
            DoInsertDockRow(panes, insert_dir, insert_layer, insert_row);
            drop.Dock().Direction(insert_dir).
                        Layer(insert_layer).
                        Row(insert_row).
                        Position(0);
            return ProcessDockResult(target, drop);
        }
        
        // determine the mouse offset and the pane size, both in the
        // direction of the dock itself, and perpendicular to the dock
        
        int offset, size;
        
        if (part->m_Orientation == wxVERTICAL)
        {
            offset = pt.y - part->m_Rect.y;
            size = part->m_Rect.GetHeight(); 
        }
         else
        {
            offset = pt.x - part->m_Rect.x;
            size = part->m_Rect.GetWidth();
        }
        
        int drop_position = part->m_Pane->m_DockPos;
        
        // if we are in the top/left part of the pane,
        // insert the pane before the pane being hovered over
        if (offset <= size/2)
        {
            drop_position = part->m_Pane->m_DockPos;
            DoInsertPane(panes,
                         part->m_Pane->m_DockDirection,
                         part->m_Pane->m_DockLayer,
                         part->m_Pane->m_DockRow,
                         part->m_Pane->m_DockPos);
        }

        // if we are in the bottom/right part of the pane,
        // insert the pane before the pane being hovered over
        if (offset > size/2)
        {
            drop_position = part->m_Pane->m_DockPos+1;
            DoInsertPane(panes,
                         part->m_Pane->m_DockDirection,
                         part->m_Pane->m_DockLayer,
                         part->m_Pane->m_DockRow,
                         part->m_Pane->m_DockPos+1);
        }

        drop.Dock().
             Direction(part->m_Dock->m_DockDirection).
             Layer(part->m_Dock->m_DockLayer).
             Row(part->m_Dock->m_DockRow).
             Position(drop_position);
        return ProcessDockResult(target, drop);
    }

    return false;
}


void wxFrameManager::OnHintFadeTimer(wxTimerEvent& event)
{
#ifdef __WXMSW__
    if (!m_HintWnd || m_HintFadeAmount >= 50)
    {
        m_HintFadeTimer.Stop();
        return;
    }
    
    m_HintFadeAmount += 5;
    MakeWindowTransparent(m_HintWnd, m_HintFadeAmount);
#endif
}

void wxFrameManager::ShowHint(const wxRect& rect)
{
    #ifdef __WXMSW__
    
    // First, determine if the operating system can handle transparency.
    // Transparency is available on Win2000 and above
    
    static int os_type = -1;
    static int ver_major = -1;
    
    if (os_type == -1)
        os_type = ::wxGetOsVersion(&ver_major);

    // If the transparent flag is set, and the OS supports it,
    // go ahead and use a transparent hint
    
    if ((m_Flags & wxAUI_MGR_TRANSPARENT_HINT) != 0 &&
        os_type == wxOS_WINDOWS_NT && ver_major >= 5)
    {
        if (m_LastHint == rect)
            return;
        m_LastHint = rect;
        
        int initial_fade = 50;
        if (m_Flags & wxAUI_MGR_TRANSPARENT_HINT_FADE)
            initial_fade = 0;
            
        if (m_HintWnd == NULL)
        {
            wxPoint pt = rect.GetPosition();
            wxSize size = rect.GetSize();
            m_HintWnd = new wxFrame(m_Frame, -1, wxEmptyString, pt, size,
                                     wxFRAME_TOOL_WINDOW |
                                     wxFRAME_FLOAT_ON_PARENT |
                                     wxFRAME_NO_TASKBAR |
                                     wxNO_BORDER);

            MakeWindowTransparent(m_HintWnd, initial_fade);
            m_HintWnd->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION));
            m_HintWnd->Show();

            // if we are dragging a floating pane, set the focus
            // back to that floating pane (otherwise it becomes unfocused)
            if (m_Action == actionDragFloatingPane && m_ActionWindow)
                m_ActionWindow->SetFocus();

        }
         else
        {
            wxPoint pt = rect.GetPosition();
            wxSize size = rect.GetSize();
            MakeWindowTransparent(m_HintWnd, initial_fade);
            m_HintWnd->SetSize(pt.x, pt.y, rect.width, rect.height);
        }
        
        if (m_Flags & wxAUI_MGR_TRANSPARENT_HINT_FADE)
        {
            // start fade in timer
            m_HintFadeAmount = 0;
            m_HintFadeTimer.SetOwner(this, 101);
            m_HintFadeTimer.Start(5);
        }
        
        return;
    }
    #endif

    if (m_LastHint != rect)
    {
        // remove the last hint rectangle
        m_LastHint = rect;
        m_Frame->Refresh();
        m_Frame->Update();
    }
    
    wxScreenDC screendc;
    wxRegion clip(1, 1, 10000, 10000);

    // clip all floating windows, so we don't draw over them
    int i, pane_count;
    for (i = 0, pane_count = m_PanesArray.GetCount(); i < pane_count; ++i)
    {
        wxPaneInfo& pane = m_PanesArray.Item(i);

        if (pane.IsFloating() &&
            pane.m_Frame->IsShown())
        {
            wxRect rect = pane.m_Frame->GetRect();
            #ifdef __WXGTK__
            // wxGTK returns the client size, not the whole frame size
            rect.width += 15;
            rect.height += 35;
            rect.Inflate(5);
            #endif

            clip.Subtract(rect);
        }
    }

    screendc.SetClippingRegion(clip);

    wxBitmap stipple = wxPaneCreateStippleBitmap();
    wxBrush brush(stipple);
    screendc.SetBrush(brush);
    screendc.SetPen(*wxTRANSPARENT_PEN);

    screendc.DrawRectangle(rect.x, rect.y, 5, rect.height);
    screendc.DrawRectangle(rect.x+5, rect.y, rect.width-10, 5);
    screendc.DrawRectangle(rect.x+rect.width-5, rect.y, 5, rect.height);
    screendc.DrawRectangle(rect.x+5, rect.y+rect.height-5, rect.width-10, 5);
}

void wxFrameManager::HideHint()
{   
    // hides a transparent window hint (currently wxMSW only)
    #ifdef __WXMSW__
    if (m_HintWnd)
    {
        MakeWindowTransparent(m_HintWnd, 0);
        m_HintFadeTimer.Stop();
        m_LastHint = wxRect();
        return;
    }
    #endif
    
    // hides a painted hint by redrawing the frame window
    if (!m_LastHint.IsEmpty())
    {
        m_Frame->Refresh();
        m_Frame->Update();
        m_LastHint = wxRect();
    }
}



// DrawHintRect() draws a drop hint rectangle. First calls DoDrop() to
// determine the exact position the pane would be at were if dropped.  If
// the pame would indeed become docked at the specified drop point,
// DrawHintRect() then calls ShowHint() to indicate this drop rectangle.
// "pane_window" is the window pointer of the pane being dragged, pt is
// the mouse position, in client coordinates
void wxFrameManager::DrawHintRect(wxWindow* pane_window,
                                  const wxPoint& pt,
                                  const wxPoint& offset)
{
    wxRect rect;

    // we need to paint a hint rectangle; to find out the exact hint rectangle,
    // we will create a new temporary layout and then measure the resulting
    // rectangle; we will create a copy of the docking structures (m_dock)
    // so that we don't modify the real thing on screen

    int i, pane_count, part_count;
    wxDockInfoArray docks;
    wxPaneInfoArray panes;
    wxDockUIPartArray uiparts;
    wxPaneInfo hint = GetPane(pane_window);
    hint.m_Name = wxT("__HINT__");

    if (!hint.IsOk())
        return;

    CopyDocksAndPanes(docks, panes, m_Docks, m_PanesArray);

    // remove any pane already there which bears the same window;
    // this happens when you are moving a pane around in a dock
    for (i = 0, pane_count = panes.GetCount(); i < pane_count; ++i)
    {
        if (panes.Item(i).m_Window == pane_window)
        {
            RemovePaneFromDocks(docks, panes.Item(i));
            panes.RemoveAt(i);
            break;
        }
    }

    // find out where the new pane would be
    if (!DoDrop(docks, panes, hint, pt, offset))
    {
        HideHint();
        return;
    }

    panes.Add(hint);

    wxSizer* sizer = LayoutAll(panes, docks, uiparts, true);
    wxSize client_size = m_Frame->GetClientSize();
    sizer->SetDimension(0, 0, client_size.x, client_size.y);
    sizer->Layout();

    for (i = 0, part_count = uiparts.GetCount();
         i < part_count; ++i)
    {
        wxDockUIPart& part = uiparts.Item(i);

        if (part.m_Type == wxDockUIPart::typePaneBorder &&
            part.m_Pane && part.m_Pane->m_Name == wxT("__HINT__"))
        {
            rect = wxRect(part.m_SizerItem->GetPosition(),
                          part.m_SizerItem->GetSize());
            break;
        }
    }
    
    delete sizer;

    if (rect.IsEmpty())
    {
        HideHint();
        return;
    }

    // actually show the hint rectangle on the screen
    m_Frame->ClientToScreen(&rect.x, &rect.y);
    ShowHint(rect);
}

void wxFrameManager::OnFloatingPaneMoveStart(wxWindow* wnd)
{
    // try to find the pane
    wxPaneInfo& pane = GetPane(wnd);
    wxASSERT_MSG(pane.IsOk(), wxT("Pane window not found"));
    
    #ifdef __WXMSW__
    if (m_Flags & wxAUI_MGR_TRANSPARENT_DRAG)
        MakeWindowTransparent(pane.m_Frame, 150);
    #endif
}

void wxFrameManager::OnFloatingPaneMoving(wxWindow* wnd)
{
    // try to find the pane
    wxPaneInfo& pane = GetPane(wnd);
    wxASSERT_MSG(pane.IsOk(), wxT("Pane window not found"));
    
    wxPoint pt = ::wxGetMousePosition();
    wxPoint client_pt = m_Frame->ScreenToClient(pt);
    
    // calculate the offset from the upper left-hand corner
    // of the frame to the mouse pointer
    wxPoint frame_pos = pane.m_Frame->GetPosition();
    wxPoint action_offset(pt.x-frame_pos.x, pt.y-frame_pos.y);

    // no hint for toolbar floating windows
    if (pane.IsToolbar() && m_Action == actionDragFloatingPane)
    {
        if (m_Action == actionDragFloatingPane)
        {
            wxDockInfoArray docks;
            wxPaneInfoArray panes;
            wxDockUIPartArray uiparts;
            wxPaneInfo hint = pane;
            
            CopyDocksAndPanes(docks, panes, m_Docks, m_PanesArray);

            // find out where the new pane would be
            if (!DoDrop(docks, panes, hint, client_pt))
                return;
            if (hint.IsFloating())
                return;
            
            pane = hint;
            m_Action = actionDragToolbarPane;
            m_ActionWindow = pane.m_Window;
            
            Update();
        }
        
        return;
    }


    // if a key modifier is pressed while dragging the frame,
    // don't dock the window
    if (wxGetKeyState(WXK_CONTROL) || wxGetKeyState(WXK_ALT))
    {
        HideHint();
        return;
    }


    DrawHintRect(wnd, client_pt, action_offset);

    #ifdef __WXGTK__
    // this cleans up some screen artifacts that are caused on GTK because
    // we aren't getting the exact size of the window (see comment
    // in DrawHintRect)
    //Refresh();
    #endif
    
    
    // reduces flicker
    ////////////////////////////////m_Frame->Update();   //SIL. 24-may-2006 : -- boh - l'ho tolto e funziona meglio :-)
}

void wxFrameManager::OnFloatingPaneMoved(wxWindow* wnd)
{
    // try to find the pane
    wxPaneInfo& pane = GetPane(wnd);
    wxASSERT_MSG(pane.IsOk(), wxT("Pane window not found"));
    
    wxPoint pt = ::wxGetMousePosition();
    wxPoint client_pt = m_Frame->ScreenToClient(pt);
    
    // calculate the offset from the upper left-hand corner
    // of the frame to the mouse pointer
    wxPoint frame_pos = pane.m_Frame->GetPosition();
    wxPoint action_offset(pt.x-frame_pos.x, pt.y-frame_pos.y);
    

    // if a key modifier is pressed while dragging the frame,
    // don't dock the window
    if (wxGetKeyState(WXK_CONTROL) || wxGetKeyState(WXK_ALT))
    {
        HideHint();
        return;
    }


    // do the drop calculation
    DoDrop(m_Docks, m_PanesArray, pane, client_pt, action_offset);

    // if the pane is still floating, update it's floating
    // position (that we store)
    if (pane.IsFloating())
    {
        pane.m_FloatingPos = pane.m_Frame->GetPosition();
        
        #ifdef __WXMSW__
        if (m_Flags & wxAUI_MGR_TRANSPARENT_DRAG)
            MakeWindowTransparent(pane.m_Frame, 255);
        #endif
    }
        
    Update();
    
    HideHint();
}

void wxFrameManager::OnFloatingPaneResized(wxWindow* wnd, const wxSize& size)
{
    // try to find the pane
    wxPaneInfo& pane = GetPane(wnd);
    wxASSERT_MSG(pane.IsOk(), wxT("Pane window not found"));
    
    pane.m_FloatingSize = size;
}

void wxFrameManager::OnFloatingPaneClosed(wxWindow* wnd)
{
    // try to find the pane
    wxPaneInfo& pane = GetPane(wnd);
    wxASSERT_MSG(pane.IsOk(), wxT("Pane window not found"));

    // reparent the pane window back to us and
    // prepare the frame window for destruction
    pane.m_Window->Show(false);
    pane.m_Window->Reparent(m_Frame);
    pane.m_Frame = NULL;
    pane.Hide();
    Update(); // Paolo 09/11/2006 bug fix to update the check into the MenuBar
}

void wxFrameManager::OnFloatingPaneActivated(wxWindow* wnd)
{
    if (GetFlags() & wxAUI_MGR_ALLOW_ACTIVE_PANE)
    {
        // try to find the pane
        wxPaneInfo& pane = GetPane(wnd);
        wxASSERT_MSG(pane.IsOk(), wxT("Pane window not found"));

        SetActivePane(m_PanesArray, wnd);
        Repaint();
    }
}

// Render() draws all of the pane captions, sashes,
// backgrounds, captions, grippers, pane borders and buttons.
// It renders the entire user interface.

void wxFrameManager::Render(wxDC* dc)
{
#ifdef __WXMAC__
    dc->Clear() ;
#endif
    int i, part_count;
    for (i = 0, part_count = m_UIParts.GetCount();
         i < part_count; ++i)
    {
        wxDockUIPart& part = m_UIParts.Item(i);

        // don't draw hidden pane items
        //if (part.sizer_item && !part.sizer_item->IsShown())
        if (part.m_Pane && !part.m_Pane->IsShown()) // Paolo 09/11/2006: bug fix on quit application
            continue;                           // with floating panels: avoid crashes
        
        switch (part.m_Type)
        {
            case wxDockUIPart::typeDockSizer:
            case wxDockUIPart::typePaneSizer:
                m_Art->DrawSash(*dc, part.m_Orientation, part.m_Rect);
                break;
            case wxDockUIPart::typeBackground:
                m_Art->DrawBackground(*dc, part.m_Orientation, part.m_Rect);
                break;
            case wxDockUIPart::typeCaption:
                m_Art->DrawCaption(*dc, part.m_Pane->m_Caption, part.m_Rect, *part.m_Pane);
                break;
            case wxDockUIPart::typeGripper:
                m_Art->DrawGripper(*dc, part.m_Rect, *part.m_Pane);
                break;     
            case wxDockUIPart::typePaneBorder:
                m_Art->DrawBorder(*dc, part.m_Rect, *part.m_Pane);
                break;
            case wxDockUIPart::typePaneButton:
                m_Art->DrawPaneButton(*dc, part.m_Button->m_ButtonId,
                        wxAUI_BUTTON_STATE_NORMAL, part.m_Rect, *part.m_Pane);
                break;
        }
    }
}

void wxFrameManager::Repaint(wxDC* dc)
{
#ifdef __WXMAC__
    if ( dc == NULL )
    {
        m_Frame->Refresh() ;
        m_Frame->Update() ;
        return ;
    }
#endif
    int w, h;
    m_Frame->GetClientSize(&w, &h);

    // figure out which dc to use; if one
    // has been specified, use it, otherwise
    // make a client dc
    wxClientDC* client_dc = NULL;
    if (!dc)
    {
        client_dc = new wxClientDC(m_Frame);
        dc = client_dc;
    }

    // if the frame has a toolbar, the client area
    // origin will not be (0,0).
    wxPoint pt = m_Frame->GetClientAreaOrigin();
    if (pt.x != 0 || pt.y != 0)
        dc->SetDeviceOrigin(pt.x, pt.y);

    // render all the items
    Render(dc);

    // if we created a client_dc, delete it
    if (client_dc)
        delete client_dc;
}

void wxFrameManager::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(m_Frame);
    Repaint(&dc);
}

void wxFrameManager::OnEraseBackground(wxEraseEvent& event)
{
#ifdef __WXMAC__
    event.Skip() ;
#endif
}

void wxFrameManager::OnSize(wxSizeEvent& event)
{
    if (m_Frame)
    {
      ((albaGUIMDIFrame *)m_Frame)->OnSize(event);
      DoFrameLayout();
      Repaint();
    }
}


void wxFrameManager::OnSetCursor(wxSetCursorEvent& event)
{
    // determine cursor
    wxDockUIPart* part = HitTest(event.GetX(), event.GetY());
    wxCursor cursor = wxNullCursor;

    if (part)
    {
        if (part->m_Type == wxDockUIPart::typeDockSizer ||
            part->m_Type == wxDockUIPart::typePaneSizer)
        {
            // a dock may not be resized if it has a single
            // pane which is not resizable
            if (part->m_Type == wxDockUIPart::typeDockSizer && part->m_Dock &&
                part->m_Dock->m_Panes.GetCount() == 1 &&
                part->m_Dock->m_Panes.Item(0)->IsFixed())
                    return;

            // panes that may not be resized do not get a sizing cursor
            if (part->m_Pane && part->m_Pane->IsFixed())
                return;

            if (part->m_Orientation == wxVERTICAL)
                cursor = wxCursor(wxCURSOR_SIZEWE);
                 else
                cursor = wxCursor(wxCURSOR_SIZENS);
        }
         else if (part->m_Type == wxDockUIPart::typeGripper)
        {
            cursor = wxCursor(wxCURSOR_SIZING);
        }
    }
    
    event.SetCursor(cursor);
}



void wxFrameManager::UpdateButtonOnScreen(wxDockUIPart* button_ui_part,
                                          const wxMouseEvent& event)
{
    wxDockUIPart* hit_test = HitTest(event.GetX(), event.GetY());

    int state = wxAUI_BUTTON_STATE_NORMAL;
    
    if (hit_test == button_ui_part)
    {
        if (event.LeftDown())
            state = wxAUI_BUTTON_STATE_PRESSED;
             else
            state = wxAUI_BUTTON_STATE_HOVER;
    }
     else
    {
        if (event.LeftDown())
            state = wxAUI_BUTTON_STATE_HOVER;
    }

    // now repaint the button with hover state
    wxClientDC cdc(m_Frame);

    // if the frame has a toolbar, the client area
    // origin will not be (0,0).
    wxPoint pt = m_Frame->GetClientAreaOrigin();
    if (pt.x != 0 || pt.y != 0)
        cdc.SetDeviceOrigin(pt.x, pt.y);

    m_Art->DrawPaneButton(cdc,
              button_ui_part->m_Button->m_ButtonId,
              state,
              button_ui_part->m_Rect,
              *hit_test->m_Pane);
}

void wxFrameManager::OnLeftDown(wxMouseEvent& event)
{
    wxDockUIPart* part = HitTest(event.GetX(), event.GetY());
    if (part)
    {
        if (part->m_Dock && part->m_Dock->m_DockDirection == wxAUI_DOCK_CENTER)
            return;

        if (part->m_Type == wxDockUIPart::typeDockSizer ||
            part->m_Type == wxDockUIPart::typePaneSizer)
        {
            // a dock may not be resized if it has a single
            // pane which is not resizable
            if (part->m_Type == wxDockUIPart::typeDockSizer && part->m_Dock &&
                part->m_Dock->m_Panes.GetCount() == 1 &&
                part->m_Dock->m_Panes.Item(0)->IsFixed())
                    return;

            // panes that may not be resized should be ignored here
            if (part->m_Pane && part->m_Pane->IsFixed())
                return;

            m_Action = actionResize;
            m_ActionPart = part;
            m_ActionHintRect = wxRect();
            m_ActionStart = wxPoint(event.m_x, event.m_y);
            m_ActionOffset = wxPoint(event.m_x - part->m_Rect.x,
                                      event.m_y - part->m_Rect.y);
            m_Frame->CaptureMouse();
        }
         else if (part->m_Type == wxDockUIPart::typePaneButton)
        {
            m_Action = actionClickButton;
            m_ActionPart = part;
            m_ActionStart = wxPoint(event.m_x, event.m_y);
            m_Frame->CaptureMouse();

            UpdateButtonOnScreen(part, event);
        }
         else if (part->m_Type == wxDockUIPart::typeCaption ||
                  part->m_Type == wxDockUIPart::typeGripper)
        {
            if (GetFlags() & wxAUI_MGR_ALLOW_ACTIVE_PANE)
            {
                // set the caption as active
                SetActivePane(m_PanesArray, part->m_Pane->m_Window);
                Repaint();
            }

            m_Action = actionClickCaption;
            m_ActionPart = part;
            m_ActionStart = wxPoint(event.m_x, event.m_y);
            m_ActionOffset = wxPoint(event.m_x - part->m_Rect.x,
                                      event.m_y - part->m_Rect.y);
            m_Frame->CaptureMouse();
        }
#ifdef __WXMAC__
        else
        {
            event.Skip();
        }
#endif
    }
#ifdef __WXMAC__
    else
    {
        event.Skip();
    }
#else
    event.Skip();
#endif
}


void wxFrameManager::OnLeftUp(wxMouseEvent& event)
{
    if (m_Action == actionResize)
    {
        m_Frame->ReleaseMouse();

        // get rid of the hint rectangle
        wxScreenDC dc;
        DrawResizeHint(dc, m_ActionHintRect);

        // resize the dock or the pane
        if (m_ActionPart && m_ActionPart->m_Type==wxDockUIPart::typeDockSizer)
        {
            wxRect& rect = m_ActionPart->m_Dock->m_Rect;

            wxPoint new_pos(event.m_x - m_ActionOffset.x,
                            event.m_y - m_ActionOffset.y);

            switch (m_ActionPart->m_Dock->m_DockDirection)
            {
                case wxAUI_DOCK_LEFT:
                    m_ActionPart->m_Dock->m_Size = new_pos.x - rect.x;
                    break;
                case wxAUI_DOCK_TOP:
                    m_ActionPart->m_Dock->m_Size = new_pos.y - rect.y;
                    break;
                case wxAUI_DOCK_RIGHT:
                    m_ActionPart->m_Dock->m_Size = rect.x + rect.width -
                                  new_pos.x - m_ActionPart->m_Rect.GetWidth();
                    break;
                case wxAUI_DOCK_BOTTOM:
                    m_ActionPart->m_Dock->m_Size = rect.y + rect.height -
                                  new_pos.y - m_ActionPart->m_Rect.GetHeight();
                    break;
            }

            Update();
            Repaint(NULL);
        }
         else if (m_ActionPart &&
                  m_ActionPart->m_Type == wxDockUIPart::typePaneSizer)
        {
            wxDockInfo& dock = *m_ActionPart->m_Dock;
            wxPaneInfo& pane = *m_ActionPart->m_Pane;

            int total_proportion = 0;
            int dock_pixels = 0;
            int new_pixsize = 0;

            int caption_size = m_Art->GetMetric(wxAUI_ART_CAPTION_SIZE);
            int pane_border_size = m_Art->GetMetric(wxAUI_ART_PANE_BORDER_SIZE);
            int sash_size = m_Art->GetMetric(wxAUI_ART_SASH_SIZE);

            wxPoint new_pos(event.m_x - m_ActionOffset.x,
                            event.m_y - m_ActionOffset.y);

            // determine the pane rectangle by getting the pane part
            wxDockUIPart* pane_part = GetPanePart(pane.m_Window);
            wxASSERT_MSG(pane_part,
                       wxT("Pane border part not found -- shouldn't happen"));

            // determine the new pixel size that the user wants;
            // this will help us recalculate the pane's proportion
            if (dock.IsHorizontal())
                new_pixsize = new_pos.x - pane_part->m_Rect.x;
                 else
                new_pixsize = new_pos.y - pane_part->m_Rect.y;

            // determine the size of the dock, based on orientation
            if (dock.IsHorizontal())
                dock_pixels = dock.m_Rect.GetWidth();
                 else
                dock_pixels = dock.m_Rect.GetHeight();

            // determine the total proportion of all resizable panes,
            // and the total size of the dock minus the size of all
            // the fixed panes
            int i, dock_pane_count = dock.m_Panes.GetCount();
            int pane_position = -1;
            for (i = 0; i < dock_pane_count; ++i)
            {
                wxPaneInfo& p = *dock.m_Panes.Item(i);
                if (p.m_Window == pane.m_Window)
                    pane_position = i;
                
                // while we're at it, subtract the pane sash
                // width from the dock width, because this would
                // skew our proportion calculations
                if (i > 0)
                    dock_pixels -= sash_size;
             
                // also, the whole size (including decorations) of
                // all fixed panes must also be subtracted, because they
                // are not part of the proportion calculation
                if (p.IsFixed())
                {
                    if (dock.IsHorizontal())
                        dock_pixels -= p.m_BestSize.x;
                         else
                        dock_pixels -= p.m_BestSize.y;
                }
                 else
                {
                    total_proportion += p.m_DockProportion;
                }
            }
            
            // find a pane in our dock to 'steal' space from or to 'give'
            // space to -- this is essentially what is done when a pane is
            // resized; the pane should usually be the first non-fixed pane
            // to the right of the action pane
            int borrow_pane = -1;
            for (i = pane_position+1; i < dock_pane_count; ++i)
            {
                wxPaneInfo& p = *dock.m_Panes.Item(i);
                if (!p.IsFixed())
                {
                    borrow_pane = i;
                    break;
                }
            }
            
            
            // demand that the pane being resized is found in this dock
            // (this assert really never should be raised)
            wxASSERT_MSG(pane_position != -1, wxT("Pane not found in dock"));
            
            // prevent division by zero
            if (dock_pixels == 0 || total_proportion == 0 || borrow_pane == -1)
            {
                m_Action = actionNone;
                return;
            }

            // calculate the new proportion of the pane
            int new_proportion = (new_pixsize*total_proportion)/dock_pixels;
            
            // default minimum size
            int min_size = 0;
            
            // check against the pane's minimum size, if specified. please note
            // that this is not enough to ensure that the minimum size will
            // not be violated, because the whole frame might later be shrunk,
            // causing the size of the pane to violate it's minimum size
            if (pane.m_MinSize.IsFullySpecified())
            {
                min_size = 0;
                
                if (pane.HasBorder())
                    min_size += (pane_border_size*2);

                // calculate minimum size with decorations (border,caption)
                if (pane_part->m_Orientation == wxVERTICAL)
                {
                    min_size += pane.m_MinSize.y;
                    if (pane.HasCaption())
                        min_size += caption_size;
                }
                 else
                {
                    min_size += pane.m_MinSize.x;
                }
            }
            
            
            // for some reason, an arithmatic error somewhere is causing
            // the proportion calculations to always be off by 1 pixel;
            // for now we will add the 1 pixel on, but we really should
            // determine what's causing this.
            min_size++;

            int min_proportion = (min_size*total_proportion)/dock_pixels;
                
            if (new_proportion < min_proportion)
                new_proportion = min_proportion;
            
            
            
            int prop_diff = new_proportion - pane.m_DockProportion;

            // borrow the space from our neighbor pane to the
            // right or bottom (depending on orientation)
            dock.m_Panes.Item(borrow_pane)->m_DockProportion -= prop_diff;
            pane.m_DockProportion = new_proportion;
            
            // repaint
            Update();
            Repaint(NULL);
        }
    }
     else if (m_Action == actionClickButton)
    {
        m_HoverButton = NULL;
        m_Frame->ReleaseMouse();     
        UpdateButtonOnScreen(m_ActionPart, event);

        // make sure we're still over the item that was originally clicked
        if (m_ActionPart == HitTest(event.GetX(), event.GetY()))
        { 
            // fire button-click event
            wxFrameManagerEvent e(wxEVT_AUI_PANEBUTTON);
            e.SetPane(m_ActionPart->m_Pane);
            e.SetButton(m_ActionPart->m_Button->m_ButtonId);
            ProcessMgrEvent(e);
        }
    }
     else if (m_Action == actionClickCaption)
    {
        m_Frame->ReleaseMouse();
    }
     else if (m_Action == actionDragFloatingPane)
    {
        m_Frame->ReleaseMouse();
    }
     else if (m_Action == actionDragToolbarPane)
    {
        m_Frame->ReleaseMouse();

        wxPaneInfo& pane = GetPane(m_ActionWindow);
        wxASSERT_MSG(pane.IsOk(), wxT("Pane window not found"));
    
        // save the new positions
        wxDockInfoPtrArray docks;
        FindDocks(m_Docks, pane.m_DockDirection,
                  pane.m_DockLayer, pane.m_DockRow, docks);
        if (docks.GetCount() == 1)
        {
            wxDockInfo& dock = *docks.Item(0);
            
            wxArrayInt pane_positions, pane_sizes;
            GetPanePositionsAndSizes(dock, pane_positions, pane_sizes);
            
            int i, dock_pane_count = dock.m_Panes.GetCount();
            for (i = 0; i < dock_pane_count; ++i)
                dock.m_Panes.Item(i)->m_DockPos = pane_positions[i];
        }
        
        pane.m_State &= ~wxPaneInfo::actionPane;
        Update();
    }
     else
    {
        event.Skip();
    }

    m_Action = actionNone;
    m_LastMouseMove = wxPoint(); // see comment in OnMotion()
}


void wxFrameManager::OnMotion(wxMouseEvent& event)
{
    // sometimes when Update() is called from inside this method,
    // a spurious mouse move event is generated; this check will make
    // sure that only real mouse moves will get anywhere in this method;
    // this appears to be a bug somewhere, and I don't know where the
    // mouse move event is being generated.  only verified on MSW
    
    wxPoint mouse_pos = event.GetPosition();
    if (m_LastMouseMove == mouse_pos)
        return;
    m_LastMouseMove = mouse_pos;

    
    if (m_Action == actionResize)
    {
        wxPoint pos = m_ActionPart->m_Rect.GetPosition();
        if (m_ActionPart->m_Orientation == wxHORIZONTAL)
            pos.y = wxMax(0, event.m_y - m_ActionOffset.y);
             else
            pos.x = wxMax(0, event.m_x - m_ActionOffset.x);

        wxRect rect(m_Frame->ClientToScreen(pos),
                    m_ActionPart->m_Rect.GetSize());

        wxScreenDC dc;
        if (!m_ActionHintRect.IsEmpty())
            DrawResizeHint(dc, m_ActionHintRect);
        DrawResizeHint(dc, rect);
        m_ActionHintRect = rect;
    }
     else if (m_Action == actionClickCaption)
    {
        int drag_x_threshold = wxSystemSettings::GetMetric(wxSYS_DRAG_X);
        int drag_y_threshold = wxSystemSettings::GetMetric(wxSYS_DRAG_Y);
        
        // caption has been clicked.  we need to check if the mouse
        // is now being dragged. if it is, we need to change the
        // mouse action to 'drag'
        if (abs(event.m_x - m_ActionStart.x) > drag_x_threshold ||
            abs(event.m_y - m_ActionStart.y) > drag_y_threshold)     
        {
            wxPaneInfo* pane_info = m_ActionPart->m_Pane;

            if (!pane_info->IsToolbar())
            {
                if ((m_Flags & wxAUI_MGR_ALLOW_FLOATING) &&
                    pane_info->IsFloatable()) 
                {
                    m_Action = actionDragFloatingPane;

                    // set initial float position
                    wxPoint pt = m_Frame->ClientToScreen(event.GetPosition());
                    pane_info->m_FloatingPos = wxPoint(pt.x - m_ActionOffset.x,
                                                      pt.y - m_ActionOffset.y);
                    // float the window
                    pane_info->Float();
                    Update();

                    m_ActionWindow = pane_info->m_Frame;
                    
                    // action offset is used here to make it feel "natural" to the user
                    // to drag a docked pane and suddenly have it become a floating frame.
                    // Sometimes, however, the offset where the user clicked on the docked
                    // caption is bigger than the width of the floating frame itself, so
                    // in that case we need to set the action offset to a sensible value
                    wxSize frame_size = m_ActionWindow->GetSize();
                    if (frame_size.x <= m_ActionOffset.x)
                        m_ActionOffset.x = 30;
                }
            }
             else
            {
                m_Action = actionDragToolbarPane;
                m_ActionWindow = pane_info->m_Window;
            }
        }
    }
     else if (m_Action == actionDragFloatingPane)
    {
        wxPoint pt = m_Frame->ClientToScreen(event.GetPosition());
        m_ActionWindow->Move(pt.x - m_ActionOffset.x,
                             pt.y - m_ActionOffset.y);
    }
     else if (m_Action == actionDragToolbarPane)
    {
        wxPaneInfo& pane = GetPane(m_ActionWindow);
        wxASSERT_MSG(pane.IsOk(), wxT("Pane window not found"));

        pane.m_State |= wxPaneInfo::actionPane;

        wxPoint pt = event.GetPosition();
        DoDrop(m_Docks, m_PanesArray, pane, pt, m_ActionOffset);
        
        // if DoDrop() decided to float the pane, set up
        // the floating pane's initial position
        if (pane.IsFloating())
        {
            wxPoint pt = m_Frame->ClientToScreen(event.GetPosition());
            pane.m_FloatingPos = wxPoint(pt.x - m_ActionOffset.x,
                                        pt.y - m_ActionOffset.y);
        }
        
        // this will do the actiual move operation;
        // in the case that the pane has been floated,
        // this call will create the floating pane
        // and do the reparenting
        Update();
        
        // if the pane has been floated, change the mouse
        // action actionDragFloatingPane so that subsequent
        // EVT_MOTION() events will move the floating pane
        if (pane.IsFloating())
        {
            pane.m_State &= ~wxPaneInfo::actionPane;
            m_Action = actionDragFloatingPane;
            m_ActionWindow = pane.m_Frame;
        }
    } 
     else
    {
        wxDockUIPart* part = HitTest(event.GetX(), event.GetY());
        if (part && part->m_Type == wxDockUIPart::typePaneButton)
        {
            if (part != m_HoverButton)
            {
                // make the old button normal
                if (m_HoverButton)
                    UpdateButtonOnScreen(m_HoverButton, event);

                // mouse is over a button, so repaint the
                // button in hover mode
                UpdateButtonOnScreen(part, event);
                m_HoverButton = part;
            }
        }
         else
        {
            if (m_HoverButton)
            {
                m_HoverButton = NULL;
                Repaint();
            }
             else
            {
                event.Skip();
            }
        }
    }
}

void wxFrameManager::OnLeaveWindow(wxMouseEvent& event)
{
    if (m_HoverButton)
    {
        m_HoverButton = NULL;
        Repaint();
    }
}

void wxFrameManager::OnChildFocus(wxChildFocusEvent& event)
{
    // when a child pane has it's focus set, we should change the 
    // pane's active state to reflect this. (this is only true if 
    // active panes are allowed by the owner)
    if (GetFlags() & wxAUI_MGR_ALLOW_ACTIVE_PANE)
    {
        if (GetPane(event.GetWindow()).IsOk())
        {
            SetActivePane(m_PanesArray, event.GetWindow());
            m_Frame->Refresh();
        }
    }
}


// OnPaneButton() is an event handler that is called
// when a pane button has been pressed.
void wxFrameManager::OnPaneButton(wxFrameManagerEvent& event)
{
    wxPaneInfo& pane = *(event.m_Pane);
    
    if (event.m_Button == wxPaneInfo::buttonClose)
    {
        pane.Hide();
        Update();
    }
     else if (event.m_Button == wxPaneInfo::buttonPin)
    {
        if ((m_Flags & wxAUI_MGR_ALLOW_FLOATING) &&
            pane.IsFloatable())
                pane.Float();
        Update();
    }
}














//--------------------------------------------------------------------------
bool albaGUIDockManager::AddPane(wxWindow* window,const wxPaneInfo& pane_info, const wxString &menu, const wxString &subMenu)
//--------------------------------------------------------------------------
{
  AddMenuItem(window,pane_info.m_Caption, menu, subMenu);
  return wxFrameManager::AddPane(window,pane_info);
}
//--------------------------------------------------------------------------
bool albaGUIDockManager::AddPane(wxWindow* window,int direction,const wxString& caption)
//--------------------------------------------------------------------------
{
  AddMenuItem(window,caption);
  return wxFrameManager::AddPane(window,direction,caption);
}
//--------------------------------------------------------------------------
void albaGUIDockManager::Update()
//--------------------------------------------------------------------------
{
  wxFrameManager::Update();
  UpdateMenuItems();
}
//--------------------------------------------------------------------------
void albaGUIDockManager::AddMenuItem(wxWindow* window,const wxString& caption , const wxString &menu, const wxString &subMenu)
//--------------------------------------------------------------------------
{
  if (!caption.IsEmpty())
  {
    wxMenuBar* menubar = this->m_Frame->GetMenuBar();
    if ( menubar )
    {
      int idx = menubar->FindMenu(menu);
      if( idx != wxNOT_FOUND)
      {
        wxMenu *m = menubar->GetMenu(idx);
				if(subMenu != wxEmptyString)
				{
					int ids = m->FindItem(subMenu);
					wxMenuItem *sm = m->FindItem(ids);
					if( ids != wxNOT_FOUND && sm->GetSubMenu())
					{		
						sm->GetSubMenu()->Append(window->GetId(), caption, "", wxITEM_CHECK);
					}
				}
				else
				{
          m->Append(window->GetId(), caption, "", wxITEM_CHECK);
				}
        //m_MenuBar->Check(id,);
      }
    }
  }
}
//--------------------------------------------------------------------------
void albaGUIDockManager::UpdateMenuItems()
//--------------------------------------------------------------------------
{
  wxMenuBar* menubar = this->m_Frame->GetMenuBar();
  if ( !menubar ) return;

  int i, pane_count = m_PanesArray.GetCount();
  for (i = 0; i < pane_count; ++i)
  {
    wxPaneInfo& p = m_PanesArray.Item(i);
    if( menubar->FindItem(p.m_Window->GetId() ) )
      menubar->Check(p.m_Window->GetId(), p.IsShown());
  }
}
