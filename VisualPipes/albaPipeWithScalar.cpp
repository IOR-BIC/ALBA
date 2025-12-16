 /*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeWithScalar
 Authors: Gianluigi Crimi, Nicola Vanella
 
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

#include "albaPipeWithScalar.h"
#include "albaSceneNode.h"
#include "albaGUI.h"
#include "albaGUIMaterialButton.h"
#include "albaAxes.h"
#include "mmaMaterial.h"
#include "albaGUILutPreset.h"
#include "albaDataVector.h"
#include "albaVMESurface.h"
#include "albaGUIMaterialButton.h"
#include "albaVMEItemVTK.h"
#include "vtkALBASmartPointer.h"
#include "vtkALBAAssembly.h"
#include "vtkPointData.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkDataSetMapper.h"
#include "vtkPolyData.h"
#include "vtkProperty.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkLookupTable.h"
#include "vtkCellData.h"
#include "albaGUIDialog.h"
#include "albaGUIHistogramWidget.h"

#include <vector>
#include "vtkALBAPolyDataNormals.h"
#include "albaGUILutSlider.h"
#include "vtkScalarBarActor.h"
#include "vtkTextProperty.h"
#include "vtkALBADistanceFilter.h"
#include "vtkPolyDataWriter.h"


//----------------------------------------------------------------------------
albaPipeWithScalar::albaPipeWithScalar()
:albaPipe()
{

	m_ScalarIndex = 0;
	m_NumberOfArrays = 0;
	m_OldScalarIndex = m_OldActiveScalarType = m_OldMapsGenActive = -1;
	m_NumberOfComponents = 0;
	m_Table = NULL;

	m_ActiveScalarType = POINT_TYPE;
	m_PointCellArraySeparation = 0;

	m_ScalarsInComboBoxNames = NULL;
	m_ScalarsVTKName = NULL;
	m_LutSwatch = NULL;
	m_LutSlider = NULL;
	m_ScalarComboBox = NULL;
	m_ComponentsComboBox = NULL;
	m_ProbeVolume = NULL;

	m_MapsStackActive = m_ProbeMapActive = m_ScalarMapActive   = m_ShowScalarBar = 0;
	m_ScalarBarPos = SB_ON_RIGHT;
	m_ScalarBarActor = NULL;
	m_ScalarBarLabNum = 2;
	m_Histogram = NULL;
	m_Dialog = NULL;
	m_ProbeFilter = NULL;

	m_ShowScalarBar = 0;
}
//----------------------------------------------------------------------------
albaPipeWithScalar::~albaPipeWithScalar()
{
	DeleteHistogramDialog();
	vtkDEL(m_Table);
	cppDEL(m_LutSlider);

	delete[] m_ScalarsInComboBoxNames;
	delete[] m_ScalarsVTKName;
}

//----------------------------------------------------------------------------
void albaPipeWithScalar::ManageScalarOnExecutePipe(vtkDataSet * dataSet)
{
	CreateFieldDataControlArrays();

	m_ObjectMaterial = (mmaMaterial *)m_Vme->GetAttribute("MaterialAttributes");
	if (m_ObjectMaterial == NULL)
	{
		m_ObjectMaterial = mmaMaterial::New();
		m_Vme->SetAttribute("MaterialAttributes", m_ObjectMaterial);
	}


	// point type scalars
	vtkDataArray *scalars = dataSet->GetPointData()->GetScalars();

	double sr[2] = { 0,1 };
	if (scalars)
	{
		scalars->Modified();
		scalars->GetRange(sr);
		m_ActiveScalarType = POINT_TYPE;
	}
	else
	{
		scalars = dataSet->GetCellData()->GetScalars();
		if (scalars)
		{
			scalars->Modified();
			scalars->GetRange(sr);
			m_ActiveScalarType = CELL_TYPE;
		}
	}

	vtkNEW(m_Table);
	m_Table->Build();
	m_Table->DeepCopy(m_ObjectMaterial->m_ColorLut);
	lutPreset(19, m_Table);


	m_Table->SetValueRange(sr);
	m_Table->SetHueRange(0.667, 0.0);
	m_Table->SetTableRange(sr);
	m_Table->Build();
	
	if (m_LutSlider)
	{
		m_LutSlider->SetRange(sr);
		m_LutSlider->SetSubRange(sr);
	}

	m_ObjectMaterial->m_ColorLut->DeepCopy(m_Table);
	m_ObjectMaterial->m_ColorLut->Build();
	m_Mapper->SetLookupTable(m_Table);
	m_Mapper->SetScalarRange(sr);

	if (m_MapsStackActive || m_ActiveScalarType == POINT_TYPE)
		m_Mapper->SetScalarModeToUsePointData();
	else if (m_ActiveScalarType == CELL_TYPE)
		m_Mapper->SetScalarModeToUseCellData();

	if (m_MapsStackActive || m_ScalarMapActive)
		m_Mapper->ScalarVisibilityOn();
	else
		m_Mapper->ScalarVisibilityOff();

	m_Mapper->Modified();
}

//----------------------------------------------------------------------------
void albaPipeWithScalar::CreateScalarsGui(albaGUI *gui)
{
  gui->Divider(2);
	gui->Bool(ID_SCALAR_MAP_ACTIVE,_("Enable scalar field mapping"), &m_ScalarMapActive, 1);
	m_ScalarComboBox=gui->Combo(ID_SCALARS,"",&m_ScalarIndex,m_NumberOfArrays,m_ScalarsInComboBoxNames);
	m_ComponentsComboBox = gui->Combo(ID_COMPONENTS, "", &m_ComponentIndex, m_NumberOfComponents, NULL);

	gui->Divider();
	gui->Bool(ID_PROBE_MAPS, _("Enable Volume Scalar Probe Map"), &m_ProbeMapActive, 1);
	gui->Button(ID_SELECT_PROBE_VME, &m_ProbeVolName, "Select Volume...","Select Scalar Source Volume"),
	gui->Divider(1);
	

	m_LutSwatch = gui->Lut(ID_LUT, "Lut", m_Table);

	m_LutSlider = new albaGUILutSlider(gui, ID_LUT_SLIDER, wxPoint(0, 0), wxSize(304, 22), wxBORDER_NONE);
	m_LutSlider->SetListener(this);
	m_LutSlider->SetFloatingPointTextOn();
	if (m_Table)
	{
		m_LutSlider->SetRange(m_Table->GetRange());
		m_LutSlider->SetSubRange(m_Table->GetRange());
	}
	gui->Add(m_LutSlider);

	gui->Button(ID_SHOW_HISTOGRAM, "Show Histogram");

	wxString numStrs[] = { "Three","Four","Five","Six","Seven","Eight","Nine","Ten" };
	wxString posStrs[] = { "Top", "Bottom", "Left", "Right" };

	gui->Bool(ID_ENABLE_SCALAR_BAR, "Show scalar bar", &m_ShowScalarBar, 1);
	gui->Combo(ID_SCALAR_BAR_LAB_N, "Label Number",  &m_ScalarBarLabNum, 8,numStrs);
	gui->Combo(ID_SCALAR_BAR_POS, "Bar Position", &m_ScalarBarPos, 4, posStrs);

	EnableDisableGuiComponents();
}

//----------------------------------------------------------------------------
void albaPipeWithScalar::CreateProbeMapStack()
{
	vtkNEW(m_ProbeFilter);
	
	m_ProbeFilter->SetDistanceModeToScalar();
	m_ProbeFilter->SetSource(m_ProbeVolume->GetOutput()->GetVTKData());
	m_ProbeFilter->SetInputData(m_Mapper->GetInput());
	m_ProbeFilter->SetFilterModeToDensity();
	m_ProbeFilter->SetInputMatrix(m_Vme->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
	m_ProbeFilter->SetOutOfBoundsDensity(-9999);
	m_ProbeFilter->Update();

	m_Mapper->SetInputConnection(m_ProbeFilter->GetOutputPort());
	m_Mapper->SetScalarVisibility(true);
	m_MapsStackActive = true;


	UpdateActiveScalarsInVMEDataVectorItems();
}

//----------------------------------------------------------------------------
void albaPipeWithScalar::OnEvent(albaEventBase *alba_event)
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch (e->GetId())
		{
			case ID_SCALARS:
			{
				m_ActiveScalarType = (m_ScalarIndex < m_PointCellArraySeparation) ? POINT_TYPE : CELL_TYPE;
				UpdateActiveScalarsInVMEDataVectorItems();
				UpdateComonentsMangaement();
				UpdateScalarBarTitle();
				GetLogicManager()->CameraUpdate();
			}
			break;
			case ID_COMPONENTS:
			{
				UpdateVisualizationWithNewSelectedScalars();
				UpdateScalarBarTitle();
				GetLogicManager()->CameraUpdate();
			}
			break;
			case ID_LUT:
			{
				double sr[2];
				m_Table->GetTableRange(sr);
				SetScalarRange(sr);
			}
			break;
			case ID_RANGE_MODIFIED:
			{
				double sr[2];
				m_LutSlider->GetSubRange(sr);
				SetScalarRange(sr);
			}
			break;
			case ID_SCALAR_MAP_ACTIVE:
			{
				m_Mapper->SetScalarVisibility(m_ScalarMapActive);
				if (m_ScalarBarActor)
				{
					m_ScalarBarActor->SetTitle(m_ScalarsVTKName[m_ScalarIndex]);
					ShowScalarBarActor(m_ShowScalarBar);
				}

				UpdateActiveScalarsInVMEDataVectorItems();
				UpdateComonentsMangaement();
				UpdateScalarBarTitle();
				GetLogicManager()->CameraUpdate();
			}
			break;
			case ID_PROBE_MAPS:
			{
				SetProbeMapActive(m_ProbeMapActive);
			}
			break;
			case ID_SELECT_PROBE_VME:
			{
				albaString s(_("Choose Probe Map Volume"));
				albaEvent e(this, VME_CHOOSE, &s);
				e.SetPointer(&albaPipeWithScalar::VolumeAccept);
				albaEventMacro(e);

				albaVME *vme = e.GetVme();
				if (vme == NULL)
					return;

				SetProbeVolume(vme);
			}
			break;
			case ID_ENABLE_SCALAR_BAR:
			{
				ShowScalarBarActor(m_ShowScalarBar);
				EnableDisableGuiComponents();

				GetLogicManager()->CameraUpdate();
			}
			break;

			case ID_SCALAR_BAR_LAB_N:
			{
				SetScalarBarLabelsNum(m_ScalarBarLabNum + 3);
				GetLogicManager()->CameraUpdate();
			}
			break;

			case ID_SCALAR_BAR_POS:
			{
				SetScalarBarPos(m_ScalarBarPos);
				GetLogicManager()->CameraUpdate();
			}
			break;
			case ID_SHOW_HISTOGRAM:
				CreateHistogramDialog();
				break;
			default:
				albaEventMacro(*e);
				break;
		}
	}
  else if(alba_event->GetId() == VME_TIME_SET)
  {
    UpdateActiveScalarsInVMEDataVectorItems();
  }
}

//----------------------------------------------------------------------------
void albaPipeWithScalar::SetScalarBarLabelsNum(int num)
{
	m_ScalarBarLabNum = num - 3;
	if (m_ScalarBarActor)
		m_ScalarBarActor->SetNumberOfLabels(num);
}

//----------------------------------------------------------------------------
void albaPipeWithScalar::SetScalarRange(double * sr)
{
	m_Table->SetTableRange(sr);
	m_Mapper->SetScalarRange(sr);
	if (m_LutSlider)
		m_LutSlider->SetSubRange(sr);

	GetLogicManager()->CameraUpdate();
}

//----------------------------------------------------------------------------
void albaPipeWithScalar::DestroyProbeMapStack()
{
	//restore old mapper input
	m_Mapper->SetInputData(m_ProbeFilter->GetInput());

	m_Mapper->SetScalarVisibility(m_ScalarMapActive);
	if (m_ScalarBarActor)
	{
		m_ScalarBarActor->SetTitle(m_ScalarMapActive ? m_ScalarsVTKName[m_ScalarIndex] : "");
		ShowScalarBarActor(m_ShowScalarBar);
	}

	vtkDEL(m_ProbeFilter);
	m_MapsStackActive = false;
}

//----------------------------------------------------------------------------
void albaPipeWithScalar::CreateHistogramDialog()
{
	DeleteHistogramDialog();
	m_Dialog = new albaGUIDialog("Histogram", albaRESIZABLE | albaCLOSEWINDOW, this);
	albaGUI *gui = new albaGUI(this);
	m_Histogram = new albaGUIHistogramWidget(gui, -1, wxPoint(0, 0), wxSize(630, 660), wxTAB_TRAVERSAL, true);
	m_Histogram->SetListener(this);
	gui->Add(m_Histogram, 1);
	gui->AddGui(m_Histogram->GetGui());
	gui->FitGui();
	gui->Update();

	m_Dialog->Add(gui, 1);
	m_Dialog->SetMinSize(wxSize(630, 660));
	m_Dialog->SetSize(wxSize(630, 660));
	m_Dialog->Show();
	m_Dialog->Fit();
	m_Dialog->FitInside();

	UpdateVisualizationWithNewSelectedScalars();
}

wxString albaPipeWithScalar::GetScalarName(int id) const
{
	if (id < 0 || id >= m_NumberOfArrays)
		return "";
	else
		return m_ScalarsVTKName[id];
}

//----------------------------------------------------------------------------
void albaPipeWithScalar::DeleteHistogramDialog()
{
	cppDEL(m_Histogram);
	cppDEL(m_Dialog);
}

//----------------------------------------------------------------------------
void albaPipeWithScalar::EnableDisableGuiComponents()
{
	if (m_Gui)
	{
		bool scalarMangement = (m_ScalarMapActive && !m_ProbeMapActive) || (m_ProbeMapActive && m_ProbeVolume);
		m_Gui->Enable(ID_SCALAR_MAP_ACTIVE, m_NumberOfArrays > 0 && !m_ProbeMapActive);
		m_Gui->Enable(ID_SCALARS, m_ScalarMapActive && !m_ProbeMapActive);
		m_Gui->Enable(ID_COMPONENTS, m_ScalarMapActive && !m_ProbeMapActive && m_NumberOfComponents > 1);
		m_Gui->Enable(ID_SELECT_PROBE_VME, m_ProbeMapActive);
		m_Gui->Enable(ID_LUT, scalarMangement);
		m_LutSlider->Enable(scalarMangement);
		m_Gui->Enable(ID_ENABLE_SCALAR_BAR, scalarMangement);
		m_Gui->Enable(ID_SCALAR_BAR_LAB_N, scalarMangement && m_ShowScalarBar);
		m_Gui->Enable(ID_SCALAR_BAR_POS, scalarMangement && m_ShowScalarBar);
		m_Gui->Enable(ID_SHOW_HISTOGRAM, scalarMangement);
		m_Gui->Update();
	}
}

//----------------------------------------------------------------------------
void albaPipeWithScalar::UpdateActiveScalarsInVMEDataVectorItems()
{

	m_Vme->Update();
	
	if (m_OldMapsGenActive != m_ScalarMapActive || m_OldActiveScalarType != m_ActiveScalarType || m_OldScalarIndex != m_ScalarIndex)
		UpdateVisualizationWithNewSelectedScalars();

	m_OldMapsGenActive = m_ScalarMapActive;
	m_OldActiveScalarType = m_OldActiveScalarType;
	m_OldScalarIndex = m_ScalarIndex;

}
//----------------------------------------------------------------------------
void albaPipeWithScalar::UpdateVisualizationWithNewSelectedScalars()
{

	if (m_ScalarMapActive == false && m_MapsStackActive == false)
		return;

  vtkDataSet *data = m_Vme->GetOutput()->GetVTKData();
	vtkDataArray *scalars = NULL;

  double sr[2]={0,1};
	if (m_MapsStackActive || (m_ActiveScalarType == POINT_TYPE && m_PointCellArraySeparation > 0))
	{
		if (m_MapsStackActive)
		{
			m_ProbeVolume->GetOutput()->GetVTKData()->GetScalarRange(sr);
			scalars = m_Mapper->GetInput()->GetPointData()->GetScalars(DISTANCE_FILTER_SCALARS_NAME);
		}
		else
		{
			scalars = data->GetPointData()->GetScalars(m_ScalarsVTKName[m_ScalarIndex]);
			scalars->GetRange(sr, m_ComponentIndex);
		}
	}
	else if (m_ActiveScalarType == CELL_TYPE && (m_NumberOfArrays - m_PointCellArraySeparation > 0))
	{
		scalars = data->GetCellData()->GetScalars(m_ScalarsVTKName[m_ScalarIndex]);
		scalars->GetRange(sr, m_ComponentIndex);
	}

	if (!scalars)
		return;

		if (m_Histogram)
		m_Histogram->SetData(scalars,m_ComponentIndex);

  m_Table->SetTableRange(sr);
  m_Table->SetValueRange(sr);
  m_Table->SetHueRange(0.667, 0.0);

	if (m_LutSlider)
	{
		m_LutSlider->SetRange(sr);
		m_LutSlider->SetSubRange(sr);
	}
  m_ObjectMaterial->m_ColorLut->DeepCopy(m_Table);
  m_ObjectMaterial->UpdateFromLut();


  if(m_MapsStackActive || m_ActiveScalarType == POINT_TYPE)
    m_Mapper->SetScalarModeToUsePointFieldData();
  else if(m_ActiveScalarType == CELL_TYPE)
    m_Mapper->SetScalarModeToUseCellFieldData();

	m_Mapper->ColorByArrayComponent(scalars->GetName(), m_ComponentIndex);

  m_Mapper->SetLookupTable(m_Table);
  m_Mapper->SetScalarRange(sr);
  m_Mapper->Update();

  m_Actor->Modified();
  if(m_ScalarBarActor)
	m_ScalarBarActor->Modified();
}

//----------------------------------------------------------------------------
void albaPipeWithScalar::UpdateComonentsMangaement()
{
	if (m_Gui)
	{
		vtkDataArray *scalars=GetCurrentScalars();

		if(scalars)
			m_NumberOfComponents = scalars->GetNumberOfComponents();
		else
			m_NumberOfComponents = 0;

		m_ComponentIndex = 0;

		m_ComponentsComboBox->Freeze();
		m_ComponentsComboBox->Clear();

		for (int i = 0; i < m_NumberOfComponents; i++)
			m_ComponentsComboBox->Append(GetComponentName(scalars, i).GetCStr());
		
		// update items
		m_ComponentsComboBox->Thaw();

		EnableDisableGuiComponents();
	}
}

//----------------------------------------------------------------------------
vtkDataArray *albaPipeWithScalar::GetCurrentScalars()
{
	vtkDataSet *data = m_Vme->GetOutput()->GetVTKData();

	if (m_MapsStackActive || (m_ActiveScalarType == POINT_TYPE && m_PointCellArraySeparation > 0))
	{
		if (m_MapsStackActive)
			return m_Mapper->GetInput()->GetPointData()->GetScalars(DISTANCE_FILTER_SCALARS_NAME);
		else
			return data->GetPointData()->GetScalars(m_ScalarsVTKName[m_ScalarIndex]);
	}
	else if (m_ActiveScalarType == CELL_TYPE && (m_NumberOfArrays - m_PointCellArraySeparation > 0))
		return data->GetCellData()->GetScalars(m_ScalarsVTKName[m_ScalarIndex]);

	return NULL;
}

//----------------------------------------------------------------------------
albaString albaPipeWithScalar::GetComponentName(vtkDataArray *scalars, int compNum)
{
	albaString compStr;
	compStr.Printf("Comp %d", compNum + 1);
	return compStr;
}

//----------------------------------------------------------------------------
void albaPipeWithScalar::UpdateScalarBarTitle()
{
	if (m_ScalarBarActor)
	{
		vtkDataArray *scalars = GetCurrentScalars();

		if (!scalars)
			return;

		if (scalars->GetNumberOfComponents() <= 1)
			m_ScalarBarActor->SetTitle(m_ScalarsVTKName[m_ScalarIndex]);
		else
		{
			albaString title;
			title.Printf("%s\n%s", m_ScalarsVTKName[m_ScalarIndex].ToAscii(), GetComponentName(scalars,m_ComponentIndex).GetCStr());
			m_ScalarBarActor->SetTitle(title);
		}
	}
}

//----------------------------------------------------------------------------
void albaPipeWithScalar::CreateFieldDataControlArrays()
{
  //String array allocation
	vtkPointData * pointData = m_Vme->GetOutput()->GetVTKData()->GetPointData();
	vtkCellData * cellData = m_Vme->GetOutput()->GetVTKData()->GetCellData();

  int numPointScalars = pointData->GetNumberOfArrays();
	int numCellScalars = cellData->GetNumberOfArrays();

  wxString *tempScalarsPointsName=new wxString[numPointScalars + numCellScalars];
  int count=0;

  int pointArrayNumber;
  for(pointArrayNumber = 0; pointArrayNumber<numPointScalars; pointArrayNumber++)
  {
		const char *arrayName=pointData->GetArrayName(pointArrayNumber);
    if(arrayName && strcmp(arrayName,"")!=0 && strcmp(arrayName, "normals") != 0 && strcmp(arrayName, "scalars") != 0)
    {
      count++;
      tempScalarsPointsName[count-1]=pointData->GetArrayName(pointArrayNumber);
    }
  }
  for(int cellArrayNumber=0;cellArrayNumber<numCellScalars;cellArrayNumber++)
  {
		const char *arrayName=cellData->GetArrayName(cellArrayNumber);
    if(arrayName && strcmp(arrayName,"")!=0)
    {
      count++;
      tempScalarsPointsName[count-1]=cellData->GetArrayName(cellArrayNumber);
    }
  }
	m_ScalarsInComboBoxNames = new wxString[count];
  m_ScalarsVTKName = new wxString[count];

  for(int j=0;j<count;j++)
  {
    m_ScalarsVTKName[j]=tempScalarsPointsName[j];
    if(j<pointArrayNumber)
      m_ScalarsInComboBoxNames[j]="[POINT] " + tempScalarsPointsName[j];
    else
      m_ScalarsInComboBoxNames[j]="[CELL] " + tempScalarsPointsName[j];
  }

  m_PointCellArraySeparation = numPointScalars;
	m_NumberOfArrays = numPointScalars + numCellScalars;


  delete []tempScalarsPointsName;
}

//----------------------------------------------------------------------------
void albaPipeWithScalar::SetLookupTable(vtkLookupTable *table)
{
	if(m_Table==NULL || table==NULL ) return;

	int n = table->GetNumberOfTableValues();
	if(n>256) n=256;
	m_Table->SetNumberOfTableValues(n);
	m_Table->SetRange(table->GetRange());
	for(int i=0; i<n; i++)
	{
		double *rgba;
		rgba = table->GetTableValue(i);
		m_Table->SetTableValue(i,rgba[0],rgba[1],rgba[2],rgba[3]);
	}
}

//----------------------------------------------------------------------------
void albaPipeWithScalar::SetLookupTableToMapper()
{
	m_Mapper->SetLookupTable(m_Table);
}

//----------------------------------------------------------------------------
void albaPipeWithScalar::CreateScalarBarActor()
{
	////scalar field map
	m_ScalarBarActor = vtkScalarBarActor::New();
	m_ScalarBarActor->SetLookupTable(m_Table);
	((vtkActor2D*)m_ScalarBarActor)->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
	((vtkActor2D*)m_ScalarBarActor)->GetPositionCoordinate()->SetValue(0.1, 0.01);
	m_ScalarBarActor->SetOrientationToHorizontal();
	m_ScalarBarActor->SetWidth(0.8);
	m_ScalarBarActor->SetHeight(0.08);
	m_ScalarBarActor->SetLabelFormat("%6.3g");
	m_ScalarBarActor->SetNumberOfLabels(m_ScalarBarLabNum + 3);
	m_ScalarBarActor->GetLabelTextProperty()->SetColor(0.8, 0.8, 0.8);
	m_ScalarBarActor->SetPickable(0);
	m_ScalarBarActor->SetVisibility(false);

	SetScalarBarPos(m_ScalarBarPos);
	
	if(m_RenFront)
		m_RenFront->AddActor2D(m_ScalarBarActor);
}

//----------------------------------------------------------------------------
void albaPipeWithScalar::DeleteScalarBarActor()
{
	if(m_RenFront)
		m_RenFront->RemoveActor2D(m_ScalarBarActor);
	vtkDEL(m_ScalarBarActor);
}

//----------------------------------------------------------------------------
void albaPipeWithScalar::SetScalarBarPos(int pos)
{
	m_ScalarBarPos = pos;
	if (m_ScalarBarActor)
	{
		switch (pos)
		{
			case SB_ON_BOTTOM:
				((vtkActor2D*)m_ScalarBarActor)->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
				((vtkActor2D*)m_ScalarBarActor)->GetPositionCoordinate()->SetValue(0.1, 0.01);
				m_ScalarBarActor->SetOrientationToHorizontal();
				m_ScalarBarActor->SetWidth(0.8);
				m_ScalarBarActor->SetHeight(0.08);
			break;
			case SB_ON_TOP:
				((vtkActor2D*)m_ScalarBarActor)->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
				((vtkActor2D*)m_ScalarBarActor)->GetPositionCoordinate()->SetValue(0.1, 0.91);
				m_ScalarBarActor->SetOrientationToHorizontal();
				m_ScalarBarActor->SetWidth(0.8);
				m_ScalarBarActor->SetHeight(0.08);
				break;
			case SB_ON_LEFT:
				m_ScalarBarActor->SetOrientationToVertical();
				m_ScalarBarActor->SetWidth(0.08);
				m_ScalarBarActor->SetHeight(0.9);
				((vtkActor2D*)m_ScalarBarActor)->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
				((vtkActor2D*)m_ScalarBarActor)->GetPositionCoordinate()->SetValue(0.01, 0.05);
				break;
			case SB_ON_RIGHT:
				m_ScalarBarActor->SetOrientationToVertical();
				m_ScalarBarActor->SetWidth(0.08);
				m_ScalarBarActor->SetHeight(0.9);
				((vtkActor2D*)m_ScalarBarActor)->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
				((vtkActor2D*)m_ScalarBarActor)->GetPositionCoordinate()->SetValue(0.91, 0.05);
				break;
			default:
				break;
		}
	}
}

//----------------------------------------------------------------------------
void albaPipeWithScalar::ShowScalarBarActor(bool show /*= true*/)
{
	m_ShowScalarBar = show;
	m_ScalarBarActor->SetVisibility(m_Selected && (m_ScalarMapActive || m_MapsStackActive) && show);
}

