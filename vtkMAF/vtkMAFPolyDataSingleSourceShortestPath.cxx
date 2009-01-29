/*=========================================================================

  Program:	 Visualization Toolkit
  Module:	 $RCSfile: vtkMAFPolyDataSingleSourceShortestPath.cxx,v $
  Language:  C++
  Date: 	 $Date: 2009-01-29 11:17:14 $
  Version:	 $Revision: 1.1.2.2 $

  This class is not mature enough to enter the official VTK release.
=========================================================================*/

#include "vtkMAFPolyDataSingleSourceShortestPath.h"
#include "vtkFloatArray.h"
#include "vtkObjectFactory.h"
#include "vtkMath.h"
#include "vtkIdList.h"
#include "vtkFloatArray.h"
#include "vtkIntArray.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkPointData.h"
#include "vtkCellArray.h"

vtkCxxRevisionMacro(vtkMAFPolyDataSingleSourceShortestPath, "$Revision: 1.1.2.2 $");
vtkStandardNewMacro(vtkMAFPolyDataSingleSourceShortestPath);

//----------------------------------------------------------------------------
vtkMAFPolyDataSingleSourceShortestPath::vtkMAFPolyDataSingleSourceShortestPath()
{
	this->IdList = vtkIdList::New();
	this->D 	 = vtkFloatArray::New();
	this->Pre	 = vtkIntArray::New();
	this->F 	 = vtkIntArray::New();
	this->S 	 = vtkIntArray::New();
	this->H 	 = vtkIntArray::New();
	this->P 	 = vtkIntArray::New();
	this->Hsize  = 0;
	this->StartVertex = 0;
	this->EndVertex   = 0;	
	this->StopWhenEndReached = 0;
	this->UseScalarWeights = 0;
	this->Adj = NULL;
	this->N = 0;
	this->AdjacencyGraphSize = 0;
}


vtkMAFPolyDataSingleSourceShortestPath::~vtkMAFPolyDataSingleSourceShortestPath()
{
	if (this->IdList)
		this->IdList->Delete();
	if (this->D)
		this->D->Delete();
	if (this->Pre)
		this->Pre->Delete();
	if (this->F)
		this->F->Delete();
	if (this->S)
		this->S->Delete();
	if (this->H)
		this->H->Delete();
	if (this->P)
		this->P->Delete();
	
	DeleteAdjacency();
}

unsigned long vtkMAFPolyDataSingleSourceShortestPath::GetMTime()
{
	unsigned long mTime=this->MTime.GetMTime();
	
	return mTime;
}

void vtkMAFPolyDataSingleSourceShortestPath::Execute()
{
	vtkPolyData *input = this->GetInput();
	vtkPolyData *output = this->GetOutput();
	
	vtkDebugMacro(<< "vtkMAFPolyDataSingleSourceShortestPath finding shortest path");
	
	init();
	
	ShortestPath(this->StartVertex, this->EndVertex);
	
	TraceShortestPath(input, output, this->StartVertex, this->EndVertex);
	
}

void vtkMAFPolyDataSingleSourceShortestPath::init()
{
	BuildAdjacency(this->GetInput());
	
	IdList->Reset();
	
	this->N = this->GetInput()->GetNumberOfPoints();
	
	this->D->SetNumberOfComponents(1);
	this->D->SetNumberOfTuples(this->N);
	this->Pre->SetNumberOfComponents(1);
	this->Pre->SetNumberOfTuples(this->N);
	this->F->SetNumberOfComponents(1);
	this->F->SetNumberOfTuples(this->N);
	this->S->SetNumberOfComponents(1);
	this->S->SetNumberOfTuples(this->N);
	this->P->SetNumberOfComponents(1);
	this->P->SetNumberOfTuples(this->N);
	
	// The heap has elements from 1 to n
	this->H->SetNumberOfComponents(1);
	this->H->SetNumberOfTuples(this->N+1);
	
	Hsize = 0;
}

void vtkMAFPolyDataSingleSourceShortestPath::DeleteAdjacency()
{
	const int npoints = this->AdjacencyGraphSize;
	
	if (this->Adj)
	{
		for (int i = 0; i < npoints; i++)
		{
			this->Adj[i]->Delete();
		}
		delete [] this->Adj;
	}
	this->Adj = NULL;
}

