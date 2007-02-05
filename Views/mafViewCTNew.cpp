/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafViewCTNew.cpp,v $
Language:  C++
Date:      $Date: 2007-02-05 20:48:31 $
Version:   $Revision: 1.1 $
Authors:   Daniele Giunchi, Matteo Giacomoni
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

#include "mafViewCTNew.h"
#include "mafViewSlice.h"
#include "mafPipeVolumeSlice.h"
#include "mafPipeSurfaceSlice.h"
#include "mmgGui.h"
#include "mmgLutSlider.h"
#include "mafVMEVolume.h"
#include "mafVMESurface.h"
#include "mafViewSlice.h"
#include "vtkProbeFilter.h"
#include "vtkPlaneSource.h"
#include "vtkMAFSmartPointer.h"
#include "vtkImageData.h"
#include "vtkPolyDataMapper.h"
#include "mmaVolumeMaterial.h"
#include "vtkPolyData.h"
#include "vtkLookupTable.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkTextMapper.h"
#include "vtkTextProperty.h"
#include "vtkActor2D.h"

#define VALUE 1.5

//----------------------------------------------------------------------------
// constants:
//----------------------------------------------------------------------------

const int CT_CHILD_VIEWS_NUMBER  = 10;

enum CT_SUBVIEW_ID
{
	CT1 = 0,
	CT2,
	CT3,
	CT4,
	CT5,
	CT6,
	CT7,
	CT8,
	CT9,
	CT10
};


