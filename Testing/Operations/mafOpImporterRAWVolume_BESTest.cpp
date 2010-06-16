/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafOpImporterRAWVolume_BESTest.cpp,v $
Language:  C++
Date:      $Date: 2010-06-16 09:12:19 $
Version:   $Revision: 1.1.2.1 $
Authors:   Roberto Mucci
==========================================================================
Copyright (c) 2002/2004 
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

#include "mafDefines.h"
#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "mafOpImporterRAWVolume_BESTest.h"
#include "mafOpImporterRAWVolume_BES.h"

#include "mafVMEStorage.h"
#include "mafVMERoot.h"
#include "mafNode.h"

#include "vtkImageData.h"

//-----------------------------------------------------------
void mafOpImporterRAWVolume_BESTest::setUp() 
//-----------------------------------------------------------
{
}
//-----------------------------------------------------------
void mafOpImporterRAWVolume_BESTest::tearDown() 
//-----------------------------------------------------------
{
}
//-----------------------------------------------------------
void mafOpImporterRAWVolume_BESTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  mafOpImporterRAWVolume_BES *importer=new mafOpImporterRAWVolume_BES();
  mafDEL(importer);
}
//-----------------------------------------------------------
void mafOpImporterRAWVolume_BESTest::TestImport() 
//-----------------------------------------------------------
{
  mafVMEStorage *Storage = mafVMEStorage::New();
  Storage->GetRoot()->SetName("root");
  Storage->GetRoot()->Initialize();

  mafOpImporterRAWVolume_BES *Importer=new mafOpImporterRAWVolume_BES("importer");
  Importer->TestModeOn();
  Importer->SetInput(Storage->GetRoot());
  mafString Filename=MED_DATA_ROOT;
  Filename<<"/RAW_Volume/Volume.raw";
  Importer->SetFileName(Filename.GetCStr());
  int Dimensions[3]={512,512,5};
  Importer->SetDataDimensions(Dimensions);
  double Spacing[3]={0.2,0.3,1.0};
  Importer->SetDataSpacing(Spacing);
  Importer->SetScalarType(Importer->SHORT_SCALAR);
  Importer->ScalarSignedOn();
  Importer->SetScalarDataToLittleEndian();
  int VOI[2]={0,5};
  Importer->SetDataVOI(VOI);
  bool ok=Importer->Import();
  CPPUNIT_ASSERT(ok);

  mafVME *VME=mafVME::SafeDownCast(Importer->GetOutput());
  vtkImageData *Data=vtkImageData::SafeDownCast(VME->GetOutput()->GetVTKData());
  Data->UpdateData();
  Data->ComputeBounds();
  CPPUNIT_ASSERT(Data);

  //Check type scalar
  CPPUNIT_ASSERT(Data->GetScalarType()==VTK_SHORT);

  //Check spacing
  double NewSpacing[3];
  Data->GetSpacing(NewSpacing);
  CPPUNIT_ASSERT(NewSpacing[0]==Spacing[0]);
  CPPUNIT_ASSERT(NewSpacing[1]==Spacing[1]);
  CPPUNIT_ASSERT(NewSpacing[2]==Spacing[2]);

  //Check number of points
  int NewDimensions[3];
  Data->GetDimensions(NewDimensions);
  CPPUNIT_ASSERT(NewDimensions[0]==Dimensions[0]);
  CPPUNIT_ASSERT(NewDimensions[1]==Dimensions[1]);
  CPPUNIT_ASSERT(NewDimensions[2]==VOI[1]-VOI[0]);

  cppDEL(Importer);
  mafDEL(Storage);
    

}