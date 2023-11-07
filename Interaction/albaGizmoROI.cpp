/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoROI
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


#include "albaGizmoROI.h"
#include "albaDecl.h"
#include "albaGizmoBoundingBox.h"
#include "albaGizmoHandle.h"
#include "albaInteractorGenericMouse.h"
#include "albaGizmoHandle.h"

#include "albaMatrix.h"
#include "albaTransform.h"
#include "albaVME.h"
#include "albaVMEOutput.h"
#include "albaSmartPointer.h"

#include "vtkTransform.h"
#include "vtkCubeSource.h"
#include "vtkDataSet.h"
#include "vtkMath.h"

#include <vector>
#include <algorithm>

using namespace std;


//----------------------------------------------------------------------------
albaGizmoROI::albaGizmoROI(albaVME *input, albaObserver* listener /* = NULL  */, int constraintModality/* =albaGizmoHandle::BOUNDS */,albaVME* parent/* =NULL */,bool showShadingPlane/* =false */)
//----------------------------------------------------------------------------
{
  assert(input);
  m_InputVME = input;
  m_Listener = listener;
	m_ConstraintModality = constraintModality;
  //no gizmo component is active at construction
  m_MinimumHandleSize = 0.0;
  m_EnableMinimumHandleSize = false;
  m_Accumulator = 0.;
  
  this->m_ActiveGizmoComponent = 0;
  this->SetModalityToLocal();

  for (int i = 0; i < 6; i++)
  {
    // Create albaGizmoHandle and send events to this
    m_GHandle[i] = NULL;
    m_GHandle[i] = new albaGizmoHandle(input, this, constraintModality,parent,showShadingPlane);
    m_GHandle[i]->SetType(i);
    m_GHandle[i]->GetHandleCenter(i,m_Center[i]);
  } 
	// create the outline gizmo
	m_OutlineGizmo = new albaGizmoBoundingBox(input, this,parent);

  UpdateOutlineBounds();
	UpdateGizmosLength();
}
//----------------------------------------------------------------------------
albaGizmoROI::~albaGizmoROI() 
//----------------------------------------------------------------------------
{
  //Destroy:
  //6 albaGizmoHandle 
  
  for (int i = 0; i < 6; i++)
  {    
    cppDEL(m_GHandle[i]);
  }

  // the gizmo outline
  cppDEL(m_OutlineGizmo);
}
//----------------------------------------------------------------------------
void albaGizmoROI::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  // get the sender
  void *sender = alba_event->GetSender();
    
  if (sender == m_GHandle[0] || sender == m_GHandle[1] || sender == m_GHandle[2] || sender == m_GHandle[3] || sender == m_GHandle[4] || sender == m_GHandle[5])
  {
    OnEventGizmoComponents(alba_event); // process events from gizmo handles
  }
  else
  {
    // forward event to the listener
    albaEventMacro(*alba_event);
  }
}