//----------------------------------------------------------------------------
mafCxxTypeMacro(mafViewCTNew);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafViewCTNew::mafViewCTNew(wxString label)
: mafViewCompound(label, 1, 1)
//----------------------------------------------------------------------------
{

	/*m_BorderColor[0][0] = 1.0; m_BorderColor[0][1] = 0.0; m_BorderColor[0][2] = 0.0;
	m_BorderColor[1][0] = 0.0; m_BorderColor[1][1] = 1.0; m_BorderColor[1][2] = 0.0;
	m_BorderColor[2][0] = 0.0; m_BorderColor[2][1] = 0.0; m_BorderColor[2][2] = 1.0;
	m_BorderColor[3][0] = 1.0; m_BorderColor[3][1] = 1.0; m_BorderColor[3][2] = 0.0;
	m_BorderColor[4][0] = 0.0; m_BorderColor[4][1] = 1.0; m_BorderColor[4][2] = 1.0;
	m_BorderColor[5][0] = 1.0; m_BorderColor[5][1] = 0.0; m_BorderColor[5][2] = 1.0;
	m_BorderColor[6][0] = 0.5; m_BorderColor[6][1] = 0.0; m_BorderColor[6][2] = 0.0;
	m_BorderColor[7][0] = 0.0; m_BorderColor[7][1] = 0.5; m_BorderColor[7][2] = 0.0;
	m_BorderColor[8][0] = 0.0; m_BorderColor[8][1] = 0.0; m_BorderColor[8][2] = 0.5;
	m_BorderColor[9][0] = 0.5; m_BorderColor[9][1] = 0.5; m_BorderColor[9][2] = 0.0;*/

	m_CurrentVolume = NULL;
	//m_LayoutConfiguration = LAYOUT_CUSTOM;

	m_ViewCTCompound    = NULL;

	m_WidthSection = 20;
	m_HeightSection = 10;
	m_Position[0] = m_Position[1] = m_Position[2] = 0.0;
	m_Normal[0] = 0.0;
	m_Normal[0] = 0.0;
	m_Normal[0] = 1.0;

	m_PlaneSec = NULL;
	m_Mapper = NULL;
	m_Actor = NULL;
}
//----------------------------------------------------------------------------
mafViewCTNew::~mafViewCTNew()
//----------------------------------------------------------------------------
{
	//m_ViewsRX[RX_FRONT_VIEW] = m_ViewsRX[RX_SIDE_VIEW] = NULL;
	vtkDEL(m_Actor);
	vtkDEL(m_Mapper);
	vtkDEL(m_PlaneSec);

	m_ViewCTCompound = NULL;
}
//----------------------------------------------------------------------------
mafView *mafViewCTNew::Copy(mafObserver *Listener)
//----------------------------------------------------------------------------
{
	mafViewCTNew *v = new mafViewCTNew(m_Label);
	v->m_Listener = Listener;
	v->m_Id = m_Id;
	for (int i=0;i<m_PluggedChildViewList.size();i++)
	{
		v->m_PluggedChildViewList.push_back(m_PluggedChildViewList[i]->Copy(this));
	}
	v->m_NumOfPluggedChildren = m_NumOfPluggedChildren;
	v->Create();
	return v;
}
//----------------------------------------------------------------------------
void mafViewCTNew::VmeShow(mafNode *node, bool show)
//----------------------------------------------------------------------------
{
	/*for(int i=0; i<this->GetNumberOfSubView(); i++)
	m_ChildViewList[i]->VmeShow(node, show);*/

	if(node->IsA("mafVMEVolumeGray"))
	{
		if(show)
		{
			m_CurrentVolume = mafVMEVolume::SafeDownCast(node);
			ProbeVolume();
		}
		else
		{
			m_CurrentVolume = NULL;
		}
	}

	mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafViewCTNew::VmeRemove(mafNode *node)
//----------------------------------------------------------------------------
{
	for(int i=0; i<this->GetNumberOfSubView(); i++)
		m_ChildViewList[i]->VmeRemove(node);

	mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafViewCTNew::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch(maf_event->GetId())
		{
		case ID_LAYOUT_HEIGHT:
		case ID_LAYOUT_WIDTH:
		case ID_LAYOUT_POSITION:
		case ID_LAYOUT_NORMAL:
		case ID_LAYOUT_UPDATE:
			ProbeVolume();
			break;
		default:
			mafEventMacro(*maf_event);
		}
	}
	else
		mafEventMacro(*maf_event);
}
//-------------------------------------------------------------------------
mmgGui* mafViewCTNew::CreateGui()
//-------------------------------------------------------------------------
{
	assert(m_Gui == NULL);
	m_Gui = new mmgGui(this);

	m_Gui->Integer(ID_LAYOUT_WIDTH,"Width",&m_WidthSection);
	m_Gui->Integer(ID_LAYOUT_HEIGHT,"Height",&m_HeightSection);

	m_Gui->VectorN(ID_LAYOUT_POSITION,"Position",&m_Position[0]);
	m_Gui->VectorN(ID_LAYOUT_NORMAL,"Normal",&m_Normal[0]);

	m_Gui->Button(ID_LAYOUT_UPDATE,"Update");

	return m_Gui;
}
//----------------------------------------------------------------------------
void mafViewCTNew::CreateGuiView()
//----------------------------------------------------------------------------
{
	/*m_GuiView = new mmgGui(this);
	wxBoxSizer *lutsSizer = new wxBoxSizer(wxHORIZONTAL);

	// create three windowing widgets
	//for (int i = RX_FRONT_VIEW; i < VIEWS_NUMBER; i++)
	for (int i = 0; i < 1; i++)
	{
	m_LutSliders[i] = new mmgLutSlider(m_GuiView,-1,wxPoint(0,0),wxSize(10,24));
	m_LutSliders[i]->SetListener(this);
	m_LutSliders[i]->SetSize(10,24);
	m_LutSliders[i]->SetMinSize(wxSize(10,24));
	lutsSizer->Add(m_LutSliders[i],wxALIGN_CENTER|wxRIGHT);
	}
	m_GuiView->Add(lutsSizer);
	m_GuiView->FitGui();
	m_GuiView->Update();
	m_GuiView->Reparent(m_Win);*/
}

