/*=========================================================================

 Program: MAF2
 Module: mafOpVolumeUnion
 Author: Simone Bnà
 
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
#include "mafOpVolumeUnion.h"
#include <wx/busyinfo.h>
#include "mafEvent.h"
#include "mafGUI.h"

#include "mafString.h"
#include "mafVME.h"
#include "mafVMEVolumeGray.h"

#include "vtkMAFDistanceFilter.h"
#include "vtkMAFSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkRectilinearGrid.h"
#include "vtkStructuredPoints.h"
#include "vtkProbeFilter.h"
#include "vtkExtractRectilinearGrid.h"
#include "vtkDoubleArray.h"
#include "vtkPointData.h"
#include <vtkShortArray.h>
#include <vtkUnsignedShortArray.h>
#include <vtkProbeFilter.h>
#include <algorithm>

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpVolumeUnion);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpVolumeUnion::mafOpVolumeUnion(const wxString &label)
: mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_OP;
	m_InputPreserving = false;
	m_Canundo	= true;

	m_FirstVMEVolume = NULL;
	m_SecondVMEVolume = NULL;

	m_VolUnionRG = NULL;

	resolutionXYZ[0] = 150;
	resolutionXYZ[1] = 150;
	resolutionXYZ[2] = 150;

	vme_is_selected = false;

}
//----------------------------------------------------------------------------
mafOpVolumeUnion::~mafOpVolumeUnion()
//----------------------------------------------------------------------------
{
	if(m_VolUnionRG) vtkDEL(m_VolUnionRG);
}

//----------------------------------------------------------------------------
mafOp *mafOpVolumeUnion::Copy()
//----------------------------------------------------------------------------
{
	return new mafOpVolumeUnion(m_Label);
}
//----------------------------------------------------------------------------
bool mafOpVolumeUnion::Accept(mafNode* node)
//----------------------------------------------------------------------------
{
	mafEvent e(this,VIEW_SELECTED);
	mafEventMacro(e);
	return (node && node->IsA("mafVMEVolumeGray"));
}
//----------------------------------------------------------------------------
void mafOpVolumeUnion::OpRun()   
////----------------------------------------------------------------------------
{
    mafEvent e(this,VIEW_SELECTED);
    mafEventMacro(e);

	m_FirstVMEVolume = mafVMEVolume::SafeDownCast(m_Input);
	m_FirstVMEVolume->Update();

	if ( !(m_FirstVMEVolume->GetOutput()->GetVTKData()->IsA("vtkRectilinearGrid")) )
	{
		wxMessageBox("The input VME has not a VTK RectilinearGrid data!");
		OpStop(OP_RUN_CANCEL);
	}

	if(!m_TestMode)
	{
		CreateGui();
		mafEventMacro(mafEvent(this, CAMERA_UPDATE));
	}
}
//----------------------------------------------------------------------------
void mafOpVolumeUnion::BuildVolumeUnion()
//----------------------------------------------------------------------------
{
	wxBusyInfo *wait = NULL;
	long progress = 0;
	if(!this->m_TestMode)
	{
		wait = new wxBusyInfo("Build Volume Union: please wait...");
		mafEventMacro(mafEvent(this,PROGRESSBAR_SHOW));
		mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE, progress));
	}

	//Input data(first volume)
	vtkMAFSmartPointer<vtkRectilinearGrid> rgrid_firstvol;
	rgrid_firstvol->DeepCopy(vtkRectilinearGrid::SafeDownCast(m_FirstVMEVolume->GetVolumeOutput()->GetVTKData()));
	rgrid_firstvol->Update();

	double orig_firstvol[3];
	double orient_firstvol[3];
	m_FirstVMEVolume->GetVolumeOutput()->GetPose(orig_firstvol, orient_firstvol, 0);

	//Input data(second volume)
	vtkMAFSmartPointer<vtkRectilinearGrid> rgrid_secondvol;
	rgrid_secondvol->DeepCopy(vtkRectilinearGrid::SafeDownCast(m_SecondVMEVolume->GetVolumeOutput()->GetVTKData()));
	rgrid_secondvol->Update();

	double orig_secondvol[3];
	double orient_secondvol[3];
	m_SecondVMEVolume->GetVolumeOutput()->GetPose(orig_secondvol, orient_secondvol, 0);

	// dim input vol
	int dim_firstvol[3];
	rgrid_firstvol->GetDimensions(dim_firstvol);
	int dim_secondvol[3];
	rgrid_secondvol->GetDimensions(dim_secondvol);

	//input bounds
	double bounds_firstvol[6];
	m_FirstVMEVolume->GetVolumeOutput()->GetBounds(bounds_firstvol);
	double bounds_secondvol[6];
	m_SecondVMEVolume->GetVolumeOutput()->GetBounds(bounds_secondvol);

	// we have to know if the two volumes interpenetrate or not
	bool xaxixcomp = false;
	bool yaxixcomp = false;
	bool zaxixcomp = false;

	if(bounds_firstvol[1] < bounds_secondvol[0] || bounds_firstvol[0] > bounds_secondvol[1] ) xaxixcomp = true;
	if(bounds_firstvol[3] < bounds_secondvol[2] || bounds_firstvol[2] > bounds_secondvol[3] ) yaxixcomp = true;
	if(bounds_firstvol[5] < bounds_secondvol[4] || bounds_firstvol[4] > bounds_secondvol[5] ) zaxixcomp = true;

	if( !(xaxixcomp || yaxixcomp || zaxixcomp) )
	{
		wxMessageBox("The two solids interpenetrate each other!");
	}

	//computation of the bounds of the total union volume
	double bounds[6];
	//lower bounds
	bounds[0] = ( bounds_firstvol[0] < bounds_secondvol[0] )?bounds_firstvol[0]:bounds_secondvol[0];
	bounds[2] = ( bounds_firstvol[2] < bounds_secondvol[2] )?bounds_firstvol[2]:bounds_secondvol[2];
	bounds[4] = ( bounds_firstvol[4] < bounds_secondvol[4] )?bounds_firstvol[4]:bounds_secondvol[4];

	//upper bounds
	bounds[1] = ( bounds_firstvol[1] > bounds_secondvol[1] )?bounds_firstvol[1]:bounds_secondvol[1];
	bounds[3] = ( bounds_firstvol[3] > bounds_secondvol[3] )?bounds_firstvol[3]:bounds_secondvol[3];
	bounds[5] = ( bounds_firstvol[5] > bounds_secondvol[5] )?bounds_firstvol[5]:bounds_secondvol[5];

	//Input data for the probe filter operation
	vtkMAFSmartPointer<vtkRectilinearGrid> rgrid_totvol;
	int resolution[3];
	resolution[0] = (int)(resolutionXYZ[0]);
	resolution[1] = (int)(resolutionXYZ[1]);
	resolution[2] = (int)(resolutionXYZ[2]);

	rgrid_totvol->SetDimensions(resolution[0],resolution[1],resolution[2]);
	
	//Coordinates
	vtkMAFSmartPointer<vtkDoubleArray> daVector[3];
	daVector[0]->SetNumberOfValues(resolution[0]);
	daVector[1]->SetNumberOfValues(resolution[1]);
	daVector[2]->SetNumberOfValues(resolution[2]);

	for (int arrayId=0; arrayId<3; arrayId++)
	{
		for (int valueId = 0; valueId<resolution[arrayId]; valueId++)
		{
			double spacing = (bounds[2*arrayId+1] - bounds[2*arrayId])/((double)(resolution[arrayId]-1));
			double Val = bounds[2*arrayId] + spacing*((double)(valueId));
			daVector[arrayId]->SetValue(valueId, Val);
		}
	}

	rgrid_totvol->SetXCoordinates(daVector[0]);
	rgrid_totvol->SetYCoordinates(daVector[1]);
	rgrid_totvol->SetZCoordinates(daVector[2]);

	rgrid_totvol->Update();

	if(!this->m_TestMode)
	{
		for(int i=1; i<21; i++) 
		{
			progress++;
			Sleep(150); // Workaround: I need this sleep function to update slowly the progress bar 
			mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,progress));
		}	
	}
	 
	 
	//Translation of the coordinates
	vtkDataArray *daVector_firstvol[3];

	daVector_firstvol[0] = rgrid_firstvol->GetXCoordinates();
	daVector_firstvol[1] = rgrid_firstvol->GetYCoordinates();
	daVector_firstvol[2] = rgrid_firstvol->GetZCoordinates();
	
	for (int arrayId=0; arrayId<3; arrayId++)
	{
		int numTuples = daVector_firstvol[arrayId]->GetNumberOfTuples();
		for (int tupleId = 0; tupleId<numTuples; tupleId++)
		{
			double oldVal = daVector_firstvol[arrayId]->GetComponent(tupleId,0);
			double newVal = orig_firstvol[arrayId] + oldVal;
			daVector_firstvol[arrayId]->SetComponent(tupleId,0, newVal);
		}
	}

	rgrid_firstvol->SetXCoordinates(daVector_firstvol[0]);
	rgrid_firstvol->SetYCoordinates(daVector_firstvol[1]);
	rgrid_firstvol->SetZCoordinates(daVector_firstvol[2]);

	//----

	
	// projection of the rgrid_firstvol selected into the rgrid_totvol
	vtkMAFSmartPointer<vtkProbeFilter> sampleVolume1;
	sampleVolume1->SetInput(rgrid_totvol);
	//The source is the dataset to probe
	sampleVolume1->SetSource(rgrid_firstvol);
	sampleVolume1->Update();


	if(!this->m_TestMode)
	{
		for(int i=21; i<61; i++) 
		{
			progress++;
			Sleep(150);
		    mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,progress));
		}
	}
	

	//Translation of the coordinates
	vtkDataArray *daVector_secondvol[3];

	daVector_secondvol[0] = rgrid_secondvol->GetXCoordinates();
	daVector_secondvol[1] = rgrid_secondvol->GetYCoordinates();
	daVector_secondvol[2] = rgrid_secondvol->GetZCoordinates();

	for (int arrayId=0; arrayId<3; arrayId++)
	{
		int numTuples = daVector_secondvol[arrayId]->GetNumberOfTuples();
		for (int tupleId = 0; tupleId<numTuples; tupleId++)
		{
			double oldVal = daVector_secondvol[arrayId]->GetComponent(tupleId,0);
			double newVal = orig_secondvol[arrayId] + oldVal;
			daVector_secondvol[arrayId]->SetComponent(tupleId, 0, newVal);
		}
	}

	rgrid_secondvol->SetXCoordinates(daVector_secondvol[0]);
	rgrid_secondvol->SetYCoordinates(daVector_secondvol[1]);
	rgrid_secondvol->SetZCoordinates(daVector_secondvol[2]);

	//----

	// projection of the rgrid_secondvol selected into the rgrid_totvol
	vtkMAFSmartPointer<vtkProbeFilter> sampleVolume2;
	sampleVolume2->SetInput(rgrid_totvol);
	//The source is the dataset to probe
	sampleVolume2->SetSource(rgrid_secondvol);
	sampleVolume2->Update();

	if(!this->m_TestMode)
	{
		for(int i=61; i<99; i++) 
		{
			progress++;
			Sleep(150);
			mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,progress));
		}
	}
	
	//Output volume
	//Coordinates
	m_VolUnionRG = vtkRectilinearGrid::New();
	m_VolUnionRG->SetDimensions(resolution[0],resolution[1],resolution[2]);
	m_VolUnionRG->SetXCoordinates(rgrid_totvol->GetXCoordinates());
	m_VolUnionRG->SetYCoordinates(rgrid_totvol->GetYCoordinates());
	m_VolUnionRG->SetZCoordinates(rgrid_totvol->GetZCoordinates());
	m_VolUnionRG->Update();

	//Scalars
	vtkDataArray* rgarray_VolUnionRG= vtkDataArray::CreateDataArray(sampleVolume1->GetOutput()->GetPointData()->GetScalars()->GetDataType());
	rgarray_VolUnionRG->Allocate(resolution[0]*resolution[1]*resolution[2]);
	rgarray_VolUnionRG->SetNumberOfComponents(1);
	rgarray_VolUnionRG->SetNumberOfTuples(resolution[0]*resolution[1]*resolution[2]);

	vtkDataArray *rgarray_sampleVolume1 = sampleVolume1->GetOutput()->GetPointData()->GetScalars();
	vtkDataArray *rgarray_sampleVolume2 = sampleVolume2->GetOutput()->GetPointData()->GetScalars();

	// Sum the two probe-filter outputs into the total volume 
	for(int i=0; i<resolution[0]*resolution[1]*resolution[2]; i++) {
		double val_sampleVolume1 = rgarray_sampleVolume1->GetComponent(i, 0);
		double val_sampleVolume2 = rgarray_sampleVolume2->GetComponent(i, 0);
		double val_union = (val_sampleVolume1 + val_sampleVolume2);
		rgarray_VolUnionRG->SetComponent(i,0,val_union);
	}

	m_VolUnionRG->GetPointData()->SetScalars(rgarray_VolUnionRG);
    m_VolUnionRG->Update();

	if(!this->m_TestMode)
	{
		for(int i=99; i<101; i++) 
		{
			progress++;
			Sleep(150);
			mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,progress));
		}
	}

	if(!this->m_TestMode)
	{
	    mafEventMacro(mafEvent(this,PROGRESSBAR_HIDE));
	}
	if(wait) delete wait;

	vtkDEL(rgarray_VolUnionRG);

	return;
}
//----------------------------------------------------------------------------
void mafOpVolumeUnion::OpDo()
//----------------------------------------------------------------------------
{
	if(m_VolUnionRG) {
		((mafVMEVolume*)m_Input)->GetOutput()->GetVTKData()->SetUpdateExtentToWholeExtent();
		((mafVMEVolume*)m_Input)->SetData(m_VolUnionRG,((mafVME*)m_Input)->GetTimeStamp());
	}

	((mafVMEVolume*)m_Input)->GetOutput()->Update();
	((mafVMEVolume*)m_Input)->Update();

}
//----------------------------------------------------------------------------
void mafOpVolumeUnion::OpUndo()
//----------------------------------------------------------------------------
{
	if(m_FirstVMEVolume->GetOutput()->GetVTKData())
		((mafVMEVolume*)m_Input)->SetData(m_FirstVMEVolume->GetOutput()->GetVTKData(),((mafVME*)m_Input)->GetTimeStamp());

	((mafVMEVolume*)m_Input)->GetOutput()->Update();
	((mafVMEVolume*)m_Input)->Update();

}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------
enum VOLUME_UNION_WIDGET_ID
{
  ID_RESOLUTION,
  ID_HELP,
  ID_VOL_SELECT
};

//----------------------------------------------------------------------------
bool mafOpVolumeUnion::VmeChoose(mafString title,mafEvent *e)
//----------------------------------------------------------------------------
{
	e->SetArg((long)&mafOpVolumeUnion::VmeUnionAccept);
	e->SetString(&title);
	e->SetId(VME_CHOOSE);
	mafEventMacro(*e);
	if(e->GetVme()) {
        m_SecondVMEVolume = mafVMEVolume::SafeDownCast(e->GetVme());
		return true;
	}	
	else {
		return false;
	}
}


//----------------------------------------------------------------------------
void mafOpVolumeUnion::CreateGui() 
//----------------------------------------------------------------------------
{
	m_Gui = new mafGUI(this);
	mafEvent buildHelpGui;
	buildHelpGui.SetSender(this);
	buildHelpGui.SetId(GET_BUILD_HELP_GUI);
	mafEventMacro(buildHelpGui);

	if (buildHelpGui.GetArg() == true)
	{
		m_Gui->Button(ID_HELP, "Help","");	
	}

	m_Gui->Label("");
	m_Gui->Label("Choose the volume for the union operation");
	m_Gui->Button(ID_VOL_SELECT, _("Choose"), "", _("Choose the volume for the union operation"));

	m_Gui->Label("");

	int dimXYZ[3];
	m_FirstVMEVolume->GetVolumeOutput()->GetRectilinearData()->GetDimensions(dimXYZ);
	resolutionXYZ[0] = dimXYZ[0];
	resolutionXYZ[1] = dimXYZ[1];
	resolutionXYZ[2] = dimXYZ[2];
    m_Gui->VectorN(ID_RESOLUTION, _("Resolution"), resolutionXYZ, 3, 30, 1000);

	m_Gui->Label("");
	m_Gui->OkCancel();

	m_Gui->Divider();

    ShowGui();
}

//----------------------------------------------------------------------------
void mafOpVolumeUnion::OnEvent(mafEventBase *maf_event) 
////----------------------------------------------------------------------------
{
    if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch(e->GetId())
		{

			case ID_HELP:
			{
				mafEvent helpEvent;
				helpEvent.SetSender(this);
				mafString operationLabel = this->m_Label;
				helpEvent.SetString(&operationLabel);
				helpEvent.SetId(OPEN_HELP_PAGE);
				mafEventMacro(helpEvent);
			}
			break;
			case ID_VOL_SELECT:
				{
					mafString title = "Choose Union Volume";
					bool selOK = VmeChoose(title,e);
					if(selOK) {
						if ( !(m_SecondVMEVolume->GetOutput()->GetVTKData()->IsA("vtkRectilinearGrid")) )
						{
							wxMessageBox("The second input VME has not a VTK RectilinearGrid data!");
							OpStop(OP_RUN_CANCEL);
						}
					    if(m_Input == m_SecondVMEVolume)
					    {
					 	     mafMessage(_("Can't operate over the same VME"));
						     return;
					    }
					    vme_is_selected = true;
					    int dimXYZ_firstvolume[3];
					    m_FirstVMEVolume->GetVolumeOutput()->GetRectilinearData()->GetDimensions(dimXYZ_firstvolume);
					    int dimXYZ_secondvolume[3];
					    m_SecondVMEVolume->GetVolumeOutput()->GetRectilinearData()->GetDimensions(dimXYZ_secondvolume);

					    resolutionXYZ[0] = std::max(dimXYZ_firstvolume[0],dimXYZ_secondvolume[0]);
					    resolutionXYZ[1] = std::max(dimXYZ_firstvolume[1],dimXYZ_secondvolume[1]);
					    resolutionXYZ[2] = std::max(dimXYZ_firstvolume[2],dimXYZ_secondvolume[2]);

					    m_Gui->Update();
				    }
					else {
						vme_is_selected = false;
						break;
					}
				}
				break;
			case wxOK:
				    if(!vme_is_selected) {
						wxMessageBox("Please select the second volume for the volume union operation!");
						break;
					}
					BuildVolumeUnion();
					OpStop(OP_RUN_OK);
			    break;
			case wxCANCEL:
				OpStop(OP_RUN_CANCEL);
			    break;
			default:
				mafEventMacro(*e);
			    break;
		}	
	}
}

