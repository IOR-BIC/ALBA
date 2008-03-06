/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpExporterBmp.cpp,v $
  Language:  C++
  Date:      $Date: 2008-03-06 11:55:06 $
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

#include "mafOpExporterBmp.h"
#include "wx/busyinfo.h"

#include "mafDecl.h"
#include "mmgGui.h"

#include "mafVMEVolumeGray.h"

#include "vtkMAFSmartPointer.h"
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

//----------------------------------------------------------------------------
mafOpExporterBmp::mafOpExporterBmp(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType = OPTYPE_EXPORTER;
  m_Canundo = true;
  m_Input = NULL;
  m_Offset = 0;
  m_8bit = 1;
	m_DirName = "";
}
//----------------------------------------------------------------------------
mafOpExporterBmp::~mafOpExporterBmp()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool mafOpExporterBmp::Accept(mafNode *node)
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
  ID_DIROPEN,
  ID_8BIT,
	ID_INT,
};

//----------------------------------------------------------------------------
void mafOpExporterBmp::OpRun()   
//----------------------------------------------------------------------------
{
  if(!m_TestMode)
  {
    //Crete GUI
    m_Gui = new mmgGui(this);
    m_Gui->SetListener(this);
    
    m_Gui->DirOpen(ID_DIROPEN, "export dir", &m_DirName, _("choose dir") );
   
    m_Gui->Bool(ID_8BIT, "grayscale", &m_8bit, 0, _("export in 8 bit gray scale format"));
    m_Gui->Integer(ID_INT,"offset: ", &m_Offset,MININT,MAXINT, _("only if 8 bit"));

    m_Gui->Label("");
    m_Gui->OkCancel(); 
     
    m_Gui->Divider();
    m_Gui->Enable(ID_INT,false);
    m_Gui->Enable(wxOK, false);

    ShowGui(); 
	}
}
//----------------------------------------------------------------------------
void mafOpExporterBmp::OpDo()   
//----------------------------------------------------------------------------
{					
	assert(m_Input);
	this->SaveBmp();
}
//----------------------------------------------------------------------------
void mafOpExporterBmp::OpUndo()   
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* mafOpExporterBmp::Copy()   
//----------------------------------------------------------------------------
{
    mafOpExporterBmp *cp = new mafOpExporterBmp(m_Label);
    return cp;
}
//----------------------------------------------------------------------------
void mafOpExporterBmp::SaveBmp()
//----------------------------------------------------------------------------
{
  assert(m_DirName != "");
 
  wxString path,name,ext;
  ::wxSplitPath(m_DirName,&path,&name,&ext);
  path+= _("\\");
  path+= name;
  path+= _("\\");
  /*
#ifndef TEST_MODE
    wxBusyInfo wait(_("Please wait, working..."));
#endif TEST_MODE
    */

  mafVMEVolumeGray *volume=mafVMEVolumeGray::SafeDownCast(m_Input);
  volume->Update();

  vtkDataSet *ds = volume->GetVolumeOutput()->GetVTKData();
  vtkImageData *imageData = vtkImageData::SafeDownCast(ds);
  vtkRectilinearGrid *rg = vtkRectilinearGrid::SafeDownCast(ds);

  vtkMAFSmartPointer<vtkImageData> imageDataRg;

  double bounds[6];
  int dim[3], xdim, ydim, zdim, slice_size;

  volume->GetOutput()->GetBounds(bounds);

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
    slice_size = xdim*ydim;

    imageDataRg->SetOrigin(xmin, ymin, zmin);
    imageDataRg->SetDimensions(xdim, ydim, zdim);

    spacing_x = (xmax-xmin)/xdim;
    spacing_y = (ymax-ymin)/ydim;

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
  
  vtkMAFSmartPointer<vtkImageData> imageSlice;
  imageSlice->SetScalarTypeToUnsignedChar();
  imageSlice->SetDimensions(xdim, ydim, 1);
  imageSlice->SetSpacing(spacing_x, spacing_y, 1);

  vtkMAFSmartPointer<vtkDoubleArray> scalarSliceIn;
  scalarSliceIn->SetNumberOfTuples(size);

  wxString prefix;
			prefix = wxString::Format("%s%s_%dx%d",path,name,xdim,ydim);


  if (m_8bit == 0)
  {
    vtkMAFSmartPointer<vtkImageFlip> imageFlip;
    imageFlip->SetFilteredAxis(1);

    //if volume data is not UNSIGNED_CHAR or UNSIGNED_SHORT
    //volume has to be casted to the desired range 

    if (imageData->GetScalarType() != VTK_UNSIGNED_CHAR) 
    {   
      vtkMAFSmartPointer<vtkImageShiftScale> pImageCast;

      imageData->Update(); //important
      pImageCast->SetShift(-m_ScalarRange[0]);
      pImageCast->SetScale(255/(m_ScalarRange[1]-m_ScalarRange[0]));
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
    //mafEventMacro(mafEvent(this,BIND_TO_PROGRESSBAR, exporter));
    exporter->SetInput(imageFlip->GetOutput());
    exporter->SetFileDimensionality(2); // the writer will create a number of 2D images
    exporter->SetFilePattern("%s_%04d.bmp");
    exporter->SetFilePrefix((char*)prefix.c_str());
     
    exporter->Write();
  }
  else
  {
    int counter = 0;
    double tuple;
    wxString fileName;
    long progress = 0;

    mafEventMacro(mafEvent(this,PROGRESSBAR_SHOW));
    
    for ( int z = 0 ; z < zdim; z++)
    {
      if (mafFloatEquals(fmod(z,10.0f),0.0f))
      {
        progress = (z*100)/zdim;
        mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,progress));
      }
      for (int i = counter, n = 0; i < (counter + size); i++,n++)
      {
        tuple = imageData->GetPointData()->GetTuple(i)[0];
        scalarSliceIn->InsertTuple(n, &tuple);
      }
      counter += size;
      imageSlice->GetPointData()->SetScalars(scalarSliceIn);

      int fileNumber = z + m_Offset;

      fileName = wxString::Format("%s_%04d.bmp",prefix, fileNumber);

      
      WriteImageDataAsMonocromeBitmap(imageSlice, fileName.c_str());
      scalarSliceIn->Reset();
    }
    mafEventMacro(mafEvent(this,PROGRESSBAR_HIDE));
  }
}

