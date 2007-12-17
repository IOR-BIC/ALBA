/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpTriangulateSurface.cpp,v $
Language:  C++
Date:      $Date: 2007-12-17 19:07:40 $
Version:   $Revision: 1.2 $
Authors:   Alessandro Chiarini
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)

MafMedical Library use license agreement

The software named MafMedical Library and any accompanying documentation, 
manuals or data (hereafter collectively "SOFTWARE") is property of the SCS s.r.l.
This is an open-source copyright as follows:
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.
* Modified source versions must be plainly marked as such, and must not be misrepresented 
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MafMedical is partially based on OpenMAF.
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medOpTriangulateSurface.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mmgGui.h"

#include "mafVMESurface.h"

#include "vtkMAFSmartPointer.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkTriangleFilter.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(medOpTriangulateSurface);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medOpTriangulateSurface::medOpTriangulateSurface(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_OP;
	m_Canundo	= true;

	m_InputPreserving = false;

	m_PreviewResultFlag	= false;
	m_ClearInterfaceFlag= false;


	m_ResultPolydata	  = NULL;
	m_OriginalPolydata  = NULL;
}
//----------------------------------------------------------------------------
medOpTriangulateSurface::~medOpTriangulateSurface()
//----------------------------------------------------------------------------
{
	vtkDEL(m_ResultPolydata);
	vtkDEL(m_OriginalPolydata);
}
//----------------------------------------------------------------------------
bool medOpTriangulateSurface::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
	return (node && node->IsMAFType(mafVMESurface));
}
//----------------------------------------------------------------------------
mafOp *medOpTriangulateSurface::Copy()   
//----------------------------------------------------------------------------
{
	return (new medOpTriangulateSurface(m_Label));
}
//----------------------------------------------------------------------------
// Constants:
//----------------------------------------------------------------------------
enum SMOOTH_SURFACE_ID
{
	ID_SMOOTH = MINID,
	ID_PREVIEW,
	ID_CLEAR,
	ID_ITERACTION,
};
//----------------------------------------------------------------------------
void medOpTriangulateSurface::OpRun()   
//----------------------------------------------------------------------------
{  
	vtkNEW(m_ResultPolydata);
	m_ResultPolydata->DeepCopy((vtkPolyData*)((mafVME *)m_Input)->GetOutput()->GetVTKData());

	vtkNEW(m_OriginalPolydata);
	m_OriginalPolydata->DeepCopy((vtkPolyData*)((mafVME *)m_Input)->GetOutput()->GetVTKData());

	if(!m_TestMode)
  {
    CreateGui();
  }
}
//----------------------------------------------------------------------------
void medOpTriangulateSurface::CreateGui()
//----------------------------------------------------------------------------
{
  // interface:
  m_Gui = new mmgGui(this);

  m_Gui->Label("");
  m_Gui->Label(_("Triangulate Surface"),true);
  m_Gui->Button(ID_SMOOTH,_("Triangulate"));

  m_Gui->Divider(2);
  m_Gui->Label("");
  m_Gui->Button(ID_PREVIEW,_("preview"));
  m_Gui->Button(ID_CLEAR,_("clear"));
  m_Gui->OkCancel();
  m_Gui->Enable(wxOK,false);

  m_Gui->Enable(ID_PREVIEW,false);
  m_Gui->Enable(ID_CLEAR,false);

  m_Gui->Divider();

  ShowGui();
}
//----------------------------------------------------------------------------
void medOpTriangulateSurface::OpDo()
//----------------------------------------------------------------------------
{
	((mafVMESurface *)m_Input)->SetData(m_ResultPolydata,((mafVME *)m_Input)->GetTimeStamp());
	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void medOpTriangulateSurface::OpUndo()
//----------------------------------------------------------------------------
{
	((mafVMESurface *)m_Input)->SetData(m_OriginalPolydata,((mafVME *)m_Input)->GetTimeStamp());
	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void medOpTriangulateSurface::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch(e->GetId())
		{	
		case ID_SMOOTH:
			OnTriangle();
			break;
		case ID_PREVIEW:
			OnPreview(); 
			break;
		case ID_CLEAR:
			OnClear(); 
			break;
		case wxOK:
			if(m_PreviewResultFlag)
				OnPreview();
			OpStop(OP_RUN_OK);        
			break;
		case wxCANCEL:
			if(m_ClearInterfaceFlag)
				OnClear();
			OpStop(OP_RUN_CANCEL);        
			break;
		}
	}
}
//----------------------------------------------------------------------------
void medOpTriangulateSurface::OpStop(int result)
//----------------------------------------------------------------------------
{
	HideGui();
	mafEventMacro(mafEvent(this,result));        
}
//----------------------------------------------------------------------------
void medOpTriangulateSurface::OnTriangle()
//----------------------------------------------------------------------------
{
  if(!m_TestMode)
  {
	  wxBusyCursor wait;
	  m_Gui->Enable(ID_SMOOTH,false);
	  m_Gui->Enable(ID_ITERACTION,false);
	  m_Gui->Update();
  }

	vtkMAFSmartPointer<vtkTriangleFilter> smoothFilter;
	smoothFilter->SetInput(m_ResultPolydata);
	smoothFilter->Update();

	m_ResultPolydata->DeepCopy(smoothFilter->GetOutput());

  if(!m_TestMode)
  {
	  m_Gui->Enable(ID_SMOOTH,true);
	  m_Gui->Enable(ID_ITERACTION,true);

	  m_Gui->Enable(ID_PREVIEW,true);
	  m_Gui->Enable(ID_CLEAR,true);
	  m_Gui->Enable(wxOK,true);
  }

	m_PreviewResultFlag = true;
}
//----------------------------------------------------------------------------
void medOpTriangulateSurface::OnPreview()
//----------------------------------------------------------------------------
{
	wxBusyCursor wait;

	((mafVMESurface *)m_Input)->SetData(m_ResultPolydata,((mafVME *)m_Input)->GetTimeStamp());

	m_Gui->Enable(ID_PREVIEW,false);
	m_Gui->Enable(ID_CLEAR,true);
	m_Gui->Enable(wxOK,true);

	m_PreviewResultFlag   = false;
	m_ClearInterfaceFlag	= true;

	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void medOpTriangulateSurface::OnClear()  
//----------------------------------------------------------------------------
{
	wxBusyCursor wait;

	((mafVMESurface *)m_Input)->SetData(m_OriginalPolydata,((mafVME *)m_Input)->GetTimeStamp());

	m_ResultPolydata->DeepCopy(m_OriginalPolydata);


	m_Gui->Enable(ID_SMOOTH,true);
	m_Gui->Enable(ID_ITERACTION,true);

	m_Gui->Enable(ID_PREVIEW,false);
	m_Gui->Enable(ID_CLEAR,false);
	m_Gui->Enable(wxOK,false);
	m_Gui->Update();

	m_PreviewResultFlag = false;
	m_ClearInterfaceFlag= false;

	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
