/*=========================================================================

 Program: MAF2Medical
 Module: mafGizmoROI_BES
 Authors: Stefano Perticoni, Josef Kohout
 
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


#include "mafGizmoROI_BES.h"
#include "mafDecl.h"
#include "mafGizmoBoundingBox.h"
#include "mafGizmoHandle.h"
#include "mafInteractorGenericMouse.h"
#include "mafGizmoHandle.h"

#include "mafMatrix.h"
#include "mafTransform.h"
#include "mafVME.h"
#include "mafVMEOutput.h"
#include "mafSmartPointer.h"
#include "mafGizmoTranslatePlane.h"
#include "mafGizmoTranslateAxis.h"
#include "mafObserver.h"
#include "mafGizmoBoundingBox.h"

#include "vtkTransform.h"
#include "vtkCubeSource.h"
#include "vtkDataSet.h"
#include "vtkTransform.h"

#include <vector>
#include <algorithm>

using namespace std;

//----------------------------------------------------------------------------
mafGizmoROI_BES::mafGizmoROI_BES(mafVME* input, mafObserver *listener , int constraintModality,
							 mafVME* parent, double* usrBounds)
//----------------------------------------------------------------------------
{
	assert(input);
	m_InputVME = input;
	m_Listener = listener;

	for (int i = 0; i < 6; i++) {
		m_UserBounds[i] = 0;
	}

	SetConstraintModality(constraintModality, usrBounds);
	
	//no gizmo component is active at construction
	this->m_ActiveGizmoComponent = -1;
	this->SetModalityToLocal();

	for (int i = 0; i < 6; i++)
	{
		// Create mafGizmoHandle and send events to this
		m_GHandle[i] = new mafGizmoHandle(input, this, 
			(constraintModality == mafGizmoROI_BES::VTK_OUTPUT_BOUNDS ?
			mafGizmoHandle::BOUNDS : mafGizmoHandle::FREE
			),parent);		
		m_GHandle[i]->SetType(i);		
	}

	//create axis and planes
	for (int i = 0; i < 3; i++)
	{
		m_GAxis[i] = new mafGizmoTranslateAxis(input, this);	
		m_GAxis[i]->SetAxis(i);
    m_GAxis[i]->Show(false);  //default is off (because of backward compatibility)

		m_GPlane[i] = new mafGizmoTranslatePlane(input, this);
		m_GPlane[i]->SetPlane(i);
		m_GPlane[i]->Show(true);	//default is off
	}

	// create the outline gizmo
	m_OutlineGizmo = new mafGizmoBoundingBox(input, this,parent);		
}
//----------------------------------------------------------------------------
mafGizmoROI_BES::~mafGizmoROI_BES() 
//----------------------------------------------------------------------------
{
	//Destroy:
	//6 mafGizmoHandle, 3 mafGizmoTranslateAxis and 3 mafGizmoTranslatePlane

	int i = 0;
	for (; i < 3; i++)
	{    
		cppDEL(m_GPlane[i]);
		cppDEL(m_GAxis[i]);
		cppDEL(m_GHandle[i]);
	}

	for (; i < 6; i++) {
		cppDEL(m_GHandle[i]);
	}

	// the gizmo outline
	cppDEL(m_OutlineGizmo);
}

//modifies the constraint modality
void mafGizmoROI_BES::SetConstraintModality(int constraintModality, double* usrBounds/* = NULL*/)
{
	assert(m_ConstraintModality != mafGizmoROI_BES::USER_BOUNDS || usrBounds != NULL);

	m_ConstraintModality = constraintModality;	
	if (usrBounds != NULL)
	{
		for (int i = 0; i < 6; i++) {
			m_UserBounds[i] = usrBounds[i];
		}
	}
}

//returns ID of gizmo component denoted by the reference in sender
//-1 is returned if sender is not a gizmo component
int mafGizmoROI_BES::FindGizmoComponent(void* sender)
{
	for (int i = 0; i < 6; i++)	
	{
		if (sender == m_GHandle[i])
			return i + HANDLE_XMIN;
	}

	for (int i = 0; i < 3; i++)	
	{
		if (sender == m_GAxis[i])
			return i + AXIS_X;

		if (sender == m_GPlane[i])
			return i + PLANE_XN;
	}

	return -1;
}

