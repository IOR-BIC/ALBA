/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGizmoROI.cpp,v $
  Language:  C++
  Date:      $Date: 2007-06-25 10:03:01 $
  Version:   $Revision: 1.6 $
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


#include "mafGizmoROI.h"
#include "mafDecl.h"
#include "mafGizmoBoundingBox.h"
#include "mafGizmoHandle.h"
#include "mmiGenericMouse.h"
#include "mafGizmoHandle.h"

#include "mafMatrix.h"
#include "mafTransform.h"
#include "mafVME.h"
#include "mafVMEOutput.h"
#include "mafSmartPointer.h"

#include "vtkTransform.h"
#include "vtkCubeSource.h"
#include "vtkDataSet.h"

#include <vector>
#include <algorithm>

using namespace std;

//----------------------------------------------------------------------------
mafGizmoROI::mafGizmoROI(mafVME* input, mafObserver *listener , int constraintModality, mafVME* parent)
//----------------------------------------------------------------------------
{
  assert(input);
  InputVME = input;
  m_Listener = listener;
	m_ConstraintModality = constraintModality;
  //no gizmo component is active at construction
  
  this->ActiveGizmoComponent = -1;
  this->SetModalityToLocal();

  for (int i = 0; i < 6; i++)
  {
    // Create mafGizmoHandle and send events to this
    GHandle[i] = NULL;
    GHandle[i] = new mafGizmoHandle(input, this, constraintModality,parent);
    GHandle[i]->SetType(i);
  }
  
	// create the outline gizmo
	OutlineGizmo = new mafGizmoBoundingBox(input, this,parent);
}
//----------------------------------------------------------------------------
mafGizmoROI::~mafGizmoROI() 
//----------------------------------------------------------------------------
{
  //Destroy:
  //6 mafGizmoHandle 
  
  for (int i = 0; i < 6; i++)
  {    
    cppDEL(GHandle[i]);
  }

  // the gizmo outline
  cppDEL(OutlineGizmo);
}
//----------------------------------------------------------------------------
void mafGizmoROI::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  // get the sender
  void *sender = maf_event->GetSender();
    
  if (sender == GHandle[0] || sender == GHandle[1] || sender == GHandle[2] || sender == GHandle[3] || sender == GHandle[4] || sender == GHandle[5])
  {
    OnEventGizmoComponents(maf_event); // process events from gizmo handles
  }
  else
  {
    // forward event to the listener
    mafEventMacro(*maf_event);
  }
}

//----------------------------------------------------------------------------
void mafGizmoROI::OnEventGizmoComponents(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    // receiving events from gizmo handles
    switch(e->GetId())
    {
      //receiving pose matrixes from isa
      case ID_TRANSFORM:
      {
        void *sender = e->GetSender();
        long arg = e->GetArg();

        // if a gizmo has been picked highlight the gizmo and register the active component
        if (arg == mmiGenericMouse::MOUSE_DOWN)
        {
          if (sender == GHandle[0])
          {
            this->Highlight(0);
            ActiveGizmoComponent = 0;
          }
          else if (sender == GHandle[1])
          {
            this->Highlight(1);
            ActiveGizmoComponent = 1;
          }
          else if (sender == GHandle[2])
          {
            this->Highlight(2);
            ActiveGizmoComponent = 2;
          }
          else if (sender == GHandle[3])
          {
            this->Highlight(3);
            ActiveGizmoComponent = 3;
          }
          else if (sender == GHandle[4])
          {
            this->Highlight(4);
            ActiveGizmoComponent = 4;
          }
          else if (sender == GHandle[5])
          {
            this->Highlight(5);
            ActiveGizmoComponent = 5;
          }
        }
        else if (arg == mmiGenericMouse::MOUSE_MOVE)
        {     
          if (this->Modality == G_LOCAL)
          {
            // local mode

            // translate selected gizmo
            mafTransform *tr = mafTransform::New();                   
            tr->SetMatrix(*GHandle[ActiveGizmoComponent]->GetPose());
            tr->Concatenate(*e->GetMatrix(), PRE_MULTIPLY);

            mafMatrix mat;
            mat.DeepCopy(tr->GetMatrixPointer());
            mat.SetTimeStamp(InputVME->GetTimeStamp());

            GHandle[ActiveGizmoComponent]->SetPose(&mat);

            // update other gizmos positions in order to recenter them
            UpdateHandlePositions();

            // Update the gizmo length in order to be a fixed fraction
            // of the outline gizmo dimension
            UpdateGizmosLength();
            tr->Delete();
          }          
          else
          {
            // global mode
            // not yet implemented...
          }
        }
        else if (arg == mmiGenericMouse::MOUSE_UP)
        {
        }
        mafEventMacro(mafEvent(this, CAMERA_UPDATE));
        // forward isa transform events to the listener op
        mafEventMacro(*e);
      }
      break;

      default:
      {
        mafEventMacro(*e);
      }
      break;
    }
  }
}

