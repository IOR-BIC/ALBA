/*=========================================================================

Program: ALBA
Module: albaOpFreezeVME
Authors: Daniele Giunchi , Stefano Perticoni

Copyright (c) BIC
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

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

#include "albaOpFreezeVME.h"
#include "wx/busyinfo.h"

#include "albaDecl.h"
#include "albaVME.h"
#include "albaSmartPointer.h"

#include "albaVMEGenericAbstract.h"
#include "albaVMERoot.h"
#include "albaVMERefSys.h"
#include "albaVMEVolume.h"

#include "albaVMESurface.h"
#include "albaVMEPolyline.h"
#include "albaVMEPolylineSpline.h"
#include "albaVMESurfaceParametric.h"
#include "albaVMEMeter.h"
#include "albaVMESlicer.h"
#include "albaVMEProber.h"
#include "albaVMEWrappedMeter.h"
#include "albaVMELabeledVolume.h"

#include "vtkRectilinearGrid.h"
#include "mmaMaterial.h"
#include "mmaVolumeMaterial.h"
#include "albaGUI.h"
#include "albaAbsMatrixPipe.h"

#include "vtkPolyData.h"
#include "vtkImageData.h"
#include "albaVMEComputeWrapping.H"



//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpFreezeVME);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpFreezeVME::albaOpFreezeVME(wxString label) :
albaOp(label)
	//----------------------------------------------------------------------------
{
	m_OpType  = OPTYPE_OP;
	m_Canundo = true;
	m_InputPreserving = true;

}
//----------------------------------------------------------------------------
albaOpFreezeVME::~albaOpFreezeVME( ) 
	//----------------------------------------------------------------------------
{

}
//----------------------------------------------------------------------------
bool albaOpFreezeVME::InternalAccept(albaVME*node)
	//----------------------------------------------------------------------------
{
	return (node->IsALBAType(albaVME) && 
		!node->IsALBAType(albaVMEGenericAbstract) && 
		!node->IsALBAType(albaVMERoot) &&
		//!node->IsALBAType(albaVMERefSys) &&
		!node->IsALBAType(albaVMEVolume) ); //return if is a procedural vme
}
//----------------------------------------------------------------------------
albaOp* albaOpFreezeVME::Copy()   
	//----------------------------------------------------------------------------
{
	albaOpFreezeVME *cp = new albaOpFreezeVME(m_Label);
	return cp;
}
//----------------------------------------------------------------------------
void albaOpFreezeVME::OpRun()   
	//----------------------------------------------------------------------------
{
	if(!m_TestMode)
	{
		m_Gui = new albaGUI(this);
		ShowGui();
	}

	//control the output and create the right vme
	m_Input->Update();
	albaVMEOutput *output = m_Input->GetOutput();
	output->Update();

	if(vtkImageData *imageData = vtkImageData::SafeDownCast(output->GetVTKData()))
	{
		if(albaVMELabeledVolume *labeledVolume = albaVMELabeledVolume::SafeDownCast(m_Input))
		{
			mmaMaterial *material = (mmaMaterial *)labeledVolume->GetAttribute("MaterialAttributes");

			albaSmartPointer<albaVMEVolumeGray> newVolume;
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
			if(albaVMELabeledVolume *labeledVolume = albaVMELabeledVolume::SafeDownCast(m_Input))
			{
				mmaMaterial *material = (mmaMaterial *)labeledVolume->GetAttribute("MaterialAttributes");

				albaSmartPointer<albaVMEVolumeGray> newVolume;
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
				if(albaVMEPolylineSpline *vmeSpline = albaVMEPolylineSpline::SafeDownCast(m_Input))
				{
					mmaMaterial *material = vmeSpline->GetMaterial();

					albaSmartPointer<albaVMEPolyline> newPolyline;
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
				else if(albaVMESurfaceParametric *vmeSurface = albaVMESurfaceParametric::SafeDownCast(m_Input))
				{
					mmaMaterial *material = vmeSurface->GetMaterial();

					albaSmartPointer<albaVMESurface> newSurface;
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
				else if(albaVMEMeter *meter = albaVMEMeter::SafeDownCast(m_Input))
				{
					mmaMaterial *material = meter->GetMaterial();

					albaSmartPointer<albaVMEPolyline> newPolyline;
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
				else if(albaVMERefSys *refsys = albaVMERefSys::SafeDownCast(m_Input))
				{
					mmaMaterial *material = refsys->GetMaterial();

					albaSmartPointer<albaVMESurface> surface;
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
				else if(albaVMESlicer *slicer = albaVMESlicer::SafeDownCast(m_Input))
				{
					mmaMaterial *material = slicer->GetMaterial();

					albaSmartPointer<albaVMESurface> newSurface;
					newSurface->SetName(slicer->GetName());
					newSurface->SetData(polyData,slicer->GetTimeStamp());
					vtkALBASmartPointer<vtkImageData> text;
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
				else if(albaVMEProber *prober = albaVMEProber::SafeDownCast(m_Input))
				{
					mmaMaterial *material = prober->GetMaterial();

					albaSmartPointer<albaVMESurface> newSurface;
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
				else if(albaVMEWrappedMeter *wrappedMeter = albaVMEWrappedMeter::SafeDownCast(m_Input))
				{
					mmaMaterial *material = wrappedMeter->GetMaterial();

					albaSmartPointer<albaVMEPolyline> newPolyline;
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
				else if(albaVMEComputeWrapping *wrappedMeter = albaVMEComputeWrapping::SafeDownCast(m_Input))
				{
					mmaMaterial *material = wrappedMeter->GetMaterial();

					albaSmartPointer<albaVMEPolyline> newPolyline;
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
void albaOpFreezeVME::OpDo()   
	//----------------------------------------------------------------------------
{
	GetLogicManager()->CameraUpdate(); GetLogicManager()->CameraUpdate();
}


