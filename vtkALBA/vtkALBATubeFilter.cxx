/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkALBALandmarkCloudPolydataFilter.cxx,v $
Language:  C++
Date:      $Date: 2009-10-08 13:44:29 $
Version:   $Revision: 1.1.2.5 $
Authors:   Ken Martin, Will Schroeder, Bill Lorensen, Gianluigi Crimi
==========================================================================
Copyright (c) 2012
=========================================================================*/

#include "vtkALBATubeFilter.h"

#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkFloatArray.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkPolyLine.h"

vtkCxxRevisionMacro(vtkALBATubeFilter, "$Revision: 1.80 $");
vtkStandardNewMacro(vtkALBATubeFilter);

#define EPSILON 1e-8

// Construct object with radius 0.5, radius variation turned off, the number 
// of sides set to 3, and radius factor of 10.
vtkALBATubeFilter::vtkALBATubeFilter()
{
	this->Radius = 0.5;
	this->VaryRadius = VTK_VARY_RADIUS_OFF;
	this->NumberOfSides = 3;
	this->RadiusFactor = 10;

	this->DefaultNormal[0] = this->DefaultNormal[1] = 0.0;
	this->DefaultNormal[2] = 1.0;

	this->UseDefaultNormal = 0;
	this->SidesShareVertices = 1;
	this->Capping = 0;
	this->OnRatio = 1;
	this->Offset = 0;

	this->GenerateTCoords = VTK_TCOORDS_OFF;
	this->TextureLength = 1.0;
}

