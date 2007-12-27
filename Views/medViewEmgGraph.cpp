/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medViewEmgGraph.cpp,v $
  Language:  C++
  Date:      $Date: 2007-12-27 13:04:30 $
  Version:   $Revision: 1.1 $
  Authors:   Roberto Mucci
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

#include "medViewEmgGraph.h"
#include "mmgGui.h"
#include "mafRWI.h"
#include "mafSceneGraph.h"
#include "mafSceneNode.h"
#include "mafNode.h"
#include "mafNodeIterator.h"
#include "mafVMEImage.h"
#include "mafPipeFactory.h"
#include "medPipeGraph.h"

#include "vtkMAFSmartPointer.h"
#include "vtkTextProperty.h"
#include "vtkDoubleArray.h"
#include "vtkXYPlotActor.h"
#include "vtkProperty2D.h"
#include "vtkRenderer.h"
#include "vtkPointData.h"
#include "vtkRectilinearGrid.h"
#include "vtkLegendBoxActor.h"
#include "vtkLookupTable.h"
#include "vtkRayCast3DPicker.h"
#include "vtkCellPicker.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(medViewEmgGraph);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medViewEmgGraph::medViewEmgGraph( wxString label)
:mafViewVTK(label,CAMERA_RX_FRONT,false)
//----------------------------------------------------------------------------
{
  m_Xmin			= 0;
  m_Xmax			= 0;
  m_Ymin			= 0;
  m_Ymax			= 0;

  m_PlotActor	= NULL;
  m_LegendBoxActor = NULL;

  m_Legend = false;
  m_InputPresent = false;

  m_XTitle		= "Time";
  m_YTitle		= "";
}
//----------------------------------------------------------------------------
medViewEmgGraph::~medViewEmgGraph()
//----------------------------------------------------------------------------
{
  if (m_InputPresent)
  {
    m_PlotActor->RemoveAllInputs();
    m_InputPresent = false;
  }

  if(m_Rwi && m_PlotActor)
    m_Rwi->m_RenFront->RemoveActor2D(m_PlotActor);
  if(m_Rwi && m_OldColour)
    m_Rwi->m_RenFront->SetBackground(m_OldColour);

  vtkDEL(m_PlotActor);
  cppDEL(m_Sg);
  cppDEL(m_Rwi); 
}
//----------------------------------------------------------------------------
mafView *medViewEmgGraph::Copy(mafObserver *Listener)
//----------------------------------------------------------------------------
{
  medViewEmgGraph *v = new medViewEmgGraph(m_Label);
  v->m_Listener = Listener;
  v->m_Id = m_Id;
  v->m_PipeMap = m_PipeMap;
  v->Create();
  return v;
}

//----------------------------------------------------------------------------
void medViewEmgGraph::Create()
//----------------------------------------------------------------------------
{
  m_Rwi = new mafRWI(mafGetFrame());
  m_Rwi->SetListener(this);
  m_Rwi->CameraSet(m_CameraPosition);
  m_Win = m_Rwi->m_RwiBase;

  m_Sg  = new mafSceneGraph(this,m_Rwi->m_RenFront,m_Rwi->m_RenBack);
  m_Sg->SetListener(this);
  m_Rwi->m_Sg = m_Sg;

  m_Rwi->m_RenFront->GetBackground(m_OldColour); // Save the old Color so we can restore it
  m_Rwi->m_RenFront->SetBackground(255,255,255); 

  vtkNEW(m_Picker3D);
  vtkNEW(m_Picker2D);
  m_Picker2D->SetTolerance(0.005);
  m_Picker2D->InitializePickList();
}

