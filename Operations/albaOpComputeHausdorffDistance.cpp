/*=========================================================================
Program:   LHP
Module:    $RCSfile: lhpOpComputeHausdorffDistance.cpp,v $
Language:  C++
Date:      $Date: 2012-03-20 15:32:41 $
Version:   $Revision: 1.1.2.4 $
Authors:   Eleonora Mambrini
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

#include "wx/busyinfo.h"

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
mafCxxTypeMacro(lhpOpComputeHausdorffDistance);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
lhpOpComputeHausdorffDistance::lhpOpComputeHausdorffDistance(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo	= true;

  m_FilenameSTL1 = new mafString("");
  m_FilenameSTL2 = new mafString("");
  m_OutputDir = new mafString("");

  m_VMEName1 = new mafString("NONE");
  m_VMEName2 = new mafString("NONE");

  m_SurfaceOutput = NULL;
  m_SurfaceInput1 = m_SurfaceInput2 = NULL;

  m_VmeOrSTL1 = m_VmeOrSTL2= 0;

  m_STLImporter = NULL;
}
//----------------------------------------------------------------------------
lhpOpComputeHausdorffDistance::~lhpOpComputeHausdorffDistance()
//----------------------------------------------------------------------------
{
  mafDEL(m_SurfaceOutput);
  if(m_STLImporter)
    mafDEL(m_STLImporter);
  for(int i=0;i<m_ImportedSurfaces.size();i++)
  {
    mafVMESurface *surface = m_ImportedSurfaces[i];
    mafDEL(m_ImportedSurfaces[i]);
  }
  m_ImportedSurfaces.clear();

	delete m_FilenameSTL1;
	delete m_FilenameSTL2;
	delete m_OutputDir;
	delete m_VMEName1;
	delete m_VMEName2;
}
//----------------------------------------------------------------------------
bool lhpOpComputeHausdorffDistance::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  //return ( node != NULL && node->IsA("mafVMESurface") );
  return true;
}
//----------------------------------------------------------------------------
mafOp *lhpOpComputeHausdorffDistance::Copy()   
//----------------------------------------------------------------------------
{
  return (new lhpOpComputeHausdorffDistance(m_Label));
}
//----------------------------------------------------------------------------
void lhpOpComputeHausdorffDistance::OpRun()   
//----------------------------------------------------------------------------
{
  //m_SurfaceInput = mafVMESurface::SafeDownCast(m_Input);

  //m_SurfaceInput->Update();

  CreateGui();
}
//----------------------------------------------------------------------------
void lhpOpComputeHausdorffDistance::CreateGui()
//----------------------------------------------------------------------------
{
  // interface:
  m_Gui = new mafGUI(this);

  m_Gui->Label(" ");

  //////////////////////////////////////////////////////////////////////////
  // Surface 1
  //////////////////////////////////////////////////////////////////////////
  m_Gui->Label("Surface 1", true);
  wxString choices[2];
  choices[0] = "Select a Surface";
  choices[1] = "Import a STL file";
  m_Gui->Radio(ID_VME_OR_STL1,"", &m_VmeOrSTL1, 2, choices, 2);
  m_Gui->Button(ID_SELECT_SURFACE1, m_VMEName1,"Select", "");
  m_Gui->FileOpen(ID_STL1, "Import STL", m_FilenameSTL1);

  m_Gui->Enable(ID_STL1, false);
  //////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////
  // Surface 2
  //////////////////////////////////////////////////////////////////////////
  m_Gui->Label("Surface 2", true);
  m_Gui->Radio(ID_VME_OR_STL2,"", &m_VmeOrSTL2, 2, choices, 2);
  m_Gui->Button(ID_SELECT_SURFACE2, m_VMEName2, "Select","");
  m_Gui->FileOpen(ID_STL2, "Import STL", m_FilenameSTL2);

  m_Gui->Enable(ID_STL2, false);
  //////////////////////////////////////////////////////////////////////////

  //m_Gui->Label("Output Directory", true);
  //m_Gui->DirOpen(-1, "", m_OutputDir);

  m_Gui->TwoButtons(ID_OP_CANCEL, ID_OP_OK, "Cancel", "Ok");

  m_Gui->Enable(ID_OP_OK, false);

  ShowGui();
}


//----------------------------------------------------------------------------
void lhpOpComputeHausdorffDistance::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
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
        mafString title = mafString("Select a surface:");
        mafEvent e(this,VME_CHOOSE);
        e.SetString(&title);
        e.SetArg((long)(&lhpOpComputeHausdorffDistance::SurfaceAccept)); // accept only mafVMESurface
        mafEventMacro(e);
        mafVME *vme = (mafVME *)e.GetVme();

        if(vme)
        {
          m_SurfaceInput1 = mafVMESurface::SafeDownCast(vme);
          m_VMEName1->Copy(vme->GetName());
          if(m_SurfaceInput1 && m_SurfaceInput2)
            m_Gui->Enable(ID_OP_OK, true);
          m_Gui->Update();
        }
      }
      break;
    case ID_SELECT_SURFACE2:
      {
        mafString title = mafString("Select a surface:");
        mafEvent e(this,VME_CHOOSE);
        e.SetString(&title);
        e.SetArg((long)(&lhpOpComputeHausdorffDistance::SurfaceAccept)); // accept only mafVMESurface
        mafEventMacro(e);
        mafVME *vme = (mafVME *)e.GetVme();

        if(vme)
        {
          m_SurfaceInput2 = mafVMESurface::SafeDownCast(vme);
          m_VMEName2->Copy(vme->GetName());
          if(m_SurfaceInput1 && m_SurfaceInput2)
            m_Gui->Enable(ID_OP_OK, true);
          m_Gui->Update();
        }
      }
      break;
    case ID_STL1:
      {
        if(m_STLImporter==NULL)
          m_STLImporter = new mafOpImporterSTL();
        m_STLImporter->SetFileName(m_FilenameSTL1->GetCStr());
        m_STLImporter->ImportSTL();
        std::vector<mafVMESurface*> importedSurfaces;
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
          m_STLImporter = new mafOpImporterSTL();
        m_STLImporter->SetFileName(m_FilenameSTL2->GetCStr());
        m_STLImporter->ImportSTL();
        std::vector<mafVMESurface*> importedSurfaces;
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
void lhpOpComputeHausdorffDistance::OpStop(int result)
//----------------------------------------------------------------------------
{
  if(m_Gui)
    HideGui();
  mafEventMacro(mafEvent(this,result));  
}
//----------------------------------------------------------------------------
void lhpOpComputeHausdorffDistance::OpUndo()
//----------------------------------------------------------------------------
{
  if (m_SurfaceOutput != NULL)
  {
    m_SurfaceOutput->ReparentTo(NULL);
  }
}
//----------------------------------------------------------------------------
void lhpOpComputeHausdorffDistance::OpDo()
//----------------------------------------------------------------------------
{
 /* if(m_SurfaceInput1)
    m_SurfaceInput1->ReparentTo(m_Input->GetParent());
  if(m_SurfaceInput2)
    m_SurfaceInput2->ReparentTo(m_Input->GetParent());
*/
  if (m_SurfaceOutput != NULL)
  {
    m_SurfaceOutput->ReparentTo(m_Input);
  }
}



