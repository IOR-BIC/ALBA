/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEGizmo
 Authors: Marco Petrone, Paolo Quadrani
 
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


#include "albaVMEGizmo.h"
#include "albaTransform.h"
#include "albaVMEOutputSurface.h"
#include "albaDataPipeCustom.h"
#include "albaTagArray.h"
#include "mmaMaterial.h"

#include "vtkPolyData.h"
#include "vtkALBADataPipe.h"
#include "vtkAlgorithmOutput.h"

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMEGizmo)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
albaVMEGizmo::albaVMEGizmo()
//-------------------------------------------------------------------------
{
  m_Mediator = NULL;
  m_GizmoData = NULL;
	m_InputConnection = NULL;
  albaNEW(m_Transform);
  albaVMEOutputSurface *output=albaVMEOutputSurface::New(); // an output with no data
  output->SetTransform(m_Transform); // force my transform in the output
  SetOutput(output);

  // attach a datapipe which creates a bridge between VTK and ALBA
  albaDataPipeCustom *dpipe=albaDataPipeCustom::New();
  SetDataPipe(dpipe);
  
  // modified by Stefano 14-3-2005: gizmos set to not visible in the tree and node chooser
  GetTagArray()->SetTag(albaTagItem("VISIBLE_IN_THE_TREE", 0.0));

  SetVisibleToTraverse(false);

  m_TextValue = "";
  m_TextVisibility = false;

  m_TextColour[0] = m_TextColour[1] = m_TextColour[2] = 1.0;
}

//-------------------------------------------------------------------------
albaVMEGizmo::~albaVMEGizmo()
//-------------------------------------------------------------------------
{
  albaDEL(m_Transform);
  vtkDEL(m_GizmoData);
}
//-------------------------------------------------------------------------
int albaVMEGizmo::DeepCopy(albaVME *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a)==ALBA_OK)
  {
    albaVMEGizmo *vme_gizmo=albaVMEGizmo::SafeDownCast(a);
    m_Transform->SetMatrix(vme_gizmo->m_Transform->GetMatrix());
    SetData(vme_gizmo->GetData());
    return ALBA_OK;
  }  
  return ALBA_ERROR;
}
//-------------------------------------------------------------------------
bool albaVMEGizmo::Equals(albaVME *vme)
//-------------------------------------------------------------------------
{
  if (Superclass::Equals(vme))
  {
    // test only matrices
    return (m_Transform->GetMatrix() == ((albaVMEGizmo *)vme)->m_Transform->GetMatrix());
  }
  return false;
}
//-------------------------------------------------------------------------
mmaMaterial *albaVMEGizmo::GetMaterial()
//-------------------------------------------------------------------------
{
  mmaMaterial *material = (mmaMaterial *)GetAttribute("MaterialAttributes");
  if (material == NULL)
  {
    material = mmaMaterial::New();
    SetAttribute("MaterialAttributes", material);
  }
  return material;
}

//-------------------------------------------------------------------------
void albaVMEGizmo::SetData(vtkPolyData *data)
//-------------------------------------------------------------------------
{
  assert(data);  // just check if data is set to NULL...
	if (data != m_GizmoData || m_InputConnection != NULL)
  {
		vtkDEL(m_GizmoData);
    m_GizmoData = data;
    m_GizmoData->Register(NULL);
		m_InputConnection = NULL;
    
    // set data as input to VTK 
    albaDataPipeCustom *dpipe=albaDataPipeCustom::SafeDownCast(GetDataPipe());
    dpipe->GetVTKDataPipe()->SetNthInput(0,data);
    
    Modified();
  }
}

//----------------------------------------------------------------------------
void albaVMEGizmo::SetDataConnection(vtkAlgorithmOutput *input)
{
	assert(input);  // just check if data is set to NULL...
	if (input != m_InputConnection || m_GizmoData != NULL)
	{
		vtkDEL(m_GizmoData);

    m_InputConnection = input;

		// set data as input to VTK 
		albaDataPipeCustom *dpipe = albaDataPipeCustom::SafeDownCast(GetDataPipe());
		dpipe->SetInputConnection(0, input);

		Modified();
	}
}

vtkPolyData* albaVMEGizmo::GetData()
{
  if (m_GizmoData)
    return m_GizmoData;
  else if (m_InputConnection)
  {
    // Get the producer (the algorithm that generates the data)
    vtkAlgorithm* producer = m_InputConnection->GetProducer();
    producer->Update();

    // Retrieve the data object from the producer using the port index
    vtkDataObject* data = producer->GetOutputDataObject(m_InputConnection->GetIndex());

    // Cast to the expected data type (vtkPolyData in this case)
    return vtkPolyData::SafeDownCast(data);
  }
  else
    return NULL;
}

//-------------------------------------------------------------------------
albaVMEOutputSurface *albaVMEGizmo::GetSurfaceOutput()
//-------------------------------------------------------------------------
{
  return (albaVMEOutputSurface *)GetOutput();
}

//-------------------------------------------------------------------------
void albaVMEGizmo::SetMatrix(const albaMatrix &mat)
//-------------------------------------------------------------------------
{
  m_Transform->SetMatrix(mat);
  Modified();
}

//-------------------------------------------------------------------------
bool albaVMEGizmo::IsAnimated()
//-------------------------------------------------------------------------
{
  return false;
}

//-------------------------------------------------------------------------
void albaVMEGizmo::GetLocalTimeStamps(std::vector<albaTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  kframes.clear(); // no timestamps
}
//-------------------------------------------------------------------------
void albaVMEGizmo::SetTextValue(const char* text)
//-------------------------------------------------------------------------
{
  m_TextValue = albaString(text);
  InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);
}
//-------------------------------------------------------------------------
const char * albaVMEGizmo::GetTextValue()
//-------------------------------------------------------------------------
{
  return m_TextValue;
}
//-------------------------------------------------------------------------
void albaVMEGizmo::SetTextPosition(double *position3D)
//-------------------------------------------------------------------------
{
  m_TextPosition[0] = position3D[0];
  m_TextPosition[1] = position3D[1];
  m_TextPosition[2] = position3D[2];
  InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);
}
//-------------------------------------------------------------------------
void albaVMEGizmo::SetTextVisibility(int visibility)
//-------------------------------------------------------------------------
{ 
  m_TextVisibility = visibility;
  InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);
}
//-------------------------------------------------------------------------
double *albaVMEGizmo::GetTextPosition()
//-------------------------------------------------------------------------
{
  return m_TextPosition;
}
//-------------------------------------------------------------------------
double *albaVMEGizmo::GetTextColour()
//-------------------------------------------------------------------------
{
  return m_TextColour;
}

//-------------------------------------------------------------------------
void albaVMEGizmo::SetTextColour(double colour[3])
//-------------------------------------------------------------------------
{
  m_TextColour[0] = colour[0];
  m_TextColour[1] = colour[1];
  m_TextColour[2] = colour[2];
  InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);
}