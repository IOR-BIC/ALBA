/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mmgMeasureUnitSettings.cpp,v $
Language:  C++
Date:      $Date: 2006-05-16 11:45:28 $
Version:   $Revision: 1.4 $
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
  m_ScaleFactor = 1.0;
  m_UnitName    = "mm";
  m_ChoosedUnit = 0;
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
  wxString default_unit[5] = {"mm","m","inch","feet", "custom"};
  
  double current_scale = m_ScaleFactor;
  mafString current_unit_name = m_UnitName;
  int current_unit_selected = m_ChoosedUnit;

  m_Gui = new mmgGui(this);
  m_Gui->Label(_("main units"));
  m_Gui->Combo(MEASURE_DEFAULT_UNIT_ID,"",&m_ChoosedUnit,5,default_unit);
  m_Gui->Divider(2);
  m_Gui->Label(_("custom"));
  m_Gui->Double(MEASURE_SCALE_FACTOR_ID,_("scale"),&m_ScaleFactor, MINDOUBLE, MAXDOUBLE,-1,_("scale factor of new unit referred to mm"));
  m_Gui->String(MEASURE_STRING_ID,_("unit name"),&m_UnitName);
  m_Gui->Enable(MEASURE_SCALE_FACTOR_ID,m_ChoosedUnit == 4);
  m_Gui->Enable(MEASURE_STRING_ID,m_ChoosedUnit == 4);

  mmgDialog dlg(_("Settings"),mafOK | mafCANCEL);
  dlg.Add(m_Gui,1,wxEXPAND);
  int answere = dlg.ShowModal();
  if (answere == wxID_CANCEL)
  {
    m_ScaleFactor = current_scale;
    m_UnitName = current_unit_name;
    m_ChoosedUnit = current_unit_selected;
    return;
  }
  if (m_ScaleFactor == 0.0) 
  {
    m_ScaleFactor = 1.0;
  }
  wxConfig *config = new wxConfig(wxEmptyString);
  config->Write("UnitName",m_UnitName.GetCStr());
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
    case MEASURE_STRING_ID:
    case MEASURE_SCALE_FACTOR_ID:
    break;
    case MEASURE_DEFAULT_UNIT_ID:
      switch(m_ChoosedUnit) 
      {
        case 0:
          m_ScaleFactor = 1.0;
          m_UnitName = "mm";
      	break;
        case 1:
          m_ScaleFactor = 1000.0;
          m_UnitName = "m";
        break;
        case 2:
          m_ScaleFactor = 25.4;
          m_UnitName = "inch";
        break;
        case 3:
          m_ScaleFactor = 330.0;
          m_UnitName = "feet";
        break;
        case 4:
          m_ScaleFactor = 1.0;
          m_UnitName = "custom";
        break;
      }
      m_Gui->Enable(MEASURE_SCALE_FACTOR_ID,m_ChoosedUnit == 4);
      m_Gui->Enable(MEASURE_STRING_ID,m_ChoosedUnit == 4);
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
  if(config->Read(L"UnitName", &unit_name))
  {
    m_UnitName = unit_name;
    config->Read(L"UnitFactor", &factor);
    m_ScaleFactor = factor;
  }
  else
  {
    // no measure unit set; use default: "mm"
    config->Write(L"UnitName",m_UnitName.GetCStr());
    config->Write(L"UnitFactor",m_ScaleFactor);
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
  return m_UnitName;
}
