/////////////////////////////////////////////////////////////////////////////
// Name:        vtkMEDPolyDataMirror.cpp
//
// project:     MULTIMOD 
// Author:      Silvano Imboden
// Date:        28/10/2003
/////////////////////////////////////////////////////////////////////////////
#include "vtkMEDPolyDataMirror.h"

#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkDoubleArray.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkPolygon.h"
#include "vtkTriangleStrip.h"
#include "vtkPolyDataNormals.h"

vtkCxxRevisionMacro(vtkMEDPolyDataMirror, "$Revision: 1.3.2.1 $");
vtkStandardNewMacro(vtkMEDPolyDataMirror);

//----------------------------------------------------------------------------
vtkMEDPolyDataMirror::vtkMEDPolyDataMirror()
//----------------------------------------------------------------------------
{
  this->FlipNormals = 0;
  this->MirrorXCoordinate = 0;
  this->MirrorYCoordinate = 0;
  this->MirrorZCoordinate = 0;
}
//----------------------------------------------------------------------------
void vtkMEDPolyDataMirror::Execute()
//----------------------------------------------------------------------------
{
  vtkPoints *inPts;
  vtkPoints *newPts;
  vtkIdType numPts;
  vtkPolyData *input = this->GetInput();
  vtkPolyData *intermediate = vtkPolyData::New();
  vtkPolyData *output = this->GetOutput();

  vtkDebugMacro(<<"PolyDataMirror Execute begin");
  // Check input
  //
  if ( !input )
    {
    vtkErrorMacro(<<"No input data");
    return;
    }
  inPts = input->GetPoints();
  if ( !inPts )
    {
    vtkErrorMacro(<<"No input data");
    return;
    }
  numPts = inPts->GetNumberOfPoints();
  newPts = vtkPoints::New();
  newPts->SetNumberOfPoints(numPts);

  this->UpdateProgress (0);
  
  // Loop over all points
  double *p1,p2[3];
  for(int i=0; i<numPts; i++ )
  {
     p1 = inPts->GetPoint(i);
     p2[0] = this->MirrorXCoordinate ? -p1[0] : p1[0];
     p2[1] = this->MirrorYCoordinate ? -p1[1] : p1[1];
     p2[2] = this->MirrorZCoordinate ? -p1[2] : p1[2];
  	 newPts->SetPoint(i,p2);
     if(i%50 == 0) this->UpdateProgress (i/numPts); 
  }
  
  // setup the intermediate Polydata
  //
  intermediate->SetPoints(newPts);
  newPts->Delete();

	intermediate->SetVerts(input->GetVerts());
  intermediate->SetLines(input->GetLines());
  intermediate->SetPolys(input->GetPolys());
  intermediate->SetStrips(input->GetStrips());
	intermediate->Update();

  vtkPointData *pd=input->GetPointData(), *outPD=intermediate->GetPointData();
  outPD->PassData(pd);
  vtkCellData *cd=input->GetCellData(), *outCD=intermediate->GetCellData();
  outCD->PassData(cd);

  /*vtkPolyDataNormals *pdn = vtkPolyDataNormals::New();
  pdn->SetInput(intermediate);
  pdn->SplittingOff();
  pdn->ComputePointNormalsOn();   
  pdn->ComputeCellNormalsOff();
  pdn->SetFlipNormals(this->m_FlipNormals);
  pdn->Update();

	if(pdn->GetOutput()->GetNumberOfPoints() == 0)
	{*/
    output->DeepCopy(intermediate);
	/*}
	else
	{
    output->DeepCopy(pdn->GetOutput());
	}*/
  
  
  intermediate->Delete();
  //pdn->Delete();   

  this->UpdateProgress (1);
}

//----------------------------------------------------------------------------
void vtkMEDPolyDataMirror::PrintSelf(ostream& os, vtkIndent indent)
//----------------------------------------------------------------------------
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "Flip Normals: "      << (this->FlipNormals       ? "On\n" : "Off\n");
  os << indent << "MirrorXCoordinate: " << (this->MirrorXCoordinate ? "On\n" : "Off\n");
  os << indent << "MirrorYCoordinate: " << (this->MirrorYCoordinate ? "On\n" : "Off\n");
  os << indent << "MirrorZCoordinate: " << (this->MirrorZCoordinate ? "On\n" : "Off\n");
}

