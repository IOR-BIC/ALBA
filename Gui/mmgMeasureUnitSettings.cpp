/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mmgMeasureUnitSettings.cpp,v $
Language:  C++
Date:      $Date: 2006-05-16 09:24:06 $
Version:   $Revision: 1.2 $
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
  InitializeMeasureUnit();
}
//----------------------------------------------------------------------------
mmgMeasureUnitSettings::~mmgMeasureUnitSettings() 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mmgMeasureUnitSettings::ChooseMeasureUnit()
//----------------------------------------------------------------------------
{
  double current_scale = m_ScaleFactor;
  mafString current_unit_name = m_UnitName;

  mmgGui *gui = new mmgGui(this);
  gui->Double(MEASURE_SCALE_FACTOR_ID,_("scale"),&m_ScaleFactor, MINDOUBLE, MAXDOUBLE,-1,_("scale factor of new unit referred to mm"));
  gui->String(MEASURE_STRING_ID,_("unit name"),&m_UnitName);

  mmgDialog dlg(_("Settings"),mafOK | mafCANCEL);
  dlg.Add(gui,1,wxEXPAND);
  int answere = dlg.ShowModal();
  if (answere == wxID_CANCEL)
  {
    m_ScaleFactor = current_scale;
    m_UnitName = current_unit_name;
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
