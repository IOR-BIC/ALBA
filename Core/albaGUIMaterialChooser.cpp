/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIMaterialChooser
 Authors: Paolo Quadrani
 
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

#include "albaGUIMaterialChooser.h"

#include <wx/event.h>
#include "wx/busyinfo.h"
#include <wx/settings.h>
#include <wx/laywin.h>
#include <wx/treectrl.h>
#include <wx/bmpbuttn.h>

#include <fstream>

#include "albaDecl.h"
#include "mmaMaterial.h"
#include "albaRWIBase.h"
#include "albaGUIButton.h"
#include "albaGUIMaterialButton.h"
#include "albaGUIListCtrlBitmap.h"
#include "albaGUI.h"
#include "albaRWI.h"
#include "albaGUIPicButton.h"

#include "albaXMLStorage.h"
#include "albaStorageElement.h"
#include "albaVME.h"

#include "vtkProperty.h"
#include "vtkSphereSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkWindowToImageFilter.h"
#include "vtkWindow.h"
#include "vtkRenderWindow.h"
#include "vtkImageData.h"
#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkInteractorStyle.h"
#include "vtkImageShrink3D.h"
#include "vtkLight.h"
#include "vtkImageExport.h"
#include "albaViewManager.h"

//----------------------------------------------------------------------------
albaGUIMaterialChooser::albaGUIMaterialChooser(wxString dialog_title)
//----------------------------------------------------------------------------
{  
	m_ChoosedMaterial	= NULL;
  m_VmeMaterial     = NULL;
  m_Vme             = NULL;
  m_Dialog			    = NULL;
	m_Gui					    = NULL;
	m_ListCtrlMaterial= NULL;
	m_RWI					    = NULL;
	m_Sphere					= NULL;
	m_Property				= NULL;
	m_Mapper					= NULL;
	m_Actor						= NULL;
	

  m_Filename = albaGetAppDataDirectory();
  m_Filename += "/mat_library.xml";
  
  //initialize first material 
  m_MaterialName      = "new material";
	m_AmbientColor      = wxColour(0,0,0);
	m_DiffuseColor      = albaRandomColor();
	m_SpecularColor     = wxColour(255,255,255); 
  m_AmbientIntensity  = 0.1;
  m_DiffuseIntensity  = 1;
	m_SpecularIntensity = 0.5;
	m_SpecularPower     = 70; 
  m_Opacity           = 1;
  m_Wire              = 0;
}
//----------------------------------------------------------------------------
albaGUIMaterialChooser::~albaGUIMaterialChooser()
//----------------------------------------------------------------------------
{
  albaDEL(m_ChoosedMaterial); //BES: 31.3.2009 - fixed memory leak

  ClearList();
	
	if(m_RWI)
		m_RWI->m_RenFront->RemoveActor(m_Actor);

	vtkDEL(m_Sphere);
	vtkDEL(m_Property);
  vtkDEL(m_Mapper);
  vtkDEL(m_Actor);
	cppDEL(m_RWI); 
  cppDEL(m_Dialog);
};
//----------------------------------------------------------------------------
bool albaGUIMaterialChooser::ShowChooserDialog(albaVME *vme, bool remember_last_material)
//----------------------------------------------------------------------------
{
	if (m_Gui == NULL)
	{
		CreateGUI();
		CreatePipe();
	}

	LoadLibraryFromFile();
	
  if(m_List.empty())
	{
    wxBusyInfo wait("Creating material library: Please wait");
	  CreateDefaultLibrary();
	}

  m_Vme = vme;
  m_VmeMaterial = (mmaMaterial *)m_Vme->GetAttribute("MaterialAttributes");
  assert(m_VmeMaterial);

  if (!remember_last_material)
  {
    albaDEL(m_ChoosedMaterial);// = NULL;
  }

	m_ChoosedMaterialId = -1;
  SetFromMat(m_VmeMaterial);



  bool res = m_Dialog->ShowModal() != 0;
  m_VmeMaterial	= NULL;
  return res;
}