//----------------------------------------------------------------------------
void albaGizmoROI::OnEventGizmoComponents(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
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
        if (arg == albaInteractorGenericMouse::MOUSE_DOWN)
        {
          m_Accumulator = 0.;
          if (sender == m_GHandle[0])
          {
            this->Highlight(0);
            m_ActiveGizmoComponent = 0;
          }
          else if (sender == m_GHandle[1])
          {
            this->Highlight(1);
            m_ActiveGizmoComponent = 1;
          }
          else if (sender == m_GHandle[2])
          {
            this->Highlight(2);
            m_ActiveGizmoComponent = 2;
          }
          else if (sender == m_GHandle[3])
          {
            this->Highlight(3);
            m_ActiveGizmoComponent = 3;
          }
          else if (sender == m_GHandle[4])
          {
            this->Highlight(4);
            m_ActiveGizmoComponent = 4;
          }
          else if (sender == m_GHandle[5])
          {
            this->Highlight(5);
            m_ActiveGizmoComponent = 5;
          }
        }
        else if (arg == albaInteractorGenericMouse::MOUSE_MOVE)
        {     
          if (this->m_Modality == G_LOCAL)
          {
            // local mode

						double distance;
				    double actualBounds[6];
						double currentDistance;
            short component;
						m_OutlineGizmo->GetBounds(actualBounds);
            
            if(m_EnableMinimumHandleSize == true)
            {
						  if(m_ActiveGizmoComponent == 0 || m_ActiveGizmoComponent == 1)
						  {
							  component = m_ActiveGizmoComponent == 0 ? -1 : 1;
							  distance = (*e->GetMatrix()).GetElement(0,3) * component;
							  currentDistance = actualBounds[1]-actualBounds[0];
  							

                //stop it
							  if(m_MinimumHandleSize >= currentDistance+distance)
							  {	
                  double surplusDistance = distance;
  								
                  //albaLogMessage(albaString::Format("IN-VERSUS%.2f", acc));
                  distance = m_MinimumHandleSize - currentDistance;

                  surplusDistance -= distance;
                  m_Accumulator += surplusDistance;

								  distance *= component;
								  (*e->GetMatrix()).SetElement(0,3, distance);

                  
							  }
  							
                if(distance > 0)
                {
                  m_Accumulator += distance;
                  if(m_Accumulator < 0)
                  {
                    return;
                  }
                  else
                  {
                    m_Accumulator =.0;
                  }
                }
                /*else //only for log
                {
                  albaLogMessage(albaString::Format("distance%.2f", distance));
                  albaLogMessage(albaString::Format("acc%.2f", m_Accumulator));
                }*/

  							
						  }
						  else if(m_ActiveGizmoComponent == 2 || m_ActiveGizmoComponent == 3)
						  {
							  component = m_ActiveGizmoComponent == 2 ? -1 : 1;
							  distance = (*e->GetMatrix()).GetElement(1,3) * component;
							  currentDistance = actualBounds[3]-actualBounds[2];

                //stop it
                if(m_MinimumHandleSize >= currentDistance+distance)
                {	
                  double surplusDistance = distance;

                  //albaLogMessage(albaString::Format("IN-VERSUS%.2f", acc));
                  distance = m_MinimumHandleSize - currentDistance;

                  surplusDistance -= distance;
                  m_Accumulator += surplusDistance;

                  distance *= component;
                  (*e->GetMatrix()).SetElement(1,3, distance);


                }

                if(distance > 0)
                {
                  m_Accumulator += distance;
                  if(m_Accumulator < 0)
                  {
                    return;
                  }
                  else
                  {
                    m_Accumulator =.0;
                  }
                }
                /*else //only for log
                {
                albaLogMessage(albaString::Format("distance%.2f", distance));
                albaLogMessage(albaString::Format("acc%.2f", m_Accumulator));
                }*/
						  }
						  else if(m_ActiveGizmoComponent == 4 || m_ActiveGizmoComponent == 5)
						  {
							  component = m_ActiveGizmoComponent == 4 ? -1 : 1;
							  distance = (*e->GetMatrix()).GetElement(2,3) * component;
							  currentDistance = actualBounds[5]-actualBounds[4];

                //stop it
                if(m_MinimumHandleSize >= currentDistance+distance)
                {	
                  double surplusDistance = distance;

                  //albaLogMessage(albaString::Format("IN-VERSUS%.2f", acc));
                  distance = m_MinimumHandleSize - currentDistance;

                  surplusDistance -= distance;
                  m_Accumulator += surplusDistance;

                  distance *= component;
                  (*e->GetMatrix()).SetElement(2,3, distance);


                }

                if(distance > 0)
                {
                  m_Accumulator += distance;
                  if(m_Accumulator < 0)
                  {
                    return;
                  }
                  else
                  {
                    m_Accumulator =.0;
                  }
                }
                /*else //only for log
                {
                albaLogMessage(albaString::Format("distance%.2f", distance));
                albaLogMessage(albaString::Format("acc%.2f", m_Accumulator));
                }*/
						  }

            }

						// translate selected gizmo
						albaTransform *tr = albaTransform::New();                   
						tr->SetMatrix(*m_GHandle[m_ActiveGizmoComponent]->GetPose());
						tr->Concatenate(*e->GetMatrix(), PRE_MULTIPLY);

						albaMatrix mat;
						mat.DeepCopy(tr->GetMatrixPointer());
						mat.SetTimeStamp(m_InputVME->GetTimeStamp());

						//must set the matrix?

            m_GHandle[m_ActiveGizmoComponent]->SetPose(&mat);

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
        else if (arg == albaInteractorGenericMouse::MOUSE_UP)
        {
        }
				GetLogicManager()->CameraUpdate();
        // forward isa transform events to the listener op
        albaEventMacro(*e);
      }
      break;

      default:
      {
        albaEventMacro(*e);
      }
      break;
    }
  }
}

//----------------------------------------------------------------------------
void albaGizmoROI::Highlight (int component) 
//----------------------------------------------------------------------------
{
  m_GHandle[component]->Highlight(true);
  for (int i = 0; i < 6; i++ )
  {
    if (i != component)
    {
      m_GHandle[i]->Highlight(false);
    }
  }
}

//----------------------------------------------------------------------------
void albaGizmoROI::HighlightOff () 
//----------------------------------------------------------------------------
{
  for (int i = 0; i < 6; i++ )
  {
    m_GHandle[i]->Highlight(false);
  }
}

//----------------------------------------------------------------------------  
void albaGizmoROI::Show(bool show)
//----------------------------------------------------------------------------
{
  ShowHandles(show);
  ShowROI(show);
}

