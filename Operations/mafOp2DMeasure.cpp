/*=========================================================================

 Program: MAF2
 Module: mafOp2DMeasure
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafOp2DMeasure.h"

#include "mafVME.h"
#include "mafTagItem.h"
#include "mafTagArray.h"
#include "mafInteractor2DDistance.h"
#include "mafInteractor2DAngle.h"
#include "mafInteractor2DIndicator.h"
#include <wx/busyinfo.h>

#include "mafDecl.h"
#include "mafEvent.h"
#include "mafGUI.h"

//----------------------------------------------------------------------------
// Widgets ID's
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOp2DMeasure);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOp2DMeasure::mafOp2DMeasure(const wxString &label) 
:mafOp(label)
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

  m_DistanceInteractor2D = NULL;
	m_AngleInteractor2D    = NULL;

  m_ManualDistance = "";
  m_ManualAngle    = "";

}
//----------------------------------------------------------------------------
mafOp2DMeasure::~mafOp2DMeasure()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool mafOp2DMeasure::Accept(mafVME *node)
//----------------------------------------------------------------------------
{
  mafEvent e(this,VIEW_SELECTED);
  mafEventMacro(e);
  return e.GetBool();
}
//----------------------------------------------------------------------------
mafOp* mafOp2DMeasure::Copy()
//----------------------------------------------------------------------------
{
	return (new mafOp2DMeasure(m_Label));
}
//----------------------------------------------------------------------------
void mafOp2DMeasure::OpRun()   
//----------------------------------------------------------------------------
{
  m_DistanceInteractor2D = mafInteractor2DDistance::New();
  mafEventMacro(mafEvent(this,PER_PUSH,(mafObject *)m_DistanceInteractor2D));
  m_DistanceInteractor2D->SetListener(this);

	m_AngleInteractor2D = mafInteractor2DAngle::New();
	//mafEventMacro(mafEvent(this,PER_PUSH,(mafObject *)m_2DAngleInteractor));
	//m_2DAngleInteractor->SetListener(this);
  m_IndicatorInteractor2D = mafInteractor2DIndicator::New();
  
  wxString measure[5] = {_("points"), _("lines"), _("angle by lines"), _("angle by points"), _("indicator")};
  
  //in this vector put the measure index that starts a new intercator type.
  m_FirstPositionInteractor.push_back(0); // mafInteractor2DDistance (in this iterator there are 2 kind of measures)
  m_FirstPositionInteractor.push_back(2); // mafInteractor2DAngle (in this iterator there are 2 kind of measures)
  m_FirstPositionInteractor.push_back(4);	// mafInteractor2DIndicator
                                          // eventually next value will be "5"

  // setup Gui
	m_Gui = new mafGUI(this);
	m_Gui->SetListener(this);
	
  m_Gui->Label(_("measure type"),true);
  m_Gui->Combo(ID_MEASURE_TYPE,"",&m_MeasureType,5,measure);
  m_Gui->Bool(ID_PLOT_PROFILE,_("plot profile"),&m_GenerateHistogramFlag);
  m_Gui->Divider(2);
  
  
  
  //m_Gui->Label(_("distance: "),&m_DistanceMeasure);  
  m_Gui->String(ID_MANUAL_DISTANCE,_("Distance:"),&m_ManualDistance);
  m_Gui->Divider();
  
	
  //m_Gui->Label(_("angle: "),&m_AcuteAngle);
  m_Gui->String(ID_MANUAL_ANGLE,_("Angle:"),&m_ManualAngle);
  //m_Gui->Label("obtuse angle: ",&m_ObtuseAngle);
  m_Gui->String(ID_MANUAL_INDICATOR,_("Indicator:"),&m_ManualIndicator);

  m_Gui->Divider();
	m_Gui->Button(ID_UNDO_MEASURE,_("Undo Measure"));

  m_Gui->Label(_("Measure description."),true);
  m_Gui->Button(ID_STORE_MEASURE,_("Store"));
  //m_Gui->Button(ID_ADD_TO_VME_TREE,"Add to msf");
  m_Gui->Button(ID_REMOVE_MEASURE,_("Remove"));
  m_MeasureList = m_Gui->ListBox(ID_MEASURE_LIST);
	m_Gui->OkCancel();

  // storing
  mafVME *root = m_Input->GetRoot();
  if(root->GetTagArray()->IsTagPresent("2D_MEASURE"))
  {
    mafTagItem *measure_item = root->GetTagArray()->GetTag("2D_MEASURE");
    int c = measure_item->GetNumberOfComponents();
    for(int i = 0; i < c; i++)
    {
      mafString value;
      value = measure_item->GetComponent(i);
      m_MeasureList->Append(_(value));
    }
  }

  if(m_MeasureList->GetCount() == 0)
  {
    //m_Gui->Enable(ID_ADD_TO_VME_TREE,false);
    m_Gui->Enable(ID_REMOVE_MEASURE,false);
  }
  m_Gui->Enable(ID_PLOT_PROFILE, m_MeasureType == 0);
  m_Gui->Enable(ID_MANUAL_DISTANCE, m_DistanceInteractor2D->SizeMeasureVector() != 0);
  m_Gui->Enable(ID_MANUAL_ANGLE, m_AngleInteractor2D->SizeMeasureVector() != 0);
  m_Gui->Enable(ID_MANUAL_INDICATOR, m_IndicatorInteractor2D->SizeMeasureVector() != 0);
  
  m_Gui->Enable(ID_UNDO_MEASURE, m_AngleInteractor2D->SizeMeasureVector() != 0 || m_DistanceInteractor2D->SizeMeasureVector() != 0 || m_IndicatorInteractor2D->SizeMeasureVector() != 0);        
  
	m_Gui->Divider();

  ShowGui();
}
//----------------------------------------------------------------------------
void mafOp2DMeasure::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
    if (e->GetSender() == m_Gui)
    {
      switch(e->GetId())
      {
        case ID_MEASURE_TYPE:
					if(m_MeasureType == 0 || m_MeasureType == 1)
					{
						mafEventMacro(mafEvent(this,PER_POP));	
						mafEventMacro(mafEvent(this,PER_PUSH,(mafObject *)m_DistanceInteractor2D));
						m_DistanceInteractor2D->SetListener(this);

						if(m_DistanceInteractor2D->SizeMeasureVector() != 0)
							m_DistanceMeasure = mafString(wxString::Format("%.2f" ,RoundValue(m_DistanceInteractor2D->GetLastDistance())));
						else
							m_DistanceMeasure = mafString("0");


						if(m_DistanceInteractor2D->IsDisableUndoAndOkCancel() || m_IndicatorInteractor2D->IsDisableUndoAndOkCancel())
						{           
							wxMessageBox("Fix the label in the window");
							m_MeasureType = 0;
							m_Gui->Update();
						}
						else
						{
							m_DistanceInteractor2D->SetMeasureType(m_MeasureType - m_FirstPositionInteractor[0]);
							m_Gui->Enable(ID_PLOT_PROFILE, m_MeasureType == 0);
						}
						m_Gui->Enable(ID_MANUAL_DISTANCE, m_DistanceInteractor2D->SizeMeasureVector() != 0);
						m_Gui->Enable(ID_MANUAL_ANGLE, false);
						m_Gui->Update();
					}
					else if(m_MeasureType == 2 || m_MeasureType == 3)
					{
						mafEventMacro(mafEvent(this,PER_POP));	
						mafEventMacro(mafEvent(this,PER_PUSH,(mafObject *)m_AngleInteractor2D));
						m_AngleInteractor2D->SetListener(this);

						if(m_AngleInteractor2D->SizeMeasureVector() != 0)
							m_AcuteAngle = mafString(wxString::Format("%.2f" ,RoundValue(m_AngleInteractor2D->GetLastAngle())));
						else
							m_AcuteAngle = mafString("0");

						if(m_DistanceInteractor2D->IsDisableUndoAndOkCancel() || m_IndicatorInteractor2D->IsDisableUndoAndOkCancel())
						{           
							wxMessageBox("Fix the label in the window");
							m_MeasureType = 0;
							m_Gui->Update();
						}
						else
						{
							m_AngleInteractor2D->SetMeasureType(m_MeasureType - m_FirstPositionInteractor[1]);
							m_Gui->Enable(ID_PLOT_PROFILE, m_MeasureType == 0);
						}
						m_Gui->Enable(ID_MANUAL_DISTANCE, false);
						m_Gui->Enable(ID_MANUAL_ANGLE, m_AngleInteractor2D->SizeMeasureVector() != 0 && (m_MeasureType == 2 || m_MeasureType == 3));
						m_Gui->Update();
					
					}
          else if(m_MeasureType == 4)
          {
            mafEventMacro(mafEvent(this,PER_POP));	
						mafEventMacro(mafEvent(this,PER_PUSH,(mafObject *)m_IndicatorInteractor2D));
						m_IndicatorInteractor2D->SetListener(this);
            m_Gui->Enable(ID_PLOT_PROFILE, false);
            m_Gui->Update();
          }
          else
          {
            wxMessageBox("Measure Not Yet Implemented");
          }
        break;
        case ID_PLOT_PROFILE:
          m_DistanceInteractor2D->GenerateHistogram(m_GenerateHistogramFlag != 0);
        break;
        case ID_UNDO_MEASURE:
          if(m_DistanceInteractor2D->IsDisableUndoAndOkCancel() || m_IndicatorInteractor2D->IsDisableUndoAndOkCancel())
            wxMessageBox("Fix the label in the window");
          else
          {
						if(m_InteractorType[m_InteractorType.size()-1] == ID_DISTANCE_TYPE)
						  {
							  m_DistanceInteractor2D->UndoMeasure();
                m_InteractorType.pop_back();
  							
							  if(m_InteractorType.size() != 0 && m_InteractorType[m_InteractorType.size()-1] == ID_DISTANCE_TYPE)
                {
								  m_ManualDistance = m_DistanceInteractor2D->GetLabel();
                  m_ManualAngle    = "";   
                  m_ManualIndicator= "";
                  m_Gui->Enable(ID_MANUAL_DISTANCE, true);
                  m_Gui->Enable(ID_MANUAL_ANGLE, false);
                  m_Gui->Enable(ID_MANUAL_INDICATOR, false);
                }
                else if(m_InteractorType.size() != 0 && m_InteractorType[m_InteractorType.size()-1] == ID_ANGLE_TYPE)
                {
                  m_ManualDistance = "";
                  m_ManualIndicator= "";
                  m_ManualAngle = m_AngleInteractor2D->GetLabel();
                  m_Gui->Enable(ID_MANUAL_ANGLE, true);
                  m_Gui->Enable(ID_MANUAL_DISTANCE, false);
                  m_Gui->Enable(ID_MANUAL_INDICATOR, false);
                }
                else if(m_InteractorType.size() != 0 && m_InteractorType[m_InteractorType.size()-1] == ID_INDICATOR_TYPE)
                {
                  m_ManualDistance = "";
                  m_ManualAngle = "";
                  m_ManualIndicator= m_IndicatorInteractor2D->GetLabel();
                  m_Gui->Enable(ID_MANUAL_ANGLE, false);
                  m_Gui->Enable(ID_MANUAL_DISTANCE, false);
                  m_Gui->Enable(ID_MANUAL_INDICATOR, true);
                }
                else if (m_InteractorType.size() == 0)
                {
                  m_ManualDistance = "";
                  m_ManualAngle    = "";
                  m_ManualIndicator= "";
                  m_Gui->Enable(ID_UNDO_MEASURE, false);
	                								
                  m_Gui->Enable(ID_MANUAL_DISTANCE, false);
						      m_Gui->Enable(ID_MANUAL_ANGLE, false);
                  m_Gui->Enable(ID_MANUAL_INDICATOR, false);
							    m_Gui->Update();
                  return;
                }


                m_Gui->Enable(ID_UNDO_MEASURE, m_AngleInteractor2D->SizeMeasureVector() != 0 || m_DistanceInteractor2D->SizeMeasureVector() != 0 || m_IndicatorInteractor2D->SizeMeasureVector() != 0);        
				
							  m_Gui->Update();
						  }
						else if(m_InteractorType[m_InteractorType.size()-1] == ID_ANGLE_TYPE)
						  {
							  m_AngleInteractor2D->UndoMeasure();
                m_InteractorType.pop_back();
  		
							  if(m_InteractorType.size() != 0 && m_InteractorType[m_InteractorType.size()-1] == ID_DISTANCE_TYPE)
                {
								  m_ManualDistance = m_DistanceInteractor2D->GetLabel();
                  m_ManualAngle    = "";   
                  m_ManualIndicator= "";
                  m_Gui->Enable(ID_MANUAL_DISTANCE, true);
                  m_Gui->Enable(ID_MANUAL_ANGLE, false);
                  m_Gui->Enable(ID_MANUAL_INDICATOR, false);
                }
                else if(m_InteractorType.size() != 0 && m_InteractorType[m_InteractorType.size()-1] == ID_ANGLE_TYPE)
                {
                  m_ManualDistance = "";
                  m_ManualIndicator= "";
                  m_ManualAngle = m_AngleInteractor2D->GetLabel();
                  m_Gui->Enable(ID_MANUAL_ANGLE, true);
                  m_Gui->Enable(ID_MANUAL_DISTANCE, false);
                  m_Gui->Enable(ID_MANUAL_INDICATOR, false);
                }
                else if(m_InteractorType.size() != 0 && m_InteractorType[m_InteractorType.size()-1] == ID_INDICATOR_TYPE)
                {
                  m_ManualDistance = "";
                  m_ManualAngle = "";
                  m_ManualIndicator= m_IndicatorInteractor2D->GetLabel();
                  m_Gui->Enable(ID_MANUAL_ANGLE, false);
                  m_Gui->Enable(ID_MANUAL_DISTANCE, false);
                  m_Gui->Enable(ID_MANUAL_INDICATOR, true);
                }
                else if (m_InteractorType.size() == 0)
                {
                  m_ManualDistance = "";
                  m_ManualAngle    = "";
                  m_ManualIndicator= "";
                  m_Gui->Enable(ID_UNDO_MEASURE, false);
	                								
                  m_Gui->Enable(ID_MANUAL_DISTANCE, false);
						      m_Gui->Enable(ID_MANUAL_ANGLE, false);
                  m_Gui->Enable(ID_MANUAL_INDICATOR, false);
							    m_Gui->Update();
                  return;
                }

                m_Gui->Enable(ID_UNDO_MEASURE, m_AngleInteractor2D->SizeMeasureVector() != 0 || m_DistanceInteractor2D->SizeMeasureVector() != 0 || m_IndicatorInteractor2D->SizeMeasureVector() != 0);        
				
							  m_Gui->Update();
						 
            } 
            else if(m_InteractorType[m_InteractorType.size()-1] == ID_INDICATOR_TYPE)
            {
                m_IndicatorInteractor2D->UndoMeasure();
                m_InteractorType.pop_back();
  		
							  if(m_InteractorType.size() != 0 && m_InteractorType[m_InteractorType.size()-1] == ID_DISTANCE_TYPE)
                {
								  m_ManualDistance = m_DistanceInteractor2D->GetLabel();
                  m_ManualAngle    = "";   
                  m_ManualIndicator= "";
                  m_Gui->Enable(ID_MANUAL_DISTANCE, true);
                  m_Gui->Enable(ID_MANUAL_ANGLE, false);
                  m_Gui->Enable(ID_MANUAL_INDICATOR, false);
                }
                else if(m_InteractorType.size() != 0 && m_InteractorType[m_InteractorType.size()-1] == ID_ANGLE_TYPE)
                {
                  m_ManualDistance = "";
                  m_ManualIndicator= "";
                  m_ManualAngle = m_AngleInteractor2D->GetLabel();
                  m_Gui->Enable(ID_MANUAL_ANGLE, true);
                  m_Gui->Enable(ID_MANUAL_DISTANCE, false);
                  m_Gui->Enable(ID_MANUAL_INDICATOR, false);
                }
                else if(m_InteractorType.size() != 0 && m_InteractorType[m_InteractorType.size()-1] == ID_INDICATOR_TYPE)
                {
                  m_ManualDistance = "";
                  m_ManualAngle = "";
                  m_ManualIndicator= m_IndicatorInteractor2D->GetLabel();
                  m_Gui->Enable(ID_MANUAL_ANGLE, false);
                  m_Gui->Enable(ID_MANUAL_DISTANCE, false);
                  m_Gui->Enable(ID_MANUAL_INDICATOR, true);
                }
                else if (m_InteractorType.size() == 0)
                {
                  m_ManualDistance = "";
                  m_ManualAngle    = "";
                  m_ManualIndicator= "";
                  m_Gui->Enable(ID_UNDO_MEASURE, false);
	                								
                  m_Gui->Enable(ID_MANUAL_DISTANCE, false);
						      m_Gui->Enable(ID_MANUAL_ANGLE, false);
                  m_Gui->Enable(ID_MANUAL_INDICATOR, false);
							    m_Gui->Update();
                  return;
                }

                m_Gui->Enable(ID_UNDO_MEASURE, m_AngleInteractor2D->SizeMeasureVector() != 0 || m_DistanceInteractor2D->SizeMeasureVector() != 0 || m_IndicatorInteractor2D->SizeMeasureVector() != 0);        
				
							  m_Gui->Update();
            
            }
          }
        break;
        case ID_MANUAL_DISTANCE:
          if(m_DistanceInteractor2D->IsDisableUndoAndOkCancel() || m_IndicatorInteractor2D->IsDisableUndoAndOkCancel())
            wxMessageBox("Fix the label in the window");
          else if(wxString(m_ManualDistance).ToDouble(m_Unused) != false)
          {
						if(atof(m_ManualDistance) > 0)
						{
              m_DistanceMeasure = mafString(m_ManualDistance);
              m_DistanceInteractor2D->SetManualDistance(atof(m_ManualDistance));
              m_Gui->Update();
						}
          }
					else
					{
						//wxMessageBox(L"label");
						m_DistanceInteractor2D->SetLabel(m_ManualDistance);
					}
        break;
        case ID_MANUAL_ANGLE:
          if(m_DistanceInteractor2D->IsDisableUndoAndOkCancel() || m_IndicatorInteractor2D->IsDisableUndoAndOkCancel())
            wxMessageBox("Fix the label in the window");
          else if(wxString(m_ManualAngle).ToDouble(m_Unused) != false)
					{
						if(atof(m_ManualAngle) >= 0 && atof(m_ManualAngle) <= 180)
						{
							m_AcuteAngle = mafString(m_ManualAngle);
							m_AngleInteractor2D->SetManualAngle(atof(m_ManualAngle));
							m_Gui->Update();
						}
					}
					else
					{
						//wxMessageBox(L"label");
						m_AngleInteractor2D->SetLabel(m_ManualAngle);
					}
        break;
        case ID_MANUAL_INDICATOR:
          if(m_DistanceInteractor2D->IsDisableUndoAndOkCancel() || m_IndicatorInteractor2D->IsDisableUndoAndOkCancel())
            wxMessageBox("Fix the label in the window");
          else
          {
            m_IndicatorInteractor2D->SetLabel(m_ManualIndicator);
            m_Gui->Update();
          }
        break;
        case ID_STORE_MEASURE:
        {         
          m_MeasureText = wxGetTextFromUser("",_("Insert measure description"), _(m_MeasureText));
          if(m_MeasureText == "") break;
          mafString t;
          if(m_MeasureType == 0 || m_MeasureType == 1)
          {
            t = m_DistanceMeasure;
            t += _(" ");
            t += m_MeasureText;
          }
          else
          {
            //t = m_AcuteAngle + "° (" + m_ObtuseAngle + "°) " + m_MeasureText;
            t = m_AcuteAngle;
            t += "° (";
            t += m_MeasureText;
          }
          m_MeasureList->Append(_(t));
          m_MeasureText = "";
          m_Gui->Enable(ID_REMOVE_MEASURE,true);
        }
        break;
        case ID_REMOVE_MEASURE:
        {
          int sel = m_MeasureList->GetSelection();
          if(sel != -1)
            m_MeasureList->Delete(sel);
          if(m_MeasureList->GetCount() == 0)
          {
            m_Gui->Enable(ID_REMOVE_MEASURE,false);
          }
        }
        break;
        case wxOK:
        case wxCANCEL:
          if(m_DistanceInteractor2D->IsDisableUndoAndOkCancel() || m_IndicatorInteractor2D->IsDisableUndoAndOkCancel())
            wxMessageBox("Fix the label in the window");
          else
            OpStop(OP_RUN_CANCEL);
        break;
      }
    }
    else
    {
      switch(e->GetId())
      {
        case mafInteractor2DDistance::ID_RESULT_MEASURE:
        {
          double measure = RoundValue(e->GetDouble());
          m_DistanceMeasure = wxString::Format("%g", measure);
          m_AcuteAngle = "0";
          m_ObtuseAngle = "0";
          m_Gui->Enable(ID_UNDO_MEASURE, m_AngleInteractor2D->SizeMeasureVector() != 0 || m_DistanceInteractor2D->SizeMeasureVector() != 0 || m_IndicatorInteractor2D->SizeMeasureVector() != 0);        
          m_Gui->Enable(ID_MANUAL_DISTANCE, m_DistanceInteractor2D->SizeMeasureVector() != 0);
          m_Gui->Enable(ID_MANUAL_ANGLE,false);
          m_Gui->Enable(ID_MANUAL_INDICATOR,false);
          if(m_DistanceInteractor2D->GetRegisterMeasure())
					{
						m_InteractorType.push_back(ID_DISTANCE_TYPE);
					}
					m_ManualDistance = measure;
          m_Gui->Update();
        }
        break;
        case mafInteractor2DAngle::ID_RESULT_ANGLE:
        {
          double measure = RoundValue(e->GetDouble());
          m_DistanceMeasure = "0";
          m_Gui->Enable(ID_UNDO_MEASURE, m_AngleInteractor2D->SizeMeasureVector() != 0 || m_DistanceInteractor2D->SizeMeasureVector() != 0 || m_IndicatorInteractor2D->SizeMeasureVector() != 0);        
          m_Gui->Enable(ID_MANUAL_ANGLE, m_AngleInteractor2D->SizeMeasureVector() != 0);
          m_Gui->Enable(ID_MANUAL_DISTANCE, false);
          m_Gui->Enable(ID_MANUAL_INDICATOR,false);
          m_AcuteAngle = wxString::Format("%g",measure);
          m_ObtuseAngle = wxString::Format("%g", 180.0 - measure);
					if(m_AngleInteractor2D->GetRegisterMeasure())
					{
						m_InteractorType.push_back(ID_ANGLE_TYPE);
					}
					m_ManualAngle = measure;
          m_Gui->Update();
        }
        break;
        case mafInteractor2DIndicator::ID_RESULT_INDICATOR:
        {
          m_Gui->Enable(ID_MANUAL_ANGLE, false);
          m_Gui->Enable(ID_MANUAL_DISTANCE, false);
          m_Gui->Enable(ID_MANUAL_INDICATOR,m_IndicatorInteractor2D->SizeMeasureVector() != 0);
          m_Gui->Enable(ID_UNDO_MEASURE, m_AngleInteractor2D->SizeMeasureVector() != 0 || m_DistanceInteractor2D->SizeMeasureVector() != 0 || m_IndicatorInteractor2D->SizeMeasureVector() != 0);        
          if(m_IndicatorInteractor2D->GetRegisterMeasure())
					{
						m_InteractorType.push_back(ID_INDICATOR_TYPE);
            m_ManualIndicator = "Label";
					}
          
          m_Gui->Update();
        }
        break;
        default:
					Superclass::OnEvent(e);
        break; 
      }
    }
	}
}
//----------------------------------------------------------------------------
void mafOp2DMeasure::OpStop(int result)
//----------------------------------------------------------------------------
{
  int c = m_MeasureList->GetCount();
  mafTagItem measure_item;
  measure_item.SetName("2D_MEASURE");
  measure_item.SetNumberOfComponents(c);
  for(int i = 0; i < c; i++)
    measure_item.SetComponent(mafString(m_MeasureList->GetString(i)),i);
  mafVME *root = m_Input->GetRoot();
  if(root->GetTagArray()->IsTagPresent("2D_MEASURE"))
  root->GetTagArray()->DeleteTag("2D_MEASURE");
  root->GetTagArray()->SetTag(measure_item);

	GetLogicManager()->VmeModified(root);

	m_DistanceInteractor2D->RemoveMeter();  
	m_AngleInteractor2D->RemoveMeter();
  m_IndicatorInteractor2D->RemoveMeter();
 
	HideGui();
  mafEventMacro(mafEvent(this,PER_POP));	
  mafDEL(m_DistanceInteractor2D);
	mafDEL(m_AngleInteractor2D);
  mafDEL(m_IndicatorInteractor2D);

	mafEventMacro(mafEvent(this,result));
}

//----------------------------------------------------------------------------
char ** mafOp2DMeasure::GetIcon()
{
#include "pic/OP_2D_MEASURES.xpm"
	return OP_2D_MEASURES_xpm;
}