/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafViewCTNew.cpp,v $
Language:  C++
Date:      $Date: 2007-09-25 09:49:43 $
Version:   $Revision: 1.33 $
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
#include "mafGizmoPath.h"
#include "mmaVolumeMaterial.h"
#include "mafTransform.h"
#include "mafTagArray.h"
#include "mafViewSingleSlice.h"

#include "vtkProbeFilter.h"
#include "vtkPlaneSource.h"
#include "vtkMAFSmartPointer.h"
#include "vtkImageData.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyData.h"
#include "vtkLookupTable.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkTextMapper.h"
#include "vtkTextProperty.h"
#include "vtkActor2D.h"
#include "vtkMath.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkLandmarkTransform.h"
#include "vtkPoints.h"
#include "vtkPointData.h"
#include "mafPipePolylineSlice.h"

//----------------------------------------------------------------------------
// constants:
//----------------------------------------------------------------------------

#define CT_COMPOUND 0
#define VALUE 1.5

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafViewCTNew);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafViewCTNew::mafViewCTNew(wxString label)
: mafViewCompound(label, 1, 1)
//----------------------------------------------------------------------------
{
	m_CurrentVolume = NULL;

	m_ViewCTCompound    = NULL;

	m_WidthSection = 30;
	m_HeightSection = 50;

  m_Thickness = 0;
	m_AdditionalProfileNumber = 0;
	m_ProfileDistance = 0;

	for(int i=0; i<CT_CHILD_VIEWS_NUMBER; i++)
	{
		m_Normal.push_back(new double[3]);
		m_Position.push_back(new double[3]);
		m_TextActor.push_back(vtkActor2D::New());
		m_Actor.push_back(vtkActor::New());
		m_Text.push_back(vtkTextMapper::New());
		m_Text[i]->GetTextProperty()->SetColor(1.0,1.0,1.0);
		m_Mapper.push_back(vtkPolyDataMapper::New());
		m_Prober.push_back(vtkProbeFilter::New());
		m_PlaneSec.push_back(vtkPlaneSource::New());
	}

	m_CurrentZ = 0.0;

}
//----------------------------------------------------------------------------
mafViewCTNew::~mafViewCTNew()
//----------------------------------------------------------------------------
{
		for(int i=0; i<m_Actor.size(); i++)
		{
			cppDEL(m_Normal[i]);
			cppDEL(m_Position[i]);
			vtkDEL(m_TextActor[i]);
			vtkDEL(m_Actor[i]);
			vtkDEL(m_Text[i]);
			vtkDEL(m_Mapper[i]);
			vtkDEL(m_Prober[i]);
			vtkDEL(m_PlaneSec[i]);
		}
		m_Normal.clear();
		m_Position.clear();
		m_TextActor.clear();
		m_Actor.clear();
		m_Text.clear();
		m_Mapper.clear();
		m_Prober.clear();
		m_PlaneSec.clear();

		m_ViewCTCompound = NULL;

		m_CurrentVolume = NULL;
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
			double b[6];
			m_CurrentVolume->GetOutput()->Update();
			m_CurrentVolume->GetOutput()->GetBounds(b);
			double wholeScalarRangeVol[2];
			m_CurrentVolume->GetOutput()->GetVTKData()->GetScalarRange(wholeScalarRangeVol);
			m_MinLUTHistogram = wholeScalarRangeVol[0];
			m_MaxLUTHistogram = wholeScalarRangeVol[1];
			SetCurrentZ((b[5]-b[4])/2);
			for(int idSubView=0; idSubView<CT_CHILD_VIEWS_NUMBER; idSubView++)
			{
				if(m_CurrentVolume)
				{
					mafViewVTK* view =((mafViewVTK *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND])->GetSubView(idSubView));
					view->VmeAdd(node);
					view->VmeShow(node,true);
				}
			}
			ProbeVolume();
		}
		else
		{
			for(int idSubView=0; idSubView<CT_CHILD_VIEWS_NUMBER; idSubView++)
			{
				if(m_CurrentVolume)
				{
					mafViewVTK* view =((mafViewVTK *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND])->GetSubView(idSubView));
					view->GetSceneGraph()->m_RenFront->RemoveActor(m_Actor[idSubView]);
					view->GetSceneGraph()->m_RenFront->RemoveActor(m_TextActor[idSubView]);
				}
			}
			CameraUpdate();
			m_CurrentVolume = NULL;
		}

		m_Gui->Enable(ID_LAYOUT_WIDTH,m_CurrentVolume!=NULL);
		m_Gui->Enable(ID_LAYOUT_HEIGHT,m_CurrentVolume!=NULL);
		m_Gui->Enable(ID_LAYOUT_THICKNESS,m_CurrentVolume!=NULL);
		m_Gui->Update();
	}
	else if(node->IsA("mafVMEPolyline"))
	{
		if(node->GetTagArray()->IsTagPresent("MFL_MANDIBULAR_CANAL"))
		{
			m_ChildViewList[CT_COMPOUND]->VmeShow(node, show);
      if(show)
      {
			  for(int i=0; i< CT_CHILD_VIEWS_NUMBER; i++)
			  {
				  UpdateSliceView(i);
			  }
      
        for(int i=0; i< CT_CHILD_VIEWS_NUMBER; i++)
        {
          mafPipePolylineSlice *pipeSlice = (mafPipePolylineSlice*)((mafViewCompound *)m_ChildViewList[CT_COMPOUND])->GetSubView(i)->GetNodePipe(node);
          if(pipeSlice) 
          {
            pipeSlice->SplineModeOn();
            pipeSlice->FillOn();
            pipeSlice->SetAlphaFilling(2.0);
          }
        }

			  CameraUpdate();
      }
      else
      {
        for(int i=0; i< CT_CHILD_VIEWS_NUMBER; i++)
        {
          ((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND])->GetSubView(i))->UpdateSurfacesList(node);
        }
      }
		}
	}
}
//----------------------------------------------------------------------------
void mafViewCTNew::VmeRemove(mafNode *node)
//----------------------------------------------------------------------------
{
	if(node == m_CurrentVolume)
	{
		m_CurrentVolume = NULL;

		for(int i=0; i<m_Actor.size(); i++)
		{
			mafViewVTK* view =((mafViewVTK *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND])->GetSubView(i));

			if(m_Actor[i])
				view->GetSceneGraph()->m_RenFront->RemoveActor(m_Actor[i]);
			if(m_TextActor[i])
				view->GetSceneGraph()->m_RenFront->RemoveActor(m_TextActor[i]);
		}

		m_Gui->Enable(ID_LAYOUT_WIDTH,m_CurrentVolume!=NULL);
		m_Gui->Enable(ID_LAYOUT_HEIGHT,m_CurrentVolume!=NULL);
		m_Gui->Enable(ID_LAYOUT_THICKNESS,m_CurrentVolume!=NULL);
		m_Gui->Update();
	}

	for(int i=0; i<this->GetNumberOfSubView(); i++)
		m_ChildViewList[i]->VmeRemove(node);

	CameraUpdate();
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
			ProbeVolume();
      //send here
      e->SetDouble(m_WidthSection);
      e->SetSender(this);
      mafEventMacro(*e);
    break;
		case ID_LAYOUT_THICKNESS:
			{
				ProbeVolume();
			}
			break;
		/*case ID_LAYOUT_UPDATE:
			ProbeVolume();
			mafEventMacro(mafEvent(this,CAMERA_UPDATE));
			break;*/
			case MOUSE_UP:
			case MOUSE_MOVE:
			{
				if(e->GetString()->Equals("Z"))
				{
					vtkPoints *p = (vtkPoints *)e->GetVtkObj();
					double pos[3];
					p->GetPoint(0,pos);
					SetCurrentZ(pos[2]);
					ProbeVolume();
				}
			}
			break;
      case mafViewSingleSlice::ID_POSITION:
        {
          SetCurrentZ(e->GetDouble());
          ProbeVolume();
        }
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
  
  // end creation
  
  m_Gui = new mmgGui(this);

	m_Gui->Integer(ID_LAYOUT_WIDTH,_("Width"),&m_WidthSection);
	m_Gui->Integer(ID_LAYOUT_HEIGHT,_("Height"),&m_HeightSection);

	m_Gui->Double(ID_LAYOUT_THICKNESS,_("Thickness"), &m_Thickness,0,50,2,_("define the thickness of the slice"));

	//m_Gui->Button(ID_LAYOUT_UPDATE,"Update");

	m_Gui->Enable(ID_LAYOUT_WIDTH,m_CurrentVolume!=NULL);
	m_Gui->Enable(ID_LAYOUT_HEIGHT,m_CurrentVolume!=NULL);
	m_Gui->Enable(ID_LAYOUT_THICKNESS,m_CurrentVolume!=NULL);

	m_Gui->Label("");

	m_Gui->Update();
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
	m_ViewCTCompound = new mafViewCompound("CT view",2,5);
	mafViewSlice *vs = new mafViewSlice("Slice view",CAMERA_ARB,false,false,true);
	vs->PlugVisualPipe("mafVMEPolyline","mafPipePolylineSlice");
  vs->PlugVisualPipe("mafVMEVolumeGray","dpVisualPipeNull");
  
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
void mafViewCTNew::SetCurrentZ(double Z)
//----------------------------------------------------------------------------
{
	m_CurrentZ = Z;
	for(int i=0;i<m_Position.size();i++)
		m_Position[i][2] = m_CurrentZ;
}
//----------------------------------------------------------------------------
void mafViewCTNew::SetCurrentZ(int idview,double Z)
//----------------------------------------------------------------------------
{
	m_CurrentZ = Z;
	m_Position[idview][2] = m_CurrentZ;
}
//----------------------------------------------------------------------------
void mafViewCTNew::SetMatrix(int idview,mafMatrix *matrix)
//----------------------------------------------------------------------------
{
	matrix->GetVersor(0, m_Normal[idview]);
	mafTransform::GetPosition(*matrix, m_Position[idview]);

	SetCurrentZ(idview,m_CurrentZ);

	UpdateSliceView(idview);
}
//----------------------------------------------------------------------------
void mafViewCTNew::UpdateSliceView(int idview)
//----------------------------------------------------------------------------
{
	mafViewSlice* view =((mafViewSlice *)((mafViewCompound *)m_ChildViewList[0])->GetSubView(idview));

	double position_shifted[3];
	position_shifted[0]=m_Position[idview][0]+m_Normal[idview][0]*0.1;
	position_shifted[1]=m_Position[idview][1]+m_Normal[idview][1]*0.1;
	position_shifted[2]=m_Position[idview][2]+m_Normal[idview][2]*0.1;

	view->SetNormal(m_Normal[idview]);
	view->SetSliceLocalOrigin(position_shifted);
}
//----------------------------------------------------------------------------
void mafViewCTNew::SetColorText(int idView,double *color)
//----------------------------------------------------------------------------
{
	m_Text[idView]->GetTextProperty()->SetColor(color);
}
//----------------------------------------------------------------------------
void mafViewCTNew::SetColorText(double **color)
//----------------------------------------------------------------------------
{
	for(int idView=0;idView<CT_CHILD_VIEWS_NUMBER;idView++)
		m_Text[idView]->GetTextProperty()->SetColor(color[idView]);
}
//----------------------------------------------------------------------------
void mafViewCTNew::ProbeVolume()
//----------------------------------------------------------------------------
{
	if (m_CurrentVolume == NULL) return;


  if(m_Thickness == 0)
  {
    m_AdditionalProfileNumber = 0;
    m_ProfileDistance = 0;
  }
  else
  {
    double spacing;
    spacing = (m_Spacing[0] <= m_Spacing[1] ? m_Spacing[0] : m_Spacing[1]);
    m_ProfileDistance = 2.0 * spacing;
    m_AdditionalProfileNumber = m_Thickness / m_ProfileDistance + 1;
    m_AdditionalProfileNumber  /= 2;
    //mafLogMessage(m_AdditionalProfileNumber);
    if (m_AdditionalProfileNumber * 2 > 4)
      wxMessageBox(L"Warning: with this Thickness value, it will be required more time to process");
  }


  double b[6];
  m_CurrentVolume->GetOutput()->Update();
  m_CurrentVolume->GetOutput()->GetBounds(b);
  vtkImageData *vtk_data = ((vtkImageData *)m_CurrentVolume->GetOutput()->GetVTKData());
  vtk_data->GetSpacing(m_Spacing);

  for(int idSubView=0; idSubView<CT_CHILD_VIEWS_NUMBER; idSubView++)
  {
    mafViewSlice *vslice = ((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND])->GetSubView(idSubView));
	  if (m_Actor.size()>0)
	  {
		  vslice->GetSceneGraph()->m_RenFront->RemoveActor(m_Actor[idSubView]);
	  }


    mmaVolumeMaterial *vol_material = (m_CurrentVolume)->GetMaterial();
    mmaVolumeMaterial *copy;
    mafNEW(copy);
    copy->DeepCopy(vol_material);
    vtkLookupTable *lut = copy->m_ColorLut; 

	  vtkMAFSmartPointer<vtkPoints> source_points;
	  source_points->SetNumberOfPoints(2);
	  source_points->SetPoint(0,0,0,0);
	  source_points->SetPoint(1,1,0,0);

	  vtkMAFSmartPointer<vtkPoints> target_points;
	  target_points->SetNumberOfPoints(2);
	  target_points->SetPoint(0,m_Position[idSubView]);
		
	  vtkMath::Normalize(m_Normal[idSubView]);
	  double p2[3];
	  p2[0]=m_Position[idSubView][0] + m_Normal[idSubView][0];
	  p2[1]=m_Position[idSubView][1] + m_Normal[idSubView][1];
	  p2[2]=m_Position[idSubView][2] + m_Normal[idSubView][2];
	  target_points->SetPoint(1,p2);

	  vtkMAFSmartPointer<vtkLandmarkTransform> trans_matrix;
	  trans_matrix->SetSourceLandmarks(source_points.GetPointer());
	  trans_matrix->SetTargetLandmarks(target_points.GetPointer());
	  trans_matrix->SetModeToRigidBody();
	  trans_matrix->Update();

	  vtkMAFSmartPointer<vtkTransform> trans;
	  trans->SetMatrix(trans_matrix->GetMatrix());

	  vtkMAFSmartPointer<vtkTransformPolyDataFilter> trans_poly;
	  trans_poly->SetTransform(trans);


	  //+++ create plane

	  m_PlaneSec[idSubView]->SetOrigin(0,m_WidthSection/2.0,m_HeightSection/2.0);
	  m_PlaneSec[idSubView]->SetPoint1(0,-m_WidthSection/2.0,m_HeightSection/2.0);
	  m_PlaneSec[idSubView]->SetPoint2(0,m_WidthSection/2.0,-m_HeightSection/2.0);
	  m_PlaneSec[idSubView]->SetXResolution((int)(m_WidthSection/(VALUE*m_Spacing[0])));
	  m_PlaneSec[idSubView]->SetYResolution((int)(m_HeightSection/(VALUE*m_Spacing[1])));
	  //planeSec->SetNormal(1,0,0);
	  m_PlaneSec[idSubView]->Update();

	  trans_poly->SetInput(m_PlaneSec[idSubView]->GetOutput());
	  trans_poly->Update();

	  //+++using prober

	  //double *b = new double[6];
	  ////////////////////////////////////////////////
	  // No Thickness Code
	  ////////////////////////////////////////////////
	  if(m_AdditionalProfileNumber == 0)
	  {
	  
			//prober->SetInput(m_PlaneSection[s]);
			m_Prober[idSubView]->SetInput(trans_poly->GetOutput());
			m_Prober[idSubView]->SetSource(vtk_data);
			m_Prober[idSubView]->Update();
			//vtkMAFSmartPointer<vtkPolyDataMapper> m_Mapper;

			//m_Mapper->SetInput(m_Prober->GetPolyDataOutput());
			//m_Mapper->SetInput(sphere->GetOutput());

			double sr[2];
			m_Prober[idSubView]->GetOutput()->GetScalarRange(sr);
			lut->SetTableRange(sr);

			m_Prober[idSubView]->GetPolyDataOutput()->GetBounds(b);

			double *centerSlice = new double[3];
			m_Prober[idSubView]->GetPolyDataOutput()->GetCenter(centerSlice);
			//m_SliceOriginVector.push_back(centerSlice);
			//m_SliceBoundsVector.push_back(b);
			//}

			vtkMAFSmartPointer<vtkTransform> transform;
			transform->SetMatrix(m_CurrentVolume->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
			transform->Update();

			vtkMAFSmartPointer<vtkTransformPolyDataFilter> tpdf;
			tpdf->SetInput(m_Prober[idSubView]->GetPolyDataOutput());
			tpdf->SetTransform(transform);
			tpdf->Update();

		  
			m_Mapper[idSubView]->SetInput(tpdf->GetOutput());

			m_Mapper[idSubView]->SetScalarRange(sr);
			m_Mapper[idSubView]->SetLookupTable(lut);

		}
		////////////////////////////////////////////////
		// Thickness Code
		////////////////////////////////////////////////
		else
		{
			std::vector<vtkProbeFilter *> probeVector;

			for(int u= -m_AdditionalProfileNumber; u <= m_AdditionalProfileNumber ; u++)
			{
				vtkMAFSmartPointer<vtkPolyData> plane;
				plane->DeepCopy(trans_poly->GetOutput());

				probeVector.push_back(vtkProbeFilter::New());

				// traslo il piano (in realta' tenere conto della normale)
				//calculate direction p1-p2

				double center[3];
				plane->GetCenter(center);

				double distanceCenterP2;
				distanceCenterP2 = sqrt(vtkMath::Distance2BetweenPoints(center, p2));

				vtkMAFSmartPointer<vtkTransform> tr;
				tr->Translate(((u * m_ProfileDistance)/distanceCenterP2) * (p2[0] - center[0]), ((u * m_ProfileDistance)/distanceCenterP2) * (p2[1] - center[1]) , ((u * m_ProfileDistance)/distanceCenterP2) * (p2[2] - center[2]));
				tr->Update();

				vtkMAFSmartPointer<vtkTransformPolyDataFilter> tpdf;
				tpdf->SetInput(plane);
				tpdf->SetTransform(tr);
				tpdf->Update(); 

				probeVector[probeVector.size()-1]->SetSource(vtk_data);
				probeVector[probeVector.size()-1]->SetInput(tpdf->GetOutput());
				probeVector[probeVector.size()-1]->Update();
			}

			int number = probeVector[m_AdditionalProfileNumber]->GetPolyDataOutput()->GetNumberOfPoints();
			std::vector<double> sumVector;
			for(int j= 0 ; j< number; j++)
			{
				sumVector.push_back(0.0);  
				for(int i=0; i< probeVector.size(); i++)
				{
					sumVector[j] += probeVector[i]->GetPolyDataOutput()->GetPointData()->GetScalars()->GetTuple1(j);
				}
				sumVector[j] = sumVector[j]/(m_AdditionalProfileNumber*2+1);
			}


			for(int k=0; k< number; k++)
				probeVector[m_AdditionalProfileNumber]->GetPolyDataOutput()->GetPointData()->GetScalars()->SetTuple1(k , sumVector[k]);

			double range[2];
			probeVector[m_AdditionalProfileNumber]->GetPolyDataOutput()->GetScalarRange(range);

			lut->SetRange(range);

			vtkMAFSmartPointer<vtkTransform> transform;
			transform->SetMatrix(m_CurrentVolume->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
			transform->Update();

			vtkMAFSmartPointer<vtkTransformPolyDataFilter> tpdf;
			tpdf->SetInput(probeVector[m_AdditionalProfileNumber]->GetPolyDataOutput());
			tpdf->SetTransform(transform);
			tpdf->Update();

			m_Mapper[idSubView]->SetInput(probeVector[m_AdditionalProfileNumber]->GetPolyDataOutput());
			m_Mapper[idSubView]->SetScalarRange(range);
			m_Mapper[idSubView]->SetLookupTable(lut);  	

			probeVector[m_AdditionalProfileNumber]->GetPolyDataOutput()->GetBounds(b);
		}
	  
	  m_Actor[idSubView]->SetMapper(m_Mapper[idSubView]);
	  
	  mafString t;
		t = wxString::Format("%d",idSubView);
	  //m_Text[idSubView]->SetInput(t);
	  
	  m_TextActor[idSubView]->SetMapper(m_Text[idSubView]);


	  vslice->GetRWI()->GetCamera()->SetFocalPoint(m_Position[idSubView]);
	  vslice->GetRWI()->GetCamera()->SetPosition(p2);
	  vslice->GetRWI()->GetCamera()->SetViewUp(0,0,1);
	  vslice->GetRWI()->GetCamera()->SetClippingRange(0.1,1000);
	  vslice->GetRWI()->GetCamera()->ParallelProjectionOn();


	  vslice->GetSceneGraph()->m_RenFront->AddActor(m_Actor[idSubView]);
	  vslice->GetSceneGraph()->m_RenFront->AddActor2D(m_TextActor[idSubView]);

    //vslice->GetRWI()->GetImage()
    double newb[6];
		double middleX = (b[1]+b[0])/2;
		double middleY = (b[3]+b[2])/2;
		double middleZ = (b[5]+b[4])/2;

		double distX = fabs(b[1]-b[0]);
		double distY = fabs(b[3]-b[2]);
		double distZ = fabs(b[5]-b[4]);
		

		double distXY = sqrt(distX*distX + distY*distY);

		double max = distZ > distXY ? distZ : distXY;
		if(distZ / distXY > 1.5)
		  max /= 3;
		else
			max /= 2;

		newb[0] = middleX - max;
		newb[1] = middleX + max;
		newb[2] = middleY - max;
		newb[3] = middleY + max;
		newb[4] = middleZ - max;
		newb[5] = middleZ + max;

	  vslice->GetSceneGraph()->m_RenFront->ResetCamera(newb);
	  m_Gui->Update();
  }
	SetCTLookupTable(m_MinLUTHistogram,m_MaxLUTHistogram);
}
//----------------------------------------------------------------------------
void mafViewCTNew::CameraUpdate()
//----------------------------------------------------------------------------
{
	//for(int i=0; i<m_NumOfChildView; i++)
	//	m_ChildViewList[i]->CameraUpdate();
	Superclass::CameraUpdate();
}
//----------------------------------------------------------------------------
void mafViewCTNew::CameraReset(mafNode *node)
//----------------------------------------------------------------------------
{
	if(m_CurrentVolume)
	{
		ProbeVolume();
		CameraUpdate();
	}
	else
	{
		Superclass::CameraReset();
	}
}
//----------------------------------------------------------------------------
void mafViewCTNew::SetCTLookupTable(double range[2])
//----------------------------------------------------------------------------
{
  SetCTLookupTable(range[0] , range[1]);
}
//----------------------------------------------------------------------------
void mafViewCTNew::SetCTLookupTable(double min, double max)
//----------------------------------------------------------------------------
{
  if(m_CurrentVolume)
  {
		m_MinLUTHistogram = min;
		m_MaxLUTHistogram = max;
    double wholeScalarRangeVol[2];
    m_CurrentVolume->GetOutput()->GetVTKData()->GetScalarRange(wholeScalarRangeVol);

    for(int idSubView=0; idSubView<CT_CHILD_VIEWS_NUMBER; idSubView++)
    {
      mafViewSlice *vslice = ((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND])->GetSubView(idSubView));

      //calculate the variation
      //double wholeScalarRangeData[2];
      //m_Prober[idSubView]->GetOutput()->GetScalarRange(wholeScalarRangeData);

      double mapLow,mapHigh;
      mapLow = wholeScalarRangeVol[0] + /*((wholeScalarRangeData[1] - wholeScalarRangeData[0])/(wholeScalarRangeVol[1] - wholeScalarRangeVol[0])) * */ (min - wholeScalarRangeVol[0]);
      mapHigh = wholeScalarRangeVol[1] + /*((wholeScalarRangeData[1] - wholeScalarRangeData[0])/(wholeScalarRangeVol[1] - wholeScalarRangeVol[0])) * */(max - wholeScalarRangeVol[1]);

      m_Mapper[idSubView]->SetScalarRange(mapLow, mapHigh);
    }
		CameraUpdate();
  }
}
//----------------------------------------------------------------------------
void mafViewCTNew::SetTextValue(int index, double value)
//----------------------------------------------------------------------------
{
  mafString t;
  t = mafString(wxString::Format("%.1f", value < 0.0 ? 0.0 : value));
  m_Text[index]->SetInput(t);
}