/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgGui.h,v $
  Language:  C++
  Date:      $Date: 2005-04-29 06:05:55 $
  Version:   $Revision: 1.9 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2005
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mmgGui_H__
#define __mmgGui_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "wx/grid.h"
#include "mafDecl.h"
#include "mmgPanel.h"
#include "mafEvent.h"
//----------------------------------------------------------------------------
// class forward :
//----------------------------------------------------------------------------
class mmgFloatSlider;
class mmgCheckListBox;

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

#define   MININT    -32000
#define   MAXINT     32000
#define   MINFLOAT  -2000000000 
#define   MAXFLOAT   2000000000

//----------------------------------------------------------------------------
// Constants to be used with mmgGUI::GetMetrics()
//----------------------------------------------------------------------------
enum mmgGUI_constants
{
  GUI_ROW_MARGIN,
  GUI_LABEL_MARGIN,
  GUI_WIDGET_MARGIN,
  GUI_LABEL_HEIGHT,
  GUI_BUTTON_HEIGHT,
  GUI_LABEL_WIDTH,
  GUI_WIDGET_WIDTH,
  GUI_FULL_WIDTH,
  GUI_DATA_WIDTH
};
//----------------------------------------------------------------------------
// mmgGui :
/**  mmgGui is a panel with function to easily create GUI.
The user calls function like Vector,String,Color ecc.. .
mmgGui take care of: 
- creating and Layout the widgets,
- filter and validate user input, widget are constrained in range and type. 
- manage widget initialization user variables updates. 
- notify the user of gui-events by sending mafEvents to a mafEventListener.

\par implementation details:
On creation of widget, the user passes an ID that will be
used by the widget when sending notification back. 
One command may cause the creation of more than one widget, ex:
the Vector create 3 wxTextCtrl, and I can't use the same ID for every widget.
So user-ID are translated into widgets-ID using 
two member functions : GetModuleID and GetId. GetId increments an internal ID_counter.
\sa GetId GetModuleId m_id m_table
*/
//----------------------------------------------------------------------------
class mmgGui: public mmgPanel, public mafEventListener
{
public:
           mmgGui(mafEventListener *listener);
  virtual ~mmgGui();
    
  /** Set the Listener that will receive event-notification, the Listener can be changed any time  */
  void SetListener(mafEventListener *listener)   {m_Listener=listener;}; 

  /** Separator widget. */
	void Divider(long style=0);

  /** Label widget. */
  void Label(wxString label,bool bold = false, bool multiline = false);

  /** Label widget. */
  void Label(wxString *var ,bool bold = false, bool multiline = false);

  /** Double label widget. */
	void Label(wxString label1,wxString  label2, bool bold = false);

  /** Double label widget. */
  void Label(wxString label1,wxString *var, bool bold = false);

  //---------------------------------------------
  /** Label widget. */
  //void Label(mafString label,bool bold = false, bool multiline = false);

  /** Label widget. */
  //void Label(mafString *var ,bool bold = false, bool multiline = false);

  /** Double label widget. */
  //void Label(mafString label1,mafString  label2, bool bold = false);

  /** Double label widget. */
  //void Label(mafString label1,mafString *var, bool bold = false);
  //---------------------------------------------

  /** String entry widget. */
  void String(int id,wxString label,wxString *var, wxString tooltip = "");

  /** String entry widget. */
  void String(int id,const char *label,mafString *var, const char *tooltip = "");
  
  /** Integer entry widget. */
  void Integer(int id,wxString label,int *var, int min = MININT, int max = MAXINT, wxString tooltip = "");

  /** Float entry widget. */
  void Float(int id,wxString label,float *var, float min = MINFLOAT, float max = MAXFLOAT, int flag=0, int decimal_digit = 2, wxString tooltip = "");

  /** Double entry widget. */
  void Double(int id,wxString label,double *var, double	min = MINFLOAT, double max = MAXFLOAT, int flag=0, int decimal_digit = 2, wxString tooltip = "");

  /** Integer vector3 entry widget. */
  void Vector(int id,wxString label, int var[3], int min = MININT, int max = MAXINT, wxString tooltip = "");

  /** Integer vector3 entry widget. */
  void Vector(int id,wxString label, int var[3], int minx, int maxx, int miny, int maxy, int minz, int maxz, wxString tooltip = "");

  /** Float vector3 entry widget. */
  void Vector(int id,wxString label, float var[3], float min = MINFLOAT, float max = MAXFLOAT, int decimal_digit = 2, wxString tooltip = "");

  /** Float vector3 entry widget. */
  void Vector(int id,wxString label, float var[3], float minx, float maxx, float miny, float maxy, float minz, float maxz, int decimal_digit = 2, wxString tooltip = "");

  /** Double vector3 entry widget. */
  void Vector(int id,wxString label, double var[3], double min = MINFLOAT, double max = MAXFLOAT, int decimal_digit = 2, wxString tooltip = "");

  /** Double vector3 entry widget. */
  void Vector(int id,wxString label, double var[3], double minx, double maxx, double miny, double maxy, double minz, double maxz, int decimal_digit = 2, wxString tooltip = "");

  /** Double vectorN entry widget. */
  void VectorN(int id,wxString label, double *var, int num_elem = 3, double min = MINFLOAT, double max = MAXFLOAT, int decimal_digit = 2, wxString tooltip = "");
  
  /** Int vectorN entry widget. */
  void VectorN(int id,wxString label, int *var,int num_elem = 3,int min = MININT, int max = MAXINT, wxString tooltip = "");

