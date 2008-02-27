/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medPipeGraph.cpp,v $
  Language:  C++
  Date:      $Date: 2008-02-27 10:19:18 $
  Version:   $Revision: 1.17 $
  Authors:   Roberto Mucci
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time_Array error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <vnl/vnl_vector.h>

#include "medPipeGraph.h"
#include "mafDecl.h"
#include "mafSceneNode.h"
#include "mmgGui.h"
#include "mmgCheckListBox.h"

#include "medVMEAnalog.h"
#include "mafTagArray.h"
#include "mafTagItem.h"
#include "mafVMEOutputScalar.h"
#include "mafVMEOutputScalarMatrix.h"

#include "vtkTextProperty.h"
#include "vtkDoubleArray.h"
#include "vtkXYPlotActor.h"
#include "vtkProperty2D.h"
#include "vtkTextProperty.h"
#include "vtkRenderer.h"
#include "vtkPointData.h"
#include "vtkRectilinearGrid.h"
#include "vtkLegendBoxActor.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(medPipeGraph);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// constants
//----------------------------------------------------------------------------
const int X_VALUES_TO_VALUE = 0;
const int X_VALUES_TO_INDEX = 1;

//----------------------------------------------------------------------------
medPipeGraph::medPipeGraph()
:mafPipe()
//----------------------------------------------------------------------------
{
  m_PlotActor	= NULL;
  m_CheckBox  = NULL;
  m_Legend    = FALSE;

  m_Xmin			= 0;
  m_Xmax			= 0;
  m_Ymin			= 0;
  m_Ymax			= 0;
  m_Xlabel		= 10;
  m_Ylabel		= 10;
  m_NumberOfSignals = 0;

  m_X_title		= "Time";
  m_Y_title		= "Scalar";
  m_ItemName  = "analog_";
}
//----------------------------------------------------------------------------
medPipeGraph::~medPipeGraph()
//----------------------------------------------------------------------------
{
  m_RenFront->RemoveActor2D(m_PlotActor);
  m_RenFront->SetBackground(m_OldColour);

  vtkDEL(m_PlotActor);
  }
//----------------------------------------------------------------------------
void medPipeGraph::Create(mafSceneNode *n)
//----------------------------------------------------------------------------
{
  int randomColorR, randomColorG, randomColorB;
  double timeData;
  int counter = 0;
  Superclass::Create(n);

  m_EmgPlot = medVMEAnalog::SafeDownCast(m_Vme);
  m_NumberOfSignals = m_EmgPlot->GetScalarOutput()->GetScalarData().rows();
  m_TimeStamp = m_EmgPlot->GetScalarOutput()->GetScalarData().columns();

  m_EmgPlot->Update();
  m_TimeArray = vtkDoubleArray::New();
  vnl_vector<double> rowTime = m_EmgPlot->GetScalarOutput()->GetScalarData().get_row(0);

  for (int t = 0; t < m_TimeStamp; t++)
  {
   timeData = rowTime.get(t);
   m_TimeArray->InsertValue(counter,timeData);
   counter++;
  } 

  vtkNEW(m_PlotActor);
  m_PlotActor->GetProperty()->SetColor(0.02,0.06,0.62);	
  m_PlotActor->GetProperty()->SetLineWidth(1.4);
  m_PlotActor->SetLabelFormat("%g");
  m_PlotActor->SetTitle(m_EmgPlot->GetName());

  vtkTextProperty* tProp = m_PlotActor->GetTitleTextProperty();
  tProp->SetColor(0.02,0.06,0.62);
  tProp->SetFontFamilyToArial();
  tProp->ItalicOff();
  tProp->BoldOff();
  tProp->SetFontSize(5);

  m_PlotActor->SetAxisTitleTextProperty(tProp);
  m_PlotActor->SetAxisLabelTextProperty(tProp);
  m_PlotActor->SetTitleTextProperty(tProp);	
  m_PlotActor->SetXTitle(m_X_title);
  m_PlotActor->SetYTitle(m_Y_title);

  m_LegendBox_Actor = m_PlotActor->GetLegendBoxActor();
  m_PlotActor->SetLegendPosition(0.75, 0.85); //Set position and size of the Legend Box
  m_PlotActor->SetLegendPosition2(0.35, 0.25);
  m_PlotActor->SetPosition(0.01,0.01);
  m_PlotActor->SetPosition2(0.9,0.9);
  m_PlotActor->SetVisibility(1);
  m_PlotActor->SetXValuesToValue();

  for (int n = 0 ; n < m_NumberOfSignals; n++)
  {
    randomColorR = rand() % 255;
    randomColorG = rand() % 255;
    randomColorB = rand() % 255;
    m_PlotActor->SetPlotColor(n ,randomColorR,randomColorG,randomColorB);
  }
  m_RenFront->GetBackground(m_OldColour); // Save the old Color so we can restore it
  m_RenFront->SetBackground(1,1,1);   
}

