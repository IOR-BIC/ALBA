/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaClassicICPRegistration
 Authors: Stefania Paperini paperini@tecno.ior.it
 
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

#include "albaClassicICPRegistration.h"
#include "vtkCellLocator.h"
#include "vtkTransform.h"
#include "vtkObjectFactory.h"
#include "vtkMath.h"

//#include "vcl_fstream.h"
#include <iostream>

#include "albaICPUtility.h"

	vtkCxxRevisionMacro(albaClassicICPRegistration, "$Revision: 1.1.2.1 $");
  vtkStandardNewMacro(albaClassicICPRegistration);

/*//--------------------------------------------------------------------------
albaClassicICPRegistration* albaClassicICPRegistration::New()
//----------------------------------------------------------------------------
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("albaClassicICPRegistration");
  if(ret)
    {
    return (albaClassicICPRegistration*)ret;
    }

  // If the factory was unable to create the object, then create it here.
  return new albaClassicICPRegistration;
}*/

//-------------------------------------------------------------------------
//albaCxxTypeMacro(albaClassicICPRegistration)
//-------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaClassicICPRegistration::albaClassicICPRegistration() : vtkIterativeClosestPointTransform()
//----------------------------------------------------------------------------
{
  this->Source	= NULL;
  this->Target	= NULL;
  this->Locator = NULL;
  this->ICPUtil	= albaICPUtility::New();
  this->SaveResults = 0;
  this->Convergence	= 1e-5;
  //modified by Stefano 7-11-2004
  this->RegistrationError = 0;

}
//----------------------------------------------------------------------------
albaClassicICPRegistration::~albaClassicICPRegistration()
//----------------------------------------------------------------------------
{
  ReleaseSource();
  ReleaseTarget();
  ReleaseLocator();
  vtkDEL(this->ICPUtil);
}

//----------------------------------------------------------------------------
void albaClassicICPRegistration::SetSource(vtkDataSet *source)
//----------------------------------------------------------------------------
{
  if (this->Source == source)
    {
    return;
    }

  if (this->Source)
    {
    this->ReleaseSource();
    }

  if (source)
    {
    source->Register(this);
    }

  this->Source = source;
  this->Modified();
}
//----------------------------------------------------------------------------
void albaClassicICPRegistration::ReleaseSource(void) 
//----------------------------------------------------------------------------
{
  if (this->Source) 
    {
    this->Source->UnRegister(this);
    this->Source = NULL;
    }
}
//----------------------------------------------------------------------------
void albaClassicICPRegistration::SetTarget(vtkDataSet *target)
//----------------------------------------------------------------------------
{
  if (this->Target == target)
    {
    return;
    }

  if (this->Target)
    {
    this->ReleaseTarget();
    }

  if (target)
    {
    target->Register(this);
    }

  this->Target = target;
  this->Modified();
}
//----------------------------------------------------------------------------
void albaClassicICPRegistration::ReleaseTarget(void) 
//----------------------------------------------------------------------------
{
  if (this->Target) 
    {
    this->Target->UnRegister(this);
    this->Target = NULL;
    }
}
//----------------------------------------------------------------------------
void albaClassicICPRegistration::SetLocator(vtkCellLocator *locator)
//----------------------------------------------------------------------------
{
  if (this->Locator == locator)
    {
    return;
    }

  if (this->Locator)
    {
    this->ReleaseLocator();
    }

  if (locator)
    {
    locator->Register(this);
    }

  this->Locator = locator;
  this->Modified();
}
//----------------------------------------------------------------------------
void albaClassicICPRegistration::ReleaseLocator(void) 
//----------------------------------------------------------------------------
{
  if (this->Locator) 
    {
    this->Locator->UnRegister(this);
    this->Locator = NULL;
    }
}
//----------------------------------------------------------------------------
void albaClassicICPRegistration::CreateDefaultLocator() 
//----------------------------------------------------------------------------
{
  if (this->Locator) 
    {
    this->ReleaseLocator();
    }

  this->Locator = vtkCellLocator::New();
}

