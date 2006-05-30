/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mmgMeasureUnitSettings.cpp,v $
Language:  C++
Date:      $Date: 2006-05-30 11:26:01 $
Version:   $Revision: 1.5 $
Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mmgMeasureUnitSettings.h"
#include <wx/config.h>
#include "mafDecl.h"
#include "mmgGui.h"
#include "mmgDialog.h"

//----------------------------------------------------------------------------
mmgMeasureUnitSettings::mmgMeasureUnitSettings(mafObserver *Listener)
//----------------------------------------------------------------------------
{
	m_Listener    = Listener;
  m_DefaultUnits[0] = "mm";
  m_DefaultUnits[1] = "m";
  m_DefaultUnits[2] = "inch";
  m_DefaultUnits[3] = "feet";
  m_DefaultUnits[4] = "custom";

  m_DefaultFactors[0] = 1.0;
  m_DefaultFactors[1] = 1000.0;
  m_DefaultFactors[2] = 25.4;
  m_DefaultFactors[3] = 330.0;
  m_DefaultFactors[4] = 1.0;

  m_ChoosedDataUnit = 0;
  m_ChoosedVisualUnit = 0;

  m_DataUnitName  = m_DefaultUnits[m_ChoosedDataUnit];
  m_VisualUnitName= m_DefaultUnits[m_ChoosedVisualUnit];
  m_ScaleFactor = m_DefaultFactors[m_ChoosedVisualUnit] / m_DefaultFactors[m_ChoosedDataUnit];
  m_Gui         = NULL;
  InitializeMeasureUnit();
}
//----------------------------------------------------------------------------
mmgMeasureUnitSettings::~mmgMeasureUnitSettings() 
//----------------------------------------------------------------------------
{
  m_Gui = NULL; // gui is destroyed by the dialog.
}
//----------------------------------------------------------------------------
void mmgMeasureUnitSettings::ChooseMeasureUnit()
//----------------------------------------------------------------------------
{
  double current_scale = m_ScaleFactor;
  int current_data_unit_selected = m_ChoosedDataUnit;
  int current_visual_unit_selected = m_ChoosedVisualUnit;

  m_Gui = new mmgGui(this);
  m_Gui->Label(_("main units"));
  m_Gui->Combo(MEASURE_DEFAULT_DATA_UNIT_ID,_("data"),&m_ChoosedDataUnit,5,m_DefaultUnits);
  m_Gui->Combo(MEASURE_DEFAULT_VISUAL_UNIT_ID,_("visual"),&m_ChoosedVisualUnit,5,m_DefaultUnits);
  m_Gui->Divider(2);
  m_Gui->Label(_("custom"));
  m_Gui->String(MEASURE_DATA_STRING_ID,_("data unit"),&m_DataUnitName);
  m_Gui->String(MEASURE_VISUAL_STRING_ID,_("visual unit"),&m_VisualUnitName);
  m_Gui->Double(MEASURE_SCALE_FACTOR_ID,_("scale"),&m_ScaleFactor, MINDOUBLE, MAXDOUBLE,-1,_("scale factor of new unit referred to mm"));
  m_Gui->Enable(MEASURE_SCALE_FACTOR_ID,m_ChoosedDataUnit == 4 || m_ChoosedVisualUnit == 4);
  m_Gui->Enable(MEASURE_DATA_STRING_ID,m_ChoosedDataUnit == 4);
  m_Gui->Enable(MEASURE_VISUAL_STRING_ID,m_ChoosedDataUnit == 4);

  mmgDialog dlg(_("Settings"),mafOK | mafCANCEL);
  dlg.Add(m_Gui,1,wxEXPAND);
  int answere = dlg.ShowModal();
  if (answere == wxID_CANCEL)
  {
    m_ScaleFactor       = current_scale;
    m_ChoosedDataUnit   = current_data_unit_selected;
    m_ChoosedVisualUnit = current_visual_unit_selected;
    m_DataUnitName      = m_DefaultUnits[m_ChoosedDataUnit];
    m_VisualUnitName    = m_DefaultUnits[m_ChoosedVisualUnit];
    return;
  }
  if (m_ScaleFactor == 0.0) 
  {
    m_ScaleFactor = 1.0;
  }
  wxConfig *config = new wxConfig(wxEmptyString);
  config->Write("UnitName",m_VisualUnitName.GetCStr());
  config->Write("UnitFactor",m_ScaleFactor);
  cppDEL(config);

  /*wxString msg = _("Restart application to make new settings available!");
  wxString caption = _("Warning");
  wxMessageBox(msg, caption);*/
  mafEventMacro(mafEvent(this,MEASURE_UNIT_UPDATED));
}
//----------------------------------------------------------------------------
void mmgMeasureUnitSettings::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	switch(maf_event->GetId())
  {
    case MEASURE_DATA_STRING_ID:
    case MEASURE_SCALE_FACTOR_ID:
    break;
    case MEASURE_DEFAULT_VISUAL_UNIT_ID:
    case MEASURE_DEFAULT_DATA_UNIT_ID:
      if (m_DefaultFactors[m_ChoosedDataUnit] != 0.0)
      {
        m_ScaleFactor = m_DefaultFactors[m_ChoosedVisualUnit] / m_DefaultFactors[m_ChoosedDataUnit];
      }
      m_VisualUnitName  = m_DefaultUnits[m_ChoosedVisualUnit];
      m_DataUnitName    = m_DefaultUnits[m_ChoosedDataUnit];
      m_Gui->Enable(MEASURE_SCALE_FACTOR_ID,m_ChoosedDataUnit == 4 || m_ChoosedVisualUnit == 4);
      m_Gui->Enable(MEASURE_DATA_STRING_ID,m_ChoosedDataUnit == 4);
      m_Gui->Enable(MEASURE_VISUAL_STRING_ID,m_ChoosedVisualUnit == 4);
      m_Gui->Update();
    break;
    default:
      mafEventMacro(*maf_event);
    break; 
  }
}
//----------------------------------------------------------------------------
void mmgMeasureUnitSettings::InitializeMeasureUnit()
//----------------------------------------------------------------------------
{
  wxConfig *config = new wxConfig(wxEmptyString);
  double factor;
  wxString unit_name;
  if(config->Read(L"VisualUnitName", &unit_name))
  {
    m_VisualUnitName = unit_name;
  }
  else
  {
    config->Write(L"VisualUnitName",m_VisualUnitName.GetCStr());
  }
  if(config->Read(L"DataUnitName", &unit_name))
  {
    m_DataUnitName = unit_name;
  }
  else
  {
    config->Write(L"DataUnitName",m_DataUnitName.GetCStr());
  }
  if(config->Read(L"ScaleFactor", &factor))
  {
    m_ScaleFactor = factor;
  }
  else
  {
    config->Write(L"ScaleFactor",m_ScaleFactor);
  }

  cppDEL(config);
}
//----------------------------------------------------------------------------
double mmgMeasureUnitSettings::GetScaleFactor()
//----------------------------------------------------------------------------
{
  return m_ScaleFactor;
}
//----------------------------------------------------------------------------
mafString mmgMeasureUnitSettings::GetUnitName()
//----------------------------------------------------------------------------
{
  return m_VisualUnitName;
}
