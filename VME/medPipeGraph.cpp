/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medPipeGraph.cpp,v $
  Language:  C++
  Date:      $Date: 2007-07-19 12:31:32 $
  Version:   $Revision: 1.5 $
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

#include "medPipeGraph.h"
#include "mafDecl.h"
#include "mafSceneNode.h"
#include "mmgGui.h"
#include "mmgCheckListBox.h"
#include "mmaMaterial.h"
#include "mafMatrixVector.h"

#include "mafScalarInterpolator.h"

#include "mafVME.h"
#include "mmgMaterialButton.h"
#include "mafSceneNode.h"
#include "mafTagArray.h"
#include "mmaMaterial.h"
#include "mafTagItem.h"
#include "mafVMELandmark.h"
#include "mafASCIIImporterUtility.h"
#include "mafVMEOutputScalar.h"

#include "vnl/vnl_matrix.h"
#include <wx/txtstrm.h>
#include <wx/wfstream.h>
#include <wx/string.h>

#include "vtkMAFAssembly.h"
#include "vtkDataArray.h"
#include "vtkDoubleArray.h"
#include "vtkDataSet.h"
#include "vtkXYPlotActor.h"
#include "vtkProperty2D.h"
#include "vtkTextProperty.h"
#include "vtkRenderer.h"
#include "vtkDataSetCollection.h"
#include "vtkPointData.h"
#include "vtkDataObject.h"
#include "vtkRectilinearGrid.h"
#include "medVMEEmg.h"
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
  m_Actor1	= NULL;

  m_CheckBox  = NULL;
  m_Legend    = FALSE;

  m_Xmin			= 0;
  m_Xmax			= 0;
  m_Ymin			= 0;
  m_Ymax			= 0;
  m_Xlabel		= 10;
  m_Ylabel		= 10;

  m_X_title		= "Time";
  m_Y_title		= "Scalar";
 
}
//----------------------------------------------------------------------------
medPipeGraph::~medPipeGraph()
//----------------------------------------------------------------------------
{
  m_RenFront->RemoveActor2D(m_Actor1);
  m_RenFront->SetBackground(m_OldColour);

  vtkDEL(m_Actor1);
  }
//----------------------------------------------------------------------------
void medPipeGraph::Create(mafSceneNode *n)
//----------------------------------------------------------------------------
{
  Superclass::Create(n);
 
  medVMEEmg *m_Emg_plot = medVMEEmg::SafeDownCast(m_Vme);

  m_Emg_plot->Update();
  m_Emg_plot->GetTimeStamps(m_TimeVector);
  
  time_Array = vtkDoubleArray::New();
  
  mafTimeStamp t;
  for (t = 0; t < m_TimeVector.size(); t++)
  {
    time_Array->InsertNextValue(m_TimeVector[t]);
  }  
  
  vtkNEW(m_Actor1);

  m_Actor1->GetProperty()->SetColor(0.02,0.06,0.62);	
  m_Actor1->GetProperty()->SetLineWidth(2);
  m_Actor1->SetLabelFormat("%g");
  m_Actor1->SetTitle(m_Emg_plot->GetName());

  vtkTextProperty* tProp = m_Actor1->GetTitleTextProperty();
  tProp->SetColor(0.02,0.06,0.62);
  tProp->SetFontFamilyToArial();
  tProp->ItalicOff();
  tProp->BoldOff();
  tProp->SetFontSize(6);

  m_Actor1->SetAxisTitleTextProperty(tProp);
  m_Actor1->SetAxisLabelTextProperty(tProp);
  m_Actor1->SetTitleTextProperty(tProp);	
  m_Actor1->SetXTitle(m_X_title);
  m_Actor1->SetYTitle(m_Y_title);

  m_LegendBox_Actor = m_Actor1->GetLegendBoxActor();

  m_Actor1->SetPosition(0.03,0.03);
  m_Actor1->SetPosition2(0.9,0.9);
  m_Actor1->SetVisibility(1);
  m_Actor1->SetXValuesToValue();
  m_Actor1->SetPlotColor(0,1,0,0);
  m_Actor1->SetPlotColor(1,0,1,1);
  m_Actor1->SetPlotColor(2,0,191,255);
  m_Actor1->SetPlotColor(3,112,219,147);
  m_Actor1->SetPlotColor(4,105,105,105);
  m_Actor1->SetPlotColor(5,0,0,128);
  m_Actor1->SetPlotColor(6,0,0,205);
  m_Actor1->SetPlotColor(7,0,100,0);
  m_Actor1->SetPlotColor(8,0,255,0);
  m_Actor1->SetPlotColor(9,255,255,0);
  m_Actor1->SetPlotColor(10,255,130,710);
  m_Actor1->SetPlotColor(11,255,165,790);
  m_Actor1->SetPlotColor(12,124,252,0);
  m_Actor1->SetPlotColor(13,189,183,107);
  m_Actor1->SetPlotColor(14,255,215,0);
  m_Actor1->SetPlotColor(15,188,143,143);
  m_Actor1->SetPlotColor(16,139,69,19);
  m_Actor1->SetPlotColor(17,250,128,114);
  m_Actor1->SetPlotColor(18,255,165,0);
  m_Actor1->SetPlotColor(19,255,105,180);
  m_Actor1->SetPlotColor(20,255,20,147);
  m_Actor1->SetPlotColor(21,255,0,255);
  m_Actor1->SetPlotColor(22,160,32,240);
  m_Actor1->SetPlotColor(23,139,137,137);
  m_Actor1->SetPlotColor(24,255,99,71);
  m_Actor1->SetPlotColor(25,92,92,92);
  m_Actor1->SetPlotColor(26,135,135,135);
  m_Actor1->SetPlotColor(27,176,176,176);
  m_Actor1->SetPlotColor(28,214,214,214);
  m_Actor1->SetPlotColor(29,255,218,185);
  m_Actor1->SetPlotColor(30,240,255,240);
  m_Actor1->SetPlotColor(31,112,128,144);
  m_Actor1->SetPlotColor(32,124,252,0);

  m_RenFront->GetBackground(m_OldColour); // Save the old Color so we can restore it
  m_RenFront->SetBackground(1,1,1);   
}