//----------------------------------------------------------------------------
void mafViewCTNew::PackageView()
//----------------------------------------------------------------------------
{
	m_ViewCTCompound = new mafViewCompound("CT view",1,1);
	mafViewSlice *vs = new mafViewSlice("Slice view");
	/*m_Vs->PlugVisualPipe("mafVMEVolumeGray", "mafPipeVolumeSlice",MUTEX);
	m_Vs->PlugVisualPipe("mafVMESurface", "mafPipeSurfaceSlice",MUTEX);
	m_Vs->PlugVisualPipe("mafVMESurfaceParametric", "mafPipeSurfaceSlice",MUTEX);
	m_Vs->PlugVisualPipe("mafVMELandmark", "mafPipeSurfaceSlice",MUTEX);
	m_Vs->PlugVisualPipe("mafVMELandmarkCloud", "mafPipeSurfaceSlice",MUTEX);*/
	m_ViewCTCompound->PlugChildView(vs);
	PlugChildView(m_ViewCTCompound);
}
//----------------------------------------------------------------------------
void mafViewCTNew::EnableWidgets(bool enable)
//----------------------------------------------------------------------------
{
}
/*//----------------------------------------------------------------------------
void mafViewCTNew::LayoutSubViewCustom(int width, int height)
//----------------------------------------------------------------------------
{
// this implement the Fixed SubViews Layout
int border = 2;
int x_pos, c, i;

int step_width  = (width-border); // /3
i = 0;
for (c = 0; c < m_NumOfChildView; c++)
{
x_pos = c*(step_width + border);
m_ChildViewList[i]->GetWindow()->SetSize(x_pos, 0, step_width, height);
i++;
}
((mafViewCompound*)m_ChildViewList[i-1])->OnLayout();
}*/
//----------------------------------------------------------------------------
void mafViewCTNew::VmeSelect(mafNode *node, bool select)
//----------------------------------------------------------------------------
{
	for(int i=0; i<m_NumOfChildView; i++)
		m_ChildViewList[i]->VmeSelect(node, select);
}
//----------------------------------------------------------------------------
void mafViewCTNew::ProbeVolume()
//----------------------------------------------------------------------------
{
	double b[6];
	m_CurrentVolume->GetOutput()->Update();
	m_CurrentVolume->GetOutput()->GetBounds(b);
	vtkImageData *vtk_data = ((vtkImageData *)m_CurrentVolume->GetOutput()->GetVTKData());
	vtk_data->GetSpacing(m_Spacing);

	mmaVolumeMaterial *vol_material = (m_CurrentVolume)->GetMaterial();
	vtkLookupTable *lut = vol_material->m_ColorLut;


	//+++ create plane
	vtkNEW(m_PlaneSec);
	m_PlaneSec->SetOrigin((b[1] - b[0])/2,(b[3] - b[2])/2,(b[5] - b[4])/2);
	m_PlaneSec->SetPoint1((b[1] - b[0])/2 + m_WidthSection/2.0,(b[3] - b[2])/2,(b[5] - b[4])/2);
	m_PlaneSec->SetPoint2((b[1] - b[0])/2,(b[3] - b[2])/2+ m_HeightSection/2.0,(b[5] - b[4])/2);
	m_PlaneSec->SetXResolution((int)(m_WidthSection/(VALUE*m_Spacing[0])));
	m_PlaneSec->SetYResolution((int)(m_HeightSection/(VALUE*m_Spacing[1])));
	m_PlaneSec->Update();

	//+++using prober

	//double *b = new double[6];
	////////////////////////////////////////////////
	// No Thickness Code
	////////////////////////////////////////////////
	//if(m_AdditionalProfileNumber == 0)
	//{
	vtkMAFSmartPointer<vtkProbeFilter> prober;
	//prober->SetInput(m_PlaneSection[s]);
	prober->SetInput(m_PlaneSec->GetOutput());
	prober->SetSource(vtk_data);
	prober->Update();
	//vtkMAFSmartPointer<vtkPolyDataMapper> m_Mapper;
	vtkNEW(m_Mapper);

	m_Mapper->SetInput(prober->GetPolyDataOutput());
	//m_Mapper->SetInput(sphere->GetOutput());

	double sr[2];
	prober->GetOutput()->GetScalarRange(sr);
	lut->SetTableRange(sr);

	m_Mapper->SetScalarRange(sr);
	m_Mapper->SetLookupTable(lut);

	prober->GetPolyDataOutput()->GetBounds(b);

	double *centerSlice = new double[3];
	prober->GetPolyDataOutput()->GetCenter(centerSlice);
	//m_SliceOriginVector.push_back(centerSlice);
	//m_SliceBoundsVector.push_back(b);
	//}


	//+++creare attore
	//++++settare nelle slice
	vtkNEW(m_Actor);
	m_Actor->SetMapper(m_Mapper);
	//m_Actor->GetProperty()->SetOpacity(0.7);

	//m_SectionActor.push_back(m_Actor);

	vtkMAFSmartPointer<vtkTextMapper> text;
	mafString t;
	t = "test";
	text->SetInput(t);
	text->GetTextProperty()->SetColor(1.0,1.0,1.0);
	//text->GetTextProperty()->BoldOn();

	//text->GetTextProperty()->SetJustificationToRight();
	//text->GetTextProperty()->SetVerticalJustificationToBottom();
	vtkMAFSmartPointer<vtkActor2D> textActor;
	textActor->SetMapper(text);


	/*if (s == m_StartIndexSliceVisualized)
	{
	for(int i = 0 ; i < DEFAULT_NUM_SLICES; i++)
	{
	mafViewSliceDP *vslice = (mafViewSliceDP *)((mafViewCompoundDP *)m_ChildViewList[1])->GetSubView(i);
	vslice->BorderDelete();
	}
	}*/

	//mafViewSliceDP *vslice = (mafViewSliceDP *)((mafViewCompoundDP *)m_ChildViewList[1])->GetSubView(s-m_StartIndexSliceVisualized);
	mafViewSlice *vslice = (mafViewSlice *)m_ChildViewList[0];


	//m_SectionLabel.push_back(textActor);


	//vslice->GetRWI()->GetCamera()->SetFocalPoint(p2);
	//vslice->GetRWI()->GetCamera()->SetPosition(p1);
	vslice->GetRWI()->GetCamera()->SetViewUp(0,0,1);
	vslice->GetRWI()->GetCamera()->SetClippingRange(0.1,1000);
	//vslice->GetRWI()->GetCamera()->ParallelProjectionOn();

	/*int h = m_Size.GetHeight();
	int w = m_Size.GetWidth();
	textActor->SetPosition(w/(m_NumOfSection+1) ,DEFAULT_POS_ACTOR_X*h);*/



	vslice->GetSceneGraph()->m_RenFront->AddActor(m_Actor);
	vslice->GetSceneGraph()->m_RenFront->AddActor2D(textActor);
	/*if (s == m_StartIndexSliceVisualized)
	{
	vslice->BorderCreate(m_StartMarkerColor);
	}
	else if(s == m_EndIndexSliceVisualized)
	{
	vslice->BorderCreate(m_EndMarkerColor);
	}
	else
	{
	vslice->BorderDelete();
	}
	*/

	vslice->GetSceneGraph()->m_RenFront->ResetCamera(b);
	vslice->CameraUpdate();

	/*vslice->GetRWI()->GetCamera()->GetViewPlaneNormal(m_SliceNormal);
	double divergence;
	divergence = (vtkImageData::SafeDownCast(vtk_data))->GetSpacing()[0]/10;
	m_SliceOriginVector[s-m_StartIndexSliceVisualized][0] = m_SliceOriginVector[s-m_StartIndexSliceVisualized][0] + m_SliceNormal[0] * divergence;
	m_SliceOriginVector[s-m_StartIndexSliceVisualized][1] = m_SliceOriginVector[s-m_StartIndexSliceVisualized][1] + m_SliceNormal[1] * divergence;
	m_SliceOriginVector[s-m_StartIndexSliceVisualized][2] = m_SliceOriginVector[s-m_StartIndexSliceVisualized][2] + m_SliceNormal[2] * divergence;*/
	//mafLogMessage(wxString::Format(L"%f %f %f" , m_SliceNormal[0],m_SliceNormal[1],m_SliceNormal[2] ));

}