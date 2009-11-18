/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpEqualizeHistogram.cpp,v $
Language:  C++
Date:      $Date: 2009-11-18 10:22:04 $
Version:   $Revision: 1.1.2.1 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2009
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

#include "medOpEqualizeHistogram.h"
#include "mafNode.h"
#include "mafGUI.h"
#include "mafVMEVolumeGray.h"

#include "vtkMAFSmartPointer.h"
#include "vtkImageData.h"
#include "vtkImageCast.h"

#include "itkVTKImageToImageFilter.h"
#include "itkImageToVTKImageFilter.h"
#include "itkAdaptiveHistogramEqualizationImageFilter.h"

const unsigned int Dimension = 3;

typedef float InputPixelTypeFloat;
typedef itk::Image< InputPixelTypeFloat,Dimension > InputImageTypeFloat;
typedef itk::VTKImageToImageFilter< InputImageTypeFloat > ConvertervtkTOitk;
typedef itk::ImageToVTKImageFilter< InputImageTypeFloat > ConverteritkTOvtk;
typedef itk::AdaptiveHistogramEqualizationImageFilter<InputImageTypeFloat> HistogramEqualizationType;

//----------------------------------------------------------------------------
mafCxxTypeMacro(medOpEqualizeHistogram);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medOpEqualizeHistogram::medOpEqualizeHistogram(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo	= true;

  m_InputPreserving = false;

  m_Alpha = 0.0;
  m_Beta = 0.0;
  m_Radius[0] = m_Radius[1] = m_Radius[2] = 5;

  m_VolumeInput = NULL;
  m_VolumeOutput = NULL;
}
//----------------------------------------------------------------------------
medOpEqualizeHistogram::~medOpEqualizeHistogram()
//----------------------------------------------------------------------------
{
  mafDEL(m_VolumeOutput);
}
//----------------------------------------------------------------------------
bool medOpEqualizeHistogram::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return ( node != NULL && node->IsA("mafVMEVolumeGray") );
}
//----------------------------------------------------------------------------
mafOp *medOpEqualizeHistogram::Copy()   
//----------------------------------------------------------------------------
{
  return (new medOpEqualizeHistogram(m_Label));
}
//----------------------------------------------------------------------------
// Constants:
//----------------------------------------------------------------------------
enum U_OP_EQUALIZE_HISTOGRAM_ID
{
  ID_MY_OP = MINID,
  // ToDO: add your custom IDs...
  ID_ALPHA,
  ID_BETA,
  ID_RADIUS,
};
//----------------------------------------------------------------------------
void medOpEqualizeHistogram::OpRun()   
//----------------------------------------------------------------------------
{
  m_VolumeInput = mafVMEVolumeGray::SafeDownCast(m_Input);

  mafNEW(m_VolumeOutput);
  mafString name = m_VolumeInput->GetName();
  name<<" - Equalized Histogram";
  m_VolumeOutput->SetName(name);
  m_VolumeOutput->Update();

  CreateGui();
}
//----------------------------------------------------------------------------
void medOpEqualizeHistogram::CreateGui()
//----------------------------------------------------------------------------
{
  // interface:
  m_Gui = new mafGUI(this);

  // ToDO: add your custom widgets...
  m_Gui->Double(ID_ALPHA,_("Alpha"),&m_Alpha,0.0,1.0);
  m_Gui->Double(ID_BETA,_("Beta"),&m_Beta,0.0,1.0);
  m_Gui->Vector(ID_RADIUS,_("Radius"),m_Radius);
  
  m_Gui->OkCancel();

  ShowGui();
}
//----------------------------------------------------------------------------
void medOpEqualizeHistogram::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {	
    case wxOK:
      Algorithm();
      OpStop(OP_RUN_OK);        
      break;
    case wxCANCEL:
      OpStop(OP_RUN_CANCEL);        
      break;
    }
  }
}
//----------------------------------------------------------------------------
void medOpEqualizeHistogram::OpStop(int result)
//----------------------------------------------------------------------------
{
  HideGui();
  mafEventMacro(mafEvent(this,result));  
}
//----------------------------------------------------------------------------
void medOpEqualizeHistogram::OpUndo()
//----------------------------------------------------------------------------
{
  if (m_VolumeOutput != NULL)
  {
    m_VolumeOutput->ReparentTo(NULL);
  }
}
//----------------------------------------------------------------------------
void medOpEqualizeHistogram::OpDo()
//----------------------------------------------------------------------------
{
  if (m_VolumeOutput != NULL)
  {
    m_VolumeOutput->ReparentTo(m_Input);
  }
}
//----------------------------------------------------------------------------
void medOpEqualizeHistogram::Algorithm()
//----------------------------------------------------------------------------
{
  vtkImageData *im = vtkImageData::SafeDownCast(m_VolumeInput->GetOutput()->GetVTKData());
  im->Update();

  vtkMAFSmartPointer<vtkImageCast> vtkImageToFloat;
  vtkImageToFloat->SetOutputScalarTypeToFloat ();
  vtkImageToFloat->SetInput(im);
  vtkImageToFloat->Modified();
  vtkImageToFloat->Update();

  ConvertervtkTOitk::Pointer vtkTOitk = ConvertervtkTOitk::New();
  vtkTOitk->SetInput( vtkImageToFloat->GetOutput() );
  vtkTOitk->Update();

  HistogramEqualizationType::Pointer histeqFilter = HistogramEqualizationType::New();
  histeqFilter->GetOutput()->ReleaseDataFlagOn(); 
  HistogramEqualizationType::ImageSizeType radius;
  radius[0] = m_Radius[0];
  radius[1] = m_Radius[1];
  radius[2] = m_Radius[2];
  radius.Fill(0);
  histeqFilter->SetRadius(radius);   
  histeqFilter->SetAlpha(m_Alpha);
  histeqFilter->SetBeta(m_Beta); 
  histeqFilter->SetInput(vtkTOitk->GetOutput());
  histeqFilter->Update();

  ConverteritkTOvtk::Pointer itkTOvtk = ConverteritkTOvtk::New();
  itkTOvtk->SetInput( histeqFilter->GetOutput() );
  itkTOvtk->Update();

  vtkMAFSmartPointer<vtkImageData> imOut;
  imOut->DeepCopy(itkTOvtk->GetOutput());
  imOut->Update();

  m_VolumeOutput->SetData(imOut,m_VolumeInput->GetTimeStamp());
  m_VolumeOutput->Update();

}