// The edge cost function should be implemented as a callback function to
// allow more advanced weighting
double vtkMAFPolyDataSingleSourceShortestPath::EdgeCost(vtkPolyData *pd, vtkIdType u, vtkIdType v)
{
	double p1[3];
	pd->GetPoint(u,p1);
	double p2[3];
	pd->GetPoint(v,p2);
	
	double w = sqrt(vtkMath::Distance2BetweenPoints(p1, p2));
	
	if (this->UseScalarWeights)
	{
		// Note this edge cost is not symmetric!
		vtkFloatArray *scalars = (vtkFloatArray*)pd->GetPointData()->GetScalars();
		//		float s1 = scalars->GetValue(u);
		float s2 = scalars->GetValue(v);
		
		if (s2)
			w  /= (s2*s2);
	}
	return w;
}


// This is probably a horribly inefficient way to do it.
void vtkMAFPolyDataSingleSourceShortestPath::BuildAdjacency(vtkPolyData *pd)
{
	int i;
	
	int npoints = pd->GetNumberOfPoints();
	int ncells = pd->GetNumberOfCells();
	
	DeleteAdjacency();
	
	this->Adj = new vtkIdList*[npoints];

	// Remember size, so it can be deleted again
	this->AdjacencyGraphSize = npoints;

	for (i = 0; i < npoints; i++)
	{
		this->Adj[i] = vtkIdList::New();
	}
	
	for (i = 0; i < ncells; i++)
	{
		// Possible types
		//		VTK_VERTEX, VTK_POLY_VERTEX, VTK_LINE, VTK_POLY_LINE,VTK_TRIANGLE, VTK_QUAD, VTK_POLYGON, or VTK_TRIANGLE_STRIP.
		
		vtkIdType ctype = pd->GetCellType(i);
		
		// Until now only handle polys and triangles
		// TODO: All types
		if (ctype == VTK_POLYGON || ctype == VTK_TRIANGLE || ctype == VTK_LINE)
		{
			vtkIdType *pts;
			vtkIdType npts;
			pd->GetCellPoints (i, npts, pts);
			
			vtkIdType u = pts[0];
			vtkIdType v = pts[npts-1];
			
			Adj[u]->InsertUniqueId(v);
			Adj[v]->InsertUniqueId(u);
			for (int j = 0; j < npts-1; j++)
			{
				vtkIdType u = pts[j];
				vtkIdType v = pts[j+1];
				Adj[u]->InsertUniqueId(v);
				Adj[v]->InsertUniqueId(u);
			}
		}
	}
}

void vtkMAFPolyDataSingleSourceShortestPath::TraceShortestPath(vtkPolyData *inPd, vtkPolyData *outPd,
															vtkIdType startv, vtkIdType endv)
{
	vtkPoints *points = vtkPoints::New();
	vtkCellArray *lines = vtkCellArray::New();
	
	// n is far to many. Adjusted later
	lines->InsertNextCell(this->N);
	
	// trace backward
	int npoints = 0;
	int v = endv;
	double pt[3];
	vtkIdType id;
	while (v != startv)
	{
		IdList->InsertNextId(v);
		
		inPd->GetPoint(v,pt);
		id = points->InsertNextPoint(pt);
		lines->InsertCellPoint(id);
		npoints++;
		
		v = this->Pre->GetValue(v);
	}
	IdList->InsertNextId(v);
	
	inPd->GetPoint(v,pt);
	id = points->InsertNextPoint(pt);
	lines->InsertCellPoint(id);
	npoints++;
				
	lines->UpdateCellCount(npoints);
	outPd->SetPoints(points);
	points->Delete();
	outPd->SetLines(lines);
	lines->Delete();
}


void vtkMAFPolyDataSingleSourceShortestPath::InitSingleSource(int startv)
{
	for (int v = 0; v < this->N; v++)
	{
		// d will be updated with first visit of vertex
		this->D->SetValue(v, -1);
		this->Pre->SetValue(v, -1);
		this->S->SetValue(v, 0);
		this->F->SetValue(v, 0);
	}
	
	this->D->SetValue(startv, 0);
}


void vtkMAFPolyDataSingleSourceShortestPath::Relax(int u, int v, double w)
{
	if (this->D->GetValue(v) > this->D->GetValue(u) + w)
	{
		this->D->SetValue(v, this->D->GetValue(u) + w);
		this->Pre->SetValue(v, u);
		
		HeapDecreaseKey(v);
	}
}

