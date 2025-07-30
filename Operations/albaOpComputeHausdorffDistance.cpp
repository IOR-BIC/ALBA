/*=========================================================================
Program:   LHP
Module:    $RCSfile: albaOpComputeHausdorffDistance.cpp,v $
Language:  C++
Date:      $Date: 2012-03-20 15:32:41 $
Version:   $Revision: 1.1.2.4 $
Authors:   Eleonora Mambrini, Gianluigi Crimi
==========================================================================
Copyright (c) 2007
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)
=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the alba must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaOpComputeHausdorffDistance.h"

#include "albaGUIBusyInfo.h"

#include "albaEvent.h"
#include "albaGUI.h"
#include "albaGUIDialog.h"
#include "albaGUILutPreset.h"
#include "albaOpImporterSTL.h"
#include "albaVME.h"
#include "albaString.h"
#include "albaVMEMesh.h"
#include "albaVMESurface.h"

#include "mmaMaterial.h"

#include "vtkALBASmartPointer.h"
#include "vtkPolyData.h"
#include "vtkUnstructuredGrid.h"
#include "albaComputeHausdorffDistance.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTransform.h"

//-------------------------------------------------------------------------
// Widget enumeration:
//-------------------------------------------------------------------------
enum OP_COMPUTE_HAUSDORFF_DISTANCE_IDs
{
  ID_FIRST = MINID,
  ID_VME_OR_STL1,
  ID_VME_OR_STL2,
  ID_SELECT_SURFACE1,
  ID_SELECT_SURFACE2,
  ID_STL1,
  ID_STL2,
  ID_OUTPUT_DIR,
  ID_OP_OK,
  ID_OP_CANCEL,
};

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpComputeHausdorffDistance);

//----------------------------------------------------------------------------
albaOpComputeHausdorffDistance::albaOpComputeHausdorffDistance(const wxString &label) :albaOp(label)
{
  m_OpType	= OPTYPE_OP;
  m_Canundo	= true;

  m_FilenameSTL1 = new albaString("");
  m_FilenameSTL2 = new albaString("");
  m_OutputDir = new albaString("");

  m_VMEName1 = new albaString("NONE");
  m_VMEName2 = new albaString("NONE");

  m_SurfaceOutput = NULL;
  m_SurfaceInput1 = m_SurfaceInput2 = NULL;

  m_VmeOrSTL1 = m_VmeOrSTL2= 0;

  m_STLImporter = NULL;
}
//----------------------------------------------------------------------------
albaOpComputeHausdorffDistance::~albaOpComputeHausdorffDistance()
{
  albaDEL(m_SurfaceOutput);
  if(m_STLImporter)
    albaDEL(m_STLImporter);
  for(int i=0;i<m_ImportedSurfaces.size();i++)
  {
    albaVMESurface *surface = m_ImportedSurfaces[i];
    albaDEL(m_ImportedSurfaces[i]);
  }
  m_ImportedSurfaces.clear();

	delete m_FilenameSTL1;
	delete m_FilenameSTL2;
	delete m_OutputDir;
	delete m_VMEName1;
	delete m_VMEName2;
}
//----------------------------------------------------------------------------
bool albaOpComputeHausdorffDistance::InternalAccept(albaVME* node)
{
  //return ( node != NULL && node->IsA("albaVMESurface") );
  return true;
}
//----------------------------------------------------------------------------
albaOp *albaOpComputeHausdorffDistance::Copy()   
{
  return (new albaOpComputeHausdorffDistance(m_Label));
}
//----------------------------------------------------------------------------
void albaOpComputeHausdorffDistance::OpRun()   
{
	SetSurfaceInput1(m_Input);

  CreateGui();
}
//----------------------------------------------------------------------------
void albaOpComputeHausdorffDistance::CreateGui()
{
  // interface:
  m_Gui = new albaGUI(this);

  m_Gui->Label(" ");

  //////////////////////////////////////////////////////////////////////////
  // Surface 1
  //////////////////////////////////////////////////////////////////////////
  m_Gui->Label("Surface 1", true);
  wxString choices[2];
  choices[0] = "Select a Surface";
  choices[1] = "Import a STL";
  m_Gui->Radio(ID_VME_OR_STL1,"", &m_VmeOrSTL1, 2, choices, 2);
  m_Gui->Button(ID_SELECT_SURFACE1, m_VMEName1,"Select", "");
  m_Gui->FileOpen(ID_STL1, "File:", m_FilenameSTL1);

  m_Gui->Enable(ID_STL1, false);
  //////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////
  // Surface 2
  //////////////////////////////////////////////////////////////////////////
  m_Gui->Label("Surface 2", true);
  m_Gui->Radio(ID_VME_OR_STL2,"", &m_VmeOrSTL2, 2, choices, 2);
  m_Gui->Button(ID_SELECT_SURFACE2, m_VMEName2, "Select","");
  m_Gui->FileOpen(ID_STL2, "File:", m_FilenameSTL2);

  m_Gui->Enable(ID_STL2, false);
  //////////////////////////////////////////////////////////////////////////

  //m_Gui->Label("Output Directory", true);
  //m_Gui->DirOpen(-1, "", m_OutputDir);

  m_Gui->TwoButtons(ID_OP_CANCEL, ID_OP_OK, "Cancel", "Ok");

  m_Gui->Enable(ID_OP_OK, false);

  ShowGui();
}


//----------------------------------------------------------------------------
void albaOpComputeHausdorffDistance::OnEvent(albaEventBase *alba_event)
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {
    case ID_VME_OR_STL1:
      {
        m_Gui->Enable(ID_STL1, (m_VmeOrSTL1 == 1));
        m_Gui->Enable(ID_SELECT_SURFACE1, (m_VmeOrSTL1==0));
      }
      break;
    case ID_VME_OR_STL2:
      {
        m_Gui->Enable(ID_STL2, (m_VmeOrSTL2 == 1));
        m_Gui->Enable(ID_SELECT_SURFACE2, (m_VmeOrSTL2==0));
      }
      break;
    case ID_SELECT_SURFACE1:
      {
				SetSurfaceInput1(ChooseSurface());
      }
      break;
    case ID_SELECT_SURFACE2:
      {
				SetSurfaceInput2(ChooseSurface());
      }
      break;
    case ID_STL1:
      {
        if(m_STLImporter==NULL)
          m_STLImporter = new albaOpImporterSTL();
        m_STLImporter->SetFileName(m_FilenameSTL1->GetCStr());
        m_STLImporter->ImportSTL();
        std::vector<albaVMESurface*> importedSurfaces;
        m_STLImporter->GetImportedSTL(importedSurfaces);
        m_SurfaceInput1 = importedSurfaces[0];

        m_SurfaceInput1->ReparentTo(m_Input);

        cppDEL(m_STLImporter);

        if(m_SurfaceInput1 && m_SurfaceInput2)
          m_Gui->Enable(ID_OP_OK, true);
        m_Gui->Update();
      }
      break;
    case ID_STL2:
      {
        if(m_STLImporter==NULL)
          m_STLImporter = new albaOpImporterSTL();
        m_STLImporter->SetFileName(m_FilenameSTL2->GetCStr());
        m_STLImporter->ImportSTL();
        std::vector<albaVMESurface*> importedSurfaces;
        m_STLImporter->GetImportedSTL(importedSurfaces);
        m_SurfaceInput2 = importedSurfaces[0];

        m_SurfaceInput2->ReparentTo(m_Input);

        cppDEL(m_STLImporter);

        if(m_SurfaceInput1 && m_SurfaceInput2)
          m_Gui->Enable(ID_OP_OK, true);
        m_Gui->Update();
      }
      break;
    case ID_OP_OK:
      {
        int result = ComputeDistance();
        OpStop(result);  
        //OpStop(OP_RUN_OK);
      }
      break;
    case ID_OP_CANCEL:
      OpStop(OP_RUN_CANCEL);        
      break;
    }
  }
}

//----------------------------------------------------------------------------
albaVME *albaOpComputeHausdorffDistance::ChooseSurface()
{
	albaString title = albaString("Select a surface:");
	albaEvent e(this, VME_CHOOSE);
	e.SetString(&title);
	e.SetPointer(&SurfaceAccept);
	albaEventMacro(e);
	albaVME *vme = (albaVME *)e.GetVme();
	return vme;
}

//----------------------------------------------------------------------------
void albaOpComputeHausdorffDistance::SetSurfaceInput1(albaVME * vme)
{
	if (SurfaceAccept(vme))
	{
		m_SurfaceInput1 = vme;
		m_VMEName1->Copy(vme->GetName());
		if (m_Gui)
		{
			if (m_SurfaceInput1 && m_SurfaceInput2)
				m_Gui->Enable(ID_OP_OK, true);
			m_Gui->Update();
		}
	}
}

//----------------------------------------------------------------------------
void albaOpComputeHausdorffDistance::SetSurfaceInput2(albaVME * vme)
{
	if (vme)
	{
		m_SurfaceInput2 = vme;
		m_VMEName2->Copy(vme->GetName());
		if (m_Gui)
		{
			if (m_SurfaceInput1 && m_SurfaceInput2)
				m_Gui->Enable(ID_OP_OK, true);
			m_Gui->Update();
		}
	}
}

//----------------------------------------------------------------------------
void albaOpComputeHausdorffDistance::OpStop(int result)
{
  if(m_Gui)
    HideGui();
  albaEventMacro(albaEvent(this,result));  
}
//----------------------------------------------------------------------------
void albaOpComputeHausdorffDistance::OpUndo()
{
  if (m_SurfaceOutput != NULL)
  {
    m_SurfaceOutput->ReparentTo(NULL);
  }
}

//----------------------------------------------------------------------------
bool albaOpComputeHausdorffDistance::SurfaceAccept(albaVME* node)
{
	return(node && (node->IsA("albaVMESurface") || node->IsA("albaVMESurfaceParametric")));
}

//----------------------------------------------------------------------------
void albaOpComputeHausdorffDistance::OpDo()
{
 /* if(m_SurfaceInput1)
    m_SurfaceInput1->ReparentTo(m_Input->GetParent());
  if(m_SurfaceInput2)
    m_SurfaceInput2->ReparentTo(m_Input->GetParent());
*/
  if (m_SurfaceOutput != NULL)
  {
    m_SurfaceOutput->ReparentTo(m_Input);
		m_SurfaceOutput->SetAbsMatrix(albaMatrix(),m_SurfaceInput1->GetMTime());
  }
}