//----------------------------------------------------------------------------  
void albaGizmoROI::ShowHandles(bool show)
//----------------------------------------------------------------------------  
{
  for (int i = 0; i < 6; i++)
  {
    m_GHandle[i]->Show(show);
  }
}

//----------------------------------------------------------------------------  
void albaGizmoROI::ShowROI(bool show)
//----------------------------------------------------------------------------  
{
  m_OutlineGizmo->Show(show);
}
  
//----------------------------------------------------------------------------  
void albaGizmoROI::SetConstrainRefSys(albaMatrix *constrain)
//----------------------------------------------------------------------------
{
  for (int i =0; i < 6; i++)
  {
    m_GHandle[i]->SetConstrainRefSys(constrain);
  }
}

//----------------------------------------------------------------------------  
void albaGizmoROI::SetInput(albaVME *input)
//----------------------------------------------------------------------------
{
  this->m_InputVME = input;
  for (int i = 0; i < 6; i++)
  {
    m_GHandle[i]->SetInput(input);
  }

  m_OutlineGizmo->SetInput(input);
}

//----------------------------------------------------------------------------  
void albaGizmoROI::UpdateHandlePositions()
//----------------------------------------------------------------------------
{
  double posMin[3] = {0, 0, 0};
  double posMax[3] = {0, 0, 0};
  double oldPos[3] = {0, 0, 0};

  // move 0 or 1 => recenter 2, 3, 4, 5
  if (m_ActiveGizmoComponent == 0 || m_ActiveGizmoComponent == 1)
  {
    albaTransform::GetPosition(*m_GHandle[0]->GetPose(), posMin);
    albaTransform::GetPosition(*m_GHandle[1]->GetPose(), posMax);
    
    double xMean = (posMin[0] + posMax[0]) / 2;
    for (int i = 2; i < 6; i++)
    {      
      albaTransform::GetPosition(*m_GHandle[i]->GetPose(), oldPos);
      oldPos[0] = xMean;
      albaTransform::SetPosition(*m_GHandle[i]->GetPose(), oldPos);
      albaTransform::SetPosition(m_GHandle[i]->GetPivotMatrix(), oldPos);
			//Matteo 23-08-06
			albaSmartPointer<albaTransform> tr;
			tr->SetMatrix(*m_GHandle[i]->GetPose());

			albaMatrix mat;
			mat.DeepCopy(tr->GetMatrixPointer());
			mat.SetTimeStamp(m_InputVME->GetTimeStamp());

			m_GHandle[i]->SetPose(&mat);
			//End Matteo
    }
  }
  // move 2 or 3 => recenter 0, 1, 4, 5
  else if (m_ActiveGizmoComponent == 2 || m_ActiveGizmoComponent == 3)
  {
    albaTransform::GetPosition(*m_GHandle[2]->GetPose(), posMin);
    albaTransform::GetPosition(*m_GHandle[3]->GetPose(), posMax);
    
    double yMean = (posMin[1] + posMax[1]) / 2;

    for (int i = 0; i < 6; i++)
    {      
      if (i != 2 && i != 3)
      {      
        albaTransform::GetPosition(*m_GHandle[i]->GetPose(), oldPos);
        oldPos[1] = yMean;
        albaTransform::SetPosition(*m_GHandle[i]->GetPose(), oldPos);
        albaTransform::SetPosition(m_GHandle[i]->GetPivotMatrix(), oldPos);
				//Matteo 23-08-06
				albaSmartPointer<albaTransform> tr;
				tr->SetMatrix(*m_GHandle[i]->GetPose());

				albaMatrix mat;
				mat.DeepCopy(tr->GetMatrixPointer());
				mat.SetTimeStamp(m_InputVME->GetTimeStamp());

				m_GHandle[i]->SetPose(&mat);
				//End Matteo
      }
    }
  }
  // move 4 or 5 => recenter 0, 1, 2, 3
  else if (m_ActiveGizmoComponent == 4 || m_ActiveGizmoComponent == 5)
  {
    albaTransform::GetPosition(*m_GHandle[4]->GetPose(), posMin);
    albaTransform::GetPosition(*m_GHandle[5]->GetPose(), posMax);
    
    double zMean = (posMin[2] + posMax[2]) / 2;

    for (int i = 0; i < 4; i++)
    {      
      albaTransform::GetPosition(*m_GHandle[i]->GetPose(), oldPos);
      oldPos[2] = zMean;
      albaTransform::SetPosition(*m_GHandle[i]->GetPose(), oldPos);
      albaTransform::SetPosition(m_GHandle[i]->GetPivotMatrix(), oldPos);
			//Matteo 23-08-06
			albaSmartPointer<albaTransform> tr;
			tr->SetMatrix(*m_GHandle[i]->GetPose());

			albaMatrix mat;
			mat.DeepCopy(tr->GetMatrixPointer());
			mat.SetTimeStamp(m_InputVME->GetTimeStamp());

			m_GHandle[i]->SetPose(&mat);
			//End Matteo
    }
  }
  // since handles position has changed outline bounds must be recomputed
  UpdateOutlineBounds();
}
//----------------------------------------------------------------------------
void albaGizmoROI::ShowShadingPlane(bool show)
//----------------------------------------------------------------------------
{
    
  for (int i=0;i<6;i++)
  {
    m_GHandle[i]->ShowShadingPlane(show);
  }
}
//----------------------------------------------------------------------------
void albaGizmoROI::UpdateOutlineBounds()
//----------------------------------------------------------------------------
{
	double pos[3] = {0, 0, 0};
	
  double pos1_new[3] = {0, 0, 0};
	double pos2_new[3] = {0, 0, 0};
	double b[6],center[3];

	m_OutlineGizmo->GetBounds(b);

	albaTransform::GetPosition(*m_OutlineGizmo->GetPose(), pos2_new);
	albaTransform::GetPosition(*m_GHandle[m_ActiveGizmoComponent]->GetPose(), pos1_new);
	m_GHandle[m_ActiveGizmoComponent]->GetHandleCenter(m_ActiveGizmoComponent,center);
	int i;
	if(m_ActiveGizmoComponent==0||m_ActiveGizmoComponent==1)
		i=0;
	else if(m_ActiveGizmoComponent==2||m_ActiveGizmoComponent==3)
		i=1;
	else if(m_ActiveGizmoComponent==4||m_ActiveGizmoComponent==5)
		i=2;
	pos1_new[i]-=pos2_new[i];
	pos1_new[i]+=center[i];
	b[m_ActiveGizmoComponent]=pos1_new[i];

  m_OutlineGizmo->SetBounds(b);

  for (int i = 0;i<6;i++)
  {
    m_GHandle[i]->UpdateShadingPlaneDimension(b);
  }
}

