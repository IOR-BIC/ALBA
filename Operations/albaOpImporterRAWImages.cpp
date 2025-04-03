/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterRAWImages
 Authors: Stefania Paperini porting Matteo Giacomoni
 
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

#include "wx/busyinfo.h"
#include "wx/dir.h"

#include "albaOpImporterRAWImages.h"
#include "albaEvent.h"
#include "albaEventIO.h"
#include "albaStorage.h"
#include "albaGUI.h"
#include "albaOp.h"
#include "albaGUIValidator.h"
#include "albaRWIBase.h"
#include "albaRWI.h"
#include "albaGUIDialogPreview.h"
#include "albaDeviceButtonsPadMouse.h"
#include "albaInteractorDICOMImporter.h"
#include "albaTagArray.h"
#include "albaVMEVolumeGray.h"
#include "albaVMEVolumeRGB.h"
#include "albaVME.h"

#include "vtkRenderer.h"
#include "vtkImageReader.h"
#include "vtkImageImport.h"
#include "vtkImageAppendComponents.h"
#include "vtkDirectory.h"
#include "vtkTexture.h" 
#include "vtkPolyDataMapper.h"
#include "vtkPlaneSource.h"
#include "vtkStructuredPoints.h"
#include "vtkImageToStructuredPoints.h"
#include "vtkWindowLevelLookupTable.h"
#include "vtkOutlineFilter.h"
#include "vtkProperty.h"
#include "vtkPoints.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkRectilinearGrid.h"
#include "vtkDoubleArray.h"
#include "wx/filename.h"