//----------------------------------------------------------------------------
void mafGizmoROI_BES::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	// get the sender
	void *sender = maf_event->GetSender();	
	if (FindGizmoComponent(sender) >= 0)
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
void mafGizmoROI_BES::OnEventGizmoComponents(mafEventBase *maf_event)
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
				if (arg == mafInteractorGenericMouse::MOUSE_DOWN)
				{
					int component = FindGizmoComponent(sender);

					this->Highlight(component);
					m_ActiveGizmoComponent = component;					
				}
				else if (arg == mafInteractorGenericMouse::MOUSE_MOVE)
				{     
					 mafSmartPointer<mafMatrix> tr;
					if (this->m_Modality == G_LOCAL)
					{
						// local mode
						int gtype = GetActiveGizmoComponentType();
						int gindex = GetActiveGizmoComponentIndex();

						mafMatrix* oldMatrix;
						if (gtype == GHANDLE)
							oldMatrix = m_GHandle[gindex]->GetPose();
						else if (gtype == GAXIS)
							oldMatrix = m_GAxis[gindex]->GetAbsPose();
						else
							oldMatrix = m_GPlane[gindex]->GetAbsPose();

						vtkTransform *currTr = vtkTransform::New();
						currTr->PostMultiply();
						currTr->SetMatrix(oldMatrix->GetVTKMatrix());
						currTr->Concatenate(e->GetMatrix()->GetVTKMatrix());
						currTr->Update();

						//new transformation
						tr->DeepCopy(currTr->GetMatrix());
						tr->SetTimeStamp(m_InputVME->GetTimeStamp());

						if (gtype == GHANDLE)
							m_GHandle[gindex]->SetPose(tr);
						else if (gtype == GAXIS)
							m_GAxis[gindex]->SetAbsPose(tr);
						else
							m_GPlane[gindex]->SetAbsPose(tr);
						currTr->Delete();
						
						//update other gizmos
						UpdateGizmos();											
					}          
					else
					{
						// global mode
						// not yet implemented...						
					}
				}
				else if (arg == mafInteractorGenericMouse::MOUSE_UP)
				{
				}

				mafEventMacro(mafEvent(this, CAMERA_UPDATE));
				
				// forward isa transform events to the listener op
				// instanciating the gizmo; the sender is changed to "this" so that the operation can check for
				// gizmo sending events
				e->SetSender(this);
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
void mafGizmoROI_BES::Highlight (int component) 
//----------------------------------------------------------------------------
{				
	for (int i = 0; i < 6; i++ ){
		m_GHandle[i]->Highlight(i + HANDLE_XMIN == component);
	}

	for (int i = 0; i < 3; i++ )
	{
		m_GAxis[i]->Highlight(i + AXIS_X == component);
		m_GPlane[i]->Highlight(i + PLANE_XN == component);
	}	
}

//----------------------------------------------------------------------------
void mafGizmoROI_BES::HighlightOff () 
//----------------------------------------------------------------------------
{
	for (int i = 0; i < 6; i++ ){
		m_GHandle[i]->Highlight(false);
	}

	for (int i = 0; i < 3; i++ )
	{
		m_GAxis[i]->Highlight(false);
		m_GPlane[i]->Highlight(false);
	}	
}

//----------------------------------------------------------------------------  
void mafGizmoROI_BES::Show(bool show)
//----------------------------------------------------------------------------
{
	ShowAxis(show);
	ShowHandles(show);
  ShowPlanes(show);
	ShowROI(show);
}

//----------------------------------------------------------------------------  
void mafGizmoROI_BES::ShowHandles(bool show)
//----------------------------------------------------------------------------  
{
	for (int i = 0; i < 6; i++ ){
		m_GHandle[i]->Show(show);
	}
}

//----------------------------------------------------------------------------  
void mafGizmoROI_BES::ShowAxis(bool show)
//----------------------------------------------------------------------------  
{
	for (int i = 0; i < 3; i++ ){
		m_GAxis[i]->Show(show);
	}
}

//----------------------------------------------------------------------------  
void mafGizmoROI_BES::ShowPlanes(bool show)
//----------------------------------------------------------------------------  
{
	for (int i = 0; i < 3; i++ ){
		m_GPlane[i]->Show(show);
	}
}

//----------------------------------------------------------------------------  
void mafGizmoROI_BES::ShowROI(bool show)
//----------------------------------------------------------------------------  
{
	m_OutlineGizmo->Show(show);
}

//----------------------------------------------------------------------------  
void mafGizmoROI_BES::SetConstrainRefSys(mafMatrix *constrain)
//----------------------------------------------------------------------------
{
	for (int i = 0; i < 6; i++ ){
		m_GHandle[i]->SetConstrainRefSys(constrain);
	}

	//axis and planes has no constraints ?!
}