void vtkALBATubeFilter::Execute()
{
	vtkPolyData *input = this->GetInput();
	vtkPolyData *output = this->GetOutput();
	vtkPointData *pd = input->GetPointData();
	vtkPointData *outPD = output->GetPointData();
	vtkCellData *cd = input->GetCellData();
	vtkCellData *outCD = output->GetCellData();
	vtkCellArray *inLines = NULL;
	vtkDataArray *inNormals;
	vtkDataArray *inScalars = pd->GetScalars();
	vtkDataArray *inVectors = pd->GetVectors();

	vtkPoints *inPts;
	vtkIdType numPts = 0;
	vtkIdType numLines;
	vtkIdType numNewPts, numNewCells;
	vtkPoints *newPts;
	int deleteNormals = 0;
	vtkFloatArray *newNormals;
	vtkIdType i;
	double range[2], maxSpeed = 0;
	vtkCellArray *newStrips;
	vtkIdType npts = 0, *pts = NULL;
	vtkIdType offset = 0;
	vtkFloatArray *newTCoords = NULL;
	int abort = 0;
	vtkIdType inCellId;
	double oldRadius = 1.0;

	// Check input and initialize
	//
	vtkDebugMacro(<< "Creating tube");

	if (!(inPts = input->GetPoints()) ||
		(numPts = inPts->GetNumberOfPoints()) < 1 ||
		!(inLines = input->GetLines()) ||
		(numLines = inLines->GetNumberOfCells()) < 1)
	{
		return;
	}

	// Create the geometry and topology
	numNewPts = numPts * this->NumberOfSides;
	newPts = vtkPoints::New();
	newPts->Allocate(numNewPts);
	newNormals = vtkFloatArray::New();
	newNormals->SetName("TubeNormals");
	newNormals->SetNumberOfComponents(3);
	newNormals->Allocate(3 * numNewPts);
	newStrips = vtkCellArray::New();
	newStrips->Allocate(newStrips->EstimateSize(1, numNewPts));
	vtkCellArray *singlePolyline = vtkCellArray::New();

	// Point data: copy scalars, vectors, tcoords. Normals may be computed here.
	outPD->CopyNormalsOff();
	if ((this->GenerateTCoords == VTK_TCOORDS_FROM_SCALARS && inScalars) ||
		this->GenerateTCoords == VTK_TCOORDS_FROM_LENGTH ||
		this->GenerateTCoords == VTK_TCOORDS_FROM_NORMALIZED_LENGTH)
	{
		newTCoords = vtkFloatArray::New();
		newTCoords->SetNumberOfComponents(2);
		newTCoords->Allocate(numNewPts);
		outPD->CopyTCoordsOff();
	}
	outPD->CopyAllocate(pd, numNewPts);

	int generateNormals = 0;
	if (!(inNormals = pd->GetNormals()) || this->UseDefaultNormal)
	{
		deleteNormals = 1;
		inNormals = vtkFloatArray::New();
		inNormals->SetNumberOfComponents(3);
		inNormals->SetNumberOfTuples(numPts);

		if (this->UseDefaultNormal)
		{
			for (i = 0; i < numPts; i++)
			{
				inNormals->SetTuple(i, this->DefaultNormal);
			}
		}
		else
		{
			// Normal generation has been moved to lower in the function.
			// This allows each different polylines to share vertices, but have
			// their normals (and hence their tubes) calculated independently
			generateNormals = 1;
		}
	}

	// If varying width, get appropriate info.
	//
	if (inScalars)
	{
		inScalars->GetRange(range, 0);
		if ((range[1] - range[0]) == 0.0)
		{
			if (this->VaryRadius == VTK_VARY_RADIUS_BY_SCALAR)
			{
				vtkWarningMacro(<< "Scalar range is zero!");
			}
			range[1] = range[0] + 1.0;
		}
		if (this->VaryRadius == VTK_VARY_RADIUS_BY_ABSOLUTE_SCALAR)
		{
			// temporarily set the radius to 1.0 so that radius*scalar = scalar
			oldRadius = this->Radius;
			this->Radius = 1.0;
			if (range[0] < 0.0)
			{
				vtkWarningMacro(<< "Scalar values fall below zero when using absolute radius values!");
			}
		}
	}
	if (inVectors)
	{
		maxSpeed = inVectors->GetMaxNorm();
	}

	// Copy selected parts of cell data; certainly don't want normals
	//
	numNewCells = inLines->GetNumberOfCells() * this->NumberOfSides + 2;
	outCD->CopyNormalsOff();
	outPD->CopyAllocate(pd, numNewCells);

	//  Create points along each polyline that are connected into NumberOfSides
	//  triangle strips. Texture coordinates are optionally generated.
	//
	this->Theta = 2.0*vtkMath::Pi() / this->NumberOfSides;
	vtkPolyLine *lineNormalGenerator = vtkPolyLine::New();
	for (inCellId = 0, inLines->InitTraversal();
	inLines->GetNextCell(npts, pts) && !abort; inCellId++)
	{
		this->UpdateProgress((double)inCellId / numLines);
		abort = this->GetAbortExecute();
		
		vtkIdType cleanNpts = 0, *cleanPts = NULL;
		
		this->CleanLine(inPts, npts, pts, cleanNpts, cleanPts);
				
		if (cleanNpts < 2)
		{
			vtkWarningMacro(<< "Less than two points in line!");
			continue; //skip tubing this polyline
		}

		// If necessary calculate normals, each polyline calculates its
		// normals independently, avoiding conflicts at shared vertices.
		if (generateNormals)
		{
			singlePolyline->Reset(); //avoid instantiation
			singlePolyline->InsertNextCell(cleanNpts, cleanPts);
			if (!lineNormalGenerator->GenerateSlidingNormals(inPts, singlePolyline,
				inNormals))
			{
				vtkWarningMacro(<< "No normals for line!");
				continue; //skip tubing this polyline
			}
		}

		// Generate the points around the polyline. The tube is not stripped
		// if the polyline is bad.
		//
		if (!this->GeneratePoints(offset, cleanNpts, cleanPts, inPts, newPts, pd, outPD,
			newNormals, inScalars, range, inVectors,
			maxSpeed, inNormals))
		{
			vtkWarningMacro(<< "Could not generate points!");
			continue; //skip tubing this polyline
		}

		// Generate the strips for this polyline (including caps)
		//
		this->GenerateStrips(offset, cleanNpts, cleanPts, inCellId, cd, outCD, newStrips);

		// Generate the texture coordinates for this polyline
		//
		if (newTCoords)
		{
			this->GenerateTextureCoords(offset, cleanNpts, cleanPts, inPts, inScalars, newTCoords);
		}

		// Compute the new offset for the next polyline
		offset = this->ComputeOffset(offset, cleanNpts);

		delete[] cleanPts;

	}//for all polylines

	singlePolyline->Delete();

	// reset the radius to ite orginal value if necessary
	if (this->VaryRadius == VTK_VARY_RADIUS_BY_ABSOLUTE_SCALAR)
	{
		this->Radius = oldRadius;
	}

	// Update ourselves
	//
	if (deleteNormals)
	{
		inNormals->Delete();
	}

	if (newTCoords)
	{
		outPD->SetTCoords(newTCoords);
		newTCoords->Delete();
	}

	output->SetPoints(newPts);
	newPts->Delete();

	output->SetStrips(newStrips);
	newStrips->Delete();

	outPD->SetNormals(newNormals);
	newNormals->Delete();
	lineNormalGenerator->Delete();

	output->Squeeze();
}