//----------------------------------------------------------------------------
void mafGizmoROI::Highlight (int component) 
//----------------------------------------------------------------------------
{
  GHandle[component]->Highlight(true);
  for (int i = 0; i < 6; i++ )
  {
    if (i != component)
    {
      GHandle[i]->Highlight(false);
    }
  }
}

//----------------------------------------------------------------------------
void mafGizmoROI::HighlightOff () 
//----------------------------------------------------------------------------
{
  for (int i = 0; i < 6; i++ )
  {
    GHandle[i]->Highlight(false);
  }
}

//----------------------------------------------------------------------------  
void mafGizmoROI::Show(bool show)
//----------------------------------------------------------------------------
{
  ShowHandles(show);
  ShowROI(show);
}

//----------------------------------------------------------------------------  
void mafGizmoROI::ShowHandles(bool show)
//----------------------------------------------------------------------------  
{
  for (int i = 0; i < 6; i++)
  {
    GHandle[i]->Show(show);
  }
}

//----------------------------------------------------------------------------  
void mafGizmoROI::ShowROI(bool show)
//----------------------------------------------------------------------------  
{
  OutlineGizmo->Show(show);
}
  
//----------------------------------------------------------------------------  
void mafGizmoROI::SetConstrainRefSys(mafMatrix *constrain)
//----------------------------------------------------------------------------
{
  for (int i =0; i < 6; i++)
  {
    GHandle[i]->SetConstrainRefSys(constrain);
  }
}

//----------------------------------------------------------------------------  
void mafGizmoROI::SetInput(mafVME *input)
//----------------------------------------------------------------------------
{
  this->InputVME = input;
  for (int i = 0; i < 6; i++)
  {
    GHandle[i]->SetInput(input);
  }

  OutlineGizmo->SetInput(input);
}

//----------------------------------------------------------------------------  
void mafGizmoROI::UpdateHandlePositions()
//----------------------------------------------------------------------------
{
  double posMin[3] = {0, 0, 0};
  double posMax[3] = {0, 0, 0};
  double oldPos[3] = {0, 0, 0};

  // move 0 or 1 => recenter 2, 3, 4, 5
  if (ActiveGizmoComponent == 0 || ActiveGizmoComponent == 1)
  {
    mafTransform::GetPosition(*GHandle[0]->GetPose(), posMin);
    mafTransform::GetPosition(*GHandle[1]->GetPose(), posMax);
    
    double xMean = (posMin[0] + posMax[0]) / 2;
    for (int i = 2; i < 6; i++)
    {      
      mafTransform::GetPosition(*GHandle[i]->GetPose(), oldPos);
      oldPos[0] = xMean;
      mafTransform::SetPosition(*GHandle[i]->GetPose(), oldPos);
      mafTransform::SetPosition(GHandle[i]->GetPivotMatrix(), oldPos);
			//Matteo 23-08-06
			mafSmartPointer<mafTransform> tr;
			tr->SetMatrix(*GHandle[i]->GetPose());

			mafMatrix mat;
			mat.DeepCopy(tr->GetMatrixPointer());
			mat.SetTimeStamp(InputVME->GetTimeStamp());

			GHandle[i]->SetPose(&mat);
			//End Matteo
    }
  }
  // move 2 or 3 => recenter 0, 1, 4, 5
  else if (ActiveGizmoComponent == 2 || ActiveGizmoComponent == 3)
  {
    mafTransform::GetPosition(*GHandle[2]->GetPose(), posMin);
    mafTransform::GetPosition(*GHandle[3]->GetPose(), posMax);
    
    double yMean = (posMin[1] + posMax[1]) / 2;

    for (int i = 0; i < 6; i++)
    {      
      if (i != 2 && i != 3)
      {      
        mafTransform::GetPosition(*GHandle[i]->GetPose(), oldPos);
        oldPos[1] = yMean;
        mafTransform::SetPosition(*GHandle[i]->GetPose(), oldPos);
        mafTransform::SetPosition(GHandle[i]->GetPivotMatrix(), oldPos);
				//Matteo 23-08-06
				mafSmartPointer<mafTransform> tr;
				tr->SetMatrix(*GHandle[i]->GetPose());

				mafMatrix mat;
				mat.DeepCopy(tr->GetMatrixPointer());
				mat.SetTimeStamp(InputVME->GetTimeStamp());

				GHandle[i]->SetPose(&mat);
				//End Matteo
      }
    }
  }
  // move 4 or 5 => recenter 0, 1, 2, 3
  else if (ActiveGizmoComponent == 4 || ActiveGizmoComponent == 5)
  {
    mafTransform::GetPosition(*GHandle[4]->GetPose(), posMin);
    mafTransform::GetPosition(*GHandle[5]->GetPose(), posMax);
    
    double zMean = (posMin[2] + posMax[2]) / 2;

    for (int i = 0; i < 4; i++)
    {      
      mafTransform::GetPosition(*GHandle[i]->GetPose(), oldPos);
      oldPos[2] = zMean;
      mafTransform::SetPosition(*GHandle[i]->GetPose(), oldPos);
      mafTransform::SetPosition(GHandle[i]->GetPivotMatrix(), oldPos);
			//Matteo 23-08-06
			mafSmartPointer<mafTransform> tr;
			tr->SetMatrix(*GHandle[i]->GetPose());

			mafMatrix mat;
			mat.DeepCopy(tr->GetMatrixPointer());
			mat.SetTimeStamp(InputVME->GetTimeStamp());

			GHandle[i]->SetPose(&mat);
			//End Matteo
    }
  }
  // since handles position has changed outline bounds must be recomputed
  UpdateOutlineBounds();
}

