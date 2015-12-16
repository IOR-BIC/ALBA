/*=========================================================================

 Program: MAF2
 Module: mafOpImporterRAWVolume_BES
 Authors: Paolo Quadrani     Silvano Imboden     Josef Kohout
 
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

#include "mafOpImporterRAWVolume_BES.h"

#include "mafGUI.h"
#include "mafRWI.h"
#include "mafGUIDialogPreview.h"
#include "mafTagItem.h"
#include "mafTagArray.h"

#include "mafVMEVolumeGray.h"
#include "mafVMEVolumeRGB.h"
#include "mafEventIO.h"
#include "mafStorage.h"

#ifdef VME_VOLUME_LARGE
#include "mafVolumeLargeWriter.h"
#include "vtkMAFLargeImageReader.h"
#include "vtkMAFLargeImageData.h"
#include "vtkMAFLargeDataSetCallback.h"
#endif // VME_VOLUME_LARGE

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

#include "wx/busyinfo.h"
//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpImporterRAWVolume_BES);
//----------------------------------------------------------------------------

#include "mafMemDbg.h"
#include "mafProgressBarHelper.h"
//#define New() NewDbg(__FILE__, __LINE__)

//----------------------------------------------------------------------------
mafOpImporterRAWVolume_BES::mafOpImporterRAWVolume_BES(const wxString &label) : mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType			= OPTYPE_IMPORTER;
	m_Canundo			= true;
	m_RawFile			= mafGetLastUserFolder().c_str();
#ifdef VME_VOLUME_LARGE
	m_OutputFileName = m_RawFile;
#endif // VME_VOLUME_LARGE

	m_VolumeGray	= NULL;
	m_VolumeRGB   = NULL;
#ifdef VME_VOLUME_LARGE
	m_VolumeLarge = NULL;
#endif // VME_VOLUME_LARGE
	m_GuiSlider   = NULL;

	m_Endian		 = 1;
	m_ScalarType = 1;
	m_Signed	   = 0;
	m_FileHeader = 0;  

	m_DataSpacing[0] = m_DataSpacing[1] = m_DataSpacing[2] = 1.0;

	m_DataDimemsion[0] = 512;
	m_DataDimemsion[1] = 512;
	m_DataDimemsion[2] = 1;

	m_VOI[0] = m_VOI[2] = m_VOI[4] = 0;
	m_VOI[1] = m_DataDimemsion[0] - 1;
  m_VOI[3] = m_DataDimemsion[1] - 1;
  m_VOI[5] = m_DataDimemsion[2] - 1;  

	m_NumberOfByte = 0;

	m_BuildRectilinearGrid = false;
	m_CoordFile = "";

	m_Dialog = NULL;
	m_Reader = NULL;
#ifdef VME_VOLUME_LARGE
	m_ReaderLarge = NULL;
#endif // VME_VOLUME_LARGE
	m_Actor	 = NULL;
	m_Texture = NULL;

	m_CurrentSlice = 0;
	m_SliceSlider	 = NULL;

  m_UseLookupTable = 1; //use lookup table (on by the default)
  m_NumberOfScalarComponents = 1; //grey scale data
#ifdef VME_VOLUME_LARGE
	m_MemLimit = 16;	//memory limit in MB, default is 16 MB
#endif // VME_VOLUME_LARGE
}
//----------------------------------------------------------------------------
mafOpImporterRAWVolume_BES::~mafOpImporterRAWVolume_BES()
//----------------------------------------------------------------------------
{
	mafDEL(m_VolumeGray);
	mafDEL(m_VolumeRGB);
#ifdef VME_VOLUME_LARGE
	mafDEL(m_VolumeLarge);
#endif // VME_VOLUME_LARGE
}
//----------------------------------------------------------------------------
mafOp *mafOpImporterRAWVolume_BES::Copy()
//----------------------------------------------------------------------------
{
	return new mafOpImporterRAWVolume_BES(m_Label);
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
	ID_VOI_X,
  ID_VOI_Y,
  ID_VOI_Z,
	ID_SPC,
	ID_HEADER,
	ID_GUESS,
	ID_SLICE,
	ID_COORD,
	ID_MEMLIMIT,
  ID_COMPONENTS, 
  ID_LOOKUPTABLE,
};
//----------------------------------------------------------------------------
void mafOpImporterRAWVolume_BES::OpRun()
//----------------------------------------------------------------------------
{
	m_Gui = new mafGUI(this);

	//dialog +++++++++++++++++++++++++++++++++++++++
	m_Dialog = new mafGUIDialogPreview(_("raw importer"), mafCLOSEWINDOW | mafRESIZABLE | mafUSEGUI | mafUSERWI);

	//Preview Pipeline ++++++++++++++++++++++++++++++
	vtkNEW(m_Reader);
#ifdef VME_VOLUME_LARGE
	vtkNEW(m_ReaderLarge);
#endif // VME_VOLUME_LARGE
	vtkNEW(m_LookupTable);
	vtkNEW(m_Texture);
  vtkNEW(m_Plane);

//  texture input will be set according to update
//	texture->SetInput((vtkImageData*)m_Reader->GetOutput()->GetSnapshot());
	m_Texture->InterpolateOn();
	m_Texture->MapColorScalarsThroughLookupTableOn();
	m_Texture->SetLookupTable((vtkLookupTable *)m_LookupTable);	

	vtkMAFSmartPointer<vtkPolyDataMapper> mapper;
	mapper ->SetInput(m_Plane->GetOutput());

	vtkNEW(m_Actor);
	m_Actor->SetMapper(mapper);
	m_Actor->SetTexture(m_Texture);
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

  m_Gui->Integer(ID_COMPONENTS,"comps", &m_NumberOfScalarComponents, 1, 4,
    _("define the number of components in the data set, e.g., typical grey-scale data sets have"
      "one component only, RGB data sets have 3 components"));

	m_Gui->Divider(0);
	m_Gui->Label(_("dimensions (x,y,z)"));
	m_Gui->Vector(ID_DIM, "",m_DataDimemsion,1,100000);   
	m_Gui->VectorN(ID_VOI_X, "VOI x" ,&m_VOI[0],2,0,MAXINT,_("define the range of voxels in x dimension to import."));
  m_Gui->VectorN(ID_VOI_Y, "VOI y" ,&m_VOI[2],2,0,MAXINT,_("define the range of voxels in y dimension to import."));
  m_Gui->VectorN(ID_VOI_Z, "VOI z" ,&m_VOI[4],2,0,MAXINT,_("define the range of voxels in z dimension to import."));

	m_Gui->Divider(0);
	m_Gui->Label(_("spacing in mm/pixel (x,y,z)"));
	m_Gui->Vector(ID_SPC, "",m_DataSpacing,0.0000001, 100000,-1);

	m_Gui->Button(ID_COORD,_("load"),_("z coords"),_("load the file for non regulary spaced raw volume"));

	m_Gui->Divider(0);
	m_Gui->Button(ID_GUESS,_("guess"),_("header size"));
	m_Gui->Integer(ID_HEADER,_(" "),&m_FileHeader,0);

	m_Gui->Divider(0);

#ifdef VME_VOLUME_LARGE
#ifdef __WIN32__
	MEMORYSTATUSEX ms; 
	ms.dwLength = sizeof(ms);
	GlobalMemoryStatusEx(&ms);
	
	int nMaxMem = ms.ullAvailPhys / (1024*1024);	//available memory
#else
	int nMaxMem = 512;	//some constant
#endif

	nMaxMem /= 2;		//keep 50% free

	m_Gui->Label(_("memory limit [MB]: "));
	m_Gui->Slider(ID_MEMLIMIT, "", &m_MemLimit, 1, nMaxMem, 
		_("if the data to be loaded is larger than the specified memory limit,"
		  "it will be loaded as VolumeLarge VME"));
#endif // VME_VOLUME_LARGE
  
	m_Gui->Label("");
	m_Gui->OkCancel();

	//slice slider +++++++++++++++++++++++++++++++++++++++++++
	m_GuiSlider = new mafGUI(this);  
	m_SliceSlider = m_GuiSlider->Slider(ID_SLICE,_("slice num"),&m_CurrentSlice,0);
  m_GuiSlider->Bool(ID_LOOKUPTABLE, _("use lookup table"), &m_UseLookupTable, 1, 
    _("determines whether the default lookup table should be used for the preview"));
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
	{
		if (m_TestMode)
		{
			//messages are suppressed in Test Mode
			if (!Import()) 
				res = OP_RUN_CANCEL;
		}
		else
		{
			wxBusyInfo wait(_("Importing RAW data, please wait..."));

			if (!Import()) 
				res = OP_RUN_CANCEL;			
		}
	}

	//cleanup +++++++++++++++++++++++++++++++++  
	m_Dialog->GetRWI()->m_RenFront->RemoveActor(m_Actor);

	vtkDEL(m_Plane);
  vtkDEL(m_Reader);
#ifdef VME_VOLUME_LARGE
	vtkDEL(m_ReaderLarge);
#endif // VME_VOLUME_LARGE
	vtkDEL(m_LookupTable);
	vtkDEL(m_Actor);
	vtkDEL(m_Texture);
	cppDEL(m_GuiSlider);
	cppDEL(m_Dialog);

	//finish the Run +++++++++++++++++++++++++++++++++
	mafEventMacro(mafEvent(this,res));

#ifdef VME_VOLUME_LARGE
if (res == OP_RUN_OK && (IsVolumeLarge()/* || IsFileLarge()*/))
	{
		//save the VME data, it should not prompt for saving
		mafEventMacro( mafEvent(this, MENU_FILE_SAVE));
	}