//----------------------------------------------------------------------------
void vtkALBATubeFilter::CleanLine(vtkPoints *inPts, vtkIdType npts, vtkIdType *pts, vtkIdType &cleanNpts, vtkIdType *&cleanPts)
{
	cleanPts = new vtkIdType[npts];
	double lastPoint[3];
	
	inPts->GetPoint(pts[0], lastPoint);
	cleanPts[0] = pts[0];
	cleanNpts = 1;

	for (vtkIdType i = 1; i < npts; i++)
	{
		double *currPoint = inPts->GetPoint(i);
		double dist = vtkMath::Distance2BetweenPoints(lastPoint, currPoint);
		if (dist > EPSILON)
		{
			cleanPts[cleanNpts] = pts[i];
			cleanNpts++;
			inPts->GetPoint(pts[i], lastPoint);
		}
	}
}

int vtkALBATubeFilter::GeneratePoints(vtkIdType offset,
	vtkIdType npts, vtkIdType *pts,
	vtkPoints *inPts, vtkPoints *newPts,
	vtkPointData *pd, vtkPointData *outPD,
	vtkFloatArray *newNormals,
	vtkDataArray *inScalars, double range[2],
	vtkDataArray *inVectors, double maxSpeed,
	vtkDataArray *inNormals)
{
	vtkIdType j;
	int i, k;
	double p[3];
	double pNext[3];
	double sNext[3];
	double sPrev[3];
	double startCapNorm[3], endCapNorm[3];
	double n[3];
	double s[3];
	double bevelAngle;
	double w[3];
	double nP[3];
	double sFactor = 1.0;
	double normal[3];
	vtkIdType ptId = offset;

	// Use "averaged" segment to create beveled effect. 
	// Watch out for first and last points.
	//
	for (j = 0; j < npts; j++)
	{
		if (j == 0) //first point
		{
			inPts->GetPoint(pts[0], p);
			inPts->GetPoint(pts[1], pNext);
			for (i = 0; i<3; i++)
			{
				sNext[i] = pNext[i] - p[i];
				sPrev[i] = sNext[i];
				startCapNorm[i] = -sPrev[i];
			}
			vtkMath::Normalize(startCapNorm);
		}
		else if (j == (npts - 1)) //last point
		{
			for (i = 0; i<3; i++)
			{
				sPrev[i] = sNext[i];
				p[i] = pNext[i];
				endCapNorm[i] = sNext[i];
			}
			vtkMath::Normalize(endCapNorm);
		}
		else
		{
			for (i = 0; i<3; i++)
			{
				p[i] = pNext[i];
			}
			inPts->GetPoint(pts[j + 1], pNext);
			for (i = 0; i<3; i++)
			{
				sPrev[i] = sNext[i];
				sNext[i] = pNext[i] - p[i];
			}
		}

		inNormals->GetTuple(pts[j], n);

		if (vtkMath::Normalize(sNext) == 0.0)
		{
			vtkWarningMacro(<< "Coincident points!");
			return 0;
		}

		for (i = 0; i<3; i++)
		{
			s[i] = (sPrev[i] + sNext[i]) / 2.0; //average vector
		}
		// if s is zero then just use sPrev cross n
		if (vtkMath::Normalize(s) == 0.0)
		{
			vtkDebugMacro(<< "Using alternate bevel vector");
			vtkMath::Cross(sPrev, n, s);
			if (vtkMath::Normalize(s) == 0.0)
			{
				vtkDebugMacro(<< "Using alternate bevel vector");
			}
		}

		if ((bevelAngle = vtkMath::Dot(sNext, sPrev)) > 1.0)
		{
			bevelAngle = 1.0;
		}
		if (bevelAngle < -1.0)
		{
			bevelAngle = -1.0;
		}
		bevelAngle = acos((double)bevelAngle) / 2.0; //(0->90 degrees)
		if ((bevelAngle = cos(bevelAngle)) == 0.0)
		{
			bevelAngle = 1.0;
		}

		bevelAngle = this->Radius / bevelAngle; //keep tube constant radius

		vtkMath::Cross(s, n, w);
		if (vtkMath::Normalize(w) == 0.0)
		{
			vtkWarningMacro(<< "Bad normal s = " << s[0] << " " << s[1] << " " << s[2]
				<< " n = " << n[0] << " " << n[1] << " " << n[2]);
			return 0;
		}

		vtkMath::Cross(w, s, nP); //create orthogonal coordinate system
		vtkMath::Normalize(nP);

		// Compute a scale factor based on scalars or vectors
		if (inScalars && this->VaryRadius == VTK_VARY_RADIUS_BY_SCALAR)
		{
			sFactor = 1.0 + ((this->RadiusFactor - 1.0) *
				(inScalars->GetComponent(pts[j], 0) - range[0])
				/ (range[1] - range[0]));
		}
		else if (inVectors && this->VaryRadius == VTK_VARY_RADIUS_BY_VECTOR)
		{
			sFactor =
				sqrt((double)maxSpeed / vtkMath::Norm(inVectors->GetTuple(pts[j])));
			if (sFactor > this->RadiusFactor)
			{
				sFactor = this->RadiusFactor;
			}
		}
		else if (inScalars &&
			this->VaryRadius == VTK_VARY_RADIUS_BY_ABSOLUTE_SCALAR)
		{
			sFactor = inScalars->GetComponent(pts[j], 0);
			if (sFactor < 0.0)
			{
				vtkWarningMacro(<< "Scalar value less than zero, skipping line");
				return 0;
			}
		}

		//create points around line
		if (this->SidesShareVertices)
		{
			for (k = 0; k < this->NumberOfSides; k++)
			{
				for (i = 0; i<3; i++)
				{
					normal[i] = w[i] * cos((double)k*this->Theta) +
						nP[i] * sin((double)k*this->Theta);
					s[i] = p[i] + this->Radius * sFactor * normal[i];
				}
				newPts->InsertPoint(ptId, s);
				newNormals->InsertTuple(ptId, normal);
				outPD->CopyData(pd, pts[j], ptId);
				ptId++;
			}//for each side
		}
		else
		{
			double n_left[3], n_right[3];
			for (k = 0; k < this->NumberOfSides; k++)
			{
				for (i = 0; i<3; i++)
				{
					// Create duplicate vertices at each point
					// and adjust the associated normals so that they are
					// oriented with the facets. This preserves the tube's
					// polygonal appearance, as if by flat-shading around the tube,
					// while still allowing smooth (gouraud) shading along the
					// tube as it bends.
					normal[i] = w[i] * cos((double)(k + 0.0)*this->Theta) +
						nP[i] * sin((double)(k + 0.0)*this->Theta);
					n_right[i] = w[i] * cos((double)(k - 0.5)*this->Theta) +
						nP[i] * sin((double)(k - 0.5)*this->Theta);
					n_left[i] = w[i] * cos((double)(k + 0.5)*this->Theta) +
						nP[i] * sin((double)(k + 0.5)*this->Theta);
					s[i] = p[i] + this->Radius * sFactor * normal[i];
				}
				newPts->InsertPoint(ptId, s);
				newNormals->InsertTuple(ptId, n_right);
				outPD->CopyData(pd, pts[j], ptId);
				newPts->InsertPoint(ptId + 1, s);
				newNormals->InsertTuple(ptId + 1, n_left);
				outPD->CopyData(pd, pts[j], ptId + 1);
				ptId += 2;
			}//for each side
		}//else separate vertices
	}//for all points in polyline

	 //Produce end points for cap. They are placed at tail end of points.
	if (this->Capping)
	{
		int numCapSides = this->NumberOfSides;
		int capIncr = 1;
		if (!this->SidesShareVertices)
		{
			numCapSides = 2 * this->NumberOfSides;
			capIncr = 2;
		}

		//the start cap
		for (k = 0; k < numCapSides; k += capIncr)
		{
			newPts->GetPoint(offset + k, s);
			newPts->InsertPoint(ptId, s);
			newNormals->InsertTuple(ptId, startCapNorm);
			outPD->CopyData(pd, pts[0], ptId);
			ptId++;
		}
		//the end cap
		int endOffset = offset + (npts - 1)*this->NumberOfSides;
		if (!this->SidesShareVertices)
		{
			endOffset = offset + 2 * (npts - 1)*this->NumberOfSides;
		}
		for (k = 0; k < numCapSides; k += capIncr)
		{
			newPts->GetPoint(endOffset + k, s);
			newPts->InsertPoint(ptId, s);
			newNormals->InsertTuple(ptId, endCapNorm);
			outPD->CopyData(pd, pts[npts - 1], ptId);
			ptId++;
		}
	}//if capping

	return 1;
}

