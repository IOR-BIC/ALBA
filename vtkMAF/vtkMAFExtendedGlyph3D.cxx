/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkMAFExtendedGlyph3D.cxx,v $
  Language:  C++
  Date:      $Date: 2010-04-06 15:06:49 $
  Version:   $Revision: 1.1.2.1 $

  Copyright (c) 1993-2002 Ken Martin, Will Schroeder, Bill Lorensen 
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkMAFExtendedGlyph3D.h"

#include "vtkCell.h"
#include "vtkDataSet.h"
#include "vtkFloatArray.h"
#include "vtkIdList.h"
#include "vtkIdTypeArray.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkTransform.h"
#include "vtkUnsignedCharArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"

vtkStandardNewMacro(vtkMAFExtendedGlyph3D);

// Construct object with scaling on, scaling mode is by scalar value,
// scale factor = 1.0, the range is (0,1), orient geometry is on, and
// orientation is by vector. Clamping and indexing are turned off. No
// initial sources are defined.
//------------------------------------------------------------------------------
vtkMAFExtendedGlyph3D::vtkMAFExtendedGlyph3D()
{
  this->Scaling = 0;
  this->ScalarVisibility = 0;
  this->ColorMode = VTK_COLOR_BY_SCALE;
  this->ScaleMode = VTK_SCALE_BY_SCALAR;
  this->ScaleFactor = 1.0;
  this->Range[0] = 0.0;
  this->Range[1] = 1.0;
  this->Orient = 1;
  this->VectorMode = VTK_USE_VECTOR;
  this->Clamping = 0;
  this->IndexMode = VTK_INDEXING_OFF;
	//At least one input, one source
	this->SetNumberOfInputPorts(2);
	this->GeneratePointIds = 0;
  this->PointIdsName = NULL;
  this->SetPointIdsName("InputPointIds");
  this->InputScalarsSelection = NULL;
  this->InputVectorsSelection = NULL;
  this->InputNormalsSelection = NULL;
}

//------------------------------------------------------------------------------
vtkMAFExtendedGlyph3D::~vtkMAFExtendedGlyph3D()
{
  if (this->PointIdsName)
    {
    delete []PointIdsName;
    }
  this->SetInputScalarsSelection(NULL);
  this->SetInputVectorsSelection(NULL);
  this->SetInputNormalsSelection(NULL);
}

