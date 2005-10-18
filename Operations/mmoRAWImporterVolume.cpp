/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoRAWImporterVolume.cpp,v $
  Language:  C++
  Date:      $Date: 2005-10-18 11:56:44 $
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

#include "mmoRAWImporterVolume.h"
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

//----------------------------------------------------------------------------
mmoRAWImporterVolume::mmoRAWImporterVolume(wxString label) : mafOp(label)
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

	m_NumberOfByte = 0;
	
	m_Dialog = NULL;
	m_Reader = NULL;
	m_Actor	 = NULL;

	m_CurrentSlice = 0;
	m_SliceSlider	 = NULL;
}
//----------------------------------------------------------------------------
mmoRAWImporterVolume::~mmoRAWImporterVolume()
//----------------------------------------------------------------------------
{
	mafDEL(m_VolumeGray);
  mafDEL(m_VolumeRGB);
}
//----------------------------------------------------------------------------
mafOp *mmoRAWImporterVolume::Copy()
//----------------------------------------------------------------------------
{
	return new mmoRAWImporterVolume(m_Label);
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
	ID_SPC,
	ID_HEADER,
	ID_GUESS,
	ID_SLICE,
};
//----------------------------------------------------------------------------
void mmoRAWImporterVolume::OpRun()   
//----------------------------------------------------------------------------
{
	m_Gui = new mmgGui(this);

	//dialog +++++++++++++++++++++++++++++++++++++++
	m_Dialog = new mmgDialogPreview("raw importer", mafCLOSEWINDOW | mafRESIZABLE | mafUSEGUI | mafUSERWI);

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
	
	m_Dialog->GetRWI()->m_RwiBase->SetMouse(m_Mouse);
  m_Dialog->GetRWI()->m_RenFront->AddActor(m_Actor);
  
	//GUI +++++++++++++++++++++++++++++++++++++++++++
	m_Gui->Show(true);
	m_Gui->Divider(0);
  mafString wildc = "Raw data (*.raw)|*.raw|All files (*.*)|*.*";
	m_Gui->FileOpen(ID_FILE,"file",&m_RawFile,wildc);

  wxString endian_choices[2] = {"Big Endian","Little Endian"};
  wxString scalar_choices[5] = {"char","short","int","float","double"};
	m_Gui->Divider(0);
	m_Gui->Combo(ID_BITS,"endian",&m_Endian,2,endian_choices);
	m_Gui->Combo(ID_SCALAR_TYPE,"scalar type",&m_ScalarType,5,scalar_choices);
	m_Gui->Bool(ID_SIGNED,"signed",&m_Signed);

	m_Gui->Divider(0);
	m_Gui->Label("dimensions (x,y,z)");
	m_Gui->Vector(ID_DIM, "",m_DataDimemsion,1,10000); 
	
	m_Gui->Divider(0);
	m_Gui->Label("spacing in mm/pixel (x,y,z)");
	m_Gui->Vector(ID_SPC, "",m_DataSpacing,0.0000001, 100000); 

	m_Gui->Divider(0);
	m_Gui->Button(ID_GUESS,"guess","header size");
	m_Gui->Integer(ID_HEADER," ",&m_FileHeader,0);

	m_Gui->Label("");
	m_Gui->OkCancel();

	//slice slider +++++++++++++++++++++++++++++++++++++++++++
  m_GuiSlider = new mmgGui(this);
  m_SliceSlider = m_GuiSlider->Slider(ID_SLICE,"slice num",&m_CurrentSlice,0);
   
	EnableWidgets(false);

	//sizing & positioning +++++++++++++++++++++++++++++++++++++++++++
//	m_Gui->FitGui();
//	m_Gui->SetSize(220, 390);
	m_Gui->Update();
   
  m_Dialog->GetGui()->AddGui(m_Gui);
  m_Dialog->Add(m_GuiSlider, 0, wxEXPAND);

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
void mmoRAWImporterVolume::EnableWidgets(bool enable)
//----------------------------------------------------------------------------
{
  assert(m_Gui && m_SliceSlider);
	
	m_Gui->Enable(ID_BITS,		    enable);
	m_Gui->Enable(ID_SCALAR_TYPE,	enable);
	m_Gui->Enable(ID_SIGNED,	    enable);
	m_Gui->Enable(ID_DIM,		      enable); 
	m_Gui->Enable(ID_SPC,		      enable); 
	m_Gui->Enable(ID_GUESS,		    enable);
	m_Gui->Enable(ID_HEADER,	    enable);
	m_Gui->Enable(wxOK,		        enable);

	m_GuiSlider->Enable(ID_SLICE,enable);
}
//----------------------------------------------------------------------------
void mmoRAWImporterVolume::	OnEvent(mafEventBase *maf_event) 
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
      case ID_DIM:
      case ID_SPC:
      case ID_SLICE:
        if( m_CurrentSlice >= m_DataDimemsion[2] ) 
          m_CurrentSlice = m_DataDimemsion[2]-1;
        m_SliceSlider->SetRange(0,m_DataDimemsion[2] - 1);
        m_Gui->Update();
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
void mmoRAWImporterVolume::UpdateReader()
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
			m_Gui->Enable(ID_SIGNED,true);
		break;
		case DOUBLE_SCALAR:
			m_Reader->SetDataScalarType(VTK_DOUBLE);
			if(m_Endian == 0)
				m_Reader->SetDataByteOrderToBigEndian();
			else
				m_Reader->SetDataByteOrderToLittleEndian();
			m_NumberOfByte = 8;
			m_Gui->Enable(ID_SIGNED,true);
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
bool mmoRAWImporterVolume::Import()
//----------------------------------------------------------------------------
{
	wxBusyInfo wait("Reading File, please wait...");

 	vtkMAFSmartPointer<vtkImageReader> reader;
	reader->SetFileName(m_RawFile);  

	switch(m_ScalarType)
	{
		case CHAR_SCALAR:
			reader->SetDataScalarType( (m_Signed) ? VTK_CHAR : VTK_UNSIGNED_CHAR);
		break;
		case SHORT_SCALAR:
			reader->SetDataScalarType( (m_Signed) ? VTK_SHORT : VTK_UNSIGNED_SHORT);
			if(m_Endian == 1)
				reader->SetDataByteOrderToBigEndian();
			else if(m_Endian == 2)
				reader->SetDataByteOrderToLittleEndian();
		break;
		case INT_SCALAR:
			reader->SetDataScalarType( (m_Signed) ? VTK_INT : VTK_UNSIGNED_INT);
			if(m_Endian == 1)
				reader->SetDataByteOrderToBigEndian();
			else if(m_Endian == 2)
				reader->SetDataByteOrderToLittleEndian();
		break;
		case FLOAT_SCALAR:
			reader->SetDataScalarType(VTK_FLOAT);
		break;
		case DOUBLE_SCALAR:
			reader->SetDataScalarType(VTK_DOUBLE);
		break;
	}

	reader->SetDataExtent(0, m_DataDimemsion[0] - 1, 0, m_DataDimemsion[1] - 1, 0, m_DataDimemsion[2] - 1);
	reader->SetDataVOI(0, m_DataDimemsion[0] - 1, 0, m_DataDimemsion[1] - 1, 0, m_DataDimemsion[2] - 1);
	reader->SetDataSpacing(m_DataSpacing);
	reader->SetHeaderSize(m_FileHeader);
	reader->SetFileDimensionality(3);
	reader->Update();

	mafNEW(m_VolumeGray);
  mafNEW(m_VolumeRGB);
  if (m_VolumeGray->SetData(reader->GetOutput(),0) == MAF_OK)
  {
    m_Output = m_VolumeGray;
  }
  else if (m_VolumeRGB->SetData(reader->GetOutput(),0) == MAF_OK)
  {
    m_Output = m_VolumeRGB;
  }
  else
  {
    wxMessageBox("Some importing error occurred!!", "Warning!");
    return false;
  }

	mafTagItem tag_Nature;
	tag_Nature.SetName("VME_NATURE");
	tag_Nature.SetValue("NATURAL");

	wxString name, ext, path;
  wxSplitPath(m_RawFile.GetCStr(),&path,&name,&ext);
  m_Output->SetName(name.c_str());
  m_Output->GetTagArray()->SetTag(tag_Nature);
	return true;
}
//----------------------------------------------------------------------------
int mmoRAWImporterVolume::GetFileLength(const char * filename) 
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
