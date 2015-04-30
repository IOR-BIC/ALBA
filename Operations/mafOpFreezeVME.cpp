/*=========================================================================

Program: MAF2
Module: mafOpFreezeVME
Authors: Daniele Giunchi , Stefano Perticoni

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

#include "mafOpFreezeVME.h"
#include "wx/busyinfo.h"

#include "mafDecl.h"
#include "mafVME.h"
#include "mafSmartPointer.h"

#include "mafVMEGenericAbstract.h"
#include "mafVMERoot.h"
#include "mafVMERefSys.h"
#include "mafVMEVolume.h"

#include "mafVMESurface.h"
#include "mafVMEPolyline.h"
#include "mafVMEPolylineSpline.h"
#include "mafVMESurfaceParametric.h"
#include "mafVMEMeter.h"
#include "mafVMESlicer.h"
#include "mafVMEProber.h"
#include "mafVMEWrappedMeter.h"
#include "mafVMELabeledVolume.h"

#include "vtkRectilinearGrid.h"
#include "mmaMaterial.h"
#include "mmaVolumeMaterial.h"
#include "mafGUI.h"
#include "mafAbsMatrixPipe.h"

#include "vtkPolyData.h"
#include "vtkImageData.h"
#include "mafVMEComputeWrapping.H"



//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpFreezeVME);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpFreezeVME::mafOpFreezeVME(wxString label) :
mafOp(label)
	//----------------------------------------------------------------------------
{
	m_OpType  = OPTYPE_OP;
	m_Canundo = true;
	m_InputPreserving = true;

}
//----------------------------------------------------------------------------
mafOpFreezeVME::~mafOpFreezeVME( ) 
	//----------------------------------------------------------------------------
{

}
//----------------------------------------------------------------------------
bool mafOpFreezeVME::Accept(mafNode *node)
	//----------------------------------------------------------------------------
{
	return (node->IsMAFType(mafVME) && 
		!node->IsMAFType(mafVMEGenericAbstract) && 
		!node->IsMAFType(mafVMERoot) &&
		//!node->IsMAFType(mafVMERefSys) &&
		!node->IsMAFType(mafVMEVolume) ); //return if is a procedural vme
}
//----------------------------------------------------------------------------
mafOp* mafOpFreezeVME::Copy()   
	//----------------------------------------------------------------------------
{
	mafOpFreezeVME *cp = new mafOpFreezeVME(m_Label);
	return cp;
}
//----------------------------------------------------------------------------
void mafOpFreezeVME::OpRun()   
	//----------------------------------------------------------------------------
{
	if(!m_TestMode)
	{
		m_Gui = new mafGUI(this);
		ShowGui();
	}

	//control the output and create the right vme
	mafVME *vme = mafVME::SafeDownCast(m_Input);
	vme->Update();
	mafVMEOutput *output = vme->GetOutput();
	output->Update();

	if(vtkImageData *imageData = vtkImageData::SafeDownCast(output->GetVTKData()))
	{
		if(mafVMELabeledVolume *labeledVolume = mafVMELabeledVolume::SafeDownCast(vme))
		{
			mmaMaterial *material = (mmaMaterial *)labeledVolume->GetAttribute("MaterialAttributes");

			mafSmartPointer<mafVMEVolumeGray> newVolume;
			newVolume->SetName(labeledVolume->GetName());
			newVolume->SetData(imageData,labeledVolume->GetTimeStamp());
			newVolume->Update();

			if(material)
			{
				newVolume->GetMaterial()->DeepCopy(material);
				newVolume->GetMaterial()->UpdateProp();
			}

			newVolume->SetMatrix(*labeledVolume->GetOutput()->GetMatrix());
			m_Output=newVolume;
			if (m_Output)
			{
				m_Output->ReparentTo(m_Input->GetParent());
				if(!m_TestMode)
					OpStop(OP_RUN_OK);
			}
		}
	}
	else
		if(vtkRectilinearGrid *rectilinearGrid = vtkRectilinearGrid::SafeDownCast(output->GetVTKData()))
		{
			if(mafVMELabeledVolume *labeledVolume = mafVMELabeledVolume::SafeDownCast(vme))
			{
				mmaMaterial *material = (mmaMaterial *)labeledVolume->GetAttribute("MaterialAttributes");

				mafSmartPointer<mafVMEVolumeGray> newVolume;
				newVolume->SetName(labeledVolume->GetName());
				newVolume->SetData(rectilinearGrid,labeledVolume->GetTimeStamp());
				newVolume->Update();

				if(material)
				{
					newVolume->GetMaterial()->DeepCopy(material);
					newVolume->GetMaterial()->UpdateProp();
				}

				newVolume->SetMatrix(*labeledVolume->GetOutput()->GetMatrix());
				m_Output=newVolume;
				if (m_Output)
				{
					m_Output->ReparentTo(m_Input->GetParent());
					if(!m_TestMode)
						OpStop(OP_RUN_OK);
				}
			}
		}
		else
			if(vtkPolyData *polyData = vtkPolyData::SafeDownCast(output->GetVTKData()))
			{
				if(mafVMEPolylineSpline *vmeSpline = mafVMEPolylineSpline::SafeDownCast(vme))
				{
					mmaMaterial *material = vmeSpline->GetMaterial();

					mafSmartPointer<mafVMEPolyline> newPolyline;
					newPolyline->SetName(vmeSpline->GetName());
					newPolyline->SetData(polyData,vmeSpline->GetTimeStamp());
					newPolyline->Update();

					if(material)
					{
						newPolyline->GetMaterial()->DeepCopy(material);
						newPolyline->GetMaterial()->UpdateProp();
					}
					newPolyline->SetMatrix(*vmeSpline->GetOutput()->GetMatrix());
					m_Output=newPolyline;
					if (m_Output)
					{
						m_Output->ReparentTo(m_Input->GetParent());
						if(!m_TestMode)
							OpStop(OP_RUN_OK);
					}
				}
				else if(mafVMESurfaceParametric *vmeSurface = mafVMESurfaceParametric::SafeDownCast(vme))
				{
					mmaMaterial *material = vmeSurface->GetMaterial();

					mafSmartPointer<mafVMESurface> newSurface;
					newSurface->SetName(vmeSurface->GetName());
					newSurface->SetData(polyData,vmeSurface->GetTimeStamp());
					newSurface->Update();

					if(material)
					{
						newSurface->GetMaterial()->DeepCopy(material);
						newSurface->GetMaterial()->UpdateProp();
					}

					newSurface->SetMatrix(*vmeSurface->GetOutput()->GetMatrix());
					m_Output=newSurface;
					if (m_Output)
					{
						m_Output->ReparentTo(m_Input->GetParent());
						if(!m_TestMode)
							OpStop(OP_RUN_OK);
					}
				}
				else if(mafVMEMeter *meter = mafVMEMeter::SafeDownCast(vme))
				{
					mmaMaterial *material = meter->GetMaterial();

					mafSmartPointer<mafVMEPolyline> newPolyline;
					newPolyline->SetName(meter->GetName());
					newPolyline->SetData(polyData,meter->GetTimeStamp());
					newPolyline->Update();

					if(material)
					{
						newPolyline->GetMaterial()->DeepCopy(material);
						newPolyline->GetMaterial()->UpdateProp();
					}

					newPolyline->SetMatrix(*meter->GetOutput()->GetMatrix());
					m_Output=newPolyline;
					if (m_Output)
					{
						m_Output->ReparentTo(m_Input->GetParent());
						if(!m_TestMode)
							OpStop(OP_RUN_OK);
					}
				}
				else if(mafVMERefSys *refsys = mafVMERefSys::SafeDownCast(vme))
				{
					mmaMaterial *material = refsys->GetMaterial();

					mafSmartPointer<mafVMESurface> surface;
					surface->SetName(refsys->GetName());
					surface->SetData(polyData,refsys->GetTimeStamp());
					surface->Update();

					if(material)
					{
						surface->GetMaterial()->DeepCopy(material);
						surface->GetMaterial()->UpdateProp();
					}

					surface->SetMatrix(*refsys->GetOutput()->GetMatrix());
					m_Output=surface;
					if (m_Output)
					{
						m_Output->ReparentTo(m_Input->GetParent());
						if(!m_TestMode)
							OpStop(OP_RUN_OK);
					}
				}
				else if(mafVMESlicer *slicer = mafVMESlicer::SafeDownCast(vme))
				{
					mmaMaterial *material = slicer->GetMaterial();

					mafSmartPointer<mafVMESurface> newSurface;
					newSurface->SetName(slicer->GetName());
					newSurface->SetData(polyData,slicer->GetTimeStamp());
					vtkMAFSmartPointer<vtkImageData> text;
					text->DeepCopy(slicer->GetSurfaceOutput()->GetTexture());
					newSurface->GetSurfaceOutput()->SetTexture(text);
					newSurface->Update();

					if(material)
					{
						newSurface->GetMaterial()->DeepCopy(material);
						newSurface->GetMaterial()->SetMaterialTexture(newSurface->GetSurfaceOutput()->GetTexture());
						newSurface->GetMaterial()->UpdateProp();
					}

					newSurface->SetMatrix(*slicer->GetOutput()->GetMatrix());
					m_Output=newSurface;
					if (m_Output)
					{
						m_Output->ReparentTo(m_Input->GetParent());
						if(!m_TestMode)
							OpStop(OP_RUN_OK);
					}

				}
				else if(mafVMEProber *prober = mafVMEProber::SafeDownCast(vme))
				{
					mmaMaterial *material = prober->GetMaterial();

					mafSmartPointer<mafVMESurface> newSurface;
					newSurface->SetName(prober->GetName());
					newSurface->SetData(polyData,prober->GetTimeStamp());
					newSurface->Update();

					if(material)
					{
						newSurface->GetMaterial()->DeepCopy(material);
						newSurface->GetMaterial()->UpdateProp();
					}

					newSurface->SetMatrix(*prober->GetOutput()->GetMatrix());
					m_Output=newSurface;
					if (m_Output)
					{
						m_Output->ReparentTo(m_Input->GetParent());
						if(!m_TestMode)
							OpStop(OP_RUN_OK);
					}
				}
				else if(mafVMEWrappedMeter *wrappedMeter = mafVMEWrappedMeter::SafeDownCast(vme))
				{
					mmaMaterial *material = wrappedMeter->GetMaterial();

					mafSmartPointer<mafVMEPolyline> newPolyline;
					newPolyline->SetName(wrappedMeter->GetName());
					newPolyline->SetData(polyData,wrappedMeter->GetTimeStamp());
					newPolyline->Update();

					if(material)
					{
						newPolyline->GetMaterial()->DeepCopy(material);
						newPolyline->GetMaterial()->UpdateProp();
					}

					newPolyline->SetMatrix(*wrappedMeter->GetOutput()->GetMatrix());
					m_Output=newPolyline;
					if (m_Output)
					{
						m_Output->ReparentTo(m_Input->GetParent());
						if(!m_TestMode)
							OpStop(OP_RUN_OK);
					}
				}
				else if(medVMEComputeWrapping *wrappedMeter = medVMEComputeWrapping::SafeDownCast(vme))
				{
					mmaMaterial *material = wrappedMeter->GetMaterial();

					mafSmartPointer<mafVMEPolyline> newPolyline;
					newPolyline->SetName(wrappedMeter->GetName());
					newPolyline->SetData(polyData,wrappedMeter->GetTimeStamp());
					newPolyline->Update();

					if(material)
					{
						newPolyline->GetMaterial()->DeepCopy(material);
						newPolyline->GetMaterial()->UpdateProp();
					}

					newPolyline->SetMatrix(*wrappedMeter->GetOutput()->GetMatrix());
					m_Output=newPolyline;
					if (m_Output)
					{
						m_Output->ReparentTo(m_Input->GetParent());
						if(!m_TestMode)
							OpStop(OP_RUN_OK);
					}
				}
				else
					OpStop(OP_RUN_CANCEL);

			}
			else
				OpStop(OP_RUN_CANCEL);


}
//----------------------------------------------------------------------------
void mafOpFreezeVME::OpDo()   
	//----------------------------------------------------------------------------
{
	mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}