//----------------------------------------------------------------------------  
void mafGizmoROI_BES::SetInput(mafVME *input)
//----------------------------------------------------------------------------
{
	this->m_InputVME = input;
	for (int i = 0; i < 6; i++){
		m_GHandle[i]->SetInput(input);
	}

	for (int i = 0; i < 3; i++)
	{
		m_GAxis[i]->SetInput(input);
		m_GPlane[i]->SetInput(input);
	}

	m_OutlineGizmo->SetInput(input);
}


//Updates the position of all gizmos according to the current 
//position of active gizmo component (m_ActiveComponent)
//NB: the position of active gizmo component may be changed here as well
/*virtual*/ void mafGizmoROI_BES::UpdateGizmos()
{
	int gtype = GetActiveGizmoComponentType();
	int gindex = GetActiveGizmoComponentIndex();

	//if (gtype == GHANDLE)
	//{
	//	// update other gizmos positions in order to recenter them
	//	UpdateHandlePositions();

	//	//update translation gizmos (axis and planes) in order to recenter them
	//	UpdateTranslationGizmos();

	//	// Update the gizmo length in order to be a fixed fraction
	//	// of the outline gizmo dimension
	//	UpdateGizmosLength();
	//	return;	//we are ready here
	//}
	
	double bounds[6], lengths[3], deltas[3], oldPos[3];
	this->GetBounds(bounds);

	for (int i = 0; i < 3; i++) 
	{
		lengths[i] = bounds[2*i + 1] - bounds[2*i];
		deltas[i] = 0.0;
	}
	
	int gidx2 = 2*gindex;
	if (gtype == GHANDLE)
	{
		//one of six handles moved, compute the distance it moved
		mafTransform::GetPosition(*m_GHandle[gindex]->GetPose(), oldPos);		
		bounds[gindex] += oldPos[gindex / 2];		
	}
	else 
	{
		if (gtype == GAXIS)
		{
			//AXIS_X has moved => compute the distance it moved		
			mafTransform::GetPosition(*m_GAxis[gindex]->GetAbsPose(), oldPos);
			deltas[gindex] = oldPos[gindex] - ((bounds[gidx2] + bounds[gidx2 + 1]) / 2);
		}
		else
		{
			mafTransform::GetPosition(*m_GPlane[gindex]->GetAbsPose(), oldPos);
			if (gindex > 0) //XY plane or XZ plane
				deltas[0] = oldPos[0] - ((bounds[0] + bounds[1]) / 2);	//shares x

			if (gindex != 1) //XY plane or YZ plane
				deltas[1] = oldPos[1] - ((bounds[2] + bounds[3]) / 2);	//shares y

			if (gindex < 2) //XZ plane or YZ plane
				deltas[2] = oldPos[2] - ((bounds[4] + bounds[5]) / 2);	//shares z
		}

		//compute new bounds
		for (int i = 0; i < 3; i++)
		{
			bounds[2*i] += deltas[i];
			bounds[2*i + 1] += deltas[i];
		}
	}

	//check if we new bounds are within valid limits
	if (m_ConstraintModality != mafGizmoROI_BES::FREE)
	{
		//we are limited to stay within a bound
		double vmeLocBounds[6];
		
		if (m_ConstraintModality == mafGizmoROI_BES::USER_BOUNDS)
			memcpy(vmeLocBounds, m_UserBounds, sizeof(vmeLocBounds));
		else
		{
			// check for VTK bounds	
			m_InputVME->GetOutput()->Update();
			m_InputVME->GetOutput()->GetVTKData()->GetBounds(vmeLocBounds);
		}

		// new bounds must be internal do vme bounds
		for (int i = 0; i < 3; i++)
		{
			int i2 = 2*i;
			if (bounds[i2] < vmeLocBounds[i2])
			{
				bounds[i2] = vmeLocBounds[i2];
				
				if (gtype != GHANDLE)
					bounds[i2 + 1] = bounds[i2] + lengths[i];
			}
			else if (bounds[i2 + 1] > vmeLocBounds[i2 + 1])
			{
				bounds[i2 + 1] = vmeLocBounds[i2 + 1];
				
				if (gtype != GHANDLE)
					bounds[i2] = bounds[i2 + 1] - lengths[i];
			}
		}		
	}

	this->SetBounds(bounds);	//it does everything	
}
//----------------------------------------------------------------------------  
void mafGizmoROI_BES::UpdateHandlePositions()
//----------------------------------------------------------------------------
{
	if (GetActiveGizmoComponentType() != GHANDLE)
		return;	//invalid call

	double posMin[3] = {0, 0, 0};
	double posMax[3] = {0, 0, 0};
	double oldPos[3] = {0, 0, 0};
	
	// move 0 or 1 => recenter 2, 3, 4, 5
	if (m_ActiveGizmoComponent == HANDLE_XMIN || m_ActiveGizmoComponent == HANDLE_XMAX)
	{
		mafTransform::GetPosition(*m_GHandle[0]->GetPose(), posMin);
		mafTransform::GetPosition(*m_GHandle[1]->GetPose(), posMax);

		double xMean = (posMin[0] + posMax[0]) / 2;
		for (int i = 2; i < 6; i++)
		{      
			mafTransform::GetPosition(*m_GHandle[i]->GetPose(), oldPos);
			oldPos[0] = xMean;
			mafTransform::SetPosition(*m_GHandle[i]->GetPose(), oldPos);
			mafTransform::SetPosition(m_GHandle[i]->GetPivotMatrix(), oldPos);
			//Matteo 23-08-06
			mafSmartPointer<mafTransform> tr;
			tr->SetMatrix(*m_GHandle[i]->GetPose());

			mafMatrix mat;
			mat.DeepCopy(tr->GetMatrixPointer());
			mat.SetTimeStamp(m_InputVME->GetTimeStamp());

			m_GHandle[i]->SetPose(&mat);
			//End Matteo
		}
	}
	// move 2 or 3 => recenter 0, 1, 4, 5
	else if (m_ActiveGizmoComponent == HANDLE_YMIN || m_ActiveGizmoComponent == HANDLE_YMAX)
	{
		mafTransform::GetPosition(*m_GHandle[2]->GetPose(), posMin);
		mafTransform::GetPosition(*m_GHandle[3]->GetPose(), posMax);

		double yMean = (posMin[1] + posMax[1]) / 2;

		for (int i = 0; i < 6; i++)
		{      
			if (i != 2 && i != 3)
			{      
				mafTransform::GetPosition(*m_GHandle[i]->GetPose(), oldPos);
				oldPos[1] = yMean;
				mafTransform::SetPosition(*m_GHandle[i]->GetPose(), oldPos);
				mafTransform::SetPosition(m_GHandle[i]->GetPivotMatrix(), oldPos);
				//Matteo 23-08-06
				mafSmartPointer<mafTransform> tr;
				tr->SetMatrix(*m_GHandle[i]->GetPose());

				mafMatrix mat;
				mat.DeepCopy(tr->GetMatrixPointer());
				mat.SetTimeStamp(m_InputVME->GetTimeStamp());

				m_GHandle[i]->SetPose(&mat);
				//End Matteo
			}
		}
	}
	// move 4 or 5 => recenter 0, 1, 2, 3
	else if (m_ActiveGizmoComponent == HANDLE_ZMIN || m_ActiveGizmoComponent == HANDLE_ZMAX)
	{
		mafTransform::GetPosition(*m_GHandle[4]->GetPose(), posMin);
		mafTransform::GetPosition(*m_GHandle[5]->GetPose(), posMax);

		double zMean = (posMin[2] + posMax[2]) / 2;

		for (int i = 0; i < 4; i++)
		{      
			mafTransform::GetPosition(*m_GHandle[i]->GetPose(), oldPos);
			oldPos[2] = zMean;
			mafTransform::SetPosition(*m_GHandle[i]->GetPose(), oldPos);
			mafTransform::SetPosition(m_GHandle[i]->GetPivotMatrix(), oldPos);
			//Matteo 23-08-06
			mafSmartPointer<mafTransform> tr;
			tr->SetMatrix(*m_GHandle[i]->GetPose());

			mafMatrix mat;
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
void mafGizmoROI_BES::UpdateOutlineBounds()
//----------------------------------------------------------------------------
{
	if (GetActiveGizmoComponentType() != GHANDLE)
		return;	//axis or planes are not supported

	double pos[3] = {0, 0, 0};

	double pos1_new[3] = {0, 0, 0};
	double pos2_new[3] = {0, 0, 0};
	double b[6],center[3];

	m_OutlineGizmo->GetBounds(b);

	mafTransform::GetPosition(*m_OutlineGizmo->GetPose(), pos2_new);
	mafTransform::GetPosition(*m_GHandle[m_ActiveGizmoComponent]->GetPose(), pos1_new);
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
}

//----------------------------------------------------------------------------
void mafGizmoROI_BES::GetBounds(double bounds[6])
//----------------------------------------------------------------------------
{
	m_OutlineGizmo->GetBounds(bounds);
}

//----------------------------------------------------------------------------
void mafGizmoROI_BES::SetBounds(double bounds[6])
//----------------------------------------------------------------------------
{
	double newBounds[6] = {bounds[0], bounds[1], bounds[2], bounds[3], bounds[4], bounds[5]}; 
	if (m_ConstraintModality != mafGizmoROI_BES::FREE)
	{
		double vmeLocBounds[6];
		
		if (m_ConstraintModality == mafGizmoROI_BES::USER_BOUNDS)
			memcpy(vmeLocBounds, m_UserBounds, sizeof(vmeLocBounds));
		else
		{
			// check for bounds 		
			m_InputVME->GetOutput()->Update();
			m_InputVME->GetOutput()->GetVTKData()->GetBounds(vmeLocBounds);
		}

		// new bounds must be internal do vme bounds
		int i;
		for (i = 0; i < 6; i += 2)
		{
			if (newBounds[i] < vmeLocBounds[i])     newBounds[i]    = vmeLocBounds[i];
			if (newBounds[i] > vmeLocBounds[i+1])   newBounds[i]    = vmeLocBounds[i+1];
			if (newBounds[i+1] < vmeLocBounds[i])   newBounds[i+1]  = vmeLocBounds[i];
			if (newBounds[i+1] > vmeLocBounds[i+1]) newBounds[i+1]  = vmeLocBounds[i+1];		
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
	}

	// set new axis positions
	UpdateTranslationGizmos();
	UpdateGizmosLength();
}

//updates the position of all translation gizmos so it is in the centre
//of the current bounding box (ROI)
void mafGizmoROI_BES::UpdateTranslationGizmos()
{
	double newBounds[6];
	m_OutlineGizmo->GetBounds(newBounds);

	double posAxis[3];
	posAxis[0] = (newBounds[0] + newBounds[1]) / 2;
	posAxis[1] = (newBounds[2] + newBounds[3]) / 2;
	posAxis[2] = (newBounds[4] + newBounds[5]) / 2;

	for (int i = 0; i < 3; i++)
	{
		mafMatrix* matAxis = m_GAxis[i]->GetAbsPose();
		mafTransform::SetPosition(*matAxis, posAxis);
		m_GAxis[i]->SetAbsPose(matAxis);

		matAxis = m_GPlane[i]->GetAbsPose();
		mafTransform::SetPosition(*matAxis, posAxis);
		m_GPlane[i]->SetAbsPose(matAxis);
	}	
}

//----------------------------------------------------------------------------
void mafGizmoROI_BES::UpdateGizmosLength()
//----------------------------------------------------------------------------
{
	double bounds[6];

	// get the minimum outline dimension
	m_OutlineGizmo->GetBounds(bounds);

	vector<double> dim;
	dim.push_back(bounds[1] - bounds[0]);
	dim.push_back(bounds[3] - bounds[2]);
	dim.push_back(bounds[5] - bounds[4]);

	vector<double>::iterator result;
	result = min_element(dim.begin(), dim.end());

	double min_dim = *result;
	for (int i = 0; i < 6; i++) {
		m_GHandle[i]->SetLength(min_dim/12);
	}

	for (int i = 0; i < 3; i++) 
	{
		double dblLen = (bounds[2*i + 1] - bounds[2*i]);
		m_GAxis[i]->SetCylinderLength(dblLen / 6);
		m_GAxis[i]->SetConeLength(dblLen / 4);
	}

	for (int i = 0; i < 3; i++)
	{
		double dblLen = (bounds[2*i + 1] - bounds[2*i]);
		double dblLen2 = (bounds[(2*i + 3) % 6] - bounds[(2*i + 2) % 6]);
		if (dblLen2 < dblLen)
			dblLen = dblLen2;

		m_GPlane[(i + 2) % 3]->SetSizeLength(dblLen / 6);
	}
}

//----------------------------------------------------------------------------
void mafGizmoROI_BES::Reset()
//----------------------------------------------------------------------------
{
	double b[6];
	m_InputVME->GetOutput()->GetBounds(b);
	SetBounds(b);
	UpdateGizmosLength();
}