//----------------------------------------------------------------------------
mmaMaterial* albaGUIMaterialChooser::GetMaterial(int id)
{
	if (m_List.empty())
		CreateDefaultLibrary();
	return m_List[id];
}

//----------------------------------------------------------------------------
// constant :
//----------------------------------------------------------------------------
enum MATERIAL_PROPERTY_ID
{
	ID_NAME = MINID,
	ID_AMBIENT_INTENSITY,
	ID_AMBIENT_COLOR,
	ID_DIFFUSE_INTENSITY,
	ID_DIFFUSE_COLOR,
	ID_SPECULAR_INTENSITY,
	ID_SPECULAR_COLOR,
	ID_SPECULAR_POWER,
	ID_OPACITY,
	ID_WIRE,
	ID_ADD,
	ID_REMOVE,
	ID_RESET_MATERIALS,
	ID_BITMAP,
  ID_OK,
  ID_CANCEL,
  ID_APPLY
};
//----------------------------------------------------------------------------
void albaGUIMaterialChooser::CreateGUI() 
//----------------------------------------------------------------------------
{
	wxString wildcard = "xml file (*.xml)|*.xml|all files (*.*)|*.*";

  int x_pos,y_pos,w,h;
  albaGetFrame()->GetPosition(&x_pos,&y_pos);
  m_Dialog = new wxDialog(albaGetFrame(),-1,"Choose Material",
	wxDefaultPosition,wxDefaultSize,wxDEFAULT_DIALOG_STYLE  | wxCAPTION | wxRESIZE_BORDER);
  m_Dialog->GetSize(&w,&h);
  m_Dialog->SetSize(x_pos+5,y_pos+5,w,h);
  //m_Dialog->SetMinSize(wxSize(w,h));

	m_ListCtrlMaterial = new albaGUIListCtrlBitmap(m_Dialog,-1,0,1); 
	m_ListCtrlMaterial->SetTitle("Material library"); 
	m_ListCtrlMaterial->SetListener(this); 
	m_ListCtrlMaterial->Reset();
	//m_ListCtrlMaterial->SetSize(wxSize(200,500));
  m_ListCtrlMaterial->SetMinSize(wxSize(200,500));

	// RWI ============================
	m_RWI = new albaRWI(m_Dialog,ONE_LAYER);
	m_RWI->SetSize(0,0,50,50);
  m_RWI->m_RwiBase->Show(true);
	// GUI ============================
	m_Gui = new albaGUI(this);
  
  //m_Gui->Label		(""); 
	m_Gui->Label("Current material",1);
	m_Gui->String(ID_NAME,"Name",&m_MaterialName);

	m_Gui->Label		("");
	m_Gui->Label		("");

	m_Gui->Label		("Ambient",1);
	m_Gui->FloatSlider(ID_AMBIENT_INTENSITY,"Intensity",&m_AmbientIntensity,0.0,1.0);
	m_Gui->Color(ID_AMBIENT_COLOR,"Color",&m_AmbientColor);

	m_Gui->Label		("Diffuse",1);
	m_Gui->FloatSlider(ID_DIFFUSE_INTENSITY,"Intensity",&m_DiffuseIntensity,0.0,1.0);
	m_Gui->Color(ID_DIFFUSE_COLOR,"Color",&m_DiffuseColor);

	m_Gui->Label		("Specular",1);
	m_Gui->FloatSlider(ID_SPECULAR_INTENSITY,"Intensity",&m_SpecularIntensity,0.0,1.0);
	m_Gui->Color(ID_SPECULAR_COLOR,"Color",&m_SpecularColor);
	m_Gui->Slider(ID_SPECULAR_POWER,"Power",&m_SpecularPower,1,100);

	m_Gui->Label		("");
	m_Gui->FloatSlider(ID_OPACITY,"Opacity",&m_Opacity,0.0,1.0);
	m_Gui->Bool(ID_WIRE,"Wireframe",&m_Wire,0);

  m_Gui->Label		("");
	m_Gui->Button(ID_ADD,"Store current material in Library");
	m_Gui->Button(ID_REMOVE,"Remove selected material from Library");
	m_Gui->Button(ID_RESET_MATERIALS,"Reset Library");
  m_Gui->Enable(ID_REMOVE, false);
	m_Gui->Label		("");
	m_Gui->Label		("");
  //m_Gui->OkCancel();
  int bh = m_Gui->GetMetrics(GUI_BUTTON_HEIGHT);
  int fw = m_Gui->GetMetrics(GUI_FULL_WIDTH);
  m_OkButton  = new albaGUIButton (m_Gui,ID_OK, "Ok", wxDefaultPosition,wxSize(fw/3, bh));
  m_CancelButton = new albaGUIButton (m_Gui,ID_CANCEL,"Cancel",wxDefaultPosition,wxSize(fw/3, bh));
  m_ApplyButton = new albaGUIButton (m_Gui,ID_APPLY,"Apply",wxDefaultPosition,wxSize(fw/3, bh));
  m_OkButton->SetListener(this);
  m_CancelButton->SetListener(this);
  m_ApplyButton->SetListener(this);
  wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
  sizer->Add(m_OkButton,  0, wxRIGHT);
  sizer->Add(m_CancelButton, 0, wxRIGHT);
  sizer->Add(m_ApplyButton, 0, wxRIGHT);
  m_Gui->Add(sizer,0,wxALL); 

	//m_Gui->Update();
	//m_Gui->SetSize(wxSize(220,520));
  //m_Gui->SetMinSize(wxSize(220,520));
	m_Gui->Divider();
	m_Gui->Reparent(m_Dialog);
  m_Gui->Show(true);
  m_Gui->Fit();
  
	wxBoxSizer *v1_sizer = new wxBoxSizer(wxVERTICAL);
	v1_sizer->Add(m_ListCtrlMaterial,1,wxEXPAND | wxALL, 6);

	wxBoxSizer *v2_sizer = new wxBoxSizer(wxVERTICAL);
  v2_sizer->Add(m_RWI->m_RwiBase,0,wxALIGN_CENTRE);
  v2_sizer->Add(m_Gui,0,wxALL, 6);

	wxBoxSizer *main_sizer = new wxBoxSizer(wxHORIZONTAL);
	main_sizer->Add(v1_sizer,1,wxEXPAND | wxALIGN_LEFT);
	main_sizer->Add(v2_sizer,0,wxALIGN_LEFT);
  m_RWI->m_RwiBase->Show(true);

	// ATTACH SIZER TO DIALOG
  m_Dialog->SetSizer( main_sizer );
  main_sizer->SetSizeHints(m_Dialog);
}
//----------------------------------------------------------------------------
void albaGUIMaterialChooser::CreatePipe()
//----------------------------------------------------------------------------
{
  wxColour col = wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT);
  double br = col.Red()/255.0;
  double bg = col.Green()/255.0;
  double bb = col.Blue()/255.0;

	m_RWI->m_RenFront->SetBackground(br,bg,bb);
	m_RWI->m_RenFront->LightFollowCameraOff();
  m_RWI->m_Light->SetPosition(-1,1,1);
  m_RWI->m_Camera->ParallelProjectionOff();
  m_RWI->m_Camera->SetViewAngle(30);
  m_RWI->m_Camera->SetFocalPoint(0,0,0);
  m_RWI->m_Camera->SetPosition(0,0,2);
  m_RWI->m_Camera->SetViewUp(0,1,0);
	m_RWI->m_RwiBase->GetInteractorStyle()->SetInteractor(NULL);
	m_RWI->m_RwiBase->SetInteractorStyle(NULL);

	m_Sphere = vtkSphereSource::New();
	m_Sphere->SetPhiResolution(20);
	m_Sphere->SetThetaResolution(20);
	
	m_Mapper = vtkPolyDataMapper::New();
	m_Mapper->SetInputConnection(m_Sphere->GetOutputPort());
	m_Mapper->SetScalarVisibility(1);
  m_Mapper->SetColorMode(0);     
  m_Mapper->SetScalarMode(0);      
  m_Mapper->SetScalarRange(0,1);     
   	 
 	m_Property = vtkProperty::New();
  m_Property->SetEdgeVisibility(0);
  m_Property->SetInterpolation(1);
  m_Property->SetAmbient(m_AmbientIntensity);
  m_Property->SetDiffuse(m_DiffuseIntensity);
  m_Property->SetSpecular(m_SpecularIntensity);
  m_Property->SetSpecularPower(m_SpecularPower);
	m_Property->SetOpacity(m_Opacity);
      
	double r,g,b;
	r = m_AmbientColor.Red()/255.0;
	g = m_AmbientColor.Green()/255.0;
	b = m_AmbientColor.Blue()/255.0;
	m_Property->SetAmbientColor(r,g,b); 

	r = m_DiffuseColor.Red()/255.0;
	g = m_DiffuseColor.Green()/255.0;
	b = m_DiffuseColor.Blue()/255.0;
	m_Property->SetDiffuseColor(r,g,b); 

	r = m_SpecularColor.Red()/255.0;
	g = m_SpecularColor.Green()/255.0;
	b = m_SpecularColor.Blue()/255.0;
	m_Property->SetSpecularColor(r,g,b); 

	if(m_Wire) 
	  m_Property->SetRepresentationToWireframe();
  else
	  m_Property->SetRepresentationToSurface();

	m_Actor = vtkActor::New();
  m_Actor->SetMapper(m_Mapper);
  m_Actor->SetProperty(m_Property);
  m_Actor->SetPickable(1);
  m_Actor->SetVisibility(1);
  m_Actor->SetPosition(0,0,0); 
  m_Actor->SetScale(1,1,1); 
	m_RWI->m_RenFront->AddActor(m_Actor);
}
//----------------------------------------------------------------------------
void albaGUIMaterialChooser::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    double r,g,b;
    switch(e->GetId())
    {
      case ID_NAME:
        m_ChoosedMaterial->m_MaterialName = m_MaterialName;
      break;
      case ID_AMBIENT_INTENSITY:
        m_Property->SetAmbient(m_AmbientIntensity);
        m_ChoosedMaterial->m_AmbientIntensity = m_AmbientIntensity;
      break;
      case ID_AMBIENT_COLOR:
        r = m_AmbientColor.Red()  / 255.0;
        g = m_AmbientColor.Green()/ 255.0;
        b = m_AmbientColor.Blue() / 255.0;
        m_Property->SetAmbientColor(r,g,b); 
        m_ChoosedMaterial->m_Ambient[0] = r;
        m_ChoosedMaterial->m_Ambient[1] = g;
        m_ChoosedMaterial->m_Ambient[2] = b;
      break;
      case ID_DIFFUSE_INTENSITY:
        m_Property->SetDiffuse(m_DiffuseIntensity);
        m_ChoosedMaterial->m_DiffuseIntensity = m_DiffuseIntensity;
      break;
      case ID_DIFFUSE_COLOR:
        r = m_DiffuseColor.Red()  / 255.0;
        g = m_DiffuseColor.Green()/ 255.0;
        b = m_DiffuseColor.Blue() / 255.0;
        m_Property->SetDiffuseColor(r,g,b); 
        m_ChoosedMaterial->m_Diffuse[0] = r;
        m_ChoosedMaterial->m_Diffuse[1] = g;
        m_ChoosedMaterial->m_Diffuse[2] = b;
      break;
      case ID_SPECULAR_INTENSITY:
        m_Property->SetSpecular(m_SpecularIntensity);
        m_ChoosedMaterial->m_SpecularIntensity = m_SpecularIntensity;
      break;
      case ID_SPECULAR_COLOR:
        r = m_SpecularColor.Red()  / 255.0;
        g = m_SpecularColor.Green()/ 255.0;
        b = m_SpecularColor.Blue() / 255.0;
        m_Property->SetSpecularColor(r,g,b); 
        m_ChoosedMaterial->m_Specular[0] = r;
        m_ChoosedMaterial->m_Specular[1] = g;
        m_ChoosedMaterial->m_Specular[2] = b;
      break;
      case ID_SPECULAR_POWER:
        m_Property->SetSpecularPower(m_SpecularPower);
        m_ChoosedMaterial->m_SpecularPower = m_SpecularPower;
      break;
      case ID_OPACITY:
        m_Property->SetOpacity(m_Opacity);
        m_ChoosedMaterial->m_Opacity = m_Opacity;
      break;
      case ID_WIRE:
        if (m_Wire)
        { 
          m_Sphere->SetThetaResolution(10);
          m_Sphere->SetPhiResolution(3);
          m_Property->SetRepresentationToWireframe();
        }
        else 
        {
          m_Sphere->SetThetaResolution(20);
          m_Sphere->SetPhiResolution(20);
          m_Property->SetRepresentationToSurface();  
        }
        m_ChoosedMaterial->m_Representation = m_Wire;
      break;
      case ITEM_SELECTED:
        SelectMaterial( e->GetArg() );
      break;
      case ID_ADD:
        AddMaterial();
      break;
      case ID_REMOVE:
        RemoveMaterial();
      break;
			case ID_RESET_MATERIALS:
				CreateDefaultLibrary();
			break;
      case ID_OK:
        assert(m_VmeMaterial);
        m_VmeMaterial->DeepCopy(m_ChoosedMaterial);
        m_VmeMaterial->m_Prop->DeepCopy(m_Property);
        m_Dialog->EndModal(wxOK);
				StoreLibraryToFile();
      break;
      case ID_APPLY:
        assert(m_VmeMaterial);
        m_VmeMaterial->DeepCopy(m_ChoosedMaterial);
        m_VmeMaterial->m_Prop->DeepCopy(m_Property);
				GetLogicManager()->GetViewManager()->PropertyUpdate();
				GetLogicManager()->CameraUpdate();
      break;
      //case wxCANCEL:
      case ID_CANCEL:
        m_Dialog->EndModal(wxCANCEL); 
      break;
      case ID_GUI_UPDATE:
      break;
      default:
        e->Log();
      break; 
    }
    m_RWI->CameraUpdate();	
  }
}
//----------------------------------------------------------------------------
void albaGUIMaterialChooser::LoadLibraryFromFile()
//----------------------------------------------------------------------------
{
	if (wxFileExists(m_Filename.GetCStr()))
	{
		ClearList();

		// XML storage to restore
		albaXMLStorage restore;
		restore.SetURL(m_Filename.GetCStr());
		restore.SetFileType("MAP");
		restore.SetVersion("1.0");

		albaStorableMaterialLibrary *mat_lib = new albaStorableMaterialLibrary(&m_List);
		restore.SetDocument(mat_lib);
		restore.Restore();

		mmaMaterial *mat = NULL;
		for (int m = 0; m < m_List.size(); m++)
		{
			mat = m_List[m];
			this->m_ListCtrlMaterial->AddItem(m,mat->m_MaterialName.GetCStr(),mat->MakeIcon());
		}

		mat_lib->Delete();
	}
}
//----------------------------------------------------------------------------
void albaGUIMaterialChooser::StoreLibraryToFile()
//----------------------------------------------------------------------------
{	
	int i = 0;
	if(m_List.empty()) return;

  // XML storage to restore
  albaXMLStorage restore;
  restore.SetURL(m_Filename.GetCStr());
  restore.SetFileType("MAP");
  restore.SetVersion("1.0");

  albaStorableMaterialLibrary *mat_lib = new albaStorableMaterialLibrary(&m_List);
  restore.SetDocument(mat_lib);
  restore.Store();

  mat_lib->Delete();
}
//----------------------------------------------------------------------------
void albaGUIMaterialChooser::RemoveMaterial()
//----------------------------------------------------------------------------
{
  m_ListCtrlMaterial->DeleteItem(m_ChoosedMaterialId);

	for(int i=0;i<m_List.size();i++)
	{
		if(m_ChoosedMaterial->Equals((albaAttribute *)m_List[i]))
		{
			m_List.erase(m_List.begin()+i);
			break;
		}
	}
}
//----------------------------------------------------------------------------
void albaGUIMaterialChooser::ClearList()
//----------------------------------------------------------------------------
{
  for (int i = 0; i < m_List.size(); i++)
  {
    delete m_List[i];
  }
  m_List.clear();
	if(m_ListCtrlMaterial)
		m_ListCtrlMaterial->Reset();
}
//----------------------------------------------------------------------------
void albaGUIMaterialChooser::SelectMaterial(long matPos)
//----------------------------------------------------------------------------
{
  if(m_ListCtrlMaterial->SelectItem(matPos) || m_ChoosedMaterial == NULL)
  {
		m_ChoosedMaterialId = matPos;
		SetFromMat(m_List[matPos]);
  }

}

