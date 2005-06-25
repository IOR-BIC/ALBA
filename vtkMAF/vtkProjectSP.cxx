/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkProjectSP.cxx
  Language:  C++
  Date:      2001/01
  Version:   1.1.1


Copyright (c) 1993-1998 Ken Martin, Will Schroeder, Bill Lorensen.

This software is copyrighted by Ken Martin, Will Schroeder and Bill Lorensen.
The following terms apply to all files associated with the software unless
explicitly disclaimed in individual files. This copyright specifically does
not apply to the related textbook "The Visualization Toolkit" ISBN
013199837-4 published by Prentice Hall which is covered by its own copyright.

The authors hereby grant permission to use, copy, and distribute this
software and its documentation for any purpose, provided that existing
copyright notices are retained in all copies and that this notice is included
verbatim in any distributions. Additionally, the authors grant permission to
modify this software and its documentation for any purpose, provided that
such modifications are not distributed without the explicit consent of the
authors and that existing copyright notices are retained in all copies. Some
of the algorithms implemented by this software are patented, observe all
applicable patent law.

IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN
"AS IS" BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.


=========================================================================*/
#include "vtkProjectSP.h"
//#include "vtkMath2.h"
#include "vtkObjectFactory.h"
#include "vtkStructuredPoints.h"
#include "vtkPointData.h"
#include "vtkDataArray.h"

vtkCxxRevisionMacro(vtkProjectSP, "$Revision: 1.3 $");
vtkStandardNewMacro(vtkProjectSP);

// Construct object to extract all of the input data.
//=========================================================================
vtkProjectSP::vtkProjectSP()
//=========================================================================
{
  this->ProjectionMode = VTK_PROJECT_FROM_X;
}

//=========================================================================
void vtkProjectSP::ExecuteInformation()
//=========================================================================
{
  vtkImageData *input=this->GetInput();
  vtkStructuredPoints *output=this->GetOutput();
  int dims[3], outDims[3], wholeExtent[6];
  
  if (this->GetInput() == NULL)
    {
    vtkErrorMacro("Missing input");
    return;
    }
  this->vtkStructuredPointsToStructuredPointsFilter::ExecuteInformation();

  input->GetWholeExtent( wholeExtent );
  dims[0] = wholeExtent[1] - wholeExtent[0] + 1;
  dims[1] = wholeExtent[3] - wholeExtent[2] + 1;
  dims[2] = wholeExtent[5] - wholeExtent[4] + 1;
  
  switch (this->ProjectionMode) {
  case VTK_PROJECT_FROM_X:
    outDims[0] = dims[1];
    outDims[1] = dims[2];
    outDims[2] = 1;    
    break;
  case VTK_PROJECT_FROM_Y:
    outDims[0] = dims[0];
    outDims[1] = dims[2];
    outDims[2] = 1;    
    break;
  case VTK_PROJECT_FROM_Z:
    outDims[0] = dims[0];
    outDims[1] = dims[1];
    outDims[2] = 1;
  }

  wholeExtent[0] = 0;
  wholeExtent[1] = outDims[0] - 1;
  wholeExtent[2] = 0;
  wholeExtent[3] = outDims[1] - 1;
  wholeExtent[4] = 0;
  wholeExtent[5] = outDims[2] - 1;
  
  
  output->SetWholeExtent( wholeExtent );
  output->SetUpdateExtent( wholeExtent );   // cosi funziona - Silvano & Robez


  vtkDebugMacro(<<"Whole Extent is " << wholeExtent[1] << " " << wholeExtent[3] << " " << wholeExtent[5]);
}

