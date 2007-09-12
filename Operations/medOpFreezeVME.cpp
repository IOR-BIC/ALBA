/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpFreezeVME.cpp,v $
  Language:  C++
  Date:      $Date: 2007-09-12 13:55:02 $
  Version:   $Revision: 1.2 $
  Authors:   Daniele Giunchi
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

#include "medOpFreezeVME.h"
#include "wx/busyinfo.h"

#include "mafDecl.h"
#include "mafVME.h"
#include "mafSmartPointer.h"

#include "mafVMEGenericAbstract.h"
#include "mafVMERoot.h"
#include "mafVMERefSys.h"
#include "mafVMEVolume.h"

#include "mafVMESurface.h"
#include "mafVMEPolyline.h"
#include "mafVMEPolylineSpline.h"
#include "mafVMESurfaceParametric.h"
#include "mafVMEMeter.h"
#include "mafVMESlicer.h"
#include "mafVMEProber.h"
#include "medVMEWrappedMeter.h"

#include "mmaMaterial.h"
#include "mmgGui.h"
#include "mafAbsMatrixPipe.h"

#include "vtkPolyData.h"
#include "vtkImageData.h"


//----------------------------------------------------------------------------
mafCxxTypeMacro(medOpFreezeVME);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medOpFreezeVME::medOpFreezeVME(wxString label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_OP;
	m_Canundo = true;
	m_InputPreserving = true;
  
}
//----------------------------------------------------------------------------
medOpFreezeVME::~medOpFreezeVME( ) 
//----------------------------------------------------------------------------
{
  
}
//----------------------------------------------------------------------------
bool medOpFreezeVME::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node->IsMAFType(mafVME) && 
          !node->IsMAFType(mafVMEGenericAbstract) && 
          !node->IsMAFType(mafVMERoot) &&
          //!node->IsMAFType(mafVMERefSys) &&
          !node->IsMAFType(mafVMEVolume) ); //return if is a procedural vme
}
//----------------------------------------------------------------------------
mafOp* medOpFreezeVME::Copy()   
//----------------------------------------------------------------------------
{
  medOpFreezeVME *cp = new medOpFreezeVME(m_Label);
  return cp;
}
//----------------------------------------------------------------------------
void medOpFreezeVME::OpRun()   
//----------------------------------------------------------------------------
{
  if(!m_TestMode)
    m_Gui = new mmgGui(this);

	//control the output and create the right vme
	mafVME *vme = mafVME::SafeDownCast(m_Input);
  vme->Update();
  mafVMEOutput *output = vme->GetOutput();
  output->Update();
	
	/*if(vtkImageData *imageData = vtkImageData::SafeDownCast(output->GetVTKData()))
	{
	}
	else if(vtkRectilinearGrid *rectilinearGrid = vtkRectilinearGrid::SafeDownCast(output->GetVTKData()))
	{
	}
	else*/
	if(vtkPolyData *polyData = vtkPolyData::SafeDownCast(output->GetVTKData()))
	{
		if(mafVMEPolylineSpline *vmeSpline = mafVMEPolylineSpline::SafeDownCast(vme))
	  {
	  	mmaMaterial *material = vmeSpline->GetMaterial();
	  	
	  	mafSmartPointer<mafVMEPolyline> newPolyline;
			newPolyline->SetName(vmeSpline->GetName());
			newPolyline->SetData(polyData,vmeSpline->GetTimeStamp());
			newPolyline->Update();

      if(material)
      {
			  newPolyline->GetMaterial()->DeepCopy(material);
        newPolyline->GetMaterial()->UpdateProp();
      }
			newPolyline->SetMatrix(*vmeSpline->GetOutput()->GetMatrix());
			m_Output=newPolyline;
      if (m_Output)
      {
        m_Output->ReparentTo(m_Input->GetParent());
        if(!m_TestMode)
          OpStop(OP_RUN_OK);
      }
	  }
	  else if(mafVMESurfaceParametric *vmeSurface = mafVMESurfaceParametric::SafeDownCast(vme))
		{
			mmaMaterial *material = vmeSurface->GetMaterial();
			
			mafSmartPointer<mafVMESurface> newSurface;
			newSurface->SetName(vmeSurface->GetName());
			newSurface->SetData(polyData,vmeSurface->GetTimeStamp());
			newSurface->Update();

      if(material)
      {
        newSurface->GetMaterial()->DeepCopy(material);
        newSurface->GetMaterial()->UpdateProp();
      }
      
      newSurface->SetMatrix(*vmeSurface->GetOutput()->GetMatrix());
			m_Output=newSurface;
      if (m_Output)
      {
        m_Output->ReparentTo(m_Input->GetParent());
        if(!m_TestMode)
          OpStop(OP_RUN_OK);
      }
		}
		else if(mafVMEMeter *meter = mafVMEMeter::SafeDownCast(vme))
		{
			mmaMaterial *material = meter->GetMaterial();
			
			mafSmartPointer<mafVMEPolyline> newPolyline;
			newPolyline->SetName(meter->GetName());
			newPolyline->SetData(polyData,meter->GetTimeStamp());
			newPolyline->Update();

      if(material)
      {
        newPolyline->GetMaterial()->DeepCopy(material);
        newPolyline->GetMaterial()->UpdateProp();
      }
			
      newPolyline->SetMatrix(*meter->GetOutput()->GetMatrix());
      m_Output=newPolyline;
      if (m_Output)
      {
        m_Output->ReparentTo(m_Input->GetParent());
        if(!m_TestMode)
          OpStop(OP_RUN_OK);
      }
		}
    else if(mafVMERefSys *refsys = mafVMERefSys::SafeDownCast(vme))
    {
      mmaMaterial *material = refsys->GetMaterial();

      mafSmartPointer<mafVMESurface> surface;
      surface->SetName(refsys->GetName());
      surface->SetData(polyData,refsys->GetTimeStamp());
      surface->Update();

      if(material)
      {
        surface->GetMaterial()->DeepCopy(material);
        surface->GetMaterial()->UpdateProp();
      }

      surface->SetMatrix(*refsys->GetOutput()->GetMatrix());
      m_Output=surface;
      if (m_Output)
      {
        m_Output->ReparentTo(m_Input->GetParent());
        if(!m_TestMode)
          OpStop(OP_RUN_OK);
      }
    }
    else if(mafVMESlicer *slicer = mafVMESlicer::SafeDownCast(vme))
    {
      mmaMaterial *material = slicer->GetMaterial();

      mafSmartPointer<mafVMESurface> newSurface;
      newSurface->SetName(slicer->GetName());
      newSurface->SetData(polyData,slicer->GetTimeStamp());
      newSurface->GetSurfaceOutput()->SetTexture(slicer->GetSurfaceOutput()->GetTexture());
      newSurface->Update();

      if(material)
      {
        newSurface->GetMaterial()->DeepCopy(material);
        newSurface->GetMaterial()->SetMaterialTexture(newSurface->GetSurfaceOutput()->GetTexture());
        newSurface->GetMaterial()->UpdateProp();
      }

      newSurface->SetMatrix(*slicer->GetOutput()->GetMatrix());
      m_Output=newSurface;
      if (m_Output)
      {
        m_Output->ReparentTo(m_Input->GetParent());
        if(!m_TestMode)
          OpStop(OP_RUN_OK);
      }
      
    }
    else if(mafVMEProber *prober = mafVMEProber::SafeDownCast(vme))
    {
      mmaMaterial *material = prober->GetMaterial();

      mafSmartPointer<mafVMESurface> newSurface;
      newSurface->SetName(prober->GetName());
      newSurface->SetData(polyData,prober->GetTimeStamp());
      newSurface->Update();

      if(material)
      {
        newSurface->GetMaterial()->DeepCopy(material);
        newSurface->GetMaterial()->UpdateProp();
      }

      newSurface->SetMatrix(*prober->GetOutput()->GetMatrix());
      m_Output=newSurface;
      if (m_Output)
      {
        m_Output->ReparentTo(m_Input->GetParent());
        if(!m_TestMode)
          OpStop(OP_RUN_OK);
      }
    }
		else if(medVMEWrappedMeter *wrappedMeter = medVMEWrappedMeter::SafeDownCast(vme))
		{
			mmaMaterial *material = wrappedMeter->GetMaterial();
			
			mafSmartPointer<mafVMEPolyline> newPolyline;
			newPolyline->SetName(wrappedMeter->GetName());
			newPolyline->SetData(polyData,wrappedMeter->GetTimeStamp());
			newPolyline->Update();

      if(material)
      {
        newPolyline->GetMaterial()->DeepCopy(material);
        newPolyline->GetMaterial()->UpdateProp();
      }
			
      newPolyline->SetMatrix(*wrappedMeter->GetOutput()->GetMatrix());
      m_Output=newPolyline;
      if (m_Output)
      {
        m_Output->ReparentTo(m_Input->GetParent());
        if(!m_TestMode)
          OpStop(OP_RUN_OK);
      }
		}
    else
      OpStop(OP_RUN_CANCEL);
		
	}
	else
	  OpStop(OP_RUN_CANCEL);

	
}
//----------------------------------------------------------------------------
void medOpFreezeVME::OpDo()   
//----------------------------------------------------------------------------
{
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}