//----------------------------------------------------------------------------
void albaGUIMaterialChooser::SetFromMat(mmaMaterial * mat)
{
	if (m_ChoosedMaterial == NULL)
		albaNEW(m_ChoosedMaterial);

	m_ChoosedMaterial->DeepCopy(mat);
	
	//copy chose material on m_Property
	m_Property->DeepCopy(m_ChoosedMaterial->m_Prop);

	//update GUI related vars
	double rgb[3];
	m_Property->GetAmbientColor(rgb);
	m_AmbientColor.Set(255 * rgb[0], 255 * rgb[1], 255 * rgb[2]);
	m_Property->GetDiffuseColor(rgb);
	m_DiffuseColor.Set(255 * rgb[0], 255 * rgb[1], 255 * rgb[2]);
	m_Property->GetSpecularColor(rgb);
	m_SpecularColor.Set(255 * rgb[0], 255 * rgb[1], 255 * rgb[2]);

	m_AmbientIntensity = m_Property->GetAmbient();
	m_DiffuseIntensity = m_Property->GetDiffuse();
	m_SpecularIntensity = m_Property->GetSpecular();
	m_SpecularPower = m_Property->GetSpecularPower();
	m_Opacity = m_Property->GetOpacity();
	m_MaterialName = m_ChoosedMaterial->m_MaterialName;

	m_Wire = m_Property->GetRepresentation() == VTK_WIREFRAME;
	if (m_Wire)
	{
		m_Sphere->SetThetaResolution(10);
		m_Sphere->SetPhiResolution(3);
		m_Property->SetRepresentationToWireframe();
	}
	else
	{
		m_Sphere->SetThetaResolution(20);
		m_Sphere->SetPhiResolution(20);
		m_Property->SetRepresentationToSurface();
	}

	m_Gui->Update();
}

