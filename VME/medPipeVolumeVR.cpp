/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medPipeVolumeVR.cpp,v $
  Language:  C++
  Date:      $Date: 2007-01-19 15:24:10 $
  Version:   $Revision: 1.2 $
  Authors:   Daniele Giunchi
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

#include "medPipeVolumeVR.h"

#include "mafDecl.h"
#include "mmgGui.h"
#include "mmgLutPreset.h"

#include "vtkMAFAssembly.h"
#include "mmaVolumeMaterial.h"
#include "mafVME.h"
#include "mafVMEVolume.h"

//#include "vtkOutlineCornerFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkProperty.h"

#include "vtkPiecewiseFunction.h"
#include "vtkColorTransferFunction.h"
#include "vtkVolumeProperty.h"
#include "vtkVolumeTextureMapper2D.h"
#include "vtkVolume.h"
#include "vtkLODProp3D.h"
#include "vtkImageShiftScale.h"
#include "vtkProbeFilter.h" 
#include "vtkRectilinearGrid.h" 
#include "vtkStructuredPoints.h"
#include "vtkLookupTable.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(medPipeVolumeVR);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medPipeVolumeVR::medPipeVolumeVR()
:mafPipe()
//----------------------------------------------------------------------------
{
  v_pf			= NULL;
	v_ctf			= NULL;
	
	v_vpHi		= NULL;
	v_vpLo		= NULL; 
	v_vtmHi		= NULL;
	v_vtmLo		= NULL;  

	v_lod			= NULL;
	v_imageshift	= NULL;
	v_gf			= NULL;
	m_range[0]		= 0;
	m_range[1]		= 0;
	m_UnsignRange[0]= 0;
	m_UnsignRange[1]= 0;
	m_Rescale		= false;
	m_isStructured = true;
	m_act_a = NULL;

	
	v_structuredImage = NULL;
  v_probe = NULL;
}
//----------------------------------------------------------------------------
void medPipeVolumeVR::Create(mafSceneNode *n)
//----------------------------------------------------------------------------
{
  Superclass::Create(n);

  m_NumberOfSlices = 64; //<< was parameter

	if(m_NumberOfSlices != 64) m_SliceSet = true;
	else m_SliceSet = false;


	//assert(mafGetBaseType(m_Vme) == VME_GRAY_VOLUME);

	m_created = true;
  
	m_Vme->Update();
	mafString vmeControl = m_Vme->GetOutput()->GetVTKData()->GetClassName();
	if(vmeControl == L"vtkRectilinearGrid")
	{
		//sorry
		/*m_sel_a = NULL;
		m_act_a = NULL;
		m_created = false;
		return;*/

		m_isStructured = false;

		int dim[3];
		double bounds[6];

		((vtkRectilinearGrid *)m_Vme->GetOutput()->GetVTKData())->GetDimensions(dim);

		//Get the bounds for m_Vme as (Xmin,Xmax,Ymin,Ymax,Zmin,Zmax)
		m_Vme->GetOutput()->GetVTKData()->GetBounds(bounds);

		v_structuredImage = vtkStructuredPoints::New();

		v_probe = vtkProbeFilter::New();
		
		//m_NumberOfSlices is 128 as default


		double spaceInt_x = (bounds[1]-bounds[0])/(dim[0]-1);
		double spaceInt_y = (bounds[3]-bounds[2])/(dim[1]-1);;
		double spaceInt_z = (bounds[5]-bounds[4])/((double)m_NumberOfSlices);


		v_structuredImage->SetOrigin(bounds[0],bounds[2],bounds[4]);

		int dim_z = (bounds[5]-bounds[4])/spaceInt_z + 1;

		v_structuredImage->SetDimensions(dim[0],dim[1],dim_z);

		v_structuredImage->SetSpacing(spaceInt_x,spaceInt_y,spaceInt_z);

		v_structuredImage->Update();

		v_probe->SetInput(v_structuredImage);
		v_probe->SetSource(m_Vme->GetOutput()->GetVTKData());

		v_probe->Update();


	}//end if(m_Vme->GetCurrentData()->IsA(L"vtkRectilinearGrid")) 

	

  //-------------------- selection hilight --------------------
/*	vtkOutlineCornerFilter *sel_ocf = vtkOutlineCornerFilter::New();
	sel_ocf->SetInput(m_Vme->GetOutput()->GetVTKData());  

	vtkPolyDataMapper *sel_m = vtkPolyDataMapper::New();
	sel_m->SetInput(sel_ocf->GetOutput());

	vtkProperty *sel_p = vtkProperty::New();
	sel_p->SetColor(1,1,1);
	sel_p->SetAmbient(1);
	sel_p->SetRepresentationToWireframe();
	sel_p->SetInterpolationToFlat();

	m_sel_a = vtkActor::New();
	m_sel_a->SetMapper(sel_m);
	m_sel_a->VisibilityOff();
	m_sel_a->PickableOff();
	m_sel_a->SetProperty(sel_p);
	m_sel_a->SetScale(1.01,1.01,1.01);

	m_AssemblyFront->AddPart(m_sel_a);
  
	sel_ocf->Delete();
	sel_m->Delete();
	sel_p->Delete();
*/
  //-------------------- volume pipeline --------------------
/*
	vtkPiecewiseFunction  *pf = vtkPiecewiseFunction::New();  
	pf->AddPoint(20,0);
	pf->AddPoint(255,0.2);

	vtkColorTransferFunction *ctf = vtkColorTransferFunction::New();  
	ctf->AddRGBPoint(0,  0,0,0);
	ctf->AddRGBPoint(64, 1,0,0);
	ctf->AddRGBPoint(128,0,0,1);
	ctf->AddRGBPoint(192,0,1,0);
	ctf->AddRGBPoint(255,0,0.2,0);

	vtkVolumeProperty *vp = vtkVolumeProperty::New();  
	vp->SetColor(ctf);
	vp->SetScalarOpacity(pf);

	vtkVolumeTextureMapper2D *vtm = vtkVolumeTextureMapper2D::New();  
	vtm->SetInput((vtkImageData *)m_Vme->GetCurrentData());
	//vtm->SetMaximumNumberOfPlanes();

	vtkVolumeTextureMapper2D *vtm2 = vtkVolumeTextureMapper2D::New();  
	vtm2->SetInput((vtkImageData *)m_Vme->GetCurrentData());
	vtm2->SetMaximumNumberOfPlanes(30);


	vtkLODProp3D *lod = vtkLODProp3D ::New();
	//lod->AddLOD(vtm,  vp,1);
	//lod->AddLOD(vtm2, vp,1000);
	lod->AddLOD(vtm,  vp,0);
	lod->AddLOD(vtm2, vp,0);

	m_act_a = (vtkActor*)lod;
	m_asm1->AddPart(lod);
	m_asm1->PickableOff();
	lod->PickableOff();

	pf->Delete();
	ctf->Delete();
	vp->Delete();
	vtm->Delete();
	vtm2->Delete();
*/

	v_pf	= vtkPiecewiseFunction::New();
	v_ctf = vtkColorTransferFunction::New();
	v_gf  = vtkPiecewiseFunction::New();

	v_vpHi = vtkVolumeProperty::New(); 
		v_vpHi->SetColor(v_ctf);
		v_vpHi->SetScalarOpacity(v_pf);
		//v_vpHi->SetGradientOpacity(v_gf);
		v_vpHi->DisableGradientOpacityOn();
		v_vpHi->SetInterpolationTypeToLinear();
		v_vpHi->ShadeOn();
		//v_vpHi->SetAmbient(0.1);
		//v_vpHi->SetDiffuse(0.9);
		//v_vpHi->SetSpecular(0.2);
		//v_vpHi->SetSpecularPower(100.0);
		v_vpHi->Modified();

	v_vpLo = vtkVolumeProperty::New(); 
		v_vpLo->SetColor(v_ctf);
		v_vpLo->SetScalarOpacity(v_pf);
		//v_vpLo->SetGradientOpacity(v_gf);
		v_vpLo->DisableGradientOpacityOn();
		v_vpLo->ShadeOff();
	
	v_lodP = vtkProperty::New();
		v_lodP->SetAmbient(0.1);
		v_lodP->SetDiffuse(0.9);
		v_lodP->SetSpecular(0.2);
		v_lodP->SetSpecularPower(10.0);

	v_vtmHi = vtkVolumeTextureMapper2D::New();
	v_vtmLo = vtkVolumeTextureMapper2D::New(); 
	
	m_Vme->GetOutput()->GetVTKData()->GetScalarRange(m_range);

	m_Minimum = m_range[0];
	m_Maximum = m_range[1];

	if(m_Minimum < 0.0)
	{
		m_Rescale = true;

		v_imageshift = vtkImageShiftScale::New();

		if (!m_isStructured) 
			{

				v_imageshift->SetInput((vtkImageData *)v_probe->GetOutput());

			}

		else 
			{
				v_imageshift->SetInput((vtkImageData *)m_Vme->GetOutput()->GetVTKData());

			}

		v_imageshift->SetShift(- m_Minimum);

		v_imageshift->SetOutputScalarTypeToUnsignedShort();

		v_imageshift->Update();
				
		v_vtmHi->SetInput((vtkImageData *)v_imageshift->GetOutput());
		v_vtmLo->SetInput((vtkImageData *)v_imageshift->GetOutput());

		((vtkImageData *)v_imageshift->GetOutput())->GetScalarRange(m_UnsignRange);


		// Originale
		//v_pf->AddPoint(m_UnsignRange[0],0);
 		//v_pf->AddPoint(m_UnsignRange[1],1.0);

		//v_gf->AddPoint(m_UnsignRange[0],0.4);
		//v_gf->AddPoint(m_UnsignRange[1],0.8);

		//v_ctf->AddRGBPoint(m_UnsignRange[0], 0,0,0);
		//v_ctf->AddRGBPoint(m_UnsignRange[1],0,0.2,0);

		/// vecchio punto
		//v_pf->AddPoint(0,0);
		//v_pf->AddPoint(10501,0);
		//v_pf->AddPoint(21000,1);
		//v_pf->AddPoint(32767,1);

		//v_ctf->AddRGBPoint(0,1.0,0.55,0.021);
		//v_ctf->AddRGBPoint(8121,1.0,0.42,0.2);
		//v_ctf->AddRGBPoint(16909,0.51,0.51,0.51);
		//v_ctf->AddRGBPoint(32767,1,1,1);

		/// Nuovo punto
		v_pf->AddPoint(    0,0.0);
		v_pf->AddPoint(22737,0.0);
		v_pf->AddPoint(44327,1.0);
		v_pf->AddPoint(65535,1.0);

		v_ctf->AddRGBPoint(    0, 0.00, 0.00, 0.00);
		v_ctf->AddRGBPoint(11655, 0.74, 0.19, 0.14);
		v_ctf->AddRGBPoint(31908, 0.96, 0.64, 0.42);
		v_ctf->AddRGBPoint(33818, 0.76, 0.78, 0.25);
		v_ctf->AddRGBPoint(41843, 1.00, 1.00, 1.00);
		v_ctf->AddRGBPoint(65535, 1.00, 1.00, 1.00);

		//v_gf->AddPoint(0,0);
		//v_gf->AddPoint(65535,1);

	}
	else 
	{
		if (!m_isStructured)
			{
				v_vtmHi->SetInput((vtkImageData *)v_probe->GetOutput());
				v_vtmLo->SetInput((vtkImageData *)v_probe->GetOutput());

			}

		else 
			{
				v_vtmHi->SetInput((vtkImageData *)m_Vme->GetOutput()->GetVTKData());
				v_vtmLo->SetInput((vtkImageData *)m_Vme->GetOutput()->GetVTKData());

			}

		
			/// Nuovo punto
			v_pf->AddPoint(    0,0.0);
			v_pf->AddPoint(22737,0.0);
			v_pf->AddPoint(44327,1.0);
			v_pf->AddPoint(65535,1.0);

			v_ctf->AddRGBPoint(    0, 0.00, 0.00, 0.00);
			v_ctf->AddRGBPoint(11655, 0.74, 0.19, 0.14);
			v_ctf->AddRGBPoint(31908, 0.96, 0.64, 0.42);
			v_ctf->AddRGBPoint(33818, 0.76, 0.78, 0.25);
			v_ctf->AddRGBPoint(41843, 1.00, 1.00, 1.00);
			v_ctf->AddRGBPoint(65535, 1.00, 1.00, 1.00);

			//v_gf->AddPoint(0,0);
			//v_gf->AddPoint(65535,1);


		/*v_pf->AddPoint(m_range[0],0);
		v_pf->AddPoint(m_range[1],1.0);

		v_gf->AddPoint(m_range[0],0.4);
		v_gf->AddPoint(m_range[1],0.8);

		v_ctf->AddRGBPoint(m_range[0], 0,0,0);

		v_ctf->AddRGBPoint(m_range[1],0,0.2,0);*/
    //v_pf->AddPoint(m_UnsignRange[0],0);
    //v_pf->AddPoint(0,0);
    //v_pf->AddPoint(20000,0);
    //v_pf->AddPoint(24000,1);
    //v_pf->AddPoint(32767,1);
    
		//v_pf->AddPoint(m_UnsignRange[1],1.0);
    //v_gf->AddPoint(0,1);
		//v_gf->AddPoint(m_UnsignRange[0],0.4);
		//v_gf->AddPoint(m_UnsignRange[1],0.8);

		//v_ctf->AddRGBPoint(m_UnsignRange[0], 0,0,0);
    //v_ctf->AddRGBPoint(0,1.0,0.55,0.021);
    //v_ctf->AddRGBPoint(16909,0.51,0.51,0.51);
    //v_ctf->AddRGBPoint(30000,1,1,1);


		//v_ctf->AddRGBPoint(m_UnsignRange[1],0,0.2,0);


	}
  v_vtmHi->SetMaximumNumberOfPlanes(1024);
  v_vtmHi->SetTargetTextureSize(512,512);

  v_vtmLo->SetMaximumNumberOfPlanes(128);
  v_vtmLo->SetTargetTextureSize(32, 32);


  v_lod = vtkLODProp3D ::New();
	v_lod->AddLOD(v_vtmLo, v_vpLo, 0);
	v_lod->AddLOD(v_vtmHi, v_vpHi, 0);
	v_lod->PickableOff();
	v_lod->SetLODProperty(1, v_lodP);
	v_lod->SetLODProperty(2, v_lodP);
	v_lod->Modified();

  m_act_a = (vtkActor*)v_lod;

  m_AssemblyFront->AddPart(v_lod);
  m_AssemblyFront->PickableOff();

   //SIL. 20-6-2003 modified code - global visibility handling is temporary disable
  /*
	vtkTagItem *t = m_Vme->GetTagArray()->GetTag(L"visible");
  if(t) Show(t->GetValueAsDouble()!= 0);
	*/
}
//----------------------------------------------------------------------------
medPipeVolumeVR::~medPipeVolumeVR()
//----------------------------------------------------------------------------
{
  if(!m_created) return;

  m_AssemblyFront->RemovePart(m_act_a);
  //m_AssemblyFront->RemovePart(m_sel_a);

  vtkDEL(m_act_a);
 // vtkDEL(m_sel_a);

  vtkDEL(v_pf);
  vtkDEL(v_ctf);
  vtkDEL(v_vpLo);
  vtkDEL(v_vpHi);
  vtkDEL(v_vtmLo);
  vtkDEL(v_vtmHi);
  vtkDEL(v_imageshift);
  vtkDEL(v_gf);
  vtkDEL(v_structuredImage);
  vtkDEL(v_probe);
}
//----------------------------------------------------------------------------
void medPipeVolumeVR::Show(bool show)
//----------------------------------------------------------------------------
{
  if(!m_created) return;
	m_act_a->SetVisibility(show);
	//if(m_Selected) m_sel_a->SetVisibility(show);
}
//----------------------------------------------------------------------------
void medPipeVolumeVR::Select(bool sel)
//----------------------------------------------------------------------------
{
	if(!m_created || m_act_a == NULL) return;

	m_Selected = sel;

	//if(m_act_a->GetVisibility()) 
	//	m_sel_a->SetVisibility(sel);
}
//----------------------------------------------------------------------------
mmgGui *medPipeVolumeVR::CreateGui()
//----------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = new mmgGui(this);
  

  return m_Gui;
}
//----------------------------------------------------------------------------
void medPipeVolumeVR::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId()) 
    {
      case ID_LUT_CHOOSER:
        
      break;
    }
  }
}
//----------------------------------------------------------------------------
void medPipeVolumeVR::AddPoint(int scalarPoint,double opacity)
//----------------------------------------------------------------------------
{
	v_pf->AddPoint((double)scalarPoint,opacity);		
}
//----------------------------------------------------------------------------
void medPipeVolumeVR::AddPoint(int scalarPoint,double red,double green,double blue)
//----------------------------------------------------------------------------
{
	v_ctf->AddRGBPoint((double)scalarPoint,red,green,blue);

}


