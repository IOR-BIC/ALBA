/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgLutEditor.h,v $
  Language:  C++
  Date:      $Date: 2005-12-01 15:22:06 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmgLutEditor_H__
#define __mmgLutEditor_H__

#include "mafDecl.h"
#include "mafEvent.h"
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
  - scalars range editing
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
  void OnEvent( mafEventBase *event );

  void SetLut(vtkLookupTable *lut);

  static void ShowLutDialog(vtkLookupTable *lut);

protected:

  void UpdateInfo();
  void CopyLut(vtkLookupTable *from, vtkLookupTable *to);
  
  mafObserver              *m_Listener;

  int m_preset;
  //wxString m_presets[2];
  //int m_num_preset;

  int m_numEntry;
  double m_valueRange[2]; 
  wxString m_info; // selected indexes
  
  mmgLutSwatch   *m_ls;
  mmgLutWidget   *m_lw;
  mmgColorWidget *m_cw;

  vtkLookupTable *m_external_lut;  // given lut that will be modified by "ok" or "apply"
  vtkLookupTable *m_lut;           // internal lut -- initialized in SetLut
};
#endif

