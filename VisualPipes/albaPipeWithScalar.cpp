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

#include <vector>
#include "vtkALBAPolyDataNormals.h"


//----------------------------------------------------------------------------
albaPipeWithScalar::albaPipeWithScalar()
:albaPipe()
{

	m_ScalarIndex = 0;
	m_NumberOfArrays = 0;
	m_Table = NULL;

	m_ActiveScalarType = POINT_TYPE;
	m_PointCellArraySeparation = 0;

	m_ScalarsInComboBoxNames = NULL;
	m_ScalarsVTKName = NULL;

	m_ScalarMapActive = 0;
}
//----------------------------------------------------------------------------
albaPipeWithScalar::~albaPipeWithScalar()
{
	vtkDEL(m_Table);

	delete[] m_ScalarsInComboBoxNames;
	delete[] m_ScalarsVTKName;
}

//----------------------------------------------------------------------------
void albaPipeWithScalar::ManageScalarOnExecutePipe(vtkDataSet * dataSet)
{
	CreateFieldDataControlArrays();

	m_ObjectMaterial = (mmaMaterial *)m_Vme->GetAttribute("MaterialAttributes");

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
	lutPreset(4, m_Table);
	m_Table->Build();
	m_Table->DeepCopy(m_ObjectMaterial->m_ColorLut);

	m_Table->SetValueRange(sr);
	m_Table->SetHueRange(0.667, 0.0);
	m_Table->SetTableRange(sr);
	m_Table->Build();
	

	m_ObjectMaterial->m_ColorLut->DeepCopy(m_Table);
	m_ObjectMaterial->m_ColorLut->Build();
	m_Mapper->SetLookupTable(m_Table);
	m_Mapper->SetScalarRange(sr);

	if (m_ActiveScalarType == POINT_TYPE)
		m_Mapper->SetScalarModeToUsePointData();
	if (m_ActiveScalarType == CELL_TYPE)
		m_Mapper->SetScalarModeToUseCellData();

	if (m_ScalarMapActive)
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
	gui->Combo(ID_SCALARS,"",&m_ScalarIndex,m_NumberOfArrays,m_ScalarsInComboBoxNames);	
  m_LutSwatch=gui->Lut(ID_LUT,"Lut",m_Table);

  gui->Enable(ID_SCALARS, m_ScalarMapActive != 0);
  gui->Enable(ID_LUT, m_ScalarMapActive != 0);
	gui->Enable(ID_SCALAR_MAP_ACTIVE,m_NumberOfArrays>0);
}
//----------------------------------------------------------------------------
void albaPipeWithScalar::OnEvent(albaEventBase *alba_event)
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch(e->GetId()) 
		{
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
          UpdateActiveScalarsInVMEDataVectorItems();
					GetLogicManager()->CameraUpdate();
        }
        break;
      case ID_LUT:
        {
          double sr[2];
          m_Table->GetTableRange(sr);
          m_Mapper->SetScalarRange(sr);
					GetLogicManager()->CameraUpdate();
        }
        break;
      case ID_SCALAR_MAP_ACTIVE:
        {
					m_Mapper->SetScalarVisibility(m_ScalarMapActive);
				
					if(m_Gui)
					{
						m_Gui->Enable(ID_SCALAR_MAP_ACTIVE,m_NumberOfArrays>0);
						m_Gui->Enable(ID_SCALARS, m_ScalarMapActive != 0);
						m_Gui->Enable(ID_LUT, m_ScalarMapActive != 0);
						m_Gui->Update();
					}
	
					UpdateActiveScalarsInVMEDataVectorItems();
        }
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
void albaPipeWithScalar::UpdateActiveScalarsInVMEDataVectorItems()
{
  
  m_Vme->GetOutput()->GetVTKData()->Update();
  m_Vme->Update();
  
  if(m_ActiveScalarType == POINT_TYPE && m_PointCellArraySeparation > 0)
  {
    m_Vme->GetOutput()->GetVTKData()->GetPointData()->SetActiveScalars(m_ScalarsVTKName[m_ScalarIndex].c_str());
    m_Vme->GetOutput()->GetVTKData()->GetPointData()->GetScalars()->Modified();
  }
  else if(m_ActiveScalarType == CELL_TYPE && (m_NumberOfArrays-m_PointCellArraySeparation >0) )
  {
    m_Vme->GetOutput()->GetVTKData()->GetCellData()->SetActiveScalars(m_ScalarsVTKName[m_ScalarIndex].c_str());
    m_Vme->GetOutput()->GetVTKData()->GetCellData()->GetScalars()->Modified();
  }
  m_Vme->Modified();
  m_Vme->GetOutput()->GetVTKData()->Update();
  m_Vme->Update();
  
	if(albaVMEGeneric::SafeDownCast(m_Vme) && ((albaVMEGeneric *)m_Vme)->GetDataVector())
	{
		for (albaDataVector::Iterator it = ((albaVMEGeneric *)m_Vme)->GetDataVector()->Begin(); it != ((albaVMEGeneric *)m_Vme)->GetDataVector()->End(); it++)
		{
			albaVMEItemVTK *item = albaVMEItemVTK::SafeDownCast(it->second);
			assert(item);

			vtkDataSet *outputVTK = vtkDataSet::SafeDownCast(item->GetData());
			if(outputVTK)
			{
				if(m_ActiveScalarType == POINT_TYPE && m_PointCellArraySeparation > 0)
				{
					wxString scalarsToActivate = m_ScalarsVTKName[m_ScalarIndex].c_str();
					vtkDataArray *scalarsArray = outputVTK->GetPointData()->GetArray(scalarsToActivate);

					if (scalarsArray == NULL)
					{
						std::ostringstream stringStream;
						stringStream << scalarsToActivate.c_str() << " POINT_DATA array does not exist for timestamp " \
							<< item->GetTimeStamp() << " . Skipping SetActiveScalars for this timestamp" << std::endl;
						albaLogMessage(stringStream.str().c_str());
						continue;
					}

					outputVTK->GetPointData()->SetActiveScalars(m_ScalarsVTKName[m_ScalarIndex].c_str());
					outputVTK->GetPointData()->GetScalars()->Modified();
				}
				else if(m_ActiveScalarType == CELL_TYPE && (m_NumberOfArrays - m_PointCellArraySeparation >0))
				{
					wxString scalarsToActivate = m_ScalarsVTKName[m_ScalarIndex].c_str();
					vtkDataArray *scalarsArray = outputVTK->GetCellData()->GetArray(scalarsToActivate);
        
					if (scalarsArray == NULL)
					{
						std::ostringstream stringStream;
						stringStream << scalarsToActivate.c_str() << "  CELL_DATA array does not exist for timestamp " \
						<< item->GetTimeStamp() << " . Skipping SetActiveScalars for this timestamp" << std::endl;
						albaLogMessage(stringStream.str().c_str());
						continue;
					}
        

					outputVTK->GetCellData()->SetActiveScalars(scalarsToActivate.c_str());
					outputVTK->GetCellData()->GetScalars()->Modified();
				}
				outputVTK->Modified();
				outputVTK->Update();
      
			}
		}
	}
  m_Vme->Modified();
  m_Vme->Update();
  
  UpdateVisualizationWithNewSelectedScalars();  
}
//----------------------------------------------------------------------------
void albaPipeWithScalar::UpdateVisualizationWithNewSelectedScalars()
{
  vtkDataSet *data = vtkDataSet::SafeDownCast(m_Vme->GetOutput()->GetVTKData());
  data->Update();
  double sr[2]={0,1};
  if(m_ActiveScalarType == POINT_TYPE && m_PointCellArraySeparation > 0)
    data->GetPointData()->GetScalars()->GetRange(sr);
  else if(m_ActiveScalarType == CELL_TYPE && (m_NumberOfArrays - m_PointCellArraySeparation >0))
    data->GetCellData()->GetScalars()->GetRange(sr);

  m_Table->SetTableRange(sr);
  m_Table->SetValueRange(sr);
  m_Table->SetHueRange(0.667, 0.0);
  
  m_ObjectMaterial->m_ColorLut->DeepCopy(m_Table);
  m_ObjectMaterial->UpdateFromLut();


  if(m_ActiveScalarType == POINT_TYPE)
    m_Mapper->SetScalarModeToUsePointData();
  if(m_ActiveScalarType == CELL_TYPE)
    m_Mapper->SetScalarModeToUseCellData();

  m_Mapper->SetLookupTable(m_Table);
  m_Mapper->SetScalarRange(sr);
  m_Mapper->Update();

  m_Actor->Modified();

  UpdateProperty();
	GetLogicManager()->CameraUpdate();
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

