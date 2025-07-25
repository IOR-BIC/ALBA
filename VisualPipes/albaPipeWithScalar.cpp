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
	m_Table = NULL;

	m_ActiveScalarType = POINT_TYPE;
	m_PointCellArraySeparation = 0;

	m_ScalarsInComboBoxNames = NULL;
	m_ScalarsVTKName = NULL;
	m_LutSwatch = NULL;
	m_LutSlider = NULL;
	m_ScalarComboBox = NULL;
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

	m_NumberOfArrays = m_PointCellArraySeparation + dataSet->GetCellData()->GetNumberOfArrays();

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
	m_ProbeFilter->SetInput(m_Mapper->GetInput());
	m_ProbeFilter->SetFilterModeToDensity();
	m_ProbeFilter->SetInputMatrix(m_Vme->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
	m_ProbeFilter->SetOutOfBoundsDensity(-9999);
	m_ProbeFilter->Update();

	m_Mapper->SetInput(m_ProbeFilter->GetOutput());
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

				if (m_ScalarBarActor)
					m_ScalarBarActor->SetTitle(m_ScalarsVTKName[m_ScalarIndex]);

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

				EnableDisableGuiComponents();
				UpdateActiveScalarsInVMEDataVectorItems();
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
    UpdateProperty();
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
	m_Mapper->SetInput(m_ProbeFilter->GetInput());

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
	
	
	if (m_MapsStackActive && !m_OldMapsGenActive)
	{
		//run only on first activation
		vtkDataSet * vtkData = m_ProbeFilter->GetOutput();
		vtkData->Update();
		vtkData->GetPointData()->SetActiveScalars(DISTANCE_FILTER_SCALARS_NAME);
		vtkData->GetPointData()->GetScalars()->Modified();
		vtkData->GetCellData()->SetActiveScalars("");
	}
	else if (!m_MapsStackActive && (m_OldActiveScalarType != m_OldActiveScalarType ||	m_OldScalarIndex != m_ScalarIndex))
	{
		wxString scalarsToActivate = m_ScalarsVTKName[m_ScalarIndex].ToAscii();

		//run only when maps generation is off and scalartype or scalar index is changed 
		if (albaVMEGeneric::SafeDownCast(m_Vme) && ((albaVMEGeneric *)m_Vme)->GetDataVector())
		{
			for (albaDataVector::Iterator it = ((albaVMEGeneric *)m_Vme)->GetDataVector()->Begin(); it != ((albaVMEGeneric *)m_Vme)->GetDataVector()->End(); it++)
			{
				albaVMEItemVTK *item = albaVMEItemVTK::SafeDownCast(it->second);
				assert(item);

				vtkDataSet *outputVTK = vtkDataSet::SafeDownCast(item->GetData());
				if (outputVTK)
				{
					if (m_ActiveScalarType == POINT_TYPE && m_PointCellArraySeparation > 0)
					{
						vtkPointData * pointData = outputVTK->GetPointData();
						vtkDataArray *scalarsArray = pointData->GetArray(scalarsToActivate);

						if (scalarsArray == NULL)
						{
							std::ostringstream stringStream;
							stringStream << scalarsToActivate.ToAscii() << " POINT_DATA array does not exist for timestamp " \
								<< item->GetTimeStamp() << " . Skipping SetActiveScalars for this timestamp" << std::endl;
							albaLogMessage(stringStream.str().c_str());
							continue;
						}

						if (pointData->GetScalars() == NULL || scalarsToActivate != pointData->GetScalars()->GetName() || m_OldActiveScalarType != m_ActiveScalarType)
						{
							pointData->SetActiveScalars(scalarsToActivate.ToAscii());
							pointData->GetScalars()->Modified();
							outputVTK->GetCellData()->SetActiveScalars("");
							outputVTK->Modified();
							outputVTK->Update();
						}
					}
					else if (m_ActiveScalarType == CELL_TYPE && (m_NumberOfArrays - m_PointCellArraySeparation > 0))
					{
						
						vtkCellData * cellData = outputVTK->GetCellData();
						vtkDataArray *scalarsArray = cellData->GetArray(scalarsToActivate);

						if (scalarsArray == NULL)
						{
							std::ostringstream stringStream;
							stringStream << scalarsToActivate.ToAscii() << "  CELL_DATA array does not exist for timestamp " \
								<< item->GetTimeStamp() << " . Skipping SetActiveScalars for this timestamp" << std::endl;
							albaLogMessage(stringStream.str().c_str());
							continue;
						}

						if (cellData->GetScalars() == NULL || scalarsToActivate != cellData->GetScalars()->GetName() || m_OldActiveScalarType != m_ActiveScalarType)
						{
							cellData->SetActiveScalars(scalarsToActivate.ToAscii());
							cellData->GetScalars()->Modified();
							outputVTK->GetPointData()->SetActiveScalars("");
							outputVTK->Modified();
							outputVTK->Update();
						}
					}


				}
			}
		}
		else
		{
			vtkDataSet * vtkData = m_Vme->GetOutput()->GetVTKData();
			vtkData->Update();
			if (m_ActiveScalarType == POINT_TYPE && m_PointCellArraySeparation > 0)
			{
				vtkPointData * pointData = vtkData->GetPointData();
				if (pointData->GetScalars() == NULL || scalarsToActivate != pointData->GetScalars()->GetName())
				{
					pointData->SetActiveScalars(scalarsToActivate.ToAscii());
					pointData->GetScalars()->Modified();
					vtkData->GetCellData()->SetActiveScalars("");
				}
			}
			else if (m_ActiveScalarType == CELL_TYPE && (m_NumberOfArrays - m_PointCellArraySeparation > 0))
			{
				vtkCellData * cellData = vtkData->GetCellData();
				if (cellData->GetScalars() == NULL || scalarsToActivate != cellData->GetScalars()->GetName())
				{
					cellData->SetActiveScalars(scalarsToActivate.ToAscii());
					cellData->GetScalars()->Modified();
					vtkData->GetPointData()->SetActiveScalars("");
				}
			}
		}
	}

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
  data->Update();

  double sr[2]={0,1};
	if (m_MapsStackActive || (m_ActiveScalarType == POINT_TYPE && m_PointCellArraySeparation > 0))
	{
		if (m_MapsStackActive)
		{
			m_ProbeVolume->GetOutput()->GetVTKData()->GetScalarRange(sr);
			if (m_Histogram)
				m_Histogram->SetData(m_Mapper->GetInput()->GetPointData()->GetScalars());
		}
		else
		{
			data->GetPointData()->GetScalars()->GetRange(sr);
			if (m_Histogram)
				m_Histogram->SetData(data->GetPointData()->GetScalars());
		}
		
	}
	else if (m_ActiveScalarType == CELL_TYPE && (m_NumberOfArrays - m_PointCellArraySeparation > 0))
	{
		vtkDataArray* scalars = data->GetCellData()->GetScalars();
		scalars->GetRange(sr);
		if (m_Histogram)
			m_Histogram->SetData(scalars);
	}

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
    m_Mapper->SetScalarModeToUsePointData();
  else if(m_ActiveScalarType == CELL_TYPE)
    m_Mapper->SetScalarModeToUseCellData();

  m_Mapper->SetLookupTable(m_Table);
  m_Mapper->SetScalarRange(sr);
  m_Mapper->Update();

  m_Actor->Modified();
  if(m_ScalarBarActor)
	m_ScalarBarActor->Modified();

  UpdateProperty();
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

  m_PointCellArraySeparation = pointArrayNumber;

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
void albaPipeWithScalar::UpdateProperty(bool fromTag)
{
	if (m_MapsStackActive)
	{
		m_ProbeFilter->GetOutput()->GetPointData()->GetScalars()->Modified();
	}
	else if (m_ScalarMapActive)
	{
		if (m_ActiveScalarType == POINT_TYPE)
			m_Vme->GetOutput()->GetVTKData()->GetPointData()->GetScalars()->Modified();
		else if (m_ActiveScalarType == CELL_TYPE)
			m_Vme->GetOutput()->GetVTKData()->GetCellData()->GetScalars()->Modified();
	}
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
	

	m_RenFront->AddActor2D(m_ScalarBarActor);
}

//----------------------------------------------------------------------------
void albaPipeWithScalar::DeleteScalarBarActor()
{
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