//----------------------------------------------------------------------------
void medViewEmgGraph::VmeCreatePipe(mafNode *vme)
//----------------------------------------------------------------------------
{
  mafString pipeName = "";
  GetVisualPipeName(vme, pipeName);

  mafSceneNode *n = m_Sg->Vme2Node(vme);
  assert(n && !n->m_Pipe);

  if (pipeName != "")
  {
    mafPipeFactory *pipe_factory  = mafPipeFactory::GetInstance();
    assert(pipe_factory!=NULL);
    mafObject *obj= NULL;
    obj = pipe_factory->CreateInstance(pipeName);
    mafPipe *pipe = (mafPipe*)obj;
    if (pipe)
    {
      pipe->SetListener(this);
    }
    n->m_Pipe = (mafPipe*)pipe;
  }

  vtkNEW(m_PlotActor);
  m_PlotActor->GetProperty()->SetColor(0.02,0.06,0.62);	
  m_PlotActor->GetProperty()->SetLineWidth(1.4);
  m_PlotActor->SetLabelFormat("%g");
  m_PlotActor->SetTitle(vme->GetParent()->GetName());

  vtkTextProperty* tProp = m_PlotActor->GetTitleTextProperty();
  tProp->SetColor(0.02,0.06,0.62);
  tProp->SetFontFamilyToArial();
  tProp->ItalicOff();
  tProp->BoldOff();
  tProp->SetFontSize(2);

  m_PlotActor->SetAxisTitleTextProperty(tProp);
  m_PlotActor->SetAxisLabelTextProperty(tProp);
  m_PlotActor->SetTitleTextProperty(tProp);	
  m_PlotActor->SetXTitle(m_XTitle);
  m_PlotActor->SetYTitle(m_YTitle);
  m_PlotActor->SetNumberOfLabels(20); //doesn't seem to work!!

  m_LegendBoxActor = m_PlotActor->GetLegendBoxActor();
  m_PlotActor->SetLegendPosition(0.75, 0.85); //Set position and size of the Legend Box
  m_PlotActor->SetLegendPosition2(0.35, 0.25);
  m_PlotActor->SetPosition(0.01,0.01);
  m_PlotActor->SetPosition2(0.9,0.9);
  m_PlotActor->SetVisibility(1);
  m_PlotActor->SetXValuesToValue();

}
//----------------------------------------------------------------------------
void medViewEmgGraph::VmeDeletePipe(mafNode *vme)
//----------------------------------------------------------------------------
{
  if(m_Rwi && m_PlotActor)
    m_Rwi->m_RenFront->RemoveActor2D(m_PlotActor);

  if  (m_InputPresent)
  {
    m_PlotActor->RemoveAllInputs();
    m_InputPresent = false;
  }

  mafSceneNode *n = m_Sg->Vme2Node(vme);
  assert(n && n->m_Pipe);
  cppDEL(n->m_Pipe);
}
//----------------------------------------------------------------------------
mmgGui* medViewEmgGraph::CreateGui()
//----------------------------------------------------------------------------
{
  if(m_Gui == NULL) 
    m_Gui = new mmgGui(this);

  m_Gui->Bool(ID_LEGEND,_("Legend"),&m_Legend,0,_("Show legend"));
  m_Gui->Divider(2);
  return m_Gui;
}

//----------------------------------------------------------------------------
void medViewEmgGraph::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
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
    default:
      mafEventMacro(*e);
    }  
  }
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}

//----------------------------------------------------------------------------
void medViewEmgGraph::VmeShow(mafNode *node, bool show)
//----------------------------------------------------------------------------
{
  if  (m_InputPresent)
    m_PlotActor->RemoveAllInputs();

  std::vector<mafString> legendVec;
  legendVec.clear();

  if (((mafVME *)node)->IsA("mafVMEScalar"))
  {
    Superclass::VmeShow(node, show);
    CameraUpdate();

    mafString pipeName;
    int counter = 0;
    mafNodeIterator *iter = node->GetParent()->NewIterator();
    m_Rwi->m_RenFront->AddActor2D(m_PlotActor);

    for (mafNode *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
    {
      mafSceneNode *n = m_Sg->Vme2Node(node);
      if (node->IsA("mafVMEScalar") && n && n->m_Pipe)
      {
        legendVec.push_back(node->GetName());

        int randomColorR, randomColorG, randomColorB;
        randomColorR = rand() % 255;
        randomColorG = rand() % 255;
        randomColorB = rand() % 255;
        m_PlotActor->SetPlotColor(counter ,randomColorR,randomColorG,randomColorB); 
        counter++;

        medPipeGraph *pipe_graph = (medPipeGraph *)n->m_Pipe;
        vtkRectilinearGrid *rectGrid = pipe_graph->GetData(node);

        double timesRange[2];
        rectGrid->GetXCoordinates()->GetRange(timesRange);

        double dataRange[2];
        m_PlotActor->AddInput(rectGrid);
        rectGrid->GetPointData()->GetScalars()->GetRange(dataRange);

        if(m_Ymin > dataRange[0])
          m_Ymin = dataRange[0];

        if(m_Ymax < dataRange[1])
          m_Ymax = dataRange[1];

        if(m_Xmin > timesRange[0])
          m_Xmin = timesRange[0];

        if(m_Xmax < timesRange[1])
          m_Xmax = timesRange[1];

        m_PlotActor->SetPlotRange(m_Xmin, m_Ymin, m_Xmax, m_Ymax);
      }
    }
    iter->Delete();

    m_Xmin = 0;
    m_Ymin = 0;
    m_Xmax = 0;
    m_Ymax = 0;

    //create legend
    if  (legendVec.size() > 0)
    {
      m_InputPresent = true;
      m_LegendBoxActor->SetNumberOfEntries(legendVec.size());
      for (int n = 0; n < legendVec.size(); n++)
      {
        m_LegendBoxActor->SetEntryString(n,legendVec[n]);
      }
    }
    else
    {
      m_InputPresent = false;
    }

    if (m_Legend)
    {
      m_PlotActor->LegendOn();
    }
  }
}
