/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafGUIMeasureUnitSettings.cpp,v $
Language:  C++
Date:      $Date: 2008-07-25 06:53:38 $
Version:   $Revision: 1.1 $
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

#include "mafGUIMeasureUnitSettings.h"
#include "mafDecl.h"
#include "mafGUI.h"

//----------------------------------------------------------------------------
mafGUIMeasureUnitSettings::mafGUIMeasureUnitSettings(mafObserver *Listener, const mafString &label):
mafGUISettings(Listener, label)
//----------------------------------------------------------------------------
{
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
  
  InitializeSettings();
}
//----------------------------------------------------------------------------
void mafGUIMeasureUnitSettings::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new mafGUI(this);
  m_Gui->Label(_("Application measure units"));
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
  m_Gui->Label("");
}
//----------------------------------------------------------------------------
mafGUIMeasureUnitSettings::~mafGUIMeasureUnitSettings() 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafGUIMeasureUnitSettings::OnEvent(mafEventBase *maf_event)
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
    if (m_ScaleFactor == 0.0) 
    {
      m_ScaleFactor = 1.0;
    }
  }
  m_Config->Write("VisualUnitName",m_VisualUnitName.GetCStr());
  m_Config->Write("DataUnitName",m_DataUnitName.GetCStr());
  m_Config->Write("ScaleFactor",m_ScaleFactor);
  m_Config->Flush();
  mafEventMacro(mafEvent(this,MEASURE_UNIT_UPDATED));
}
//----------------------------------------------------------------------------
void mafGUIMeasureUnitSettings::InitializeSettings()
//----------------------------------------------------------------------------
{
  double factor;
  wxString unit_name;
  if(m_Config->Read("VisualUnitName", &unit_name))
  {
    m_VisualUnitName = unit_name;
  }
  else
  {
    m_Config->Write("VisualUnitName",m_VisualUnitName.GetCStr());
  }
  if(m_Config->Read("DataUnitName", &unit_name))
  {
    m_DataUnitName = unit_name;
  }
  else
  {
    m_Config->Write("DataUnitName",m_DataUnitName.GetCStr());
  }
  if(m_Config->Read("ScaleFactor", &factor))
  {
    m_ScaleFactor = factor;
  }
  else
  {
    m_Config->Write("ScaleFactor",m_ScaleFactor);
  }

  if (m_DataUnitName == "mm")
  {
    m_ChoosedDataUnit = 0;
  }
  else if (m_DataUnitName == "m")
  {
    m_ChoosedDataUnit = 1;
  }
  else if (m_DataUnitName == "inch")
  {
    m_ChoosedDataUnit = 2;
  }
  else if (m_DataUnitName == "feet")
  {
    m_ChoosedDataUnit = 3;
  }
  else
    m_ChoosedDataUnit = 4;

  if (m_VisualUnitName == "mm")
  {
    m_ChoosedVisualUnit = 0;
  }
  else if (m_VisualUnitName == "m")
  {
    m_ChoosedVisualUnit = 1;
  }
  else if (m_VisualUnitName == "inch")
  {
    m_ChoosedVisualUnit = 2;
  }
  else if (m_VisualUnitName == "feet")
  {
    m_ChoosedVisualUnit = 3;
  }
  else
    m_ChoosedVisualUnit = 4;
}
//----------------------------------------------------------------------------
double mafGUIMeasureUnitSettings::GetScaleFactor()
//----------------------------------------------------------------------------
{
  return m_ScaleFactor;
}
//----------------------------------------------------------------------------
mafString mafGUIMeasureUnitSettings::GetUnitName()
//----------------------------------------------------------------------------
{
  return m_VisualUnitName;
}
