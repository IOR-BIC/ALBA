/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterBmp
 Authors: Roberto Mucci
 
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

#include "albaOpExporterImages.h"
#include "albaGUIBusyInfo.h"

#include "albaDecl.h"
#include "albaGUI.h"

#include "albaVMEVolumeGray.h"

#include "vtkALBASmartPointer.h"
#include "vtkDataSet.h"
#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"

#include "vtkImageShiftScale.h"
#include "vtkPointData.h"
#include "vtkImageFlip.h"
#include "vtkBMPWriter.h"
#include "vtkDoubleArray.h"
#include "vtkDirectory.h"

#include <fstream>
#include "albaProgressBarHelper.h"
#include "wx\filename.h"
#include "vtkJPEGWriter.h"
#include "vtkPNGWriter.h"
#include "vtkTIFFWriter.h"
#include "wx/dir.h"

//----------------------------------------------------------------------------
albaOpExporterImages::albaOpExporterImages(const wxString &label) :
albaOp(label)
{
  m_OpType = OPTYPE_EXPORTER;
  m_Canundo = true;
  m_Input = NULL;
	m_FileFormat = BMP;
	m_DirName = "";
}
//----------------------------------------------------------------------------
albaOpExporterImages::~albaOpExporterImages()
{
}
//----------------------------------------------------------------------------
bool albaOpExporterImages::InternalAccept(albaVME*node)
{
	return (node != NULL) && (node->IsA("albaVMEVolumeGray") || node->IsA("albaVMEVolumeGray") || node->IsA("albaVMEImage"));
}
//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

enum BMP_EXPORTER_ID
{
	ID_SINGLE_FILE = MINID,
  ID_DIROPEN,
};

