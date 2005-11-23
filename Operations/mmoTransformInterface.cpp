/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoTransformInterface.cpp,v $
  Language:  C++
  Date:      $Date: 2005-11-23 12:32:29 $
  Version:   $Revision: 1.3 $
  Authors:   Stefano Perticoni
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


#include "mmoTransformInterface.h"
#include <wx/busyinfo.h>

#include "mafDecl.h"
#include "mmgGui.h"
#include "mafGizmoTranslate.h"
#include "mafGizmoRotate.h"
#include "mafGuiTransformMouse.h"

#include "mmiGenericMouse.h"

#include "mafSmartPointer.h"
#include "mafTransform.h"
#include "mafVME.h"
#include "mafVMEGizmo.h"
#include "mafVMEGeneric.h"
#include "mafVMEOutput.h"

#include "vtkMAFSmartPointer.h"
#include "vtkTransform.h"
#include "vtkPolyData.h"
#include "vtkStructuredPoints.h"
#include "vtkRectilinearGrid.h"
#include "vtkTransformPolyDataFilter.h"

//----------------------------------------------------------------------------
mmoTransformInterface::mmoTransformInterface(wxString label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType = OPTYPE_OP;
  m_Canundo = true;

  CurrentTime = -1;
  OldAbsMatrix.Identity();
  NewAbsMatrix.Identity();

  // gizmo not active by default
  UseGizmo = 0;
  ActiveGizmo = TR_GIZMO;

  RefSysVME = NULL;
  RefSysVMEName = "unassigned";

  // operation involve scaling by default; override this behavior in derived class constructor
  EnableScaling = 1;
}
//----------------------------------------------------------------------------
mmoTransformInterface::~mmoTransformInterface( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mmoTransformInterface::PostMultiplyEventMatrix(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{  
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    long arg = e->GetArg();

    // handle incoming transform events
    vtkTransform *tr = vtkTransform::New();
    tr->PostMultiply();
    tr->SetMatrix(((mafVME *)m_Input)->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
    tr->Concatenate(e->GetMatrix()->GetVTKMatrix());
    tr->Update();

    mafSmartPointer<mafMatrix> absPose;
    absPose->DeepCopy(tr->GetMatrix());
    absPose->SetTimeStamp(CurrentTime);

    if (arg == mmiGenericMouse::MOUSE_MOVE)
    {
      // move vme
      ((mafVME *)m_Input)->SetAbsMatrix(*absPose.GetPointer());
      // update matrix for OpDo()
      NewAbsMatrix = *absPose.GetPointer();
    } 
    mafEventMacro(mafEvent(this, CAMERA_UPDATE));

    // clean up
    tr->Delete();
  }
}

//----------------------------------------------------------------------------
mafOp* mmoTransformInterface::Copy()   
//----------------------------------------------------------------------------
{
  return new mmoTransformInterface(m_Label);
}

//----------------------------------------------------------------------------
void mmoTransformInterface::OpDo()
//----------------------------------------------------------------------------
{
  if (!m_Input->IsMAFType(mafVMEGeneric))
  {
    return;
  }

  // decompose matrix
  mafSmartPointer<mafMatrix> rotMat;
  mafSmartPointer<mafMatrix> scaleMat;
  double position[3];
  double scaling[3] = {0,0,0};

  mafTransform::PolarDecomp(NewAbsMatrix, *rotMat, *scaleMat, position);

  for (int i = 0;i < 3; i++)
  {
    scaling[i] = scaleMat->GetElement(i,i);
  }

  // create the rototranslation matrix to be set as vme abs pose
  mafSmartPointer<mafMatrix> rotoTraslMatrix;
  *rotoTraslMatrix = *rotMat;
  mafTransform::SetPosition(*rotoTraslMatrix, position);

  vtkMAFSmartPointer<vtkPolyData> pd;
  vtkMAFSmartPointer<vtkRectilinearGrid> rg;
  vtkMAFSmartPointer<vtkStructuredPoints> sp;

  if (EnableScaling == 1)
  {
    // apply scale to data
    
    // create the scale transform to be applied to data
    vtkMAFSmartPointer<vtkTransform> scaleTransform;
    scaleTransform->Scale(scaling);

    if (((mafVME *)m_Input)->GetOutput()->GetVTKData()->IsA("vtkPolyData"))
    {
      // apply fast vtkPolyDataTransformFilter
      vtkPolyData *currentPD = vtkPolyData::SafeDownCast(((mafVME *)m_Input)->GetOutput()->GetVTKData());
      assert(currentPD);

      pd->DeepCopy(currentPD);

      vtkMAFSmartPointer<vtkTransformPolyDataFilter> tPDF;
      tPDF->SetInput(pd);
      tPDF->SetTransform(scaleTransform);

      // progress bar stuff
      wxString progress_string("Applying scaling to data...");
      wxBusyInfo wait(progress_string.c_str());

      //mafProgressMacro(tPDF.GetPointer(), progress_string.c_str());
      mafEventMacro(mafEvent(this,BIND_TO_PROGRESSBAR,tPDF.GetPointer()));
      tPDF->Update();

      ((mafVMEGeneric *)m_Input)->SetData(tPDF->GetOutput(),((mafVME *)m_Input)->GetTimeStamp());
    }
    else if (((mafVME *)m_Input)->GetOutput()->GetVTKData()->IsA("vtkStructuredPoints"))
    {
      wxBusyInfo wait_info("Applying scaling to data...");

      vtkStructuredPoints *currentSP = vtkStructuredPoints::SafeDownCast(((mafVME *)m_Input)->GetOutput()->GetVTKData());
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

      ((mafVMEGeneric *)m_Input)->SetData(sp,((mafVME *)m_Input)->GetTimeStamp());
    }
    else if (((mafVME *)m_Input)->GetOutput()->GetVTKData()->IsA("vtkRectilinearGrid"))
    {
      // get the dicom files from the directory
	    wxBusyInfo wait_info("Applying scaling to data...");
  
      //mafProgressBarShowMacro();
      //mafProgressBarSetTextMacro("Applying scaling to data...");
      mafEventMacro(mafEvent(this,PROGRESSBAR_SET_TEXT));
      long progress = 0;

      vtkRectilinearGrid *currentRG = vtkRectilinearGrid::SafeDownCast(((mafVME *)m_Input)->GetOutput()->GetVTKData());
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
        //mafProgressBarSetValueMacro(progress);
      }
      
      rg->Modified();

      //mafProgressBarHideMacro();
      ((mafVMEGeneric *)m_Input)->SetData(rg,((mafVME *)m_Input)->GetTimeStamp());
    }
  }
  // apply roto-translation to abs pose
  ((mafVME *)m_Input)->SetAbsMatrix(*rotoTraslMatrix, CurrentTime);
  
  mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}

//----------------------------------------------------------------------------
void mmoTransformInterface::SetRefSysVME(mafVME* refSysVME)
//----------------------------------------------------------------------------
{
  // ref sys vme must be not null
  if(refSysVME == NULL)
	return;

  RefSysVME = refSysVME;
  RefSysVmeChanged();
}