  /** Checkbutton widget. */
  void Bool (int id, wxString label, int *var, int flag = 0, wxString tooltip = ""	);

  /** Radiobutton widget. */
  void Radio(int id,wxString label, int *var, int numchoices = 0, const wxString choices[] = NULL, int dim = 1, wxString tooltip = "");

  /** Combo widget. */
  void Combo(int id,wxString label, int *var, int numchoices = 0, const wxString choices[] = NULL, wxString tooltip = "");

  /** File open dialog widget. */
  void FileOpen(int id,wxString label,wxString *var, const wxString wildcard = "", wxString tooltip = "");

  /** File save dialog widget. */
  void FileSave(int id,wxString label,wxString *var, const wxString wildcard = "", wxString tooltip = "");

  /** Dir Open dialog widget. */
  void DirOpen (int id,wxString label,wxString *var, wxString tooltip = "");

  /** Colour dialog widget. */
  void Color   (int id,wxString label,wxColour *var, wxString tooltip = "");

  /** Button widget. */
  void Button  (int id,wxString button_text,wxString label="", wxString tooltip=""); 
  
  /** Button widget with variable label. */
  void Button  (int id,wxString *label,wxString button_text, wxString tooltip=""); 

  /** Ok-Cancel Button widget. */
  void OkCancel();

  /** Integer slider widget. */
	wxSlider *Slider(int id, wxString label, int *var,int min = MININT, int max = MAXINT, wxString tooltip = "");

  /** Float slider widget. */
  mmgFloatSlider *FloatSlider(int id, wxString label, float *var,float min, float max, wxSize size = wxDefaultSize, wxString tooltip = "");

  /** Checked listbox widget. */
  //SIL. 24-3-2005: - temporary removed
  //mmgCheckListBox *CheckList(int id, wxString label = "", int height = 60, wxString tooltip = "");

  /** Listbox widget. */
	wxListBox *ListBox(int id, wxString label = "", int height = 60, wxString tooltip = "", long lbox_style = 0);

  /** Grid widget. */
	wxGrid *Grid(int id, wxString label = "", int height = 60, int row = 2,int cols = 2, wxString tooltip = "");

  /** Add window to gui sizer. */
  void Add(wxWindow* window,int option = 0, int flag = wxEXPAND, int border = 0)  {m_sizer->Add(window,option,flag,border);};

  /** Add sizer to gui sizer. */
  void Add(wxSizer*  sizer, int option = 0, int flag = wxEXPAND, int border = 0)  {m_sizer->Add(sizer, option,flag,border);};

  /** Add gui to gui sizer. */
  void AddGui(mmgGui*  gui, int option = 0, int flag = wxEXPAND, int border = 0);

  /** Remove window from gui sizer. */
  bool Remove(wxWindow* window) {return m_sizer->Remove(window);};

  /** Remove sizer from gui sizer. */
  bool Remove(wxSizer*  sizer ) {return m_sizer->Remove(sizer);};

  /** Fit the gui widgets to gui sizer. */
  void FitGui();

	/** Update gui widget. */
  void Update();
  
	/** Enable/Disable gui widget. */
  void Enable(int mod_id, bool enable);
  
	/** Return the font used for bold label. */
  wxFont GetBoldFont() {return m_bold;}; 

	/** Return the measure used to layout the widgets - pass one of the GUI_xxx constants. */
  int GetMetrics( int id); 

  void OnEvent(mafEvent& e);

  /**  \par implementation details:
  GetId is used to obtain a new/unique widget_ID. 
  As a side effect a new pair widget_ID->module_ID is stored in m_table
  \sa GetId GetModuleId m_id m_table
  */
  int GetId(int mod_id)       {m_id++; assert(m_id<MAXID); m_table[m_id-MINID]=mod_id; return m_id;}; 

  /** place the GUI on a different parent, and perform the required Resize/Stretch/ and Show */
  void Reparent(wxWindow *parent);

protected:
  mafEventListener  *m_Listener;     
  wxBoxSizer        *m_sizer;

  wxColour          m_bc;
  bool              m_use_bc;
  long              m_entry_style;

  /**  \par implementation details:
  m_bold is the font used for the Bold labels
  */
  wxFont m_bold;

  /**  \par implementation details:
  m_id is a counter that holds the last generated widget_ID.
  It is used and incremented by GetId.
  \sa GetId GetModuleId m_id m_table
  */

  int m_id;                   
  /**  \par implementation details:
  m_table is an array holding a table of conversion widget_id -> user_id.
  The range of widget_IDs is between MINID and MAXID (created widget_IDs are <= then m_id)    
  To obtain the Module ID, widget_ID are shifted by MINID and used to index the array
  \sa GetId GetModuleId m_id m_table
  */

  int m_table[MAXWIDGET];     //table widget ids -> module ids

  /**
  \par implementation details:
  translate a widget_ID in it's module_ID, accessing m_table
  \sa GetId GetModuleId m_id m_table
  */
  int GetModuleId(int w_id)   {assert(w_id>0 && w_id<= m_id); return  m_table[w_id-MINID];};

  void OnSlider			  (wxCommandEvent &event) { }//@@@ this->OnEvent(mafEvent(this, GetModuleId(event.GetId()))); }
	void OnListBox      (wxCommandEvent &event);
  void OnCheckListBox (wxCommandEvent &event);

DECLARE_EVENT_TABLE()
};
#endif
