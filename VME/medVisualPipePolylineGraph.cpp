/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medVisualPipePolylineGraph.cpp,v $
Language:  C++
Date:      $Date: 2008-02-08 09:59:07 $
Version:   $Revision: 1.1 $
Authors:   DMatteo Giacomoni
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)

MafMedical Library use license agreement

The software named MafMedical Library and any accompanying documentation, 
manuals or data (hereafter collectively "SOFTWARE") is property of the SCS s.r.l.
This is an open-source copyright as follows:
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.
* Modified source versions must be plainly marked as such, and must not be misrepresented 
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MafMedical is partially based on OpenMAF.
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medVisualPipePolylineGraph.h"
#include "mafSceneNode.h"
#include "mmgGui.h"
#include "mafAxes.h"
#include "mmaMaterial.h"
#include "mafVMEOutputPolyline.h"
#include "mafEventSource.h"

#include "mafDataVector.h"
#include "mafVMEGenericAbstract.h"
#include "medVMEPolylineGraph.h"
#include "mafTagArray.h"
#include "mafVMEItemVTK.h"

#include "vtkMAFAssembly.h"

#include "vtkPointData.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkDataSetMapper.h"
#include "vtkPolyData.h"
#include "vtkProperty.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkColorTransferFunction.h"
#include "vtkCellData.h"
#include "vtkSphereSource.h"
#include "vtkGlyph3D.h"
#include "vtkTubeFilter.h"
#include "vtkAppendPolyData.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(medVisualPipePolylineGraph);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medVisualPipePolylineGraph::medVisualPipePolylineGraph()
:mafPipe()
//----------------------------------------------------------------------------
{
  m_Mapper          = NULL;
  m_Actor           = NULL;
  m_OutlineBox      = NULL;
  m_OutlineMapper   = NULL;
  m_OutlineProperty = NULL;
  m_OutlineActor    = NULL;
  m_Gui             = NULL;

  m_ScalarIndex = 0;
  m_NumberOfArrays = 0;
  m_Table						= NULL;

  m_ActiveScalarType = POINT_TYPE;
  m_PointCellArraySeparation = 0;

  m_ScalarsName = NULL;
  m_ScalarsVTKName = NULL;

  m_Sphere          = NULL;
  m_Glyph           = NULL;
  m_Tube            = NULL;

  m_Representation = TUBE;

}
//----------------------------------------------------------------------------
void medVisualPipePolylineGraph::Create(mafSceneNode *n)
//----------------------------------------------------------------------------
{
  Superclass::Create(n);

  m_Selected = false;
  m_Mapper          = NULL;
  m_Actor           = NULL;
  m_OutlineBox      = NULL;
  m_OutlineMapper   = NULL;
  m_OutlineProperty = NULL;
  m_OutlineActor    = NULL;
  m_Axes            = NULL;

  m_Vme->GetEventSource()->AddObserver(this);

  InitializeFromTag();

  ExecutePipe();

  AddActorsToAssembly(m_AssemblyFront);

  if(m_RenFront)
    m_Axes = new mafAxes(m_RenFront, m_Vme);
}
//----------------------------------------------------------------------------
void medVisualPipePolylineGraph::ExecutePipe()
//----------------------------------------------------------------------------
{
  m_Vme->Update();
  m_Vme->GetOutput()->GetVTKData()->Update();

  CreateFieldDataControlArrays();

  assert(m_Vme->GetOutput()->IsMAFType(mafVMEOutputPolyline));
  mafVMEOutputPolyline *poly_output = mafVMEOutputPolyline::SafeDownCast(m_Vme->GetOutput());
  assert(poly_output);
  poly_output->Update();
  vtkPolyData *data = vtkPolyData::SafeDownCast(poly_output->GetVTKData());
  assert(data);
  data->Update();

  vtkNEW(m_Sphere);
  m_Sphere->SetRadius(m_SphereRadius);
  m_Sphere->SetPhiResolution(m_SphereResolution);
  m_Sphere->SetThetaResolution(m_SphereResolution);

  vtkNEW(m_Glyph);
  m_Glyph->SetInput(data);
  m_Glyph->SetSource(m_Sphere->GetOutput());
  m_Glyph->NormalizeScalingOn();
  m_Glyph->SetScaleModeToScaleByScalar();

  vtkNEW(m_Tube);
  m_Tube->UseDefaultNormalOff();
  m_Tube->SetInput(data);
  m_Tube->SetRadius(m_TubeRadius);
  m_Tube->SetCapping(m_Capping);
  m_Tube->SetNumberOfSides(m_TubeResolution);

  m_PointCellArraySeparation = data->GetPointData()->GetNumberOfArrays();
  m_NumberOfArrays = m_PointCellArraySeparation + data->GetCellData()->GetNumberOfArrays();

  // point type scalars
  vtkDataArray *scalars = data->GetPointData()->GetScalars();

  double sr[2] = {0,1};
  if(scalars)
  {
    scalars->Modified();
    scalars->GetRange(sr);
    m_ActiveScalarType = POINT_TYPE;
  }
  else
  {
    scalars = data->GetCellData()->GetScalars();
    if(scalars)
    {
      scalars->Modified();
      scalars->GetRange(sr);
      m_ActiveScalarType = CELL_TYPE;
    }
  }

  vtkNEW(m_Table);
  m_Table->AddRGBPoint(sr[0],0.0,0.0,1.0);
  m_Table->AddRGBPoint((sr[0]+sr[1])/2,0.0,1.0,0.0);;
  m_Table->AddRGBPoint(sr[1],1.0,0.0,0.0);
  m_Table->Build();

  /*m_Table->SetValueRange(sr);
  m_Table->SetHueRange(0.667, 0.0);
  m_Table->SetTableRange(sr);
  m_Table->Build();*/

  vtkNEW(m_Mapper);

  if (m_Representation == TUBE)
  {
    m_Tube->Update();
    m_Mapper->SetInput(m_Tube->GetOutput());
  }
  else if (m_Representation == GLYPH)
  {
    m_Glyph->Update();
    vtkAppendPolyData *apd = vtkAppendPolyData::New();
    apd->AddInput(data);
    apd->AddInput(m_Glyph->GetOutput());
    apd->Update();
    m_Mapper->SetInput(apd->GetOutput());
    apd->Delete();
  }
  else if (m_Representation == GLYPH_UNCONNECTED)
  {
    m_Glyph->Update();
    vtkAppendPolyData *apd = vtkAppendPolyData::New();
    apd->AddInput(m_Glyph->GetOutput());
    apd->Update();
    m_Mapper->SetInput(m_Glyph->GetOutput());
    apd->Delete();
  }
  else
  {
    vtkAppendPolyData *apd = vtkAppendPolyData::New();
    apd->AddInput(data);
    apd->Update();
    m_Mapper->SetInput(apd->GetOutput());
    apd->Delete();
  }


  m_Mapper->ImmediateModeRenderingOn();
  m_Mapper->SetColorModeToMapScalars();
  m_Mapper->SetLookupTable(m_Table);
  m_Mapper->SetScalarRange(sr);

  if(m_ActiveScalarType == POINT_TYPE)
    m_Mapper->SetScalarModeToUsePointData();
  if(m_ActiveScalarType == CELL_TYPE)
    m_Mapper->SetScalarModeToUseCellData();

  m_Mapper->ScalarVisibilityOn();

  m_Mapper->Update();

  vtkNEW(m_Actor);
  m_Actor->SetMapper(m_Mapper);

  // selection highlight
  vtkNEW(m_OutlineBox);
  m_OutlineBox->SetInput(data);  

  vtkNEW(m_OutlineMapper);
  m_OutlineMapper->SetInput(m_OutlineBox->GetOutput());

  vtkNEW(m_OutlineProperty);
  m_OutlineProperty->SetColor(1,1,1);
  m_OutlineProperty->SetAmbient(1);
  m_OutlineProperty->SetRepresentationToWireframe();
  m_OutlineProperty->SetInterpolationToFlat();

  vtkNEW(m_OutlineActor);
  m_OutlineActor->SetMapper(m_OutlineMapper);
  m_OutlineActor->VisibilityOff();
  m_OutlineActor->PickableOff();
  m_OutlineActor->SetProperty(m_OutlineProperty);

}
//----------------------------------------------------------------------------
void medVisualPipePolylineGraph::InitializeFromTag()
//----------------------------------------------------------------------------
{
  mafTagItem *item = NULL;
  if (!m_Vme->GetTagArray()->IsTagPresent("REPRESENTATION"))
  {
    item = new mafTagItem();
    item->SetName("REPRESENTATION");
    item->SetValue(0);
    m_Vme->GetTagArray()->SetTag(*item);
    cppDEL(item);
  }
  item = m_Vme->GetTagArray()->GetTag("REPRESENTATION");
  m_Representation = (int)item->GetValueAsDouble();

  if (!m_Vme->GetTagArray()->IsTagPresent("SPHERE_RADIUS"))
  {
    item = new mafTagItem();
    item->SetName("SPHERE_RADIUS");
    item->SetValue(1.0);
    m_Vme->GetTagArray()->SetTag(*item);
    cppDEL(item);
  }
  item = m_Vme->GetTagArray()->GetTag("SPHERE_RADIUS");
  m_SphereRadius = item->GetValueAsDouble();

  if (!m_Vme->GetTagArray()->IsTagPresent("SPHERE_RESOLUTION"))
  {
    item = new mafTagItem();
    item->SetName("SPHERE_RESOLUTION");
    item->SetValue(10.0);
    m_Vme->GetTagArray()->SetTag(*item);
    cppDEL(item);
  }
  item = m_Vme->GetTagArray()->GetTag("SPHERE_RESOLUTION");
  m_SphereResolution = item->GetValueAsDouble();

  if (!m_Vme->GetTagArray()->IsTagPresent("TUBE_RADIUS"))
  {
    item = new mafTagItem();
    item->SetName("TUBE_RADIUS");
    item->SetValue(1.0);
    m_Vme->GetTagArray()->SetTag(*item);
    cppDEL(item);
  }
  item = m_Vme->GetTagArray()->GetTag("TUBE_RADIUS");
  m_TubeRadius = item->GetValueAsDouble();

  if (!m_Vme->GetTagArray()->IsTagPresent("TUBE_RESOLUTION"))
  {
    item = new mafTagItem();
    item->SetName("TUBE_RESOLUTION");
    item->SetValue(10.0);
    m_Vme->GetTagArray()->SetTag(*item);
    cppDEL(item);
  }
  item = m_Vme->GetTagArray()->GetTag("TUBE_RESOLUTION");
  m_TubeResolution = item->GetValueAsDouble();

  if (!m_Vme->GetTagArray()->IsTagPresent("TUBE_CAPPING"))
  {
    item = new mafTagItem();
    item->SetName("TUBE_CAPPING");
    item->SetValue(0);
    m_Vme->GetTagArray()->SetTag(*item);
    cppDEL(item);
  }
  item = m_Vme->GetTagArray()->GetTag("TUBE_CAPPING");
  m_Capping = (int)item->GetValueAsDouble();
}
//----------------------------------------------------------------------------
void medVisualPipePolylineGraph::AddActorsToAssembly(vtkMAFAssembly *assembly)
//----------------------------------------------------------------------------
{
  assembly->AddPart(m_Actor);
  assembly->AddPart(m_OutlineActor);	
}
//----------------------------------------------------------------------------
void medVisualPipePolylineGraph::RemoveActorsFromAssembly(vtkMAFAssembly *assembly)
//----------------------------------------------------------------------------
{
  assembly->RemovePart(m_Actor);
  assembly->RemovePart(m_OutlineActor);
}
//----------------------------------------------------------------------------
medVisualPipePolylineGraph::~medVisualPipePolylineGraph()
//----------------------------------------------------------------------------
{
  RemoveActorsFromAssembly(m_AssemblyFront);

  m_Vme->GetEventSource()->RemoveObserver(this);

  vtkDEL(m_Sphere);
  vtkDEL(m_Glyph);
  vtkDEL(m_Tube);
  vtkDEL(m_Table);
  vtkDEL(m_Mapper);
  vtkDEL(m_Actor);
  vtkDEL(m_OutlineBox);
  vtkDEL(m_OutlineMapper);
  vtkDEL(m_OutlineProperty);
  vtkDEL(m_OutlineActor);
  cppDEL(m_Axes);

  delete []m_ScalarsName;
  delete []m_ScalarsVTKName;
}
//----------------------------------------------------------------------------
void medVisualPipePolylineGraph::Select(bool sel)
//----------------------------------------------------------------------------
{
  m_Selected = sel;
  if(m_Actor->GetVisibility()) 
  {
    m_OutlineActor->SetVisibility(sel);
  }
}
//----------------------------------------------------------------------------
void medVisualPipePolylineGraph::UpdateProperty(bool fromTag)
//----------------------------------------------------------------------------
{
  mafVMEOutputPolyline *out_polyline = mafVMEOutputPolyline::SafeDownCast(m_Vme->GetOutput());
  out_polyline->Update();
  vtkPolyData *data = vtkPolyData::SafeDownCast(out_polyline->GetVTKData());

  if (m_Representation == TUBE)
  {
    m_Tube->Update();
    m_Mapper->SetInput(m_Tube->GetOutput());
  }
  else if (m_Representation == GLYPH)
  {
    if(!m_ScalarDim)
      m_Glyph->SetScaleModeToDataScalingOff();
    else
      m_Glyph->SetScaleModeToScaleByScalar();

    //m_Glyph->SetScaleFactor(m_SphereRadius);

    m_Glyph->Update();
    m_Glyph->Modified();
    vtkAppendPolyData *apd = vtkAppendPolyData::New();
    apd->AddInput(m_Glyph->GetOutput());
    apd->AddInput(data);
    apd->Update();
    m_Mapper->SetInput(apd->GetOutput());
    apd->Delete();
  }
  else if (m_Representation == GLYPH_UNCONNECTED)
  {
    if(!m_ScalarDim)
      m_Glyph->SetScaleModeToDataScalingOff();
    else
      m_Glyph->SetScaleModeToScaleByScalar();

    //m_Glyph->SetScaleFactor(m_SphereRadius);

    m_Glyph->Update();
    m_Glyph->Modified();
    vtkAppendPolyData *apd = vtkAppendPolyData::New();
    apd->AddInput(m_Glyph->GetOutput());
    apd->Update();
    m_Mapper->SetInput(apd->GetOutput());
    apd->Delete();
  }
  else
  {
    vtkAppendPolyData *apd = vtkAppendPolyData::New();
    apd->AddInput(data);
    apd->Update();
    m_Mapper->SetInput(apd->GetOutput());
    apd->Delete();
  }
}
//----------------------------------------------------------------------------
mmgGui *medVisualPipePolylineGraph::CreateGui()
//----------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = new mmgGui(this);

  const wxString representation_string[] = {_("line"), _("tube"), _("sphere"), _("unconnected sphere")};
  int num_choices = 4;
  m_Gui->Combo(ID_POLYLINE_REPRESENTATION,"",&m_Representation,num_choices,representation_string);

  m_Gui->Combo(ID_SCALARS,"",&m_ScalarIndex,m_NumberOfArrays,m_ScalarsName);	
  
  m_Gui->Label(_("sphere"));
  m_Gui->Bool(ID_SCALAR_DIMENSION,_("scalar dim."),&m_ScalarDim,0,_("Check to scale the sphere radius proportional to the selected scalars"));
  m_Gui->Double(ID_SPHERE_RADIUS,_("radius"),&m_SphereRadius,0);
  m_Gui->Double(ID_SPHERE_RESOLUTION,_("resolution"),&m_SphereResolution,0);

  m_Gui->Label(_("tube"));
  m_Gui->Double(ID_TUBE_RADIUS,_("radius"),&m_TubeRadius,0);
  m_Gui->Double(ID_TUBE_RESOLUTION,_("resolution"),&m_TubeResolution,0);
  m_Gui->Bool(ID_TUBE_CAPPING,_("capping"),&m_Capping);
  m_Gui->Divider(2);

  m_Gui->Enable(ID_TUBE_RADIUS, m_Representation == TUBE);
  m_Gui->Enable(ID_TUBE_CAPPING, m_Representation == TUBE);
  m_Gui->Enable(ID_TUBE_RESOLUTION, m_Representation == TUBE);
  m_Gui->Enable(ID_SPHERE_RADIUS, m_Representation == GLYPH || m_Representation == GLYPH_UNCONNECTED);
  m_Gui->Enable(ID_SPHERE_RESOLUTION, m_Representation == GLYPH || m_Representation == GLYPH_UNCONNECTED);

  m_Gui->Divider();
  m_Gui->Label("");
  m_Gui->Update();
  return m_Gui;
}
//----------------------------------------------------------------------------
void medVisualPipePolylineGraph::SetRepresentation(int representation)
//----------------------------------------------------------------------------
{
  if (representation < POLYLINE)
  {
    m_Representation = POLYLINE;
  }
  else if (representation > GLYPH_UNCONNECTED)
  {
    m_Representation = GLYPH_UNCONNECTED;
  }
  else
    m_Representation = representation;

  if (m_Gui)
  {
    m_Gui->Enable(ID_TUBE_RADIUS, m_Representation == TUBE);
    m_Gui->Enable(ID_TUBE_CAPPING, m_Representation == TUBE);
    m_Gui->Enable(ID_TUBE_RESOLUTION, m_Representation == TUBE);
    m_Gui->Enable(ID_SPHERE_RADIUS, m_Representation == GLYPH || m_Representation == GLYPH_UNCONNECTED);
    m_Gui->Enable(ID_SPHERE_RESOLUTION, m_Representation == GLYPH || m_Representation == GLYPH_UNCONNECTED);
  }
  UpdateProperty();
}
//----------------------------------------------------------------------------
void medVisualPipePolylineGraph::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId()) 
    {
    case ID_TUBE_RADIUS:
      {
        m_Tube->SetRadius(m_TubeRadius);
        mafTagItem *item = m_Vme->GetTagArray()->GetTag("TUBE_RADIUS");
        item->SetValue(m_TubeRadius);
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      }
      break;
    case ID_TUBE_CAPPING:
      {
        m_Tube->SetCapping(m_Capping);
        mafTagItem *item = m_Vme->GetTagArray()->GetTag("TUBE_CAPPING");
        item->SetValue(m_Capping);
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      }
      break;
    case ID_TUBE_RESOLUTION:
      {
        m_Tube->SetNumberOfSides(m_TubeResolution);
        mafTagItem *item = m_Vme->GetTagArray()->GetTag("TUBE_RESOLUTION");
        item->SetValue(m_TubeResolution);
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      }
      break;
    case ID_SPHERE_RADIUS:
      {
        m_Sphere->SetRadius(m_SphereRadius);
        mafTagItem *item = m_Vme->GetTagArray()->GetTag("SPHERE_RADIUS");
        item->SetValue(m_SphereRadius);
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      }
      break;
    case ID_SPHERE_RESOLUTION:
      {
        m_Sphere->SetPhiResolution(m_SphereResolution);
        m_Sphere->SetThetaResolution(m_SphereResolution);
        mafTagItem *item = m_Vme->GetTagArray()->GetTag("SPHERE_RESOLUTION");
        item->SetValue(m_SphereResolution);
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      }
      break;
    case ID_POLYLINE_REPRESENTATION:
      {
        SetRepresentation(m_Representation);
        mafTagItem *item = m_Vme->GetTagArray()->GetTag("REPRESENTATION");
        item->SetValue(m_Representation);
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      }
      break;
    case ID_SCALAR_DIMENSION:
      {
        UpdateProperty();
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      }
      break;
    case ID_SCALARS:
      {
        if(m_ScalarIndex < m_PointCellArraySeparation)
        {
          m_ActiveScalarType = POINT_TYPE;
        }
        else 
        {
          m_ActiveScalarType = CELL_TYPE;
        }
        UpdateScalars();
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      }
      break;
    case ID_LUT:
      {
        double sr[2];
        //m_Table->GetTableRange(sr);
        m_Mapper->SetScalarRange(sr);
      }
      mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      break;
    default:
      mafEventMacro(*e);
      break;
    }
  }
  else if(maf_event->GetId() == VME_TIME_SET)
  {
    /*UpdatePipeFromScalars();
    UpdateScalars();*/
    UpdatePipeFromScalars();
  }
}
//----------------------------------------------------------------------------
void medVisualPipePolylineGraph::SetActorPicking(int enable)
//----------------------------------------------------------------------------
{
  m_Actor->SetPickable(enable);
  m_Actor->Modified();
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void medVisualPipePolylineGraph::UpdateScalars()
//----------------------------------------------------------------------------
{

  m_Vme->GetOutput()->GetVTKData()->Update();
  m_Vme->Update();

  if(m_ActiveScalarType == POINT_TYPE)
  {
    m_Vme->GetOutput()->GetVTKData()->GetPointData()->SetActiveScalars(m_ScalarsVTKName[m_ScalarIndex].c_str());
    m_Vme->GetOutput()->GetVTKData()->GetPointData()->GetScalars()->Modified();
  }
  else if(m_ActiveScalarType == CELL_TYPE)
  {
    m_Vme->GetOutput()->GetVTKData()->GetCellData()->SetActiveScalars(m_ScalarsVTKName[m_ScalarIndex].c_str());
    m_Vme->GetOutput()->GetVTKData()->GetCellData()->GetScalars()->Modified();
  }
  m_Vme->Modified();
  m_Vme->GetOutput()->GetVTKData()->Update();
  m_Vme->Update();


  for (mafDataVector::Iterator it = ((medVMEPolylineGraph *)m_Vme)->GetDataVector()->Begin(); it != ((medVMEPolylineGraph *)m_Vme)->GetDataVector()->End(); it++)
  {
    mafVMEItemVTK *item = mafVMEItemVTK::SafeDownCast(it->second);
    assert(item);

    vtkPolyData *outputVTK = vtkPolyData::SafeDownCast(item->GetData());
    if(outputVTK)
    {
      if(m_ActiveScalarType == POINT_TYPE)
      {
        outputVTK->GetPointData()->SetActiveScalars(m_ScalarsVTKName[m_ScalarIndex].c_str());
        outputVTK->GetPointData()->GetScalars()->Modified();
      }
      else if(m_ActiveScalarType == CELL_TYPE)
      {
        outputVTK->GetCellData()->SetActiveScalars(m_ScalarsVTKName[m_ScalarIndex].c_str());
        outputVTK->GetCellData()->GetScalars()->Modified();
      }
      outputVTK->Modified();
      outputVTK->Update();

    }
  }
  m_Vme->Modified();
  m_Vme->Update();

  UpdatePipeFromScalars();
}
//----------------------------------------------------------------------------
void medVisualPipePolylineGraph::UpdatePipeFromScalars()
//----------------------------------------------------------------------------
{
  vtkPolyData *data = vtkPolyData::SafeDownCast(m_Vme->GetOutput()->GetVTKData());
  data->Update();
  double sr[2];
  if(m_ActiveScalarType == POINT_TYPE)
    data->GetPointData()->GetScalars()->GetRange(sr);
  else if(m_ActiveScalarType == CELL_TYPE)
    data->GetCellData()->GetScalars()->GetRange(sr);

  m_Table->RemoveAllPoints();
  m_Table->AddRGBPoint(sr[0],0.0,0.0,1.0);
  m_Table->AddRGBPoint((sr[0]+sr[1])/2,0.0,1.0,0.0);;
  m_Table->AddRGBPoint(sr[1],1.0,0.0,0.0);
  m_Table->Build();

  m_Glyph->SelectInputScalars(m_ScalarsName[m_ScalarIndex].c_str());
  m_Glyph->SetRange(sr);
  m_Glyph->Update();

  if(m_ActiveScalarType == POINT_TYPE)
    m_Mapper->SetScalarModeToUsePointData();
  if(m_ActiveScalarType == CELL_TYPE)
    m_Mapper->SetScalarModeToUseCellData();

  m_Mapper->SetLookupTable(m_Table);
  m_Mapper->SetScalarRange(sr);
  m_Mapper->Update();

  m_Actor->Modified();

  UpdateProperty();
}

//----------------------------------------------------------------------------
void medVisualPipePolylineGraph::CreateFieldDataControlArrays()
//----------------------------------------------------------------------------
{
  //String array allocation
  int numPointScalars = m_Vme->GetOutput()->GetVTKData()->GetPointData()->GetNumberOfArrays();
  int numCellScalars = m_Vme->GetOutput()->GetVTKData()->GetCellData()->GetNumberOfArrays();

  wxString *tempScalarsPointsName=new wxString[numPointScalars + numCellScalars];
  int count=0;

  int pointArrayNumber;
  for(pointArrayNumber = 0;pointArrayNumber<numPointScalars;pointArrayNumber++)
  {
    if(strcmp(m_Vme->GetOutput()->GetVTKData()->GetPointData()->GetArrayName(pointArrayNumber),"")!=0)
    {
      count++;
      tempScalarsPointsName[count-1]=m_Vme->GetOutput()->GetVTKData()->GetPointData()->GetArrayName(pointArrayNumber);
    }
  }
  for(int cellArrayNumber=0;cellArrayNumber<numCellScalars;cellArrayNumber++)
  {
    if(strcmp(m_Vme->GetOutput()->GetVTKData()->GetCellData()->GetArrayName(cellArrayNumber),"")!=0)
    {
      count++;
      tempScalarsPointsName[count-1]=m_Vme->GetOutput()->GetVTKData()->GetCellData()->GetArrayName(cellArrayNumber);
    }
  }

  m_ScalarsName = new wxString[count];
  m_ScalarsVTKName = new wxString[count];

  for(int j=0;j<count;j++)
  {
    m_ScalarsVTKName[j]=tempScalarsPointsName[j];
    if(j<pointArrayNumber)
      m_ScalarsName[j]="[POINT] " + tempScalarsPointsName[j];
    else
      m_ScalarsName[j]="[CELL] " + tempScalarsPointsName[j];
  }

  m_PointCellArraySeparation = pointArrayNumber;

  delete []tempScalarsPointsName;

}