#include "albaMemDbg.h"
#include "albaDbg.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpImporterRAWImages);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpImporterRAWImages::albaOpImporterRAWImages(wxString label) : albaOp(label)
//----------------------------------------------------------------------------
{
  m_OpType					= OPTYPE_IMPORTER;
  m_Canundo					= true;
  m_RawDirectory		= albaGetLastUserFolder();
  m_VtkRawDirectory	= NULL;
  m_Output					= NULL;

  m_Bit				= 1;
  m_RgbType		= 0;
  m_Signed		= 0;
  m_Header		= 0;

  m_Rect			= false;
  m_CoordFile	= "";	

  m_Spacing[0] = 1.0;
  m_Spacing[1] = 1.0;
  m_Spacing[2] = 1.0;

  m_Offset		   = 0;
  m_FileSpacing = 1;

  m_Dimension[0] = 512;
  m_Dimension[1] = 512;
  m_Dimension[2] = 1;

  m_CropMode = 0;
  m_ROI_2D[0] = m_ROI_2D[1] = m_ROI_2D[2] = m_ROI_2D[3] = 0.0;

  m_NumberByte	  = 0;
  m_NumberFile	  = 0;
  m_NumberSlices  = 0;

  m_Dialog		 = NULL;
  m_Rwi		 = NULL;
  m_Reader = NULL;
  m_Actor	 = NULL;

  m_CurrentSlice = 0;

  m_Prefix    = "";
  m_Pattern   = "%s%04d";
  m_Extension = ".raw";

  m_SliceText   = NULL;
  m_SliceSlider = NULL;
  m_SliceLab    = NULL;
  m_LookupTable	 = NULL;
  m_Texture		   = NULL;
  m_Plane = NULL;
  m_DicomInteractor = NULL;

  m_GizmoStatus = GIZMO_NOT_EXIST;
  m_SideToBeDragged = 0;
  m_UseLookupTable = 1; //use lookup table (on by the default)


  m_VolumeGray = NULL;
  m_VolumeRGB = NULL;

}
//----------------------------------------------------------------------------
albaOpImporterRAWImages::~albaOpImporterRAWImages()
//----------------------------------------------------------------------------
{
  albaDEL(m_VolumeGray);
  albaDEL(m_VolumeRGB);
}
//----------------------------------------------------------------------------
albaOp *albaOpImporterRAWImages::Copy()
//----------------------------------------------------------------------------
{
  return new albaOpImporterRAWImages(m_Label);
}
//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------
enum 
{
  ID_BITS = MINID,
  ID_SIGNED,
  ID_DIMENSIONS,
  ID_ROI,
  ID_SPC_X,
  ID_SPC_Y,
  ID_SPC_Z,
  ID_HEADER,
  ID_OFFSET,
  ID_SPACING,
  ID_OPEN_DIR,
  ID_SLICE,
  ID_GUESS,
  ID_STRING_EXT,
  ID_STRING_PREFIX,
  ID_STRING_PATTERN,
  ID_COORD,
  ID_RGB_TYPE,
  ID_MEMLIMIT,
  ID_LOOKUPTABLE,
};
//----------------------------------------------------------------------------
void albaOpImporterRAWImages::CreatePipeline()
//----------------------------------------------------------------------------
{
  //Preview Pipeline ++++++++++++++++++++++++++++++
  vtkNEW(m_Reader);

  vtkNEW(m_RedImage);
  vtkNEW(m_GreenImage);
  vtkNEW(m_BlueImage);

  vtkNEW(m_AppendComponents);
  m_AppendComponents->AddInputConnection(m_BlueImage->GetOutputPort());
  m_AppendComponents->AddInputConnection(m_GreenImage->GetOutputPort());

  vtkNEW(m_InterleavedImage);
  m_InterleavedImage->AddInputConnection(m_AppendComponents->GetOutputPort());
  m_InterleavedImage->AddInputConnection(m_RedImage->GetOutputPort());

  vtkNEW(m_Texture);
  //  texture input will be set according to update
  //m_Texture->SetInput(m_Reader->GetOutput());
  m_Texture->InterpolateOn();

  vtkNEW(m_LookupTable);

  m_Texture->SetLookupTable((vtkLookupTable *)m_LookupTable);

  vtkNEW(m_Plane);

  vtkNEW(m_Mapper);
  m_Mapper->SetInputConnection(m_Plane->GetOutputPort());

  vtkNEW(m_Actor);
  m_Actor->SetMapper(m_Mapper);
  m_Actor->SetTexture(m_Texture);
  m_Actor->VisibilityOff();

  ////// ROI gizmo
  vtkNEW(m_GizmoPlane);

  vtkALBASmartPointer<vtkOutlineFilter> outlineFilter;
  outlineFilter->SetInputConnection(m_GizmoPlane->GetOutputPort());

  vtkALBASmartPointer<vtkPolyDataMapper> polyDataMapper;
  polyDataMapper->SetInputConnection(outlineFilter->GetOutputPort());

  vtkNEW(m_GizmoActor);
  m_GizmoActor->GetProperty()->SetColor(0.8,0,0);
  m_GizmoActor->GetProperty()->SetLineWidth(2.0);
  m_GizmoActor->VisibilityOff();
  m_GizmoActor->SetMapper(polyDataMapper);
  ////// 

  if(!this->m_TestMode)
  {
    albaNEW(m_DicomInteractor);
    m_DicomInteractor->SetListener(this);
    m_Mouse->AddObserver(m_DicomInteractor, MCH_INPUT);
  }
}
//----------------------------------------------------------------------------
void albaOpImporterRAWImages::CreateGui()
//----------------------------------------------------------------------------
{
  int res=OP_RUN_OK;
  if(!this->m_TestMode)
  {
    m_Dialog = new albaGUIDialogPreview(_("Raw importer"), albaCLOSEWINDOW | albaRESIZABLE | albaUSEGUI | albaUSERWI);

    wxString bit_choices[4] = {_("8 bits"),_("16 bits Big Endian"),_("16 bits Little Endian"),_("24 bits (RGB)")};
    wxString type_choices[2] = {_("interleaved"),_("not interleaved")};

    m_Gui = new albaGUI(this);
    m_Gui->SetListener(this);

    m_Gui->Divider(0);
    m_Gui->DirOpen(ID_OPEN_DIR, _("Raw folder"),&m_RawDirectory);
    m_Gui->String(ID_STRING_PREFIX,_("File pref."), &m_Prefix);
    m_Gui->String(ID_STRING_PATTERN,_("File patt."), &m_Pattern);
    m_Gui->String(ID_STRING_EXT,_("File ext."), &m_Extension);
    m_Gui->Divider(0);
    m_Gui->Combo(ID_BITS,_("Bits/Pixel"),&m_Bit,4,bit_choices);
#ifdef VME_VOLUME_LARGE //LargeReader supports non-interleaved mode
    m_Gui->Combo(ID_RGB_TYPE,"",&m_RgbType,2,type_choices);
#endif
    m_Gui->Enable(ID_RGB_TYPE,false);
    m_Gui->Bool(ID_SIGNED,_("Signed"),&m_Signed);
    m_Gui->Divider(0);
    m_Gui->Label(_("Dimensions (x,y,z):"));
    m_Gui->Label(_("Z is the number of slices"));
    m_Gui->Vector(ID_DIMENSIONS,"",m_Dimension,1,10000,1,10000,1,10000);
    m_Gui->Bool(ID_ROI,_("Define ROI"),&m_CropMode);
    m_Gui->Label(_("Spacing in mm/pixel (x,y,z)"));
    //m_Gui->Vector(ID_SPC, "",m_Spacing,0.0000001, 100000,6); 	
    m_Gui->Double(ID_SPC_X,_("x: "),&m_Spacing[0],0.0000001, 100000,3,6);
    m_Gui->Double(ID_SPC_Y,_("y: "),&m_Spacing[1],0.0000001, 100000,3,6);
    m_Gui->Double(ID_SPC_Z,_("z: "),&m_Spacing[2],0.0000001, 100000,3,6);
    m_Gui->Divider(0);
    m_Gui->Label(_("X coordinates file:"));
    m_Gui->Button(ID_COORD,_("Load"),"", _("Load the file for non regularly spaced raw volume"));
    m_Gui->Divider(0);
    m_Gui->Button(ID_GUESS,_("Guess"),_("Header size"));
    m_Gui->Integer(ID_HEADER," ",&m_Header,0);
    m_Gui->Integer(ID_OFFSET,_("File offset:"),&m_Offset,0, MAXINT,_("Set the first slice number in the files name"));
    m_Gui->Integer(ID_SPACING,_("File spc.:"),&m_FileSpacing,1, MAXINT, _("Set the spacing between the slices in the files name"));
    m_Gui->Divider(0);

    m_Gui->Label(_("Crop Dim."),true);
    m_Gui->Label(_("DimX:"), &m_DimXCrop);
    m_Gui->Label(_("DimY:"), &m_DimYCrop);

    m_DimXCrop = albaString::Format("%d", 0);
    m_DimYCrop = albaString::Format("%d", 0);

		//////////////////////////////////////////////////////////////////////////
		m_Gui->Label("");
		m_Gui->Divider(1);
		m_Gui->OkCancel();
		m_Gui->Label("");

		m_Gui->Show(true);
    m_Gui->Update();
		
    //slice slider +++++++++++++++++++++++++++++++++++++++++++
    wxPoint dp = wxDefaultPosition;
    m_SliceLab     = new wxStaticText(m_Dialog, -1, _(" Slice num. "),dp, wxSize(-1,16));
    m_SliceText    = new wxTextCtrl  (m_Dialog, -1, "",					   dp, wxSize(30,16), wxNO_BORDER);
    m_SliceSlider  = new wxSlider     (m_Dialog, -1,0,0,100,		   dp, wxSize(200,22));

    m_SliceSlider->SetValidator(albaGUIValidator(this,ID_SLICE,m_SliceSlider,&m_CurrentSlice,m_SliceText));
    m_SliceText->SetValidator(albaGUIValidator(this,ID_SLICE,m_SliceText,  &m_CurrentSlice,m_SliceSlider,0,100));

#pragma warning(suppress: 6211) // warning C6211: Leaking memory 'slice_sizer' due to an exception. Consider using a local catch block to clean up memory:
    wxBoxSizer *slice_sizer = new wxBoxSizer(wxHORIZONTAL);
    slice_sizer->Add(m_SliceLab,    0, wxALIGN_CENTER|wxRIGHT, 5);
    slice_sizer->Add(m_SliceText,	 0, wxALIGN_CENTER|wxRIGHT, 5);
    slice_sizer->Add(m_SliceSlider, 1, wxALIGN_CENTER|wxEXPAND);

#pragma warning(suppress: 6211) // warning C6211: Leaking memory 'slice_sizer' due to an exception. Consider using a local catch block to clean up memory:
    m_GuiSlider = new albaGUI(this);      
    m_GuiSlider->Bool(ID_LOOKUPTABLE, _("Use lookup table"), &m_UseLookupTable, 1, 
      _("Determines whether the default lookup table should be used for the preview"));
    m_GuiSlider->Show(true);
    m_GuiSlider->Reparent(m_Dialog);
    slice_sizer->Add(m_GuiSlider, 0, wxALIGN_CENTER|wxLEFT);

    EnableWidgets(false);

    //sizing & positioning +++++++++++++++++++++++++++++++++++++++++++
    int x_init = albaGetFrame()->GetPosition().x;
    int y_init = albaGetFrame()->GetPosition().y;

    wxBoxSizer *v_sizer = new wxBoxSizer(wxVERTICAL);
    //v_sizer->Add(m_Rwi->m_RwiBase,1, wxEXPAND);
    v_sizer->Add(slice_sizer, 0, wxEXPAND);

  
    m_Dialog->GetGui()->AddGui(m_Gui);
    m_Dialog->GetRWI()->SetSize(0,0,380,275);
    m_Dialog->m_RwiSizer->Add(v_sizer, 0, wxEXPAND);
    m_Dialog->GetRWI()->SetListener(this);
    m_Dialog->GetRWI()->SetSize(0,0,500,575);
    m_Dialog->GetRWI()->CameraSet(CAMERA_CT);
    m_Dialog->GetRWI()->m_RenFront->AddActor(m_Actor);
    m_Dialog->GetRWI()->m_RenFront->AddActor(m_GizmoActor);
    m_Dialog->GetRWI()->m_RwiBase->SetMouse(m_Mouse);

    int w,h;
    m_Dialog->GetSize(&w,&h);
    m_Dialog->SetSize(x_init+10,y_init+10,w,h);

    //m_Dialog->SetSizer(h_sizer);    
    //m_Dialog->SetAutoLayout(true);	
    //h_sizer->Fit(m_Dialog);	

    //show the dialog (show return when the user choose ok or cancel ) ++++++++
    m_Dialog->ShowModal();

    //Import the file if required +++++++++++++++++++++++++++++++++	
    res = (m_Dialog->GetReturnCode() == wxID_OK) ? OP_RUN_OK : OP_RUN_CANCEL;
    if(res == OP_RUN_OK)
    { 
      wxBusyInfo wait(_("Importing RAW data, please wait..."));
      wxBusyCursor waitCursor;

      if( !Import() ) res = OP_RUN_CANCEL; // se l'import fallisce devi ritornare OP_RUN_CANCEL, cosi non verra chiamato DO
    }
  }
  else
  {
    if( !Import() ) res = OP_RUN_CANCEL;
  }

  OpStop(res);

  return;
}
//----------------------------------------------------------------------------
void albaOpImporterRAWImages::OpRun()   
//----------------------------------------------------------------------------
{
  CreatePipeline();
  CreateGui();
}
//----------------------------------------------------------------------------
void albaOpImporterRAWImages::OpStop(int result)
//----------------------------------------------------------------------------
{
  if(m_DicomInteractor)
    m_Mouse->RemoveObserver(m_DicomInteractor);

  //cleanup +++++++++++++++++++++++++++++++++
  if(!this->m_TestMode)
  {
    m_Dialog->GetRWI()->m_RenFront->RemoveActor(m_Actor);
    m_Dialog->GetRWI()->m_RenFront->RemoveActor(m_GizmoActor);
  }
  vtkDEL(m_Reader);
  vtkDEL(m_RedImage);
  vtkDEL(m_GreenImage);
  vtkDEL(m_BlueImage);
  vtkDEL(m_AppendComponents);
  vtkDEL(m_InterleavedImage);
  vtkDEL(m_LookupTable);
  vtkDEL(m_Plane);
  vtkDEL(m_Mapper);
  vtkDEL(m_Texture);
  albaDEL(m_DicomInteractor);
  vtkDEL(m_Actor);
  vtkDEL(m_GizmoPlane);
  vtkDEL(m_GizmoActor);
  vtkDEL(m_VtkRawDirectory);
  cppDEL(m_Dialog);

  if(!this->m_TestMode)
    albaEventMacro(albaEvent(this,result));
}
//----------------------------------------------------------------------------
void albaOpImporterRAWImages::OpDo()
//----------------------------------------------------------------------------
{
  assert(m_Output);
  GetLogicManager()->VmeAdd(m_Output);
}
//----------------------------------------------------------------------------
void albaOpImporterRAWImages::OpUndo()
//----------------------------------------------------------------------------
{
  assert(m_Output);
  GetLogicManager()->VmeRemove(m_Output);
}
//----------------------------------------------------------------------------
void albaOpImporterRAWImages::EnableWidgets(bool enable)
//----------------------------------------------------------------------------
{
  _VERIFY_RET(m_Gui != NULL && m_SliceLab != NULL && 
    m_SliceText != NULL && m_SliceSlider != NULL);

  m_Gui->Enable(ID_BITS,		enable);
  if(enable && m_Bit == 3)
    m_Gui->Enable(ID_RGB_TYPE,true);
  else
    m_Gui->Enable(ID_RGB_TYPE,false);
  m_Gui->Enable(ID_SIGNED,	enable);
  m_Gui->Enable(ID_DIMENSIONS, enable);
  m_Gui->Enable(ID_ROI,		  enable);
  m_Gui->Enable(ID_SPC_X,		enable);
  m_Gui->Enable(ID_SPC_Y,		enable); 
  m_Gui->Enable(ID_SPC_Z,		enable); 
  m_Gui->Enable(ID_HEADER,	enable);
  m_Gui->Enable(ID_OFFSET,	enable);
  m_Gui->Enable(ID_SPACING,	enable);
  m_Gui->Enable(ID_GUESS,		enable);
  m_Gui->Enable(ID_STRING_EXT,	   enable);
  m_Gui->Enable(ID_STRING_PREFIX,  enable);
  m_Gui->Enable(ID_STRING_PATTERN, enable);
  m_Gui->Enable(wxOK,		    enable);

  m_SliceLab->Enable(enable);
  m_SliceText->Enable(enable);
  m_SliceSlider->Enable(enable);
}
//----------------------------------------------------------------------------
void albaOpImporterRAWImages::OnEvent(albaEventBase *alba_event) 
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {	  
    case ID_OPEN_DIR:
      {
        OnOpenDir();
      }
      break;
    case ID_STRING_PREFIX:
      {
        OnStringPrefix();
      }
      break;

    case ID_STRING_EXT:
    case ID_STRING_PATTERN:
      UpdateReader();
      break;

    case ID_COORD:
      {	
        m_Gui->Enable(ID_SPC_Z, true);
        wxString rect_dir = albaGetLastUserFolder();
        wxString rect_wildc = _("Z_coordinates (*.txt)|*.txt");
        wxString file = albaGetOpenFile(rect_dir,rect_wildc,_("Open Z coordinates file")).ToAscii();
        if (file != "")
        {
          m_CoordFile = file;
          m_Rect = true;
          m_Spacing[2] = 1.0;
          m_Gui->Enable(ID_SPC_Z, false);						

          const char* nome = (m_CoordFile);
          std::ifstream f_in;
          f_in.open(nome);

          char title[256];

          f_in.getline(title,256);

          if (strcmp (title,"Z coordinates:") != 0)
          {
            wxMessageDialog dialog( NULL,"This is not a Z coordinates file!","",wxOK|wxICON_ERROR);	
            dialog.ShowModal();
            f_in.close();						
            m_Gui->Enable(ID_SPC_Z, true);
            return;
          }

          int j = 0;
          float val;
          while (!f_in.eof()) 
          {
            f_in >> val;
            j++;				
          } 
          j = j-1;

          if (j != m_Dimension[2])
          {
            wxMessageDialog dialog( NULL,"z dimension is not correct!","",wxOK|wxICON_ERROR);
            dialog.ShowModal();
            m_Dimension[2] = j;
            m_Gui->Update();      
          }

          f_in.close();
        }				
      }
      break;
    case ID_SLICE:
      {
        m_SliceSlider->SetValue(m_CurrentSlice);
        m_SliceText->SetLabel(albaString::Format("%d",m_CurrentSlice));
        //throw down
      }
    case ID_LOOKUPTABLE:
    case ID_OFFSET:
    case ID_SPACING:
    case ID_BITS:
    case ID_RGB_TYPE:
    case ID_SIGNED:
    case ID_DIMENSIONS:
    case ID_SPC_X:
    case ID_SPC_Y:
    case ID_SPC_Z:
    case ID_HEADER:
      if(m_Bit == 3)
      {
        m_Texture->SetLookupTable(NULL);
        m_Gui->Enable(ID_RGB_TYPE,true);
      }
      else
      {
        m_Texture->SetLookupTable((vtkLookupTable *)m_LookupTable);
        m_Gui->Enable(ID_RGB_TYPE,false);
      }

      //BES: 10.7.2008 - overwrite lookup table
      if (e->GetId() == ID_BITS)
      {
        m_UseLookupTable = m_Bit != 3;
        m_GuiSlider->Update();
      }

      m_SliceSlider->SetRange(0,m_NumberSlices - 1);
      m_Gui->Update();
      UpdateReader();
      if(e->GetId() == ID_SLICE)
        m_Dialog->GetRWI()->CameraUpdate();
      else
        m_Dialog->GetRWI()->CameraReset();
      break;
    case ID_GUESS:
      {	
        m_Reader->ComputeInternalFileName(m_NumberSlices-1);   
        const char * filename = m_Reader->GetInternalFileName();  				
        int len = GetFileLength(filename); 
        m_Header = len - (m_Dimension[0] * m_Dimension[1] * m_NumberByte);
        m_Gui->Update();
        m_Reader->SetHeaderSize(m_Header);
        m_Reader->Update();
        m_Dialog->GetRWI()->CameraUpdate();
      }
      break;
    case MOUSE_DOWN:
      {		
        if(m_CropMode)
        {
          long handle_id = e->GetArg();
          double pos[3];
          vtkPoints *p = (vtkPoints *)e->GetVtkObj();
          p->GetPoint(0,pos);

          //calculte the rectangle's heigh
          double b[6];
          m_GizmoPlane->GetOutput()->GetBounds(b);
          double dx = (b[1] - b[0]) / 10;
          double dy = (b[3] - b[2]) / 10;

          double O[3], P1[3], P2[3];
          m_GizmoPlane->GetOrigin(O);
          m_GizmoPlane->GetPoint1(P1);
          m_GizmoPlane->GetPoint2(P2);

          if (m_GizmoStatus == GIZMO_NOT_EXIST)
          {
            m_GizmoStatus = GIZMO_RESIZING;
            m_GizmoActor->VisibilityOn();

            pos[2] = 0;
            m_GizmoPlane->SetOrigin(pos);
            m_GizmoPlane->SetPoint1(pos[0], pos[1], pos[2]);
            m_GizmoPlane->SetPoint2(pos[0], pos[1], pos[2]);
          }
          else if (m_GizmoStatus == GIZMO_DONE)
          {
            //	  6------------5----------4--->x
            //		|												|
            //		7												3
            //		|												|
            //		8------------1----------2
            //		|
            //	  v y

            if (P1[0] + dx/2 <= pos[0] &&  pos[0] <= P2[0] - dx/2 && P1[1] - dy/2 <= pos[1] && pos[1] <= P1[1] + dy/2)
              m_SideToBeDragged = 1;
            else if (P2[0] - dx/2 <= pos[0] && pos[0] <= P2[0] + dx/2 && P1[1] - dy/2 <= pos[1] && pos[1] <= P1[1] + dy/2)
              m_SideToBeDragged = 2;
            else if (P2[0] - dx/2 <= pos[0] && pos[0] <= P2[0] + dx/2 && P2[1] + dy/2 <= pos[1] && pos[1] <= P1[1] - dy/2)
              m_SideToBeDragged = 3;
            else if (P2[0] - dx/2 <= pos[0] && pos[0] <= P2[0] + dx/2 && P2[1] - dy/2 <= pos[1] && pos[1] <= P2[1] + dy/2)
              m_SideToBeDragged = 4;
            else if (P1[0] + dx/2 <= pos[0] && pos[0] <= P2[0] - dx/2 && P2[1] - dy/2 <= pos[1] && pos[1] <= P2[1] + dy/2)
              m_SideToBeDragged = 5;
            else if (P1[0] - dx/2 <= pos[0] && pos[0] <= P1[0] + dx/2 && P2[1] - dy/2 <= pos[1] && pos[1] <= P2[1] +dy/2)
              m_SideToBeDragged = 6;
            else if (P1[0] - dx/2 <= pos[0] && pos[0] <= P1[0] + dx/2 && P2[1] + dy/2 <= pos[1] && pos[1] <= P1[1] - dy/2)
              m_SideToBeDragged = 7;
            else if (P1[0] - dx/2 <= pos[0] && pos[0] <= P1[0] + dx/2 && P1[1] - dy/2 <= pos[1] && pos[1] <= P1[1] + dy/2)
              m_SideToBeDragged = 8;
            else
              // hai pickato in un punto che non corrisponde a nessun lato
              // => crea un nuovo gizmo
            {
              m_GizmoStatus = GIZMO_RESIZING;
              m_GizmoActor->VisibilityOn();

              pos[2] = 0;
              m_GizmoPlane->SetOrigin(pos);
              m_GizmoPlane->SetPoint1(pos[0], pos[1], pos[2]);
              m_GizmoPlane->SetPoint2(pos[0], pos[1], pos[2]);
            }
          }
          m_Dialog->GetRWI()->CameraUpdate();


          m_DimXCrop = albaString::Format("%.2f", abs(m_GizmoPlane->GetPoint2()[0]-m_GizmoPlane->GetPoint1()[0]));
          m_DimYCrop = albaString::Format("%.2f", abs(m_GizmoPlane->GetPoint2()[1]-m_GizmoPlane->GetPoint1()[1]));
          m_Gui->Update();
        }
      }
      break;
    case MOUSE_MOVE:  //ridimensiona il gizmo
      {
        long handle_id = e->GetArg();
        double pos[3], oldO[3], oldP1[3], oldP2[3];
        vtkPoints *p = (vtkPoints *)e->GetVtkObj();
        p->GetPoint(0,pos);

        m_GizmoPlane->GetOrigin(oldO);
        m_GizmoPlane->GetPoint1(oldP1);
        m_GizmoPlane->GetPoint2(oldP2);

        if (m_GizmoStatus == GIZMO_RESIZING)
        {
          m_GizmoPlane->SetPoint1(oldO[0], pos[1], oldP1[2]);
          m_GizmoPlane->SetPoint2(pos[0], oldO[1], oldP1[2]);
        }
        else if (m_GizmoStatus == GIZMO_DONE)
        {
          if (m_SideToBeDragged == 0) {}
          else if (m_SideToBeDragged == 1)
            m_GizmoPlane->SetPoint1(oldP1[0], pos[1], oldP1[2]);
          else if (m_SideToBeDragged == 2)
          {
            m_GizmoPlane->SetPoint1(oldP1[0], pos[1], oldP1[2]);
            m_GizmoPlane->SetPoint2(pos[0], oldP2[1], oldP2[2]);
          }
          else if (m_SideToBeDragged == 3)
            m_GizmoPlane->SetPoint2(pos[0], oldP2[1], oldP2[2]);
          else if (m_SideToBeDragged == 4)
          {
            m_GizmoPlane->SetOrigin(oldO[0], pos[1], oldO[2]);
            m_GizmoPlane->SetPoint2(pos[0], pos[1], oldP2[2]);
          }
          else if (m_SideToBeDragged == 5)
          {
            m_GizmoPlane->SetOrigin(oldO[0], pos[1], oldO[2]);
            m_GizmoPlane->SetPoint2(oldP2[0], pos[1], oldP2[2]);
          }
          else if (m_SideToBeDragged == 6)
          {
            m_GizmoPlane->SetOrigin(pos[0], pos[1], oldO[2]);
            m_GizmoPlane->SetPoint1(pos[0], oldP1[1], oldP2[2]);
            m_GizmoPlane->SetPoint2(oldP2[0], pos[1], oldP2[2]);
          }
          else if (m_SideToBeDragged == 7)
          {
            m_GizmoPlane->SetOrigin(pos[0], oldO[1], oldO[2]);
            m_GizmoPlane->SetPoint1(pos[0], oldP1[1], oldP1[2]);
          }
          else if (m_SideToBeDragged == 8)
          {
            m_GizmoPlane->SetOrigin(pos[0], oldO[1], oldO[2]);
            m_GizmoPlane->SetPoint1(pos[0], pos[1], oldP1[2]);
          }
        }
        m_Dialog->GetRWI()->CameraUpdate();
        m_DimXCrop = albaString::Format("%.2f", abs(m_GizmoPlane->GetPoint2()[0]-m_GizmoPlane->GetPoint1()[0]));
        m_DimYCrop = albaString::Format("%.2f", abs(m_GizmoPlane->GetPoint2()[1]-m_GizmoPlane->GetPoint1()[1]));
        m_Gui->Update();
      }
      break;
    case MOUSE_UP:  //blocca il gizmo
      if (m_CropMode)
      {
        if (m_GizmoStatus == GIZMO_RESIZING)
          m_GizmoStatus = 	GIZMO_DONE;
        else if (m_GizmoStatus == GIZMO_DONE)
          m_SideToBeDragged = 0;
        double b[6], img_bounds[6];
        m_Reader->GetOutput()->GetBounds(img_bounds);
        m_GizmoPlane->GetOutput()->GetBounds(b);
        m_ROI_2D[0] = b[0];
        m_ROI_2D[1] = b[1];
        m_ROI_2D[2] = b[2];
        m_ROI_2D[3] = b[3];
      }
      m_DimXCrop = albaString::Format("%.2f", abs(m_GizmoPlane->GetPoint2()[0]-m_GizmoPlane->GetPoint1()[0]));
      m_DimYCrop = albaString::Format("%.2f", abs(m_GizmoPlane->GetPoint2()[1]-m_GizmoPlane->GetPoint1()[1]));
      m_Gui->Update();
      break;
    case wxOK:
        if(!ControlFilenameList())
        {
          wxMessageBox("Control numerical order of raw files");
          return;
        }
        m_Dialog->EndModal(wxID_OK);
      break;
    case wxCANCEL:
      m_Dialog->EndModal(wxID_CANCEL);
      break;
    default:
      albaEventMacro(*e);
      break;
    }
  }
}