//----------------------------------------------------------------------------
void mafOpExporterBmp::OnEvent(mafEventBase *maf_event) 
//----------------------------------------------------------------------------
{ 
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case ID_8BIT:
        m_Gui->Enable(ID_INT, m_8bit != 0);
        break;
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
          mafEventMacro(*e); 
        }
        break;
    }
  }
}
// --------------------------------------------------------------
bool mafOpExporterBmp::WriteImageDataAsMonocromeBitmap(vtkImageData *img, mafString filename)
// --------------------------------------------------------------
{
#ifdef WIN32
  // check filename
  if( !filename ) return false;
  FILE *f = fopen( filename, "wb");
  if(!f) return false;
  fclose(f); // to be reopended later


  // check the img
  if(!img) return false;
  if(!img->GetPointData()) return false;
  if(!img->GetPointData()->GetScalars() ) return false;
  if(!img->GetPointData()->GetScalars()->GetNumberOfTuples() ) return false;
  if( img->GetPointData()->GetScalars()->GetNumberOfComponents() != 1 ) return false;

  int *img_dim = img->GetDimensions();
  int img_w = img_dim[0];
  int img_h = img_dim[1];
  int img_z = img_dim[2];
  if( img_w==0 || img_h==0 || img_z!=1 ) return false;
  
  double spacing_x = img->GetSpacing()[0];
  double spacing_y = img->GetSpacing()[1];

  vtkMAFSmartPointer<vtkDataArray> scal = img->GetPointData()->GetScalars();
  int n_scal =  scal->GetNumberOfTuples();
  double *tuple = scal->GetTuple(0);

  if( n_scal < img_w * img_h ) return false;         // wrong number of scalars -- improbable   
  if( m_ScalarRange[0] == m_ScalarRange[1] ) return false; // all scalars are the same value -- probable error   

  // (scalar_value + scal_sum) * scal_mul == scalar_value normalized in 0..255
  double scal_sum = -m_ScalarRange[0];
  double scal_mul = 255.0 / (m_ScalarRange[1]-m_ScalarRange[0]); //Scalar range of the Volume


  ////////////////////////////////////////
  //Write the Bitmap
  ////////////////////////////////////////

  // a Windows Bitmap file is:
  //   - a BITMAPFILEHEADER
  //   - followed by a BITMAPINFO
  //   - which is a BITMAPFILEHEADER followed by the PALETTE
  //   - then follows the byte of the image body. 
  //   - where ROWS are writtem bottom to top
  //   - and each row must O-padded to be DWORD aligned 
  //
  //   in this code the BITMAPINFO will allocate enought space 
  //   to hold both the palette and the image-body

  // from windows.h :
  //typedef struct {
  //  WORD    bfType;
  //  DWORD   bfSize;
  //  WORD    bfReserved1;
  //  WORD    bfReserved2;
  //  DWORD   bfOffBits;
  //} BITMAPFILEHEADER;

  //typedef struct {
  //  BITMAPINFOHEADER    bmiHeader;
  //  RGBQUAD             bmiColors[1]; // first element of a ?lenght vector
  //} BITMAPINFO;

  // Row Lenght in File
  unsigned int BytePerRow=img_w;          
  if (BytePerRow%4 != 0) 
    BytePerRow=( BytePerRow /4 +1 ) * 4;

  long bih_size     = sizeof(BITMAPINFOHEADER);
  long palette_size = 256 * sizeof(RGBQUAD); 
  long img_size     = BytePerRow * img_h;
  long bi_size      = bih_size + palette_size + img_size;

  BITMAPFILEHEADER hdr;
  hdr.bfType       = 0x4d42;
  hdr.bfSize       = sizeof (BITMAPFILEHEADER) + bi_size;
  hdr.bfReserved1  = 0;
  hdr.bfReserved2  = 0;
  hdr.bfOffBits    = sizeof (BITMAPFILEHEADER) + bih_size + palette_size;

  BITMAPINFO *bi = (BITMAPINFO *) malloc( bi_size );
  if( !bi ) return false;

  BITMAPINFOHEADER *bih = (BITMAPINFOHEADER *)bi;
  bih->biSize			      =(DWORD)40;
  bih->biWidth			    = img_w;
  bih->biHeight			    = img_h;
  bih->biPlanes			    = 1;      // 8 bit per pixel ( using palette )
  bih->biBitCount		    = 8;      // 8 bit per pixel ( using palette )
  bih->biCompression	  = BI_RGB; // no compression
  bih->biSizeImage		  = BytePerRow * img_h;  // image-body size in file
  bih->biXPelsPerMeter	= 1000/spacing_x;  // Number of pixels per meter in X direction (width)
  bih->biYPelsPerMeter	= 1000/spacing_y;  // Number of pixels per meter in Y direction (height)
  bih->biClrUsed		    = 256;
  bih->biClrImportant	  = 0;

  // setup the palette with a grayscale   
  for(int i=0; i<256; i++)
  {
    bi->bmiColors[i].rgbRed      =  i;
    bi->bmiColors[i].rgbGreen    =  i;
    bi->bmiColors[i].rgbBlue     =  i;
    bi->bmiColors[i].rgbReserved =  0;
  }

  //fill the image body
  unsigned char *p = (unsigned char*)( bi );
  p += (  bih_size + palette_size );
  for(int j=img_h-1; j>=0; j--) // rows must be written from bottom to top
  {
    for(int i=0; i<img_w; i++)
    {
      double s = scal->GetTuple(img_w*j + i)[0];
      *p++ = (s + scal_sum) * scal_mul;
    }
    // padding bytes
    for(int k=img_w; k<BytePerRow; k++)
    {
      *p++ = 0;
    }
  }

  //write the image
  f = fopen( filename, "wb");
  fwrite(&hdr,1,sizeof (BITMAPFILEHEADER),f);

  int chunksize  = 10000;
  int num_chunk  = bi_size/chunksize;
  int last_chunk = bi_size%chunksize;
  int written = 0;
  unsigned char *buff = (unsigned char*)( bi );

  for( int i=0; i<num_chunk; i++ )
  {
    written = fwrite(buff,1,chunksize,f);
    //if(written != chunksize)
    //    int place_for_breakpoint =0;
    buff +=chunksize;
  }
  written = fwrite(buff,1,last_chunk,f);
  //if(written != last_chunk)
  //  int place_for_breakpoint =0;

  fclose(f);
  free(bi);
  return true;
#else
return false;
#endif
}