//----------------------------------------------------------------------------
void medPipeGraph::UpdateGraph()
//----------------------------------------------------------------------------
{
  mafTimeStamp t;
  vtkDoubleArray *scalar;
  //scalar = vtkDoubleArray::New();
  m_vtkData.clear();
  scalar_Array.clear();

  medVMEEmg *m_Emg_plot = medVMEEmg::SafeDownCast(m_Vme);
  
  int x_dim = m_TimeVector.size(); 
  
  int counter_array = 0;
  int counter_legend = 0;
  
  mafString name;

  for (int c = 0; c < 32 ; c++)
  { 
    if (m_CheckBox->IsItemChecked(c))
    {
      name = "analog_" + mafString(c+1);
      m_LegendBox_Actor->SetNumberOfEntries(32);
      m_LegendBox_Actor->SetEntryString(counter_legend,name);
      counter_legend++;
      
      int counter = 0;
      
      scalar = vtkDoubleArray::New();
      
      for (t = 0; t < m_TimeVector.size(); t++)
      { 
        m_Emg_plot->SetTimeStamp(m_TimeVector[t]);
        double scalar_data = m_Emg_plot->GetScalarOutput()->GetScalarData().get(0,c);
        scalar->InsertValue(counter,scalar_data);
        counter++;
      }
      scalar_Array.push_back(scalar);
          
      vtkRectilinearGrid *rect_grid;
      rect_grid = vtkRectilinearGrid::New();

      rect_grid->SetDimensions(x_dim, 1, 1);
      rect_grid->SetXCoordinates(time_Array); 
      rect_grid->GetPointData()->SetScalars(scalar_Array.at(counter_array));
      
      m_vtkData.push_back(rect_grid);
      m_Actor1->AddInput(m_vtkData.at(counter_array));
      counter_array++;
    }    
  }

  double times_range[2];
  time_Array->GetRange(times_range);

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

  m_Actor1->SetPlotRange(m_Xmin, m_Ymin, m_Xmax, m_Ymax); 

  m_Xmin = 0;
  m_Ymin = 0;
  m_Xmax = 0;
  m_Ymax = 0;

  m_Actor1->SetNumberOfXLabels(m_Xlabel);
  m_Actor1->SetNumberOfYLabels(m_Ylabel);

  m_RenFront->AddActor2D(m_Actor1);
}
//----------------------------------------------------------------------------
mmgGui* medPipeGraph::CreateGui()
//----------------------------------------------------------------------------
{
  Superclass::CreateGui();

  if(m_Gui == NULL) 
    m_Gui = new mmgGui(this);

  wxString name;
  bool checked = FALSE;

   m_CheckBox = m_Gui->CheckList(ID_CHECK_BOX,_("Item"),360,_("Chose item to plot"));

  for (int n = 1; n <= 32; n++)
  {
    name = "analog_" + wxString::Format("%d", n);
    m_CheckBox->AddItem(n , name, checked);
  }

  m_Gui->Divider(1);
  m_Gui->Button(ID_DRAW,_("Plot"), _(""),_("Draw selected items"));
  m_Gui->Divider(1);
  m_Gui->Bool(ID_LEGEND,_("Legend"),&m_Legend,0,_("Show legend"));
  m_Gui->Divider();

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
        m_Actor1->RemoveAllInputs();
        UpdateGraph();
      }
      break;
    case ID_LEGEND:
      {
        switch (m_Legend)
        {
        case TRUE:
          {
            m_Actor1->LegendOn();
          }
          break;
        case FALSE:
          {
            m_Actor1->LegendOff();
          } 
          break;
        }
      }
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