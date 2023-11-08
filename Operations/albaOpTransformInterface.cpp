/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpTransformInterface
 Authors: Stefano Perticoni
 
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

#include "albaDecl.h"
#include "albaOpTransformInterface.h"
#include <wx/busyinfo.h>
#include "albaRefSys.h"
#include "albaGUI.h"
#include "albaGizmoTranslate.h"
#include "albaGizmoRotate.h"
#include "albaGUITransformMouse.h"

#include "albaInteractorGenericMouse.h"

#include "albaVME.h"
#include "albaVMEGizmo.h"
#include "albaVMEGeneric.h"
#include "albaVMEOutput.h"

#include "vtkALBASmartPointer.h"
#include "vtkTransform.h"
#include "vtkPolyData.h"
#include "vtkUnstructuredGrid.h"
#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTransformFilter.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpTransformInterface);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpTransformInterface::albaOpTransformInterface(const wxString &label) :
albaOp(label)
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
albaOpTransformInterface::~albaOpTransformInterface( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaOpTransformInterface::PostMultiplyEventMatrix(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{  
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    long arg = e->GetArg();

    // handle incoming transform events
    vtkTransform *tr = vtkTransform::New();
    tr->PostMultiply();
    tr->SetMatrix(m_Input->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
    tr->Concatenate(e->GetMatrix()->GetVTKMatrix());
    tr->Update();

    albaMatrix absPose;
    absPose.DeepCopy(tr->GetMatrix());
    absPose.SetTimeStamp(m_CurrentTime);

    if (arg == albaInteractorGenericMouse::MOUSE_MOVE)
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
albaOp* albaOpTransformInterface::Copy()   
//----------------------------------------------------------------------------
{
  return new albaOpTransformInterface(m_Label);
}

//----------------------------------------------------------------------------
void albaOpTransformInterface::OpDo()
//----------------------------------------------------------------------------
{
	// decompose matrix
	albaMatrix rotMat;
	albaMatrix scaleMat;
	double position[3];
	double scaling[3] = { 0,0,0 };

	albaTransform::PolarDecomp(m_NewAbsMatrix, rotMat, scaleMat, position);

	for (int i = 0; i < 3; i++)
	{
		scaling[i] = scaleMat.GetElement(i, i);
	}

	// create the roto-translation matrix to be set as vme abs pose
	albaMatrix rotoTraslMatrix;
	rotoTraslMatrix = rotMat;
	albaTransform::SetPosition(rotoTraslMatrix, position);

	vtkALBASmartPointer<vtkPolyData> pd;
	vtkALBASmartPointer<vtkUnstructuredGrid> ug;
	vtkALBASmartPointer<vtkRectilinearGrid> rg;
	vtkALBASmartPointer<vtkImageData> sp;

	if (m_EnableScaling == 1 && 
			!( 
				// group has no dataset
				m_Input->IsA("albaVMEGroup") ||
				//  landmarks do not scale
				m_Input->IsA("albaVMELandmark") ||
				//  parametric surfaces do not scale
				m_Input->IsA("albaVMESurfaceParametric") ||
				//  refSys do not scale
				m_Input->IsA("albaVMERefSys") ||
				//  slicer  do not scale
				m_Input->IsA("albaVMESlicer") ||
				//  slicer  do not scale
				m_Input->IsA("albaVMELandmarkCloud")
			 )
		 )
  {
    // apply scale to data
    
    // create the scale transform to be applied to data
    vtkALBASmartPointer<vtkTransform> scaleTransform;
    scaleTransform->Scale(scaling);

    albaVME *inVME = m_Input;

    vtkDataSet *dataSet = inVME->GetOutput()->GetVTKData();
    
    std::ostringstream stringStream;
    stringStream << "albaOpTransformInterface : Applying scaling to vtk dataset..."  << std::endl;
    albaLogMessage(stringStream.str().c_str());

    if (dataSet->IsA("vtkPolyData"))
    {
      // apply fast vtkPolyDataTransformFilter
      vtkPolyData *currentPD = vtkPolyData::SafeDownCast(m_Input->GetOutput()->GetVTKData());
      assert(currentPD);

      pd->DeepCopy(currentPD);

      vtkALBASmartPointer<vtkTransformPolyDataFilter> tPDF;
      tPDF->SetInput(pd);
      tPDF->SetTransform(scaleTransform);

      // progress bar stuff
      if (!m_TestMode)
      {
        wxString progress_string("Applying scaling to data...");
        wxBusyInfo wait(progress_string.ToAscii());
      }

      albaEventMacro(albaEvent(this,BIND_TO_PROGRESSBAR,tPDF.GetPointer()));
      tPDF->Update();

      ((albaVMEGeneric *)m_Input)->SetData(tPDF->GetOutput(),m_Input->GetTimeStamp());
    }
    else if (dataSet->IsA("vtkUnstructuredGrid"))
    {
      // apply fast vtkPolyDataTransformFilter
      vtkUnstructuredGrid *currentUG = vtkUnstructuredGrid::SafeDownCast(m_Input->GetOutput()->GetVTKData());
      assert(currentUG);

      ug->DeepCopy(currentUG);

      vtkALBASmartPointer<vtkTransformFilter> tf;
      tf->SetInput(ug);
      tf->SetTransform(scaleTransform);

      // progress bar stuff
      wxString progress_string("Applying scaling to data...");
      wxBusyInfo wait(progress_string.ToAscii());

      albaEventMacro(albaEvent(this,BIND_TO_PROGRESSBAR,tf.GetPointer()));
      tf->Update();

      ((albaVMEGeneric *)m_Input)->SetData(tf->GetOutput(),m_Input->GetTimeStamp());
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

      ((albaVMEGeneric *)m_Input)->SetData(sp,m_Input->GetTimeStamp());
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
        albaEventMacro(albaEvent(this,PROGRESSBAR_SET_VALUE,progress));
      }
      
      rg->Modified();

      ((albaVMEGeneric *)m_Input)->SetData(rg,m_Input->GetTimeStamp());
    }
  }
  // apply roto-translation to abs pose
  m_Input->SetAbsMatrix(rotoTraslMatrix, m_CurrentTime);
  
  m_Input->GetOutput()->Update();

	GetLogicManager()->CameraUpdate();
}

//----------------------------------------------------------------------------
void albaOpTransformInterface::SetRefSysVME(albaVME* refSysVME)
//----------------------------------------------------------------------------
{
  // ref sys vme must be not null
  if(refSysVME == NULL)
	return;

  m_RefSysVME = refSysVME;
  RefSysVmeChanged();
	GetLogicManager()->CameraUpdate();

}