void vtkALBATubeFilter::GenerateStrips(vtkIdType offset, vtkIdType npts,
	vtkIdType* vtkNotUsed(pts),
	vtkIdType inCellId,
	vtkCellData *cd, vtkCellData *outCD,
	vtkCellArray *newStrips)
{
	vtkIdType i, outCellId;
	int k;
	int i1, i2, i3;

	if (this->SidesShareVertices)
	{
		for (k = this->Offset; k<(this->NumberOfSides + this->Offset);
		k += this->OnRatio)
		{
			i1 = k % this->NumberOfSides;
			i2 = (k + 1) % this->NumberOfSides;
			outCellId = newStrips->InsertNextCell(npts * 2);
			outCD->CopyData(cd, inCellId, outCellId);
			for (i = 0; i < npts; i++)
			{
				i3 = i*this->NumberOfSides;
				newStrips->InsertCellPoint(offset + i2 + i3);
				newStrips->InsertCellPoint(offset + i1 + i3);
			}
		} //for each side of the tube
	}
	else
	{
		for (k = this->Offset; k<(this->NumberOfSides + this->Offset);
		k += this->OnRatio)
		{
			i1 = 2 * (k % this->NumberOfSides) + 1;
			i2 = 2 * ((k + 1) % this->NumberOfSides);
			outCellId = newStrips->InsertNextCell(npts * 2);
			outCD->CopyData(cd, inCellId, outCellId);
			for (i = 0; i < npts; i++)
			{
				i3 = i * 2 * this->NumberOfSides;
				newStrips->InsertCellPoint(offset + i2 + i3);
				newStrips->InsertCellPoint(offset + i1 + i3);
			}
		} //for each side of the tube
	}

	// Take care of capping. The caps are n-sided polygons that can be
	// easily triangle stripped.
	if (this->Capping)
	{
		vtkIdType startIdx = offset + npts*this->NumberOfSides;
		vtkIdType idx;

		if (!this->SidesShareVertices)
		{
			startIdx = offset + 2 * npts*this->NumberOfSides;
		}

		//The start cap
		outCellId = newStrips->InsertNextCell(this->NumberOfSides);
		outCD->CopyData(cd, inCellId, outCellId);
		newStrips->InsertCellPoint(startIdx);
		newStrips->InsertCellPoint(startIdx + 1);
		for (i1 = this->NumberOfSides - 1, i2 = 2, k = 0; k<(this->NumberOfSides - 2); k++)
		{
			if ((k % 2))
			{
				idx = startIdx + i2;
				newStrips->InsertCellPoint(idx);
				i2++;
			}
			else
			{
				idx = startIdx + i1;
				newStrips->InsertCellPoint(idx);
				i1--;
			}
		}

		//The end cap - reversed order to be consistent with normal
		startIdx += this->NumberOfSides;
		outCellId = newStrips->InsertNextCell(this->NumberOfSides);
		outCD->CopyData(cd, inCellId, outCellId);
		newStrips->InsertCellPoint(startIdx);
		newStrips->InsertCellPoint(startIdx + this->NumberOfSides - 1);
		for (i1 = this->NumberOfSides - 2, i2 = 1, k = 0; k<(this->NumberOfSides - 2); k++)
		{
			if ((k % 2))
			{
				idx = startIdx + i1;
				newStrips->InsertCellPoint(idx);
				i1--;
			}
			else
			{
				idx = startIdx + i2;
				newStrips->InsertCellPoint(idx);
				i2++;
			}
		}
	}
}