//----------------------------------------------------------------------------
void mafGizmoROI::UpdateOutlineBounds()
//----------------------------------------------------------------------------
{
	double pos[3] = {0, 0, 0};
	
  double pos1_new[3] = {0, 0, 0};
	double pos2_new[3] = {0, 0, 0};
	double b[6],center[3];

	OutlineGizmo->GetBounds(b);

	mafTransform::GetPosition(*OutlineGizmo->GetPose(), pos2_new);
	mafTransform::GetPosition(*GHandle[ActiveGizmoComponent]->GetPose(), pos1_new);
	GHandle[ActiveGizmoComponent]->GetHandleCenter(ActiveGizmoComponent,center);
	int i;
	if(ActiveGizmoComponent==0||ActiveGizmoComponent==1)
		i=0;
	else if(ActiveGizmoComponent==2||ActiveGizmoComponent==3)
		i=1;
	else if(ActiveGizmoComponent==4||ActiveGizmoComponent==5)
		i=2;
	pos1_new[i]-=pos2_new[i];
	pos1_new[i]+=center[i];
	b[ActiveGizmoComponent]=pos1_new[i];

  OutlineGizmo->SetBounds(b);  
}

//----------------------------------------------------------------------------
void mafGizmoROI::GetBounds(double bounds[6])
//----------------------------------------------------------------------------
{
  OutlineGizmo->GetBounds(bounds);
}

//----------------------------------------------------------------------------
void mafGizmoROI::SetBounds(double bounds[6])
//----------------------------------------------------------------------------
{
	double newBounds[6] = {bounds[0], bounds[1], bounds[2], bounds[3], bounds[4], bounds[5]}; 
	if(m_ConstraintModality==mafGizmoHandle::BOUNDS)
	{
		InputVME->GetOutput()->Update();
		// check for bounds 
		double vmeLocBounds[6];
		InputVME->GetOutput()->GetVTKData()->GetBounds(vmeLocBounds);

		// new bounds must be internal do vme bounds
		int i;
		for (i = 0; i < 6; i++)
		{  
			if (i % 2 == 0)
			{
				if (newBounds[i] < vmeLocBounds[i])     newBounds[i]    = vmeLocBounds[i];
				if (newBounds[i] > vmeLocBounds[i+1])   newBounds[i]    = vmeLocBounds[i+1];
				if (newBounds[i+1] < vmeLocBounds[i])   newBounds[i+1]  = vmeLocBounds[i];
				if (newBounds[i+1] > vmeLocBounds[i+1]) newBounds[i+1]  = vmeLocBounds[i+1];
			}
		}
	}

  // set box gizmo bounds
  OutlineGizmo->SetBounds(newBounds);

  // set new gizmo positions
  // calculate bb centers
  // set the positions
  
  for (int i = 0; i < 6; i++)
  {
    GHandle[i]->SetBBCenters(newBounds);
  }
}

//----------------------------------------------------------------------------
void mafGizmoROI::UpdateGizmosLength()
//----------------------------------------------------------------------------
{
  double bounds[6];

  // get the minimum outline dimension
  OutlineGizmo->GetBounds(bounds);

  vector<double> dim;
  dim.push_back(bounds[1] - bounds[0]);
  dim.push_back(bounds[3] - bounds[2]);
  dim.push_back(bounds[5] - bounds[4]);

  vector<double>::iterator result;
  result = min_element(dim.begin(), dim.end());

  double min_dim = *result;
  for (int i = 0; i <6; i++)
  {
    GHandle[i]->SetLength(min_dim/12);
  }
}

//----------------------------------------------------------------------------
void mafGizmoROI::Reset()
//----------------------------------------------------------------------------
{
  double b[6];
  InputVME->GetOutput()->GetBounds(b);
  SetBounds(b);
  UpdateGizmosLength();
}