//----------------------------------------------------------------------------
void albaGUIMaterialChooser::AddMaterial()
//----------------------------------------------------------------------------
{
	albaString currentMatName=m_ChoosedMaterial->m_MaterialName;
	
	//If exist a material with the same name I update that material
	for(int i=0;i<m_List.size();i++)
	{
		if(!currentMatName.Compare(m_List[i]->m_MaterialName))
		{
			m_List[i]->DeepCopy(m_ChoosedMaterial);
			m_ChoosedMaterial->UpdateProp();
			m_ListCtrlMaterial->SetItemIcon(i,m_ChoosedMaterial->MakeIcon());
			m_ListCtrlMaterial->SelectItem(i);
			m_ListCtrlMaterial->Update();
			m_Gui->Update();
			return;
		}
	}

  mmaMaterial *mat = mmaMaterial::New();
	mat->DeepCopy(m_ChoosedMaterial);

	m_ChoosedMaterialId = m_List.size()-1;

	// insert mat in the list
  m_List.push_back(mat);

	// insert mat in the tree
	m_ListCtrlMaterial->AddItem(m_ChoosedMaterialId, mat->m_MaterialName.GetCStr(), mat->MakeIcon());
  m_ListCtrlMaterial->SelectItem(m_ChoosedMaterialId);
	SelectMaterial(m_ChoosedMaterialId);
}
//----------------------------------------------------------------------------
void albaGUIMaterialChooser::CreateDefaultLibrary()
//----------------------------------------------------------------------------
{
  ClearList();

	if(m_ListCtrlMaterial)
		m_ListCtrlMaterial->Reset();

  char *mat_lib[] = {
	"0.10  0.25  0.00  0.00  0.75  0.96  0.29  0.29  0.18  1.00  0.50  0.50  80.00  1.00 2 Muscle",
	"0.10  0.25  0.00  0.00  0.91  0.95  0.94  0.81  0.06  1.00  1.00  1.00   1.00  1.00 2 Bone",
	"0.10  0.00  0.00  0.50  0.91  0.50  0.50  0.75  1.00  1.00  1.00  1.00  23.00  1.00 2 Metal_blue",
	"0.10  0.00  0.00  0.50  0.91  0.75  0.75  0.75  1.00  1.00  1.00  1.00  23.00  1.00 2 metal_gray",
	"1.00  0.00  0.00  1.00  1.00  0.00  0.00  1.00  0.00  1.00  1.00  1.00   1.00  1.00 1 Wireframe_flat",
	"0.10  0.00  0.00  1.00  1.00  0.50  0.50  1.00  0.17  1.00  1.00  1.00 100.00  1.00 1 Wireframe_shaded",
	"0.10  0.00  0.00  0.50  0.91  0.50  0.50  0.75  1.00  1.00  1.00  1.00 100.00  0.26 2 Glass_blue",
	"0.10  0.00  0.00  0.50  0.91  1.00  1.00  0.79  1.00  1.00  1.00  1.00 100.00  0.26 2 Glass_yellow",
	"0.10  1.00  1.00  1.00  1.00  0.89  0.65  0.84  0.50  1.00  1.00  1.00 100.00  1.00 2 Plastic_pink",
	"0.10  1.00  1.00  1.00  1.00  0.87  0.33  0.33  0.50  1.00  1.00  1.00 100.00  1.00 2 Plastic_red",
	"0.10  1.00  1.00  1.00  1.00  0.87  0.56  0.34  0.50  1.00  1.00  1.00 100.00  1.00 2 Plastic_orange",
	"0.10  1.00  1.00  1.00  1.00  0.87  0.77  0.34  0.50  1.00  1.00  1.00 100.00  1.00 2 Plastic_yellow",
	"0.10  1.00  1.00  1.00  1.00  0.77  0.87  0.34  0.50  1.00  1.00  1.00 100.00  1.00 2 Plastic_green1",
	"0.10  1.00  1.00  1.00  1.00  0.56  0.87  0.34  0.50  1.00  1.00  1.00 100.00  1.00 2 Plastic_green2",
	"0.10  1.00  1.00  1.00  1.00  0.34  0.87  0.34  0.50  1.00  1.00  1.00 100.00  1.00 2 Plastic_green3",
	"0.10  1.00  1.00  1.00  1.00  0.34  0.87  0.77  0.50  1.00  1.00  1.00 100.00  1.00 2 Plastic_light_blue",
	"0.10  1.00  1.00  1.00  1.00  0.34  0.34  0.88  0.50  1.00  1.00  1.00 100.00  1.00 2 Plastic_blue",
	"0.10  1.00  1.00  1.00  1.00  0.76  0.34  0.88  0.50  1.00  1.00  1.00 100.00  1.00 2 Plastic_purple",
	"0.10  1.00  1.00  1.00  1.00  0.87  0.34  0.56  0.50  1.00  1.00  1.00 100.00  1.00 2 Plastic_fuchsia",
	"0.10  1.00  1.00  1.00  0.86  0.98  0.79  0.62  0.13  1.00  1.00  1.00  20.00  1.00 2 Skin_color",
	};

  int num = sizeof(mat_lib) / sizeof(mat_lib[0]);
  char   name[512];
	float a,ac[3],d,dc[3],s,sc[3],sp,o;	
  int    r;	

	for(int i=0; i<num; i++)
	{
		int res = sscanf(mat_lib[i],"%f %f %f %f %f %f %f %f %f %f %f %f %f %f %d %s",
  		 &a,&ac[0],&ac[1],&ac[2],
			 &d,&dc[0],&dc[1],&dc[2],
			 &s,&sc[0],&sc[1],&sc[2],
			 &sp,&o,&r,&name 			
		); 
    if(res != 16) break;

		//Replace underscore in name string
		int j=0;
		while(name[j]!=0)
		{
			if(name[j]=='_')
				name[j]=' ';
			j++;
		}

    mmaMaterial *mat = mmaMaterial::New();
		mat->m_MaterialName = name; 
    mat->m_AmbientIntensity = a;
    mat->m_Ambient[0] = ac[0];
    mat->m_Ambient[1] = ac[1];
    mat->m_Ambient[2] = ac[2];
    mat->m_DiffuseIntensity = d;
    mat->m_Diffuse[0] = dc[0];
    mat->m_Diffuse[1] = dc[1];
    mat->m_Diffuse[2] = dc[2];
    mat->m_SpecularIntensity = s;
    mat->m_Specular[0] = sc[0];
    mat->m_Specular[1] = sc[1];
    mat->m_Specular[2] = sc[2];
    mat->m_SpecularPower = sp;
    mat->m_Opacity = o;
    mat->m_Representation = r;
    mat->UpdateProp();

		//mat->m_MaterialName.Replace("_"," "); 

		// insert mat in the list
    m_List.push_back(mat);

		// insert mat in the tree
		if(m_ListCtrlMaterial)
			m_ListCtrlMaterial->AddItem(i,mat->m_MaterialName.GetCStr(),mat->MakeIcon());
  }
}
//------------------------------------------------------------------------------
albaCxxTypeMacro(albaStorableMaterialLibrary);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
albaStorableMaterialLibrary::albaStorableMaterialLibrary(std::vector<mmaMaterial *> *mat_list) : m_MaterialList(mat_list)
//------------------------------------------------------------------------------
{
}
//------------------------------------------------------------------------------
// example of de-serialization code
int albaStorableMaterialLibrary::InternalRestore(albaStorageElement *element)
//------------------------------------------------------------------------------
{
  std::vector<albaObject *> attrs;
  element->RestoreObjectVector("MaterialLib", attrs);
  for (unsigned int i = 0; i < attrs.size(); i++)
  {
    mmaMaterial *item = mmaMaterial::SafeDownCast(attrs[i]);
    m_MaterialList->push_back(item);
  }
  return ALBA_OK;
}
//------------------------------------------------------------------------------
int albaStorableMaterialLibrary::InternalStore( albaStorageElement *parent )
//------------------------------------------------------------------------------
{
  std::vector<albaObject *> attrs;
  for (unsigned int m = 0; m < m_MaterialList->size(); m++)
  {
    attrs.push_back((*m_MaterialList)[m]);
  }

  return parent->StoreObjectVector("MaterialLib", attrs);
}