void vtkALBATubeFilter::GenerateTextureCoords(vtkIdType offset,
	vtkIdType npts, vtkIdType *pts,
	vtkPoints *inPts,
	vtkDataArray *inScalars,
	vtkFloatArray *newTCoords)
{
	vtkIdType i;
	int k;
	double tc = 0.0;

	int numSides = this->NumberOfSides;
	if (!this->SidesShareVertices)
	{
		numSides = 2 * this->NumberOfSides;
	}

	double s0, s;
	//The first texture coordinate is always 0.
	for (k = 0; k < numSides; k++)
	{
		newTCoords->InsertTuple2(offset + k, 0.0, 0.0);
	}
	if (this->GenerateTCoords == VTK_TCOORDS_FROM_SCALARS)
	{
		s0 = inScalars->GetTuple1(pts[0]);
		for (i = 1; i < npts; i++)
		{
			s = inScalars->GetTuple1(pts[i]);
			tc = (s - s0) / this->TextureLength;
			for (k = 0; k < numSides; k++)
			{
				newTCoords->InsertTuple2(offset + i*numSides + k, tc, 0.0);
			}
		}
	}
	else if (this->GenerateTCoords == VTK_TCOORDS_FROM_LENGTH)
	{
		double xPrev[3], x[3], len = 0.0;
		inPts->GetPoint(pts[0], xPrev);
		for (i = 1; i < npts; i++)
		{
			inPts->GetPoint(pts[i], x);
			len += sqrt(vtkMath::Distance2BetweenPoints(x, xPrev));
			tc = len / this->TextureLength;
			for (k = 0; k < numSides; k++)
			{
				newTCoords->InsertTuple2(offset + i*numSides + k, tc, 0.0);
			}
			xPrev[0] = x[0]; xPrev[1] = x[1]; xPrev[2] = x[2];
		}
	}
	else if (this->GenerateTCoords == VTK_TCOORDS_FROM_NORMALIZED_LENGTH)
	{
		double xPrev[3], x[3], length = 0.0, len = 0.0;
		inPts->GetPoint(pts[0], xPrev);
		for (i = 1; i < npts; i++)
		{
			inPts->GetPoint(pts[i], x);
			length += sqrt(vtkMath::Distance2BetweenPoints(x, xPrev));
			xPrev[0] = x[0]; xPrev[1] = x[1]; xPrev[2] = x[2];
		}

		inPts->GetPoint(pts[0], xPrev);
		for (i = 1; i < npts; i++)
		{
			inPts->GetPoint(pts[i], x);
			len += sqrt(vtkMath::Distance2BetweenPoints(x, xPrev));
			tc = len / length;
			for (k = 0; k < numSides; k++)
			{
				newTCoords->InsertTuple2(offset + i * 2 + k, tc, 0.0);
			}
			xPrev[0] = x[0]; xPrev[1] = x[1]; xPrev[2] = x[2];
		}
	}

	// Capping, set the endpoints as appropriate
	if (this->Capping)
	{
		int ik;
		vtkIdType startIdx = offset + npts*numSides;

		//start cap
		for (ik = 0; ik < this->NumberOfSides; ik++)
		{
			newTCoords->InsertTuple2(startIdx + ik, 0.0, 0.0);
		}

		//end cap
		for (ik = 0; ik < this->NumberOfSides; ik++)
		{
			newTCoords->InsertTuple2(startIdx + this->NumberOfSides + ik, tc, 0.0);
		}
	}
}