//------------------------------------------------------------------------------
int vtkMAFExtendedGlyph3D::RequestData( vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
	// get the info objects
	vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
	vtkInformation *outInfo = outputVector->GetInformationObject(0);

	// Initialize some frequently used values.
	vtkDataSet  *input = vtkDataSet::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
	vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkPointData *pd;
  vtkDataArray *inScalars;
  vtkDataArray *inVectors;
  int requestedGhostLevel;
  unsigned char* inGhostLevels=0;
  vtkDataArray *inNormals, *sourceNormals = NULL;
  vtkIdType numPts, numSourcePts, numSourceCells, inPtId, i;
  int index;
  vtkPoints *sourcePts = NULL;
  vtkPoints *newPts;
  vtkDataArray *newScalars=NULL;
  vtkDataArray *newVectors=NULL;
  vtkDataArray *newNormals=NULL;
  double *x, *v = NULL, vNew[3], s = 0.0, vMag = 0.0, value;
  vtkTransform *trans = vtkTransform::New();
  vtkCell *cell;
  vtkIdList *cellPts;
  int npts;
  vtkIdList *pts;
  vtkIdType ptIncr, cellId;
  int haveVectors, haveNormals;
  double scalex,scaley,scalez, den;
  
  vtkPointData *outputPD = output->GetPointData();
  
  int numberOfSources = this->GetNumberOfSources();
  vtkPolyData *defaultSource = NULL;
  vtkIdTypeArray *pointIds=0;

  vtkDebugMacro(<<"Generating glyphs");

  pts = vtkIdList::New();
  pts->Allocate(VTK_CELL_SIZE);

  if (!input)
    {
    vtkErrorMacro(<<"No input");
    return 1;
    }

  pd = input->GetPointData();
  inScalars = pd->GetScalars(this->InputScalarsSelection);
  inVectors = pd->GetVectors(this->InputVectorsSelection);
  inNormals = pd->GetNormals(this->InputNormalsSelection);

  vtkDataArray* temp = 0;
  if (pd)
    {
    temp = pd->GetArray("vtkGhostLevels");
    }
  if ( (!temp) || (temp->GetDataType() != VTK_UNSIGNED_CHAR)
    || (temp->GetNumberOfComponents() != 1))
    {
    vtkDebugMacro("No appropriate ghost levels field available.");
    }
  else
    {
    inGhostLevels = ((vtkUnsignedCharArray*)temp)->GetPointer(0);
    }

  requestedGhostLevel = output->GetGhostLevel();
  
  
  numPts = input->GetNumberOfPoints();
  if (numPts < 1)
    {
    vtkDebugMacro(<<"No points to glyph!");
    pts->Delete();
    trans->Delete();
    return 1;
    }

  // Check input for consistency
  //
  if ( (den = this->Range[1] - this->Range[0]) == 0.0 )
    {
    den = 1.0;
    }
  if ( this->VectorMode != VTK_VECTOR_ROTATION_OFF &&
       ((this->VectorMode == VTK_USE_VECTOR && inVectors != NULL) ||
        (this->VectorMode == VTK_USE_NORMAL && inNormals != NULL)) )
    {
    haveVectors = 1;
    }
  else
    {
    haveVectors = 0;
    }

  if ( (this->IndexMode == VTK_INDEXING_BY_SCALAR && !inScalars) ||
       (this->IndexMode == VTK_INDEXING_BY_VECTOR &&
       ((!inVectors && this->VectorMode == VTK_USE_VECTOR) ||
        (!inNormals && this->VectorMode == VTK_USE_NORMAL))) )
    {
    if ( this->GetSource(0) == NULL )
      {
      vtkErrorMacro(<<"Indexing on but don't have data to index with");
      pts->Delete();
      trans->Delete();
      return 1;
      }
    else
      {
      vtkWarningMacro(<<"Turning indexing off: no data to index with");
      this->IndexMode = VTK_INDEXING_OFF;
      }
    }

  // Allocate storage for output PolyData
  //
  outputPD->CopyScalarsOff();
  outputPD->CopyVectorsOff();
  outputPD->CopyNormalsOff();

  if (!this->GetSource(0))
    {
    defaultSource = vtkPolyData::New();
    defaultSource->Allocate();
    vtkPoints *defaultPoints = vtkPoints::New();
    defaultPoints->Allocate(6);
    defaultPoints->InsertNextPoint(0, 0, 0);
    defaultPoints->InsertNextPoint(1, 0, 0);
    vtkIdType defaultPointIds[2];
    defaultPointIds[0] = 0;
    defaultPointIds[1] = 1;
    defaultSource->SetPoints(defaultPoints);
    defaultSource->InsertNextCell(VTK_LINE, 2, defaultPointIds);
    this->SetUpdateExtent(0, 1, 0);
    this->SetSource(defaultSource);
    defaultSource->Delete();
    defaultSource = NULL;
    defaultPoints->Delete();
    defaultPoints = NULL;
    }
  
  if ( this->IndexMode != VTK_INDEXING_OFF )
    {
    pd = NULL;
    numSourcePts = numSourceCells = 0;
    haveNormals = 1;
    for (numSourcePts=numSourceCells=i=0; i < numberOfSources; i++)
      {
      if ( this->GetSource(i) != NULL )
        {
        numSourcePts += this->GetSource(i)->GetNumberOfPoints();
        numSourceCells += this->GetSource(i)->GetNumberOfCells();
        if ( !(sourceNormals = this->GetSource(i)->GetPointData()->GetNormals()) )
          {
          haveNormals = 0;
          }
        }
      }
    }
  else
    {
    sourcePts = this->GetSource(0)->GetPoints();
    numSourcePts = sourcePts->GetNumberOfPoints();
    numSourceCells = this->GetSource(0)->GetNumberOfCells();

    sourceNormals = this->GetSource(0)->GetPointData()->GetNormals();
    if ( sourceNormals )
      {
      haveNormals = 1;
      }
    else
      {
      haveNormals = 0;
      }

    // Prepare to copy output.
    pd = this->GetSource(0)->GetPointData();
    outputPD->CopyAllocate(pd,numPts*numSourcePts);
    }

  newPts = vtkPoints::New();
  newPts->Allocate(numPts*numSourcePts);
  if ( this->GeneratePointIds )
    {
    pointIds = vtkIdTypeArray::New();
    pointIds->SetName(this->PointIdsName);
    pointIds->Allocate(numPts*numSourcePts);
    outputPD->AddArray(pointIds);
    }
  if ( this->ColorMode == VTK_COLOR_BY_SCALAR && inScalars )
    {
    newScalars = inScalars->NewInstance();
    newScalars->SetNumberOfComponents(inScalars->GetNumberOfComponents());
    newScalars->Allocate(inScalars->GetNumberOfComponents()*numPts*numSourcePts);
    newScalars->SetName(inScalars->GetName());
    }
  else if ( (this->ColorMode == VTK_COLOR_BY_SCALE) && inScalars)
    {
    newScalars = vtkFloatArray::New();
    newScalars->Allocate(numPts*numSourcePts);
    newScalars->SetName("GlyphScale");
    if (this->ScaleMode == VTK_SCALE_BY_SCALAR)
      {
      newScalars->SetName(inScalars->GetName());
      }
    }
  else if ( (this->ColorMode == VTK_COLOR_BY_VECTOR) && haveVectors)
    {
    newScalars = vtkFloatArray::New();
    newScalars->Allocate(numPts*numSourcePts);
    newScalars->SetName("VectorMagnitude");
    }
  if ( haveVectors )
    {
    newVectors = vtkFloatArray::New();
    newVectors->SetNumberOfComponents(3);
    newVectors->Allocate(3*numPts*numSourcePts);
    newVectors->SetName("GlyphVector");
    }
  if ( haveNormals )
    {
    newNormals = vtkFloatArray::New();
    newNormals->SetNumberOfComponents(3);
    newNormals->Allocate(3*numPts*numSourcePts);
    newNormals->SetName("Normals");
    }

  // Setting up for calls to PolyData::InsertNextCell()
  if (this->IndexMode != VTK_INDEXING_OFF )
    {
    output->Allocate(3*numPts*numSourceCells,numPts*numSourceCells);
    }
  else
    {
    output->Allocate(this->GetSource(0),3*numPts*numSourceCells,numPts*numSourceCells);
    }

  // Traverse all Input points, transforming Source points and copying
  // point attributes.
  //
  ptIncr=0;
  for (inPtId=0; inPtId < numPts; inPtId++)
    {
    
    if (inScalars && this->ScalarVisibility)
    {
      if (inScalars->GetComponent(inPtId, 0)==0)
        continue;
    }

    scalex = scaley = scalez = 1.0;
    if ( ! (inPtId % 10000) )
      {
      this->UpdateProgress ((double)inPtId/numPts);
      if (this->GetAbortExecute())
        {
        break;
        }
      }

    // Get the scalar and vector data
    if ( inScalars )
      {
      s = inScalars->GetComponent(inPtId, 0);
      if ( this->ScaleMode == VTK_SCALE_BY_SCALAR ||
           this->ScaleMode == VTK_DATA_SCALING_OFF )
        {
        scalex = scaley = scalez = s;
        }
      }
    
    if ( haveVectors )
      {
      if ( this->VectorMode == VTK_USE_NORMAL )
        {
        v = inNormals->GetTuple(inPtId);
        }
      else
        {
        v = inVectors->GetTuple(inPtId);
        }
      vMag = vtkMath::Norm(v);
      if ( this->ScaleMode == VTK_SCALE_BY_VECTORCOMPONENTS )
        {
        scalex = v[0];
        scaley = v[1];
        scalez = v[2];
        }
      else if ( this->ScaleMode == VTK_SCALE_BY_VECTOR )
        {
        scalex = scaley = scalez = vMag;
        }
      }
    
    // Clamp data scale if enabled
    if ( this->Clamping )
      {
      scalex = (scalex < this->Range[0] ? this->Range[0] :
                (scalex > this->Range[1] ? this->Range[1] : scalex));
      scalex = (scalex - this->Range[0]) / den;
      scaley = (scaley < this->Range[0] ? this->Range[0] :
                (scaley > this->Range[1] ? this->Range[1] : scaley));
      scaley = (scaley - this->Range[0]) / den;
      scalez = (scalez < this->Range[0] ? this->Range[0] :
                (scalez > this->Range[1] ? this->Range[1] : scalez));
      scalez = (scalez - this->Range[0]) / den;
      }
    
    // Compute index into table of glyphs
    if ( this->IndexMode == VTK_INDEXING_OFF )
      {
      index = 0;
      }
    else 
      {
      if ( this->IndexMode == VTK_INDEXING_BY_SCALAR )
        {
        value = s;
        }
      else
        {
        value = vMag;
        }
      
      index = (int) ((double)(value - this->Range[0]) * numberOfSources / den);
      index = (index < 0 ? 0 :
              (index >= numberOfSources ? (numberOfSources-1) : index));
      
      if ( this->GetSource(index) != NULL )
        {
        sourcePts = this->GetSource(index)->GetPoints();
        sourceNormals = this->GetSource(index)->GetPointData()->GetNormals();
        numSourcePts = sourcePts->GetNumberOfPoints();
        numSourceCells = this->GetSource(index)->GetNumberOfCells();
        }
      }
    
    // Make sure we're not indexing into empty glyph
    if ( this->GetSource(index) == NULL )
      {
      continue;
      }

    // Check ghost points.
    // If we are processing a piece, we do not want to duplicate 
    // glyphs on the borders.  The corrct check here is:
    // ghostLevel > 0.  I am leaving this over glyphing here because
    // it make a nice example (sphereGhost.tcl) to show the 
    // point ghost levels with the glyph filter.  I am not certain 
    // of the usefullness of point ghost levels over 1, but I will have
    // to think about it.
    if (inGhostLevels && inGhostLevels[inPtId] > requestedGhostLevel)
      {
      continue;
      }
    
    // Now begin copying/transforming glyph
    trans->Identity();
    
    // Copy all topology (transformation independent)
    for (cellId=0; cellId < numSourceCells; cellId++)
      {
      cell = this->GetSource(index)->GetCell(cellId);
      cellPts = cell->GetPointIds();
      npts = cellPts->GetNumberOfIds();
      for (pts->Reset(), i=0; i < npts; i++) 
        {
        pts->InsertId(i,cellPts->GetId(i) + ptIncr);
        }
      output->InsertNextCell(cell->GetCellType(),pts);
      }
    
    // translate Source to Input point
    x = input->GetPoint(inPtId);
    trans->Translate(x[0], x[1], x[2]);
    
    if ( haveVectors )
      {
      // Copy Input vector
      for (i=0; i < numSourcePts; i++) 
        {
        newVectors->InsertTuple(i+ptIncr, v);
        }
      if (this->Orient && (vMag > 0.0))
        {
        // if there is no y or z component
        if ( v[1] == 0.0 && v[2] == 0.0 )
          {
          if (v[0] < 0) //just flip x if we need to
            {
            trans->RotateWXYZ(180.0,0,1,0);
            }
          }
        else
          {
          vNew[0] = (v[0]+vMag) / 2.0;
          vNew[1] = v[1] / 2.0;
          vNew[2] = v[2] / 2.0;
          trans->RotateWXYZ((double)180.0,vNew[0],vNew[1],vNew[2]);
          }
        }
      }
    
    // determine scale factor from scalars if appropriate
    if ( inScalars )
      {
      // Copy scalar value
      if (this->ColorMode == VTK_COLOR_BY_SCALE)
        {
        for (i=0; i < numSourcePts; i++) 
          {
          newScalars->InsertTuple(i+ptIncr, &scalex); // = scaley = scalez
          }
        }
      else if (this->ColorMode == VTK_COLOR_BY_SCALAR)
        {
        for (i=0; i < numSourcePts; i++)
          {
          outputPD->CopyTuple(inScalars, newScalars, inPtId, ptIncr+i);
          }
        }
      }
    if (haveVectors && this->ColorMode == VTK_COLOR_BY_VECTOR)
      {
      for (i=0; i < numSourcePts; i++) 
        {
        newScalars->InsertTuple(i+ptIncr, &vMag);
        }
      }
    
    // scale data if appropriate
    if ( this->Scaling )
      {
      if ( this->ScaleMode == VTK_DATA_SCALING_OFF )
        {
        scalex = scaley = scalez = this->ScaleFactor;
        }
      else
        {
        scalex *= this->ScaleFactor;
        scaley *= this->ScaleFactor;
        scalez *= this->ScaleFactor;
        }
      
      if ( scalex == 0.0 )
        {
        scalex = 1.0e-10;
        }
      if ( scaley == 0.0 )
        {
        scaley = 1.0e-10;
        }
      if ( scalez == 0.0 )
        {
        scalez = 1.0e-10;
        }
      trans->Scale(scalex,scaley,scalez);
      }
    
    // multiply points and normals by resulting matrix
    trans->TransformPoints(sourcePts,newPts);
    
    if ( haveNormals )
      {
      trans->TransformNormals(sourceNormals,newNormals);
      }
    
    // Copy point data from source (if possible)
    if ( pd ) 
      {
      for (i=0; i < numSourcePts; i++)
        {
        outputPD->CopyData(pd,i,ptIncr+i);
        }
      }

    // If point ids are to be generated, do it here
    if ( this->GeneratePointIds )
      {
      for (i=0; i < numSourcePts; i++)
        {
        pointIds->InsertNextValue(inPtId);
        }
      }

    ptIncr += numSourcePts;
    } 
  
  // Update ourselves and release memory
  //
  output->SetPoints(newPts);
  newPts->Delete();

  if (newScalars)
    {
    outputPD->SetScalars(newScalars);
    newScalars->Delete();
    }

  if (newVectors)
    {
    outputPD->SetVectors(newVectors);
    newVectors->Delete();
    }

  if (newNormals)
    {
    outputPD->SetNormals(newNormals);
    newNormals->Delete();
    }
  if (pointIds)
  {
    pointIds->Delete();
  }

  output->Squeeze();
  trans->Delete();
  pts->Delete();

	return 1;
}