//----------------------------------------------------------------------------
int albaOpComputeHausdorffDistance::ComputeDistance()
{
  m_SurfaceInput1->GetOutput()->Update();
  m_SurfaceInput2->GetOutput()->Update();
  vtkPolyData *inputData1 = vtkPolyData::SafeDownCast(m_SurfaceInput1->GetOutput()->GetVTKData());
  vtkPolyData *inputData2 = vtkPolyData::SafeDownCast(m_SurfaceInput2->GetOutput()->GetVTKData());

	vtkALBASmartPointer<vtkTransform> input1Tra, input2Tra;
	vtkALBASmartPointer<vtkTransformPolyDataFilter> input1TraFilter, input2TraFilter;

	albaMatrix *input1Matr = m_SurfaceInput1->GetOutput()->GetAbsMatrix();
	input1Tra->SetMatrix(input1Matr->GetVTKMatrix());
	input1TraFilter->SetInputData(inputData1);
	input1TraFilter->SetTransform(input1Tra);
	input1TraFilter->Update();

	albaMatrix *input2Matr = m_SurfaceInput2->GetOutput()->GetAbsMatrix();
	input2Tra->SetMatrix(input2Matr->GetVTKMatrix());
	input2TraFilter->SetInputData(inputData2);
	input2TraFilter->SetTransform(input2Tra);
	input2TraFilter->Update();


  vtkPolyData *outputData;
  albaLogMessage("albaOpComputeHausdorffDistance: Creating Hausdorff Distance filter...");
  albaComputeHausdorffDistance *filter = new albaComputeHausdorffDistance();
  albaLogMessage("Created.");
  albaLogMessage("Set input");
  filter->SetData(input1TraFilter->GetOutput(), input2TraFilter->GetOutput());
  filter->SetListener(m_Listener);
  albaLogMessage("Get Output");
  outputData = filter->GetOutput();

  albaLogMessage("Creating output surface...");
  albaNEW(m_SurfaceOutput);

  //m_SurfaceOutput->DeepCopy(m_SurfaceInput);
  m_SurfaceOutput->SetDataByDetaching(outputData, 0);
  m_SurfaceOutput->Modified();

	albaString outName;
	outName.Printf("H. dist [%s to %s]", m_VMEName1->GetCStr(), m_VMEName2->GetCStr());
  m_SurfaceOutput->SetName(outName.GetCStr());
	
	if (m_Input->GetRoot()->FindInTreeById(m_SurfaceInput1->GetId()))
		m_SurfaceOutput->ReparentTo(m_SurfaceInput1);
	else
		m_SurfaceOutput->ReparentTo(m_Input);

  m_SurfaceOutput->Update();

  lutPreset(14, m_SurfaceOutput->GetMaterial()->m_ColorLut);
  albaLogMessage("Preset ten-steps LUT");
  m_SurfaceOutput->GetMaterial()->UpdateFromLut();
  m_SurfaceOutput->GetMaterial()->UpdateProp();

  m_Output = m_SurfaceOutput;

	m_Surface1Area = filter->GetTotalAreaSurf1();
	m_Surface2Area = filter->GetTotalAreaSurf2();
	
  //m_SurfaceData->Delete();
  cppDEL(filter);

  return OP_RUN_OK;

}
