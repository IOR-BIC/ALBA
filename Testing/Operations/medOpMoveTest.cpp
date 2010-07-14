/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpMoveTest.cpp,v $
Language:  C++
Date:      $Date: 2010-07-14 12:18:42 $
Version:   $Revision: 1.1.2.1 $
Authors:   Simone Brazzale
==========================================================================
Copyright (c) 2010
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
#include "medOpMoveTest.h"
#include "medOpMove.h"

#include "mafVMEGroup.h"
#include "mafVMEVolumeGray.h"
#include "medVMEPolylineGraph.h"
#include "mafVMESurface.h"
#include "vtkMAFSmartPointer.h"
#include "mafVMERoot.h"
#include "mafVME.h"
#include "mafVMEExternalData.h"

#include "vtkSphereSource.h"
#include "vtkDataSet.h"
//#include "vtkCellData.h"
//#include "vtkPolyData.h"

//-----------------------------------------------------------
void medOpMoveTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  medOpMove *move = new medOpMove();
  mafDEL(move);
}
//-----------------------------------------------------------
void medOpMoveTest::TestAccept() 
//-----------------------------------------------------------
{
  mafSmartPointer<mafVMEGroup> group;
  mafSmartPointer<mafVMEVolumeGray> volume;
  mafSmartPointer<medVMEPolylineGraph> polyline;
  mafSmartPointer<mafVMESurface> surface;
  mafSmartPointer<mafVMERoot> root;
  mafSmartPointer<mafVMEExternalData> external;

  medOpMove *move = new medOpMove();
  move->TestModeOn();

  // Try with accepted data type
  CPPUNIT_ASSERT( move->Accept(group) );
  CPPUNIT_ASSERT( move->Accept(volume) );
  CPPUNIT_ASSERT( move->Accept(polyline) );
  CPPUNIT_ASSERT( move->Accept(surface) );

  // Try with unwanted data type
  CPPUNIT_ASSERT( !move->Accept(root) );
  CPPUNIT_ASSERT( !move->Accept(external) );

  mafDEL(move);
}
//-----------------------------------------------------------
void medOpMoveTest::TestReset() 
//-----------------------------------------------------------
{
  vtkMAFSmartPointer<vtkSphereSource> sphere;
  sphere->Update();

  mafSmartPointer<mafVMESurface> surface;
  surface->SetData(sphere->GetOutput(),0.0);
  surface->GetOutput()->GetVTKData()->Update();
  surface->GetOutput()->Update();
  surface->Update();

  mafMatrix* old_matrix;
  mafNEW(old_matrix);
  old_matrix->DeepCopy((mafMatrix*)surface->GetOutput()->GetAbsMatrix());
  double value = old_matrix->GetElement(0,0);  

  medOpMove *move = new medOpMove();
  move->TestModeOn();
  move->SetInput(surface);
  move->OpRun();

  surface->GetOutput()->GetAbsMatrix()->SetElement(0,0,value*2);
  CPPUNIT_ASSERT( !((mafMatrix*)surface->GetOutput()->GetAbsMatrix())->Equals(old_matrix) );

  move->Reset();  
  CPPUNIT_ASSERT( ((mafMatrix*)surface->GetOutput()->GetAbsMatrix())->Equals(old_matrix) );

  mafDEL(old_matrix);
  mafDEL(move);
}