//----------------------------------------------------------------------------
void albaClassicICPRegistration::SetResultsFileName(const char *name)
//----------------------------------------------------------------------------
{
	this->ResultsFile = name;
	this->Modified();
}
//----------------------------------------------------------------------------
unsigned long albaClassicICPRegistration::GetMTime()
//----------------------------------------------------------------------------
{
  unsigned long result = vtkIterativeClosestPointTransform::GetMTime();
  unsigned long mtime;

  if (this->Source)
    {
    mtime = this->Source->GetMTime(); 
    if (mtime > result)
      {
      result = mtime;
      }
    }

  if (this->Target)
    {
    mtime = this->Target->GetMTime(); 
    if (mtime > result)
      {
      result = mtime;
      }
    }

  if (this->Locator)
    {
    mtime = this->Locator->GetMTime(); 
    if (mtime > result)
      {
      result = mtime;
      }
    }

  return result;
}
//----------------------------------------------------------------------------
vtkAbstractTransform *albaClassicICPRegistration::MakeTransform()
//----------------------------------------------------------------------------
{
  return albaClassicICPRegistration::New(); 
}
//----------------------------------------------------------------------------
void albaClassicICPRegistration::InternalDeepCopy(vtkAbstractTransform *transform)
//----------------------------------------------------------------------------
{
  albaClassicICPRegistration *t = (albaClassicICPRegistration *)transform;

  this->SetSource(t->GetSource());
  this->SetTarget(t->GetTarget());
  this->SetLocator(t->GetLocator());
  
  this->Modified();
}
//----------------------------------------------------------------------------
void albaClassicICPRegistration::InternalUpdate()
//----------------------------------------------------------------------------
{
  // Check source, target
  if (this->Source == NULL || !this->Source->GetNumberOfPoints())
  {
    vtkErrorMacro(<<"Can't execute with NULL or empty input");
    return;
  }

  if (this->Target == NULL || !this->Target->GetNumberOfPoints())
  {
    vtkErrorMacro(<<"Can't execute with NULL or empty target");
    return;
  }


  vnl_matrix<double> target_mtx; //This is the so called Model Shape (MS) in the Besl and McKey paper
  vnl_matrix<double> source_mtx; //This is the so called Data Shape (DS) in the Besl and McKey paper
  vnl_matrix<double> source_orig_mtx = source_mtx;

  source_mtx = this->ICPUtil->PolyData2VnlFilter((vtkPolyData *)this->Source);
  target_mtx = this->ICPUtil->PolyData2VnlFilter((vtkPolyData *)this->Target);

  //call register method
  albaICPUtility::RegResult FFReg_res = this->ICPUtil->StandardRegistration( target_mtx, source_mtx,Convergence);	
 
  //Reconstruction of mflVME with the new data from ICP
  vnl_matrix_fixed<double,4,4> T = this->ICPUtil->R_t_2_Tr(FFReg_res.R,FFReg_res.t);

  vtkMatrix4x4 *mat = vtkMatrix4x4::New();
  mat->DeepCopy(T.data_block());
	
  // Now recover accumulated result
  this->Matrix->DeepCopy(mat);

  if(this->SaveResults)
	{
		// simple testing file result
			
    std::ofstream risultati (this->ResultsFile.ToAscii(),std::ios::out); 

		risultati << "Rotation: " <<"\n" << FFReg_res.R << "\n";
		risultati << "\n" << "Translation: " <<"\n" << FFReg_res.t << "\n";
		risultati << "\n" << "Pose Matrix: " <<"\n" << FFReg_res.pose << "\n";
		risultati << "\n" << "Attitude Vector: " <<"\n" << FFReg_res.th<< "\n";

		//risultati << "\n" << "Convergence Test: " << FFReg_res.test<< "\n";

		risultati << "\n" << "\n" << "error: " << FFReg_res.err << "\n";

		risultati.close();

	}

  //modified by Stefano 7-11-2004
  this->RegistrationError = FFReg_res.err;

  mat->Delete();
}

//----------------------------------------------------------------------------
/*
void albaClassicICPRegistration::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkLinearTransform::PrintSelf(os,indent);

  if ( this->Source ) 
    {
    os << indent << "Source: " << this->Source << "\n";
    }
  else 
    {
    os << indent << "Source: (none)\n";
    }

  if ( this->Target ) 
    {
    os << indent << "Target: " << this->Target << "\n";
    }
  else 
    {
    os << indent << "Target: (none)\n";
    }

  if ( this->Locator ) 
    {
    os << indent << "Locator: " << this->Locator << "\n";
    }
  else 
    {
    os << indent << "Locator: (none)\n";
    }

  os << indent << "MaximumNumberOfIterations: " << this->MaximumNumberOfIterations << "\n";
  if(this->IterativeClosestPointTransform)
    {
    os << indent << "IterativeClosestPointTransform:\n";
    this->IterativeClosestPointTransform->PrintSelf(os, indent.GetNextIndent());
    }
}
*/