// Compute the number of points in this tube
vtkIdType vtkALBATubeFilter::ComputeOffset(vtkIdType offset, vtkIdType npts)
{
	if (this->SidesShareVertices)
	{
		offset += this->NumberOfSides * npts;
	}
	else
	{
		offset += 2 * this->NumberOfSides * npts; //points are duplicated
	}

	if (this->Capping)
	{
		offset += 2 * this->NumberOfSides; //cap points are duplicated
	}

	return offset;
}

// Description:
// Return the method of varying tube radius descriptive character string.
const char *vtkALBATubeFilter::GetVaryRadiusAsString(void)
{
	if (this->VaryRadius == VTK_VARY_RADIUS_OFF)
	{
		return "VaryRadiusOff";
	}
	else if (this->VaryRadius == VTK_VARY_RADIUS_BY_SCALAR)
	{
		return "VaryRadiusByScalar";
	}
	else if (this->VaryRadius == VTK_VARY_RADIUS_BY_ABSOLUTE_SCALAR)
	{
		return "VaryRadiusByAbsoluteScalar";
	}
	else
	{
		return "VaryRadiusByVector";
	}
}

// Description:
// Return the method of generating the texture coordinates.
const char *vtkALBATubeFilter::GetGenerateTCoordsAsString(void)
{
	if (this->GenerateTCoords == VTK_TCOORDS_OFF)
	{
		return "GenerateTCoordsOff";
	}
	else if (this->GenerateTCoords == VTK_TCOORDS_FROM_SCALARS)
	{
		return "GenerateTCoordsFromScalar";
	}
	else if (this->GenerateTCoords == VTK_TCOORDS_FROM_LENGTH)
	{
		return "GenerateTCoordsFromLength";
	}
	else
	{
		return "GenerateTCoordsFromNormalizedLength";
	}
}

void vtkALBATubeFilter::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);

	os << indent << "Radius: " << this->Radius << "\n";
	os << indent << "Vary Radius: " << this->GetVaryRadiusAsString() << endl;
	os << indent << "Radius Factor: " << this->RadiusFactor << "\n";
	os << indent << "Number Of Sides: " << this->NumberOfSides << "\n";
	os << indent << "On Ratio: " << this->OnRatio << "\n";
	os << indent << "Offset: " << this->Offset << "\n";

	os << indent << "Use Default Normal: "
		<< (this->UseDefaultNormal ? "On\n" : "Off\n");
	os << indent << "Sides Share Vertices: "
		<< (this->SidesShareVertices ? "On\n" : "Off\n");
	os << indent << "Default Normal: " << "( " << this->DefaultNormal[0] <<
		", " << this->DefaultNormal[1] << ", " << this->DefaultNormal[2] <<
		" )\n";
	os << indent << "Capping: " << (this->Capping ? "On\n" : "Off\n");
	os << indent << "Generate TCoords: "
		<< this->GetGenerateTCoordsAsString() << endl;
	os << indent << "Texture Length: " << this->TextureLength << endl;
}
