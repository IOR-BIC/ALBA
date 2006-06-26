/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medPipeDensityDistance.cpp,v $
  Language:  C++
  Date:      $Date: 2006-06-26 16:39:54 $
  Version:   $Revision: 1.2 $
  Authors:   Matteo Giacomoni
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

#include "medPipeDensityDistance.h"
#include "mafSceneNode.h"
#include "mafVMESurface.h"
#include "mmaMaterial.h"
#include "mmgGui.h"
#include "mmgMaterialButton.h"
#include "mafAxes.h"
#include "mafDataVector.h"
#include "mafVMEGenericAbstract.h"
#include "mafNode.h"
#include "mafVME.h"
#include "mmgValidator.h"
#include "vtkMAFSmartPointer.h"

#include "vtkMAFSmartPointer.h"
#include "vtkMAFAssembly.h"
#include "vtkRenderer.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyData.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkTexture.h"
#include "vtkPointData.h"
#include "vtkImageData.h"
#include "vtkTextureMapToCylinder.h"
#include "vtkTextureMapToPlane.h"
#include "vtkTextureMapToSphere.h"
#include "vtkImageMapper.h"

#include "vtkPolyDataNormals.h"
#include "vtkDistanceFilter.h"
#include "vtkColorTransferFunction.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkPolyData.h"
#include "vtkScalarBarActor.h"
#include "vtkActor2D.h"
#include "vtkLookupTable.h"
#include "vtkMassProperties.h"
#include "vtkClipPolyData.h"

#include <vector>

