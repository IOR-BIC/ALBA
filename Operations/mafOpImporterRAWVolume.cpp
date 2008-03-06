/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpImporterRAWVolume.cpp,v $
  Language:  C++
  Date:      $Date: 2008-03-06 11:55:06 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani     Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafOpImporterRAWVolume.h"
#include "wx/busyinfo.h"

#include "mmgGui.h"
#include "mafRWI.h"
#include "mmgDialogPreview.h"
#include "mafTagItem.h"
#include "mafTagArray.h"

#include "mafVMEVolumeGray.h"
#include "mafVMEVolumeRGB.h"

#include "vtkMAFSmartPointer.h"
#include "vtkRenderer.h"
#include "vtkImageReader.h"
#include "vtkImageData.h"
#include "vtkTexture.h" 
#include "vtkPolyDataMapper.h"
#include "vtkWindowLevelLookupTable.h"
#include "vtkPlaneSource.h"
#include "vtkImageToStructuredPoints.h"
#include "vtkDataArray.h"
#include "vtkPointData.h"
#include "vtkRectilinearGrid.h"
#include "vtkStructuredPoints.h"
#include "vtkDoubleArray.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpImporterRAWVolume);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpImporterRAWVolume::mafOpImporterRAWVolume(const wxString &label) : mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType			= OPTYPE_IMPORTER;
	m_Canundo			= true;
	m_RawFile			= mafGetApplicationDirectory().c_str();
	m_VolumeGray	= NULL;
  m_VolumeRGB   = NULL;
  m_GuiSlider   = NULL;

	m_Endian		 = 1;
	m_ScalarType = 1;
	m_Signed	   = 0;
	m_FileHeader = 0;
	
	m_DataSpacing[0] = m_DataSpacing[1] = m_DataSpacing[2] = 1.0;

	m_DataDimemsion[0] = 512;
	m_DataDimemsion[1] = 512;
	m_DataDimemsion[2] = 1;

  m_SliceVOI[0] = 0;
  m_SliceVOI[1] = 1;

	m_NumberOfByte = 0;

	m_BuildRectilinearGrid = false;
	m_CoordFile = "";
	
	m_Dialog = NULL;
	m_Reader = NULL;
	m_Actor	 = NULL;

	m_CurrentSlice = 0;
	m_SliceSlider	 = NULL;
}
//----------------------------------------------------------------------------
mafOpImporterRAWVolume::~mafOpImporterRAWVolume()
//----------------------------------------------------------------------------
{
	mafDEL(m_VolumeGray);
  mafDEL(m_VolumeRGB);
}
//----------------------------------------------------------------------------
mafOp *mafOpImporterRAWVolume::Copy()
//----------------------------------------------------------------------------
{
	return new mafOpImporterRAWVolume(m_Label);
}
//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------
enum RAW_IMPORTER_ID
{
	ID_FILE = MINID,
	ID_BITS,
	ID_SCALAR_TYPE,
	ID_SIGNED,
	ID_DIM,
  ID_VOI_SLICES,
	ID_SPC,
	ID_HEADER,
	ID_GUESS,
	ID_SLICE,
	ID_COORD,
};
//----------------------------------------------------------------------------
void mafOpImporterRAWVolume::OpRun()
//----------------------------------------------------------------------------
{
	m_Gui = new mmgGui(this);

	//dialog +++++++++++++++++++++++++++++++++++++++
	m_Dialog = new mmgDialogPreview(_("raw importer"), mafCLOSEWINDOW | mafRESIZABLE | mafUSEGUI | mafUSERWI);

	//Preview Pipeline ++++++++++++++++++++++++++++++
	vtkNEW(m_Reader);
	vtkNEW(m_LookupTable);

  vtkMAFSmartPointer<vtkTexture> texture;
	texture->SetInput(m_Reader->GetOutput());
	texture->InterpolateOn();
  texture->MapColorScalarsThroughLookupTableOn();
  texture->SetLookupTable((vtkLookupTable *)m_LookupTable);

	vtkMAFSmartPointer<vtkPlaneSource> plane;

	vtkMAFSmartPointer<vtkPolyDataMapper> mapper;
	mapper ->SetInput(plane->GetOutput());

	vtkNEW(m_Actor);
	m_Actor->SetMapper(mapper);
	m_Actor->SetTexture(texture);
	m_Actor->VisibilityOff();
	
	//GUI +++++++++++++++++++++++++++++++++++++++++++
	m_Gui->Show(true);
	m_Gui->Divider(0);
  mafString wildc = _("Raw data (*.raw)|*.raw|All files (*.*)|*.*");
	m_Gui->FileOpen(ID_FILE,_("file"),&m_RawFile,wildc);

  wxString endian_choices[2] = {_("Big Endian"),_("Little Endian")};
  wxString scalar_choices[5] = {_("char"),_("short"),_("int"),_("float"),_("double")};
	m_Gui->Divider(0);
	m_Gui->Combo(ID_BITS,_("endian"),&m_Endian,2,endian_choices);
	m_Gui->Combo(ID_SCALAR_TYPE,"scalar type",&m_ScalarType,5,scalar_choices);
	m_Gui->Bool(ID_SIGNED,"signed",&m_Signed);

	m_Gui->Divider(0);
	m_Gui->Label(_("dimensions (x,y,z)"));
	m_Gui->Vector(ID_DIM, "",m_DataDimemsion,1,10000); 
  m_Gui->VectorN(ID_VOI_SLICES,_("slices VOI"),m_SliceVOI,2,0,MAXINT,_("define the range of slice to import."));
	
	m_Gui->Divider(0);
	m_Gui->Label(_("spacing in mm/pixel (x,y,z)"));
	m_Gui->Vector(ID_SPC, "",m_DataSpacing,0.0000001, 100000,-1);

	m_Gui->Button(ID_COORD,_("load"),_("z coord file:"),_("load the file for non regulary spaced raw volume"));

	m_Gui->Divider(0);
	m_Gui->Button(ID_GUESS,_("guess"),_("header size"));
	m_Gui->Integer(ID_HEADER,_(" "),&m_FileHeader,0);

	m_Gui->Label("");
	m_Gui->OkCancel();

	//slice slider +++++++++++++++++++++++++++++++++++++++++++
  m_GuiSlider = new mmgGui(this);
  m_SliceSlider = m_GuiSlider->Slider(ID_SLICE,_("slice num"),&m_CurrentSlice,0);
  m_GuiSlider->Show(true);
  m_GuiSlider->Reparent(m_Dialog);
  wxBoxSizer *slider_sizer = new wxBoxSizer( wxHORIZONTAL );
  slider_sizer->Add(m_GuiSlider, 0, wxEXPAND);
   
	EnableWidgets(false);

	//sizing & positioning +++++++++++++++++++++++++++++++++++++++++++
	m_Gui->Update();
   
  m_Dialog->GetGui()->AddGui(m_Gui);
  m_Dialog->GetRWI()->SetSize(0,0,400,400);
  m_Dialog->m_RwiSizer->Add(slider_sizer, 0, wxEXPAND);
  m_Dialog->GetRWI()->m_RwiBase->SetMouse(m_Mouse);
  m_Dialog->GetRWI()->m_RenFront->AddActor(m_Actor);
  m_Dialog->GetRWI()->CameraSet(CAMERA_CT);

  int x_pos,y_pos,w,h;
  mafGetFrame()->GetPosition(&x_pos,&y_pos);
  m_Dialog->GetSize(&w,&h);
  m_Dialog->SetSize(x_pos+5,y_pos+5,w,h);

	//show the dialog (show return when the user choose ok or cancel ) ++++++++
	m_Dialog->ShowModal();

	//Import the file if required +++++++++++++++++++++++++++++++++
	int res = (m_Dialog->GetReturnCode() == wxID_OK) ? OP_RUN_OK : OP_RUN_CANCEL;
	
	if(res == OP_RUN_OK)
    if( !Import() ) res = OP_RUN_CANCEL;

  //cleanup +++++++++++++++++++++++++++++++++  
	m_Dialog->GetRWI()->m_RenFront->RemoveActor(m_Actor);
	vtkDEL(m_Reader);
  vtkDEL(m_LookupTable);
	vtkDEL(m_Actor);
  cppDEL(m_GuiSlider);
	cppDEL(m_Dialog);

  //finish the Run +++++++++++++++++++++++++++++++++
	mafEventMacro(mafEvent(this,res));
}
//----------------------------------------------------------------------------
void mafOpImporterRAWVolume::EnableWidgets(bool enable)
//----------------------------------------------------------------------------
{
  assert(m_Gui && m_SliceSlider);
	
	m_Gui->Enable(ID_BITS,		    enable);
	m_Gui->Enable(ID_SCALAR_TYPE,	enable);
	m_Gui->Enable(ID_SIGNED,	    enable);
	m_Gui->Enable(ID_DIM,		      enable); 
  m_Gui->Enable(ID_VOI_SLICES,  enable); 
	m_Gui->Enable(ID_SPC,		      enable); 
	m_Gui->Enable(ID_GUESS,		    enable);
	m_Gui->Enable(ID_HEADER,	    enable);
	m_Gui->Enable(wxOK,		        enable);

	m_GuiSlider->Enable(ID_SLICE,enable);
}
//----------------------------------------------------------------------------
void mafOpImporterRAWVolume::	OnEvent(mafEventBase *maf_event) 
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {	  
      case ID_FILE:
        EnableWidgets(true);
        m_CurrentSlice = m_DataDimemsion[2]/2;
        m_Reader->SetFileName(m_RawFile);
        UpdateReader();
        m_Actor->VisibilityOn();
        m_Dialog->GetRWI()->CameraReset();
        m_SliceSlider->SetRange(0,m_DataDimemsion[2] - 1);	 
        m_Gui->Update();
      break;
      case ID_BITS:
      case ID_SCALAR_TYPE:
      case ID_SIGNED:
      case ID_SPC:
        m_Gui->Update();
        UpdateReader();
        m_Dialog->GetRWI()->CameraUpdate();
      break;
      case ID_DIM:
        if( m_CurrentSlice >= m_DataDimemsion[2] ) 
          m_CurrentSlice = m_DataDimemsion[2]-1;
        m_SliceVOI[1] = m_DataDimemsion[2];
        m_SliceSlider->SetRange(0,m_DataDimemsion[2] - 1);
        m_Gui->Update();
        UpdateReader();
        m_Dialog->GetRWI()->CameraUpdate();
      break;
      case ID_SLICE:
        m_GuiSlider->Update();
        UpdateReader();
        m_Dialog->GetRWI()->CameraUpdate();
      break;
      case ID_GUESS:
      {				
        int len = GetFileLength(m_RawFile);
        m_FileHeader = len - (m_DataDimemsion[0]*m_DataDimemsion[1]*m_DataDimemsion[2]*m_NumberOfByte);
        m_Gui->Update();
        m_Reader->SetHeaderSize(m_FileHeader);
        m_Reader->Update();
      }
      break;
	    case ID_COORD:
			{
				wxString dir = mafGetApplicationDirectory().c_str();
				dir += _("/Data/External");
				wxString wildc =_("Z_coordinates (*.txt)|*.txt");
				wxString file = mafGetOpenFile(dir,wildc,_("Open Z coordinates file")).c_str();
				if(!file.IsEmpty())
				{
					m_CoordFile = file;
					m_BuildRectilinearGrid = true;
					m_DataSpacing[2] = 1.0;

					const char *nome = m_CoordFile.GetCStr();
					std::ifstream f_in;
					f_in.open(nome);
					char title[256];
					f_in.getline(title,256);

          if(strcmp(title,_("Z coordinates:"))!=0)
					{
						wxMessageDialog dialog(NULL,_("This is not a Z coordinates file!"),"",wxOK|wxICON_ERROR);
						dialog.ShowModal();
						f_in.close();
						return;
					}

					int j = 0;
					double value;
					while(!f_in.eof())
					{
						f_in>>value;
						j++;
					}
					
					j-=1;

					if(j!=m_DataDimemsion[2])
					{
						wxMessageDialog dialog(NULL,_("z dimension is not correct!"),"",wxOK|wxICON_ERROR);
						dialog.ShowModal();
						m_DataDimemsion[2]=j;
						m_Gui->Update();
					}
					f_in.close();
				}
			}
			break;
      case wxOK:
        m_Dialog->EndModal(wxID_OK);
      break;
      case wxCANCEL:
        m_Dialog->EndModal(wxID_CANCEL);
      break;
      default:
        mafEventMacro(*e);
      break;
    }	
  }
}
//----------------------------------------------------------------------------
void mafOpImporterRAWVolume::UpdateReader()
//----------------------------------------------------------------------------
{
	switch(m_ScalarType)
	{
		case CHAR_SCALAR:
			m_Reader->SetDataScalarType( (m_Signed) ? VTK_CHAR : VTK_UNSIGNED_CHAR);
			m_NumberOfByte = 1;
			m_Gui->Enable(ID_SIGNED,true);
		break;
		case SHORT_SCALAR:
			m_Reader->SetDataScalarType( (m_Signed) ? VTK_SHORT : VTK_UNSIGNED_SHORT);
			if(m_Endian == 0)
				m_Reader->SetDataByteOrderToBigEndian();
			else
				m_Reader->SetDataByteOrderToLittleEndian();
			m_NumberOfByte = 2;
			m_Gui->Enable(ID_SIGNED,true);
		break;
		case INT_SCALAR:
			m_Reader->SetDataScalarType( (m_Signed) ? VTK_INT : VTK_UNSIGNED_INT);
			if(m_Endian == 0)
				m_Reader->SetDataByteOrderToBigEndian();
			else
				m_Reader->SetDataByteOrderToLittleEndian();
			m_NumberOfByte = 4;
			m_Gui->Enable(ID_SIGNED,true);
		break;
		case FLOAT_SCALAR:
			m_Reader->SetDataScalarType(VTK_FLOAT);
			if(m_Endian == 0)
				m_Reader->SetDataByteOrderToBigEndian();
			else
				m_Reader->SetDataByteOrderToLittleEndian();
			m_NumberOfByte = 8;
			m_Gui->Enable(ID_SIGNED,false);
		break;
		case DOUBLE_SCALAR:
			m_Reader->SetDataScalarType(VTK_DOUBLE);
			if(m_Endian == 0)
				m_Reader->SetDataByteOrderToBigEndian();
			else
				m_Reader->SetDataByteOrderToLittleEndian();
			m_NumberOfByte = 8;
			m_Gui->Enable(ID_SIGNED,false);
		break;
	}

	m_Reader->SetDataExtent(0, m_DataDimemsion[0] - 1, 0, m_DataDimemsion[1] - 1, 0, m_DataDimemsion[2] - 1);  
	m_Reader->SetDataSpacing(m_DataSpacing);
	m_Reader->SetHeaderSize(m_FileHeader);
	m_Reader->SetFileDimensionality(3);
	m_Reader->SetDataVOI(0, m_DataDimemsion[0] - 1, 0, m_DataDimemsion[1] - 1, m_CurrentSlice, m_CurrentSlice);  
	m_Reader->Update();

  double range[2];
  m_Reader->GetOutput()->GetScalarRange(range);

  m_LookupTable->SetTableRange(range);
  m_LookupTable->SetWindow(range[1] - range[0]);
  m_LookupTable->SetLevel((range[1] + range[0]) / 2.0);
  m_LookupTable->Build();
}
//----------------------------------------------------------------------------
bool mafOpImporterRAWVolume::Import()
//----------------------------------------------------------------------------
{
	if(!m_TestMode)
		wxBusyInfo wait(_("Importing RAW data, please wait..."));

 	vtkMAFSmartPointer<vtkImageReader> reader;
	reader->SetFileName(m_RawFile);  

	switch(m_ScalarType)
	{
		case CHAR_SCALAR:
			reader->SetDataScalarType( (m_Signed) ? VTK_CHAR : VTK_UNSIGNED_CHAR);
		break;
		case SHORT_SCALAR:
			reader->SetDataScalarType( (m_Signed) ? VTK_SHORT : VTK_UNSIGNED_SHORT);
			if(m_Endian == 0)
				reader->SetDataByteOrderToBigEndian();
			else
				reader->SetDataByteOrderToLittleEndian();
		break;
		case INT_SCALAR:
			reader->SetDataScalarType( (m_Signed) ? VTK_INT : VTK_UNSIGNED_INT);
			if(m_Endian == 0)
				reader->SetDataByteOrderToBigEndian();
			else
				reader->SetDataByteOrderToLittleEndian();
		break;
		case FLOAT_SCALAR:
			reader->SetDataScalarType(VTK_FLOAT);
      if(m_Endian == 0)
        reader->SetDataByteOrderToBigEndian();
      else
        reader->SetDataByteOrderToLittleEndian();
		break;
		case DOUBLE_SCALAR:
			reader->SetDataScalarType(VTK_DOUBLE);
      if(m_Endian == 0)
        reader->SetDataByteOrderToBigEndian();
      else
        reader->SetDataByteOrderToLittleEndian();
		break;
	}

  reader->SetDataExtent(0, m_DataDimemsion[0] - 1, 0, m_DataDimemsion[1] - 1, 0, m_DataDimemsion[2] - 1);
	reader->SetDataSpacing(m_DataSpacing);
	reader->SetHeaderSize(m_FileHeader);
	reader->SetFileDimensionality(3);
  reader->SetDataVOI(0, m_DataDimemsion[0] - 1, 0, m_DataDimemsion[1] - 1, m_SliceVOI[0], m_SliceVOI[1] - 1);
  if(!this->m_TestMode)
	{
		mafEventMacro(mafEvent(this,PROGRESSBAR_SHOW));
		mafEventMacro(mafEvent(this,BIND_TO_PROGRESSBAR,reader));
	}
//  reader->SetDataVOI(0, m_DataDimemsion[0] - 1, 0, m_DataDimemsion[1] - 1, 0, m_SliceVOI[1] - m_SliceVOI[0] - 1);
//  reader->SetDataOrigin(0.0,0.0,m_SliceVOI[0]*m_DataSpacing[2]);
	reader->Update();

  vtkMAFSmartPointer<vtkImageToStructuredPoints> image_to_sp;
  image_to_sp->SetInput(reader->GetOutput());
  image_to_sp->Update();

	if(m_BuildRectilinearGrid)
	{
		// conversion from vtkStructuredPoints to vtkRectilinearGrid

		vtkMAFSmartPointer<vtkStructuredPoints> structured_data = image_to_sp->GetOutput();
		vtkMAFSmartPointer<vtkPointData> data = structured_data->GetPointData();
		vtkMAFSmartPointer<vtkDataArray> scalars = data->GetScalars();

		vtkMAFSmartPointer<vtkDoubleArray> XDoubleArray;
		vtkMAFSmartPointer<vtkDoubleArray> YDoubleArray;
		vtkMAFSmartPointer<vtkDoubleArray> ZDoubleArray;

		double origin[3];
		structured_data->GetOrigin(origin);
			
		double currentValue;
			
		for (int ix = 0; ix < m_DataDimemsion[0]; ix++)
		{
			currentValue =  origin[0]+((double)ix)*m_DataSpacing[0];
			XDoubleArray->InsertNextValue(currentValue);					
		}	

		for (int iy = 0; iy < m_DataDimemsion[1]; iy++)
		{
			currentValue =  origin[1]+((double)iy)*m_DataSpacing[1];
			YDoubleArray->InsertNextValue(currentValue);					
		}	
		const char* nome = (m_CoordFile);
		std::ifstream f_in;
		f_in.open(nome);

		char title[256];
		f_in.getline(title,256);			
			
		//z array is read from a file	
		currentValue = origin[2];

		for (int i = 0; i < m_DataDimemsion[2]; i++)
		{
			f_in>> currentValue;
			ZDoubleArray->InsertNextValue(currentValue);
				
		}

		f_in.close();

		vtkMAFSmartPointer<vtkRectilinearGrid> rectilinear_data;

		rectilinear_data->SetXCoordinates(XDoubleArray);
		rectilinear_data->SetYCoordinates(YDoubleArray);
		rectilinear_data->SetZCoordinates(ZDoubleArray);
			
		int dim[3];

		structured_data->GetDimensions(dim);
		rectilinear_data->SetDimensions(dim);
		rectilinear_data->GetPointData()->SetScalars(scalars);

		mafNEW(m_VolumeGray);
		mafNEW(m_VolumeRGB);
		if (m_VolumeGray->SetDataByDetaching((vtkDataSet *)rectilinear_data,0) == MAF_OK)
		{
			m_Output = m_VolumeGray;
		}
		else if (m_VolumeRGB->SetDataByDetaching((vtkDataSet *)rectilinear_data,0) == MAF_OK)
		{
			m_Output = m_VolumeRGB;
		}
		else
		{
			wxMessageBox(_("Some importing error occurred!!"), _("Warning!"));
			return false;
		}
		
	} 
	else
	{
		mafNEW(m_VolumeGray);
		mafNEW(m_VolumeRGB);
		if (m_VolumeGray->SetDataByDetaching((vtkDataSet *)image_to_sp->GetOutput(),0) == MAF_OK)
		{
			m_Output = m_VolumeGray;
		}
		else if (m_VolumeRGB->SetDataByDetaching((vtkDataSet *)image_to_sp->GetOutput(),0) == MAF_OK)
		{
			m_Output = m_VolumeRGB;
		}
		else
		{
			wxMessageBox(_("Some importing error occurred!!"), _("Warning!"));
			return false;
		}
	}

	mafTagItem tag_Nature;
	tag_Nature.SetName("VME_NATURE");
	tag_Nature.SetValue("NATURAL");

	wxString name, ext, path;
  wxSplitPath(m_RawFile.GetCStr(),&path,&name,&ext);
  m_Output->SetName(name.c_str());
  m_Output->GetTagArray()->SetTag(tag_Nature);
  m_Output->ReparentTo(m_Input);
	if(!m_TestMode)
		mafEventMacro(mafEvent(this,PROGRESSBAR_HIDE));
	return true;
}
//----------------------------------------------------------------------------
int mafOpImporterRAWVolume::GetFileLength(const char * filename) 
//----------------------------------------------------------------------------
{
	int l,m,len;
	ifstream file (filename, ios::in|ios::binary);
	l = file.tellg();
	file.seekg (0, ios::end);
	m = file.tellg();
	file.close();
	len = (m-l);
	return len;
}
//----------------------------------------------------------------------------
void mafOpImporterRAWVolume::SetFileName(const char *raw_file)
//----------------------------------------------------------------------------
{
  m_RawFile = raw_file;
}
//----------------------------------------------------------------------------
void mafOpImporterRAWVolume::SetScalarType(int scalar_type)
//----------------------------------------------------------------------------
{
  if (scalar_type < CHAR_SCALAR || scalar_type > DOUBLE_SCALAR)
  {
    return;
  }
  m_ScalarType = scalar_type;
}
//----------------------------------------------------------------------------
void mafOpImporterRAWVolume::ScalarSignedOn()
//----------------------------------------------------------------------------
{
  m_Signed = 1;
}
//----------------------------------------------------------------------------
void mafOpImporterRAWVolume::ScalarSignedOff()
//----------------------------------------------------------------------------
{
  m_Signed = 0;
}
//----------------------------------------------------------------------------
void mafOpImporterRAWVolume::SetDataDimensions(int dims[3])
//----------------------------------------------------------------------------
{
  memcpy(m_DataDimemsion,dims,sizeof(m_DataDimemsion));
  m_SliceVOI[1] = m_DataDimemsion[2];
}
//----------------------------------------------------------------------------
void mafOpImporterRAWVolume::SetDataSpacing(double spc[3])
//----------------------------------------------------------------------------
{
  memcpy(m_DataSpacing,spc,sizeof(m_DataSpacing));
}
//----------------------------------------------------------------------------
void mafOpImporterRAWVolume::SetScalarDataToBigEndian()
//----------------------------------------------------------------------------
{
  m_Endian = 0;
}
//----------------------------------------------------------------------------
void mafOpImporterRAWVolume::SetScalarDataToLittleEndian()
//----------------------------------------------------------------------------
{
  m_Endian = 1;
}
//----------------------------------------------------------------------------
void mafOpImporterRAWVolume::SetDataVOI(int zVOI[2])
//----------------------------------------------------------------------------
{
  memcpy(m_SliceVOI,zVOI,sizeof(m_SliceVOI));
}