//----------------------------------------------------------------------------
void albaGizmoROI::GetBounds(double bounds[6])
//----------------------------------------------------------------------------
{
  m_OutlineGizmo->GetBounds(bounds);
}

//----------------------------------------------------------------------------
void albaGizmoROI::SetBounds(double bounds[6])
//----------------------------------------------------------------------------
{
	double newBounds[6] = {bounds[0], bounds[1], bounds[2], bounds[3], bounds[4], bounds[5]}; 
	if(m_ConstraintModality==albaGizmoHandle::BOUNDS)
	{
		m_InputVME->GetOutput()->Update();
		// check for bounds 
		double vmeLocBounds[6];
		m_InputVME->GetOutput()->GetVTKData()->GetBounds(vmeLocBounds);

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
  m_OutlineGizmo->SetBounds(newBounds);

  // set new gizmo positions
  // calculate bb centers
  // set the positions
  
  for (int i = 0; i < 6; i++)
  {
    m_GHandle[i]->SetBBCenters(newBounds);
    m_GHandle[i]->UpdateShadingPlaneDimension(newBounds);
  }
}

//----------------------------------------------------------------------------
void albaGizmoROI::UpdateGizmosLength()
//----------------------------------------------------------------------------
{
  double bounds[6];

  // get the minimum outline dimension
  m_OutlineGizmo->GetBounds(bounds);

  vector<double> dim;
  dim.push_back(bounds[1] - bounds[0]);
  dim.push_back(bounds[3] - bounds[2]);
  dim.push_back(bounds[5] - bounds[4]);

  vector<double>::iterator min;
  min = min_element(dim.begin(), dim.end());
  vector<double>::iterator max;
  max = max_element(dim.begin(), dim.end());

  std::sort(dim.begin(),dim.end());
  double med_dim=dim[1];

  double min_dim = dim[0];
  
  for (int i = 0; i <6; i++)
  {
    if(med_dim/12.<(3./4.*min_dim))
    {
      double value = med_dim/12.;
      if(m_EnableMinimumHandleSize)
      {
        if(m_MinimumHandleSize > med_dim/12.)
        {
          value = m_MinimumHandleSize;
        }
      }
      m_GHandle[i]->SetLength(value);
    }
    else
    {
      double value = 3./4.*min_dim;
      if(m_EnableMinimumHandleSize)
      {
        if(m_MinimumHandleSize > 3./4.*min_dim)
        {
          value = m_MinimumHandleSize;
        }
      }

      m_GHandle[i]->SetLength(value);
    }
  }
}

//----------------------------------------------------------------------------
void albaGizmoROI::Reset()
//----------------------------------------------------------------------------
{
  double b[6];
	m_InputVME->GetOutput()->GetVMELocalBounds(b);
  SetBounds(b);
  UpdateGizmosLength();
}
