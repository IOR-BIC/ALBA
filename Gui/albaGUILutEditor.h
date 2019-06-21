/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUILutEditor
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaGUILutEditor_H__
#define __albaGUILutEditor_H__

#include "albaDecl.h"
#include "albaEvent.h"
#include "albaLUTLibrary.h"

#include "albaGUILutWidget.h"
#include "albaGUILutSwatch.h"
#include "albaGUIColorWidget.h"

#include "vtkLookupTable.h"
/** albaGUILutEditor : vtkLookupTable Editor

  USAGE:
  - call the static member albaGUILutEditor::ShowLutDialog(vtkLookupTable *lut)

  FEATURES:
  - 20 preset available @sa albaLutPreset
  - editable number of LUT entries - limited to [1..256]
  - scalars range editing  --- ( 4 programmers - Set and Get it by calling Set/GetRange on your vtkLookupTable )
  - LUT Entry selection:
    - single selection -- mouse left click
    - range selection  -- mouse left click + drag
    - sparse selection -- mouse right click
  - Selected entries can be "shaded" interpolating first and last color both in RGBA and HSV space.

@sa albaColor albaGUIColorSwatch albaGUIColorWidget albaGUILutPreset albaGUILutSwatch albaGUILutWidget
*/
class albaGUILutEditor: public wxPanel, public albaObserver
{
public:
  albaGUILutEditor(wxWindow* parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, 
               const wxSize& size = wxSize(300,800), long style = wxTAB_TRAVERSAL);
  virtual ~albaGUILutEditor(); 

  virtual void SetListener(albaObserver *Listener) {m_Listener = Listener;};
  void OnEvent(albaEventBase *alba_event);

  /** Assign the external lookup table to the widget.*/
  void SetLut(vtkLookupTable *lut);

  /** Show the dialog.*/
  static void ShowLutDialog(vtkLookupTable *lut, albaObserver *listener = NULL, int id = MINID);

protected:

  void UpdateInfo();
  
  /** Update the internal Lookup Table according to the preset selected from the combo box.*/
  void UpdateLut();

  void UpdateWidgetsOnLutChange();
  
  /** Copy the external Lookup Table given by the user to the internal one.*/
  void CopyLut(vtkLookupTable *from, vtkLookupTable *to);
	
  //void OnComboSelection(wxCommandEvent &event);
  
  albaObserver *m_Listener;

  int          m_Preset; ///< Index of lookup table preset.
  int          m_UserPreset;

  int          m_NumEntry; ///< Number of colors of the current lookup table.
  double       m_ValueRange[2]; ///< Value range of the current lookup table.
  wxString     m_Info; // selected indexes
  wxString     m_NewUserLutName;
  wxString m_UserLutLibraryDir;

  albaGUILutSwatch   *m_LutSwatch;
  albaGUILutWidget   *m_LutWidget;
  albaGUIColorWidget *m_ColorWidget;

  albaLUTLibrary *m_UserLutLibrary;
  wxComboBox   *m_UserPresetCombo;
	wxComboBox   *m_PresetCombo;
  vtkLookupTable *m_ExternalLut;  ///< Given lut that will be modified by "ok" or "apply"
  vtkLookupTable *m_Lut;          ///< Internal lut -- initialized in SetLut
  DECLARE_EVENT_TABLE()
};
#endif