//=========================================================================
void vtkProjectSP::Execute()
//=========================================================================
{
  int dims[3], outDims[3];
  double origin[3], ar[3], outOrigin[3]={0.0,0.0,0.0}, outAR[3];

  int i, j, k, dim, idx, newIdx;
  int sliceSize, jOffset, kOffset;
  float I;

  vtkStructuredPoints *input=(vtkStructuredPoints *)this->GetInput();
  vtkStructuredPoints *output=(vtkStructuredPoints *)this->GetOutput();

  output->AllocateScalars();

  vtkPointData        *pd     =input->GetPointData();
  vtkPointData        *outPD  =output->GetPointData();
  vtkDataArray        *sc     =pd->GetScalars();
  vtkDataArray        *outSc  =outPD->GetScalars();

  input->GetDimensions(dims);
  input->GetOrigin(origin);
  input->GetSpacing(ar);

  switch (this->ProjectionMode) {
  case VTK_PROJECT_FROM_X:
    outDims[0] = dims[1];
    outDims[1] = dims[2];
    outDims[2] = 1;    
    outAR[0] = ar[1];
    outAR[1] = ar[2];
    outAR[2] = 1;
    break;
  case VTK_PROJECT_FROM_Y:
    outDims[0] = dims[0];
    outDims[1] = dims[2];
    outDims[2] = 1;    
    outAR[0] = ar[0];
    outAR[1] = ar[2];
    outAR[2] = 1;
    break;
  case VTK_PROJECT_FROM_Z:
    outDims[0] = dims[0];
    outDims[1] = dims[1];
    outDims[2] = 1;
    outAR[0] = ar[0];
    outAR[1] = ar[1];
    outAR[2] = 1;
  }
  
  output->SetScalarType(input->GetScalarType());
  output->SetNumberOfScalarComponents(input->GetNumberOfScalarComponents());
  output->SetDimensions(outDims);
  output->SetSpacing(outAR);
  output->SetOrigin(outOrigin);
  
  vtkDebugMacro( <<"Output Dimension are " << outDims[0] << " " << outDims[1] << " " << outDims[2] );

  dim=2;
  sliceSize = dims[0]*dims[1];

  //
  // Traverse input data and project points to output
  //
  newIdx = 0;
  
  switch (this->ProjectionMode) {
  case VTK_PROJECT_FROM_X:
    for ( k=0; k < dims[2]; k++ )
      {
	kOffset = k * sliceSize;
	for ( j=0; j < dims[1]; j++ )
	  {
	    jOffset = j * dims[0];
	    I = 0;  
	    for ( i=dims[0]-1; i >= 0 ; i-- )	    
	      {
		idx = i + jOffset + kOffset;		
		//I += sc->GetScalar(idx);						
		I += sc->GetTuple1(idx);						
	      }
	    //outSc->InsertScalar(newIdx++, vtkMath2::Round(I / (double) dims[0]));		
	    outSc->InsertTuple1(newIdx++, (I / (double) dims[0]));		
	  }
      }
    break;
  case VTK_PROJECT_FROM_Y:        
    for ( k=0; k < dims[2]; k++ )
      {
	kOffset = k * sliceSize;
	for ( i=0; i < dims[0]; i++ )
	  {	   
	    I = 0;
	    for ( j=dims[1]-1; j >= 0; j--  )
	      {
		jOffset = j * dims[0];
		idx = i + jOffset + kOffset;
		//I += sc->GetScalar(idx);
		I += sc->GetTuple1(idx);
	      }
	    //outSc->InsertScalar(newIdx++, vtkMath2::Round(I / (double) dims[0]));
	    outSc->InsertTuple1(newIdx++, (I / (double) dims[0]));
	  }
      }
    break;  
  case VTK_PROJECT_FROM_Z:        
    for ( j=0; j < dims[1]; j++ )
      {
	jOffset = j * dims[0];
	for ( i=0; i < dims[0]; i++ )
	  {	
	    I = 0;    
	    for ( k=dims[2]-1; k >= 0; k--  )
	      {
		kOffset = k * sliceSize;
		idx = i + jOffset + kOffset;
		//I += sc->GetScalar(idx);
		I += sc->GetTuple1(idx);
	      }
	    //outSc->InsertScalar(newIdx++, vtkMath2::Round(I / (double) dims[0]));
	    outSc->InsertTuple1(newIdx++, (I / (double) dims[0]));
	  }
      } 
    break;
  } 
}


//=========================================================================
void vtkProjectSP::PrintSelf(ostream& os, vtkIndent indent)
//=========================================================================
{
  vtkStructuredPointsToStructuredPointsFilter::PrintSelf(os,indent);

  os << indent << "ProjectionMode: " <<GetProjectionModeAsString() << "\n";
}




