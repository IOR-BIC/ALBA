/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpSubdivide.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 11:14:48 $
  Version:   $Revision: 1.4 $
  Authors:   Matteo Giacomoni
==========================================================================
  Copyright (c) 2002/2008
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

#include "medDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medOpSubdivide.h"
#include "medDecl.h"
#include "mafEvent.h"
#include "mafGUI.h"

#include "mafVMESurface.h"

#include "vtkLinearSubdivisionFilter.h"
#include "vtkDataSet.h"
#include "vtkPolyData.h"
#include "vtkTriangleFilter.h"
#include "vtkCell.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(medOpSubdivide);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medOpSubdivide::medOpSubdivide(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_OP;
	m_Canundo	= true;

	m_InputPreserving = false;

  m_OriginalPolydata = NULL;
  m_ResultPolydata = NULL;
}
//----------------------------------------------------------------------------
medOpSubdivide::~medOpSubdivide()
//----------------------------------------------------------------------------
{
  vtkDEL(m_OriginalPolydata);
  vtkDEL(m_ResultPolydata);
}
//----------------------------------------------------------------------------
bool medOpSubdivide::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return ( node != NULL && node->IsA("mafVMESurface") );
}
//----------------------------------------------------------------------------
mafOp *medOpSubdivide::Copy()   
//----------------------------------------------------------------------------
{
  return (new medOpSubdivide(m_Label));
}
//----------------------------------------------------------------------------
// Constants:
//----------------------------------------------------------------------------
enum U_OP_SUBDIVIDE_ID
{
	ID_MY_OP = MINID,
  // ToDO: add your custom IDs...
  ID_NUMBER_OF_SUBDIVISION,
  ID_PREVIEW,
  ID_SUBDIVIDE,
};
//----------------------------------------------------------------------------
void medOpSubdivide::OpRun()   
//----------------------------------------------------------------------------
{
  mafVMESurface *surface = mafVMESurface::SafeDownCast(m_Input);
  surface->Update();

  vtkPolyData *polydataInput = vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData());
  polydataInput->Update();

  vtkNEW(m_OriginalPolydata);
  m_OriginalPolydata->DeepCopy(polydataInput);
  m_OriginalPolydata->Update();

  vtkNEW(m_ResultPolydata);
  m_ResultPolydata->DeepCopy(polydataInput);
  m_ResultPolydata->Update();

  for(int i=0;i<polydataInput->GetNumberOfCells();i++)
  {
    if(polydataInput->GetCell(i)->GetNumberOfPoints()!=3)
    {
      wxMessageBox(_("The mesh will be triangolarized to be subdivide"));

      vtkTriangleFilter *triangleFilter;
      vtkNEW(triangleFilter);

      triangleFilter->SetInput(polydataInput);
      triangleFilter->Update();

      surface->SetData(triangleFilter->GetOutput(),surface->GetTimeStamp());
      surface->Update();

      m_ResultPolydata->DeepCopy(triangleFilter->GetOutput());
      m_ResultPolydata->Update();

      vtkDEL(triangleFilter);

      break;
    }
  }

  CreateGui();
}
//----------------------------------------------------------------------------
void medOpSubdivide::CreateGui()
//----------------------------------------------------------------------------
{
  // interface:
  m_Gui = new mafGUI(this);

  // ToDO: add your custom widgets...

  m_NumberOfSubdivision = 1;
  m_Gui->Label(_("num. subdivision"));
  m_Gui->Integer(ID_NUMBER_OF_SUBDIVISION,"",&m_NumberOfSubdivision,0);

  m_Gui->Button(ID_SUBDIVIDE,_("subdivide"));
  m_Gui->Button(ID_PREVIEW,_("preview"));

  m_Gui->OkCancel();
  m_Gui->Divider();

  ShowGui();
}
//----------------------------------------------------------------------------
void medOpSubdivide::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {	
      case ID_PREVIEW:
        {
          mafEventMacro(mafEvent(this,CAMERA_UPDATE));
        }
      break;
      case ID_NUMBER_OF_SUBDIVISION:
        {
          if (m_NumberOfSubdivision >5)
          {
            wxMessageBox("Warning! the number of subdivision is hight, it could require too many memory!");
          }
        }
        break;
      case ID_SUBDIVIDE:
        {
          Algorithm();
        }
        break;
      case wxOK:
        OpStop(OP_RUN_OK);        
      break;
      case wxCANCEL:
        OpStop(OP_RUN_CANCEL);        
      break;
    }
  }
}
//----------------------------------------------------------------------------
void medOpSubdivide::OpStop(int result)
//----------------------------------------------------------------------------
{
  if (result == OP_RUN_CANCEL)
  {
    mafVMESurface *surface = mafVMESurface::SafeDownCast(m_Input);
    surface->Update();
    
    surface->SetData(m_OriginalPolydata,surface->GetTimeStamp());
    surface->Update();
  }

  HideGui();
  mafEventMacro(mafEvent(this,result));  
}
//----------------------------------------------------------------------------
void medOpSubdivide::OpUndo()
//----------------------------------------------------------------------------
{
  mafVMESurface *surface = mafVMESurface::SafeDownCast(m_Input);
  surface->Update();

  surface->SetData(m_OriginalPolydata,surface->GetTimeStamp());
  surface->Update();
}
//----------------------------------------------------------------------------
void medOpSubdivide::OpDo()
//----------------------------------------------------------------------------
{
  mafVMESurface *surface = mafVMESurface::SafeDownCast(m_Input);
  surface->Update();

  surface->SetData(m_ResultPolydata,surface->GetTimeStamp());
  surface->Update();
}
//----------------------------------------------------------------------------
void medOpSubdivide::Algorithm()
//----------------------------------------------------------------------------
{
  vtkLinearSubdivisionFilter *subdivisionFilter;
  vtkNEW(subdivisionFilter);

  mafVMESurface *surface = mafVMESurface::SafeDownCast(m_Input);
  surface->Update();

  vtkPolyData *polydataInput = vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData());
  polydataInput->Update();

  subdivisionFilter->SetInput(polydataInput);
  subdivisionFilter->SetNumberOfSubdivisions(m_NumberOfSubdivision);
  subdivisionFilter->Update();

  m_ResultPolydata->DeepCopy(subdivisionFilter->GetOutput());
  m_ResultPolydata->Update();

  surface->SetData(m_ResultPolydata,surface->GetTimeStamp());
  surface->Update();

  vtkDEL(subdivisionFilter);
}
