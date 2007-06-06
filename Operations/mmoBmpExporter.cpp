/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoBmpExporter.cpp,v $
  Language:  C++
  Date:      $Date: 2007-06-06 08:26:04 $
  Version:   $Revision: 1.1 $
  Authors:   Roberto Mucci
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mmoBmpExporter.h"
#include "wx/busyinfo.h"

#include "mafDecl.h"
#include "mmgGui.h"

#include "mafVMEVolumeGray.h"

#include "vtkMAFSmartPointer.h"

#include "vtkDataSet.h"
#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"

#include "vtkImageShiftScale.h."
#include "vtkPointData.h"
#include "vtkImageFlip.h"
#include "vtkBMPWriter.h"

#include <fstream>

//----------------------------------------------------------------------------
mmoBmpExporter::mmoBmpExporter(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType = OPTYPE_EXPORTER;
  m_Canundo = true;
  m_FileName = "";
  m_Input = NULL;
	
	m_ProposedDirectory = mafGetApplicationDirectory().c_str();
}
//----------------------------------------------------------------------------
mmoBmpExporter::~mmoBmpExporter()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool mmoBmpExporter::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
	return (node != NULL) && node->IsA("mafVMEVolumeGray");
}
//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

enum BMP_EXPORTER_ID
{
	ID_SINGLE_FILE = MINID,
	ID_INT,

};

//----------------------------------------------------------------------------
void mmoBmpExporter::OpRun()   
//----------------------------------------------------------------------------
{
	m_ProposedDirectory += m_Input->GetName();
	m_ProposedDirectory += ".bmp";
	if(!m_TestMode)
	{
		wxString wildc = "bmp file (*.bmp)|*.bmp";
		wxString file = mafGetSaveFile(m_ProposedDirectory,wildc).c_str(); 
		m_FileName = file;
    if (m_FileName != "")
    {
      OpStop(OP_RUN_OK);
    }
    else
    {
      OpStop(OP_RUN_CANCEL);
    }
	}
}
//----------------------------------------------------------------------------
void mmoBmpExporter::OpDo()   
//----------------------------------------------------------------------------
{					
	assert(m_Input);
	assert(m_FileName != "");
	this->SaveBmp();
}
//----------------------------------------------------------------------------
void mmoBmpExporter::OpUndo()   
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* mmoBmpExporter::Copy()   
//----------------------------------------------------------------------------
{
    mmoBmpExporter *cp = new mmoBmpExporter(m_Label);
    return cp;
}
//----------------------------------------------------------------------------
void mmoBmpExporter::OpStop(int result)
//----------------------------------------------------------------------------
{
  mafEventMacro(mafEvent(this,result));	
}
//----------------------------------------------------------------------------
void mmoBmpExporter::SaveBmp()
//----------------------------------------------------------------------------
{
  wxString path,name,ext;
  ::wxSplitPath(m_FileName,&path,&name,&ext);
  path+= _("\\");
  if(!m_TestMode)
    wxBusyInfo wait("Please wait, working...");

	mafVMEVolumeGray *volume=mafVMEVolumeGray::SafeDownCast(m_Input);
	volume->Update();

  vtkDataSet *ds = volume->GetVolumeOutput()->GetVTKData();
  vtkImageData *imageData = vtkImageData::SafeDownCast(ds);
  vtkRectilinearGrid *rg = vtkRectilinearGrid::SafeDownCast(ds);
  
  vtkMAFSmartPointer<vtkImageData> imageDataRg;
  
  double bounds[6];
  int dim[3];
  int xdim;
  int ydim;
  int zdim;
  int slice_size;

  volume->GetOutput()->GetBounds(bounds);

  double xmin = bounds[0];
  double xmax = bounds[1];
  double ymin = bounds[2];
  double ymax = bounds[3];
  double zmin = bounds[4];
  double zmax = bounds[5];	

 

	if (rg)
  {  
    rg->GetDimensions(dim);
    xdim = dim[0];
    ydim = dim[1];
    zdim = dim[2];
    slice_size = xdim*ydim;

    imageDataRg->SetOrigin(xmin, ymin, zmin);
    imageDataRg->SetDimensions(xdim, ydim, zdim);

    //setting the ImageDataacing
    double spacing_x = (xmax-xmin)/xdim;
    double spacing_y = (ymax-ymin)/ydim;

    imageDataRg->SetSpacing(spacing_x, spacing_y, 1);
    imageDataRg->SetScalarType(rg->GetPointData()->GetScalars()->GetDataType());
    imageDataRg->GetPointData()->SetScalars(rg->GetPointData()->GetScalars());
    imageDataRg->Update();

    imageData = imageDataRg;
	}
  else
  {
    imageData->GetDimensions(dim);
    xdim = dim[0];
    ydim = dim[1];
    zdim = dim[2];
  }


  vtkMAFSmartPointer<vtkImageFlip> imageFlip;
  imageFlip->SetFilteredAxis(1);

  //if volume data is not UNSIGNED_CHAR or UNSIGNED_SHORT
  //volume has to be casted to the desired range 

  if (imageData->GetScalarType() != VTK_UNSIGNED_CHAR) 
  {   
    vtkMAFSmartPointer<vtkImageShiftScale> pImageCast;

    imageData->Update(); //important
    double minmax[2];
    imageData->GetScalarRange(minmax);

    pImageCast->SetShift(-minmax[0]);
    pImageCast->SetScale(255/(minmax[1]-minmax[0]));
    pImageCast->SetOutputScalarTypeToUnsignedChar();

    pImageCast->ClampOverflowOn();
    pImageCast->SetInput(imageData);

    imageFlip->SetInput(pImageCast->GetOutput());

  }  //resampling   
  else 
  {
    imageFlip->SetInput(imageData);
  }  

  mafEventMacro(mafEvent(this,PROGRESSBAR_SHOW));
  vtkMAFSmartPointer<vtkBMPWriter> exporter;
  exporter->SetInput(imageFlip->GetOutput());
  mafEventMacro(mafEvent(this,BIND_TO_PROGRESSBAR, exporter));

  wxString prefix;

  prefix = wxString::Format("%s%s_%dx%d",path,name,xdim,ydim);

  exporter->SetFileDimensionality(2); // the writer will create a number of 2D images
  exporter->SetFilePattern("%s_%04d.bmp");

  char *c_prefix = (char*)( prefix.c_str() ); 
  exporter->SetFilePrefix(c_prefix);
  exporter->Write();

}