//----------------------------------------------------------------------------
int lhpOpComputeHausdorffDistance::ComputeDistance()
//----------------------------------------------------------------------------
{
  m_SurfaceInput1->GetSurfaceOutput()->Update();
  m_SurfaceInput2->GetSurfaceOutput()->Update();
  vtkPolyData *inputData1 = (vtkPolyData *)(m_SurfaceInput1->GetSurfaceOutput()->GetVTKData());
  vtkPolyData *inputData2 = (vtkPolyData *)(m_SurfaceInput2->GetSurfaceOutput()->GetVTKData());
  vtkPolyData *outputData;
  mafLogMessage("lhpOpComputeHausdorffDistance: Creating Hausdorff Distance filter...");
  albaOpComputeHausdorffDistance *filter = new albaOpComputeHausdorffDistance();
  albaLogMessage("Created.");
  albaLogMessage("Set input");
  filter->SetData(inputData1, inputData2);
  filter->SetListener(m_Listener);
  albaLogMessage("Get Output");
  outputData = filter->GetOutput();

  albaLogMessage("Creating output surface...");
  albaNEW(m_SurfaceOutput);

  //m_SurfaceOutput->DeepCopy(m_SurfaceInput);
  m_SurfaceOutput->SetDataByDetaching(outputData, 0);
  m_SurfaceOutput->Modified();

  m_SurfaceOutput->SetName("Hausdorff Distance Output");
  m_SurfaceOutput->ReparentTo(m_Input);
  m_SurfaceOutput->Update();

  lutPreset(14, m_SurfaceOutput->GetMaterial()->m_ColorLut);
  albaLogMessage("Preset ten-steps LUT");
  m_SurfaceOutput->GetMaterial()->UpdateFromLut();
  m_SurfaceOutput->GetMaterial()->UpdateProp();

  m_Output = m_SurfaceOutput;

  //m_SurfaceData->Delete();
  cppDEL(filter);

  return OP_RUN_OK;

}
