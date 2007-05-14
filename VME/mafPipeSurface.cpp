/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeSurface.cpp,v $
  Language:  C++
  Date:      $Date: 2007-05-14 15:59:22 $
  Version:   $Revision: 1.37 $
  Authors:   Silvano Imboden - Paolo Quadrani
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

#include "mafPipeSurface.h"
#include "mafSceneNode.h"
#include "mmgGui.h"
#include "mmgMaterialButton.h"
#include "mmgLutPreset.h"
#include "mafAxes.h"
#include "mmaMaterial.h"

#include "mafDataVector.h"
#include "mafVMESurface.h"
#include "mafVMEGenericAbstract.h"

#include "vtkMAFSmartPointer.h"
#include "vtkMAFAssembly.h"

#include "vtkPointData.h"
#include "vtkImageData.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyData.h"
#include "vtkProperty.h"
#include "mafLODActor.h"
#include "vtkRenderer.h"
#include "vtkGlyph3D.h"
#include "vtkPolyDataNormals.h"
#include "vtkActor.h"
#include "vtkLineSource.h"
#include "vtkCellCenters.h"
#include "vtkCellData.h"
#include "vtkArrowSource.h"
#include "vtkFeatureEdges.h"

#include <vector>

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafPipeSurface);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafPipeSurface::mafPipeSurface()
:mafPipe()
//----------------------------------------------------------------------------
{
  m_Mapper          = NULL;
  m_Actor           = NULL;
  m_OutlineBox      = NULL;
  m_OutlineMapper   = NULL;
  m_OutlineProperty = NULL;
  m_OutlineActor    = NULL;
  m_MaterialButton  = NULL;
  m_SurfaceMaterial = NULL;
  m_Gui             = NULL;

	m_Normal							= NULL;
	m_NormalGlyph					= NULL;
	m_NormalMapper				= NULL;
	m_NormalActor					= NULL;
	m_CenterPointsFilter	= NULL;
	m_NormalArrow					= NULL;

  m_ScalarVisibility = 0;
	m_NormalVisibility = 0;
	m_EdgeVisibility	 = 0;
  m_RenderingDisplayListFlag = 0;

  m_UseVTKProperty  = 1;
  m_UseLookupTable  = 0;

  m_EnableActorLOD  = 0;
}
//----------------------------------------------------------------------------
void mafPipeSurface::Create(mafSceneNode *n)
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

  m_Vme->Update();
  assert(m_Vme->GetOutput()->IsMAFType(mafVMEOutputSurface));
  mafVMEOutputSurface *surface_output = mafVMEOutputSurface::SafeDownCast(m_Vme->GetOutput());
  assert(surface_output);
  surface_output->Update();
  vtkPolyData *data = vtkPolyData::SafeDownCast(surface_output->GetVTKData());
  assert(data);
  data->Update();
  vtkDataArray *scalars = data->GetPointData()->GetScalars();
  double sr[2] = {0,1};
  if(scalars != NULL)
  {
    m_ScalarVisibility = 1;
    scalars->GetRange(sr);
  }

  m_SurfaceMaterial = surface_output->GetMaterial();
  assert(m_SurfaceMaterial);  // all vme that use PipeSurface must have the material correctly set

  vtkNEW(m_Mapper);
  m_Mapper->SetInput(data);
  
  m_RenderingDisplayListFlag = m_Vme->IsAnimated() ? 1 : 0;
  m_Mapper->SetImmediateModeRendering(m_RenderingDisplayListFlag);
  m_Mapper->SetScalarVisibility(m_ScalarVisibility);
  m_Mapper->SetScalarRange(sr);

  vtkNEW(m_Actor);
	m_Actor->SetMapper(m_Mapper);
  m_Actor->SetEnableHighThreshold(m_EnableActorLOD);
  if (m_SurfaceMaterial->m_MaterialType == mmaMaterial::USE_LOOKUPTABLE)
  {
    m_UseVTKProperty = 0;
    m_UseLookupTable = 1;
  }
  if (m_SurfaceMaterial->m_MaterialType == mmaMaterial::USE_VTK_PROPERTY)
  {
    m_UseVTKProperty = 1;
    m_UseLookupTable = 0;
    m_Actor->SetProperty(m_SurfaceMaterial->m_Prop);
  }

  if (m_Gui)
  {
    m_Gui->Update();
  }

  m_AssemblyFront->AddPart(m_Actor);

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

  m_AssemblyFront->AddPart(m_OutlineActor);

	m_Axes = new mafAxes(m_RenFront, m_Vme);
	if(m_Vme->IsA("mafVMERefSys"))
		m_Axes->SetVisibility(false);
	
	/*vtkNEW(m_Normal);
	m_Normal->SetInput(data);
	m_Normal->ComputeCellNormalsOn();
	m_Normal->ComputePointNormalsOff();
	m_Normal->Update();*/

	if(data->GetCellData()->GetNormals())
	{
		CreateNormalsPipe();
	}

	CreateEdgesPipe();

}
//----------------------------------------------------------------------------
void mafPipeSurface::CreateEdgesPipe()
//----------------------------------------------------------------------------
{
	mafVMEOutputSurface *surface_output = mafVMEOutputSurface::SafeDownCast(m_Vme->GetOutput());
	surface_output->Update();
	vtkPolyData *data = vtkPolyData::SafeDownCast(surface_output->GetVTKData());
	data->Update();

	vtkNEW(m_ExtractEdges);
	m_ExtractEdges->SetInput(data);
	m_ExtractEdges->SetBoundaryEdges(1);
	m_ExtractEdges->SetFeatureEdges(0);
	m_ExtractEdges->SetNonManifoldEdges(0);
	m_ExtractEdges->SetManifoldEdges(0);
	m_ExtractEdges->Update();

	vtkNEW(m_EdgesMapper);
	m_EdgesMapper->SetInput(m_ExtractEdges->GetOutput());
	m_EdgesMapper->ScalarVisibilityOff();
	m_EdgesMapper->Update();

	vtkNEW(m_EdgesActor);
	m_EdgesActor->SetMapper(m_EdgesMapper);
	m_EdgesActor->PickableOff();
	m_EdgesActor->GetProperty()->SetColor(1-m_Actor->GetProperty()->GetColor()[0],1-m_Actor->GetProperty()->GetColor()[1],1-m_Actor->GetProperty()->GetColor()[2]);
	m_EdgesActor->SetVisibility(m_EdgeVisibility);
	m_EdgesActor->Modified();

	m_AssemblyFront->AddPart(m_EdgesActor);
}
//----------------------------------------------------------------------------
void mafPipeSurface::CreateNormalsPipe()
//----------------------------------------------------------------------------
{
	mafVMEOutputSurface *surface_output = mafVMEOutputSurface::SafeDownCast(m_Vme->GetOutput());
	surface_output->Update();
	vtkPolyData *data = vtkPolyData::SafeDownCast(surface_output->GetVTKData());
	data->Update();

	vtkNEW(m_CenterPointsFilter);
	m_CenterPointsFilter->SetInput(data);
	m_CenterPointsFilter->Update();

	vtkPolyData *centers = m_CenterPointsFilter->GetOutput();
	centers->Update();
	centers->GetPointData()->SetNormals(data->GetCellData()->GetNormals());
	centers->Update();

	double bounds[6];
	data->GetBounds(bounds);
	double maxBounds = (bounds[1]-bounds[0] < bounds[3]-bounds[2])?bounds[1]-bounds[0]:bounds[3]-bounds[2];
	maxBounds = (maxBounds<bounds[5]-bounds[4])?maxBounds:bounds[5]-bounds[4];

	vtkNEW(m_NormalArrow);
	m_NormalArrow->SetTipLength(0.0);
	m_NormalArrow->SetTipRadius(0.0);
	m_NormalArrow->SetShaftRadius(0.005*maxBounds);
	m_NormalArrow->SetTipResolution(16);
	m_NormalArrow->SetShaftResolution(16);
	m_NormalArrow->Update();

	vtkNEW(m_NormalGlyph);
	m_NormalGlyph->SetInput(centers);
	m_NormalGlyph->SetSource(m_NormalArrow->GetOutput());
	m_NormalGlyph->SetVectorModeToUseNormal();
	m_NormalGlyph->Update();

	vtkNEW(m_NormalMapper);
	m_NormalMapper->SetInput(m_NormalGlyph->GetOutput());
	m_NormalMapper->Update();

	vtkNEW(m_NormalActor);
	m_NormalActor->SetMapper(m_NormalMapper);
	m_NormalActor->SetVisibility(m_NormalVisibility);
	m_NormalActor->PickableOff();
	m_NormalActor->Modified();

	m_AssemblyFront->AddPart(m_NormalActor);
}
//----------------------------------------------------------------------------
mafPipeSurface::~mafPipeSurface()
//----------------------------------------------------------------------------
{
	if(m_Actor)
		m_AssemblyFront->RemovePart(m_Actor);
	if(m_OutlineActor)
		m_AssemblyFront->RemovePart(m_OutlineActor);
	if(m_NormalActor)
		m_AssemblyFront->RemovePart(m_NormalActor);
	if(m_EdgesActor)
		m_AssemblyFront->RemovePart(m_EdgesActor);

	vtkDEL(m_Mapper);
  vtkDEL(m_Actor);
  vtkDEL(m_OutlineBox);
  vtkDEL(m_OutlineMapper);
  vtkDEL(m_OutlineProperty);
  vtkDEL(m_OutlineActor);
  cppDEL(m_Axes);
	vtkDEL(m_NormalActor);
	vtkDEL(m_NormalMapper);
	vtkDEL(m_CenterPointsFilter);
	vtkDEL(m_NormalGlyph);
	vtkDEL(m_Normal);
	vtkDEL(m_EdgesActor);
	vtkDEL(m_EdgesMapper);
	vtkDEL(m_ExtractEdges);
}
//----------------------------------------------------------------------------
void mafPipeSurface::Select(bool sel)
//----------------------------------------------------------------------------
{
	m_Selected = sel;
	if(m_Actor->GetVisibility()) 
	{
		m_OutlineActor->SetVisibility(sel);
		if(!m_Vme->IsA("mafVMERefSys"))
			m_Axes->SetVisibility(sel);
	}
}
//----------------------------------------------------------------------------
void mafPipeSurface::UpdateProperty(bool fromTag)
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mmgGui *mafPipeSurface::CreateGui()
//----------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = new mmgGui(this);
  m_Gui->Bool(ID_RENDERING_DISPLAY_LIST,"displaylist",&m_RenderingDisplayListFlag,0,"turn on/off \nrendering displaylist calculation");
  m_Gui->Bool(ID_SCALAR_VISIBILITY,"scalar vis.", &m_ScalarVisibility,0,"turn on/off the scalar visibility");
	m_Gui->Bool(ID_NORMAL_VISIBILITY,"norm. vis.",&m_NormalVisibility);
	m_Gui->Bool(ID_EDGE_VISIBILITY,"edge vis.",&m_EdgeVisibility);
  m_Gui->Divider();
  m_Gui->Bool(ID_USE_VTK_PROPERTY,"property",&m_UseVTKProperty);
  m_MaterialButton = new mmgMaterialButton(m_Vme,this);
  m_Gui->AddGui(m_MaterialButton->GetGui());
  m_MaterialButton->Enable(m_UseVTKProperty != 0);
  m_Gui->Divider();
  m_Gui->Bool(ID_USE_LOOKUP_TABLE,"lut",&m_UseLookupTable);
  double sr[2];
  m_Mapper->GetScalarRange(sr);
  m_SurfaceMaterial->m_ColorLut->SetTableRange(sr);
  m_Gui->Lut(ID_LUT,"lut",m_SurfaceMaterial->m_ColorLut);
  m_Gui->Enable(ID_LUT,m_UseLookupTable != 0);
  m_Gui->Divider(2);
  m_Gui->Bool(ID_ENABLE_LOD,"LOD",&m_EnableActorLOD);
  m_Gui->Label("");

  if (m_SurfaceMaterial == NULL)
  {
    mafVMEOutputSurface *surface_output = mafVMEOutputSurface::SafeDownCast(m_Vme->GetOutput());
    m_SurfaceMaterial = surface_output->GetMaterial();
  }
  m_Gui->Enable(ID_USE_LOOKUP_TABLE, m_SurfaceMaterial->m_ColorLut != NULL);
  m_Gui->Divider();
	return m_Gui;
}
//----------------------------------------------------------------------------
void mafPipeSurface::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId()) 
    {
      case ID_SCALAR_VISIBILITY:
      {
        m_Mapper->SetScalarVisibility(m_ScalarVisibility);
        if (m_ScalarVisibility)
        {
          vtkPolyData *data = (vtkPolyData *)m_Vme->GetOutput()->GetVTKData();
          assert(data);
          double range[2];
          data->GetScalarRange(range);
          m_Mapper->SetScalarRange(range);
        }
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      }
    	break;
      case ID_LUT:
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      break;
      case ID_ENABLE_LOD:
        m_Actor->SetEnableHighThreshold(m_EnableActorLOD);
        m_OutlineActor->SetEnableHighThreshold(m_EnableActorLOD);
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      break;
      case ID_USE_VTK_PROPERTY:
        if (m_UseVTKProperty != 0)
        {
          m_Actor->SetProperty(m_SurfaceMaterial->m_Prop);
        }
        else
        {
          m_Actor->SetProperty(NULL);
        }
        m_MaterialButton->Enable(m_UseVTKProperty != 0);
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      break;
      case ID_USE_LOOKUP_TABLE:
        m_Gui->Enable(ID_LUT,m_UseLookupTable != 0);
      break;
      case ID_RENDERING_DISPLAY_LIST:
        m_Mapper->SetImmediateModeRendering(m_RenderingDisplayListFlag);
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      break;
			case ID_NORMAL_VISIBILITY:
				if(m_NormalActor)
					m_NormalActor->SetVisibility(m_NormalVisibility);
				else if(m_Vme->GetOutput()->GetVTKData()->GetCellData()->GetNormals())
					CreateNormalsPipe();
				mafEventMacro(mafEvent(this,CAMERA_UPDATE));
				break;
			case ID_EDGE_VISIBILITY:
				if(m_EdgesActor)
					m_EdgesActor->SetVisibility(m_EdgeVisibility);
				else
					CreateEdgesPipe();
				mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      default:
        mafEventMacro(*e);
      break;
    }
  }
}
//----------------------------------------------------------------------------
void mafPipeSurface::SetEnableActorLOD(bool value)
//----------------------------------------------------------------------------
{
  m_EnableActorLOD = (int) value;
  if(m_Gui)
    m_Gui->Update();
}
//----------------------------------------------------------------------------
void mafPipeSurface::SetActorPicking(int enable)
//----------------------------------------------------------------------------
{
	m_Actor->SetPickable(enable);
  m_Actor->Modified();
	mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
