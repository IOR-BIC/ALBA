/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIMaterialChooser.cpp,v $
  Language:  C++
  Date:      $Date: 2009-04-06 14:11:03 $
  Version:   $Revision: 1.2.2.2 $
  Authors:   Paolo Quadrani
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

#include "mafGUIMaterialChooser.h"

#include <wx/event.h>
#include "wx/busyinfo.h"
#include <wx/settings.h>
#include <wx/laywin.h>
#include <wx/treectrl.h>
#include <wx/bmpbuttn.h>

#include <fstream>

#include "mafDecl.h"
#include "mmaMaterial.h"
#include "mafRWIBase.h"
#include "mafGUIButton.h"
#include "mafGUIMaterialButton.h"
#include "mafGUIListCtrlBitmap.h"
#include "mafGUI.h"
#include "mafRWI.h"
#include "mafGUIPicButton.h"

#include "mafXMLStorage.h"
#include "mafStorageElement.h"
#include "mafVME.h"

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

//----------------------------------------------------------------------------
mafGUIMaterialChooser::mafGUIMaterialChooser(wxString dialog_title)
//----------------------------------------------------------------------------
{  
	m_ChoosedMaterial	= NULL;
  m_VmeMaterial     = NULL;
  m_Vme             = NULL;
  m_Dialog			    = NULL;
	m_Gui					    = NULL;
	m_ListCtrlMaterial= NULL;
	m_RWI					    = NULL;

  m_Filename = mafGetApplicationDirectory().c_str();
  m_Filename += "/mat_library.xml";
  
  //initialize first material 
  m_MaterialName      = "new material";
	m_AmbientColor      = wxColour(0,0,0);
	m_DiffuseColor      = mafRandomColor();
	m_SpecularColor     = wxColour(255,255,255); 
  m_AmbientIntensity  = 0.1;
  m_DiffuseIntensity  = 1;
	m_SpecularIntensity = 0.5;
	m_SpecularPower     = 70; 
  m_Opacity           = 1;
  m_Wire              = 0;
	
	CreateGUI();
	CreatePipe();
}
//----------------------------------------------------------------------------
mafGUIMaterialChooser::~mafGUIMaterialChooser()
//----------------------------------------------------------------------------
{
  mafDEL(m_ChoosedMaterial); //BES: 31.3.2009 - fixed memory leak

  ClearList();
  m_RWI->m_RenFront->RemoveActor(m_Actor);

	vtkDEL(m_Sphere);
	vtkDEL(m_Property);
  vtkDEL(m_Mapper);
  vtkDEL(m_Actor);
	cppDEL(m_RWI); 
  cppDEL(m_Dialog);
};
//----------------------------------------------------------------------------
bool mafGUIMaterialChooser::ShowChooserDialog(mafVME *vme, bool remember_last_material)
//----------------------------------------------------------------------------
{
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
    mafDEL(m_ChoosedMaterial);// = NULL;
  }
  SelectMaterial(m_VmeMaterial);

  bool res = m_Dialog->ShowModal() != 0;
  m_VmeMaterial	= NULL;
  return res;
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
	ID_LOAD,
	ID_SAVE,
	ID_ADD,
	ID_REMOVE,
	ID_BITMAP,
  ID_OK,
  ID_CANCEL,
  ID_APPLY
};
//----------------------------------------------------------------------------
void mafGUIMaterialChooser::CreateGUI() 
//----------------------------------------------------------------------------
{
	wxString wildcard = "xml file (*.xml)|*.xml|all files (*.*)|*.*";

  int x_pos,y_pos,w,h;
  mafGetFrame()->GetPosition(&x_pos,&y_pos);
  m_Dialog = new wxDialog(mafGetFrame(),-1,"Choose Material",
	wxDefaultPosition,wxDefaultSize,wxDIALOG_MODAL | wxCAPTION | wxTHICK_FRAME );
  m_Dialog->GetSize(&w,&h);
  m_Dialog->SetSize(x_pos+5,y_pos+5,w,h);
  //m_Dialog->SetMinSize(wxSize(w,h));

	m_ListCtrlMaterial = new mafGUIListCtrlBitmap(m_Dialog,-1,0,1); 
	m_ListCtrlMaterial->SetTitle("material library"); 
	m_ListCtrlMaterial->SetListener(this); 
	m_ListCtrlMaterial->Reset();
	//m_ListCtrlMaterial->SetSize(wxSize(200,500));
  m_ListCtrlMaterial->SetMinSize(wxSize(200,500));

	// RWI ============================
	m_RWI = new mafRWI(m_Dialog,ONE_LAYER);
	m_RWI->SetSize(0,0,50,50);
  m_RWI->m_RwiBase->Show(true);
	// GUI ============================
	m_Gui = new mafGUI(this);
  
  //m_Gui->Label		(""); 
	m_Gui->Label("current material",1);
	m_Gui->String(ID_NAME,"name",&m_MaterialName);

	m_Gui->Label		("ambient",1);
	m_Gui->FloatSlider(ID_AMBIENT_INTENSITY,"intensity",&m_AmbientIntensity,0.0,1.0);
	m_Gui->Color(ID_AMBIENT_COLOR,"color",&m_AmbientColor);

	m_Gui->Label		("diffuse",1);
	m_Gui->FloatSlider(ID_DIFFUSE_INTENSITY,"intensity",&m_DiffuseIntensity,0.0,1.0);
	m_Gui->Color(ID_DIFFUSE_COLOR,"color",&m_DiffuseColor);

	m_Gui->Label		("specular",1);
	m_Gui->FloatSlider(ID_SPECULAR_INTENSITY,"intensity",&m_SpecularIntensity,0.0,1.0);
	m_Gui->Color(ID_SPECULAR_COLOR,"color",&m_SpecularColor);
	m_Gui->Slider(ID_SPECULAR_POWER,"power",&m_SpecularPower,1,100);

	m_Gui->Label		("");
	m_Gui->FloatSlider(ID_OPACITY,"opacity",&m_Opacity,0.0,1.0);
	m_Gui->Bool(ID_WIRE,"wireframe",&m_Wire,0);

  m_Gui->Label		("");
	m_Gui->Button(ID_ADD,"store current material in library");
	m_Gui->Button(ID_REMOVE,"remove selected material from library");
  m_Gui->Enable(ID_REMOVE, false);
	m_Gui->Label		("");
	m_Gui->FileOpen(ID_LOAD,"load library",&m_Filename,wildcard);
	m_Gui->FileSave(ID_SAVE,"save library",&m_Filename,wildcard);
	m_Gui->Label		("");
  //m_Gui->OkCancel();
  int bh = m_Gui->GetMetrics(GUI_BUTTON_HEIGHT);
  int fw = m_Gui->GetMetrics(GUI_FULL_WIDTH);
  m_OkButton  = new mafGUIButton (m_Gui,ID_OK, "Ok", wxDefaultPosition,wxSize(fw/3, bh));
  m_CancelButton = new mafGUIButton (m_Gui,ID_CANCEL,"Cancel",wxDefaultPosition,wxSize(fw/3, bh));
  m_ApplyButton = new mafGUIButton (m_Gui,ID_APPLY,"Apply",wxDefaultPosition,wxSize(fw/3, bh));
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
void mafGUIMaterialChooser::CreatePipe()
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
	m_Mapper->SetInput(m_Sphere->GetOutput());
	m_Mapper->SetImmediateModeRendering(0);
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
void mafGUIMaterialChooser::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
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
        SelectMaterial( (mmaMaterial *)( e->GetArg() ) );
      break;
      case ID_ADD:
        AddMaterial();
      break;
      case ID_REMOVE:
        RemoveMaterial();
      break;
      case ID_SAVE:
        StoreLibraryToFile();				
      break;
      case ID_LOAD:
        LoadLibraryFromFile();
      break;
      //case wxOK:
      case ID_OK:
        assert(m_VmeMaterial);
        m_VmeMaterial->DeepCopy(m_ChoosedMaterial);
        m_VmeMaterial->m_Prop->DeepCopy(m_Property);
        m_Dialog->EndModal(wxOK); 
      break;
      case ID_APPLY:
        assert(m_VmeMaterial);
        m_VmeMaterial->DeepCopy(m_ChoosedMaterial);
        m_VmeMaterial->m_Prop->DeepCopy(m_Property);
        m_Vme->ForwardUpEvent(&mafEvent(m_Vme,CAMERA_UPDATE));
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
void mafGUIMaterialChooser::LoadMaterials_old()
//----------------------------------------------------------------------------
{
	int i;
	std::ifstream f_in;
	f_in.open(m_Filename.GetCStr());

  // check if is a valid file 
	char inputline[21];
  for(i = 0; i < 20; i++) 
    f_in >> inputline[i];
	inputline[21] = '\0';
	std::string str(inputline);
	if (str.find("MATERIALSPROPERTIES:") != 0)
	{
		wxString msg = m_Filename.GetCStr();
		msg += "\n";
		msg += "is not a Material Library file";
 	::wxMessageBox(msg,"Error",wxICON_ERROR,mafGetFrame() );
		f_in.close();
		return;							
	}

  // file is ok
	ClearList();

	char line[128];
	//skipping the first line
	f_in.getline(line,128);

	char check_end[3];
	for(i=0;i<3;i++) f_in >> check_end[i];

	while(check_end[0] != 'E')
	{
		f_in.getline(line,128);

		mmaMaterial *mat = mmaMaterial::New();

		//name
		f_in.getline(line,128);
		char *material = line;
		mat->m_MaterialName = material;

		//Ambient Color:	
		f_in.getline(line,128);
		double r_amb,g_amb,b_amb,amb_intensity;
		f_in >> r_amb >> g_amb >> b_amb;
		mat->m_Prop->SetAmbientColor(r_amb,g_amb,b_amb); 
		f_in.getline(line,128);
		f_in >> amb_intensity;	
		mat->m_Prop->SetAmbient(amb_intensity);
		f_in.getline(line,128);

		//Diffuse Color:
		f_in.getline(line,128);
		double r_diff,g_diff,b_diff,diff_intensity;
		f_in >> r_diff >> g_diff >> b_diff;
		mat->m_Prop->SetDiffuseColor(r_diff,g_diff,b_diff); 
		f_in.getline(line,128);
		f_in >> diff_intensity;	
		mat->m_Prop->SetDiffuse(diff_intensity);
		f_in.getline(line,128);

		//Specular Color:
		f_in.getline(line,128);
		double r_spec,g_spec,b_spec,spec_intensity;
		int spec_power;
		f_in >> r_spec >> g_spec >> b_spec;
		mat->m_Prop->SetSpecularColor(r_spec,g_spec,b_spec); 
		f_in.getline(line,128);
		f_in >> spec_intensity;	
		mat->m_Prop->SetSpecular(spec_intensity);
		f_in.getline(line,128);
		f_in >> spec_power;	
		mat->m_Prop->SetSpecularPower(spec_power);
		f_in.getline(line,128);

		//Opacity:
		f_in.getline(line,128);
		double opacity;
		f_in >> opacity;
		mat->m_Prop->SetOpacity(opacity);
		f_in.getline(line,128);

		// insert mat in the list
    m_List.push_back(mat);

		// insert mat in the tree
		this->m_ListCtrlMaterial->AddItem((long)mat,mat->m_MaterialName.GetCStr(),&wxBitmap(50,50));

		//blank line
		f_in.getline(line,128);

		//check_end
		for(i=0;i<3;i++) f_in >> check_end[i]; 					
	
   }//end while

	f_in.close();
}
//----------------------------------------------------------------------------
void mafGUIMaterialChooser::SaveMaterials_old()
//----------------------------------------------------------------------------
{	
	int i = 0;
	if(!m_List.empty()) return;

	std::ofstream f_out;
	f_out.open(m_Filename.GetCStr());
	f_out<<"MATERIALS PROPERTIES:"<<std::endl;

	for( mmaMaterial *n = m_List[i]; i < m_List.size();)
	{			
		f_out<<"material n. "<<++i<<std::endl;

		f_out<< n->m_MaterialName.GetCStr() <<std::endl;

		f_out<<"Ambient Color:"<<std::endl;
		double data_ambient[3];
		n->m_Prop->GetAmbientColor(data_ambient);
		f_out<<data_ambient[0]<<" "<<data_ambient[1]<<" "<<data_ambient[2]<<std::endl;
		f_out<<n->m_Prop->GetAmbient()<<std::endl;

		f_out<<"Diffuse Color:"<<std::endl; 
		double data_diffuse[3];
		n->m_Prop->GetDiffuseColor(data_diffuse);
		f_out<<data_diffuse[0]<<" "<<data_diffuse[1]<<" "<<data_diffuse[2]<<std::endl;
		f_out<<n->m_Prop->GetDiffuse()<<std::endl;

		f_out<<"Specular Color:"<<std::endl;
		double data_specular[3];
		n->m_Prop->GetSpecularColor(data_specular);
		f_out<<data_specular[0]<<" "<<data_specular[1]<<" "<<data_specular[2]<<std::endl;
		f_out<<n->m_Prop->GetSpecular()<<std::endl;
		f_out<<n->m_Prop->GetSpecularPower()<<std::endl;

		f_out<<"Opacity:"<<std::endl;
		f_out<<n->m_Prop->GetOpacity()<<std::endl;
		
		f_out <<std::endl;
	}

	f_out << "END";
	f_out.close();
}
//----------------------------------------------------------------------------
void mafGUIMaterialChooser::LoadLibraryFromFile()
//----------------------------------------------------------------------------
{
  ClearList();

  // XML storage to restore
  mafXMLStorage restore;
  restore.SetURL(m_Filename.GetCStr());
  restore.SetFileType("MAP");
  restore.SetVersion("1.0");

  mafStorableMaterialLibrary *mat_lib = new mafStorableMaterialLibrary(&m_List);
  restore.SetDocument(mat_lib);
  restore.Restore();

  mmaMaterial *mat = NULL;
  for (int m = 0; m < m_List.size(); m++)
  {
    mat = m_List[m];
    this->m_ListCtrlMaterial->AddItem((long)mat,mat->m_MaterialName.GetCStr(),mat->MakeIcon());
  }

  mat_lib->Delete();
}
//----------------------------------------------------------------------------
void mafGUIMaterialChooser::StoreLibraryToFile()
//----------------------------------------------------------------------------
{	
	int i = 0;
	if(m_List.empty()) return;

  // XML storage to restore
  mafXMLStorage restore;
  restore.SetURL(m_Filename.GetCStr());
  restore.SetFileType("MAP");
  restore.SetVersion("1.0");

  mafStorableMaterialLibrary *mat_lib = new mafStorableMaterialLibrary(&m_List);
  restore.SetDocument(mat_lib);
  restore.Store();

  mat_lib->Delete();
}
//----------------------------------------------------------------------------
void mafGUIMaterialChooser::RemoveMaterial()
//----------------------------------------------------------------------------
{
  m_ListCtrlMaterial->DeleteItem((long)m_ChoosedMaterial);

	//remove m_ChoosedMaterial from the list
/*  bool found = false;
  mmaMaterial *new_selection = NULL;
	if(m_List == m_ChoosedMaterial)
	{
//    m_List = m_List->m_next;
		found = true;
		new_selection = m_List; //may be null
	}
	else
	{
  	mmaMaterial *m = m_List;
		while(m)
		{
//      if(m->m_next == m_ChoosedMaterial) 
			{
		    new_selection = m;
//			  m->m_next = m_ChoosedMaterial->m_next;
				found = true;
				break;  
			}
//      m = m->m_next; 
		}
	}

  assert(found);
  cppDEL(m_ChoosedMaterial);

	if(new_selection) 
	  SelectMaterial(new_selection);
*/
}
//----------------------------------------------------------------------------
void mafGUIMaterialChooser::ClearList()
//----------------------------------------------------------------------------
{
  for (int i = 0; i < m_List.size(); i++)
  {
    delete m_List[i];
  }
  m_List.clear();
	this->m_ListCtrlMaterial->Reset();
}
//----------------------------------------------------------------------------
void mafGUIMaterialChooser::SelectMaterial(mmaMaterial *m)
//----------------------------------------------------------------------------
{
  if(m_ListCtrlMaterial->SelectItem((long)m) || m_ChoosedMaterial == NULL)
  {
    if(m_ChoosedMaterial == NULL)
      mafNEW(m_ChoosedMaterial);
    m_ChoosedMaterial->DeepCopy(m);

    //copy chose material on m_Property
    m_Property->DeepCopy(m_ChoosedMaterial->m_Prop);

    //update GUI related vars
    double rgb[3];
    m_Property->GetAmbientColor(rgb);
    m_AmbientColor.Set(255*rgb[0],255*rgb[1],255*rgb[2]);		
    m_Property->GetDiffuseColor(rgb);
    m_DiffuseColor.Set(255*rgb[0],255*rgb[1],255*rgb[2]);		
    m_Property->GetSpecularColor(rgb);
    m_SpecularColor.Set(255*rgb[0],255*rgb[1],255*rgb[2]);		

    m_AmbientIntensity  = m_Property->GetAmbient();
    m_DiffuseIntensity  = m_Property->GetDiffuse();  
    m_SpecularIntensity = m_Property->GetSpecular(); 
    m_SpecularPower     = m_Property->GetSpecularPower();  
    m_Opacity					  = m_Property->GetOpacity(); 
    m_MaterialName      = m_ChoosedMaterial->m_MaterialName;

    m_Wire = m_Property->GetRepresentation() == 1;
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

    //enable REMOVE if m_ChoosedMaterial != NULL 
    m_Gui->Enable(ID_REMOVE, m_ChoosedMaterial != NULL);
    m_Gui->Update();
  }

//  m_ListCtrlMaterial->SelectItem((long)m_ChoosedMaterial);
}
//----------------------------------------------------------------------------
void mafGUIMaterialChooser::AddMaterial()
//----------------------------------------------------------------------------
{
  mmaMaterial *mat = mmaMaterial::New();
  mat->DeepCopy(m_ChoosedMaterial);

	// insert mat in the list
  m_List.push_back(mat);

	// insert mat in the tree
	m_ListCtrlMaterial->AddItem((long)mat, mat->m_MaterialName.GetCStr(), mat->MakeIcon());	
  m_ListCtrlMaterial->SelectItem((long)mat);
	SelectMaterial(mat);
}
//----------------------------------------------------------------------------
void mafGUIMaterialChooser::LoadLibraryFromVme(mafVME *vme)
//----------------------------------------------------------------------------
{
  assert(vme);
	
//	if(vme->GetTagArray()->FindTag("material1") == -1)
//	{
		CreateDefaultLibrary();
//  }  
/*	else
	{
    ClearList();
		
		int counter = 1;
    char tagname[100];
    while(1)
		{
			sprintf(tagname,"material%d",counter);
			if(vme->GetTagArray()->FindTag(tagname) == -1) break;

			mmaMaterial *n = mmaMaterial::New();
      n->LoadFromTag( vme->GetTagArray()->GetTag(tagname) );

			// insert mat in the list
			mmaMaterial *last;
			if(!m_List)
				m_List = n;
			else
				last->m_next = n;
			last = n;

			// insert mat in the tree
			this->m_ListCtrlMaterial->AddItem((long)n,n->m_MaterialName,n->MakeIcon());	

      counter++;
		}	
	} */
}
//----------------------------------------------------------------------------
void mafGUIMaterialChooser::StoreLibraryToVme(mafVME *vme)
//----------------------------------------------------------------------------
{
	if(vme == NULL) 
    return;
	if(!m_List.empty()) 
    return;
/*
	int counter =1;
  char tagname[100];

	for( mmaMaterial *n=m_List; n; n=n->m_next)
	{			
    vtkTagItem ti;
		sprintf(tagname,"material%d",counter++);
		ti.SetName(tagname);
		ti.SetValue(1.0);
		ti.SetNumberOfComponents(20);
  
		n->StoreToTag(&ti);
  
		vme->GetTagArray()->AddTag(ti);	
	} 
	//a previous Storing Operation may have created 
	//more material tags - these must be removed
	while(1)
	{
		sprintf(tagname,"material%d",counter++);
	  if(vme->GetTagArray()->FindTag(tagname) == -1) break; 
    vme->GetTagArray()->DeleteTag(tagname); 	  
	} */
}
//----------------------------------------------------------------------------
void mafGUIMaterialChooser::CreateDefaultLibrary()
//----------------------------------------------------------------------------
{
  ClearList();

  char *mat_lib[] = {
	"0.10  0.25  0.00  0.00  0.75  0.96  0.29  0.29  0.18  1.00  0.50  0.50  80.00  1.00 2 muscle",
	"0.10  0.25  0.00  0.00  0.91  0.95  0.94  0.81  0.06  1.00  1.00  1.00   1.00  1.00 2 bone",
	"0.10  0.00  0.00  0.50  0.91  0.50  0.50  0.75  1.00  1.00  1.00  1.00  23.00  1.00 2 metal_blue",
	"0.10  0.00  0.00  0.50  0.91  0.75  0.75  0.75  1.00  1.00  1.00  1.00  23.00  1.00 2 metal_gray",
	"1.00  0.00  0.00  1.00  1.00  0.00  0.00  1.00  0.00  1.00  1.00  1.00   1.00  1.00 1 wireframe_flat",
	"0.10  0.00  0.00  1.00  1.00  0.50  0.50  1.00  0.17  1.00  1.00  1.00 100.00  1.00 1 wireframe_shaded",
	"0.10  0.00  0.00  0.50  0.91  0.50  0.50  0.75  1.00  1.00  1.00  1.00 100.00  0.26 2 glass_blue",
	"0.10  0.00  0.00  0.50  0.91  1.00  1.00  0.79  1.00  1.00  1.00  1.00 100.00  0.26 2 glass_yellow",
	"0.10  1.00  1.00  1.00  1.00  0.89  0.65  0.84  0.50  1.00  1.00  1.00 100.00  1.00 2 plastic_pink",
	"0.10  1.00  1.00  1.00  1.00  0.87  0.33  0.33  0.50  1.00  1.00  1.00 100.00  1.00 2 plastic_red",
	"0.10  1.00  1.00  1.00  1.00  0.87  0.56  0.34  0.50  1.00  1.00  1.00 100.00  1.00 2 plastic_orange",
	"0.10  1.00  1.00  1.00  1.00  0.87  0.77  0.34  0.50  1.00  1.00  1.00 100.00  1.00 2 plastic_yellow",
	"0.10  1.00  1.00  1.00  1.00  0.77  0.87  0.34  0.50  1.00  1.00  1.00 100.00  1.00 2 plastic_green1",
	"0.10  1.00  1.00  1.00  1.00  0.56  0.87  0.34  0.50  1.00  1.00  1.00 100.00  1.00 2 plastic_green2",
	"0.10  1.00  1.00  1.00  1.00  0.34  0.87  0.34  0.50  1.00  1.00  1.00 100.00  1.00 2 plastic_green3",
	"0.10  1.00  1.00  1.00  1.00  0.34  0.87  0.77  0.50  1.00  1.00  1.00 100.00  1.00 2 plastic_light_blue",
	"0.10  1.00  1.00  1.00  1.00  0.34  0.34  0.88  0.50  1.00  1.00  1.00 100.00  1.00 2 plastic_blue",
	"0.10  1.00  1.00  1.00  1.00  0.76  0.34  0.88  0.50  1.00  1.00  1.00 100.00  1.00 2 plastic_purple",
	"0.10  1.00  1.00  1.00  1.00  0.87  0.34  0.56  0.50  1.00  1.00  1.00 100.00  1.00 2 plastic_fuxia"
	"0.10  0.25  0.00  0.00  0.75  0.96  0.29  0.29  0.18  1.00  0.50  0.50  80.00  1.00 2 muscle_colour",
  "0.10  1.00  1.00  1.00  0.86  0.98  0.79  0.62  0.13  1.00  1.00  1.00  20.00  1.00 2 skin_colour"
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
		m_ListCtrlMaterial->AddItem((long)mat,mat->m_MaterialName.GetCStr(),mat->MakeIcon());
  }
}
//------------------------------------------------------------------------------
mafCxxTypeMacro(mafStorableMaterialLibrary);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafStorableMaterialLibrary::mafStorableMaterialLibrary(std::vector<mmaMaterial *> *mat_list) : m_MaterialList(mat_list)
//------------------------------------------------------------------------------
{
}
//------------------------------------------------------------------------------
// example of de-serialization code
int mafStorableMaterialLibrary::InternalRestore(mafStorageElement *element)
//------------------------------------------------------------------------------
{
  std::vector<mafObject *> attrs;
  element->RestoreObjectVector("MaterialLib", attrs);
  for (unsigned int i = 0; i < attrs.size(); i++)
  {
    mmaMaterial *item = mmaMaterial::SafeDownCast(attrs[i]);
    m_MaterialList->push_back(item);
  }
  return MAF_OK;
}
//------------------------------------------------------------------------------
int mafStorableMaterialLibrary::InternalStore( mafStorageElement *parent )
//------------------------------------------------------------------------------
{
  std::vector<mafObject *> attrs;
  for (unsigned int m = 0; m < m_MaterialList->size(); m++)
  {
    attrs.push_back((*m_MaterialList)[m]);
  }

  return parent->StoreObjectVector("MaterialLib", attrs);
}