//----------------------------------------------------------------------------
void medPipeGraph::UpdateGraph()
//----------------------------------------------------------------------------
{
  double scalarData;
  int counter_array = 0;
  vtkDoubleArray *scalar;
  vnl_vector<double> row;
  m_vtkData.clear();
  scalar_Array.clear();

  m_EmgPlot = medVMEAnalog::SafeDownCast(m_Vme);

  CreateLegend();

  for (int c = 0; c < (m_NumberOfSignals-1) ; c++)
  {
    if (m_CheckBox->IsItemChecked(c))
    {
      int counter = 0;
      scalar = vtkDoubleArray::New();
      row = m_EmgPlot->GetScalarOutput()->GetScalarData().get_row(c+1);
   
      for (int t = 0; t < m_TimeStamp; t++)
      { 
        scalarData = row.get(t);
        scalar->InsertValue(counter,scalarData);
        counter++;
      }
      scalar_Array.push_back(scalar);
          
      vtkRectilinearGrid *rect_grid;
      rect_grid = vtkRectilinearGrid::New();

      rect_grid->SetDimensions(m_TimeStamp, 1, 1);
      rect_grid->SetXCoordinates(m_TimeArray); 
      rect_grid->GetPointData()->SetScalars(scalar_Array.at(counter_array));
      
      m_vtkData.push_back(rect_grid);
      m_PlotActor->AddInput(m_vtkData.at(counter_array));
      counter_array++;
    }    
  }

  row.clear();
  double times_range[2];
  m_TimeArray->GetRange(times_range);

  double data_range[2];
  for (unsigned long i = 0; i < m_vtkData.size(); i++)
  {
    scalar_Array.at(i)->GetRange(data_range);

    if(m_Ymin > data_range[0])
      m_Ymin = data_range[0];

    if(m_Ymax < data_range[1])
      m_Ymax = data_range[1];
  }

  if(m_Xmin > times_range[0])
    m_Xmin = times_range[0];

  if(m_Xmax < times_range[1])
    m_Xmax = times_range[1];

  m_PlotActor->SetPlotRange(m_Xmin, m_Ymin, m_Xmax, m_Ymax); 

  m_Xmin = 0;
  m_Ymin = 0;
  m_Xmax = 0;
  m_Ymax = 0;

  m_PlotActor->SetNumberOfXLabels(m_Xlabel);
  m_PlotActor->SetNumberOfYLabels(m_Ylabel);

  m_RenFront->AddActor2D(m_PlotActor);
}
//----------------------------------------------------------------------------
void medPipeGraph::CreateLegend()
//----------------------------------------------------------------------------
{
  int counter_legend = 0;
  mafString name; 
  for (int c = 0; c < (m_NumberOfSignals - 1) ; c++)
  {
    if (m_CheckBox->IsItemChecked(c))
    {
        name = m_CheckBox->GetItemLabel(c);
        m_LegendBox_Actor->SetNumberOfEntries(m_NumberOfSignals);
        m_LegendBox_Actor->SetEntryString(counter_legend,name);
        counter_legend++;
      }
  }
}
//----------------------------------------------------------------------------
void medPipeGraph::ChangeItemName()
//----------------------------------------------------------------------------
{
  m_CheckBox->SetItemLabel(m_ItemId, (wxString)m_ItemName);
  mafTagItem *t = m_Vme->GetTagArray()->GetTag("SIGNALS_NAME");
  t->SetValue(m_ItemName, m_ItemId);
  m_CheckBox->Update();
}

//----------------------------------------------------------------------------
mmgGui* medPipeGraph::CreateGui()
//----------------------------------------------------------------------------
{
  if(m_Gui == NULL) 
    m_Gui = new mmgGui(this);

  m_Gui->String(ID_ITEM_NAME,_("name :"), &m_ItemName,_(""));
  m_Gui->Divider(1);

  m_Gui->Button(ID_DRAW,_("Plot"), _(""),_("Draw selected items"));
  m_Gui->Divider();

  m_Gui->Bool(ID_LEGEND,_("Legend"),&m_Legend,0,_("Show legend"));
  m_Gui->Divider(1);

  wxString name;
  bool checked = FALSE;

  m_CheckBox = m_Gui->CheckList(ID_CHECK_BOX,_("Item"),360,_("Chose item to plot"));

  bool tagPresent = m_Vme->GetTagArray()->IsTagPresent("SIGNALS_NAME");
  if (!tagPresent)
  {
    mafTagItem tag_Sig;
    tag_Sig.SetName("SIGNALS_NAME");
    tag_Sig.SetNumberOfComponents(m_NumberOfSignals);
    m_Vme->GetTagArray()->SetTag(tag_Sig);
  }

  mafTagItem *tag_Signals = m_Vme->GetTagArray()->GetTag("SIGNALS_NAME");
  for (int n = 1; n < m_NumberOfSignals; n++)
  {
    if (tagPresent)
    {
      name = tag_Signals->GetValue(n-1);
    }
    else
    {
      name = m_ItemName + wxString::Format("%d", n);
      tag_Signals->SetValue(name.c_str(), n-1);
    }
     m_CheckBox->AddItem(n-1 , name, checked);
  }
  return m_Gui;
}

//----------------------------------------------------------------------------
void medPipeGraph::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
    case ID_DRAW:
      {
        m_PlotActor->RemoveAllInputs();
        UpdateGraph();
      }
      break;
    case ID_LEGEND:
      {
        switch (m_Legend)
        {
        case TRUE:
          {
            m_PlotActor->LegendOn();
          }
          break;
        case FALSE:
          {
            m_PlotActor->LegendOff();
          } 
          break;
        }
      }
      break;
    case ID_ITEM_NAME:
       ChangeItemName();
       CreateLegend();
      break;
    case ID_CHECK_BOX:
         m_ItemId = e->GetArg();
         m_ItemName = m_CheckBox->GetItemLabel(m_ItemId);
         m_Gui->Update();
        break;
    default:
     mafEventMacro(*e);
    }  
  }
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void medPipeGraph::MinMax(double MinMax[2], std::vector<mafTimeStamp> vec)
//----------------------------------------------------------------------------
{
  MinMax[0] = MinMax[1] = vec[0];

  for(int i = 0; i < vec.size(); i++)
  {
    if(vec[i] > MinMax[1])
      MinMax[1] = vec[i];
    if(vec[i] < MinMax[0])
      MinMax[0] = vec[i];
  }
}