/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmo2DMeasure.cpp,v $
  Language:  C++
  Date:      $Date: 2006-03-03 15:57:40 $
  Version:   $Revision: 1.6 $
  Authors:   Paolo Quadrani    
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mmi2DMeter.h"
#include <wx/busyinfo.h>

#include "mafDecl.h"
#include "mafEvent.h"
#include "mmgGui.h"

#include "mmo2DMeasure.h"

//----------------------------------------------------------------------------
mmo2DMeasure::mmo2DMeasure(wxString label) :
mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_OP;
	m_Canundo	= true;

  m_GenerateHistogramFlag = 0;
  m_MeasureType = 0;
  m_AcuteAngle  = "0";
  m_ObtuseAngle = "0";
  m_DistanceMeasure = "0";
  m_MeasureText = "";
  m_MeasureList = NULL;
  m_2DMeterInteractor = NULL;
}
//----------------------------------------------------------------------------
mmo2DMeasure::~mmo2DMeasure()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool mmo2DMeasure::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  mafEvent e(this,VIEW_SELECTED);
  mafEventMacro(e);
  return e.GetBool();
}
//----------------------------------------------------------------------------
mafOp* mmo2DMeasure::Copy()
//----------------------------------------------------------------------------
{
	return (new mmo2DMeasure(m_Label));
}
//----------------------------------------------------------------------------
// Widgets ID's
//----------------------------------------------------------------------------
enum MEASURE2D_ID
{
  ID_MEASURE_TYPE = MINID,
  ID_PLOT_PROFILE,
  ID_UNDO_MEASURE,
  ID_STORE_MEASURE,
  ID_REMOVE_MEASURE,
  ID_MEASURE_LIST,
};
//----------------------------------------------------------------------------
void mmo2DMeasure::OpRun()   
//----------------------------------------------------------------------------
{
  m_2DMeterInteractor = mmi2DMeter::New();
  mafEventMacro(mafEvent(this,PER_PUSH,(mafObject *)m_2DMeterInteractor));
  m_2DMeterInteractor->SetListener(this);
  
  wxString measure[4] = {"points", "lines", "angle by lines", "angle by points"};

  // setup Gui
	m_Gui = new mmgGui(this);
	m_Gui->SetListener(this);

  m_Gui->Label("measure type",true);
  m_Gui->Combo(ID_MEASURE_TYPE,"",&m_MeasureType,4,measure);
  m_Gui->Bool(ID_PLOT_PROFILE,"plot profile",&m_GenerateHistogramFlag);
  m_Gui->Divider();
  m_Gui->Button(ID_UNDO_MEASURE,"Undo Measure");
  m_Gui->Divider();
  m_Gui->Label("distance: ",&m_DistanceMeasure);
  m_Gui->Label("angle: ",&m_AcuteAngle);
  //m_Gui->Label("obtuse angle: ",&m_ObtuseAngle);
  m_Gui->Divider();
  m_Gui->Label("Measure description.",true);
  m_Gui->Button(ID_STORE_MEASURE,"Store");
  //m_Gui->Button(ID_ADD_TO_VME_TREE,"Add to msf");
  m_Gui->Button(ID_REMOVE_MEASURE,"Remove");
  m_MeasureList = m_Gui->ListBox(ID_MEASURE_LIST);
	m_Gui->OkCancel();

  if(m_MeasureList->Number() == 0)
  {
    //m_Gui->Enable(ID_ADD_TO_VME_TREE,false);
    m_Gui->Enable(ID_REMOVE_MEASURE,false);
  }
  m_Gui->Enable(ID_PLOT_PROFILE, m_MeasureType == 0);
  m_Gui->Enable(ID_UNDO_MEASURE, m_2DMeterInteractor->SizeMeasureVector() != 0);

  ShowGui();
}
//----------------------------------------------------------------------------
void mmo2DMeasure::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
    if (e->GetSender() == m_Gui)
    {
      switch(e->GetId())
      {
        case ID_MEASURE_TYPE:
          m_2DMeterInteractor->SetMeasureType(m_MeasureType);
          m_Gui->Enable(ID_PLOT_PROFILE, m_MeasureType == 0);
        break;
        case ID_PLOT_PROFILE:
          m_2DMeterInteractor->GenerateHistogram(m_GenerateHistogramFlag != 0);
        break;
        case ID_UNDO_MEASURE:
          m_2DMeterInteractor->UndoMeasure();
        break;
        case ID_STORE_MEASURE:
        {
          while(m_MeasureText == "")
            m_MeasureText = wxGetTextFromUser("","Insert measure description", m_MeasureText);
          wxString t;
          if(m_MeasureType == 0 || m_MeasureType == 1)
            t = m_DistanceMeasure + " " + m_MeasureText;
          else
            //t = m_AcuteAngle + "° (" + m_ObtuseAngle + "°) " + m_MeasureText;
            t = m_AcuteAngle + "° (" + m_MeasureText;
          m_MeasureList->Append(t);
          m_MeasureText = "";
          m_Gui->Enable(ID_REMOVE_MEASURE,true);
        }
        break;
        case ID_REMOVE_MEASURE:
        {
          int sel = m_MeasureList->GetSelection();
          if(sel != -1)
            m_MeasureList->Delete(sel);
          if(m_MeasureList->Number() == 0)
          {
            m_Gui->Enable(ID_REMOVE_MEASURE,false);
          }
        }
        break;
        case wxOK:
        case wxCANCEL:
          OpStop(OP_RUN_CANCEL);
        break;
      }
    }
    else
    {
      switch(e->GetId())
      {
        case mmi2DMeter::ID_RESULT_MEASURE:
        {
          double measure = RoundValue(e->GetDouble());
          m_DistanceMeasure = wxString::Format("%g", measure);
          m_AcuteAngle = "0";
          m_ObtuseAngle = "0";
          m_Gui->Enable(ID_UNDO_MEASURE, m_2DMeterInteractor->SizeMeasureVector() != 0);
          m_Gui->Update();
        }
        break;
        case mmi2DMeter::ID_RESULT_ANGLE:
        {
          double measure = RoundValue(e->GetDouble());
          m_DistanceMeasure = "0";
          m_AcuteAngle = wxString::Format("%g",measure);
          m_ObtuseAngle = wxString::Format("%g", 180.0 - measure);
          m_Gui->Enable(ID_UNDO_MEASURE, m_2DMeterInteractor->SizeMeasureVector() != 0);
          m_Gui->Update();
        }
        break;
        default:
          mafEventMacro(*e);
        break; 
      }
    }
	}
}
//----------------------------------------------------------------------------
void mmo2DMeasure::OpStop(int result)
//----------------------------------------------------------------------------
{
  m_2DMeterInteractor->RemoveMeter();  
	HideGui();
  mafEventMacro(mafEvent(this,PER_POP));
  mafDEL(m_2DMeterInteractor);

	mafEventMacro(mafEvent(this,result));
}
