/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeDensityDistance
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaPipeDensityDistance.h"
#include "albaSceneNode.h"
#include "albaVMESurface.h"
#include "mmaMaterial.h"
#include "albaGUI.h"
#include "albaGUIMaterialButton.h" 
#include "albaAxes.h"
#include "albaDataVector.h"
#include "albaVMEGenericAbstract.h"
#include "albaVME.h"
#include "albaGUIValidator.h"
#include "vtkALBASmartPointer.h"

#include "vtkALBAAssembly.h"
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

#include "vtkColorTransferFunction.h"
#include "vtkScalarBarActor.h"
#include "vtkActor2D.h"
#include "vtkLookupTable.h"
#include "vtkMassProperties.h"
#include "vtkClipPolyData.h"
#include "vtkTriangleFilter.h"

#include <vector>

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaPipeDensityDistance);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaPipeDensityDistance::albaPipeDensityDistance()
:albaPipe()
//----------------------------------------------------------------------------
{
  m_Mapper          = NULL;
  m_Actor           = NULL;
  m_DistanceFilter  = NULL;
  m_Table           = NULL;
  m_Normals         = NULL;
	m_TriangleFilter  = NULL;
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

  m_EnableMAPSFilter = true;
}
//----------------------------------------------------------------------------
void albaPipeDensityDistance::Create(albaSceneNode *n/*, bool use_axes*/)
//----------------------------------------------------------------------------
{
  Superclass::Create(n);

  vtkNEW(m_Normals);
	vtkNEW(m_TriangleFilter);
	vtkNEW(m_DistanceFilter);
	vtkNEW(m_Mapper);
	vtkNEW(m_Actor);
	vtkNEW(m_Table);
  vtkNEW(m_ScalarBar);

  assert(m_Vme->GetOutput()->IsALBAType(albaVMEOutputSurface));
  albaVMEOutputSurface *surface_output = albaVMEOutputSurface::SafeDownCast(m_Vme->GetOutput());
  assert(surface_output);
  surface_output->Update();
  vtkPolyData *data = vtkPolyData::SafeDownCast(surface_output->GetVTKData());
  assert(data);

  m_Vme->AddObserver(this);

	m_TriangleFilter->SetInputData(data);
	m_TriangleFilter->Update();

	m_Normals->SetInputConnection(m_TriangleFilter->GetOutputPort());
	m_Normals->ComputePointNormalsOn();
	m_Normals->SplittingOff();
	m_Normals->Update();

  if (m_Volume)
  {
	  m_DistanceFilter->SetSource(m_Volume->GetOutput()->GetVTKData());
	  m_DistanceFilter->SetInputConnection(m_Normals->GetOutputPort());
	  m_DistanceFilter->SetMaxDistance(m_MaxDistance);
	  m_DistanceFilter->SetThreshold(m_FirstThreshold);
	  m_DistanceFilter->SetDistanceModeToScalar();
	  m_DistanceFilter->SetInputMatrix(surface_output->GetAbsMatrix()->GetVTKMatrix());
		m_DistanceFilter->Update();

		int i;
		for (i=-4*m_MaxDistance;i<-m_MaxDistance;i++)
			m_Table->AddRGBPoint(i,m_LowColour.Red()/255.0, m_LowColour.Green()/255.0,	m_LowColour.Blue()/255.0);
		for (i=-m_MaxDistance;i<m_MaxDistance;i++)
			m_Table->AddRGBPoint(i,1.0,1.0,1.0);
		for (i=m_MaxDistance;i<=4*m_MaxDistance;i++)
			m_Table->AddRGBPoint(i,m_HiColour.Red()/255.0, m_HiColour.Green()/255.0,	m_HiColour.Blue()/255.0);
	  
		m_Mapper->SetInputConnection(m_DistanceFilter->GetOutputPort());

		//Calculate the areas
		vtkALBASmartPointer<vtkMassProperties> mass_all;
		mass_all->SetInputConnection(m_DistanceFilter->GetOutputPort());
		mass_all->Update();

		double total_area = mass_all->GetSurfaceArea();

		vtkALBASmartPointer<vtkClipPolyData> clipHigh;
		clipHigh->SetInputConnection(m_DistanceFilter->GetOutputPort());
		clipHigh->SetValue(m_MaxDistance);
		clipHigh->GenerateClippedOutputOn();
		clipHigh->Update();
		
		vtkALBASmartPointer<vtkClipPolyData> clipMidHight;
		clipMidHight->SetInputConnection(clipHigh->GetClippedOutputPort());
		clipMidHight->SetValue(0);
		clipMidHight->GenerateClippedOutputOn();
		clipMidHight->Update();

		vtkALBASmartPointer<vtkClipPolyData> clipMidLow;
		clipMidLow->SetInputConnection(clipMidHight->GetClippedOutputPort());
		clipMidLow->SetValue(-m_MaxDistance);
		clipMidLow->GenerateClippedOutputOn();
		clipMidLow->Update();

			vtkALBASmartPointer<vtkMassProperties> mass_high;
			mass_high->SetInputConnection(clipHigh->GetOutputPort());
			mass_high->Update();

		/*vtkALBASmartPointer<vtkMassProperties> mass_mid1;
		mass_mid1->SetInput(clipMidHight->GetOutput());
		mass_mid1->Update();*/

			vtkALBASmartPointer<vtkMassProperties> mass_mid;			
			mass_mid->SetInputConnection(clipMidLow->GetOutputPort());
			mass_mid->Update();
		
			vtkALBASmartPointer<vtkMassProperties> mass_low;
			mass_low->SetInputConnection(clipMidLow->GetClippedOutputPort());
			mass_low->Update();

		//double area[4];
		m_AreaDistance[0] = (mass_low->GetSurfaceArea() / total_area) * 100.0;
		m_AreaDistance[1] = (mass_mid->GetSurfaceArea() / total_area) * 100.0;
		m_AreaDistance[2] = (mass_high->GetSurfaceArea() / total_area) * 100.0;

		/*albaString message;
		message= albaString::Format("From %d To infinity\t%.3lf %" , m_MaxDistance,area[2]);
		albaLogMessage(message);
		message= albaString::Format("From %d To %d\t%.3lf %" , -m_MaxDistance,m_MaxDistance,area[1]);
		albaLogMessage(message);
		message= albaString::Format("From %d To -infinity\t%.3lf %" , m_MaxDistance,area[0]);
		albaLogMessage(message);*/
  }
  else
  {
    m_Mapper->SetInputConnection(m_Normals->GetOutputPort());
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
  m_ScalarBar->SetMaximumNumberOfColors(m_MaxDistance*8);
	m_ScalarBar->SetLabelFormat("%-#6.0f");

  m_AssemblyFront->AddPart(m_Actor);
  m_RenFront->AddActor2D(m_ScalarBar);
}
//----------------------------------------------------------------------------
albaPipeDensityDistance::~albaPipeDensityDistance()
//----------------------------------------------------------------------------
{
  m_RenFront->RemoveActor2D(m_ScalarBar);
  m_AssemblyFront->RemovePart(m_Actor);
  m_Vme->RemoveObserver(this);

  vtkDEL(m_Normals);
	vtkDEL(m_TriangleFilter);
	vtkDEL(m_Mapper);
  vtkDEL(m_Actor);
  vtkDEL(m_DistanceFilter);
  vtkDEL(m_Table);
  vtkDEL(m_ScalarBar);
  //cppDEL(m_Axes);
}
//----------------------------------------------------------------------------
void albaPipeDensityDistance::Select(bool sel)
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
albaGUI *albaPipeDensityDistance::CreateGui()
//----------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = new albaGUI(this);
  
  wxString m_Choices[2];
  m_Choices[0]="Distance";
  m_Choices[1]="Density";
  m_Gui->Radio(ID_DENSITY_DISTANCE,"",&m_DensityDistance,2,m_Choices);

  double range[2];
  range[0]=m_SecondThreshold;
  range[1]=m_FirstThreshold;
  if (m_Volume)
    m_Volume->GetOutput()->GetVTKData()->GetScalarRange(range);

	m_Gui->Divider(1);
  m_Gui->Integer(ID_FIRST_THRESHOLD,"1° Threshold",&m_FirstThreshold,range[0],range[1]);
  m_Gui->Integer(ID_SECOND_THRESHOLD,"2° Threshold",&m_SecondThreshold,range[0],range[1]);
  m_Gui->Divider(1);
  m_Gui->Integer(ID_MAX_DISTANCE,"Max Dist.",&m_MaxDistance,1,100);
  //m_Gui->Integer(ID_NUM_SECTIONS,"Intervals",&m_NumSections,2,100);
	m_Gui->Divider(1);
	m_Choices[0]=_("Discrete");
  m_Choices[1]=_("Continuos");
	m_Gui->Radio(ID_BAR_TIPOLOGY,_("Bar Tipology"),&m_BarTipology,2,m_Choices);
  m_Gui->Divider(1);

	wxString tip(_("Threshold value for density maps."));
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
		tex->SetValidator( albaGUIValidator(this,ID_AREA,tex,&m_Area[i], MINFLOAT,MAXFLOAT,3) ); 
		sizer->Add(lab, 0,wxRIGHT,1);
		sizer->Add(tex,0,wxRIGHT,1);
	}

	m_Gui->Add(sizer,0,wxALL,1);
	m_Gui->Divider(1);

	wxString tip_distance(_("Threshold value for distance maps."));
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
		tex->SetValidator( albaGUIValidator(this,ID_AREA_DISTANCE,tex,&m_AreaDistance[i], MINFLOAT,MAXFLOAT,3) ); 
		sizer_distance->Add(lab, 0,wxRIGHT,1);
		sizer_distance->Add(tex,0,wxRIGHT,1);
	}
	m_Gui->Add(sizer_distance,0,wxALL,1);

	m_Gui->Divider(1);
  m_Gui->Button(ID_SELECT_VOLUME,_("Select Volume"));
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
	m_Gui->Divider();
  return m_Gui;
}
//----------------------------------------------------------------------------
void albaPipeDensityDistance::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
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
            albaMessage(_("Invalid Thresholds"));
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
						albaMessage(_("Invalid Thresholds"));
				}
				else
				{
					UpdatePipeline();
				}
      }
      break;
    case ID_SELECT_VOLUME:
      {
        albaString title = _("Choose Volume");
        e->SetPointer(&albaPipeDensityDistance::VolumeAccept);
        e->SetString(&title);
        e->SetId(VME_CHOOSE);
        albaEventMacro(*e);
        albaVME *NewVolume = e->GetVme();
        if(NewVolume == NULL)
          return;
        else
        {
          m_Volume=NewVolume;

          albaVMEOutputSurface *surface_output = albaVMEOutputSurface::SafeDownCast(m_Vme->GetOutput());

          m_DistanceFilter->SetSource(m_Volume->GetOutput()->GetVTKData());
	        m_DistanceFilter->SetInputConnection(m_Normals->GetOutputPort());
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
				  
					m_Mapper->SetInputConnection(m_DistanceFilter->GetOutputPort());
					m_Mapper->Modified();

					//Calculate the areas
					vtkALBASmartPointer<vtkMassProperties> mass_all;
					mass_all->SetInputConnection(m_DistanceFilter->GetOutputPort());
					mass_all->Update();

					double total_area = mass_all->GetSurfaceArea();

					vtkALBASmartPointer<vtkClipPolyData> clipHigh;
					clipHigh->SetInputConnection(m_DistanceFilter->GetOutputPort());
					clipHigh->SetValue(m_MaxDistance);
					clipHigh->GenerateClippedOutputOn();
					clipHigh->Update();

					vtkALBASmartPointer<vtkClipPolyData> clipMidLow;
					clipMidLow->SetInputConnection(clipHigh->GetClippedOutputPort());
					clipMidLow->SetValue(-m_MaxDistance);
					clipMidLow->GenerateClippedOutputOn();
					clipMidLow->Update();

					vtkALBASmartPointer<vtkMassProperties> mass_high;
					mass_high->SetInputConnection(clipHigh->GetOutputPort());
					mass_high->Update();

					vtkALBASmartPointer<vtkMassProperties> mass_mid;
					mass_mid->SetInputConnection(clipMidLow->GetOutputPort());
					mass_mid->Update();

					vtkALBASmartPointer<vtkMassProperties> mass_low;
					mass_low->SetInputConnection(clipMidLow->GetClippedOutputPort());
					mass_low->Update();

					m_AreaDistance[0] = (mass_low->GetSurfaceArea() / total_area) * 100.0;
					m_AreaDistance[1] = (mass_mid->GetSurfaceArea() / total_area) * 100.0;
					m_AreaDistance[2] = (mass_high->GetSurfaceArea() / total_area) * 100.0;

					m_ScalarBar->SetMaximumNumberOfColors(m_MaxDistance*8);
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
				}
			}
			break;
		case ID_BAR_TIPOLOGY:
			{
				UpdatePipeline();
			}
			break;
      default:
        albaEventMacro(*e);
      break;
    }
  }
  else if (alba_event->GetId() == VME_ABSMATRIX_UPDATE)
  {
    UpdatePipeline();
  }
}
//----------------------------------------------------------------------------
void albaPipeDensityDistance::SetVolume(albaVME *volume)
//----------------------------------------------------------------------------
{
  m_Volume=volume;
}
//----------------------------------------------------------------------------
void albaPipeDensityDistance::UpdatePipeline()
//----------------------------------------------------------------------------
{
  if(m_DistanceFilter)
  {
    m_Table->RemoveAllPoints();
	  m_DistanceFilter->SetThreshold(m_FirstThreshold);

    if(m_DensityDistance==0)
    {
      m_ScalarBar->SetTitle(_("Distance"));
		  m_ScalarBar->SetMaximumNumberOfColors(m_NumSections);
      m_ScalarBar->Modified();

      albaVMEOutputSurface *surface_output = albaVMEOutputSurface::SafeDownCast(m_Vme->GetOutput());

      m_DistanceFilter->SetInputMatrix(surface_output->GetAbsMatrix()->GetVTKMatrix());

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
		  vtkALBASmartPointer<vtkMassProperties> mass_all;
		  mass_all->SetInput(m_DistanceFilter->GetPolyDataOutput());
		  mass_all->Update();

		  double total_area = mass_all->GetSurfaceArea();

		  double area=0;
		  step=(double)(m_MaxDistance*2)/(m_NumSections);
		  albaString message;

		  vtkALBASmartPointer<vtkClipPolyData> clip;
		  clip->SetInput(m_DistanceFilter->GetPolyDataOutput());
		  clip->SetValue(m_MaxDistance);
		  clip->GenerateClippedOutputOn();
		  clip->Update();

		  vtkALBASmartPointer<vtkMassProperties> mass;
		  mass->SetInput(clip->GetOutput());
		  mass->Update();
		  area = (mass->GetSurfaceArea() / total_area) * 100.0;

		  message= albaString::Format("From infinity To %d\t%.3lf %" , m_MaxDistance,area);
		  albaLogMessage(message);
  	
		  vtkALBASmartPointer<vtkClipPolyData> clip_old;
		  clip_old=clip;
		  for (i=m_MaxDistance-step;i>=-m_MaxDistance;i-=step)
		  {
			  vtkALBASmartPointer<vtkClipPolyData> clip;
			  clip->SetInput(clip_old->GetClippedOutput());
			  clip->SetValue(i);
			  clip->GenerateClippedOutputOn();
			  clip->Update();

			  clip_old=clip;

			  vtkALBASmartPointer<vtkMassProperties> mass;
			  mass->SetInput(clip->GetOutput());
			  mass->Update();
			  area = (mass->GetSurfaceArea() / total_area) * 100.0;

			  message= albaString::Format("From %.3lf To %.3lf\t%.3lf %" , i+step,i,area);
			  albaLogMessage(message);
		  }

		  vtkALBASmartPointer<vtkMassProperties> mass_final;
		  mass_final->SetInput(clip_old->GetClippedOutput());
		  mass_final->Update();
		  area = (mass_final->GetSurfaceArea() / total_area) * 100.0;

		  message= albaString::Format("From %d To -infinity\t%.3lf %" , -m_MaxDistance,area);
		  albaLogMessage(message);*/
  		
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
		  vtkALBASmartPointer<vtkMassProperties> mass_all;
		  mass_all->SetInputConnection(m_DistanceFilter->GetOutputPort());
		  mass_all->Update();

		  double total_area = mass_all->GetSurfaceArea();

		  vtkALBASmartPointer<vtkClipPolyData> clipHigh;
		  clipHigh->SetInputConnection(m_DistanceFilter->GetOutputPort());
		  clipHigh->SetValue(m_MaxDistance);
		  clipHigh->GenerateClippedOutputOn();
		  clipHigh->Update();

		  vtkALBASmartPointer<vtkClipPolyData> clipMidLow;
		  clipMidLow->SetInputConnection(clipHigh->GetClippedOutputPort());
		  clipMidLow->SetValue(-m_MaxDistance);
		  clipMidLow->GenerateClippedOutputOn();
		  clipMidLow->Update();

		  vtkALBASmartPointer<vtkMassProperties> mass_high;
		  mass_high->SetInputConnection(clipHigh->GetOutputPort());
		  mass_high->Update();

		  vtkALBASmartPointer<vtkMassProperties> mass_mid;
		  mass_mid->SetInputConnection(clipMidLow->GetOutputPort());
		  mass_mid->Update();

		  vtkALBASmartPointer<vtkMassProperties> mass_low;
		  mass_low->SetInputConnection(clipMidLow->GetClippedOutputPort());
		  mass_low->Update();

		  m_AreaDistance[0] = (mass_low->GetSurfaceArea() / total_area) * 100.0;
		  m_AreaDistance[1] = (mass_mid->GetSurfaceArea() / total_area) * 100.0;
		  m_AreaDistance[2] = (mass_high->GetSurfaceArea() / total_area) * 100.0;

      if(m_Gui)
        m_Gui->Update();

    }
    else if(m_DensityDistance==1)
    {
      m_ScalarBar->SetTitle(_("Density"));
      m_ScalarBar->Modified();

      albaVMEOutputSurface *surface_output = albaVMEOutputSurface::SafeDownCast(m_Vme->GetOutput());
      m_DistanceFilter->SetInputMatrix(surface_output->GetAbsMatrix()->GetVTKMatrix());

      m_DistanceFilter->SetFilterModeToDensity();
		  m_DistanceFilter->Update();

		  double range[2];
		  m_Volume->GetOutput()->GetVTKData()->GetScalarRange(range);

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
		  vtkALBASmartPointer<vtkMassProperties> mass_all;
		  mass_all->SetInputConnection(m_DistanceFilter->GetOutputPort());
		  mass_all->Update();

		  double total_area = mass_all->GetSurfaceArea();

		  vtkALBASmartPointer<vtkClipPolyData> clipHigh;
		  clipHigh->SetInputConnection(m_DistanceFilter->GetOutputPort());
		  clipHigh->SetValue(m_FirstThreshold);
		  clipHigh->GenerateClippedOutputOn();
		  clipHigh->Update();

		  vtkALBASmartPointer<vtkClipPolyData> clipMidLow;
		  clipMidLow->SetInputConnection(clipHigh->GetClippedOutputPort());
		  clipMidLow->SetValue(m_SecondThreshold);
		  clipMidLow->GenerateClippedOutputOn();
		  clipMidLow->Update();

		  vtkALBASmartPointer<vtkMassProperties> mass_high;
		  mass_high->SetInputConnection(clipHigh->GetOutputPort());
		  mass_high->Update();

		  vtkALBASmartPointer<vtkMassProperties> mass_mid;
		  mass_mid->SetInputConnection(clipMidLow->GetOutputPort());
		  mass_mid->Update();

		  vtkALBASmartPointer<vtkMassProperties> mass_low;
		  mass_low->SetInputConnection(clipMidLow->GetClippedOutputPort());
		  mass_low->Update();

		  m_Area[0] = (mass_low->GetSurfaceArea() / total_area) * 100.0;
		  m_Area[1] = (mass_mid->GetSurfaceArea() / total_area) * 100.0;
		  m_Area[2] = (mass_high->GetSurfaceArea() / total_area) * 100.0;

      if(m_Gui)
		    m_Gui->Update();
    }

    m_Actor->Modified();

		GetLogicManager()->CameraUpdate();
  }
}
//----------------------------------------------------------------------------
double albaPipeDensityDistance::GetTotalArea()
//----------------------------------------------------------------------------
{
  //Calculate the areas
  vtkALBASmartPointer<vtkMassProperties> mass_all;
  mass_all->SetInputConnection(m_DistanceFilter->GetOutputPort());
  mass_all->Update();

  return mass_all->GetSurfaceArea();
}
//----------------------------------------------------------------------------
void albaPipeDensityDistance::EnableMAPSFilterOff()
//----------------------------------------------------------------------------
{
  m_Mapper->SetInputConnection(m_Normals->GetOutputPort());
  m_Mapper->Update();
  m_EnableMAPSFilter=false;
	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaPipeDensityDistance::EnableMAPSFilterOn()
//----------------------------------------------------------------------------
{
  m_Mapper->SetInputConnection(m_DistanceFilter->GetOutputPort());
  m_Mapper->Update();
  m_EnableMAPSFilter=true;
	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaPipeDensityDistance::EnableMAPSFilter(bool enable)
//----------------------------------------------------------------------------
{
  if(enable)
    EnableMAPSFilterOn();
  else
    EnableMAPSFilterOff();
}