#endif // VME_VOLUME_LARGE
}
//----------------------------------------------------------------------------
void mafOpImporterRAWVolume_BES::EnableWidgets(bool enable)
//----------------------------------------------------------------------------
{
	assert(m_Gui && m_SliceSlider);
  if (m_Gui != NULL)
  {
	  m_Gui->Enable(ID_BITS,		    enable);
	  m_Gui->Enable(ID_SCALAR_TYPE,	enable);
	  m_Gui->Enable(ID_SIGNED,	    enable);
	  m_Gui->Enable(ID_DIM,		      enable); 
	  m_Gui->Enable(ID_VOI_X,  enable); 
    m_Gui->Enable(ID_VOI_Y,  enable); 
    m_Gui->Enable(ID_VOI_Z,  enable); 
    m_Gui->Enable(ID_COMPONENTS, enable);
	  m_Gui->Enable(ID_SPC,		      enable); 
	  m_Gui->Enable(ID_GUESS,		    enable);
	  m_Gui->Enable(ID_HEADER,	    enable);
	  m_Gui->Enable(wxOK,		        enable);
  }

  if (m_GuiSlider != NULL)
	  m_GuiSlider->Enable(ID_SLICE,enable);  
}
//----------------------------------------------------------------------------
void mafOpImporterRAWVolume_BES::	OnEvent(mafEventBase *maf_event) 
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch(e->GetId())
		{	  
		case ID_FILE:
			EnableWidgets(true);
			m_CurrentSlice = m_DataDimemsion[2]/2;
			UpdateReader();

			m_Actor->VisibilityOn();
			m_Dialog->GetRWI()->CameraReset();
			m_SliceSlider->SetRange(0,m_DataDimemsion[2] - 1);	 
			m_Gui->Update();
			break;

    case ID_DIM:      
      m_VOI[1] = m_DataDimemsion[0] - 1;
      m_VOI[3] = m_DataDimemsion[1] - 1;
      m_VOI[5] = m_DataDimemsion[2] - 1;
      m_VOI[0] = m_VOI[2] = m_VOI[4] = 0;
      //fall through to Slicer
    case ID_VOI_Z:
      if (m_CurrentSlice > m_VOI[5]) 
        m_CurrentSlice = m_VOI[5];
      else if (m_CurrentSlice < m_VOI[4]) 
        m_CurrentSlice = m_VOI[4];

      m_SliceSlider->SetRange(m_VOI[4], m_VOI[5]);
      //fall through to Resolution settings
    case ID_VOI_X:
    case ID_VOI_Y:      
    case ID_SPC:
      {
        double x_sp = (m_VOI[1] - m_VOI[0])*m_DataSpacing[0];
        double y_sp = (m_VOI[3] - m_VOI[2])*m_DataSpacing[1];
        if (x_sp > y_sp)
        {
          y_sp /= x_sp;
          x_sp = 1.0;
        }
        else
        {
          x_sp /= y_sp;
          y_sp = 1.0;
        }
        
        double origin[3];
        m_Plane->GetOrigin(origin);
        m_Plane->SetPoint1(x_sp + origin[0], origin[1], origin[2]);
        m_Plane->SetPoint2(origin[0], y_sp + origin[1], origin[2]);        

        m_Plane->Update();
      }
      //fall through to update
		case ID_BITS:
		case ID_SCALAR_TYPE:
		case ID_SIGNED:
    case ID_COMPONENTS:    
      m_Gui->Update();
      UpdateReader();
      m_Dialog->GetRWI()->CameraUpdate();
      break;

    case ID_LOOKUPTABLE:
      m_Texture->SetMapColorScalarsThroughLookupTable(m_UseLookupTable);
      m_Dialog->GetRWI()->CameraUpdate();
      break;
      			
		case ID_SLICE:
			m_GuiSlider->Update();
			UpdateReader();
			m_Dialog->GetRWI()->CameraUpdate();
			break;

		case ID_GUESS:
			{
#ifdef VME_VOLUME_LARGE
        long long len = (long long)mafVMEVolumeLargeUtils::GetFileLength(m_RawFile);
#else
        long long len = (long long)GetFileLength(m_RawFile);
#endif
				m_FileHeader = (int)(len - ((long long)m_DataDimemsion[0]*
					m_DataDimemsion[1]*m_DataDimemsion[2]*m_NumberOfByte*
          m_NumberOfScalarComponents));

				m_Gui->Update();
				UpdateReader();
			}
			break;

		case ID_COORD:
			{
				wxString dir = mafGetLastUserFolder().c_str();
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
#ifdef VME_VOLUME_LARGE
			if (VolumeLargeCheck())				
#endif // VME_VOLUME_LARGE
				m_Dialog->EndModal(wxID_OK);	//otherwise user should modify it
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

#ifdef VME_VOLUME_LARGE
//if the volume (or VOI) is large, it displays a warning that the volume 
//to be imported is large and returns true, if the operation should continue,
//false otherwise (user canceled the import)
bool mafOpImporterRAWVolume_BES::VolumeLargeCheck()
{
  mafString szStr;
  int nResult = mafVMEVolumeLargeUtils::VolumeLargeCheck(this, m_Listener,
    m_VOI, GetVTKDataType(), m_NumberOfScalarComponents, m_MemLimit, szStr);

  if (nResult == 0)
    return false;
  else if (nResult == 2)
  {
    wxString name, ext, path;
    wxSplitPath(m_RawFile.GetCStr(), &path, &name, &ext);

    m_OutputFileName = szStr;
    m_OutputFileName += wxFILE_SEP_PATH + wxString::Format("%s_%X", name, (int)time(NULL));
  }
	return true;
}
#endif // VME_VOLUME_LARGE


template< typename TR >
void mafOpImporterRAWVolume_BES::UpdateReaderT(TR* reader)
{
	reader->SetFileName(m_RawFile);
  reader->SetDataScalarType(GetVTKDataType());
  if(m_Endian == 0)
    reader->SetDataByteOrderToBigEndian();
  else
    reader->SetDataByteOrderToLittleEndian();

	switch(m_ScalarType)
	{
	case CHAR_SCALAR:		
		m_NumberOfByte = 1;
		m_Gui->Enable(ID_SIGNED,true);
		break;
	case SHORT_SCALAR:
		m_NumberOfByte = 2;
		m_Gui->Enable(ID_SIGNED,true);
		break;
	case INT_SCALAR:		
		m_NumberOfByte = 4;
		m_Gui->Enable(ID_SIGNED,true);
		break;
	case FLOAT_SCALAR:		
		m_NumberOfByte = 4;     //BES: float has 4 bytes
		m_Gui->Enable(ID_SIGNED,false);
		break;
	case DOUBLE_SCALAR:
		m_NumberOfByte = 8;
		m_Gui->Enable(ID_SIGNED,false);
		break;
	}

  reader->SetNumberOfScalarComponents(m_NumberOfScalarComponents);
	reader->SetDataExtent(0, m_DataDimemsion[0] - 1, 0, m_DataDimemsion[1] - 1, 0, m_DataDimemsion[2] - 1);  
	reader->SetDataSpacing(m_DataSpacing);
	reader->SetHeaderSize(m_FileHeader);
	reader->SetFileDimensionality(3);
	reader->SetDataVOI(m_VOI[0], m_VOI[1], m_VOI[2], m_VOI[3], m_CurrentSlice, m_CurrentSlice);
	reader->Update();
}

//----------------------------------------------------------------------------
void mafOpImporterRAWVolume_BES::UpdateReader()
//----------------------------------------------------------------------------
{
	double range[2];	

#ifdef VME_VOLUME_LARGE
	bool bIsLargeVolume = IsVolumeLarge() || IsFileLarge();
	if (bIsLargeVolume)
	{
		m_ReaderLarge->SetMemoryLimit(m_MemLimit * 1024);    

		UpdateReaderT< vtkMAFLargeImageReader >(m_ReaderLarge);		
		m_ReaderLarge->GetOutput()->GetSnapshot()->GetScalarRange(range);
		m_Texture->SetInput((vtkImageData*)m_ReaderLarge->GetOutput()->GetSnapshot());
	}
	else
#endif // VME_VOLUME_LARGE
	{    
		UpdateReaderT< vtkImageReader >(m_Reader);
		m_Reader->GetOutput()-> GetScalarRange(range);
		m_Texture->SetInput((vtkImageData*)m_Reader->GetOutput());
	}		

	m_LookupTable->SetTableRange(range);
	m_LookupTable->SetWindow(range[1] - range[0]);
	m_LookupTable->SetLevel((range[1] + range[0]) / 2.0);
	m_LookupTable->Build();
}

//returns the imported object, either vtkImageData or vtkMAFLargeImageData
//returns NULL in case of error
template< typename TR >
vtkDataObject* mafOpImporterRAWVolume_BES::ImportT(TR* reader)
{
	reader->SetFileName(m_RawFile);  
  reader->SetDataScalarType(GetVTKDataType());
  if(m_Endian == 0)
    reader->SetDataByteOrderToBigEndian();
  else
    reader->SetDataByteOrderToLittleEndian();
	
  reader->SetNumberOfScalarComponents(m_NumberOfScalarComponents);
	reader->SetDataExtent(0, m_DataDimemsion[0] - 1, 0, m_DataDimemsion[1] - 1, 0, m_DataDimemsion[2] - 1);
	reader->SetDataSpacing(m_DataSpacing);
	reader->SetHeaderSize(m_FileHeader);
	reader->SetFileDimensionality(3);
	reader->SetDataVOI(m_VOI);
	reader->Update();

	vtkDataObject* ret = reader->GetOutput();
	ret->Register(NULL);		//This is here to prevent the deletion
	reader->SetOutput(NULL);	//Disconnect the reader from the output
	ret->SetSource(NULL);		//Disconnect the output from the reader
	return ret;					//Reader should be destroyed here, image should be preserved
}

//----------------------------------------------------------------------------
bool mafOpImporterRAWVolume_BES::Import()
//----------------------------------------------------------------------------
{
	mafProgressBarHelper progressHelper(m_Listener);
	progressHelper.SetTextMode(m_TestMode);
	progressHelper.InitProgressBar();
	progressHelper.SetBarText("Importing RAW data ...");
		
  vtkMAFSmartPointer<vtkDoubleArray> ZDoubleArray;	//Ref(ZDoubleArray) = 1
  if (m_BuildRectilinearGrid)
  {
    const char* nome = (m_CoordFile);
    std::ifstream f_in;
    f_in.open(nome);

    char title[256];
    f_in.getline(title,256);

    //z array is read from a file	    
    for (int i = 0; i < m_DataDimemsion[2]; i++)
    {
      double currentValue;
      f_in>> currentValue;
      ZDoubleArray->InsertNextValue(currentValue);
    }

    f_in.close();
  }

#ifdef VME_VOLUME_LARGE
	//bool bIsLargeVolume = IsVolumeLarge() || IsFileLarge();
	if (IsVolumeLarge())
	{
		vtkMAFSmartPointer< vtkMAFLargeImageReader > reader;
		reader->SetMemoryLimit(1);	//some sampling (but fast)

		vtkMAFLargeImageData* img;
		if(m_TestMode)
			img = (vtkMAFLargeImageData*)ImportT< vtkMAFLargeImageReader >(reader);
		else
		{		
			vtkMAFLargeDataSetCallback* ppc = vtkMAFLargeDataSetCallback::New();
			ppc->SetListener(this->m_Listener);

			img = (vtkMAFLargeImageData*)reader->GetOutput();
			img->AddObserver(vtkCommand::ProgressEvent, ppc);
			ImportT< vtkMAFLargeImageReader >(reader);
			img->RemoveObserver(ppc);

			ppc->Delete();
		}
	
		mafNEW(m_VolumeLarge);
		m_VolumeLarge->SetFileName(this->m_RawFile);
		mafVolumeLargeReader* rd = NULL;
		mafVolumeLargeWriter wr;
		wr.SetInputDataSet(img);
    if (m_BuildRectilinearGrid)
      wr.SetInputZCoordinates(ZDoubleArray.GetPointer());
		wr.SetOutputFileName(m_OutputFileName);
		wr.SetListener(this->m_Listener);
		if (wr.Update())
		{
			int VOI[6];
			VOI[0] = VOI[2] = VOI[4] = 0;
			VOI[1] = m_VOI[1] - m_VOI[0];
			VOI[3] = m_VOI[3] - m_VOI[2];
			VOI[5] = m_VOI[5] - m_VOI[4];

			rd = new mafVolumeLargeReader();
			rd->SetFileName(m_OutputFileName);
			rd->SetMemoryLimit(m_MemLimit * 1024);
			rd->SetVOI(VOI);
			rd->Update();

			if(!this->m_TestMode)
			{
        vtkIdType64 nTotalSize = rd->GetLevelFilesSize();
				wxString szTotalSize = wxString::Format("%d", 
					(int)(nTotalSize / (1024*1024)));
				int nLen = (int)szTotalSize.Len();
				while (nLen > 3)
				{
					nLen -= 3;
					szTotalSize.insert(nLen, ' ');
				}

				wxMessageBox(wxString::Format(
					_("An optimised volume file with the total size\n"
					  "%s MB was successfuly constructed."					
					), szTotalSize), _("Information"), wxOK | wxICON_INFORMATION);
			}
		}

		if (rd != NULL && m_VolumeLarge->SetLargeData(rd) == MAF_OK) {
			img->Delete();

			m_Output = m_VolumeLarge;			
		}
		else
		{
			wxMessageBox(_("Some importing error occurred!!"), _("Warning!"));
			img->Delete();
			delete rd;
      return false;
		}
	}
	else
#endif // VME_VOLUME_LARGE
	{
    vtkImageData* img = NULL;

#ifdef VME_VOLUME_LARGE
    if (IsFileLarge())
    {
      //File is large => read it by our technique but it will be small volume
      vtkMAFSmartPointer< vtkMAFLargeImageReader > reader;
      reader->SetMemoryLimit(m_MemLimit * 1024);
      
      vtkMAFLargeImageData* imgLarge;
      if(m_TestMode)
        imgLarge = (vtkMAFLargeImageData*)ImportT< vtkMAFLargeImageReader >(reader);
      else
      {		
        vtkMAFLargeDataSetCallback* ppc = vtkMAFLargeDataSetCallback::New();
        ppc->SetListener(this->m_Listener);

        vtkMAFLargeImageData* img2 = (vtkMAFLargeImageData*)reader->GetOutput();
        img2->AddObserver(vtkCommand::ProgressEvent, ppc);
        imgLarge = (vtkMAFLargeImageData*)ImportT< vtkMAFLargeImageReader >(reader);        
        img2->RemoveObserver(ppc);

        ppc->Delete();
      }

      img = (vtkImageData*)imgLarge->GetSnapshot();
      img->Register(NULL);
      imgLarge->Delete();
    }
    else
    {
#endif
		  vtkMAFSmartPointer< vtkImageReader > reader;
		
		  if (!m_TestMode) {
			  mafEventMacro(mafEvent(this,BIND_TO_PROGRESSBAR, reader));
		  }

		  img = (vtkImageData*)ImportT< vtkImageReader >(reader);
#ifdef VME_VOLUME_LARGE
    }
#endif

		vtkMAFSmartPointer<vtkImageToStructuredPoints> image_to_sp;
		image_to_sp->SetInput(img);
		img->Delete();	//we no longer need img, release it

		image_to_sp->Update();

		if (m_BuildRectilinearGrid)
		{
			// conversion from vtkStructuredPoints to vtkRectilinearGrid
			vtkMAFSmartPointer<vtkStructuredPoints> structured_data = image_to_sp->GetOutput();	//image_to_sp->Output +1
			vtkMAFSmartPointer<vtkPointData> data = structured_data->GetPointData();
			vtkMAFSmartPointer<vtkDataArray> scalars = data->GetScalars();

			vtkMAFSmartPointer<vtkDoubleArray> XDoubleArray;	//Ref(XDoubleArray) = 1
			vtkMAFSmartPointer<vtkDoubleArray> YDoubleArray;	//Ref(YDoubleArray) = 1			

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
			
			vtkMAFSmartPointer<vtkRectilinearGrid> rectilinear_data;	//Ref(rectilinear_data) = 1

			rectilinear_data->SetXCoordinates(XDoubleArray);	//Ref(XDoubleArray) = 2
			rectilinear_data->SetYCoordinates(YDoubleArray);	//Ref(YDoubleArray) = 2
			rectilinear_data->SetZCoordinates(ZDoubleArray);	//Ref(ZDoubleArray) = 2

			int dim[3];

			structured_data->GetDimensions(dim);
			rectilinear_data->SetDimensions(dim);
			rectilinear_data->GetPointData()->SetScalars(scalars);

			mafNEW(m_VolumeGray);
			mafNEW(m_VolumeRGB);
			if (m_VolumeGray->SetDataByDetaching((vtkDataSet *)rectilinear_data,0) == MAF_OK) {
				m_Output = m_VolumeGray;
			}
			else if (m_VolumeRGB->SetDataByDetaching((vtkDataSet *)rectilinear_data,0) == MAF_OK) {
				m_Output = m_VolumeRGB;
			}
			else
			{
				wxMessageBox(_("Some importing error occurred!!"), _("Warning!"));
				return false;
			}
		} 
		else //if (m_BuildRectilinearGrid)
		{
			mafNEW(m_VolumeGray);
			mafNEW(m_VolumeRGB);

			vtkDataSet* volume = (vtkDataSet *)image_to_sp->GetOutput();
			if (m_VolumeGray->SetDataByDetaching(volume, 0) == MAF_OK){
				m_Output = m_VolumeGray;
			}
			else if (m_VolumeRGB->SetDataByDetaching(volume, 0) == MAF_OK){
				m_Output = m_VolumeRGB;
			}
			else
			{
				wxMessageBox(_("Some importing error occurred!!"), _("Warning!"));
				return false;
			}
		}
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

#ifndef VME_VOLUME_LARGE
//----------------------------------------------------------------------------
int mafOpImporterRAWVolume_BES::GetFileLength(const char * filename) 
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
#endif // VME_VOLUME_LARGE

//----------------------------------------------------------------------------
void mafOpImporterRAWVolume_BES::SetFileName(const char *raw_file)
//----------------------------------------------------------------------------
{
	m_RawFile = raw_file;
}

#ifdef VME_VOLUME_LARGE
//Sets the output file (with bricks)
void mafOpImporterRAWVolume_BES::SetOutputFile(const char* szOutputFile)
{
	m_OutputFileName = szOutputFile;
}
#endif // VME_VOLUME_LARGE

//----------------------------------------------------------------------------
void mafOpImporterRAWVolume_BES::SetScalarType(int scalar_type)
//----------------------------------------------------------------------------
{
	if (scalar_type < CHAR_SCALAR || scalar_type > DOUBLE_SCALAR)
	{
		return;
	}
	m_ScalarType = scalar_type;
}
//----------------------------------------------------------------------------
void mafOpImporterRAWVolume_BES::ScalarSignedOn()
//----------------------------------------------------------------------------
{
	m_Signed = 1;
}
//----------------------------------------------------------------------------
void mafOpImporterRAWVolume_BES::ScalarSignedOff()
//----------------------------------------------------------------------------
{
	m_Signed = 0;
}
//----------------------------------------------------------------------------
void mafOpImporterRAWVolume_BES::SetDataDimensions(int dims[3])
//----------------------------------------------------------------------------
{
	memcpy(m_DataDimemsion,dims,sizeof(m_DataDimemsion));
  m_VOI[0] = m_VOI[2] = m_VOI[4] = 0;
  m_VOI[1] = m_DataDimemsion[0] - 1;
  m_VOI[3] = m_DataDimemsion[1] - 1;
  m_VOI[5] = m_DataDimemsion[2] - 1; 
}
//----------------------------------------------------------------------------
void mafOpImporterRAWVolume_BES::SetDataSpacing(double spc[3])
//----------------------------------------------------------------------------
{
	memcpy(m_DataSpacing,spc,sizeof(m_DataSpacing));
}
//----------------------------------------------------------------------------
void mafOpImporterRAWVolume_BES::SetScalarDataToBigEndian()
//----------------------------------------------------------------------------
{
	m_Endian = 0;
}
//----------------------------------------------------------------------------
void mafOpImporterRAWVolume_BES::SetScalarDataToLittleEndian()
//----------------------------------------------------------------------------
{
	m_Endian = 1;
}
//----------------------------------------------------------------------------
void mafOpImporterRAWVolume_BES::SetDataVOI(int zVOI[2])
//----------------------------------------------------------------------------
{
	memcpy(&m_VOI[4],zVOI,sizeof(zVOI));
}

//----------------------------------------------------------------------------
//Allow to define a sub extent on all axis. (Default is imported all the volume)
void mafOpImporterRAWVolume_BES::SetDataVOI_XYZ(int VOI[6])
//----------------------------------------------------------------------------
{
  memcpy(m_VOI, VOI,sizeof(m_VOI));
}

//----------------------------------------------------------------------------
//Sets the number of scalar components
//By the default, it is one (grey scale data)
void mafOpImporterRAWVolume_BES::SetNumberOfScalarComponents(int nComps)
//----------------------------------------------------------------------------
{
  m_NumberOfScalarComponents = nComps;
}