//----------------------------------------------------------------------------
void albaPipeWithScalar::SetProbeVolume(albaVME *vol)
{
	m_ProbeVolume = vol;

	m_ProbeVolName = m_ProbeVolume ? m_ProbeVolume->GetName() : "";

	//need to create the probe stack
	if (vol && !m_MapsStackActive)
	{
		CreateProbeMapStack();
	}
	else
	{
		if(m_ProbeFilter)
			m_ProbeFilter->SetSource(m_ProbeVolume ? m_ProbeVolume->GetOutput()->GetVTKData() :NULL);
		UpdateActiveScalarsInVMEDataVectorItems();
	}

	m_Mapper->SetScalarVisibility(m_MapsStackActive);
	EnableDisableGuiComponents();
	UpdateActiveScalarsInVMEDataVectorItems();
	GetLogicManager()->CameraUpdate();
}

//----------------------------------------------------------------------------
bool albaPipeWithScalar::VolumeAccept(albaVME *node)
{
	return node->IsA("albaVMEVolumeGray");
}

//----------------------------------------------------------------------------
void albaPipeWithScalar::SetProbeMapActive(int val)
{
	m_ProbeMapActive = val;

	//need to disable probe filter
	if (m_MapsStackActive && !m_ProbeMapActive)
		DestroyProbeMapStack();
	else if (m_ProbeMapActive && m_ProbeVolume)
		CreateProbeMapStack();
	else if(m_ProbeMapActive && !m_MapsStackActive && m_ScalarMapActive)
		m_Mapper->SetScalarVisibility(false);
	else
		m_Mapper->SetScalarVisibility(m_ScalarMapActive);

	EnableDisableGuiComponents();
	if(m_Mapper->GetScalarVisibility())
		UpdateActiveScalarsInVMEDataVectorItems();
	GetLogicManager()->CameraUpdate();
 
}