//----------------------------------------------------------------------------
// Since indexing determines size of outputs, EstimatedWholeMemorySize is
// truly an estimate.  Ignore Indexing (although for a best estimate we
// should average the size of the sources instead of using 0).
int vtkMAFExtendedGlyph3D::RequestInformation(vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
  if (this->GetInput() == NULL)
    {
    vtkErrorMacro("Missing input");
    return 1;
    }
	return 1;
}


// Set the number of source objects in the glyph table. This should be
// done prior to specifying more than one source.
//------------------------------------------------------------------------------
void vtkMAFExtendedGlyph3D::SetNumberOfSources(int num)
{
  // one more because input has index 0.
  this->SetNumberOfInputPorts(num+1);
}

//------------------------------------------------------------------------------
int vtkMAFExtendedGlyph3D::GetNumberOfSources()
{
  // one less because input has index 0.
  return this->GetNumberOfInputPorts() - 1;
}

// Specify a source object at a specified table location.
//------------------------------------------------------------------------------
void vtkMAFExtendedGlyph3D::SetSource(int id, vtkPolyData *pd)
{
  if (id < 0)
    {
    vtkErrorMacro("Bad index " << id << " for source.");
    return;
    }
  this->SetInputData(id + 1, pd);
}

// Get a pointer to a source object at a specified table location.
//------------------------------------------------------------------------------
vtkPolyData *vtkMAFExtendedGlyph3D::GetSource(int id)
{
  if ( id < 0 || id >= this->GetNumberOfSources() )
    {
    return NULL;
    }
  else
    {
    return (vtkPolyData *)this->GetInput(id+1);
    }
}

