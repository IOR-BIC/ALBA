/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgLutEditor.h,v $
  Language:  C++
  Date:      $Date: 2008-06-03 17:02:20 $
  Version:   $Revision: 1.7 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmgLutEditor_H__
#define __mmgLutEditor_H__

#include "mafDecl.h"
#include "mafEvent.h"
#include "mafLUTLibrary.h"

#include "mmgLutWidget.h"
#include "mmgLutSwatch.h"
#include "mmgColorWidget.h"

#include "vtkLookupTable.h"
/** mmgLutEditor : vtkLookupTable Editor

  USAGE:
  - call the static member mmgLutEditor::ShowLutDialog(vtkLookupTable *lut)

  FEATURES:
  - 20 preset available @sa mafLutPreset
  - editable number of LUT entries - limited to [1..256]
  - scalars range editing  --- ( 4 programmers - Set and Get it by calling Set/GetRange on your vtkLookupTable )
  - LUT Entry selection:
    - single selection -- mouse left click
    - range selection  -- mouse left click + drag
    - sparse selection -- mouse right click
  - Selected entries can be "shaded" interpolating first and last color both in RGBA and HSV space.

@sa mafColor mmgColorSwatch mmgColorWidget mmgLutPreset mmgLutSwatch mmgLutWidget
*/
class mmgLutEditor: public wxPanel, public mafObserver
{
public:
  mmgLutEditor(wxWindow* parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, 
               const wxSize& size = wxSize(300,800), long style = wxTAB_TRAVERSAL);
  virtual ~mmgLutEditor(); 

  virtual void SetListener(mafObserver *Listener) {m_Listener = Listener;};
  void OnEvent(mafEventBase *maf_event);

  /** Assign the external lookup table to the widget.*/
  void SetLut(vtkLookupTable *lut);

  /** Show the dialog.*/
  static void ShowLutDialog(vtkLookupTable *lut, mafObserver *listener = NULL, int id = MINID);

protected:

  void UpdateInfo();
  
  /** Update the internal Lookup Table according to the preset selected from the combo box.*/
  void UpdateLut();

  void UpdateWidgetsOnLutChange();
  
  /** Copy the external Lookup Table given by the user to the internal one.*/
  void CopyLut(vtkLookupTable *from, vtkLookupTable *to);
	
  //void OnComboSelection(wxCommandEvent &event);
  
  mafObserver *m_Listener;

  int          m_Preset; ///< Index of lookup table preset.
  int          m_UserPreset;

  int          m_NumEntry; ///< Number of colors of the current lookup table.
  double       m_ValueRange[2]; ///< Value range of the current lookup table.
  wxString     m_Info; // selected indexes
  wxString     m_NewUserLutName;
  wxString m_UserLutLibraryDir;

  mmgLutSwatch   *m_LutSwatch;
  mmgLutWidget   *m_LutWidget;
  mmgColorWidget *m_ColorWidget;

  mafLUTLibrary *m_UserLutLibrary;
  wxComboBox   *m_UserPresetCombo;
  vtkLookupTable *m_ExternalLut;  ///< Given lut that will be modified by "ok" or "apply"
  vtkLookupTable *m_Lut;          ///< Internal lut -- initialized in SetLut
  DECLARE_EVENT_TABLE()
};
#endif
