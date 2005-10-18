/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmo2DMeasure.cpp,v $
  Language:  C++
  Date:      $Date: 2005-10-18 13:44:02 $
  Version:   $Revision: 1.2 $
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

  m_measure_type = 0;
  m_acute_angle  = "0";
  m_obtuse_angle = "0";
  m_dist         = "0";
  m_measure_text = "";
}
//----------------------------------------------------------------------------
mmo2DMeasure::~mmo2DMeasure()
//----------------------------------------------------------------------------
{
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
  ID_STORE_MEASURE,
  ID_REMOVE_MEASURE,
  ID_MEASURE_LIST,
};
//----------------------------------------------------------------------------
void mmo2DMeasure::OpRun()   
//----------------------------------------------------------------------------
{
  m_per2DMeter = mmi2DMeter::New();
  mafEventMacro(mafEvent(this,PER_PUSH,(mafObject *)m_per2DMeter));
  m_per2DMeter->SetListener(this);
  
  wxString measure[3] = {"points", "lines", "angle"};

  // setup Gui
	m_Gui = new mmgGui(this);
	m_Gui->SetListener(this);

  m_Gui->Label("measure type",true);
  m_Gui->Combo(ID_MEASURE_TYPE,"",&m_measure_type,3,measure);
  m_Gui->Divider();
  m_Gui->Label("distance: ",&m_dist);
  m_Gui->Label("acute angle: ",&m_acute_angle);
  m_Gui->Label("obtuse angle: ",&m_obtuse_angle);
  m_Gui->Divider();
  m_Gui->Label("Measure description.",true);
  m_Gui->Button(ID_STORE_MEASURE,"Store");
  //m_Gui->Button(ID_ADD_TO_VME_TREE,"Add to msf");
  m_Gui->Button(ID_REMOVE_MEASURE,"Remove");
  m_measure_list = m_Gui->ListBox(ID_MEASURE_LIST);
	m_Gui->OkCancel();

  if(m_measure_list->Number() == 0)
  {
    //m_Gui->Enable(ID_ADD_TO_VME_TREE,false);
    m_Gui->Enable(ID_REMOVE_MEASURE,false);
  }

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
          m_per2DMeter->SetMeasureType(m_measure_type);
        break;
        case ID_STORE_MEASURE:
        {
          while(m_measure_text == "")
            m_measure_text = wxGetTextFromUser("","Insert measure description", m_measure_text);
          wxString t;
          if(m_measure_type == 0 || m_measure_type == 1)
            t = m_dist + " " + m_measure_text;
          else
            t = m_acute_angle + "° (" + m_obtuse_angle + "°) " + m_measure_text;
          m_measure_list->Append(t);
          m_measure_text = "";
          m_Gui->Enable(ID_REMOVE_MEASURE,true);
          //m_Gui->Enable(ID_ADD_TO_VME_TREE,true);
        }
        break;
        case ID_REMOVE_MEASURE:
        {
          int sel = m_measure_list->GetSelection();
          if(sel != -1)
            m_measure_list->Delete(sel);
          if(m_measure_list->Number() == 0)
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
          m_dist = wxString::Format("%g",e->GetDouble());
          m_acute_angle = "0";
          m_obtuse_angle = "0";
          m_Gui->Update();
        break;
        case mmi2DMeter::ID_RESULT_ANGLE:
          m_dist = "0";
          m_acute_angle = wxString::Format("%g",e->GetDouble());
          m_obtuse_angle = wxString::Format("%g", 180.0 - e->GetDouble());
          m_Gui->Update();
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
  m_per2DMeter->RemoveMeter();  
	HideGui();
  mafEventMacro(mafEvent(this,PER_POP));
  mafDEL(m_per2DMeter);

	mafEventMacro(mafEvent(this,result));
}
