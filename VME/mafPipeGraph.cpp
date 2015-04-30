/*=========================================================================

 Program: MAF2
 Module: mafPipeGraph
 Authors: Roberto Mucci
 
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
// Failing in doing this will result in a run-time_Array error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <vnl/vnl_vector.h>


#include "mafPipeGraph.h"
#include "mafDecl.h"
#include "mafSceneNode.h"
#include "mafGUI.h"
#include "mafGUICheckListBox.h"

#include "mafVMEAnalog.h"
#include "mafTagArray.h"
#include "mafTagItem.h"
#include "mafVMEOutputScalar.h"
#include "mafVMEOutputScalarMatrix.h"
#include "mafEventSource.h"

#include "vtkTextProperty.h"
#include "vtkDoubleArray.h"
#include "vtkXYPlotActor.h"
#include "vtkProperty2D.h"
#include "vtkTextProperty.h"
#include "vtkRenderer.h"
#include "vtkPointData.h"
#include "vtkRectilinearGrid.h"
#include "vtkLegendBoxActor.h"
#include "vtkMAFSmartPointer.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafPipeGraph);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafPipeGraph::mafPipeGraph()
:mafPipe()
//----------------------------------------------------------------------------
{
  m_PlotActor	= NULL;
  m_PlotTimeLineActor	= NULL;
  m_CheckBox  = NULL;
  m_CheckedVector.clear();

  m_Xlabel		= 50;
  m_Ylabel		= 50;
  m_NumberOfSignals = 0;

  m_TitileX		= "";
  m_TitileY		= "";
  m_Title     = "";
  m_ItemName  = "analog_";
  m_FitPlot = 1;
  m_Legend = 0;

  m_DataMax = 0;
  m_DataMin = 0;
  m_TimeStampMax = 0;

  m_TimeStamp = 0;
  m_ItemId = 0;

  m_TimeLine = NULL;

  m_VtkData.clear();
  m_ScalarArray.clear();
}
//----------------------------------------------------------------------------
mafPipeGraph::~mafPipeGraph()
//----------------------------------------------------------------------------
{
  m_Vme->GetEventSource()->RemoveObserver(this);

  m_RenFront->RemoveActor2D(m_PlotActor);
  m_RenFront->RemoveActor2D(m_PlotTimeLineActor);
  m_RenFront->SetBackground(m_OldColour);
  m_AlwaysVisibleRenderer->SetBackground(m_OldColour);

  for(int i=0;i<m_VtkData.size();i++)
  {
    vtkDEL(m_VtkData[i]);
  }
  m_VtkData.clear();

  for(int i=0;i<m_ScalarArray.size();i++)
  {
    vtkDEL(m_ScalarArray[i]);
  }
  m_ScalarArray.clear();
  m_CheckedVector.clear();

  vtkDEL(m_TimeArray);
  m_PlotActor->RemoveAllInputs();
  m_PlotTimeLineActor->RemoveAllInputs();
  vtkDEL(m_PlotActor);
  vtkDEL(m_PlotTimeLineActor);
  }
//----------------------------------------------------------------------------
void mafPipeGraph::Create(mafSceneNode *n)
//----------------------------------------------------------------------------
{
  double timeData;
  int counter = 0;
  Superclass::Create(n);

  m_Vme->GetEventSource()->AddObserver(this);

  m_EmgPlot = mafVMEAnalog::SafeDownCast(m_Vme);
  m_NumberOfSignals = m_EmgPlot->GetScalarOutput()->GetScalarData().rows()-1; //1 row is for time information
  m_DataMin = m_EmgPlot->GetScalarOutput()->GetScalarData().min_value();
  m_DataMax = m_EmgPlot->GetScalarOutput()->GetScalarData().max_value();
  m_TimeStamp = m_EmgPlot->GetScalarOutput()->GetScalarData().columns();
  m_DataManualRange[0] = m_DataMin; //Initialize max data range 
  m_DataManualRange[1] = m_DataMax;

  m_CheckedVector.resize(m_NumberOfSignals,false);
  
  m_EmgPlot->Update();
  m_TimeArray = vtkDoubleArray::New();
  vnl_vector<double> rowTime = m_EmgPlot->GetScalarOutput()->GetScalarData().get_row(0);

  for (int t = 0; t < m_TimeStamp; t++)
  {
   timeData = rowTime.get(t);
   m_TimeArray->InsertValue(counter,timeData);
   counter++;
  } 
  m_TimeStampMax = m_TimeArray->GetValue(counter-1);
  m_TimeArray->GetRange(m_TimesManualRange); //Initialize time range at max

  //NB: I must create 2 vtkXYPlotActor in order to have a time line 
  //on the plot but not on the legend. m_PlotTimeLineActor contains
  //only the time line
  vtkNEW(m_PlotTimeLineActor);
  m_PlotTimeLineActor->GetProperty()->SetColor(0.02,0.06,0.62);	
  m_PlotTimeLineActor->GetProperty()->SetLineWidth(1.4);

  vtkTextProperty* tProp = m_PlotTimeLineActor->GetTitleTextProperty();
  tProp->SetColor(0.02,0.06,0.62);
  tProp->SetFontFamilyToArial();
  tProp->ItalicOff();
  tProp->BoldOff();
  tProp->SetFontSize(5);
 
  m_PlotTimeLineActor->SetAxisTitleTextProperty(tProp);
  m_PlotTimeLineActor->SetAxisLabelTextProperty(tProp);
  m_PlotTimeLineActor->SetTitleTextProperty(tProp);	

  m_LegendBoxTimeLine_Actor = m_PlotTimeLineActor->GetLegendBoxActor();
  m_PlotTimeLineActor->SetLegendPosition(0.75, 0.85); //Set position and size of the Legend Box
  m_PlotTimeLineActor->SetLegendPosition2(0.35, 0.25);
  m_PlotTimeLineActor->SetPosition(0.01,0.01);
  m_PlotTimeLineActor->SetPosition2(0.9,0.9);
  m_PlotTimeLineActor->SetVisibility(1);
  m_PlotTimeLineActor->SetXValuesToValue();

  m_LegendBoxTimeLine_Actor->SetNumberOfEntries(1);
  m_LegendBoxTimeLine_Actor->SetEntryString(0, "");

  m_ColorRGB[0] = 255;
  m_ColorRGB[1] = 0;
  m_ColorRGB[2] = 0;

  m_LegendBoxTimeLine_Actor->SetEntryColor(0, m_ColorRGB);

  vtkNEW(m_PlotActor);
  m_PlotActor->GetProperty()->SetColor(0.02,0.06,0.62);	
  m_PlotActor->GetProperty()->SetLineWidth(1.4);

  vtkTextProperty* tPropB = m_PlotActor->GetTitleTextProperty();
  tPropB->SetColor(0.02,0.06,0.62);
  tPropB->SetFontFamilyToArial();
  tPropB->ItalicOff();
  tPropB->BoldOff();
  tPropB->SetFontSize(5);

  m_PlotActor->SetAxisTitleTextProperty(tProp);
  m_PlotActor->SetAxisLabelTextProperty(tProp);
  m_PlotActor->SetTitleTextProperty(tProp);	

  m_LegendBox_Actor = m_PlotActor->GetLegendBoxActor();
  m_PlotActor->SetLegendPosition(0.75, 0.85); //Set position and size of the Legend Box
  m_PlotActor->SetLegendPosition2(0.35, 0.25);
  m_PlotActor->SetPosition(0.01,0.01);
  m_PlotActor->SetPosition2(0.9,0.9);
  m_PlotActor->SetVisibility(1);
  m_PlotActor->SetXValuesToValue();

  bool tagPresent = m_Vme->GetTagArray()->IsTagPresent("SIGNALS_COLOR");
  if (!tagPresent)
  {
    mafTagItem tag_Sig;
    tag_Sig.SetName("SIGNALS_COLOR");
    tag_Sig.SetNumberOfComponents(m_NumberOfSignals*3); //3 color values each signal
    m_Vme->GetTagArray()->SetTag(tag_Sig);
  }

  mafTagItem *tag_Signals = m_Vme->GetTagArray()->GetTag("SIGNALS_COLOR");
  for (long n = 0; n < m_NumberOfSignals; n++)
  {
    long id = n*3;
    if (tagPresent) //Retrieve signals colors from tag
    {
      m_ColorRGB[0] = tag_Signals->GetValueAsDouble(id);
      m_ColorRGB[1] = tag_Signals->GetValueAsDouble(id+1);
      m_ColorRGB[2] = tag_Signals->GetValueAsDouble(id+2);
      m_PlotActor->SetPlotColor(n, m_ColorRGB);
    }
    else //Create random colors
    {
      m_ColorRGB[0] = rand() % 255;
      m_ColorRGB[1] = rand() % 255;
      m_ColorRGB[2] = rand() % 255;
      m_PlotActor->SetPlotColor(n, m_ColorRGB);
      tag_Signals->SetValue(m_ColorRGB[0], id);
      tag_Signals->SetValue(m_ColorRGB[1], id+1);
      tag_Signals->SetValue(m_ColorRGB[2], id+2);
    }
  }

  tagPresent = false;
  tagPresent = m_Vme->GetTagArray()->IsTagPresent("AXIS_TITLE");
  if (!tagPresent)
  {
    mafTagItem tag_Sig;
    tag_Sig.SetName("AXIS_TITLE");
    tag_Sig.SetNumberOfComponents(2);
    m_Vme->GetTagArray()->SetTag(tag_Sig);
  }

  mafTagItem *tagAxisTile = m_Vme->GetTagArray()->GetTag("AXIS_TITLE");
  if (tagPresent) //Retrieve axis title from tag
  {
    m_TitileX = tagAxisTile->GetValue(0);
    m_TitileY = tagAxisTile->GetValue(1);
  }
  else //set default axis title
  {
    tagAxisTile->SetValue(m_TitileX,0);
    tagAxisTile->SetValue(m_TitileY,1);
  }
  m_PlotActor->SetXTitle(m_TitileX);
  m_PlotActor->SetYTitle(m_TitileY);

  m_PlotTimeLineActor->SetXTitle(m_TitileX);
  m_PlotTimeLineActor->SetYTitle(m_TitileY);

  m_RenFront->GetBackground(m_OldColour); // Save the old Color so we can restore it
  m_RenFront->SetBackground(1,1,1);  
  m_AlwaysVisibleRenderer->SetBackground(1,1,1);
}

//----------------------------------------------------------------------------
void mafPipeGraph::UpdateGraph()
//----------------------------------------------------------------------------
{
  double scalarData = 0;
  int counter_array = 0;
  vtkDoubleArray *scalar;
  vnl_vector<double> row;

  for(int i=0;i<m_VtkData.size();i++)
  {
    vtkDEL(m_VtkData[i]);
  }
  m_VtkData.clear();

  for(int i=0;i<m_ScalarArray.size();i++)
  {
    vtkDEL(m_ScalarArray[i]);
  }
  m_ScalarArray.clear();

  m_RenFront->RemoveActor2D(m_PlotActor);
  m_RenFront->RemoveActor2D(m_PlotTimeLineActor);

  m_EmgPlot = mafVMEAnalog::SafeDownCast(m_Vme);

   vtkMAFSmartPointer<vtkDoubleArray> newTimeArray;
   vtkMAFSmartPointer<vtkDoubleArray> fakeTimeArray;

   //cycle to get a fake scalar value
   for (int c = 0; c < m_NumberOfSignals ; c++)
   {
     if (m_CheckedVector.at(c)) //fill the vector with vtkDoubleArray of signals checked
     {
       scalar = vtkDoubleArray::New();
       row = m_EmgPlot->GetScalarOutput()->GetScalarData().get_row(c+1); //skip first row with time information

       if (m_FitPlot)
       {
         for (int t = 0; t < m_TimeStamp; t++) 
         { 
           scalarData = row.get(t);
           break;
         }
       }
       scalar->Delete();
     }
   }

  for (int c = 0; c < m_NumberOfSignals ; c++)
  {
    if (m_CheckedVector.at(c)) //fill the vector with vtkDoubleArray of signals checked
    {
      int counter = 0;
      scalar = vtkDoubleArray::New();
      row = m_EmgPlot->GetScalarOutput()->GetScalarData().get_row(c+1); //skip first row with time information
      
      if (m_FitPlot)
       {
        for (int t = 0; t < m_TimeStamp; t++) 
        { 
          newTimeArray->InsertValue(counter, m_TimeArray->GetValue(t));
          scalarData = row.get(t);
          scalar->InsertValue(counter, scalarData);
          counter++;
        }
     }
     else //if not Autofit plot, get values inside m_TimeManualRange
     {
      for (int t = 0; t < m_TimeStamp; t++) 
      { 
        if (m_TimesManualRange[0] <= m_TimeArray->GetValue(t) && m_TimeArray->GetValue(t) <= m_TimesManualRange[1])
        {
          newTimeArray->InsertValue(counter, m_TimeArray->GetValue(t));
          scalarData = row.get(t);
          scalar->InsertValue(counter, scalarData);
          counter++;
        }
      }
     }
      
      m_ScalarArray.push_back(scalar);
      vtkRectilinearGrid *rect_grid;
      rect_grid = vtkRectilinearGrid::New();
      rect_grid->SetDimensions(newTimeArray->GetNumberOfTuples(), 1, 1);
      rect_grid->SetXCoordinates(newTimeArray); 
      rect_grid->GetPointData()->SetScalars(m_ScalarArray.at(c)); 
      m_VtkData.push_back(rect_grid);
      m_PlotActor->AddInput(m_VtkData.at(c));
    }
    else
    {
      scalar = vtkDoubleArray::New();
      fakeTimeArray->Resize(0);
      scalar->InsertValue(0, scalarData);  //now scalarData is a fake value, already present in the plot
      m_ScalarArray.push_back(scalar);

      vtkRectilinearGrid *rect_grid;
      rect_grid = vtkRectilinearGrid::New();
      rect_grid->SetDimensions(fakeTimeArray->GetNumberOfTuples(), 1, 1);
      rect_grid->SetXCoordinates(fakeTimeArray); 
      rect_grid->GetPointData()->SetScalars(m_ScalarArray.at(c)); 
      m_VtkData.push_back(rect_grid);
      m_PlotActor->AddInput(m_VtkData.at(c));
    }
  }

  row.clear();
  newTimeArray->GetRange(m_TimesRange);

  double minY = 0;
  double maxY = 0;

  for (int i = 0; i < m_ScalarArray.size(); i++)
  {
    double dataRange[2];
    m_ScalarArray.at(i)->GetRange(dataRange);

    if(dataRange[0] < minY)
      minY = dataRange[0];

    if(dataRange[1] > maxY)
      maxY = dataRange[1];
  }

  if (m_FitPlot)
  {
    m_PlotActor->SetPlotRange(m_TimesRange[0], minY, m_TimesRange[1], maxY);
    m_PlotTimeLineActor->SetPlotRange(m_TimesRange[0], minY, m_TimesRange[1], maxY);
  }
  else
  {
    m_PlotActor->SetPlotRange(m_TimesRange[0], m_DataManualRange[0], m_TimesRange[1], m_DataManualRange[1]);
    m_PlotTimeLineActor->SetPlotRange(m_TimesRange[0], m_DataManualRange[0], m_TimesRange[1], m_DataManualRange[1]);
  }

  m_PlotActor->SetNumberOfXLabels(m_TimesRange[1]-m_TimesRange[0]); 
  m_PlotActor->SetNumberOfYLabels(m_DataMax - m_DataMin);

  m_PlotTimeLineActor->SetNumberOfXLabels(m_TimesRange[1]-m_TimesRange[0]); 
  m_PlotTimeLineActor->SetNumberOfYLabels(m_DataMax - m_DataMin);

  vtkDoubleArray *lineArray;
  vtkNEW(lineArray);
  lineArray->InsertNextTuple1(m_EmgPlot->GetTimeStamp());
  lineArray->InsertNextTuple1(m_EmgPlot->GetTimeStamp());

  vtkDoubleArray *scalarArrayLine;
  vtkNEW(scalarArrayLine);
  double scalarRange[2];
  if(m_FitPlot)
  {
    scalarRange[0]=minY+abs(minY*0.1);
    scalarRange[1]=maxY-abs(maxY*0.1);
  }
  else
  {
    scalarRange[0]=m_DataManualRange[0]+abs(m_DataManualRange[0]*0.1);
    scalarRange[1]=m_DataManualRange[1]-abs(m_DataManualRange[1]*0.1);
  }
  scalarArrayLine->InsertNextTuple1(scalarRange[0]);
  scalarArrayLine->InsertNextTuple1(scalarRange[1]);

  vtkNEW(m_TimeLine);
  m_TimeLine->SetDimensions(2, 1, 1);
  m_TimeLine->SetXCoordinates(lineArray);
  m_TimeLine->GetPointData()->SetScalars(scalarArrayLine); 
  m_TimeLine->Update();

  vtkDEL(lineArray);
  vtkDEL(scalarArrayLine);

  m_VtkData.push_back(m_TimeLine);
  m_PlotTimeLineActor->AddInput((vtkDataSet*)m_TimeLine);

  m_RenFront->AddActor2D(m_PlotActor);
  //m_RenFront->AddActor2D(m_PlotTimeLineActor);
   
}
//----------------------------------------------------------------------------
void mafPipeGraph::CreateLegend()
//----------------------------------------------------------------------------
{
  int counter_legend = 0;
  mafString name; 
  mafTagItem *tag_Signals = m_Vme->GetTagArray()->GetTag("SIGNALS_COLOR");
  m_PlotActor->RemoveAllInputs();
  for (int c = 0; c < m_NumberOfSignals ; c++)
  {
    int idx = c*3;
    if (m_CheckBox->IsItemChecked(c))
    { 
      m_PlotActor->AddInput(m_VtkData.at(c));
      m_LegendBox_Actor->SetNumberOfEntries(counter_legend + 1);
      name = m_CheckBox->GetItemLabel(c);
      m_LegendBox_Actor->SetEntryString(counter_legend, name.GetCStr());

      m_ColorRGB[0] = tag_Signals->GetValueAsDouble(idx);
      m_ColorRGB[1] = tag_Signals->GetValueAsDouble(idx+1);
      m_ColorRGB[2] = tag_Signals->GetValueAsDouble(idx+2);
      m_LegendBox_Actor->SetEntryColor(counter_legend, m_ColorRGB);
      counter_legend++;
    }
  }

  m_PlotTimeLineActor->RemoveAllInputs();

  m_PlotTimeLineActor->AddInput(m_VtkData.at(m_VtkData.size()-1));

}
//----------------------------------------------------------------------------
void mafPipeGraph::ChangeItemName()
//----------------------------------------------------------------------------
{
  m_CheckBox->SetItemLabel(m_ItemId, (wxString)m_ItemName);
  mafTagItem *t = m_Vme->GetTagArray()->GetTag("SIGNALS_NAME");
  t->SetValue(m_ItemName, m_ItemId);
  m_CheckBox->Update();
}

//----------------------------------------------------------------------------
void mafPipeGraph::ChangeAxisTitle()
//----------------------------------------------------------------------------
{
  mafTagItem *t = m_Vme->GetTagArray()->GetTag("AXIS_TITLE");
  t->SetValue(m_TitileX, 0);
  t->SetValue(m_TitileY, 1);
}

//----------------------------------------------------------------------------
void mafPipeGraph::ChangeSignalColor()
//----------------------------------------------------------------------------
{
  m_PlotActor->SetPlotColor(m_ItemId, m_ColorRGB);
  mafTagItem *t = m_Vme->GetTagArray()->GetTag("SIGNALS_COLOR");

  long colorId = m_ItemId*3;
  t->SetValue(m_ColorRGB[0], colorId);
  t->SetValue(m_ColorRGB[1], colorId+1);
  t->SetValue(m_ColorRGB[2], colorId+2);
  m_CheckBox->Update();
}

//----------------------------------------------------------------------------
mafGUI* mafPipeGraph::CreateGui()
//----------------------------------------------------------------------------
{
  if(m_Gui == NULL) 
    m_Gui = new mafGUI(this);

  m_Gui->String(ID_ITEM_NAME,_("Name :"), &m_ItemName,"");
  m_Gui->Color(ID_SIGNALS_COLOR, _("Color"), &m_SignalColor, _("Set signal color"));
  m_Gui->Divider(1);

  m_Gui->String(ID_AXIS_NAME_X,_("X Title"), &m_TitileX,_("Set X axis name"));
  m_Gui->String(ID_AXIS_NAME_Y,_("Y Title"), &m_TitileY,_("Set Y axis name"));
  m_Gui->Divider(1);

  if (m_TimeStampMax == 0.0)
  {
    m_TimeStampMax = VTK_DOUBLE_MAX;
  }

  m_Gui->VectorN(ID_RANGE_X, _("Range X"), m_TimesManualRange, 2, 0, m_TimeStampMax);
  m_Gui->VectorN(ID_RANGE_Y, _("Range Y"), m_DataManualRange, 2, m_DataMin, m_DataMax);

  m_Gui->Enable(ID_RANGE_X, !m_FitPlot);
  m_Gui->Enable(ID_RANGE_Y, !m_FitPlot);

  m_Gui->Bool(ID_FIT_PLOT,_("Autofit plot"),&m_FitPlot,1,_("Fit bounds to display all graph"));
  m_Gui->Divider();

  m_Gui->Divider();

  m_Gui->Bool(ID_LEGEND,_("Legend"),&m_Legend,0,_("Show legend"));
  m_Gui->Divider(1);

  wxString name;
  bool checked = FALSE;

  m_Gui->Label("Item");
	m_CheckBox = m_Gui->CheckList(ID_CHECK_BOX,_(""),200,_("Choose item to plot"));
  m_Gui->Button(ID_DRAW,_("Plot"), "",_("Draw selected items"));
  m_Gui->Divider();

  bool tagPresent = m_Vme->GetTagArray()->IsTagPresent("SIGNALS_NAME");
  if (!tagPresent)
  {
    mafTagItem tag_Sig;
    tag_Sig.SetName("SIGNALS_NAME");
    tag_Sig.SetNumberOfComponents(m_NumberOfSignals);
    m_Vme->GetTagArray()->SetTag(tag_Sig);
  }

  mafTagItem *tag_Signals = m_Vme->GetTagArray()->GetTag("SIGNALS_NAME");
  for (int n = 0; n < m_NumberOfSignals; n++)
  {
    if (tagPresent)
    {
      name = tag_Signals->GetValue(n);
    }
    else
    {
      name = m_ItemName + wxString::Format("%d", n);
      tag_Signals->SetValue(name.c_str(), n);
    }
     m_CheckBox->AddItem(n , name, checked);
  }
  return m_Gui;
}

//----------------------------------------------------------------------------
void mafPipeGraph::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
    case ID_RANGE_X:
    case ID_RANGE_Y:
      {
        if (m_TimesManualRange[0] >= m_TimesManualRange[1] || m_DataManualRange[0] >= m_DataManualRange[1])
        {
          mafErrorMessage("Invalid plot range!");
          return;
        }
        m_PlotActor->RemoveAllInputs();
        UpdateGraph();
        CreateLegend();
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      }
      break;
    case ID_FIT_PLOT:
      {
        m_Gui->Enable(ID_RANGE_X, !m_FitPlot);
        m_Gui->Enable(ID_RANGE_Y, !m_FitPlot);
        m_PlotActor->RemoveAllInputs();
        UpdateGraph();
        CreateLegend();
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      }
      break;
    case ID_SIGNALS_COLOR:
      {
        m_ColorRGB[0] = m_SignalColor.Red()/255.0;
        m_ColorRGB[1] = m_SignalColor.Green()/255.0;
        m_ColorRGB[2] = m_SignalColor.Blue()/255.0;
        ChangeSignalColor();
        m_PlotActor->RemoveAllInputs();
        UpdateGraph();
        CreateLegend();
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      }
      break;
    case ID_DRAW:
      {
        m_PlotActor->RemoveAllInputs();
        UpdateGraph();
        CreateLegend();
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
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
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      }
      break;
    case ID_ITEM_NAME:
      {
        ChangeItemName();
        CreateLegend();
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      }
      break;
    case ID_AXIS_NAME_X:
        m_PlotActor->SetXTitle(m_TitileX);
        m_PlotTimeLineActor->SetXTitle(m_TitileX);
        ChangeAxisTitle();
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      break;
    case ID_AXIS_NAME_Y:
        m_PlotActor->SetYTitle(m_TitileY);
        m_PlotTimeLineActor->SetYTitle(m_TitileY);
        ChangeAxisTitle();
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      break;
    case ID_CHECK_BOX:
      {
        m_ItemId = e->GetArg();
        m_ItemName = m_CheckBox->GetItemLabel(m_ItemId);
        m_Gui->Update();
        for (int i = 0; i < m_CheckedVector.size(); i++)
        {
          if (m_CheckedVector.at(i) != m_CheckBox->IsItemChecked(i))
          {
            m_CheckedVector[i] = m_CheckBox->IsItemChecked(i);
            break;
          }
        } 
      }
      break;
    default:
     mafEventMacro(*e);
    }  
  }
  else if(maf_event->GetId() == VME_TIME_SET)
  {
    if(!m_TimeLine)
      vtkNEW(m_TimeLine);

    m_RenFront->RemoveActor2D(m_PlotTimeLineActor);

    vtkDoubleArray *lineArray;
    vtkNEW(lineArray);
    lineArray->InsertNextTuple1(m_EmgPlot->GetTimeStamp());
    lineArray->InsertNextTuple1(m_EmgPlot->GetTimeStamp());

    vtkDoubleArray *scalarArrayLine;
    vtkNEW(scalarArrayLine);
    double scalarRange[2];
    
    double minY = 0;
    double maxY = 0;

    for (int i = 0; i < m_ScalarArray.size(); i++)
    {
      double dataRange[2];
      m_ScalarArray.at(i)->GetRange(dataRange);

      if(dataRange[0] < minY)
        minY = dataRange[0];

      if(dataRange[1] > maxY)
        maxY = dataRange[1];
    }

    if(m_FitPlot)
    {
      scalarRange[0]=minY+abs(minY*0.1);
      scalarRange[1]=maxY-abs(maxY*0.1);
    }
    else
    {
      scalarRange[0]=m_DataManualRange[0]+abs(m_DataManualRange[0]*0.1);
      scalarRange[1]=m_DataManualRange[1]-abs(m_DataManualRange[1]*0.1);
    }
    scalarArrayLine->InsertNextTuple1(scalarRange[0]);
    scalarArrayLine->InsertNextTuple1(scalarRange[1]);

    vtkDEL(m_TimeLine);
    vtkNEW(m_TimeLine);
    m_TimeLine->SetDimensions(2, 1, 1);
    m_TimeLine->SetXCoordinates(lineArray);
    m_TimeLine->GetPointData()->SetScalars(scalarArrayLine); 
    m_TimeLine->Update();

    vtkDEL(lineArray);
    vtkDEL(scalarArrayLine);

    m_VtkData.pop_back();
    m_VtkData.push_back(m_TimeLine);

    m_PlotTimeLineActor->AddInput((vtkDataSet*)m_TimeLine);
    m_RenFront->AddActor2D(m_PlotTimeLineActor);
    
    CreateLegend();
  }
}
//----------------------------------------------------------------------------
void mafPipeGraph::SetSignalToPlot(int index,bool plot)
//----------------------------------------------------------------------------
{
  //m_CheckBox->CheckItem(index,plot);
  if (index < m_CheckedVector.size())
  {
    //m_CheckedVector.assign(index, plot);
    m_CheckedVector.at(index) = plot;
  }
}
//----------------------------------------------------------------------------
void mafPipeGraph::SetTitleX(mafString title)
//----------------------------------------------------------------------------
{
  m_TitileX = title.GetCStr();
  m_PlotActor->SetXTitle(m_TitileX);
  m_PlotTimeLineActor->SetXTitle(m_TitileX);
  ChangeAxisTitle();
}
//----------------------------------------------------------------------------
void mafPipeGraph::SetTitleY(mafString title)
//----------------------------------------------------------------------------
{
  m_TitileY = title.GetCStr();
  m_PlotActor->SetYTitle(m_TitileY);
  m_PlotTimeLineActor->SetYTitle(m_TitileY);
  ChangeAxisTitle();
}
//----------------------------------------------------------------------------
void mafPipeGraph::SetTitle(mafString title)
//----------------------------------------------------------------------------
{
  m_Title = title.GetCStr();
  m_PlotActor->SetTitle(m_Title);
  m_PlotTimeLineActor->SetTitle(m_Title);
}