//----------------------------------------------------------------------------
mafCxxTypeMacro(medPipeDensityDistance);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medPipeDensityDistance::medPipeDensityDistance()
:mafPipe()
//----------------------------------------------------------------------------
{
  m_Mapper          = NULL;
  m_Actor           = NULL;
  m_DistanceFilter  = NULL;
  m_Table           = NULL;
  m_Normals         = NULL;
  m_Volume          = NULL;
  m_ScalarBar       = NULL;
  
  m_DensityDistance = 0;
  m_FirstThreshold = 700;
  m_SecondThreshold = 300;
  m_MaxDistance = 2;
	m_BarTipology = 0;
	m_NumSections = 3;
	m_Area[0] = 0;
	m_Area[1] = 0;
	m_Area[2] = 0;

  m_LowColour.Set(255,0,0);
	m_MidColour1.Set(255,255,0);
	m_MidColour2.Set(0,255,255);
  m_MidColour.Set(0,255,0);
	m_HiColour.Set(0,0,255);
	m_WhiteColour.Set(255,255,255);
}
//----------------------------------------------------------------------------
void medPipeDensityDistance::Create(mafSceneNode *n/*, bool use_axes*/)
//----------------------------------------------------------------------------
{
  Superclass::Create(n);

  m_VolumeAccept = new mafVolumeAccept;
  vtkNEW(m_Normals);
	vtkNEW(m_DistanceFilter);
	vtkNEW(m_Mapper);
	vtkNEW(m_Actor);
	vtkNEW(m_Table);
  vtkNEW(m_ScalarBar);

  assert(m_Vme->GetOutput()->IsMAFType(mafVMEOutputSurface));
  mafVMEOutputSurface *surface_output = mafVMEOutputSurface::SafeDownCast(m_Vme->GetOutput());
  assert(surface_output);
  surface_output->Update();
  vtkPolyData *data = vtkPolyData::SafeDownCast(surface_output->GetVTKData());
  data->Update();
  assert(data);

	m_Normals->SetInput(data);
	m_Normals->ComputePointNormalsOn();
	m_Normals->SplittingOff();
	m_Normals->Update();

  if (m_Volume)
  {
	  m_DistanceFilter->SetSource(((mafVME*)m_Volume)->GetOutput()->GetVTKData());
	  m_DistanceFilter->SetInput((vtkDataSet *)m_Normals->GetOutput());
	  m_DistanceFilter->SetMaxDistance(m_MaxDistance);
	  m_DistanceFilter->SetThreshold(m_FirstThreshold);
	  m_DistanceFilter->SetDistanceModeToScalar();
	  m_DistanceFilter->SetInputMatrix(surface_output->GetAbsMatrix()->GetVTKMatrix());
		m_DistanceFilter->Update();

		/*double i;
		m_Table->AddRGBPoint(-m_DistanceFilter->GetMaxDistance(), 1.f, 0.f, 0.0f);
		double step=(double)(m_MaxDistance*2)/(m_NumSections-1);
		double init=(double)(-m_MaxDistance+step);
		for(i=init;i<m_MaxDistance;i+=step)
		{
			if(i<=0)
				m_Table->AddRGBPoint(i, 1.f, (float)(1+(i/m_MaxDistance)), (float)(1+(i/m_MaxDistance)));
			else
				m_Table->AddRGBPoint(i, (float)(1-(i/m_MaxDistance)), (float)(1-(i/m_MaxDistance)), 1.f);
		}
		m_Table->AddRGBPoint(m_DistanceFilter->GetMaxDistance(), 0.1f, 0.1f, 1.f);
		//m_Table->AddRGBPoint(-m_DistanceFilter->GetMaxDistance(), 1.f, 0.f, 0.0f);
    //m_Table->AddRGBPoint(-0.5f *	m_DistanceFilter->GetMaxDistance(), 1.f, 0.1f, 1.f);
	  //m_Table->AddRGBPoint(0.5f *	m_DistanceFilter->GetMaxDistance(), 0.9f, 0.9f, 1.f);
	  //m_Table->AddRGBPoint(					m_DistanceFilter->GetMaxDistance(), 0.1f, 0.1f, 1.f);
	  m_Table->Build();


		//Calculate the areas
		vtkMAFSmartPointer<vtkMassProperties> mass_all;
		mass_all->SetInput(m_DistanceFilter->GetPolyDataOutput());
		mass_all->Update();

		double total_area = mass_all->GetSurfaceArea();

		double area=0;
		step=(double)(m_MaxDistance*2)/(m_NumSections);
		mafString message;

		vtkMAFSmartPointer<vtkClipPolyData> clip;
		clip->SetInput(m_DistanceFilter->GetPolyDataOutput());
		clip->SetValue(m_MaxDistance);
		clip->GenerateClippedOutputOn();
		clip->Update();

		vtkMAFSmartPointer<vtkMassProperties> mass;
		mass->SetInput(clip->GetOutput());
		mass->Update();
		area = (mass->GetSurfaceArea() / total_area) * 100.0;

		message= wxString::Format("From infinity To %d\t%.3lf %" , m_MaxDistance,area);
		mafLogMessage(message);
	
		vtkMAFSmartPointer<vtkClipPolyData> clip_old;
		clip_old=clip;
		for (i=m_MaxDistance-step;i>=-m_MaxDistance;i-=step)
		{
			vtkMAFSmartPointer<vtkClipPolyData> clip;
			clip->SetInput(clip_old->GetClippedOutput());
			clip->SetValue(i);
			clip->GenerateClippedOutputOn();
			clip->Update();

			clip_old=clip;

			vtkMAFSmartPointer<vtkMassProperties> mass;
			mass->SetInput(clip->GetOutput());
			mass->Update();
			area = (mass->GetSurfaceArea() / total_area) * 100.0;

			message= wxString::Format("From %.3lf To %.3lf\t%.3lf %" , i+step,i,area);
			mafLogMessage(message);
		}

		vtkMAFSmartPointer<vtkMassProperties> mass_final;
		mass_final->SetInput(clip_old->GetClippedOutput());
		mass_final->Update();
		area = (mass_final->GetSurfaceArea() / total_area) * 100.0;

		message= wxString::Format("From %d To -infinity\t%.3lf %" , -m_MaxDistance,area);
		mafLogMessage(message);*/

		int i;
		for (i=-4*m_MaxDistance;i<-m_MaxDistance;i++)
			m_Table->AddRGBPoint(i,m_LowColour.Red()/255.0, m_LowColour.Green()/255.0,	m_LowColour.Blue()/255.0);
		for (i=-m_MaxDistance;i<m_MaxDistance;i++)
			m_Table->AddRGBPoint(i,1.0,1.0,1.0);
		for (i=m_MaxDistance;i<=4*m_MaxDistance;i++)
			m_Table->AddRGBPoint(i,m_HiColour.Red()/255.0, m_HiColour.Green()/255.0,	m_HiColour.Blue()/255.0);
	  
		m_Mapper->SetInput((vtkPolyData*)m_DistanceFilter->GetOutput());

		//Calculate the areas
		vtkMAFSmartPointer<vtkMassProperties> mass_all;
		mass_all->SetInput(m_DistanceFilter->GetPolyDataOutput());
		mass_all->Update();

		double total_area = mass_all->GetSurfaceArea();

		vtkMAFSmartPointer<vtkClipPolyData> clipHigh;
		clipHigh->SetInput(m_DistanceFilter->GetPolyDataOutput());
		clipHigh->SetValue(m_MaxDistance);
		clipHigh->GenerateClippedOutputOn();
		clipHigh->Update();

		vtkMAFSmartPointer<vtkClipPolyData> clipMidHight;
		clipMidHight->SetInput(clipHigh->GetClippedOutput());
		clipMidHight->SetValue(0);
		clipMidHight->GenerateClippedOutputOn();
		clipMidHight->Update();

		vtkMAFSmartPointer<vtkClipPolyData> clipMidLow;
		clipMidLow->SetInput(clipMidHight->GetClippedOutput());
		clipMidLow->SetValue(-m_MaxDistance);
		clipMidLow->GenerateClippedOutputOn();
		clipMidLow->Update();

		vtkMAFSmartPointer<vtkMassProperties> mass_high;
		mass_high->SetInput(clipHigh->GetOutput());
		mass_high->Update();

		/*vtkMAFSmartPointer<vtkMassProperties> mass_mid1;
		mass_mid1->SetInput(clipMidHight->GetOutput());
		mass_mid1->Update();*/

		vtkMAFSmartPointer<vtkMassProperties> mass_mid;
		mass_mid->SetInput(clipMidLow->GetOutput());
		mass_mid->Update();

		vtkMAFSmartPointer<vtkMassProperties> mass_low;
		mass_low->SetInput(clipMidLow->GetClippedOutput());
		mass_low->Update();

		//double area[4];
		m_AreaDistance[0] = (mass_low->GetSurfaceArea() / total_area) * 100.0;
		m_AreaDistance[1] = (mass_mid->GetSurfaceArea() / total_area) * 100.0;
		m_AreaDistance[2] = (mass_high->GetSurfaceArea() / total_area) * 100.0;

		/*mafString message;
		message= wxString::Format("From %d To infinity\t%.3lf %" , m_MaxDistance,area[2]);
		mafLogMessage(message);
		message= wxString::Format("From %d To %d\t%.3lf %" , -m_MaxDistance,m_MaxDistance,area[1]);
		mafLogMessage(message);
		message= wxString::Format("From %d To -infinity\t%.3lf %" , m_MaxDistance,area[0]);
		mafLogMessage(message);*/
  }
  else
  {
    m_Mapper->SetInput(m_Normals->GetOutput());
  }
	
	m_Mapper->ScalarVisibilityOn();
	m_Mapper->SetScalarModeToUsePointData();
	m_Mapper->SetLookupTable(m_Table);
	m_Mapper->UseLookupTableScalarRangeOn();
	
	m_Actor->SetMapper(m_Mapper);

  m_ScalarBar->SetLookupTable(m_Mapper->GetLookupTable());
  ((vtkActor2D*)m_ScalarBar)->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
  ((vtkActor2D*)m_ScalarBar)->GetPositionCoordinate()->SetValue(0.1,0.01);
  m_ScalarBar->SetOrientationToHorizontal();
  m_ScalarBar->SetWidth(0.8);
  m_ScalarBar->SetHeight(0.17);
  m_ScalarBar->SetTitle("Distance");
  m_ScalarBar->SetMaximumNumberOfColors(3);
	m_ScalarBar->SetLabelFormat("%-#6.0f");

  m_AssemblyFront->AddPart(m_Actor);
  m_RenFront->AddActor2D(m_ScalarBar);
}
//----------------------------------------------------------------------------
medPipeDensityDistance::~medPipeDensityDistance()
//----------------------------------------------------------------------------
{
  delete m_VolumeAccept;
  m_RenFront->RemoveActor2D(m_ScalarBar);
  m_AssemblyFront->RemovePart(m_Actor);

  vtkDEL(m_Normals);
	vtkDEL(m_Mapper);
  vtkDEL(m_Actor);
  vtkDEL(m_DistanceFilter);
  vtkDEL(m_Table);
  vtkDEL(m_ScalarBar);
  //cppDEL(m_Axes);
}
//----------------------------------------------------------------------------
void medPipeDensityDistance::Select(bool sel)
//----------------------------------------------------------------------------
{
	m_Selected = sel;
	if(m_Actor->GetVisibility()) 
	{
			//m_OutlineActor->SetVisibility(sel);
      //m_Axes->SetVisibility(sel);
	}
}
//----------------------------------------------------------------------------
mmgGui *medPipeDensityDistance::CreateGui()
//----------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = new mmgGui(this);
  
  wxString m_Choices[2];
  m_Choices[0]="Distance";
  m_Choices[1]="Density";
  m_Gui->Radio(ID_DENSITY_DISTANCE,"",&m_DensityDistance,2,m_Choices);

  double range[2];
  range[0]=m_SecondThreshold;
  range[1]=m_FirstThreshold;
  if (m_Volume)
    ((mafVME*)m_Volume)->GetOutput()->GetVTKData()->GetScalarRange(range);

	m_Gui->Divider(1);
  m_Gui->Integer(ID_FIRST_THRESHOLD,"1° Threshold",&m_FirstThreshold,range[0],range[1]);
  m_Gui->Integer(ID_SECOND_THRESHOLD,"2° Threshold",&m_SecondThreshold,range[0],range[1]);
  m_Gui->Divider(1);
  m_Gui->Integer(ID_MAX_DISTANCE,"Max Dist.",&m_MaxDistance,1,100);
  //m_Gui->Integer(ID_NUM_SECTIONS,"Intervals",&m_NumSections,2,100);
	m_Gui->Divider(1);
	m_Choices[0]="Discrete";
  m_Choices[1]="Continuos";
	m_Gui->Radio(ID_BAR_TIPOLOGY,"Bar Tipology",&m_BarTipology,2,m_Choices);
  m_Gui->Divider(1);

	wxString tip("Threshold value for density maps.");
	wxString area_lab[3];	area_lab[0] = "% l";area_lab[1] = "% m";area_lab[2] = "% h";
	wxColour colour[3] = {m_LowColour,m_MidColour,m_HiColour};
	wxPoint p = wxDefaultPosition;
	wxSize  s = wxSize(34,16);
	wxBoxSizer   *sizer = new wxBoxSizer(wxHORIZONTAL);
  for(int i=0; i<3; i++)
	{
		wxStaticText *lab = new wxStaticText(m_Gui,-1,area_lab[i],p,s,wxALIGN_CENTRE);
		wxTextCtrl   *tex = new wxTextCtrl  (m_Gui,-1,"",         p,s,wxNO_BORDER);
		tex->SetBackgroundColour(colour[i]);
		tex->SetToolTip(tip);
		tex->SetValidator( mmgValidator(this,ID_AREA,tex,&m_Area[i], MINFLOAT,MAXFLOAT,3) ); 
		sizer->Add(lab, 0,wxRIGHT,1);
		sizer->Add(tex,0,wxRIGHT,1);
	}

	m_Gui->Add(sizer,0,wxALL,1);
	m_Gui->Divider(1);

	wxString tip_distance("Threshold value for distance maps.");
	wxString area_lab_distance[3];	area_lab_distance[0] = "% l";area_lab_distance[1] = "% m";area_lab_distance[2] = "% h";
	wxColour colour_distance[3] = {m_LowColour,m_WhiteColour,m_HiColour};
	wxPoint p1 = wxDefaultPosition;
	wxSize  s1 = wxSize(34,16);
	wxBoxSizer   *sizer_distance = new wxBoxSizer(wxHORIZONTAL);
  for(int i=0; i<3; i++)
	{
		wxStaticText *lab = new wxStaticText(m_Gui,-1,area_lab_distance[i],p1,s1,wxALIGN_CENTRE);
		wxTextCtrl   *tex = new wxTextCtrl  (m_Gui,-1,"",         p1,s1,wxNO_BORDER);
		tex->SetBackgroundColour(colour_distance[i]);
		tex->SetToolTip(tip);
		tex->SetValidator( mmgValidator(this,ID_AREA_DISTANCE,tex,&m_AreaDistance[i], MINFLOAT,MAXFLOAT,3) ); 
		sizer_distance->Add(lab, 0,wxRIGHT,1);
		sizer_distance->Add(tex,0,wxRIGHT,1);
	}
	m_Gui->Add(sizer_distance,0,wxALL,1);

	m_Gui->Divider(1);
  m_Gui->Button(ID_SELECT_VOLUME,"Select Volume");
  if(!m_Volume)
  {
    m_Gui->Enable(ID_DENSITY_DISTANCE,false);
    m_Gui->Enable(ID_FIRST_THRESHOLD,false);
    m_Gui->Enable(ID_SECOND_THRESHOLD,false);
    m_Gui->Enable(ID_MAX_DISTANCE,false);
		m_Gui->Enable(ID_NUM_SECTIONS,false);
		m_Gui->Enable(ID_BAR_TIPOLOGY,false);
  }
	else
	{
		m_Gui->Enable(ID_SECOND_THRESHOLD,false);
		m_Gui->Enable(ID_BAR_TIPOLOGY,false);
	}
  return m_Gui;
}
//----------------------------------------------------------------------------
void medPipeDensityDistance::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId()) 
    {
    case ID_DENSITY_DISTANCE:
      {
        if(m_DensityDistance==0)
        {
          m_Gui->Enable(ID_SECOND_THRESHOLD,false);
          m_Gui->Enable(ID_MAX_DISTANCE,true);
					m_Gui->Enable(ID_NUM_SECTIONS,true);
					m_Gui->Enable(ID_BAR_TIPOLOGY,false);
					m_Area[0]=0;
					m_Area[1]=0;
					m_Area[2]=0;
					m_Gui->Update();
          UpdatePipeline();
        }
        else if(m_DensityDistance==1)
        {
          m_Gui->Enable(ID_SECOND_THRESHOLD,true);
          m_Gui->Enable(ID_MAX_DISTANCE,false);
					m_Gui->Enable(ID_NUM_SECTIONS,false);
					m_Gui->Enable(ID_BAR_TIPOLOGY,true);
					m_AreaDistance[0]=0;
					m_AreaDistance[1]=0;
					m_AreaDistance[2]=0;
					m_Gui->Update();
					m_ScalarBar->SetMaximumNumberOfColors(m_NumSections);
					m_ScalarBar->Modified();
          UpdatePipeline();
        }
      }
      break;
    case ID_MAX_DISTANCE:
      {
        UpdatePipeline();
      }
      break;
    case ID_FIRST_THRESHOLD:
      {
        if(m_DensityDistance==1)
        {
          if(m_FirstThreshold>=m_SecondThreshold)
            UpdatePipeline();
          else
            mafMessage("Invalid Thresholds");
        }
        else
        {
          UpdatePipeline();
        }
      } 
      break;
    case ID_SECOND_THRESHOLD:
      {
				if(m_DensityDistance==1)
        {
					if(m_FirstThreshold>=m_SecondThreshold)
						UpdatePipeline();
					else
						mafMessage("Invalid Thresholds");
				}
				else
				{
					UpdatePipeline();
				}
      }
      break;
    case ID_SELECT_VOLUME:
      {
        mafString title = "Choose Volume";
        e->SetArg((long)m_VolumeAccept);
        e->SetString(&title);
        e->SetId(VME_CHOOSE);
        mafEventMacro(*e);
        mafNode *NewVolume = e->GetVme();
        if(NewVolume == NULL)
          return;
        else
        {
          m_Volume=NewVolume;

          mafVMEOutputSurface *surface_output = mafVMEOutputSurface::SafeDownCast(m_Vme->GetOutput());

          m_DistanceFilter->SetSource(((mafVME*)m_Volume)->GetOutput()->GetVTKData());
	        m_DistanceFilter->SetInput((vtkDataSet *)m_Normals->GetOutput());
          m_DistanceFilter->SetMaxDistance(m_MaxDistance);
	        m_DistanceFilter->SetThreshold(m_FirstThreshold);
	        m_DistanceFilter->SetDistanceModeToScalar();
	        m_DistanceFilter->SetInputMatrix(surface_output->GetAbsMatrix()->GetVTKMatrix());
          m_DistanceFilter->Modified();

					int i;
					for (i=-4*m_MaxDistance;i<-m_MaxDistance;i++)
						m_Table->AddRGBPoint(i,m_LowColour.Red()/255.0, m_LowColour.Green()/255.0,	m_LowColour.Blue()/255.0);
					for (i=-m_MaxDistance;i<m_MaxDistance;i++)
						m_Table->AddRGBPoint(i,1.0,1.0,1.0);
					for (i=m_MaxDistance;i<=4*m_MaxDistance;i++)
						m_Table->AddRGBPoint(i,m_HiColour.Red()/255.0, m_HiColour.Green()/255.0,	m_HiColour.Blue()/255.0);
				  
					m_Mapper->SetInput((vtkPolyData*)m_DistanceFilter->GetOutput());
					m_Mapper->Modified();

					//Calculate the areas
					vtkMAFSmartPointer<vtkMassProperties> mass_all;
					mass_all->SetInput(m_DistanceFilter->GetPolyDataOutput());
					mass_all->Update();

					double total_area = mass_all->GetSurfaceArea();

					vtkMAFSmartPointer<vtkClipPolyData> clipHigh;
					clipHigh->SetInput(m_DistanceFilter->GetPolyDataOutput());
					clipHigh->SetValue(m_MaxDistance);
					clipHigh->GenerateClippedOutputOn();
					clipHigh->Update();

					vtkMAFSmartPointer<vtkClipPolyData> clipMidLow;
					clipMidLow->SetInput(clipHigh->GetClippedOutput());
					clipMidLow->SetValue(-m_MaxDistance);
					clipMidLow->GenerateClippedOutputOn();
					clipMidLow->Update();

					vtkMAFSmartPointer<vtkMassProperties> mass_high;
					mass_high->SetInput(clipHigh->GetOutput());
					mass_high->Update();

					vtkMAFSmartPointer<vtkMassProperties> mass_mid;
					mass_mid->SetInput(clipMidLow->GetOutput());
					mass_mid->Update();

					vtkMAFSmartPointer<vtkMassProperties> mass_low;
					mass_low->SetInput(clipMidLow->GetClippedOutput());
					mass_low->Update();

					m_AreaDistance[0] = (mass_low->GetSurfaceArea() / total_area) * 100.0;
					m_AreaDistance[1] = (mass_mid->GetSurfaceArea() / total_area) * 100.0;
					m_AreaDistance[2] = (mass_high->GetSurfaceArea() / total_area) * 100.0;

					m_ScalarBar->SetMaximumNumberOfColors(3);
					m_ScalarBar->Modified();

          m_Gui->Enable(ID_DENSITY_DISTANCE,true);
          m_Gui->Enable(ID_FIRST_THRESHOLD,true);
          m_Gui->Enable(ID_SECOND_THRESHOLD,true);
          m_Gui->Enable(ID_MAX_DISTANCE,true);
					m_Gui->Enable(ID_NUM_SECTIONS,true);
          m_Gui->Update();
          UpdatePipeline();
        }
      }
      break;
		case ID_NUM_SECTIONS:
			{
				if(m_DensityDistance==0)
				{
					m_ScalarBar->SetMaximumNumberOfColors(m_NumSections);
					m_ScalarBar->Modified();
					UpdatePipeline();
					mafEventMacro(mafEvent(this,CAMERA_UPDATE));
				}
			}
			break;
		case ID_BAR_TIPOLOGY:
			{
				UpdatePipeline();
			}
			break;
      default:
        mafEventMacro(*e);
      break;
    }
  }
}
//----------------------------------------------------------------------------
void medPipeDensityDistance::SetVolume(mafNode *volume)
//----------------------------------------------------------------------------
{
  m_Volume=volume;
}
//----------------------------------------------------------------------------
void medPipeDensityDistance::UpdatePipeline()
//----------------------------------------------------------------------------
{
  m_Table->RemoveAllPoints();
	m_DistanceFilter->SetThreshold(m_FirstThreshold);

  if(m_DensityDistance==0)
  {
    m_ScalarBar->SetTitle("Distance");
		m_ScalarBar->SetMaximumNumberOfColors(m_NumSections);
    m_ScalarBar->Modified();

    m_DistanceFilter->SetFilterModeToDistance();
    m_DistanceFilter->SetMaxDistance(m_MaxDistance);
		m_DistanceFilter->Update();

		/*double i;
		m_Table->AddRGBPoint(-m_DistanceFilter->GetMaxDistance(), 1.f, 0.f, 0.0f);
		double step=(double)(m_MaxDistance*2)/(m_NumSections-1);
		double init=(double)(-m_MaxDistance+step);
		for(i=init;i<m_MaxDistance;i+=step)
		{
			if(i<=0)
				m_Table->AddRGBPoint(i, 1.f, (float)(1+(i/m_MaxDistance)), (float)(1+(i/m_MaxDistance)));
			else
				m_Table->AddRGBPoint(i, (float)(1-(i/m_MaxDistance)), (float)(1-(i/m_MaxDistance)),1.f );
		}
		m_Table->AddRGBPoint(m_DistanceFilter->GetMaxDistance(), 0.1f, 0.1f, 1.f);

		/*m_Table->AddRGBPoint(-m_DistanceFilter->GetMaxDistance(), 1.f, 0.f, 0.0f);
		m_Table->AddRGBPoint(-0.5f *	m_DistanceFilter->GetMaxDistance(), 1.f, 0.1f, 1.f);
		m_Table->AddRGBPoint(0.5f *	m_DistanceFilter->GetMaxDistance(), 0.9f, 0.9f, 1.f);
		m_Table->AddRGBPoint(					m_DistanceFilter->GetMaxDistance(), 0.1f, 0.1f, 1.f);
		m_Table->Build();

		m_ScalarBar->SetMaximumNumberOfColors(m_NumSections);
		m_ScalarBar->Modified();
		m_Mapper->Modified();

		//Calculate the areas
		vtkMAFSmartPointer<vtkMassProperties> mass_all;
		mass_all->SetInput(m_DistanceFilter->GetPolyDataOutput());
		mass_all->Update();

		double total_area = mass_all->GetSurfaceArea();

		double area=0;
		step=(double)(m_MaxDistance*2)/(m_NumSections);
		mafString message;

		vtkMAFSmartPointer<vtkClipPolyData> clip;
		clip->SetInput(m_DistanceFilter->GetPolyDataOutput());
		clip->SetValue(m_MaxDistance);
		clip->GenerateClippedOutputOn();
		clip->Update();

		vtkMAFSmartPointer<vtkMassProperties> mass;
		mass->SetInput(clip->GetOutput());
		mass->Update();
		area = (mass->GetSurfaceArea() / total_area) * 100.0;

		message= wxString::Format("From infinity To %d\t%.3lf %" , m_MaxDistance,area);
		mafLogMessage(message);
	
		vtkMAFSmartPointer<vtkClipPolyData> clip_old;
		clip_old=clip;
		for (i=m_MaxDistance-step;i>=-m_MaxDistance;i-=step)
		{
			vtkMAFSmartPointer<vtkClipPolyData> clip;
			clip->SetInput(clip_old->GetClippedOutput());
			clip->SetValue(i);
			clip->GenerateClippedOutputOn();
			clip->Update();

			clip_old=clip;

			vtkMAFSmartPointer<vtkMassProperties> mass;
			mass->SetInput(clip->GetOutput());
			mass->Update();
			area = (mass->GetSurfaceArea() / total_area) * 100.0;

			message= wxString::Format("From %.3lf To %.3lf\t%.3lf %" , i+step,i,area);
			mafLogMessage(message);
		}

		vtkMAFSmartPointer<vtkMassProperties> mass_final;
		mass_final->SetInput(clip_old->GetClippedOutput());
		mass_final->Update();
		area = (mass_final->GetSurfaceArea() / total_area) * 100.0;

		message= wxString::Format("From %d To -infinity\t%.3lf %" , -m_MaxDistance,area);
		mafLogMessage(message);*/
		
		int i;
		for (i=-4*m_MaxDistance;i<-m_MaxDistance;i++)
			m_Table->AddRGBPoint(i,m_LowColour.Red()/255.0, m_LowColour.Green()/255.0,	m_LowColour.Blue()/255.0);
		for (i=-m_MaxDistance;i<m_MaxDistance;i++)
			m_Table->AddRGBPoint(i,1.0,1.0,1.0);
		for (i=m_MaxDistance;i<=4*m_MaxDistance;i++)
			m_Table->AddRGBPoint(i,m_HiColour.Red()/255.0, m_HiColour.Green()/255.0,	m_HiColour.Blue()/255.0);
	  
		m_ScalarBar->SetMaximumNumberOfColors(3);
		m_ScalarBar->Modified();
		m_Mapper->Modified();

		//Calculate the areas
		vtkMAFSmartPointer<vtkMassProperties> mass_all;
		mass_all->SetInput(m_DistanceFilter->GetPolyDataOutput());
		mass_all->Update();

		double total_area = mass_all->GetSurfaceArea();

		vtkMAFSmartPointer<vtkClipPolyData> clipHigh;
		clipHigh->SetInput(m_DistanceFilter->GetPolyDataOutput());
		clipHigh->SetValue(m_MaxDistance);
		clipHigh->GenerateClippedOutputOn();
		clipHigh->Update();

		vtkMAFSmartPointer<vtkClipPolyData> clipMidLow;
		clipMidLow->SetInput(clipHigh->GetClippedOutput());
		clipMidLow->SetValue(-m_MaxDistance);
		clipMidLow->GenerateClippedOutputOn();
		clipMidLow->Update();

		vtkMAFSmartPointer<vtkMassProperties> mass_high;
		mass_high->SetInput(clipHigh->GetOutput());
		mass_high->Update();

		vtkMAFSmartPointer<vtkMassProperties> mass_mid;
		mass_mid->SetInput(clipMidLow->GetOutput());
		mass_mid->Update();

		vtkMAFSmartPointer<vtkMassProperties> mass_low;
		mass_low->SetInput(clipMidLow->GetClippedOutput());
		mass_low->Update();

		m_AreaDistance[0] = (mass_low->GetSurfaceArea() / total_area) * 100.0;
		m_AreaDistance[1] = (mass_mid->GetSurfaceArea() / total_area) * 100.0;
		m_AreaDistance[2] = (mass_high->GetSurfaceArea() / total_area) * 100.0;

		m_Gui->Update();

  }
  else if(m_DensityDistance==1)
  {
    m_ScalarBar->SetTitle("Density");
    m_ScalarBar->Modified();

    m_DistanceFilter->SetFilterModeToDensity();
		m_DistanceFilter->Update();

		double range[2];
		((mafVME*)m_Volume)->GetOutput()->GetVTKData()->GetScalarRange(range);

		if(m_BarTipology==0)
		{
			int i;
			for (i=range[0];i<m_SecondThreshold;i++)
				m_Table->AddRGBPoint(i,m_LowColour.Red()/255.0, m_LowColour.Green()/255.0,	m_LowColour.Blue()/255.0);
			for (i=m_SecondThreshold;i<m_FirstThreshold;i++)
				m_Table->AddRGBPoint(i,m_MidColour.Red()/255.0, m_MidColour.Green()/255.0,	m_MidColour.Blue()/255.0);
			for (i=m_FirstThreshold;i<=range[1];i++)
				m_Table->AddRGBPoint(i,m_HiColour.Red()/255.0, m_HiColour.Green()/255.0,	m_HiColour.Blue()/255.0);
		}
		else if(m_BarTipology==1)
		{
			m_Table->AddRGBPoint(range[0],m_LowColour.Red()/255.0, m_LowColour.Green()/255.0,	m_LowColour.Blue()/255.0);
			m_Table->AddRGBPoint(m_SecondThreshold,m_MidColour1.Red()/255.0, m_MidColour1.Green()/255.0,	m_MidColour1.Blue()/255.0);
			m_Table->AddRGBPoint(m_FirstThreshold,m_MidColour2.Red()/255.0, m_MidColour2.Green()/255.0,	m_MidColour2.Blue()/255.0);
			m_Table->AddRGBPoint(range[1],m_HiColour.Red()/255.0, m_HiColour.Green()/255.0,	m_HiColour.Blue()/255.0);
		}

		m_Table->Build();

    m_ScalarBar->SetMaximumNumberOfColors(range[1]-range[0]);
		m_ScalarBar->Modified();

		m_Mapper->Modified();


		//Calculate the areas
		vtkMAFSmartPointer<vtkMassProperties> mass_all;
		mass_all->SetInput(m_DistanceFilter->GetPolyDataOutput());
		mass_all->Update();

		double total_area = mass_all->GetSurfaceArea();

		vtkMAFSmartPointer<vtkClipPolyData> clipHigh;
		clipHigh->SetInput(m_DistanceFilter->GetPolyDataOutput());
		clipHigh->SetValue(m_FirstThreshold);
		clipHigh->GenerateClippedOutputOn();
		clipHigh->Update();

		vtkMAFSmartPointer<vtkClipPolyData> clipMidLow;
		clipMidLow->SetInput(clipHigh->GetClippedOutput());
		clipMidLow->SetValue(m_SecondThreshold);
		clipMidLow->GenerateClippedOutputOn();
		clipMidLow->Update();

		vtkMAFSmartPointer<vtkMassProperties> mass_high;
		mass_high->SetInput(clipHigh->GetOutput());
		mass_high->Update();

		vtkMAFSmartPointer<vtkMassProperties> mass_mid;
		mass_mid->SetInput(clipMidLow->GetOutput());
		mass_mid->Update();

		vtkMAFSmartPointer<vtkMassProperties> mass_low;
		mass_low->SetInput(clipMidLow->GetClippedOutput());
		mass_low->Update();

		m_Area[0] = (mass_low->GetSurfaceArea() / total_area) * 100.0;
		m_Area[1] = (mass_mid->GetSurfaceArea() / total_area) * 100.0;
		m_Area[2] = (mass_high->GetSurfaceArea() / total_area) * 100.0;

		m_Gui->Update();
  }

  m_Actor->Modified();

  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
  
}
