/*=========================================================================

 Program: MAF2
 Module: mafOpTransformInterface
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafDecl.h"
#include "mafOpTransformInterface.h"
#include <wx/busyinfo.h>
#include "mafRefSys.h"
#include "mafGUI.h"
#include "mafGizmoTranslate.h"
#include "mafGizmoRotate.h"
#include "mafGUITransformMouse.h"

#include "mafInteractorGenericMouse.h"

#include "mafVME.h"
#include "mafVMEGizmo.h"
#include "mafVMEGeneric.h"
#include "mafVMEOutput.h"

#include "vtkMAFSmartPointer.h"
#include "vtkTransform.h"
#include "vtkPolyData.h"
#include "vtkUnstructuredGrid.h"
#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTransformFilter.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpTransformInterface);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpTransformInterface::mafOpTransformInterface(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType = OPTYPE_OP;
  m_Canundo = true;

  m_CurrentTime = -1;
  m_OldAbsMatrix.Identity();
  m_NewAbsMatrix.Identity();

  // gizmo not active by default
  m_UseGizmo = 0;
  m_ActiveGizmo = TR_GIZMO;

  m_RefSysVME = NULL;
  m_RefSysVMEName = "unassigned";

  // operation involve scaling by default; override this behavior in derived class constructor
  m_EnableScaling = 1;
}
//----------------------------------------------------------------------------
mafOpTransformInterface::~mafOpTransformInterface( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOpTransformInterface::PostMultiplyEventMatrix(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{  
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    long arg = e->GetArg();

    // handle incoming transform events
    vtkTransform *tr = vtkTransform::New();
    tr->PostMultiply();
    tr->SetMatrix(m_Input->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
    tr->Concatenate(e->GetMatrix()->GetVTKMatrix());
    tr->Update();

    mafMatrix absPose;
    absPose.DeepCopy(tr->GetMatrix());
    absPose.SetTimeStamp(m_CurrentTime);

    if (arg == mafInteractorGenericMouse::MOUSE_MOVE)
    {
      // move vme
      m_Input->SetAbsMatrix(absPose);
      // update matrix for OpDo()
      m_NewAbsMatrix = absPose;
    } 
		GetLogicManager()->CameraUpdate();

    // clean up
    tr->Delete();
  }
}

//----------------------------------------------------------------------------
mafOp* mafOpTransformInterface::Copy()   
//----------------------------------------------------------------------------
{
  return new mafOpTransformInterface(m_Label);
}

//----------------------------------------------------------------------------
void mafOpTransformInterface::OpDo()
//----------------------------------------------------------------------------
{
  // decompose matrix
  mafMatrix rotMat;
  mafMatrix scaleMat;
  double position[3];
  double scaling[3] = {0,0,0};

  mafTransform::PolarDecomp(m_NewAbsMatrix, rotMat, scaleMat, position);

  for (int i = 0;i < 3; i++)
  {
    scaling[i] = scaleMat.GetElement(i,i);
  }

  // create the roto-translation matrix to be set as vme abs pose
  mafMatrix rotoTraslMatrix;
  rotoTraslMatrix = rotMat;
  mafTransform::SetPosition(rotoTraslMatrix, position);

  vtkMAFSmartPointer<vtkPolyData> pd;
  vtkMAFSmartPointer<vtkUnstructuredGrid> ug;
  vtkMAFSmartPointer<vtkRectilinearGrid> rg;
  vtkMAFSmartPointer<vtkImageData> sp;

  if (m_EnableScaling == 1 && 
      // group has no dataset
      !m_Input->IsA("mafVMEGroup") &&
      //  landmarks do not scale
      !m_Input->IsA("mafVMELandmark") && 
      //  parametric surfaces do not scale
      !m_Input->IsA("mafVMESurfaceParametric") &&
      //  refSys do not scale
      !m_Input->IsA("mafVMERefSys") &&
      //  slicer  do not scale
      !m_Input->IsA("mafVMESlicer") &&
      //  slicer  do not scale
      !m_Input->IsA("mafVMELandmarkCloud")
     )
  {
    // apply scale to data
    
    // create the scale transform to be applied to data
    vtkMAFSmartPointer<vtkTransform> scaleTransform;
    scaleTransform->Scale(scaling);

    mafVME *inVME = m_Input;

    vtkDataSet *dataSet = inVME->GetOutput()->GetVTKData();
    
    std::ostringstream stringStream;
    stringStream << "mafOpTransformInterface : Applying scaling to vtk dataset..."  << std::endl;
    mafLogMessage(stringStream.str().c_str());

    if (dataSet->IsA("vtkPolyData"))
    {
      // apply fast vtkPolyDataTransformFilter
      vtkPolyData *currentPD = vtkPolyData::SafeDownCast(m_Input->GetOutput()->GetVTKData());
      assert(currentPD);

      pd->DeepCopy(currentPD);

      vtkMAFSmartPointer<vtkTransformPolyDataFilter> tPDF;
      tPDF->SetInputData(pd);
      tPDF->SetTransform(scaleTransform);

      // progress bar stuff
      if (!m_TestMode)
      {
        wxString progress_string("Applying scaling to data...");
        wxBusyInfo wait(progress_string.c_str());
      }

      mafEventMacro(mafEvent(this,BIND_TO_PROGRESSBAR,tPDF.GetPointer()));
      tPDF->Update();

      ((mafVMEGeneric *)m_Input)->SetData(tPDF->GetOutput(),m_Input->GetTimeStamp());
    }
    else if (dataSet->IsA("vtkUnstructuredGrid"))
    {
      // apply fast vtkPolyDataTransformFilter
      vtkUnstructuredGrid *currentUG = vtkUnstructuredGrid::SafeDownCast(m_Input->GetOutput()->GetVTKData());
      assert(currentUG);

      ug->DeepCopy(currentUG);

      vtkMAFSmartPointer<vtkTransformFilter> tf;
      tf->SetInputData(ug);
      tf->SetTransform(scaleTransform);

      // progress bar stuff
      wxString progress_string("Applying scaling to data...");
      wxBusyInfo wait(progress_string.c_str());

      mafEventMacro(mafEvent(this,BIND_TO_PROGRESSBAR,tf.GetPointer()));
      tf->Update();

      ((mafVMEGeneric *)m_Input)->SetData(tf->GetOutput(),m_Input->GetTimeStamp());
    }
    else if (dataSet->IsA("vtkImageData"))
    {
      if (!m_TestMode)
      {
      	wxBusyInfo wait_info("Applying scaling to data...");
      }

      vtkImageData *currentSP = vtkImageData::SafeDownCast(m_Input->GetOutput()->GetVTKData());
      assert(currentSP);
 
      sp->DeepCopy(currentSP);

      double oldSpacing[3] = {0,0,0};
      double newSpacing[3] = {0,0,0};
      currentSP->GetSpacing(oldSpacing);

      for (int i = 0; i<3; i++)
      {
        newSpacing[i] = oldSpacing[i] * scaling[i];        
      }
      sp->SetSpacing(newSpacing);      
      sp->Modified();

      ((mafVMEGeneric *)m_Input)->SetData(sp,m_Input->GetTimeStamp());
    }
    else if (dataSet->IsA("vtkRectilinearGrid"))
    {
	    if (!m_TestMode)
	    {
	    	wxBusyInfo wait_info("Applying scaling to data...");
	    }
  
      long progress = 0;

      vtkRectilinearGrid *currentRG = vtkRectilinearGrid::SafeDownCast(m_Input->GetOutput()->GetVTKData());
      assert(currentRG);

      rg->DeepCopy(currentRG);
      
      vtkDataArray *daVector[3] = {rg->GetXCoordinates(), rg->GetYCoordinates(), rg->GetZCoordinates()};

      for (int arrayId = 0; arrayId<3; arrayId ++)
      {
        
        int numTuples = daVector[arrayId]->GetNumberOfTuples();
        
        for (int tupleId = 0; tupleId<numTuples; tupleId++)
        {
          double oldVal = daVector[arrayId]->GetComponent(tupleId, 0);
          daVector[arrayId]->SetComponent(tupleId, 0, oldVal * scaling[arrayId]);
        }

        progress = (arrayId + 1) * 100 / 3;
        mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,progress));
      }
      
      rg->Modified();

      ((mafVMEGeneric *)m_Input)->SetData(rg,m_Input->GetTimeStamp());
    }
  }
  // apply roto-translation to abs pose
  m_Input->SetAbsMatrix(rotoTraslMatrix, m_CurrentTime);
  
  m_Input->GetOutput()->Update();

	GetLogicManager()->CameraUpdate();
}

//----------------------------------------------------------------------------
void mafOpTransformInterface::SetRefSysVME(mafVME* refSysVME)
//----------------------------------------------------------------------------
{
  // ref sys vme must be not null
  if(refSysVME == NULL)
	return;

  m_RefSysVME = refSysVME;
  RefSysVmeChanged();
	GetLogicManager()->CameraUpdate();

}
