/*=========================================================================
Program:   ALBA (Agile Library for Biomedical Applications)
Module:    albaOpExtractImageFromArbitraryView.cpp
Language:  C++
Date:      $Date: 2019-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2019 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the APP must include "appDefines.h" as first.
// This force to include Window, wxWidgets and VTK exactly in this order.
//----------------------------------------------------------------------------

#include "albaOpExtractImageFromArbitraryView.h"

#include "albaDecl.h"
#include "albaEvent.h"
#include "albaGUI.h"
#include "albaObject.h"
#include "albaOp.h"
#include "albaPipe.h"
#include "albaServiceClient.h"
#include "albaTagArray.h"
#include "albaVME.h"
#include "albaVMEImage.h"
#include "albaView.h"
#include "albaViewArbitraryOrthoSlice.h"
#include "albaViewArbitrarySlice.h"
#include "albaViewCompound.h"
#include "albaViewSlice.h"
#include "albaViewVTK.h"

#include "vtkALBAAssembly.h"
#include "vtkALBASmartPointer.h"
#include "vtkCamera.h"
#include "vtkDataSet.h"
#include "vtkDataSetAttributes.h"
#include "vtkImageData.h"
#include "vtkImageExport.h"
#include "vtkPointData.h"
#include "vtkPropCollection.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkUnsignedCharArray.h"
#include "vtkWindowToImageFilter.h"

#include "wx\bitmap.h"
#include "wx\image.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpExtractImageFromArbitraryView);

//----------------------------------------------------------------------------
albaOpExtractImageFromArbitraryView::albaOpExtractImageFromArbitraryView(wxString label) :albaOp(label)
{
	m_OpType = OPTYPE_OP;
	m_Canundo = true;

	m_View = NULL;
	m_ImageSlicesGroup = NULL;

	m_Axis = 2; //Default Z

	m_ShowInTree = true;
	m_ChooseName = true;

	m_ImageName = "";
}

//----------------------------------------------------------------------------
albaOpExtractImageFromArbitraryView::~albaOpExtractImageFromArbitraryView()
{
}

//----------------------------------------------------------------------------
bool albaOpExtractImageFromArbitraryView::Accept(albaVME *node)
{
	return node->IsA("albaVMEVolumeGray");
}

//----------------------------------------------------------------------------
albaOp* albaOpExtractImageFromArbitraryView::Copy()
{
	albaOpExtractImageFromArbitraryView *cp = new albaOpExtractImageFromArbitraryView(m_Label);
	return cp;
}
//----------------------------------------------------------------------------
void albaOpExtractImageFromArbitraryView::OpRun()
{
	if (!m_TestMode)
	{
		albaEvent e(this, VIEW_SELECTED);
		albaEventMacro(e);

		if (e.GetBool())
		{
			m_View = e.GetView();

			GetLogicManager()->VmeShow(m_Input, true);

			CreateGui();
		}
		else
		{
			wxMessageBox("No View Selected");
			OpStop(OP_RUN_CANCEL);
		}
	}
}
//----------------------------------------------------------------------------
void albaOpExtractImageFromArbitraryView::OpStop(int result)
{
	if (!m_TestMode)
	{
		HideGui();
	}

	if (result == OP_RUN_OK)
	{
		ExtractImage();
	}
	
	albaEventMacro(albaEvent(this, result));
}
//----------------------------------------------------------------------------
void albaOpExtractImageFromArbitraryView::OpDo()
{
}

//----------------------------------------------------------------------------
void albaOpExtractImageFromArbitraryView::OnEvent(albaEventBase *alba_event)
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch (e->GetId())
		{
		case wxOK:
			OpStop(OP_RUN_OK);
			break;

		case wxCANCEL:
			OpStop(OP_RUN_CANCEL);
			break;

		default:
			Superclass::OnEvent(alba_event);
			break;
		}
	}
}

//----------------------------------------------------------------------------
void albaOpExtractImageFromArbitraryView::CreateGui()
{
	// Interface:
	m_Gui = new albaGUI(this);

	if(m_View && m_View->IsA("albaViewArbitraryOrthoSlice"))
	{
		m_Gui->Label("");
		wxString axisChoice[3] = { "X","Y","Z" };
		m_Gui->Combo(ID_AXIS, "Axis", &m_Axis, 3, axisChoice);
	}

	if (m_ChooseName) 
	{
		m_Gui->Divider(1);
		m_Gui->String(NULL, "Name", &m_ImageName);
	}

	//////////////////////////////////////////////////////////////////////////
	m_Gui->Label("");
	m_Gui->Divider(1);
	m_Gui->OkCancel();
	m_Gui->Label("");

	ShowGui();
}

//----------------------------------------------------------------------------
void albaOpExtractImageFromArbitraryView::ExtractImage()
{
	wxImage img;

	wxBitmap* btm = GetSliceImage();
	if (btm == NULL)
	{
		wxMessageBox("An error occurred!");
		return;
	}

	img = btm->ConvertToImage();

	//////////////////////////////////////////////////////////////////////////

	int NumberOfComponents = 3;
	vtkUnsignedCharArray *buffer;
	vtkNEW(buffer);
	buffer->SetNumberOfComponents(NumberOfComponents);

	unsigned char *p = img.GetData();
	assert(p);
	for (int i = 0; i < img.GetWidth() * img.GetHeight(); i++)
	{
		unsigned char r = *p++;
		unsigned char g = *p++;
		unsigned char b = *p++;

		buffer->InsertNextTuple3(r, g, b);
	}

	vtkImageData *vtkimg;
	vtkNEW(vtkimg);
	vtkimg->SetNumberOfScalarComponents(NumberOfComponents);
	vtkimg->SetScalarTypeToUnsignedChar();
	vtkimg->SetDimensions(img.GetWidth(), img.GetHeight(), 1);
	vtkimg->SetUpdateExtentToWholeExtent();
	assert(vtkimg->GetPointData());
	vtkimg->GetPointData()->SetScalars(buffer);

	//////////////////////////////////////////////////////////////////////////
	// Find or Create Snapshots group
	wxString m_GroupName = "Slices";
	//albaVME *root = m_Input->GetRoot();

	if (m_ImageSlicesGroup == NULL || m_ImageSlicesGroup != (albaVMEGroup *)m_Input->FindInTreeByName(m_GroupName))
	{
		m_ImageSlicesGroup = (albaVMEGroup *)m_Input->FindInTreeByName(m_GroupName);

		if (m_ImageSlicesGroup == NULL)
		{
			albaNEW(m_ImageSlicesGroup);
			m_ImageSlicesGroup->SetName(m_GroupName);
			if (!m_ShowInTree) m_ImageSlicesGroup->GetTagArray()->SetTag(albaTagItem("VISIBLE_IN_THE_TREE", 0.0));
			m_ImageSlicesGroup->ReparentTo(m_Input);
			m_ImageSlicesGroup->Delete();
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// Generate Image Slice name
	wxString imageName = "Slice";

	wxString axisName[3] = { "X","Y","Z" };

	char tmp[20];
	int count = 1;
	do
	{
		sprintf(tmp, "%s_%s_%d", imageName, axisName[m_Axis], count);
		count++;
	} while (m_ImageSlicesGroup->FindInTreeByName(tmp) != NULL);

	imageName = tmp;

	if (m_ChooseName)
	{
		if (!m_ImageName.IsEmpty())
			imageName = m_ImageName;
	}

	//////////////////////////////////////////////////////////////////////////	
	albaVMEImage *image;
	albaNEW(image);

	image->SetData(vtkimg, 0);
	image->SetName(imageName);
	image->SetTimeStamp(0);
	if (!m_ShowInTree) image->GetTagArray()->SetTag(albaTagItem("VISIBLE_IN_THE_TREE", 0.0));
	image->ReparentTo(m_ImageSlicesGroup);

	albaDEL(image);
	//////////////////////////////////////////////////////////////////////////
	vtkDEL(buffer);
	vtkDEL(vtkimg);
}

//----------------------------------------------------------------------------
wxBitmap * albaOpExtractImageFromArbitraryView::GetSliceImage()
{
	vtkRenderer *renderer;
	vtkRenderWindow *renderWindow;

	vtkNEW(renderer);
	vtkNEW(renderWindow);

	renderer->SetBackground(0.0, 0.0, 0.0);

	double bounds[6];
	m_Input->GetOutput()->GetBounds(bounds);

	renderWindow->AddRenderer(renderer);
	renderWindow->SetSize(bounds[3] - bounds[2], bounds[1] - bounds[0]);
	renderWindow->SetPosition(0, 0);

	albaPipe *pipeSlice = NULL;
	if (m_View)
	{
		if (m_View->IsA("albaViewArbitrarySlice"))
		{
			pipeSlice = albaViewArbitrarySlice::SafeDownCast(m_View)->GetPipeSlice();
		}
		else if (m_View->IsA("albaViewArbitraryOrthoSlice"))
		{
			pipeSlice = albaViewArbitraryOrthoSlice::SafeDownCast(m_View)->GetPipeSlice(m_Axis);

			if (m_Axis == 0)
				renderWindow->SetSize(bounds[5] - bounds[4], bounds[3] - bounds[2]);
			else if (m_Axis == 1)
				renderWindow->SetSize(bounds[1] - bounds[0], bounds[5] - bounds[4]);
		}
	}
	if (pipeSlice == NULL) return NULL;

	vtkPropCollection *actorList = vtkPropCollection::New();
	pipeSlice->GetAssemblyFront()->GetActors(actorList);

	actorList->InitTraversal();
	vtkProp *actor = actorList->GetNextProp();
	while (actor)
	{
		renderer->AddActor(actor);
		renderWindow->Render();

		actor = actorList->GetNextProp();
	}

	// Set Camera Properties
	double x, y, z, vx, vy, vz;
	x = 0; y = 0; z = -1; vx = 0; vy = -1; vz = 0; // axis = Z

	renderer->GetActiveCamera()->ParallelProjectionOn();
	renderer->GetActiveCamera()->SetFocalPoint(0, 0, 0);
	renderer->GetActiveCamera()->SetPosition(x * 100, y * 100, z * 100);
	renderer->GetActiveCamera()->SetViewUp(vx, vy, vz);
	renderer->GetActiveCamera()->SetClippingRange(0.1, 1000);
	renderer->GetActiveCamera()->Zoom(2.0);
	//renderer->ResetCamera();

	renderWindow->Render();

	int dim[3];
	int magnification = 1;
	renderWindow->OffScreenRenderingOn();

	vtkALBASmartPointer<vtkWindowToImageFilter> w2i;
	w2i->SetInput(renderWindow);
	w2i->SetMagnification(magnification);
	w2i->Update();
	w2i->GetOutput()->GetDimensions(dim);

	renderWindow->OffScreenRenderingOff();

	assert(dim[0] > 0 && dim[1] > 0);
	unsigned char *buffer = new unsigned char[dim[0] * dim[1] * 3];

	//flip it - windows Bitmap are upside-down
	vtkALBASmartPointer<vtkImageExport> ie;
	ie->SetInput(w2i->GetOutput());
	ie->ImageLowerLeftOff();
	ie->SetExportVoidPointer(buffer);
	ie->Export();

	// Translate to a wxBitmap
	wxImage  *img = new wxImage(dim[0], dim[1], buffer, TRUE);
	wxBitmap *bmp = new wxBitmap(img, 24);

	delete img;
	delete buffer;

	vtkDEL(renderer);
	vtkDEL(renderWindow);

	return bmp;
}