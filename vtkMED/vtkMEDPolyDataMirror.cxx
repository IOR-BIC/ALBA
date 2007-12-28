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

vtkCxxRevisionMacro(vtkMEDPolyDataMirror, "$Revision: 1.2 $");
vtkStandardNewMacro(vtkMEDPolyDataMirror);

//----------------------------------------------------------------------------
vtkMEDPolyDataMirror::vtkMEDPolyDataMirror()
//----------------------------------------------------------------------------
{
  this->m_FlipNormals = 0;
  this->m_MirrorXCoordinate = 0;
  this->m_MirrorYCoordinate = 0;
  this->m_MirrorZCoordinate = 0;
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
     p2[0] = this->m_MirrorXCoordinate ? -p1[0] : p1[0];
     p2[1] = this->m_MirrorYCoordinate ? -p1[1] : p1[1];
     p2[2] = this->m_MirrorZCoordinate ? -p1[2] : p1[2];
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

  vtkPolyDataNormals *pdn = vtkPolyDataNormals::New();
  pdn->SetInput(intermediate);
  pdn->SplittingOff();
  pdn->ComputePointNormalsOn();   
  pdn->ComputeCellNormalsOff();
  pdn->SetFlipNormals(this->m_FlipNormals);
  pdn->Update();

	if(pdn->GetOutput()->GetNumberOfPoints() == 0)
	{
    output->DeepCopy(intermediate);
	}
	else
	{
    output->DeepCopy(pdn->GetOutput());
	}
  
  
  intermediate->Delete();
  pdn->Delete();   

  this->UpdateProgress (1);
}

//----------------------------------------------------------------------------
void vtkMEDPolyDataMirror::PrintSelf(ostream& os, vtkIndent indent)
//----------------------------------------------------------------------------
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "Flip Normals: "      << (this->m_FlipNormals       ? "On\n" : "Off\n");
  os << indent << "m_MirrorXCoordinate: " << (this->m_MirrorXCoordinate ? "On\n" : "Off\n");
  os << indent << "m_MirrorYCoordinate: " << (this->m_MirrorYCoordinate ? "On\n" : "Off\n");
  os << indent << "m_MirrorZCoordinate: " << (this->m_MirrorZCoordinate ? "On\n" : "Off\n");
}