//----------------------------------------------------------------------------
void medPipeVolumeVR::AddGradPoint(int scalarPoint,double gradient)
//----------------------------------------------------------------------------
{

	v_gf->AddPoint((double)scalarPoint,gradient);	

}

//----------------------------------------------------------------------------
void medPipeVolumeVR::RemoveOpacityPoint(int scalarPoint)
//----------------------------------------------------------------------------
{
	v_pf->RemovePoint((double)scalarPoint);
}
//----------------------------------------------------------------------------
void medPipeVolumeVR::RemoveColorPoint(int scalarPoint)
//----------------------------------------------------------------------------
{
	v_ctf->RemovePoint((double)scalarPoint);	
}

//----------------------------------------------------------------------------
void medPipeVolumeVR::RemoveOpacityGradPoint(int scalarPoint)
//----------------------------------------------------------------------------
{

	v_gf->RemovePoint((double)scalarPoint);	

}
//----------------------------------------------------------------------------
void medPipeVolumeVR::SetNumberPoints(int n)
//----------------------------------------------------------------------------
{

	m_NumberOfSlices = n;
	if (m_isStructured) return;

	int dim[3];
	double bounds[6];

	((vtkStructuredPoints *)m_Vme->GetOutput()->GetVTKData())->GetDimensions(dim);

	//Get the bounds for m_Vme as (Xmin,Xmax,Ymin,Ymax,Zmin,Zmax)
	m_Vme->GetOutput()->GetVTKData()->GetBounds(bounds);

	double spaceInt_x = (bounds[1]-bounds[0])/(dim[0]-1);
	double spaceInt_y = (bounds[3]-bounds[2])/(dim[1]-1);;
	double spaceInt_z = (bounds[5]-bounds[4])/((double)m_NumberOfSlices);

	if(v_structuredImage) v_structuredImage->Delete();

	v_structuredImage = vtkStructuredPoints::New();

	v_structuredImage->SetOrigin(bounds[0],bounds[2],bounds[4]);
	int dim_z = (bounds[5]-bounds[4])/spaceInt_z + 1;

	v_structuredImage->SetDimensions(dim[0],dim[1],dim_z);
	v_structuredImage->SetSpacing(spaceInt_x,spaceInt_y,spaceInt_z);

	//v_structuredImage->Modified();
	v_structuredImage->Update();

	if(v_probe) v_probe->Delete();

	v_probe = vtkProbeFilter::New();

	v_probe->SetInput(v_structuredImage);
	v_probe->SetSource(m_Vme->GetOutput()->GetVTKData());

	v_probe->Update();



		

}