//----------------------------------------------------------------------------
void albaOpImporterRAWImages::	UpdateReader() 
//----------------------------------------------------------------------------
{
  wxString prefix = m_RawDirectory + "\\" + m_Prefix;
  m_Reader->SetFilePrefix(prefix);

  wxString pattern = m_Pattern + m_Extension;
  m_Reader->SetFilePattern(pattern.ToAscii());

  m_Reader->SetDataScalarType(GetVTKDataType());
  m_Reader->SetNumberOfScalarComponents(1);
  switch(m_Bit)
  {	  
  case 0:
    m_NumberByte = 1;
    break;
  case 1:
    m_Reader->SetDataByteOrderToBigEndian();
    m_NumberByte = 2;
    break;
  case 2:
    m_Reader->SetDataByteOrderToLittleEndian();
    m_NumberByte = 2;
    break;
  case 3:
    m_NumberByte = 3;
#ifndef VME_VOLUME_LARGE //LargeReader support non-interleaved mode
    if(m_RgbType == 0)
#endif
      m_Reader->SetNumberOfScalarComponents(3);
    break;
  }

  m_Reader->SetFileNameSliceOffset(m_Offset);
  m_Reader->SetFileNameSliceSpacing(m_FileSpacing);
#ifndef VME_VOLUME_LARGE //LargeReader support non-interleaved mode
  if(m_RgbType == 0)
  {
#endif
    m_Reader->SetDataExtent(0, m_Dimension[0] - 1, 0, m_Dimension[1] - 1, 0, m_NumberSlices - 1);
    m_Reader->SetDataVOI(0, m_Dimension[0] - 1, 0, m_Dimension[1] - 1, m_CurrentSlice, m_CurrentSlice);
#ifndef VME_VOLUME_LARGE //LargeReader support non-interleaved mode
  }
  else
  {
    m_Reader->SetDataExtent(0, m_Dimension[0] - 1, 0, (m_Dimension[1]*3) - 1, 0, m_NumberSlices - 1);
    m_Reader->SetDataVOI(0, m_Dimension[0] - 1, 0, (m_Dimension[1]*3) - 1, m_CurrentSlice, m_CurrentSlice);
  }
#endif

  m_Reader->SetDataSpacing(m_Spacing);
  m_Reader->SetHeaderSize(m_Header);
  m_Reader->SetFileDimensionality(2);
  m_Reader->SetDataOrigin(0, 0, m_Offset);

  m_Plane->SetPoint1(m_Dimension[0],0,0);
  m_Plane->SetPoint2(0,m_Dimension[1],0);


  if(m_RgbType)
  {
    // convert non interleaved images into interleaved images.
    const void *rgb_InputPointer = m_Reader->GetOutput()->GetPointData()->GetScalars()->GetVoidPointer(0);
    int num_in_scalars  = m_Reader->GetOutput()->GetPointData()->GetNumberOfTuples();

    m_RedImage->SetNumberOfScalarComponents(1);
    m_RedImage->SetDataExtent(0, m_Dimension[0] - 1, 0, m_Dimension[1] - 1, 0, m_NumberSlices - 1);
    m_RedImage->SetDataScalarTypeToUnsignedChar();
    m_RedImage->SetDataSpacing(m_Spacing);
    m_RedImage->SetImportVoidPointer((unsigned char *)rgb_InputPointer);
    m_GreenImage->SetNumberOfScalarComponents(1);
    m_GreenImage->SetDataExtent(0, m_Dimension[0] - 1, 0, m_Dimension[1] - 1, 0, m_NumberSlices - 1);
    m_GreenImage->SetDataScalarTypeToUnsignedChar();
    m_GreenImage->SetDataSpacing(m_Spacing);
    m_GreenImage->SetImportVoidPointer((unsigned char *)rgb_InputPointer + m_Dimension[0] * m_Dimension[1]);
    m_BlueImage->SetNumberOfScalarComponents(1);
    m_BlueImage->SetDataExtent(0, m_Dimension[0] - 1, 0, m_Dimension[1] - 1, 0, m_NumberSlices - 1);
    m_BlueImage->SetDataScalarTypeToUnsignedChar();
    m_BlueImage->SetDataSpacing(m_Spacing);
    m_BlueImage->SetImportVoidPointer((unsigned char *)rgb_InputPointer + 2 * m_Dimension[0] * m_Dimension[1]);

    m_RedImage->Update();
    m_GreenImage->Update();
    m_BlueImage->Update();
    m_AppendComponents->Modified();
    m_AppendComponents->Update();
    m_InterleavedImage->Modified();
    m_InterleavedImage->Update();
    m_Texture->SetInputConnection(m_InterleavedImage->GetOutputPort());
    m_Texture->SetInputConnection(m_AppendComponents->GetOutputPort());
    m_Texture->Modified();
  }
  else
  {
    m_Texture->SetInputConnection(m_Reader->GetOutputPort());
  }

  m_Reader->Update();
  double range[2];
  m_Reader->GetOutput()->GetScalarRange(range);

  m_LookupTable->SetTableRange(range);
  m_LookupTable->SetWindow(range[1] - range[0]);
  m_LookupTable->SetLevel((range[1] + range[0]) / 2.0);
  m_LookupTable->Build();
}
//----------------------------------------------------------------------------
bool albaOpImporterRAWImages::Import()
//----------------------------------------------------------------------------
{
  wxString prefix = m_RawDirectory + "\\" + m_Prefix;
  wxString pattern = m_Pattern + m_Extension;
  vtkALBASmartPointer< vtkImageReader > r;
  r->SetFilePrefix(prefix);
  r->SetFilePattern(pattern.ToAscii());

  r->SetDataScalarType(GetVTKDataType());
  r->SetNumberOfScalarComponents(1);
  r->SetDataByteOrderToLittleEndian();
  switch(m_Bit)
  {	  
  case 1:
    r->SetDataByteOrderToBigEndian();
    break;
  case 3:

      r->SetNumberOfScalarComponents(3);
    break;
  }

  r->SetFileNameSliceOffset(m_Offset);
  r->SetFileNameSliceSpacing(m_FileSpacing);

  if(m_RgbType == 0)
  {
    r->SetDataExtent(0, m_Dimension[0] - 1, 0, m_Dimension[1] - 1, 0, m_NumberSlices - 1);
    if(this->m_CropMode)
      r->SetDataVOI(m_ROI_2D[0], m_ROI_2D[1], m_ROI_2D[2], m_ROI_2D[3], 0, m_Dimension[2] - 1);
    else
      r->SetDataVOI(0, m_Dimension[0] - 1, 0, m_Dimension[1] - 1, 0, m_Dimension[2] - 1);
  }
  else
  {
    r->SetDataExtent(0, m_Dimension[0] - 1, 0, (m_Dimension[1]*3) - 1, 0, m_NumberSlices - 1);
    r->SetDataVOI(0, m_Dimension[0] - 1, 0, (m_Dimension[1]*3) - 1, 0, m_Dimension[2] - 1);
  }

  r->SetDataSpacing(m_Spacing);
  r->SetHeaderSize(m_Header);
  r->SetFileDimensionality(2);
  r->SetDataOrigin(0, 0, m_Offset);
  r->Update();

  wxString slice_name = m_RawDirectory;
  wxString path, name, ext;
	wxFileName::SplitPath(slice_name, &path, &name, &ext);

  vtkALBASmartPointer<vtkImageToStructuredPoints> convert;

  vtkDoubleArray* ZDoubleArray = NULL;
  if (m_Rect)
  {    
    //if we want to construct a rectilinear grid, load z-coordinates
    ZDoubleArray = vtkDoubleArray::New();

    char title[256];
    const char* nome = (m_CoordFile);
    std::ifstream f_in;
    f_in.open(nome);
    f_in.getline(title,256);

    //z array is read from a file	
    for (int i = 0; i <= m_Dimension[2]; i++)
    {
      double currentValue;
      f_in >> currentValue;
      ZDoubleArray->InsertNextValue(currentValue);
    }
    f_in.close();
  }

  if(m_RgbType)
  {
    // convert non interleaved images into interleaved images.
    const void *rgb_InputPointer = r->GetOutput()->GetPointData()->GetScalars()->GetVoidPointer(0);
    int num_in_scalars  = r->GetOutput()->GetPointData()->GetNumberOfTuples();

    m_RedImage->SetNumberOfScalarComponents(1);
    m_RedImage->SetDataExtent(0, m_Dimension[0] - 1, 0, m_Dimension[1] - 1, 0, m_NumberSlices - 1);
    m_RedImage->SetDataScalarTypeToUnsignedChar();
    m_RedImage->SetDataSpacing(m_Spacing);
    m_RedImage->SetImportVoidPointer((unsigned char *)rgb_InputPointer);
    m_GreenImage->SetNumberOfScalarComponents(1);
    m_GreenImage->SetDataExtent(0, m_Dimension[0] - 1, 0, m_Dimension[1] - 1, 0, m_NumberSlices - 1);
    m_GreenImage->SetDataScalarTypeToUnsignedChar();
    m_GreenImage->SetDataSpacing(m_Spacing);
    m_GreenImage->SetImportVoidPointer((unsigned char *)rgb_InputPointer + m_Dimension[0] * m_Dimension[1]);
    m_BlueImage->SetNumberOfScalarComponents(1);
    m_BlueImage->SetDataExtent(0, m_Dimension[0] - 1, 0, m_Dimension[1] - 1, 0, m_NumberSlices - 1);
    m_BlueImage->SetDataScalarTypeToUnsignedChar();
    m_BlueImage->SetDataSpacing(m_Spacing);
    m_BlueImage->SetImportVoidPointer((unsigned char *)rgb_InputPointer + 2 * m_Dimension[0] * m_Dimension[1]);

    m_AppendComponents->Modified();
    m_InterleavedImage->Modified();
    convert->SetInputConnection(r->GetOutputPort());
  }
  else
  {
    convert->SetInputConnection(r->GetOutputPort());
  }

  convert->Update();

  ///////////////////////////////////////////////////////////////////////
  if(m_Rect)
  {
    // conversion from vtkImageData to vtkRectilinearGrid
    vtkImageData	*structured_data = convert->GetOutput();
    vtkPointData *data = structured_data->GetPointData();
    vtkDataArray *scalars = data->GetScalars();
    vtkDoubleArray *XDoubleArray = vtkDoubleArray::New();
    vtkDoubleArray *YDoubleArray = vtkDoubleArray::New();

    double origin[3];
    double currentValue;
    structured_data->GetOrigin(origin);

    for (int ix = 0; ix < m_Dimension[0]; ix++)
    {
      currentValue =  origin[0]+((double)ix)*m_Spacing[0];
      XDoubleArray->InsertNextValue(currentValue);					
    }

    for (int iy = 0; iy < m_Dimension[1]; iy++)
    {
      currentValue =  origin[1]+((double)iy)*m_Spacing[1];
      YDoubleArray->InsertNextValue(currentValue);					
    }

    vtkRectilinearGrid *rectilinear_data = vtkRectilinearGrid::New();
    rectilinear_data->SetXCoordinates(XDoubleArray);
    rectilinear_data->SetYCoordinates(YDoubleArray);
    rectilinear_data->SetZCoordinates(ZDoubleArray);
    rectilinear_data->SetDimensions(m_Dimension[0],m_Dimension[1],m_Dimension[2]);
    rectilinear_data->GetPointData()->SetScalars(scalars);

    vtkDEL(XDoubleArray);
    vtkDEL(YDoubleArray);
    vtkDEL(ZDoubleArray);

    //create VME
    albaNEW(m_VolumeGray);
    albaNEW(m_VolumeRGB);
    if (m_VolumeGray->SetDataByDetaching((vtkDataSet *)rectilinear_data,0) == ALBA_OK)
    {
      m_Output = m_VolumeGray;
    }
    else if (m_VolumeRGB->SetDataByDetaching((vtkDataSet *)rectilinear_data,0) == ALBA_OK)
    {
      m_Output = m_VolumeRGB;
    }
    else
    {
      wxMessageBox(_("Some importing error occurred!!"), _("Warning!"));
      vtkDEL(rectilinear_data);        
      return false;
    }

    vtkDEL(rectilinear_data);
  } 
  else //if (m_Rect)
  {
    assert(ZDoubleArray == NULL);

    albaNEW(m_VolumeGray);
    albaNEW(m_VolumeRGB);
    if (m_VolumeGray->SetDataByDetaching((vtkDataSet *)convert->GetOutput(),0) == ALBA_OK)
    {
      m_Output = m_VolumeGray;
    }
    else if (m_VolumeRGB->SetDataByDetaching((vtkDataSet *)convert->GetOutput(),0) == ALBA_OK)
    {
      m_Output = m_VolumeRGB;
    }
    else
    {
      wxMessageBox(_("Some importing error occurred!!"), _("Warning!"));
      return false;
    }
  }
   if(!m_Output) return false;	

   albaTagItem tag_Nature;
   tag_Nature.SetName(_("VME_NATURE"));
   tag_Nature.SetValue(_("NATURAL"));

   m_Output->SetName(name.ToAscii());
   m_Output->ReparentTo(m_Input);

   m_Output->GetTagArray()->SetTag(tag_Nature);
   //m_Output->Register(m_Output); //increment reference count so that the vme can't die

   return true;
}
//----------------------------------------------------------------------------
int albaOpImporterRAWImages::GetFileLength(const char * filename) 
//----------------------------------------------------------------------------
{
  int l,m,len;
  std::ifstream file (filename, ios::in|ios::binary);
  l = file.tellg();
  file.seekg (0, ios::end);
  m = file.tellg();
  file.close();
  len = (m-l);
  return len;
}
//----------------------------------------------------------------------------
void albaOpImporterRAWImages::OnStringPrefix() 
//----------------------------------------------------------------------------
{
  const char* nome_file = (m_Prefix);
  int length = m_Prefix.Len();
  m_NumberSlices = 0;
  for( int i = 0; i < m_NumberFile; i++)
  {
    if((strncmp(m_VtkRawDirectory->GetFile(i),nome_file,length) == 0))
      m_NumberSlices++;	
  }

  UpdateReader();
  m_Dimension[2] = m_NumberSlices;
  if(!this->m_TestMode)
  {
    m_SliceSlider->SetRange(0,m_NumberSlices - 1);
    m_CurrentSlice = m_NumberSlices/2;	
    m_SliceSlider->SetValue(m_CurrentSlice);
    m_SliceText->SetLabel(albaString::Format("%d",m_CurrentSlice));
    m_Actor->VisibilityOn();
    m_Dialog->GetRWI()->CameraReset();    	 
    m_Gui->Update();
  }
}
//----------------------------------------------------------------------------
void albaOpImporterRAWImages::OnOpenDir() 
//----------------------------------------------------------------------------
{
  if(!this->m_TestMode)
    wxBusyInfo wait("Reading File, please wait...");

  vtkNEW(m_VtkRawDirectory);
  if (m_VtkRawDirectory->Open(m_RawDirectory) == 0)
    albaLogMessage("Directory <%s> can not be opened", m_RawDirectory);

  if(!this->m_TestMode)
  {
    wxBusyInfo wait_info("Reading raw directory: please wait");
    EnableWidgets(true);
  }

  m_NumberFile = m_VtkRawDirectory->GetNumberOfFiles();
  m_NumberSlices = 0;
}
//----------------------------------------------------------------------------
bool albaOpImporterRAWImages::ControlFilenameList()
//----------------------------------------------------------------------------
{
  //control how many files are present in directory
  wxString prefix = m_RawDirectory + "\\" + m_Prefix;
  wxString pattern = m_Pattern + m_Extension;

  wxDir dir(m_RawDirectory.GetCStr());
  bool result = true;
  if ( !dir.IsOpened() )
  {
    // deal with the error here - wxDir would already log an error message
    // explaining the exact reason of the failure
    result = false;
  }

  wxArrayString SkinFiles;
  const wxString FileSpec = "*" + m_Extension;
  const int flags = wxDIR_FILES;

  if (m_RawDirectory != wxEmptyString && wxDirExists(m_RawDirectory.GetCStr()))
  {
    // Get all .zip files
    wxDir::GetAllFiles(m_RawDirectory.GetCStr(), &SkinFiles, FileSpec, flags);
  }

  
  wxString numbers = m_Pattern.AfterLast('%');
  numbers = numbers.Mid(0,numbers.Length()-1);

  int n = atoi(numbers.ToAscii());

  for(long number = m_Offset,  j =0; number<m_Offset+SkinFiles.GetCount()*m_FileSpacing;number = number + m_FileSpacing, j++)
  {
    wxString fileName = SkinFiles[j];
    fileName = fileName.AfterLast('\\');
    fileName = fileName.BeforeLast('.');

    wxString search;
    search = albaString::Format("%ld",number);

    wxString final;
    for(int i=0; i< n; i++)
    {
      final.Append("0");
    }
    final.Append(search);
    final = final.Mid(final.Length()-numbers.Length()-2,final.Length());

    wxString control;
    control = m_Prefix;
    control.Append(final);

    if(fileName.CompareTo(control) != 0)
    {
      result = false;
      break;
    }
  }

  return result;
}