//------------------------------------------------------------------------------
void vtkMAFExtendedGlyph3D::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Generate Point Ids "
     << (this->GeneratePointIds ? "On\n" : "Off\n");

  os << indent << "PointIdsName: " << (this->PointIdsName ? this->PointIdsName
       : "(none)") << "\n";

  os << indent << "Color Mode: " << this->GetColorModeAsString() << endl;

  if ( this->GetNumberOfSources() < 2 )
    {
    if ( this->GetSource(0) != NULL )
      {
      os << indent << "Source: (" << this->GetSource(0) << ")\n";
      }
    else
      {
      os << indent << "Source: (none)\n";
      }
    }
  else
    {
    os << indent << "A table of " << this->GetNumberOfSources() << " glyphs has been defined\n";
    }

  os << indent << "Scaling: " << (this->Scaling ? "On\n" : "Off\n");
  
  os << indent << "Scale Mode: ";
  if ( this->ScaleMode == VTK_SCALE_BY_SCALAR )
    {
    os << "Scale by scalar\n";
    }
  else if ( this->ScaleMode == VTK_SCALE_BY_VECTOR )
    {
    os << "Scale by vector\n";
    }
  else
    {
    os << "Data scaling is turned off\n";
    }

  os << indent << "Scale Factor: " << this->ScaleFactor << "\n";
  os << indent << "Clamping: " << (this->Clamping ? "On\n" : "Off\n");
  os << indent << "Range: (" << this->Range[0] << ", " << this->Range[1] << ")\n";
  os << indent << "Orient: " << (this->Orient ? "On\n" : "Off\n");
  os << indent << "Orient Mode: " << (this->VectorMode == VTK_USE_VECTOR ?
                                       "Orient by vector\n" : "Orient by normal\n");
  os << indent << "Index Mode: ";
  if ( this->IndexMode == VTK_INDEXING_BY_SCALAR )
    {
    os << "Index by scalar value\n";
    }
  else if ( this->IndexMode == VTK_INDEXING_BY_VECTOR )
    {
    os << "Index by vector value\n";
    }
  else
    {
    os << "Indexing off\n";
    }
  os << indent << "InputScalarsSelection: " 
     << (this->InputScalarsSelection ? this->InputScalarsSelection : "(none)") << "\n";
  os << indent << "InputVectorsSelection: " 
     << (this->InputVectorsSelection ? this->InputVectorsSelection : "(none)") << "\n";
  os << indent << "InputNormalsSelection: " 
     << (this->InputNormalsSelection ? this->InputNormalsSelection : "(none)") << "\n";
}

//------------------------------------------------------------------------------
int vtkMAFExtendedGlyph3D::FillInputPortInformation(int, vtkInformation *info)
{
	info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");
	return 1;
}

//------------------------------------------------------------------------------
int	vtkMAFExtendedGlyph3D::RequestUpdateExtent( vtkInformation *request, vtkInformationVector **inputVector,	vtkInformationVector *outputVector)
{
	this->vtkPolyDataAlgorithm::RequestUpdateExtent(request, inputVector,	outputVector);
	
	vtkPolyData *outPd;

  if (this->GetInput() == NULL)
    {
    vtkErrorMacro("Missing input");
    return 0;
    }

 
  outPd = this->GetOutput();
  if (this->GetSource())
    {
    this->SetUpdateExtent(0, 1, 0);
    }
  this->SetUpdateExtent(outPd->GetPiece(),
                                    outPd->GetNumberOfPieces(),
                                    outPd->GetGhostLevel());
  
	return 1;
}
