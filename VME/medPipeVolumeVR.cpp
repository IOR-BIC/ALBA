/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medPipeVolumeVR.cpp,v $
  Language:  C++
  Date:      $Date: 2008-09-03 12:26:42 $
  Version:   $Revision: 1.6 $
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
#include "mafGUI.h"
#include "mafGUILutPreset.h"

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
#include "vtkImageResample.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(medPipeVolumeVR);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medPipeVolumeVR::medPipeVolumeVR()
:mafPipe()
//----------------------------------------------------------------------------
{
  m_PiecewiseFunction			= NULL;
	m_ColorTransferFunction	= NULL;
	
	m_VolumePropertyHigh				= NULL;
	m_VolumePropertyLow					= NULL; 
	m_VolumeTextureMapperHigh		= NULL;
	m_VolumeTextureMapperLow		= NULL;  
	
	m_PropertyLOD	= NULL;
	m_ActorLOD		= NULL;

	m_ImageShift				= NULL;
	m_GradientFunction	= NULL;
	m_Range[0]		= 0;
	m_Range[1]		= 0;
	m_UnsignRange[0]= 0;
	m_UnsignRange[1]= 0;

	m_Rescale				= false;
	m_IsStructured	= true;
	
	m_StructuredImage = NULL;
  m_Probe						= NULL;
	m_ResampleFilter	= NULL;
	m_ResampleFactor	= 1.0;
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

	m_Created = true;
  
	m_Vme->Update();

	mafString vmeControl = m_Vme->GetOutput()->GetVTKData()->GetClassName();
	if(vmeControl == "vtkRectilinearGrid")
	{
		//sorry
		/*m_sel_a = NULL;
		m_act_a = NULL;
		m_created = false;
		return;*/

		m_IsStructured = false;

		int dim[3];
		double bounds[6];

		((vtkRectilinearGrid *)m_Vme->GetOutput()->GetVTKData())->GetDimensions(dim);

		//Get the bounds for m_Vme as (Xmin,Xmax,Ymin,Ymax,Zmin,Zmax)
		m_Vme->GetOutput()->GetVTKData()->GetBounds(bounds);

		m_StructuredImage = vtkStructuredPoints::New();

		m_Probe = vtkProbeFilter::New();
		
		//m_NumberOfSlices is 128 as default


		double spaceInt_x = (bounds[1]-bounds[0])/(dim[0]-1);
		double spaceInt_y = (bounds[3]-bounds[2])/(dim[1]-1);;
		double spaceInt_z = (bounds[5]-bounds[4])/((double)m_NumberOfSlices);


		m_StructuredImage->SetOrigin(bounds[0],bounds[2],bounds[4]);

		int dim_z = (bounds[5]-bounds[4])/spaceInt_z + 1;

		m_StructuredImage->SetDimensions(dim[0],dim[1],dim_z);

		m_StructuredImage->SetSpacing(spaceInt_x,spaceInt_y,spaceInt_z);

		m_StructuredImage->Update();

		m_Probe->SetInput(m_StructuredImage);
		m_Probe->SetSource(m_Vme->GetOutput()->GetVTKData());

		m_Probe->Update();


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

	m_PiecewiseFunction	= vtkPiecewiseFunction::New();
	m_ColorTransferFunction = vtkColorTransferFunction::New();
	m_GradientFunction  = vtkPiecewiseFunction::New();

	m_VolumePropertyHigh = vtkVolumeProperty::New(); 
	m_VolumePropertyHigh->SetColor(m_ColorTransferFunction);
	m_VolumePropertyHigh->SetScalarOpacity(m_PiecewiseFunction);
	m_VolumePropertyHigh->SetGradientOpacity(m_GradientFunction);
	//m_VolumePropertyHigh->DisableGradientOpacityOff();
	m_VolumePropertyHigh->SetInterpolationTypeToLinear();
	m_VolumePropertyHigh->ShadeOn();
	//m_VolumePropertyHigh->SetAmbient(0.1);
	//m_VolumePropertyHigh->SetDiffuse(0.9);
	//m_VolumePropertyHigh->SetSpecular(0.2);
	//m_VolumePropertyHigh->SetSpecularPower(100.0);
	m_VolumePropertyHigh->Modified();
	
	m_VolumePropertyLow = vtkVolumeProperty::New(); 
	m_VolumePropertyLow->SetColor(m_ColorTransferFunction);
	m_VolumePropertyLow->SetScalarOpacity(m_PiecewiseFunction);
	m_VolumePropertyLow->SetGradientOpacity(m_GradientFunction);
	//m_VolumePropertyLow->DisableGradientOpacityOff();
	m_VolumePropertyLow->ShadeOff();
	
	m_PropertyLOD = vtkProperty::New();
	m_PropertyLOD->SetAmbient(0.1);
	m_PropertyLOD->SetDiffuse(0.9);
	m_PropertyLOD->SetSpecular(0.2);
	m_PropertyLOD->SetSpecularPower(10.0);

	m_VolumeTextureMapperHigh = vtkVolumeTextureMapper2D::New();
	m_VolumeTextureMapperLow = vtkVolumeTextureMapper2D::New(); 
	
	m_Vme->GetOutput()->GetVTKData()->GetScalarRange(m_Range);

	m_Minimum = m_Range[0];
	m_Maximum = m_Range[1];

	if(m_Minimum < 0.0)
	{
		m_Rescale = true;

		m_ImageShift = vtkImageShiftScale::New();

		if (!m_IsStructured) 
			{

				m_ImageShift->SetInput((vtkImageData *)m_Probe->GetOutput());

			}

		else 
			{
				vtkDataSet* data = m_Vme->GetOutput()->GetVTKData();
				vtkNEW(m_ResampleFilter);
				m_ResampleFilter->SetInput((vtkImageData*)data);
				for(int i=0;i<3;i++)
					m_ResampleFilter->SetAxisMagnificationFactor(i,m_ResampleFactor);
				m_ResampleFilter->Update();
				m_ImageShift->SetInput((vtkImageData *)m_ResampleFilter->GetOutput());

			}

		m_ImageShift->SetShift(- m_Minimum);

		m_ImageShift->SetOutputScalarTypeToUnsignedShort();

		m_ImageShift->Update();
				
		m_VolumeTextureMapperHigh->SetInput((vtkImageData *)m_ImageShift->GetOutput());
		m_VolumeTextureMapperLow->SetInput((vtkImageData *)m_ImageShift->GetOutput());

		((vtkImageData *)m_ImageShift->GetOutput())->GetScalarRange(m_UnsignRange);


		// Originale
		//m_PiecewiseFunction->AddPoint(m_UnsignRange[0],0);
 		//m_PiecewiseFunction->AddPoint(m_UnsignRange[1],1.0);

		//v_gf->AddPoint(m_UnsignRange[0],0.4);
		//v_gf->AddPoint(m_UnsignRange[1],0.8);

		//m_ColorTransferFunction->AddRGBPoint(m_UnsignRange[0], 0,0,0);
		//m_ColorTransferFunction->AddRGBPoint(m_UnsignRange[1],0,0.2,0);

		/// vecchio punto
		//m_PiecewiseFunction->AddPoint(0,0);
		//m_PiecewiseFunction->AddPoint(10501,0);
		//m_PiecewiseFunction->AddPoint(21000,1);
		//m_PiecewiseFunction->AddPoint(32767,1);

		//m_ColorTransferFunction->AddRGBPoint(0,1.0,0.55,0.021);
		//m_ColorTransferFunction->AddRGBPoint(8121,1.0,0.42,0.2);
		//m_ColorTransferFunction->AddRGBPoint(16909,0.51,0.51,0.51);
		//m_ColorTransferFunction->AddRGBPoint(32767,1,1,1);

		/// Nuovo punto
		m_PiecewiseFunction->AddPoint(    0,0.0);
		m_PiecewiseFunction->AddPoint(22737,0.0);
		m_PiecewiseFunction->AddPoint(44327,1.0);
		m_PiecewiseFunction->AddPoint(65535,1.0);

		m_ColorTransferFunction->AddRGBPoint(    0, 0.00, 0.00, 0.00);
		m_ColorTransferFunction->AddRGBPoint(11655, 0.74, 0.19, 0.14);
		m_ColorTransferFunction->AddRGBPoint(31908, 0.96, 0.64, 0.42);
		m_ColorTransferFunction->AddRGBPoint(33818, 0.76, 0.78, 0.25);
		m_ColorTransferFunction->AddRGBPoint(41843, 1.00, 1.00, 1.00);
		m_ColorTransferFunction->AddRGBPoint(65535, 1.00, 1.00, 1.00);

		//v_gf->AddPoint(0,0);
		//v_gf->AddPoint(65535,1);

	}
	else 
	{
		if (!m_IsStructured)
			{
				m_VolumeTextureMapperHigh->SetInput((vtkImageData *)m_Probe->GetOutput());
				m_VolumeTextureMapperLow->SetInput((vtkImageData *)m_Probe->GetOutput());

			}

		else 
			{
				vtkDataSet* data = m_Vme->GetOutput()->GetVTKData();
				data->Update();
				vtkNEW(m_ResampleFilter);
				m_ResampleFilter->SetInput((vtkImageData*)data);
				for(int i=0;i<3;i++)
					m_ResampleFilter->SetAxisMagnificationFactor(i,m_ResampleFactor);
				m_ResampleFilter->Update();

				m_VolumeTextureMapperHigh->SetInput((vtkImageData *)m_ResampleFilter->GetOutput());
				m_VolumeTextureMapperLow->SetInput((vtkImageData *)m_ResampleFilter->GetOutput());

			}

		
			/// Nuovo punto
			m_PiecewiseFunction->AddPoint(    0,0.0);
			m_PiecewiseFunction->AddPoint(22737,0.0);
			m_PiecewiseFunction->AddPoint(44327,1.0);
			m_PiecewiseFunction->AddPoint(65535,1.0);

			m_ColorTransferFunction->AddRGBPoint(    0, 0.00, 0.00, 0.00);
			m_ColorTransferFunction->AddRGBPoint(11655, 0.74, 0.19, 0.14);
			m_ColorTransferFunction->AddRGBPoint(31908, 0.96, 0.64, 0.42);
			m_ColorTransferFunction->AddRGBPoint(33818, 0.76, 0.78, 0.25);
			m_ColorTransferFunction->AddRGBPoint(41843, 1.00, 1.00, 1.00);
			m_ColorTransferFunction->AddRGBPoint(65535, 1.00, 1.00, 1.00);

			/*m_GradientFunction->AddPoint(0,0);
			m_GradientFunction->AddPoint(33818,1);*/


		/*m_PiecewiseFunction->AddPoint(m_Range[0],0);
		m_PiecewiseFunction->AddPoint(m_Range[1],1.0);

		m_GradientFunction->AddPoint(m_Range[0],0.4);
		m_GradientFunction->AddPoint(m_Range[1],0.8);

		m_ColorTransferFunction->AddRGBPoint(m_Range[0], 0,0,0);

		m_ColorTransferFunction->AddRGBPoint(m_Range[1],0,0.2,0);*/
    //m_PiecewiseFunction->AddPoint(m_UnsignRange[0],0);
    //m_PiecewiseFunction->AddPoint(0,0);
    //m_PiecewiseFunction->AddPoint(20000,0);
    //m_PiecewiseFunction->AddPoint(24000,1);
    //m_PiecewiseFunction->AddPoint(32767,1);
    
		//m_PiecewiseFunction->AddPoint(m_UnsignRange[1],1.0);
    //m_GradientFunction->AddPoint(0,1);
		//m_GradientFunction->AddPoint(m_UnsignRange[0],0.4);
		//m_GradientFunction->AddPoint(m_UnsignRange[1],0.8);

		//m_ColorTransferFunction->AddRGBPoint(m_UnsignRange[0], 0,0,0);
    //m_ColorTransferFunction->AddRGBPoint(0,1.0,0.55,0.021);
    //m_ColorTransferFunction->AddRGBPoint(16909,0.51,0.51,0.51);
    //m_ColorTransferFunction->AddRGBPoint(30000,1,1,1);


		//m_ColorTransferFunction->AddRGBPoint(m_UnsignRange[1],0,0.2,0);


	}
  m_VolumeTextureMapperHigh->SetMaximumNumberOfPlanes(1024);
  m_VolumeTextureMapperHigh->SetTargetTextureSize(512,512);

  m_VolumeTextureMapperLow->SetMaximumNumberOfPlanes(128);
  m_VolumeTextureMapperLow->SetTargetTextureSize(32, 32);


  m_ActorLOD = vtkLODProp3D ::New();
	m_ActorLOD->AddLOD(m_VolumeTextureMapperLow, m_VolumePropertyLow, 0);
	m_ActorLOD->AddLOD(m_VolumeTextureMapperHigh, m_VolumePropertyHigh, 0);
	m_ActorLOD->PickableOff();
	m_ActorLOD->SetLODProperty(1, m_PropertyLOD);
	m_ActorLOD->SetLODProperty(2, m_PropertyLOD);
	m_ActorLOD->Modified();

  m_AssemblyFront->AddPart(m_ActorLOD);
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
  if(!m_Created) return;

  m_AssemblyFront->RemovePart(m_ActorLOD);

  vtkDEL(m_ActorLOD);
	vtkNEW(m_PropertyLOD);
  vtkDEL(m_PiecewiseFunction);
  vtkDEL(m_ColorTransferFunction);
  vtkDEL(m_VolumePropertyLow);
  vtkDEL(m_VolumePropertyHigh);
  vtkDEL(m_VolumeTextureMapperLow);
  vtkDEL(m_VolumeTextureMapperHigh);
  vtkDEL(m_ImageShift);
  vtkDEL(m_GradientFunction);
  vtkDEL(m_StructuredImage);
  vtkDEL(m_Probe);
	vtkDEL(m_ResampleFilter);
}
//----------------------------------------------------------------------------
void medPipeVolumeVR::Show(bool show)
//----------------------------------------------------------------------------
{
  if(!m_Created) return;
	m_ActorLOD->SetVisibility(show);
	//if(m_Selected) m_sel_a->SetVisibility(show);
}
//----------------------------------------------------------------------------
void medPipeVolumeVR::Select(bool sel)
//----------------------------------------------------------------------------
{
	if(!m_Created || m_ActorLOD == NULL) return;

	m_Selected = sel;

	//if(m_act_a->GetVisibility()) 
	//	m_sel_a->SetVisibility(sel);
}
//----------------------------------------------------------------------------
mafGUI *medPipeVolumeVR::CreateGui()
//----------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = new mafGUI(this);
  
	m_Gui->Double(ID_RESAMPLE_FACTOR,_("Resample"),&m_ResampleFactor,0.00001,1);

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
			case ID_RESAMPLE_FACTOR:
				{
					for(int i=0;i<3;i++)
						m_ResampleFilter->SetAxisMagnificationFactor(i,m_ResampleFactor);

					m_ResampleFilter->Update();
					mafEventMacro(mafEvent(this,CAMERA_UPDATE));
				}
				break;
    }
  }
}
//----------------------------------------------------------------------------
void medPipeVolumeVR::AddPoint(int scalarPoint,double opacity)
//----------------------------------------------------------------------------
{
	m_PiecewiseFunction->AddPoint((double)scalarPoint,opacity);		
}
//----------------------------------------------------------------------------
void medPipeVolumeVR::AddPoint(int scalarPoint,double red,double green,double blue)
//----------------------------------------------------------------------------
{
	m_ColorTransferFunction->AddRGBPoint((double)scalarPoint,red,green,blue);
}
//----------------------------------------------------------------------------
void medPipeVolumeVR::AddGradPoint(int scalarPoint,double gradient)
//----------------------------------------------------------------------------
{
	m_GradientFunction->AddPoint((double)scalarPoint,gradient);	
}
//----------------------------------------------------------------------------
void medPipeVolumeVR::RemoveOpacityPoint(int scalarPoint)
//----------------------------------------------------------------------------
{
	m_PiecewiseFunction->RemovePoint((double)scalarPoint);
}
//----------------------------------------------------------------------------
void medPipeVolumeVR::RemoveColorPoint(int scalarPoint)
//----------------------------------------------------------------------------
{
	m_ColorTransferFunction->RemovePoint((double)scalarPoint);	
}
//----------------------------------------------------------------------------
void medPipeVolumeVR::RemoveOpacityGradPoint(int scalarPoint)
//----------------------------------------------------------------------------
{
	m_GradientFunction->RemovePoint((double)scalarPoint);	
}
//----------------------------------------------------------------------------
void medPipeVolumeVR::SetNumberPoints(int n)
//----------------------------------------------------------------------------
{

	m_NumberOfSlices = n;
	if (m_IsStructured) return;

	int dim[3];
	double bounds[6];

	((vtkStructuredPoints *)m_Vme->GetOutput()->GetVTKData())->GetDimensions(dim);

	//Get the bounds for m_Vme as (Xmin,Xmax,Ymin,Ymax,Zmin,Zmax)
	m_Vme->GetOutput()->GetVTKData()->GetBounds(bounds);

	double spaceInt_x = (bounds[1]-bounds[0])/(dim[0]-1);
	double spaceInt_y = (bounds[3]-bounds[2])/(dim[1]-1);;
	double spaceInt_z = (bounds[5]-bounds[4])/((double)m_NumberOfSlices);

	if(m_StructuredImage) m_StructuredImage->Delete();

	m_StructuredImage = vtkStructuredPoints::New();

	m_StructuredImage->SetOrigin(bounds[0],bounds[2],bounds[4]);
	int dim_z = (bounds[5]-bounds[4])/spaceInt_z + 1;

	m_StructuredImage->SetDimensions(dim[0],dim[1],dim_z);
	m_StructuredImage->SetSpacing(spaceInt_x,spaceInt_y,spaceInt_z);

	//m_StructuredImage->Modified();
	m_StructuredImage->Update();

	if(m_Probe) m_Probe->Delete();

	m_Probe = vtkProbeFilter::New();

	m_Probe->SetInput(m_StructuredImage);
	m_Probe->SetSource(m_Vme->GetOutput()->GetVTKData());

	m_Probe->Update();	
}
//----------------------------------------------------------------------------
double medPipeVolumeVR::GetResampleFactor()
//----------------------------------------------------------------------------
{
	return m_ResampleFactor;
}
//----------------------------------------------------------------------------
void medPipeVolumeVR::SetResampleFactor(double value)
//----------------------------------------------------------------------------
{
	m_ResampleFactor = value;

	
	if(m_ResampleFilter)
	{
		m_AssemblyFront->RemovePart(m_ActorLOD);
		vtkDEL(m_ActorLOD);
		vtkDEL(m_VolumeTextureMapperHigh);
		vtkDEL(m_VolumeTextureMapperLow);

		m_ResampleFilter->Update();

		for(int i=0;i<3;i++)
			m_ResampleFilter->SetAxisMagnificationFactor(i,m_ResampleFactor);

		m_ResampleFilter->Update();

		vtkNEW(m_VolumeTextureMapperHigh);
		vtkNEW(m_VolumeTextureMapperLow);

		m_VolumeTextureMapperHigh->SetInput(m_ResampleFilter->GetOutput());
		m_VolumeTextureMapperLow->SetInput(m_ResampleFilter->GetOutput());

		m_VolumeTextureMapperHigh->SetMaximumNumberOfPlanes(1024);
		m_VolumeTextureMapperHigh->SetTargetTextureSize(512,512);

		m_VolumeTextureMapperLow->SetMaximumNumberOfPlanes(128);
		m_VolumeTextureMapperLow->SetTargetTextureSize(32, 32);

		vtkNEW(m_ActorLOD);

		m_ActorLOD->AddLOD(m_VolumeTextureMapperLow, m_VolumePropertyLow, 0);
		m_ActorLOD->AddLOD(m_VolumeTextureMapperHigh, m_VolumePropertyHigh, 0);
		m_ActorLOD->PickableOff();
		m_ActorLOD->SetLODProperty(1, m_PropertyLOD);
		m_ActorLOD->SetLODProperty(2, m_PropertyLOD);
		m_ActorLOD->Modified();

		m_AssemblyFront->AddPart(m_ActorLOD);
	}

	if(m_Gui)
		m_Gui->Update();
}
//----------------------------------------------------------------------------
void medPipeVolumeVR::VolumePropertyHighShadeOn()
//----------------------------------------------------------------------------
{
	m_VolumePropertyHigh->ShadeOn();
	m_VolumePropertyHigh->Modified();

}
//----------------------------------------------------------------------------
void medPipeVolumeVR::VolumePropertyHighShadeOff()
//----------------------------------------------------------------------------
{
	m_VolumePropertyHigh->ShadeOff();
	m_VolumePropertyHigh->Modified();
}