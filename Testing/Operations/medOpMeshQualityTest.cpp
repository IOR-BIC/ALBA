/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpMeshQualityTest.cpp,v $
Language:  C++
Date:      $Date: 2010-03-29 08:55:45 $
Version:   $Revision: 1.1.2.1 $
Authors:   Eleonora Mambrini
==========================================================================
Copyright (c) 2007
CINECA - Interuniversity Consortium (www.cineca.it)
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)

MafMedical Library use license agreement

The software named MafMedical Library and any accompanying documentation, 
manuals or data (hereafter collectively "SOFTWARE") is property of the SCS s.r.l.
This is an open-source copyright as follows:
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.
* Modified source versions must be plainly marked as such, and must not be misrepresented 
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MafMedical is partially based on OpenMAF.
=========================================================================*/

#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medOpMeshQualityTest.h"
#include "medOpMeshQuality.h"

#include "mafString.h"
#include "mafVMESurface.h"

#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkTriangleQualityRatio.h"
#include "vtkPolyDataReader.h"
#include "vtkMAFSmartPointer.h"

#include <string>
#include <assert.h>


class MeshQualityDummy: public medOpMeshQuality
{
public:
  
  double GetMeanRatio();
  double GetMaxRatio();
  double GetMinRatio();

  void OpExecute();
};

double MeshQualityDummy::GetMeanRatio()
{
  assert(m_CheckMeshQuality);

  return m_CheckMeshQuality->GetMeanRatio();
}

double MeshQualityDummy::GetMaxRatio()
{
  assert(m_CheckMeshQuality);

  return m_CheckMeshQuality->GetMaxRatio();
}

double MeshQualityDummy::GetMinRatio()
{
  assert(m_CheckMeshQuality);

  return m_CheckMeshQuality->GetMinRatio();
}

void MeshQualityDummy::OpExecute()
{
  vtkNEW(m_CheckMeshQuality);
  TestModeOn();

  CreatePolydataPipeline();
}

//-----------------------------------------------------------
void medOpMeshQualityTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  medOpMeshQuality *meshQuality = new medOpMeshQuality();
  mafDEL(meshQuality);
}
//-----------------------------------------------------------
void medOpMeshQualityTest::TestOpRun() 
//-----------------------------------------------------------
{
  mafString filename=MED_DATA_ROOT;
  filename<<"/Surface/Sphere.vtk";
  vtkMAFSmartPointer<vtkPolyDataReader> reader;
  reader->SetFileName(filename.GetCStr());
  reader->Update();

  mafSmartPointer<mafVMESurface>surface;
  surface->SetData(reader->GetOutput(),0.0);
  surface->GetOutput()->GetVTKData()->Update();
  surface->Update();
  CPPUNIT_ASSERT(surface);

  surface->Update();
  surface->GetOutput()->GetVTKData()->Update();
  vtkPolyData *data=vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData());
  CPPUNIT_ASSERT(data);

  data->Update();

  vtkTriangleQualityRatio *triangleQualityRatio;
  vtkNEW(triangleQualityRatio);
  triangleQualityRatio->SetInput(data);
  triangleQualityRatio->Update(); 

  MeshQualityDummy *meshQuality = new MeshQualityDummy();
  meshQuality->SetInput(surface);
  meshQuality->OpExecute();

  CPPUNIT_ASSERT(meshQuality->GetMaxRatio() == triangleQualityRatio->GetMaxRatio());
  CPPUNIT_ASSERT(meshQuality->GetMinRatio() == triangleQualityRatio->GetMinRatio());
  CPPUNIT_ASSERT(meshQuality->GetMeanRatio() == triangleQualityRatio->GetMeanRatio());

  cppDEL(meshQuality);
  vtkDEL(triangleQualityRatio);
  //mafDEL(storage);

}