void vtkMAFPolyDataSingleSourceShortestPath::ShortestPath(int startv, int endv)
{
	int i, u, v;
	
	InitSingleSource(startv);
	
	HeapInsert(startv);
	this->F->SetValue(startv, 1);
	
	int stop = 0;
	while ((u = HeapExtractMin()) >= 0 && !stop)
	{
		// u is now in s since the shortest path to u is determined
		this->S->SetValue(u, 1);
		// remove u from the front set
		this->F->SetValue(u, 0);
		
		if (u == endv && StopWhenEndReached)
			stop = 1;
		
		// Update all vertices v adjacent to u
		for (i = 0; i < Adj[u]->GetNumberOfIds(); i++)
		{
			v = Adj[u]->GetId(i);
			
			// s is the set of vertices with determined shortest path...do not use them again
			if (!this->S->GetValue(v))
			{
				// Only relax edges where the end is not in s and edge is in the front set
				double w = EdgeCost(this->GetInput(), u, v);
				
				if (this->F->GetValue(v))
				{
					Relax(u, v, w);
				}
				// add edge v to front set
				else
				{
					this->F->SetValue(v, 1);
					this->D->SetValue(v, this->D->GetValue(u) + w);
					
					// Set Predecessor of v to be u
					this->Pre->SetValue(v, u);
					
					HeapInsert(v);
				}
			}
		}
	}
}


void vtkMAFPolyDataSingleSourceShortestPath::Heapify(int i)
{
	// left node
	int l = i * 2;
	
	// right node
	int r = i * 2 + 1;
	
	int smallest = -1;
	
	// The value of element v is d(v)
	// the heap stores the vertex numbers
	if (l <= Hsize && this->D->GetValue(this->H->GetValue(l)) < this->D->GetValue(this->H->GetValue(i)))
		smallest = l;
	else
		smallest = i;
	
	if (r <= Hsize && this->D->GetValue(this->H->GetValue(r))< this->D->GetValue(this->H->GetValue(smallest)))
		smallest = r;
	
	if (smallest != i)
	{
		int t = this->H->GetValue(i);
		
		this->H->SetValue(i, this->H->GetValue(smallest));
		
		// where is H(i)
		this->P->SetValue(this->H->GetValue(i), i);
		
		// H and p is kinda inverse
		this->H->SetValue(smallest, t);
		this->P->SetValue(t, smallest);
		
		Heapify(smallest);
	}
}

// Insert vertex v. Weight is given in d(v)
// H has indices 1..n
void vtkMAFPolyDataSingleSourceShortestPath::HeapInsert(int v)
{
	if (Hsize >= this->H->GetNumberOfTuples()-1)
		return;
	
	Hsize++;
	int i = Hsize;
	
	while (i > 1 && this->D->GetValue(this->H->GetValue(i/2)) > this->D->GetValue(v))
	{
		this->H->SetValue(i, this->H->GetValue(i/2));
		this->P->SetValue(this->H->GetValue(i), i);
		i /= 2;
	}
	// H and p is kinda inverse
	this->H->SetValue(i, v);
	this->P->SetValue(v, i);
}

int vtkMAFPolyDataSingleSourceShortestPath::HeapExtractMin()
{
	if (Hsize == 0)
		return -1;
	
	int minv = this->H->GetValue(1);
	this->P->SetValue(minv, -1);
	
	this->H->SetValue(1, this->H->GetValue(Hsize));
	this->P->SetValue(this->H->GetValue(1), 1);
	
	Hsize--;
	Heapify(1);
	
	return minv;
}

void vtkMAFPolyDataSingleSourceShortestPath::HeapDecreaseKey(int v)
{
	// where in H is vertex v
	int i = this->P->GetValue(v);
	if (i < 1 || i > Hsize)
		return;
	
	while (i > 1 && this->D->GetValue(this->H->GetValue(i/2)) > this->D->GetValue(v))
	{
		this->H->SetValue(i, this->H->GetValue(i/2));
		this->P->SetValue(this->H->GetValue(i), i);
		i /= 2;
	}
	
	// H and p is kinda inverse
	this->H->SetValue(i, v);
	this->P->SetValue(v, i);
}

void vtkMAFPolyDataSingleSourceShortestPath::PrintSelf(ostream& os, vtkIndent indent)
{
	vtkPolyDataToPolyDataFilter::PrintSelf(os,indent);

	// Add all members later...
}

double vtkMAFPolyDataSingleSourceShortestPath::GetPathLength()
{ 
  int i=0, size;
  size = GetOutput()->GetNumberOfPoints();
  double point1[3], point2[3];
  double length = 0.;
  for(; i < size; i++)
  {
    if(i==0) continue;

    GetOutput()->GetPoint(i-1, point1);
    GetOutput()->GetPoint(i, point2);
    length += sqrt(vtkMath::Distance2BetweenPoints(point1,point2));
  }

  return length;
}