//----------------------------------------------------------------------------
void albaOpExporterImages::OpRun()   
{
  if(!m_TestMode)
  {
    //Crete GUI
    m_Gui = new albaGUI(this);
    
    m_Gui->DirOpen(ID_DIROPEN, "export dir", &m_DirName, _("choose dir") );

		const wxString fileFormats[] = { "BMP", "JPEG", "PNG", "TIFF"};

		m_Gui->Radio(ID_SINGLE_FILE, "file format", &m_FileFormat, 4, fileFormats);

    m_Gui->Label("");
    m_Gui->OkCancel(); 
     
    m_Gui->Divider();
    m_Gui->Enable(wxOK, false);

    ShowGui(); 
	}
}
//----------------------------------------------------------------------------
void albaOpExporterImages::OpDo()   
{					
	assert(m_Input);
	this->SaveImages();
}
//----------------------------------------------------------------------------
void albaOpExporterImages::OpUndo()   
{
}
//----------------------------------------------------------------------------
albaOp* albaOpExporterImages::Copy()   
{
    albaOpExporterImages *cp = new albaOpExporterImages(m_Label);
    return cp;
}
//----------------------------------------------------------------------------
void albaOpExporterImages::SaveImages()
{
  assert(m_DirName != "");

  albaGUIBusyInfo busy("Exporting files...", m_TestMode);

  // Check if directory is not empty and create subdirectory with input name
  vtkDirectory *dir = vtkDirectory::New();
  if (dir->Open(m_DirName.GetCStr()))
  {
    if (dir->GetNumberOfFiles() > 2) // More than "." and ".."
    {
      wxString inputName = m_Input->GetName();
      wxString newDirName = wxString(m_DirName.GetCStr()) + wxFILE_SEP_PATH + inputName;

      // Check if subdirectory exists and append progressive number if needed
      int counter = 0;
      wxString finalDirName = newDirName;

      while (wxDir::Exists(finalDirName))
      {
        counter++;
        finalDirName = albaString::Format("%s_%d", newDirName.ToAscii(), counter);
      }

      // Create the subdirectory
      if (!wxFileName::Mkdir(finalDirName, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL))
      {
        albaErrorMessage("Failed to create subdirectory: %s", finalDirName.ToAscii());
				return; // Exit if directory creation fails
      }

      m_DirName = finalDirName;
    }
  }
  dir->Delete();

  vtkDataSet *ds = m_Input->GetOutput()->GetVTKData();
  vtkImageData *imageData = vtkImageData::SafeDownCast(ds);
  vtkRectilinearGrid *rg = vtkRectilinearGrid::SafeDownCast(ds);

  vtkALBASmartPointer<vtkImageData> imageDataRg;

  double bounds[6];
  int dim[3], xdim, ydim, zdim, slice_size;

  m_Input->GetOutput()->GetBounds(bounds);

  double xmin = bounds[0];
  double xmax = bounds[1];
  double ymin = bounds[2];
  double ymax = bounds[3];
  double zmin = bounds[4];
  double zmax = bounds[5];

  //setting the ImageData
  double spacing_x, spacing_y;
  if (rg)
  {
    rg->Update();
    rg->GetDimensions(dim);
    xdim = dim[0];
    ydim = dim[1];
    zdim = dim[2];
    slice_size = xdim * ydim;

    imageDataRg->SetOrigin(xmin, ymin, zmin);
    imageDataRg->SetDimensions(xdim, ydim, zdim);

    spacing_x = (xmax - xmin) / xdim;
    spacing_y = (ymax - ymin) / ydim;

    imageDataRg->SetSpacing(spacing_x, spacing_y, 1);
    imageDataRg->SetScalarType(rg->GetPointData()->GetScalars()->GetDataType());
    imageDataRg->GetPointData()->SetScalars(rg->GetPointData()->GetScalars());
    imageDataRg->Update();

    imageData = imageDataRg;
  }
  else
  {
    imageData->Update();
    imageData->GetDimensions(dim);
    xdim = dim[0];
    ydim = dim[1];
    zdim = dim[2];

    spacing_x = imageData->GetSpacing()[0];
    spacing_y = imageData->GetSpacing()[1];
  }

  int size = xdim * ydim;
  imageData->GetScalarRange(m_ScalarRange);

  vtkALBASmartPointer<vtkImageData> imageSlice;
  imageSlice->SetScalarTypeToUnsignedChar();
  imageSlice->SetDimensions(xdim, ydim, 1);
  imageSlice->SetSpacing(spacing_x, spacing_y, 1);

  vtkALBASmartPointer<vtkDoubleArray> scalarSliceIn;
  scalarSliceIn->SetNumberOfTuples(size);

  albaString prefix;
  prefix.Printf("%s\\%s_%dx%d", m_DirName.GetCStr(), m_Input->GetName(), xdim, ydim);


  vtkALBASmartPointer<vtkImageFlip> imageFlip;
  imageFlip->SetFilteredAxis(1);

  //if volume data is not UNSIGNED_CHAR or UNSIGNED_SHORT
  //volume has to be casted to the desired range 

  if (imageData->GetScalarType() != VTK_UNSIGNED_CHAR)
  {
    vtkALBASmartPointer<vtkImageShiftScale> pImageCast;

    imageData->Update(); //important
    pImageCast->SetShift(-m_ScalarRange[0]);
    pImageCast->SetScale(255 / (m_ScalarRange[1] - m_ScalarRange[0]));
    pImageCast->SetOutputScalarTypeToUnsignedChar();

    pImageCast->ClampOverflowOn();
    pImageCast->SetInput(imageData);

    imageFlip->SetInput(pImageCast->GetOutput());

  }  //resampling   
  else
  {
    imageFlip->SetInput(imageData);
  }

	vtkImageWriter *exporter;

  switch (m_FileFormat)
  {
  case FileFormat::BMP:
    exporter = vtkBMPWriter::New();
    exporter->SetFilePattern("%s_%04d.bmp");
    break;
  case FileFormat::JPEG:
    exporter = vtkJPEGWriter::New();
    exporter->SetFilePattern("%s_%04d.jpeg");
    break;
  case FileFormat::PNG:
    exporter = vtkPNGWriter::New();
    exporter->SetFilePattern("%s_%04d.png");
    break;
  case FileFormat::TIFF:
    exporter = vtkTIFFWriter::New();
    exporter->SetFileName("%s");
    exporter->SetFilePattern("%s_%04d.tiff");
    break;
  default:
    exporter = vtkBMPWriter::New();
    exporter->SetFilePattern("%s_%04d.bmp");
    break;
  }
  

  exporter->SetInput(imageFlip->GetOutput());
  exporter->SetFileDimensionality(2); // the writer will create a number of 2D images
  exporter->SetFilePrefix((char *)prefix.GetCStr());

  exporter->Write();
}

//----------------------------------------------------------------------------
void albaOpExporterImages::OnEvent(albaEventBase *alba_event) 
{ 
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch (e->GetId())
    {
    case ID_DIROPEN:
      m_Gui->Enable(wxOK, !m_DirName.IsEmpty());
      break;
    case wxOK:
    {
      OpStop(OP_RUN_OK);
    }
    break;
    case wxCANCEL:
    {
      OpStop(OP_RUN_CANCEL);
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
char **albaOpExporterImages::GetIcon()
{
#include "pic/MENU_IMPORT_IMAGE.xpm"
	return MENU_IMPORT_IMAGE_xpm;

}