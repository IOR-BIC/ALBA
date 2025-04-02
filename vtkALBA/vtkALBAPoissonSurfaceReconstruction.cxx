/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAPoissonSurfaceReconstruction
 Authors: Fuli Wu
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include <windows.h>
#include "vtkALBAPoissonSurfaceReconstruction.h"

#include "vtkObjectFactory.h"
#include "vtkFloatArray.h"
#include "vtkMath.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkStreamingDemandDrivenPipeline.h"


vtkStandardNewMacro(vtkALBAPoissonSurfaceReconstruction);

vtkDataSet* vtk_psr_input;
vtkPolyData* vtk_psr_output;

//----------------------------------------------------------------------------
vtkALBAPoissonSurfaceReconstruction::vtkALBAPoissonSurfaceReconstruction()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
vtkALBAPoissonSurfaceReconstruction::~vtkALBAPoissonSurfaceReconstruction()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
int vtkALBAPoissonSurfaceReconstruction::RequestData( vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector)
//----------------------------------------------------------------------------
{
	// get the info objects
	vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
	vtkInformation *outInfo = outputVector->GetInformationObject(0);

	// Initialize some frequently used values.
	vtkDataSet  *input = vtkDataSet::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
	vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));
	  

  // ghost cell stuff
  unsigned char  *cellGhostLevels = NULL;

  // make sure output is initialized
  // create some points for the output
  {
  vtkPoints *points = vtkPoints::New();
  output->SetPoints(points);
  points->Delete();
  }

  {
  vtkCellArray *polys = vtkCellArray::New();
  output->SetPolys(polys);
  polys->Delete();
  }

  vtk_psr_input = input;
  vtk_psr_output = output;

  PSR_main();

	return 1;
}

//----------------------------------------------------------------------------
int vtkALBAPoissonSurfaceReconstruction::RequestInformation(vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector)
//----------------------------------------------------------------------------
{
  if (this->GetInput() == NULL)
    {
    vtkErrorMacro("No Input");
    }
	return 1;
}

//----------------------------------------------------------------------------
void vtkALBAPoissonSurfaceReconstruction::PrintSelf(ostream& os, vtkIndent indent)
//----------------------------------------------------------------------------
{
  this->Superclass::PrintSelf(os,indent);
}

int vtkALBAPoissonSurfaceReconstruction::FillInputPortInformation(int, vtkInformation *info)
{
	info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");
	return 1;
}

//----------------------------------------------------------------------------
int	vtkALBAPoissonSurfaceReconstruction::RequestUpdateExtent( vtkInformation *request, vtkInformationVector **inputVector,	vtkInformationVector *outputVector)
{
	this->vtkPolyDataAlgorithm::RequestUpdateExtent(request, inputVector,	outputVector);

  int piece, numPieces, ghostLevels;
  
  if (this->GetInput() == NULL)
    {
    vtkErrorMacro("No Input");
    return 1;
    }
  piece = GetOutput()->GetPiece();
  numPieces = GetOutput()->GetNumberOfPieces();
  ghostLevels = GetOutput()->GetGhostLevel();
  
  if (numPieces > 1)
    {
    ++ghostLevels;
    }

	// Aggiorna l'extent utilizzando vtkStreamingDemandDrivenPipeline
	vtkInformation* outInfo = this->GetOutputInformation(0);
    outInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(),piece, numPieces, ghostLevels); 
	return 1;
}





//////////////////
// FunctionData //
//////////////////
template<int Degree,class Real>
const int FunctionData<Degree,Real>::DOT_FLAG=1;
template<int Degree,class Real>
const int FunctionData<Degree,Real>::D_DOT_FLAG=2;
template<int Degree,class Real>
const int FunctionData<Degree,Real>::D2_DOT_FLAG=4;
template<int Degree,class Real>
const int FunctionData<Degree,Real>::VALUE_FLAG=1;
template<int Degree,class Real>
const int FunctionData<Degree,Real>::D_VALUE_FLAG=2;

template<int Degree,class Real>
FunctionData<Degree,Real>::FunctionData(void){
	dotTable=dDotTable=d2DotTable=NULL;
	valueTables=dValueTables=NULL;
	res=0;
}

template<int Degree,class Real>
FunctionData<Degree,Real>::~FunctionData(void){
	if(res){
		if(  dotTable){delete[]   dotTable;}
		if( dDotTable){delete[]  dDotTable;}
		if(d2DotTable){delete[] d2DotTable;}
		if( valueTables){delete[]  valueTables;}
		if(dValueTables){delete[] dValueTables;}
    if(baseFunctions){delete[] baseFunctions;}
	}
	dotTable=dDotTable=d2DotTable=NULL;
	valueTables=dValueTables=NULL;
	res=0;
}

template<int Degree,class Real>
void FunctionData<Degree,Real>::Set(const int& maxDepth,const PPolynomial<Degree>& F,const int& normalize,const int& useDotRatios){
	this->normalize=normalize;
	this->useDotRatios=useDotRatios;

	depth=maxDepth;
	res=BinaryNode<double>::CumulativeCenterCount(depth);
	res2=(1<<(depth+1))+1;
	baseFunctions=new PPolynomial<Degree+1>[res];
	// Scale the function so that it has:
	// 0] Value 1 at 0
	// 1] Integral equal to 1
	// 2] Square integral equal to 1
	switch(normalize){
		case 2:
			baseFunction=F/sqrt((F*F).integral(F.polys[0].start,F.polys[F.polyCount-1].start));
			break;
		case 1:
			baseFunction=F/F.integral(F.polys[0].start,F.polys[F.polyCount-1].start);
			break;
		default:
			baseFunction=F/F(0);
	}
	dBaseFunction=baseFunction.derivative();
	double c1,w1;
	for(int i=0;i<res;i++){
		BinaryNode<double>::CenterAndWidth(i,c1,w1);
		baseFunctions[i]=baseFunction.scale(w1).shift(c1);
		// Scale the function so that it has L2-norm equal to one
		switch(normalize){
			case 2:
				baseFunctions[i]/=sqrt(w1);
				break;
			case 1:
				baseFunctions[i]/=w1;
				break;
		}
	}
}
template<int Degree,class Real>
void FunctionData<Degree,Real>::SetDotTables(const int& flags){
	ClearDotTables(flags);
	int size;
	size=(res*res+res)>>1;
	if(flags & DOT_FLAG){
		dotTable=new Real[size];
		memset(dotTable,0,sizeof(Real)*size);
	}
	if(flags & D_DOT_FLAG){
		dDotTable=new Real[size];
		memset(dDotTable,0,sizeof(Real)*size);
	}
	if(flags & D2_DOT_FLAG){
		d2DotTable=new Real[size];
		memset(d2DotTable,0,sizeof(Real)*size);
	}
	double t1,t2;
	t1=baseFunction.polys[0].start;
	t2=baseFunction.polys[baseFunction.polyCount-1].start;
	for(int i=0;i<res;i++){
		double c1,c2,w1,w2;
		BinaryNode<double>::CenterAndWidth(i,c1,w1);
		double start1	=t1*w1+c1;
		double end1		=t2*w1+c1;
		for(int j=0;j<=i;j++){
			BinaryNode<double>::CenterAndWidth(j,c2,w2);
			int idx=SymmetricIndex(i,j);

			double start	=t1*w2+c2;
			double end		=t2*w2+c2;

			if(start<start1){start=start1;}
			if(end>end1)	{end=end1;}
			if(start>=end){continue;}

			BinaryNode<double>::CenterAndWidth(j,c2,w2);
			Real dot=dotProduct(c1,w1,c2,w2);
			if(fabs(dot)<1e-15){continue;}
			if(flags & DOT_FLAG){dotTable[idx]=dot;}
			if(useDotRatios){
				if(flags & D_DOT_FLAG){
					dDotTable [idx]=-dDotProduct(c1,w1,c2,w2)/dot;
				}
				if(flags & D2_DOT_FLAG){d2DotTable[idx]=d2DotProduct(c1,w1,c2,w2)/dot;}
			}
			else{
				if(flags & D_DOT_FLAG){
					dDotTable[idx]= dDotProduct(c1,w1,c2,w2);
				}
				if(flags & D2_DOT_FLAG){d2DotTable[idx]=d2DotProduct(c1,w1,c2,w2);}
			}
		}
	}
}
template<int Degree,class Real>
void FunctionData<Degree,Real>::ClearDotTables(const int& flags){
	if((flags & DOT_FLAG) && dotTable){
		delete[] dotTable;
		dotTable=NULL;
	}
	if((flags & D_DOT_FLAG) && dDotTable){
		delete[] dDotTable;
		dDotTable=NULL;
	}
	if((flags & D2_DOT_FLAG) && d2DotTable){
		delete[] d2DotTable;
		d2DotTable=NULL;
	}
}
template<int Degree,class Real>
void FunctionData<Degree,Real>::SetValueTables(const int& flags,const double& smooth){
	ClearValueTables();
	if(flags &   VALUE_FLAG){ valueTables=new Real[res*res2];}
	if(flags & D_VALUE_FLAG){dValueTables=new Real[res*res2];}
	PPolynomial<Degree+1> function;
	PPolynomial<Degree>  dFunction;
	for(int i=0;i<res;i++){
		if(smooth>0){
			function=baseFunctions[i].MovingAverage(smooth);
			dFunction=baseFunctions[i].derivative().MovingAverage(smooth);
		}
		else{
			function=baseFunctions[i];
			dFunction=baseFunctions[i].derivative();
		}
		for(int j=0;j<res2;j++){
			double x=double(j)/(res2-1);
			if(flags &   VALUE_FLAG){ valueTables[j*res+i]=Real( function(x));}
			if(flags & D_VALUE_FLAG){dValueTables[j*res+i]=Real(dFunction(x));}
		}
	}
}
template<int Degree,class Real>
void FunctionData<Degree,Real>::SetValueTables(const int& flags,const double& valueSmooth,const double& normalSmooth){
	ClearValueTables();
	if(flags &   VALUE_FLAG){ valueTables=new Real[res*res2];}
	if(flags & D_VALUE_FLAG){dValueTables=new Real[res*res2];}
	PPolynomial<Degree+1> function;
	PPolynomial<Degree>  dFunction;
	for(int i=0;i<res;i++){
		if(valueSmooth>0)	{ function=baseFunctions[i].MovingAverage(valueSmooth);}
		else				{ function=baseFunctions[i];}
		if(normalSmooth>0)	{dFunction=baseFunctions[i].derivative().MovingAverage(normalSmooth);}
		else				{dFunction=baseFunctions[i].derivative();}

		for(int j=0;j<res2;j++){
			double x=double(j)/(res2-1);
			if(flags &   VALUE_FLAG){ valueTables[j*res+i]=Real( function(x));}
			if(flags & D_VALUE_FLAG){dValueTables[j*res+i]=Real(dFunction(x));}
		}
	}
}


template<int Degree,class Real>
void FunctionData<Degree,Real>::ClearValueTables(void){
	if( valueTables){delete[]  valueTables;}
	if(dValueTables){delete[] dValueTables;}
	valueTables=dValueTables=NULL;
}
template<int Degree,class Real>
Real FunctionData<Degree,Real>::dotProduct(const double& center1,const double& width1,const double& center2,const double& width2) const{
	double r=fabs(baseFunction.polys[0].start);
	switch(normalize){
		case 2:
			return Real((baseFunction*baseFunction.scale(width2/width1).shift((center2-center1)/width1)).integral(-2*r,2*r)*width1/sqrt(width1*width2));
		case 1:
			return Real((baseFunction*baseFunction.scale(width2/width1).shift((center2-center1)/width1)).integral(-2*r,2*r)*width1/(width1*width2));
		default:
			return Real((baseFunction*baseFunction.scale(width2/width1).shift((center2-center1)/width1)).integral(-2*r,2*r)*width1);
	}
}
template<int Degree,class Real>
Real FunctionData<Degree,Real>::dDotProduct(const double& center1,const double& width1,const double& center2,const double& width2) const{
	double r=fabs(baseFunction.polys[0].start);
	switch(normalize){
		case 2:
			return Real((dBaseFunction*baseFunction.scale(width2/width1).shift((center2-center1)/width1)).integral(-2*r,2*r)/sqrt(width1*width2));
		case 1:
			return Real((dBaseFunction*baseFunction.scale(width2/width1).shift((center2-center1)/width1)).integral(-2*r,2*r)/(width1*width2));
		default:
			return Real((dBaseFunction*baseFunction.scale(width2/width1).shift((center2-center1)/width1)).integral(-2*r,2*r));
	}
}
template<int Degree,class Real>
Real FunctionData<Degree,Real>::d2DotProduct(const double& center1,const double& width1,const double& center2,const double& width2) const{
	double r=fabs(baseFunction.polys[0].start);
	switch(normalize){
		case 2:
			return Real((dBaseFunction*dBaseFunction.scale(width2/width1).shift((center2-center1)/width1)).integral(-2*r,2*r)/width2/sqrt(width1*width2));
		case 1:
			return Real((dBaseFunction*dBaseFunction.scale(width2/width1).shift((center2-center1)/width1)).integral(-2*r,2*r)/width2/(width1*width2));
		default:
			return Real((dBaseFunction*dBaseFunction.scale(width2/width1).shift((center2-center1)/width1)).integral(-2*r,2*r)/width2);
	}
}
template<int Degree,class Real>
inline int FunctionData<Degree,Real>::SymmetricIndex(const int& i1,const int& i2){
	if(i1>i2)	{return ((i1*i1+i1)>>1)+i2;}
	else		{return ((i2*i2+i2)>>1)+i1;}
}
template<int Degree,class Real>
inline int FunctionData<Degree,Real>::SymmetricIndex(const int& i1,const int& i2,int& index){
	if(i1<i2){
		index=((i2*i2+i2)>>1)+i1;
		return 1;
	}
	else{
		index=((i1*i1+i1)>>1)+i2;
		return 0;
	}
}

//////////////////
// Geometry //
//////////////////
template<class Real>
Real Random(void){return Real(rand())/RAND_MAX;}

template<class Real>
Point3D<Real> RandomBallPoint(void){
	Point3D<Real> p;
	while(1){
		p.coords[0]=Real(1.0-2.0*Random<Real>());
		p.coords[1]=Real(1.0-2.0*Random<Real>());
		p.coords[2]=Real(1.0-2.0*Random<Real>());
		double l=SquareLength(p);
		if(l<=1){return p;}
	}
}
template<class Real>
Point3D<Real> RandomSpherePoint(void){
	Point3D<Real> p=RandomBallPoint<Real>();
	Real l=Real(Length(p));
	p.coords[0]/=l;
	p.coords[1]/=l;
	p.coords[2]/=l;
	return p;
}

template<class Real>
double SquareDistance(const Point3D<Real>& p1,const Point3D<Real>& p2){
	return (p1.coords[0]-p2.coords[0])*(p1.coords[0]-p2.coords[0])+(p1.coords[1]-p2.coords[1])*(p1.coords[1]-p2.coords[1])+(p1.coords[2]-p2.coords[2])*(p1.coords[2]-p2.coords[2]);
}

template<class Real>
double Distance(const Point3D<Real>& p1,const Point3D<Real>& p2){return sqrt(SquareDistance(p1,p2));}

template<class Real>
void EdgeCollapse(const Real& edgeRatio,std::vector<TriangleIndex>& triangles,std::vector< Point3D<Real> >& positions,std::vector< Point3D<Real> >* normals){
	int i,j,*remapTable,*pointCount,idx[3];
	Point3D<Real> p[3],q[2],c;
	double d[3],a;
	double Ratio=12.0/sqrt(3.0);	// (Sum of Squares Length / Area) for and equilateral triangle

	remapTable=new int[positions.size()];
	pointCount=new int[positions.size()];
	for(i=0;i<int(positions.size());i++){
		remapTable[i]=i;
		pointCount[i]=1;
	}
	for(i=int(triangles.size()-1);i>=0;i--){
		for(j=0;j<3;j++){
			idx[j]=triangles[i].idx[j];
			while(remapTable[idx[j]]<idx[j]){idx[j]=remapTable[idx[j]];}
		}
		if(idx[0]==idx[1] || idx[0]==idx[2] || idx[1]==idx[2]){
			triangles[i]=triangles[triangles.size()-1];
			triangles.pop_back();
			continue;
		}
		for(j=0;j<3;j++){
			p[j].coords[0]=positions[idx[j]].coords[0]/pointCount[idx[j]];
			p[j].coords[1]=positions[idx[j]].coords[1]/pointCount[idx[j]];
			p[j].coords[2]=positions[idx[j]].coords[2]/pointCount[idx[j]];
		}
		for(j=0;j<3;j++){
			q[0].coords[j]=p[1].coords[j]-p[0].coords[j];
			q[1].coords[j]=p[2].coords[j]-p[0].coords[j];
			d[j]=SquareDistance(p[j],p[(j+1)%3]);
		}
		CrossProduct(q[0],q[1],c);
		a=Length(c)/2;

		if((d[0]+d[1]+d[2])*edgeRatio > a*Ratio){
			// Find the smallest edge
			j=0;
			if(d[1]<d[j]){j=1;}
			if(d[2]<d[j]){j=2;}

			int idx1,idx2;
			if(idx[j]<idx[(j+1)%3]){
				idx1=idx[j];
				idx2=idx[(j+1)%3];
			}
			else{
				idx2=idx[j];
				idx1=idx[(j+1)%3];
			}
			positions[idx1].coords[0]+=positions[idx2].coords[0];
			positions[idx1].coords[1]+=positions[idx2].coords[1];
			positions[idx1].coords[2]+=positions[idx2].coords[2];
			if(normals){
				(*normals)[idx1].coords[0]+=(*normals)[idx2].coords[0];
				(*normals)[idx1].coords[1]+=(*normals)[idx2].coords[1];
				(*normals)[idx1].coords[2]+=(*normals)[idx2].coords[2];
			}
			pointCount[idx1]+=pointCount[idx2];
			remapTable[idx2]=idx1;
			triangles[i]=triangles[triangles.size()-1];
			triangles.pop_back();
		}
	}
	int pCount=0;
	for(i=0;i<int(positions.size());i++){
		for(j=0;j<3;j++){positions[i].coords[j]/=pointCount[i];}
		if(normals){
			Real l=Real(Length((*normals)[i]));
			for(j=0;j<3;j++){(*normals)[i].coords[j]/=l;}
		}
		if(remapTable[i]==i){ // If vertex i is being used
			positions[pCount]=positions[i];
			if(normals){(*normals)[pCount]=(*normals)[i];}
			pointCount[i]=pCount;
			pCount++;
		}
	}
	positions.resize(pCount);
	for(i=int(triangles.size()-1);i>=0;i--){
		for(j=0;j<3;j++){
			idx[j]=triangles[i].idx[j];
			while(remapTable[idx[j]]<idx[j]){idx[j]=remapTable[idx[j]];}
			triangles[i].idx[j]=pointCount[idx[j]];
		}
		if(idx[0]==idx[1] || idx[0]==idx[2] || idx[1]==idx[2]){
			triangles[i]=triangles[triangles.size()-1];
			triangles.pop_back();
		}
	}

	delete[] pointCount;
	delete[] remapTable;
}
template<class Real>
void TriangleCollapse(const Real& edgeRatio,std::vector<TriangleIndex>& triangles,std::vector< Point3D<Real> >& positions,std::vector< Point3D<Real> >* normals){
	int i,j,*remapTable,*pointCount,idx[3];
	Point3D<Real> p[3],q[2],c;
	double d[3],a;
	double Ratio=12.0/sqrt(3.0);	// (Sum of Squares Length / Area) for and equilateral triangle

	remapTable=new int[positions.size()];
	pointCount=new int[positions.size()];
	for(i=0;i<int(positions.size());i++){
		remapTable[i]=i;
		pointCount[i]=1;
	}
	for(i=int(triangles.size()-1);i>=0;i--){
		for(j=0;j<3;j++){
			idx[j]=triangles[i].idx[j];
			while(remapTable[idx[j]]<idx[j]){idx[j]=remapTable[idx[j]];}
		}
		if(idx[0]==idx[1] || idx[0]==idx[2] || idx[1]==idx[2]){
			triangles[i]=triangles[triangles.size()-1];
			triangles.pop_back();
			continue;
		}
		for(j=0;j<3;j++){
			p[j].coords[0]=positions[idx[j]].coords[0]/pointCount[idx[j]];
			p[j].coords[1]=positions[idx[j]].coords[1]/pointCount[idx[j]];
			p[j].coords[2]=positions[idx[j]].coords[2]/pointCount[idx[j]];
		}
		for(j=0;j<3;j++){
			q[0].coords[j]=p[1].coords[j]-p[0].coords[j];
			q[1].coords[j]=p[2].coords[j]-p[0].coords[j];
			d[j]=SquareDistance(p[j],p[(j+1)%3]);
		}
		CrossProduct(q[0],q[1],c);
		a=Length(c)/2;

		if((d[0]+d[1]+d[2])*edgeRatio > a*Ratio){
			// Find the smallest edge
			j=0;
			if(d[1]<d[j]){j=1;}
			if(d[2]<d[j]){j=2;}

			int idx1,idx2,idx3;
			if(idx[0]<idx[1]){
				if(idx[0]<idx[2]){
					idx1=idx[0];
					idx2=idx[2];
					idx3=idx[1];
				}
				else{
					idx1=idx[2];
					idx2=idx[0];
					idx3=idx[1];
				}
			}
			else{
				if(idx[1]<idx[2]){
					idx1=idx[1];
					idx2=idx[2];
					idx3=idx[0];
				}
				else{
					idx1=idx[2];
					idx2=idx[1];
					idx3=idx[0];
				}
			}
			positions[idx1].coords[0]+=positions[idx2].coords[0]+positions[idx3].coords[0];
			positions[idx1].coords[1]+=positions[idx2].coords[1]+positions[idx3].coords[1];
			positions[idx1].coords[2]+=positions[idx2].coords[2]+positions[idx3].coords[2];
			if(normals){
				(*normals)[idx1].coords[0]+=(*normals)[idx2].coords[0]+(*normals)[idx3].coords[0];
				(*normals)[idx1].coords[1]+=(*normals)[idx2].coords[1]+(*normals)[idx3].coords[1];
				(*normals)[idx1].coords[2]+=(*normals)[idx2].coords[2]+(*normals)[idx3].coords[2];
			}
			pointCount[idx1]+=pointCount[idx2]+pointCount[idx3];
			remapTable[idx2]=idx1;
			remapTable[idx3]=idx1;
			triangles[i]=triangles[triangles.size()-1];
			triangles.pop_back();
		}
	}
	int pCount=0;
	for(i=0;i<int(positions.size());i++){
		for(j=0;j<3;j++){positions[i].coords[j]/=pointCount[i];}
		if(normals){
			Real l=Real(Length((*normals)[i]));
			for(j=0;j<3;j++){(*normals)[i].coords[j]/=l;}
		}
		if(remapTable[i]==i){ // If vertex i is being used
			positions[pCount]=positions[i];
			if(normals){(*normals)[pCount]=(*normals)[i];}
			pointCount[i]=pCount;
			pCount++;
		}
	}
	positions.resize(pCount);
	for(i=int(triangles.size()-1);i>=0;i--){
		for(j=0;j<3;j++){
			idx[j]=triangles[i].idx[j];
			while(remapTable[idx[j]]<idx[j]){idx[j]=remapTable[idx[j]];}
			triangles[i].idx[j]=pointCount[idx[j]];
		}
		if(idx[0]==idx[1] || idx[0]==idx[2] || idx[1]==idx[2]){
			triangles[i]=triangles[triangles.size()-1];
			triangles.pop_back();
		}
	}
	delete[] pointCount;
	delete[] remapTable;
}


/////////////
// OctNode //
/////////////
template<class NodeData,class Real> const int OctNode<NodeData,Real>::DepthShift=5;
template<class NodeData,class Real> const int OctNode<NodeData,Real>::OffsetShift=19;
template<class NodeData,class Real> const int OctNode<NodeData,Real>::DepthMask=(1<<DepthShift)-1;
template<class NodeData,class Real> const int OctNode<NodeData,Real>::OffsetMask=(1<<OffsetShift)-1;
template<class NodeData,class Real> const int OctNode<NodeData,Real>::OffsetShift1=DepthShift;
template<class NodeData,class Real> const int OctNode<NodeData,Real>::OffsetShift2=OffsetShift1+OffsetShift;
template<class NodeData,class Real> const int OctNode<NodeData,Real>::OffsetShift3=OffsetShift2+OffsetShift;

template<class NodeData,class Real> int OctNode<NodeData,Real>::UseAlloc=0;
template<class NodeData,class Real> Allocator<OctNode<NodeData,Real> > OctNode<NodeData,Real>::Allocator;

template<class NodeData,class Real>
void OctNode<NodeData,Real>::SetAllocator(int blockSize)
{
	if(blockSize>0)
	{
		UseAlloc=1;
		Allocator.Set(blockSize);
	}
	else{UseAlloc=0;}
}
template<class NodeData,class Real>
int OctNode<NodeData,Real>::UseAllocator(void){return UseAlloc;}

template <class NodeData,class Real>
OctNode<NodeData,Real>::OctNode(void){
	parent=children=NULL;
	d=off[0]=off[1]=off[2]=0;
}

template <class NodeData,class Real>
OctNode<NodeData,Real>::~OctNode(void){
	if(!UseAlloc){if(children){delete[] children;}}
	parent=children=NULL;
}
template <class NodeData,class Real>
void OctNode<NodeData,Real>::setFullDepth(const int& maxDepth){
	if(maxDepth){
		if(!children){initChildren();}
		for(int i=0;i<8;i++){children[i].setFullDepth(maxDepth-1);}
	}
}

template <class NodeData,class Real>
int OctNode<NodeData,Real>::initChildren(void){
	int i,j,k;

	if(UseAlloc){children=Allocator.NewElements(8);}
	else{
		if(children){delete[] children;}
		children=NULL;
		children=new OctNode[Cube::CORNERS];
	}
	if(!children){
		fprintf(stderr,"Failed to initialize children in OctNode::initChildren\n");
		exit(0);
		return 0;
	}
	int d,off[3];
	depthAndOffset(d,off);
	for(i=0;i<2;i++){
		for(j=0;j<2;j++){
			for(k=0;k<2;k++){
				int idx=Cube::CornerIndex(i,j,k);
				children[idx].parent=this;
				children[idx].children=NULL;
				int off2[3];
				off2[0]=(off[0]<<1)+i;
				off2[1]=(off[1]<<1)+j;
				off2[2]=(off[2]<<1)+k;
				Index(d+1,off2,children[idx].d,children[idx].off);
			}
		}
	}
	return 1;
}
template <class NodeData,class Real>
inline void OctNode<NodeData,Real>::Index(const int& depth,const int offset[3],short& d,short off[3]){
	d=short(depth);
	off[0]=short((1<<depth)+offset[0]-1);
	off[1]=short((1<<depth)+offset[1]-1);
	off[2]=short((1<<depth)+offset[2]-1);
}

template<class NodeData,class Real>
inline void OctNode<NodeData,Real>::depthAndOffset(int& depth,int offset[3]) const {
	depth=int(d);
	offset[0]=(int(off[0])+1)&(~(1<<depth));
	offset[1]=(int(off[1])+1)&(~(1<<depth));
	offset[2]=(int(off[2])+1)&(~(1<<depth));
}
template<class NodeData,class Real>
inline int OctNode<NodeData,Real>::depth(void) const {return int(d);}
template<class NodeData,class Real>
inline void OctNode<NodeData,Real>::DepthAndOffset(const long long& index,int& depth,int offset[3]){
	depth=int(index&DepthMask);
	offset[0]=(int((index>>OffsetShift1)&OffsetMask)+1)&(~(1<<depth));
	offset[1]=(int((index>>OffsetShift2)&OffsetMask)+1)&(~(1<<depth));
	offset[2]=(int((index>>OffsetShift3)&OffsetMask)+1)&(~(1<<depth));
}
template<class NodeData,class Real>
inline int OctNode<NodeData,Real>::Depth(const long long& index){return int(index&DepthMask);}
template <class NodeData,class Real>
void OctNode<NodeData,Real>::centerAndWidth(Point3D<Real>& center,Real& width) const{
	int depth,offset[3];
	depth=int(d);
	offset[0]=(int(off[0])+1)&(~(1<<depth));
	offset[1]=(int(off[1])+1)&(~(1<<depth));
	offset[2]=(int(off[2])+1)&(~(1<<depth));
	width=Real(1.0/(1<<depth));
	for(int dim=0;dim<DIMENSION;dim++){center.coords[dim]=Real(0.5+offset[dim])*width;}
}
template <class NodeData,class Real>
inline void OctNode<NodeData,Real>::CenterAndWidth(const long long& index,Point3D<Real>& center,Real& width){
	int depth,offset[3];
	depth=index&DepthMask;
	offset[0]=(int((index>>OffsetShift1)&OffsetMask)+1)&(~(1<<depth));
	offset[1]=(int((index>>OffsetShift2)&OffsetMask)+1)&(~(1<<depth));
	offset[2]=(int((index>>OffsetShift3)&OffsetMask)+1)&(~(1<<depth));
	width=Real(1.0/(1<<depth));
	for(int dim=0;dim<DIMENSION;dim++){center.coords[dim]=Real(0.5+offset[dim])*width;}
}

template <class NodeData,class Real>
int OctNode<NodeData,Real>::maxDepth(void) const{
	if(!children){return 0;}
	else{
		int c,d;
		for(int i=0;i<Cube::CORNERS;i++){
			d=children[i].maxDepth();
			if(!i || d>c){c=d;}
		}
		return c+1;
	}
}
template <class NodeData,class Real>
int OctNode<NodeData,Real>::nodes(void) const{
	if(!children){return 1;}
	else{
		int c=0;
		for(int i=0;i<Cube::CORNERS;i++){c+=children[i].nodes();}
		return c+1;
	}
}
template <class NodeData,class Real>
int OctNode<NodeData,Real>::leaves(void) const{
	if(!children){return 1;}
	else{
		int c=0;
		for(int i=0;i<Cube::CORNERS;i++){c+=children[i].leaves();}
		return c;
	}
}
template<class NodeData,class Real>
int OctNode<NodeData,Real>::maxDepthLeaves(const int& maxDepth) const{
	if(depth()>maxDepth){return 0;}
	if(!children){return 1;}
	else{
		int c=0;
		for(int i=0;i<Cube::CORNERS;i++){c+=children[i].maxDepthLeaves(maxDepth);}
		return c;
	}
}
template <class NodeData,class Real>
const OctNode<NodeData,Real>* OctNode<NodeData,Real>::root(void) const{
	const OctNode* temp=this;
	while(temp->parent){temp=temp->parent;}
	return temp;
}


template <class NodeData,class Real>
const OctNode<NodeData,Real>* OctNode<NodeData,Real>::nextBranch(const OctNode* current) const{
	if(!current->parent || current==this){return NULL;}
	if(current-current->parent->children==Cube::CORNERS-1){return nextBranch(current->parent);}
	else{return current+1;}
}
template <class NodeData,class Real>
OctNode<NodeData,Real>* OctNode<NodeData,Real>::nextBranch(OctNode* current){
	if(!current->parent || current==this){return NULL;}
	if(current-current->parent->children==Cube::CORNERS-1){return nextBranch(current->parent);}
	else{return current+1;}
}
template <class NodeData,class Real>
const OctNode<NodeData,Real>* OctNode<NodeData,Real>::nextLeaf(const OctNode* current) const{
	if(!current){
		const OctNode<NodeData,Real>* temp=this;
		while(temp->children){temp=&temp->children[0];}
		return temp;
	}
	if(current->children){return current->nextLeaf();}
	const OctNode* temp=nextBranch(current);
	if(!temp){return NULL;}
	else{return temp->nextLeaf();}
}
template <class NodeData,class Real>
OctNode<NodeData,Real>* OctNode<NodeData,Real>::nextLeaf(OctNode* current){
	if(!current){
		OctNode<NodeData,Real>* temp=this;
		while(temp->children){temp=&temp->children[0];}
		return temp;
	}
	if(current->children){return current->nextLeaf();}
	OctNode* temp=nextBranch(current);
	if(!temp){return NULL;}
	else{return temp->nextLeaf();}
}

template <class NodeData,class Real>
const OctNode<NodeData,Real>* OctNode<NodeData,Real>::nextNode(const OctNode* current) const{
	if(!current){return this;}
	else if(current->children){return &current->children[0];}
	else{return nextBranch(current);}
}
template <class NodeData,class Real>
OctNode<NodeData,Real>* OctNode<NodeData,Real>::nextNode(OctNode* current){
	if(!current){return this;}
	else if(current->children){return &current->children[0];}
	else{return nextBranch(current);}
}

template <class NodeData,class Real>
void OctNode<NodeData,Real>::printRange(void) const{
	Point3D<Real> center;
	Real width;
	centerAndWidth(center,width);
	for(int dim=0;dim<DIMENSION;dim++){
		printf("%[%f,%f]",center.coords[dim]-width/2,center.coords[dim]+width/2);
		if(dim<DIMENSION-1){printf("x");}
		else printf("\n");
	}
}

template <class NodeData,class Real>
void OctNode<NodeData,Real>::AdjacencyCountFunction::Function(const OctNode<NodeData,Real>* node1,const OctNode<NodeData,Real>* node2){count++;}

template <class NodeData,class Real>
template<class NodeAdjacencyFunction>
void OctNode<NodeData,Real>::processNodeNodes(OctNode* node,NodeAdjacencyFunction* F,const int& processCurrent){
	if(processCurrent){F->Function(this,node);}
	if(children){__processNodeNodes(node,F);}
}
template <class NodeData,class Real>
template<class NodeAdjacencyFunction>
void OctNode<NodeData,Real>::processNodeFaces(OctNode* node,NodeAdjacencyFunction* F,const int& fIndex,const int& processCurrent){
	if(processCurrent){F->Function(this,node);}
	if(children){
		int c1,c2,c3,c4;
		Cube::FaceCorners(fIndex,c1,c2,c3,c4);
		__processNodeFaces(node,F,c1,c2,c3,c4);
	}
}
template <class NodeData,class Real>
template<class NodeAdjacencyFunction>
void OctNode<NodeData,Real>::processNodeEdges(OctNode* node,NodeAdjacencyFunction* F,const int& eIndex,const int& processCurrent){
	if(processCurrent){F->Function(this,node);}
	if(children){
		int c1,c2;
		Cube::EdgeCorners(eIndex,c1,c2);
		__processNodeEdges(node,F,c1,c2);
	}
}
template <class NodeData,class Real>
template<class NodeAdjacencyFunction>
void OctNode<NodeData,Real>::processNodeCorners(OctNode* node,NodeAdjacencyFunction* F,const int& cIndex,const int& processCurrent){
	if(processCurrent){F->Function(this,node);}
	OctNode<NodeData,Real>* temp=this;
	while(temp->children){
		temp=&temp->children[cIndex];
		F->Function(temp,node);
	}
}
template <class NodeData,class Real>
template<class NodeAdjacencyFunction>
void OctNode<NodeData,Real>::__processNodeNodes(OctNode* node,NodeAdjacencyFunction* F){
	F->Function(&children[0],node);
	F->Function(&children[1],node);
	F->Function(&children[2],node);
	F->Function(&children[3],node);
	F->Function(&children[4],node);
	F->Function(&children[5],node);
	F->Function(&children[6],node);
	F->Function(&children[7],node);
	if(children[0].children){children[0].__processNodeNodes(node,F);}
	if(children[1].children){children[1].__processNodeNodes(node,F);}
	if(children[2].children){children[2].__processNodeNodes(node,F);}
	if(children[3].children){children[3].__processNodeNodes(node,F);}
	if(children[4].children){children[4].__processNodeNodes(node,F);}
	if(children[5].children){children[5].__processNodeNodes(node,F);}
	if(children[6].children){children[6].__processNodeNodes(node,F);}
	if(children[7].children){children[7].__processNodeNodes(node,F);}
}
template <class NodeData,class Real>
template<class NodeAdjacencyFunction>
void OctNode<NodeData,Real>::__processNodeEdges(OctNode* node,NodeAdjacencyFunction* F,const int& cIndex1,const int& cIndex2){
	F->Function(&children[cIndex1],node);
	F->Function(&children[cIndex2],node);
	if(children[cIndex1].children){children[cIndex1].__processNodeEdges(node,F,cIndex1,cIndex2);}
	if(children[cIndex2].children){children[cIndex2].__processNodeEdges(node,F,cIndex1,cIndex2);}
}
template <class NodeData,class Real>
template<class NodeAdjacencyFunction>
void OctNode<NodeData,Real>::__processNodeFaces(OctNode* node,NodeAdjacencyFunction* F,const int& cIndex1,const int& cIndex2,const int& cIndex3,const int& cIndex4){
	F->Function(&children[cIndex1],node);
	F->Function(&children[cIndex2],node);
	F->Function(&children[cIndex3],node);
	F->Function(&children[cIndex4],node);
	if(children[cIndex1].children){children[cIndex1].__processNodeFaces(node,F,cIndex1,cIndex2,cIndex3,cIndex4);}
	if(children[cIndex2].children){children[cIndex2].__processNodeFaces(node,F,cIndex1,cIndex2,cIndex3,cIndex4);}
	if(children[cIndex3].children){children[cIndex3].__processNodeFaces(node,F,cIndex1,cIndex2,cIndex3,cIndex4);}
	if(children[cIndex4].children){children[cIndex4].__processNodeFaces(node,F,cIndex1,cIndex2,cIndex3,cIndex4);}
}
template<class NodeData,class Real>
template<class NodeAdjacencyFunction>
void OctNode<NodeData,Real>::ProcessNodeAdjacentNodes(const int& maxDepth,OctNode* node1,const int& width1,OctNode* node2,const int& width2,NodeAdjacencyFunction* F,const int& processCurrent){
	int c1[3],c2[3],w1,w2;
	node1->centerIndex(maxDepth+1,c1);
	node2->centerIndex(maxDepth+1,c2);
	w1=node1->width(maxDepth+1);
	w2=node2->width(maxDepth+1);

	ProcessNodeAdjacentNodes(c1[0]-c2[0],c1[1]-c2[1],c1[2]-c2[2],node1,(width1*w1)>>1,node2,(width2*w2)>>1,w2,F,processCurrent);
}
template<class NodeData,class Real>
template<class NodeAdjacencyFunction>
void OctNode<NodeData,Real>::ProcessNodeAdjacentNodes(const int& dx,const int& dy,const int& dz,
													  OctNode<NodeData,Real>* node1,const int& radius1,
													  OctNode<NodeData,Real>* node2,const int& radius2,const int& width2,
													  NodeAdjacencyFunction* F,const int& processCurrent){
	if(!Overlap(dx,dy,dz,radius1+radius2)){return;}
	if(processCurrent){F->Function(node2,node1);}
	if(!node2->children){return;}
	__ProcessNodeAdjacentNodes(-dx,-dy,-dz,node1,radius1,node2,radius2,width2/2,F);
}
template<class NodeData,class Real>
template<class TerminatingNodeAdjacencyFunction>
void OctNode<NodeData,Real>::ProcessTerminatingNodeAdjacentNodes(const int& maxDepth,OctNode* node1,const int& width1,OctNode* node2,const int& width2,TerminatingNodeAdjacencyFunction* F,const int& processCurrent){
	int c1[3],c2[3],w1,w2;
	node1->centerIndex(maxDepth+1,c1);
	node2->centerIndex(maxDepth+1,c2);
	w1=node1->width(maxDepth+1);
	w2=node2->width(maxDepth+1);

	ProcessTerminatingNodeAdjacentNodes(c1[0]-c2[0],c1[1]-c2[1],c1[2]-c2[2],node1,(width1*w1)>>1,node2,(width2*w2)>>1,w2,F,processCurrent);
}
template<class NodeData,class Real>
template<class TerminatingNodeAdjacencyFunction>
void OctNode<NodeData,Real>::ProcessTerminatingNodeAdjacentNodes(const int& dx,const int& dy,const int& dz,
																 OctNode<NodeData,Real>* node1,const int& radius1,
																 OctNode<NodeData,Real>* node2,const int& radius2,const int& width2,
																 TerminatingNodeAdjacencyFunction* F,const int& processCurrent)
{
	if(!Overlap(dx,dy,dz,radius1+radius2)){return;}
	if(processCurrent){F->Function(node2,node1);}
	if(!node2->children){return;}
	__ProcessTerminatingNodeAdjacentNodes(-dx,-dy,-dz,node1,radius1,node2,radius2,width2/2,F);
}
template<class NodeData,class Real>
template<class PointAdjacencyFunction>
void OctNode<NodeData,Real>::ProcessPointAdjacentNodes(const int& maxDepth,const int c1[3],OctNode* node2,const int& width2,PointAdjacencyFunction* F,const int& processCurrent){
	int c2[3],w2;
	node2->centerIndex(maxDepth+1,c2);
	w2=node2->width(maxDepth+1);
	ProcessPointAdjacentNodes(c1[0]-c2[0],c1[1]-c2[1],c1[2]-c2[2],node2,(width2*w2)>>1,w2,F,processCurrent);
}
template<class NodeData,class Real>
template<class PointAdjacencyFunction>
void OctNode<NodeData,Real>::ProcessPointAdjacentNodes(const int& dx,const int& dy,const int& dz,
													   OctNode<NodeData,Real>* node2,const int& radius2,const int& width2,
													   PointAdjacencyFunction* F,const int& processCurrent)
{
	if(!Overlap(dx,dy,dz,radius2)){return;}
	if(processCurrent){F->Function(node2);}
	if(!node2->children){return;}
	__ProcessPointAdjacentNodes(-dx,-dy,-dz,node2,radius2,width2>>1,F);
}
template<class NodeData,class Real>
template<class NodeAdjacencyFunction>
void OctNode<NodeData,Real>::ProcessFixedDepthNodeAdjacentNodes(const int& maxDepth,
																OctNode<NodeData,Real>* node1,const int& width1,
																OctNode<NodeData,Real>* node2,const int& width2,
																const int& depth,NodeAdjacencyFunction* F,const int& processCurrent)
{
	int c1[3],c2[3],w1,w2;
	node1->centerIndex(maxDepth+1,c1);
	node2->centerIndex(maxDepth+1,c2);
	w1=node1->width(maxDepth+1);
	w2=node2->width(maxDepth+1);

	ProcessFixedDepthNodeAdjacentNodes(c1[0]-c2[0],c1[1]-c2[1],c1[2]-c2[2],node1,(width1*w1)>>1,node2,(width2*w2)>>1,w2,depth,F,processCurrent);
}
template<class NodeData,class Real>
template<class NodeAdjacencyFunction>
void OctNode<NodeData,Real>::ProcessFixedDepthNodeAdjacentNodes(const int& dx,const int& dy,const int& dz,
																OctNode<NodeData,Real>* node1,const int& radius1,
																OctNode<NodeData,Real>* node2,const int& radius2,const int& width2,
																const int& depth,NodeAdjacencyFunction* F,const int& processCurrent)
{
	int d=node2->depth();
	if(d>depth){return;}
	if(!Overlap(dx,dy,dz,radius1+radius2)){return;}
	if(d==depth){if(processCurrent){F->Function(node2,node1);}}
	else{
		if(!node2->children){return;}
		__ProcessFixedDepthNodeAdjacentNodes(-dx,-dy,-dz,node1,radius1,node2,radius2,width2/2,depth-1,F);
	}
}
template<class NodeData,class Real>
template<class NodeAdjacencyFunction>
void OctNode<NodeData,Real>::ProcessMaxDepthNodeAdjacentNodes(const int& maxDepth,
															  OctNode<NodeData,Real>* node1,const int& width1,
															  OctNode<NodeData,Real>* node2,const int& width2,
															  const int& depth,NodeAdjacencyFunction* F,const int& processCurrent)
{
	int c1[3],c2[3],w1,w2;
	node1->centerIndex(maxDepth+1,c1);
	node2->centerIndex(maxDepth+1,c2);
	w1=node1->width(maxDepth+1);
	w2=node2->width(maxDepth+1);
	ProcessMaxDepthNodeAdjacentNodes(c1[0]-c2[0],c1[1]-c2[1],c1[2]-c2[2],node1,(width1*w1)>>1,node2,(width2*w2)>>1,w2,depth,F,processCurrent);
}
template<class NodeData,class Real>
template<class NodeAdjacencyFunction>
void OctNode<NodeData,Real>::ProcessMaxDepthNodeAdjacentNodes(const int& dx,const int& dy,const int& dz,
															  OctNode<NodeData,Real>* node1,const int& radius1,
															  OctNode<NodeData,Real>* node2,const int& radius2,const int& width2,
															  const int& depth,NodeAdjacencyFunction* F,const int& processCurrent)
{
	int d=node2->depth();
	if(d>depth){return;}
	if(!Overlap(dx,dy,dz,radius1+radius2)){return;}
	if(processCurrent){F->Function(node2,node1);}
	if(d<depth && node2->children){__ProcessMaxDepthNodeAdjacentNodes(-dx,-dy,-dz,node1,radius1,node2,radius2,width2>>1,depth-1,F);}
}
template <class NodeData,class Real>
template<class NodeAdjacencyFunction>
void OctNode<NodeData,Real>::__ProcessNodeAdjacentNodes(const int& dx,const int& dy,const int& dz,
														OctNode* node1,const int& radius1,
														OctNode* node2,const int& radius2,const int& cWidth2,
														NodeAdjacencyFunction* F)
{
	int cWidth=cWidth2>>1;
	int radius=radius2>>1;
	int o=ChildOverlap(dx,dy,dz,radius1+radius,cWidth);
	if(o){
		int dx1=dx-cWidth;
		int dx2=dx+cWidth;
		int dy1=dy-cWidth;
		int dy2=dy+cWidth;
		int dz1=dz-cWidth;
		int dz2=dz+cWidth;
		if(o&  1){F->Function(&node2->children[0],node1);if(node2->children[0].children){__ProcessNodeAdjacentNodes(dx1,dy1,dz1,node1,radius1,&node2->children[0],radius,cWidth,F);}}
		if(o&  2){F->Function(&node2->children[1],node1);if(node2->children[1].children){__ProcessNodeAdjacentNodes(dx2,dy1,dz1,node1,radius1,&node2->children[1],radius,cWidth,F);}}
		if(o&  4){F->Function(&node2->children[2],node1);if(node2->children[2].children){__ProcessNodeAdjacentNodes(dx1,dy2,dz1,node1,radius1,&node2->children[2],radius,cWidth,F);}}
		if(o&  8){F->Function(&node2->children[3],node1);if(node2->children[3].children){__ProcessNodeAdjacentNodes(dx2,dy2,dz1,node1,radius1,&node2->children[3],radius,cWidth,F);}}
		if(o& 16){F->Function(&node2->children[4],node1);if(node2->children[4].children){__ProcessNodeAdjacentNodes(dx1,dy1,dz2,node1,radius1,&node2->children[4],radius,cWidth,F);}}
		if(o& 32){F->Function(&node2->children[5],node1);if(node2->children[5].children){__ProcessNodeAdjacentNodes(dx2,dy1,dz2,node1,radius1,&node2->children[5],radius,cWidth,F);}}
		if(o& 64){F->Function(&node2->children[6],node1);if(node2->children[6].children){__ProcessNodeAdjacentNodes(dx1,dy2,dz2,node1,radius1,&node2->children[6],radius,cWidth,F);}}
		if(o&128){F->Function(&node2->children[7],node1);if(node2->children[7].children){__ProcessNodeAdjacentNodes(dx2,dy2,dz2,node1,radius1,&node2->children[7],radius,cWidth,F);}}
	}
}
template <class NodeData,class Real>
template<class TerminatingNodeAdjacencyFunction>
void OctNode<NodeData,Real>::__ProcessTerminatingNodeAdjacentNodes(const int& dx,const int& dy,const int& dz,
																   OctNode* node1,const int& radius1,
																   OctNode* node2,const int& radius2,const int& cWidth2,
																   TerminatingNodeAdjacencyFunction* F)
{
	int cWidth=cWidth2>>1;
	int radius=radius2>>1;
	int o=ChildOverlap(dx,dy,dz,radius1+radius,cWidth);
	if(o){
		int dx1=dx-cWidth;
		int dx2=dx+cWidth;
		int dy1=dy-cWidth;
		int dy2=dy+cWidth;
		int dz1=dz-cWidth;
		int dz2=dz+cWidth;
		if(o&  1){if(F->Function(&node2->children[0],node1) && node2->children[0].children){__ProcessTerminatingNodeAdjacentNodes(dx1,dy1,dz1,node1,radius1,&node2->children[0],radius,cWidth,F);}}
		if(o&  2){if(F->Function(&node2->children[1],node1) && node2->children[1].children){__ProcessTerminatingNodeAdjacentNodes(dx2,dy1,dz1,node1,radius1,&node2->children[1],radius,cWidth,F);}}
		if(o&  4){if(F->Function(&node2->children[2],node1) && node2->children[2].children){__ProcessTerminatingNodeAdjacentNodes(dx1,dy2,dz1,node1,radius1,&node2->children[2],radius,cWidth,F);}}
		if(o&  8){if(F->Function(&node2->children[3],node1) && node2->children[3].children){__ProcessTerminatingNodeAdjacentNodes(dx2,dy2,dz1,node1,radius1,&node2->children[3],radius,cWidth,F);}}
		if(o& 16){if(F->Function(&node2->children[4],node1) && node2->children[4].children){__ProcessTerminatingNodeAdjacentNodes(dx1,dy1,dz2,node1,radius1,&node2->children[4],radius,cWidth,F);}}
		if(o& 32){if(F->Function(&node2->children[5],node1) && node2->children[5].children){__ProcessTerminatingNodeAdjacentNodes(dx2,dy1,dz2,node1,radius1,&node2->children[5],radius,cWidth,F);}}
		if(o& 64){if(F->Function(&node2->children[6],node1) && node2->children[6].children){__ProcessTerminatingNodeAdjacentNodes(dx1,dy2,dz2,node1,radius1,&node2->children[6],radius,cWidth,F);}}
		if(o&128){if(F->Function(&node2->children[7],node1) && node2->children[7].children){__ProcessTerminatingNodeAdjacentNodes(dx2,dy2,dz2,node1,radius1,&node2->children[7],radius,cWidth,F);}}
	}
}
template <class NodeData,class Real>
template<class PointAdjacencyFunction>
void OctNode<NodeData,Real>::__ProcessPointAdjacentNodes(const int& dx,const int& dy,const int& dz,
														 OctNode* node2,const int& radius2,const int& cWidth2,
														 PointAdjacencyFunction* F)
{
	int cWidth=cWidth2>>1;
	int radius=radius2>>1;
	int o=ChildOverlap(dx,dy,dz,radius,cWidth);
	if(o){
		int dx1=dx-cWidth;
		int dx2=dx+cWidth;
		int dy1=dy-cWidth;
		int dy2=dy+cWidth;
		int dz1=dz-cWidth;
		int dz2=dz+cWidth;
		if(o&  1){F->Function(&node2->children[0]);if(node2->children[0].children){__ProcessPointAdjacentNodes(dx1,dy1,dz1,&node2->children[0],radius,cWidth,F);}}
		if(o&  2){F->Function(&node2->children[1]);if(node2->children[1].children){__ProcessPointAdjacentNodes(dx2,dy1,dz1,&node2->children[1],radius,cWidth,F);}}
		if(o&  4){F->Function(&node2->children[2]);if(node2->children[2].children){__ProcessPointAdjacentNodes(dx1,dy2,dz1,&node2->children[2],radius,cWidth,F);}}
		if(o&  8){F->Function(&node2->children[3]);if(node2->children[3].children){__ProcessPointAdjacentNodes(dx2,dy2,dz1,&node2->children[3],radius,cWidth,F);}}
		if(o& 16){F->Function(&node2->children[4]);if(node2->children[4].children){__ProcessPointAdjacentNodes(dx1,dy1,dz2,&node2->children[4],radius,cWidth,F);}}
		if(o& 32){F->Function(&node2->children[5]);if(node2->children[5].children){__ProcessPointAdjacentNodes(dx2,dy1,dz2,&node2->children[5],radius,cWidth,F);}}
		if(o& 64){F->Function(&node2->children[6]);if(node2->children[6].children){__ProcessPointAdjacentNodes(dx1,dy2,dz2,&node2->children[6],radius,cWidth,F);}}
		if(o&128){F->Function(&node2->children[7]);if(node2->children[7].children){__ProcessPointAdjacentNodes(dx2,dy2,dz2,&node2->children[7],radius,cWidth,F);}}
	}
}
template <class NodeData,class Real>
template<class NodeAdjacencyFunction>
void OctNode<NodeData,Real>::__ProcessFixedDepthNodeAdjacentNodes(const int& dx,const int& dy,const int& dz,
																  OctNode* node1,const int& radius1,
																  OctNode* node2,const int& radius2,const int& cWidth2,
																  const int& depth,NodeAdjacencyFunction* F)
{
	int cWidth=cWidth2>>1;
	int radius=radius2>>1;
	int o=ChildOverlap(dx,dy,dz,radius1+radius,cWidth);
	if(o){
		int dx1=dx-cWidth;
		int dx2=dx+cWidth;
		int dy1=dy-cWidth;
		int dy2=dy+cWidth;
		int dz1=dz-cWidth;
		int dz2=dz+cWidth;
		if(node2->depth()==depth){
			if(o&  1){F->Function(&node2->children[0],node1);}
			if(o&  2){F->Function(&node2->children[1],node1);}
			if(o&  4){F->Function(&node2->children[2],node1);}
			if(o&  8){F->Function(&node2->children[3],node1);}
			if(o& 16){F->Function(&node2->children[4],node1);}
			if(o& 32){F->Function(&node2->children[5],node1);}
			if(o& 64){F->Function(&node2->children[6],node1);}
			if(o&128){F->Function(&node2->children[7],node1);}
		}
		else{
			if(o&  1){if(node2->children[0].children){__ProcessFixedDepthNodeAdjacentNodes(dx1,dy1,dz1,node1,radius1,&node2->children[0],radius,cWidth,depth,F);}}
			if(o&  2){if(node2->children[1].children){__ProcessFixedDepthNodeAdjacentNodes(dx2,dy1,dz1,node1,radius1,&node2->children[1],radius,cWidth,depth,F);}}
			if(o&  4){if(node2->children[2].children){__ProcessFixedDepthNodeAdjacentNodes(dx1,dy2,dz1,node1,radius1,&node2->children[2],radius,cWidth,depth,F);}}
			if(o&  8){if(node2->children[3].children){__ProcessFixedDepthNodeAdjacentNodes(dx2,dy2,dz1,node1,radius1,&node2->children[3],radius,cWidth,depth,F);}}
			if(o& 16){if(node2->children[4].children){__ProcessFixedDepthNodeAdjacentNodes(dx1,dy1,dz2,node1,radius1,&node2->children[4],radius,cWidth,depth,F);}}
			if(o& 32){if(node2->children[5].children){__ProcessFixedDepthNodeAdjacentNodes(dx2,dy1,dz2,node1,radius1,&node2->children[5],radius,cWidth,depth,F);}}
			if(o& 64){if(node2->children[6].children){__ProcessFixedDepthNodeAdjacentNodes(dx1,dy2,dz2,node1,radius1,&node2->children[6],radius,cWidth,depth,F);}}
			if(o&128){if(node2->children[7].children){__ProcessFixedDepthNodeAdjacentNodes(dx2,dy2,dz2,node1,radius1,&node2->children[7],radius,cWidth,depth,F);}}
		}
	}
}
template <class NodeData,class Real>
template<class NodeAdjacencyFunction>
void OctNode<NodeData,Real>::__ProcessMaxDepthNodeAdjacentNodes(const int& dx,const int& dy,const int& dz,
																OctNode* node1,const int& radius1,
																OctNode* node2,const int& radius2,const int& cWidth2,
																const int& depth,NodeAdjacencyFunction* F)
{
	int cWidth=cWidth2>>1;
	int radius=radius2>>1;
	int o=ChildOverlap(dx,dy,dz,radius1+radius,cWidth);
	if(o){
		int dx1=dx-cWidth;
		int dx2=dx+cWidth;
		int dy1=dy-cWidth;
		int dy2=dy+cWidth;
		int dz1=dz-cWidth;
		int dz2=dz+cWidth;
		if(node2->depth()<=depth){
			if(o&  1){F->Function(&node2->children[0],node1);}
			if(o&  2){F->Function(&node2->children[1],node1);}
			if(o&  4){F->Function(&node2->children[2],node1);}
			if(o&  8){F->Function(&node2->children[3],node1);}
			if(o& 16){F->Function(&node2->children[4],node1);}
			if(o& 32){F->Function(&node2->children[5],node1);}
			if(o& 64){F->Function(&node2->children[6],node1);}
			if(o&128){F->Function(&node2->children[7],node1);}
		}
		if(node2->depth()<depth){
			if(o&  1){if(node2->children[0].children){__ProcessMaxDepthNodeAdjacentNodes(dx1,dy1,dz1,node1,radius1,&node2->children[0],radius,cWidth,depth,F);}}
			if(o&  2){if(node2->children[1].children){__ProcessMaxDepthNodeAdjacentNodes(dx2,dy1,dz1,node1,radius1,&node2->children[1],radius,cWidth,depth,F);}}
			if(o&  4){if(node2->children[2].children){__ProcessMaxDepthNodeAdjacentNodes(dx1,dy2,dz1,node1,radius1,&node2->children[2],radius,cWidth,depth,F);}}
			if(o&  8){if(node2->children[3].children){__ProcessMaxDepthNodeAdjacentNodes(dx2,dy2,dz1,node1,radius1,&node2->children[3],radius,cWidth,depth,F);}}
			if(o& 16){if(node2->children[4].children){__ProcessMaxDepthNodeAdjacentNodes(dx1,dy1,dz2,node1,radius1,&node2->children[4],radius,cWidth,depth,F);}}
			if(o& 32){if(node2->children[5].children){__ProcessMaxDepthNodeAdjacentNodes(dx2,dy1,dz2,node1,radius1,&node2->children[5],radius,cWidth,depth,F);}}
			if(o& 64){if(node2->children[6].children){__ProcessMaxDepthNodeAdjacentNodes(dx1,dy2,dz2,node1,radius1,&node2->children[6],radius,cWidth,depth,F);}}
			if(o&128){if(node2->children[7].children){__ProcessMaxDepthNodeAdjacentNodes(dx2,dy2,dz2,node1,radius1,&node2->children[7],radius,cWidth,depth,F);}}
		}
	}
}
template <class NodeData,class Real>
inline int OctNode<NodeData,Real>::ChildOverlap(const int& dx,const int& dy,const int& dz,const int& d,const int& cRadius2)
{
	int w1=d-cRadius2;
	int w2=d+cRadius2;
	int overlap=0;

	int test=0,test1=0;
	if(dx<w2 && dx>-w1){test =1;}
	if(dx<w1 && dx>-w2){test|=2;}

	if(!test){return 0;}
	if(dz<w2 && dz>-w1){test1 =test;}
	if(dz<w1 && dz>-w2){test1|=test<<4;}

	if(!test1){return 0;}
	if(dy<w2 && dy>-w1){overlap =test1;}
	if(dy<w1 && dy>-w2){overlap|=test1<<2;}
	return overlap;
}

template <class NodeData,class Real>
OctNode<NodeData,Real>* OctNode<NodeData,Real>::getNearestLeaf(const Point3D<Real>& p){
	Point3D<Real> center;
	Real width;
	OctNode<NodeData,Real>* temp;
	int cIndex;
	if(!children){return this;}
	centerAndWidth(center,width);
	temp=this;
	while(temp->children){
		cIndex=CornerIndex(center,p);
		temp=&temp->children[cIndex];
		width/=2;
		if(cIndex&1){center.coords[0]+=width/2;}
		else		{center.coords[0]-=width/2;}
		if(cIndex&2){center.coords[1]+=width/2;}
		else		{center.coords[1]-=width/2;}
		if(cIndex&4){center.coords[2]+=width/2;}
		else		{center.coords[2]-=width/2;}
	}
	return temp;
}
template <class NodeData,class Real>
const OctNode<NodeData,Real>* OctNode<NodeData,Real>::getNearestLeaf(const Point3D<Real>& p) const{
	int nearest;
	Real temp,dist2;
	if(!children){return this;}
	for(int i=0;i<Cube::CORNERS;i++){
		temp=SquareDistance(children[i].center,p);
		if(!i || temp<dist2){
			dist2=temp;
			nearest=i;
		}
	}
	return children[nearest].getNearestLeaf(p);
}

template <class NodeData,class Real>
int OctNode<NodeData,Real>::CommonEdge(const OctNode<NodeData,Real>* node1,const int& eIndex1,const OctNode<NodeData,Real>* node2,const int& eIndex2){
	int o1,o2,i1,i2,j1,j2;

	Cube::FactorEdgeIndex(eIndex1,o1,i1,j1);
	Cube::FactorEdgeIndex(eIndex2,o2,i2,j2);
	if(o1!=o2){return 0;}

	int dir[2];
	int idx1[2];
	int idx2[2];
	switch(o1){
		case 0:	dir[0]=1;	dir[1]=2;	break;
		case 1:	dir[0]=0;	dir[1]=2;	break;
		case 2:	dir[0]=0;	dir[1]=1;	break;
	};
	int d1,d2,off1[3],off2[3];
	node1->depthAndOffset(d1,off1);
	node2->depthAndOffset(d2,off2);
	idx1[0]=off1[dir[0]]+(1<<d1)+i1;
	idx1[1]=off1[dir[1]]+(1<<d1)+j1;
	idx2[0]=off2[dir[0]]+(1<<d2)+i2;
	idx2[1]=off2[dir[1]]+(1<<d2)+j2;
	if(d1>d2){
		idx2[0]<<=(d1-d2);
		idx2[1]<<=(d1-d2);
	}
	else{
		idx1[0]<<=(d2-d1);
		idx1[1]<<=(d2-d1);
	}
	if(idx1[0]==idx2[0] && idx1[1]==idx2[1]){return 1;}
	else									{return 0;}
}
template<class NodeData,class Real>
int OctNode<NodeData,Real>::CornerIndex(const Point3D<Real>& center,const Point3D<Real>& p){
	int cIndex=0;
	if(p.coords[0]>center.coords[0]){cIndex|=1;}
	if(p.coords[1]>center.coords[1]){cIndex|=2;}
	if(p.coords[2]>center.coords[2]){cIndex|=4;}
	return cIndex;
}
template <class NodeData,class Real>
template<class NodeData2>
OctNode<NodeData,Real>& OctNode<NodeData,Real>::operator = (const OctNode<NodeData2,Real>& node){
	int i;
	if(children){delete[] children;}
	children=NULL;

	depth=node.depth;
	for(i=0;i<DIMENSION;i++){this->offset[i] = node.offset[i];}
	if(node.children){
		initChildren();
		for(i=0;i<Cube::CORNERS;i++){children[i] = node.children[i];}
	}
	return *this;
}
template <class NodeData,class Real>
int OctNode<NodeData,Real>::CompareForwardDepths(const void* v1,const void* v2){
	return ((const OctNode<NodeData,Real>*)v1)->depth-((const OctNode<NodeData,Real>*)v2)->depth;
}
template <class NodeData,class Real>
int OctNode<NodeData,Real>::CompareForwardPointerDepths(const void* v1,const void* v2){
	const OctNode<NodeData,Real> *n1,*n2;
	n1=(*(const OctNode<NodeData,Real>**)v1);
	n2=(*(const OctNode<NodeData,Real>**)v2);
	if(n1->d!=n2->d){return int(n1->d)-int(n2->d);}
	while(n1->parent != n2->parent){
		n1=n1->parent;
		n2=n2->parent;
	}
	if(n1->off[0]!=n2->off[0]){return int(n1->off[0])-int(n2->off[0]);}
	if(n1->off[1]!=n2->off[1]){return int(n1->off[1])-int(n2->off[1]);}
	return int(n1->off[2])-int(n2->off[2]);
	return 0;
}
template <class NodeData,class Real>
int OctNode<NodeData,Real>::CompareBackwardDepths(const void* v1,const void* v2){
	return ((const OctNode<NodeData,Real>*)v2)->depth-((const OctNode<NodeData,Real>*)v1)->depth;
}
template <class NodeData,class Real>
int OctNode<NodeData,Real>::CompareBackwardPointerDepths(const void* v1,const void* v2){
	return (*(const OctNode<NodeData,Real>**)v2)->depth()-(*(const OctNode<NodeData,Real>**)v1)->depth();
}
template <class NodeData,class Real>
inline int OctNode<NodeData,Real>::Overlap2(const int &depth1,const int offSet1[DIMENSION],const Real& multiplier1,const int &depth2,const int offSet2[DIMENSION],const Real& multiplier2){
	int d=depth2-depth1;
	Real w=multiplier2+multiplier1*(1<<d);
	Real w2=Real((1<<(d-1))-0.5);
	if(
		fabs(Real(offSet2[0]-(offSet1[0]<<d))-w2)>=w ||
		fabs(Real(offSet2[1]-(offSet1[1]<<d))-w2)>=w ||
		fabs(Real(offSet2[2]-(offSet1[2]<<d))-w2)>=w
		){return 0;}
	return 1;
}
template <class NodeData,class Real>
inline int OctNode<NodeData,Real>::Overlap(const int& c1,const int& c2,const int& c3,const int& dWidth){
	if(c1>=dWidth || c1<=-dWidth || c2>=dWidth || c2<=-dWidth || c3>=dWidth || c3<=-dWidth){return 0;}
	else{return 1;}
}
template <class NodeData,class Real>
OctNode<NodeData,Real>* OctNode<NodeData,Real>::faceNeighbor(const int& faceIndex,const int& forceChildren){return __faceNeighbor(faceIndex>>1,faceIndex&1,forceChildren);}
template <class NodeData,class Real>
const OctNode<NodeData,Real>* OctNode<NodeData,Real>::faceNeighbor(const int& faceIndex) const {return __faceNeighbor(faceIndex>>1,faceIndex&1);}
template <class NodeData,class Real>
OctNode<NodeData,Real>* OctNode<NodeData,Real>::__faceNeighbor(const int& dir,const int& off,const int& forceChildren){
	if(!parent){return NULL;}
	int pIndex=int(this-parent->children);
	pIndex^=(1<<dir);
	if((pIndex & (1<<dir))==(off<<dir)){return &parent->children[pIndex];}
//	if(!(((pIndex>>dir)^off)&1)){return &parent->children[pIndex];}
	else{
		OctNode* temp=parent->__faceNeighbor(dir,off,forceChildren);
		if(!temp){return NULL;}
		if(!temp->children){
			if(forceChildren){temp->initChildren();}
			else{return temp;}
		}
		return &temp->children[pIndex];
	}
}
template <class NodeData,class Real>
const OctNode<NodeData,Real>* OctNode<NodeData,Real>::__faceNeighbor(const int& dir,const int& off) const {
	if(!parent){return NULL;}
	int pIndex=int(this-parent->children);
	pIndex^=(1<<dir);
	if((pIndex & (1<<dir))==(off<<dir)){return &parent->children[pIndex];}
//	if(!(((pIndex>>dir)^off)&1)){return &parent->children[pIndex];}
	else{
		const OctNode* temp=parent->__faceNeighbor(dir,off);
		if(!temp || !temp->children){return temp;}
		else{return &temp->children[pIndex];}
	}
}

template <class NodeData,class Real>
OctNode<NodeData,Real>* OctNode<NodeData,Real>::edgeNeighbor(const int& edgeIndex,const int& forceChildren){
	int idx[2],o,i[2];
	Cube::FactorEdgeIndex(edgeIndex,o,i[0],i[1]);
	switch(o){
		case 0:	idx[0]=1;	idx[1]=2;	break;
		case 1:	idx[0]=0;	idx[1]=2;	break;
		case 2:	idx[0]=0;	idx[1]=1;	break;
	};
	return __edgeNeighbor(o,i,idx,forceChildren);
}
template <class NodeData,class Real>
const OctNode<NodeData,Real>* OctNode<NodeData,Real>::edgeNeighbor(const int& edgeIndex) const {
	int idx[2],o,i[2];
	Cube::FactorEdgeIndex(edgeIndex,o,i[0],i[1]);
	switch(o){
		case 0:	idx[0]=1;	idx[1]=2;	break;
		case 1:	idx[0]=0;	idx[1]=2;	break;
		case 2:	idx[0]=0;	idx[1]=1;	break;
	};
	return __edgeNeighbor(o,i,idx);
}
template <class NodeData,class Real>
const OctNode<NodeData,Real>* OctNode<NodeData,Real>::__edgeNeighbor(const int& o,const int i[2],const int idx[2]) const{
	if(!parent){return NULL;}
	int pIndex=int(this-parent->children);
	int aIndex,x[DIMENSION];

	Cube::FactorCornerIndex(pIndex,x[0],x[1],x[2]);
	aIndex=(~((i[0] ^ x[idx[0]]) | ((i[1] ^ x[idx[1]])<<1))) & 3;
	pIndex^=(7 ^ (1<<o));
	if(aIndex==1)	{	// I can get the neighbor from the parent's face adjacent neighbor
		const OctNode* temp=parent->__faceNeighbor(idx[0],i[0]);
		if(!temp || !temp->children){return NULL;}
		else{return &temp->children[pIndex];}
	}
	else if(aIndex==2)	{	// I can get the neighbor from the parent's face adjacent neighbor
		const OctNode* temp=parent->__faceNeighbor(idx[1],i[1]);
		if(!temp || !temp->children){return NULL;}
		else{return &temp->children[pIndex];}
	}
	else if(aIndex==0)	{	// I can get the neighbor from the parent
		return &parent->children[pIndex];
	}
	else if(aIndex==3)	{	// I can get the neighbor from the parent's edge adjacent neighbor
		const OctNode* temp=parent->__edgeNeighbor(o,i,idx);
		if(!temp || !temp->children){return temp;}
		else{return &temp->children[pIndex];}
	}
	else{return NULL;}
}
template <class NodeData,class Real>
OctNode<NodeData,Real>* OctNode<NodeData,Real>::__edgeNeighbor(const int& o,const int i[2],const int idx[2],const int& forceChildren){
	if(!parent){return NULL;}
	int pIndex=int(this-parent->children);
	int aIndex,x[DIMENSION];

	Cube::FactorCornerIndex(pIndex,x[0],x[1],x[2]);
	aIndex=(~((i[0] ^ x[idx[0]]) | ((i[1] ^ x[idx[1]])<<1))) & 3;
	pIndex^=(7 ^ (1<<o));
	if(aIndex==1)	{	// I can get the neighbor from the parent's face adjacent neighbor
		OctNode* temp=parent->__faceNeighbor(idx[0],i[0],0);
		if(!temp || !temp->children){return NULL;}
		else{return &temp->children[pIndex];}
	}
	else if(aIndex==2)	{	// I can get the neighbor from the parent's face adjacent neighbor
		OctNode* temp=parent->__faceNeighbor(idx[1],i[1],0);
		if(!temp || !temp->children){return NULL;}
		else{return &temp->children[pIndex];}
	}
	else if(aIndex==0)	{	// I can get the neighbor from the parent
		return &parent->children[pIndex];
	}
	else if(aIndex==3)	{	// I can get the neighbor from the parent's edge adjacent neighbor
		OctNode* temp=parent->__edgeNeighbor(o,i,idx,forceChildren);
		if(!temp){return NULL;}
		if(!temp->children){
			if(forceChildren){temp->initChildren();}
			else{return temp;}
		}
		return &temp->children[pIndex];
	}
	else{return NULL;}
}

template <class NodeData,class Real>
const OctNode<NodeData,Real>* OctNode<NodeData,Real>::cornerNeighbor(const int& cornerIndex) const {
	int pIndex,aIndex=0;
	if(!parent){return NULL;}

	pIndex=int(this-parent->children);
	aIndex=(cornerIndex ^ pIndex);	// The disagreement bits
	pIndex=(~pIndex)&7;				// The antipodal point
	if(aIndex==7){					// Agree on no bits
		return &parent->children[pIndex];
	}
	else if(aIndex==0){				// Agree on all bits
		const OctNode* temp=((const OctNode*)parent)->cornerNeighbor(cornerIndex);
		if(!temp || !temp->children){return temp;}
		else{return &temp->children[pIndex];}
	}
	else if(aIndex==6){				// Agree on face 0
		const OctNode* temp=((const OctNode*)parent)->__faceNeighbor(0,cornerIndex & 1);
		if(!temp || !temp->children){return NULL;}
		else{return & temp->children[pIndex];}
	}
	else if(aIndex==5){				// Agree on face 1
		const OctNode* temp=((const OctNode*)parent)->__faceNeighbor(1,(cornerIndex & 2)>>1);
		if(!temp || !temp->children){return NULL;}
		else{return & temp->children[pIndex];}
	}
	else if(aIndex==3){				// Agree on face 2
		const OctNode* temp=((const OctNode*)parent)->__faceNeighbor(2,(cornerIndex & 4)>>2);
		if(!temp || !temp->children){return NULL;}
		else{return & temp->children[pIndex];}
	}
	else if(aIndex==4){				// Agree on edge 2
		const OctNode* temp=((const OctNode*)parent)->edgeNeighbor(8 | (cornerIndex & 1) | (cornerIndex & 2) );
		if(!temp || !temp->children){return NULL;}
		else{return & temp->children[pIndex];}
	}
	else if(aIndex==2){				// Agree on edge 1
		const OctNode* temp=((const OctNode*)parent)->edgeNeighbor(4 | (cornerIndex & 1) | ((cornerIndex & 4)>>1) );
		if(!temp || !temp->children){return NULL;}
		else{return & temp->children[pIndex];}
	}
	else if(aIndex==1){				// Agree on edge 0
		const OctNode* temp=((const OctNode*)parent)->edgeNeighbor(((cornerIndex & 2) | (cornerIndex & 4))>>1 );
		if(!temp || !temp->children){return NULL;}
		else{return & temp->children[pIndex];}
	}
	else{return NULL;}
}
template <class NodeData,class Real>
OctNode<NodeData,Real>* OctNode<NodeData,Real>::cornerNeighbor(const int& cornerIndex,const int& forceChildren){
	int pIndex,aIndex=0;
	if(!parent){return NULL;}

	pIndex=int(this-parent->children);
	aIndex=(cornerIndex ^ pIndex);	// The disagreement bits
	pIndex=(~pIndex)&7;				// The antipodal point
	if(aIndex==7){					// Agree on no bits
		return &parent->children[pIndex];
	}
	else if(aIndex==0){				// Agree on all bits
		OctNode* temp=((OctNode*)parent)->cornerNeighbor(cornerIndex,forceChildren);
		if(!temp){return NULL;}
		if(!temp->children){
			if(forceChildren){temp->initChildren();}
			else{return temp;}
		}
		return &temp->children[pIndex];
	}
	else if(aIndex==6){				// Agree on face 0
		OctNode* temp=((OctNode*)parent)->__faceNeighbor(0,cornerIndex & 1,0);
		if(!temp || !temp->children){return NULL;}
		else{return & temp->children[pIndex];}
	}
	else if(aIndex==5){				// Agree on face 1
		OctNode* temp=((OctNode*)parent)->__faceNeighbor(1,(cornerIndex & 2)>>1,0);
		if(!temp || !temp->children){return NULL;}
		else{return & temp->children[pIndex];}
	}
	else if(aIndex==3){				// Agree on face 2
		OctNode* temp=((OctNode*)parent)->__faceNeighbor(2,(cornerIndex & 4)>>2,0);
		if(!temp || !temp->children){return NULL;}
		else{return & temp->children[pIndex];}
	}
	else if(aIndex==4){				// Agree on edge 2
		OctNode* temp=((OctNode*)parent)->edgeNeighbor(8 | (cornerIndex & 1) | (cornerIndex & 2) );
		if(!temp || !temp->children){return NULL;}
		else{return & temp->children[pIndex];}
	}
	else if(aIndex==2){				// Agree on edge 1
		OctNode* temp=((OctNode*)parent)->edgeNeighbor(4 | (cornerIndex & 1) | ((cornerIndex & 4)>>1) );
		if(!temp || !temp->children){return NULL;}
		else{return & temp->children[pIndex];}
	}
	else if(aIndex==1){				// Agree on edge 0
		OctNode* temp=((OctNode*)parent)->edgeNeighbor(((cornerIndex & 2) | (cornerIndex & 4))>>1 );
		if(!temp || !temp->children){return NULL;}
		else{return & temp->children[pIndex];}
	}
	else{return NULL;}
}
////////////////////////
// OctNodeNeighborKey //
////////////////////////
template<class NodeData,class Real>
OctNode<NodeData,Real>::Neighbors::Neighbors(void){clear();}
template<class NodeData,class Real>
void OctNode<NodeData,Real>::Neighbors::clear(void){
	for(int i=0;i<3;i++){for(int j=0;j<3;j++){for(int k=0;k<3;k++){neighbors[i][j][k]=NULL;}}}
}
template<class NodeData,class Real>
OctNode<NodeData,Real>::NeighborKey::NeighborKey(void){neighbors=NULL;}
template<class NodeData,class Real>
OctNode<NodeData,Real>::NeighborKey::~NeighborKey(void){
	if(neighbors){delete[] neighbors;}
	neighbors=NULL;
}

template<class NodeData,class Real>
void OctNode<NodeData,Real>::NeighborKey::set(const int& d){
	if(neighbors){delete[] neighbors;}
	neighbors=NULL;
	if(d<0){return;}
	neighbors=new Neighbors[d+1];
}
template<class NodeData,class Real>
typename OctNode<NodeData,Real>::Neighbors& OctNode<NodeData,Real>::NeighborKey::setNeighbors(OctNode<NodeData,Real>* node){
	int d=node->depth();
	if(node!=neighbors[d].neighbors[1][1][1]){
		neighbors[d].clear();

		if(!node->parent){neighbors[d].neighbors[1][1][1]=node;}
		else{
			int i,j,k,x1,y1,z1,x2,y2,z2;
			int idx=int(node-node->parent->children);
			Cube::FactorCornerIndex(  idx   ,x1,y1,z1);
			Cube::FactorCornerIndex((~idx)&7,x2,y2,z2);
			for(i=0;i<2;i++){
				for(j=0;j<2;j++){
					for(k=0;k<2;k++){
						neighbors[d].neighbors[x2+i][y2+j][z2+k]=&node->parent->children[Cube::CornerIndex(i,j,k)];
					}
				}
			}
			Neighbors& temp=setNeighbors(node->parent);

			// Set the neighbors from across the faces
			i=x1<<1;
			if(temp.neighbors[i][1][1]){
				if(!temp.neighbors[i][1][1]->children){temp.neighbors[i][1][1]->initChildren();}
				for(j=0;j<2;j++){for(k=0;k<2;k++){neighbors[d].neighbors[i][y2+j][z2+k]=&temp.neighbors[i][1][1]->children[Cube::CornerIndex(x2,j,k)];}}
			}
			j=y1<<1;
			if(temp.neighbors[1][j][1]){
				if(!temp.neighbors[1][j][1]->children){temp.neighbors[1][j][1]->initChildren();}
				for(i=0;i<2;i++){for(k=0;k<2;k++){neighbors[d].neighbors[x2+i][j][z2+k]=&temp.neighbors[1][j][1]->children[Cube::CornerIndex(i,y2,k)];}}
			}
			k=z1<<1;
			if(temp.neighbors[1][1][k]){
				if(!temp.neighbors[1][1][k]->children){temp.neighbors[1][1][k]->initChildren();}
				for(i=0;i<2;i++){for(j=0;j<2;j++){neighbors[d].neighbors[x2+i][y2+j][k]=&temp.neighbors[1][1][k]->children[Cube::CornerIndex(i,j,z2)];}}
			}

			// Set the neighbors from across the edges
			i=x1<<1;	j=y1<<1;
			if(temp.neighbors[i][j][1]){
				if(!temp.neighbors[i][j][1]->children){temp.neighbors[i][j][1]->initChildren();}
				for(k=0;k<2;k++){neighbors[d].neighbors[i][j][z2+k]=&temp.neighbors[i][j][1]->children[Cube::CornerIndex(x2,y2,k)];}
			}
			i=x1<<1;	k=z1<<1;
			if(temp.neighbors[i][1][k]){
				if(!temp.neighbors[i][1][k]->children){temp.neighbors[i][1][k]->initChildren();}
				for(j=0;j<2;j++){neighbors[d].neighbors[i][y2+j][k]=&temp.neighbors[i][1][k]->children[Cube::CornerIndex(x2,j,z2)];}
			}
			j=y1<<1;	k=z1<<1;
			if(temp.neighbors[1][j][k]){
				if(!temp.neighbors[1][j][k]->children){temp.neighbors[1][j][k]->initChildren();}
				for(i=0;i<2;i++){neighbors[d].neighbors[x2+i][j][k]=&temp.neighbors[1][j][k]->children[Cube::CornerIndex(i,y2,z2)];}
			}

			// Set the neighbor from across the corner
			i=x1<<1;	j=y1<<1;	k=z1<<1;
			if(temp.neighbors[i][j][k]){
				if(!temp.neighbors[i][j][k]->children){temp.neighbors[i][j][k]->initChildren();}
				neighbors[d].neighbors[i][j][k]=&temp.neighbors[i][j][k]->children[Cube::CornerIndex(x2,y2,z2)];
			}
		}
	}
	return neighbors[d];
}

template<class NodeData,class Real>
typename OctNode<NodeData,Real>::Neighbors& OctNode<NodeData,Real>::NeighborKey::getNeighbors(OctNode<NodeData,Real>* node){
	int d=node->depth();
	if(node!=neighbors[d].neighbors[1][1][1]){
		neighbors[d].clear();

		if(!node->parent){neighbors[d].neighbors[1][1][1]=node;}
		else{
			int i,j,k,x1,y1,z1,x2,y2,z2;
			int idx=int(node-node->parent->children);
			Cube::FactorCornerIndex(  idx   ,x1,y1,z1);
			Cube::FactorCornerIndex((~idx)&7,x2,y2,z2);
			for(i=0;i<2;i++){
				for(j=0;j<2;j++){
					for(k=0;k<2;k++){
						neighbors[d].neighbors[x2+i][y2+j][z2+k]=&node->parent->children[Cube::CornerIndex(i,j,k)];
					}
				}
			}
			Neighbors& temp=getNeighbors(node->parent);

			// Set the neighbors from across the faces
			i=x1<<1;
			if(temp.neighbors[i][1][1] && temp.neighbors[i][1][1]->children){
				for(j=0;j<2;j++){for(k=0;k<2;k++){neighbors[d].neighbors[i][y2+j][z2+k]=&temp.neighbors[i][1][1]->children[Cube::CornerIndex(x2,j,k)];}}
			}
			j=y1<<1;
			if(temp.neighbors[1][j][1] && temp.neighbors[1][j][1]->children){
				for(i=0;i<2;i++){for(k=0;k<2;k++){neighbors[d].neighbors[x2+i][j][z2+k]=&temp.neighbors[1][j][1]->children[Cube::CornerIndex(i,y2,k)];}}
			}
			k=z1<<1;
			if(temp.neighbors[1][1][k] && temp.neighbors[1][1][k]->children){
				for(i=0;i<2;i++){for(j=0;j<2;j++){neighbors[d].neighbors[x2+i][y2+j][k]=&temp.neighbors[1][1][k]->children[Cube::CornerIndex(i,j,z2)];}}
			}

			// Set the neighbors from across the edges
			i=x1<<1;	j=y1<<1;
			if(temp.neighbors[i][j][1] && temp.neighbors[i][j][1]->children){
				for(k=0;k<2;k++){neighbors[d].neighbors[i][j][z2+k]=&temp.neighbors[i][j][1]->children[Cube::CornerIndex(x2,y2,k)];}
			}
			i=x1<<1;	k=z1<<1;
			if(temp.neighbors[i][1][k] && temp.neighbors[i][1][k]->children){
				for(j=0;j<2;j++){neighbors[d].neighbors[i][y2+j][k]=&temp.neighbors[i][1][k]->children[Cube::CornerIndex(x2,j,z2)];}
			}
			j=y1<<1;	k=z1<<1;
			if(temp.neighbors[1][j][k] && temp.neighbors[1][j][k]->children){
				for(i=0;i<2;i++){neighbors[d].neighbors[x2+i][j][k]=&temp.neighbors[1][j][k]->children[Cube::CornerIndex(i,y2,z2)];}
			}

			// Set the neighbor from across the corner
			i=x1<<1;	j=y1<<1;	k=z1<<1;
			if(temp.neighbors[i][j][k] && temp.neighbors[i][j][k]->children){
				neighbors[d].neighbors[i][j][k]=&temp.neighbors[i][j][k]->children[Cube::CornerIndex(x2,y2,z2)];
			}
		}
	}
	return neighbors[node->depth()];
}

/////////////////////////
// OctNodeNeighborKey2 //
/////////////////////////
template<class NodeData,class Real>
OctNode<NodeData,Real>::Neighbors2::Neighbors2(void){clear();}
template<class NodeData,class Real>
void OctNode<NodeData,Real>::Neighbors2::clear(void){
	for(int i=0;i<3;i++){for(int j=0;j<3;j++){for(int k=0;k<3;k++){neighbors[i][j][k]=NULL;}}}
}
template<class NodeData,class Real>
OctNode<NodeData,Real>::NeighborKey2::NeighborKey2(void){neighbors=NULL;}
template<class NodeData,class Real>
OctNode<NodeData,Real>::NeighborKey2::~NeighborKey2(void){
	if(neighbors){delete[] neighbors;}
	neighbors=NULL;
}

template<class NodeData,class Real>
void OctNode<NodeData,Real>::NeighborKey2::set(const int& d){
	if(neighbors){delete[] neighbors;}
	neighbors=NULL;
	if(d<0){return;}
	neighbors=new Neighbors2[d+1];
}
template<class NodeData,class Real>
typename OctNode<NodeData,Real>::Neighbors2& OctNode<NodeData,Real>::NeighborKey2::getNeighbors(const OctNode<NodeData,Real>* node){
	int d=node->depth();
	if(node!=neighbors[d].neighbors[1][1][1]){
		neighbors[d].clear();

		if(!node->parent){neighbors[d].neighbors[1][1][1]=node;}
		else{
			int i,j,k,x1,y1,z1,x2,y2,z2;
			int idx=int(node-node->parent->children);
			Cube::FactorCornerIndex(  idx   ,x1,y1,z1);
			Cube::FactorCornerIndex((~idx)&7,x2,y2,z2);
			for(i=0;i<2;i++){
				for(j=0;j<2;j++){
					for(k=0;k<2;k++){
						neighbors[d].neighbors[x2+i][y2+j][z2+k]=&node->parent->children[Cube::CornerIndex(i,j,k)];
					}
				}
			}
			Neighbors2& temp=getNeighbors(node->parent);

			// Set the neighbors from across the faces
			i=x1<<1;
			if(temp.neighbors[i][1][1] && temp.neighbors[i][1][1]->children){
				for(j=0;j<2;j++){for(k=0;k<2;k++){neighbors[d].neighbors[i][y2+j][z2+k]=&temp.neighbors[i][1][1]->children[Cube::CornerIndex(x2,j,k)];}}
			}
			j=y1<<1;
			if(temp.neighbors[1][j][1] && temp.neighbors[1][j][1]->children){
				for(i=0;i<2;i++){for(k=0;k<2;k++){neighbors[d].neighbors[x2+i][j][z2+k]=&temp.neighbors[1][j][1]->children[Cube::CornerIndex(i,y2,k)];}}
			}
			k=z1<<1;
			if(temp.neighbors[1][1][k] && temp.neighbors[1][1][k]->children){
				for(i=0;i<2;i++){for(j=0;j<2;j++){neighbors[d].neighbors[x2+i][y2+j][k]=&temp.neighbors[1][1][k]->children[Cube::CornerIndex(i,j,z2)];}}
			}

			// Set the neighbors from across the edges
			i=x1<<1;	j=y1<<1;
			if(temp.neighbors[i][j][1] && temp.neighbors[i][j][1]->children){
				for(k=0;k<2;k++){neighbors[d].neighbors[i][j][z2+k]=&temp.neighbors[i][j][1]->children[Cube::CornerIndex(x2,y2,k)];}
			}
			i=x1<<1;	k=z1<<1;
			if(temp.neighbors[i][1][k] && temp.neighbors[i][1][k]->children){
				for(j=0;j<2;j++){neighbors[d].neighbors[i][y2+j][k]=&temp.neighbors[i][1][k]->children[Cube::CornerIndex(x2,j,z2)];}
			}
			j=y1<<1;	k=z1<<1;
			if(temp.neighbors[1][j][k] && temp.neighbors[1][j][k]->children){
				for(i=0;i<2;i++){neighbors[d].neighbors[x2+i][j][k]=&temp.neighbors[1][j][k]->children[Cube::CornerIndex(i,y2,z2)];}
			}

			// Set the neighbor from across the corner
			i=x1<<1;	j=y1<<1;	k=z1<<1;
			if(temp.neighbors[i][j][k] && temp.neighbors[i][j][k]->children){
				neighbors[d].neighbors[i][j][k]=&temp.neighbors[i][j][k]->children[Cube::CornerIndex(x2,y2,z2)];
			}
		}
	}
	return neighbors[node->depth()];
}

/*
template <class NodeData,class Real>
int OctNode<NodeData,Real>::write(const char* fileName) const{
	FILE* fp=fopen(fileName,"wb");
	if(!fp){return 0;}
	int ret=write(fp);
	fclose(fp);
	return ret;
}
template <class NodeData,class Real>
int OctNode<NodeData,Real>::write(FILE* fp) const{
	fwrite(this,sizeof(OctNode<NodeData,Real>),1,fp);
	if(children){for(int i=0;i<Cube::CORNERS;i++){children[i].write(fp);}}
	return 1;
}
template <class NodeData,class Real>
int OctNode<NodeData,Real>::read(const char* fileName){
	FILE* fp=fopen(fileName,"rb");
	if(!fp){return 0;}
	int ret=read(fp);
	fclose(fp);
	return ret;
}
template <class NodeData,class Real>
int OctNode<NodeData,Real>::read(FILE* fp){
	fread(this,sizeof(OctNode<NodeData,Real>),1,fp);
	parent=NULL;
	if(children){
		children=NULL;
		initChildren();
		for(int i=0;i<Cube::CORNERS;i++){
			children[i].read(fp);
			children[i].parent=this;
		}
	}
	return 1;
}
*/

template<class NodeData,class Real>
int OctNode<NodeData,Real>::width(const int& maxDepth) const {
	int d=depth();
	return 1<<(maxDepth-d); 
}
template<class NodeData,class Real>
void OctNode<NodeData,Real>::centerIndex(const int& maxDepth,int index[DIMENSION]) const {
	int d,o[3];
	depthAndOffset(d,o);
	for(int i=0;i<DIMENSION;i++){index[i]=BinaryNode<Real>::CornerIndex(maxDepth,d+1,o[i]<<1,1);}
}

////////////////////////
// StartingPolynomial //
////////////////////////
template<int Degree>
template<int Degree2>
StartingPolynomial<Degree+Degree2> StartingPolynomial<Degree>::operator * (const StartingPolynomial<Degree2>& p) const{
	StartingPolynomial<Degree+Degree2> sp;
	if(start>p.start){sp.start=start;}
	else{sp.start=p.start;}
	sp.p=this->p*p.p;
	return sp;
}
template<int Degree>
StartingPolynomial<Degree> StartingPolynomial<Degree>::scale(const double& s) const{
	StartingPolynomial q;
	q.start=start*s;
	q.p=p.Scale(s);
	return q;
}
template<int Degree>
StartingPolynomial<Degree> StartingPolynomial<Degree>::shift(const double& s) const{
	StartingPolynomial q;
	q.start=start+s;
	q.p=p.Shift(s);
	return q;
}


template<int Degree>
int StartingPolynomial<Degree>::operator < (const StartingPolynomial<Degree>& sp) const{
	if(start<sp.start){return 1;}
	else{return 0;}
}
template<int Degree>
int StartingPolynomial<Degree>::Compare(const void* v1,const void* v2){
	double d=((StartingPolynomial*)(v1))->start-((StartingPolynomial*)(v2))->start;
	if		(d<0)	{return -1;}
	else if	(d>0)	{return  1;}
	else			{return  0;}
}



////////////////////
//  SparseNMatrix //
////////////////////
///////////////////////////////////////////
// Static Allocator Methods and Memebers //
///////////////////////////////////////////
template<class T,int Dim> int SparseNMatrix<T,Dim>::UseAlloc=0;
template<class T,int Dim> Allocator<NMatrixEntry<T,Dim> > SparseNMatrix<T,Dim>::Allocator;
template<class T,int Dim> int SparseNMatrix<T,Dim>::UseAllocator(void){return UseAlloc;}
template<class T,int Dim>
void SparseNMatrix<T,Dim>::SetAllocator(const int& blockSize){
	if(blockSize>0){
		UseAlloc=1;
		Allocator.Set(blockSize);
	}
	else{UseAlloc=0;}
}
////////////////////////////////////////
// SparseNMatrix Methods and Memebers //
////////////////////////////////////////

template<class T,int Dim>
SparseNMatrix<T,Dim>::SparseNMatrix()
{
	rows=0;
	rowSizes=NULL;
	m_ppElements=NULL;
}

template<class T,int Dim>
SparseNMatrix<T,Dim>::SparseNMatrix( int rows ){Resize(rows);}

template<class T,int Dim>
SparseNMatrix<T,Dim>::SparseNMatrix( const SparseNMatrix& M )
{
	Resize(M.rows);
	for (int i=0; i<rows; i++){
		SetRowSize(i,M.rowSizes[i]);
		for(int j=0;j<rowSizes[i];j++){m_ppElements[i][j]=M.m_ppElements[i][j];}
	}
}
template<class T,int Dim>
int SparseNMatrix<T,Dim>::Entries(void){
	int e=0;
	for(int i=0;i<rows;i++){e+=int(rowSizes[i]);}
	return e;
}
template<class T,int Dim>
SparseNMatrix<T,Dim>& SparseNMatrix<T,Dim>::operator = (const SparseNMatrix<T,Dim>& M)
{
	Resize(M.rows);
	for (int i=0; i<rows; i++){
		SetRowSize(i,M.rowSizes[i]);
		for (int j=0; j<rowSizes[i]; j++){m_ppElements[i][j]=M.m_ppElements[i][j];}
	}
	return *this;
}

template<class T,int Dim>
SparseNMatrix<T,Dim>::~SparseNMatrix(){Resize(0);}

template<class T,int Dim>
void SparseNMatrix<T,Dim>::Resize( int r )
{
	int i;
	if(rows>0){
		if(!UseAlloc){for(i=0;i<rows;i++){if(rowSizes[i]){free(m_ppElements[i]);}}}
		free(m_ppElements);
		free(rowSizes);
	}
	rows=r;
	if(r){
		rowSizes=(int*)malloc(sizeof(int)*r);
		memset(rowSizes,0,sizeof(int)*r);
		m_ppElements=(NMatrixEntry<T,Dim>**)malloc(sizeof(NMatrixEntry<T,Dim>*)*r);
	}
}

template<class T,int Dim>
void SparseNMatrix<T,Dim>::SetRowSize(int row,int count){
	if(row>=0 && row<rows){
		if(UseAlloc){m_ppElements[row]=Allocator.NewElements(count);}
		else{
			if(rowSizes[row]){free(m_ppElements[row]);}
			if(count>0){m_ppElements[row]=(NMatrixEntry<T,Dim>*)malloc(sizeof(NMatrixEntry<T,Dim>)*count);}
		}
		rowSizes[row]=count;
	}
}

template<class T,int Dim>
SparseNMatrix<T,Dim> SparseNMatrix<T,Dim>::operator * (const T& V) const
{
	SparseNMatrix<T,Dim> M(*this);
	M *= V;
	return M;
}

template<class T,int Dim>
SparseNMatrix<T,Dim>& SparseNMatrix<T,Dim>::operator *= (const T& V)
{
	for (int i=0; i<rows(); i++)
	{
		for(int ii=0;ii<m_ppElements[i];i++){
			for(int jj=0;jj<Dim;jj++){
				m_ppElements[i][ii].Value[jj]*=V;
			}
		}
	}
	return *this;
}

template<class T,int Dim>
template<class T2>
NVector<T2,Dim> SparseNMatrix<T,Dim>::operator * (const Vector<T2>& V) const
{
	NVector<T2,Dim> R( rows );
	
	for (int i=0; i<rows; i++)
	{
		T2 temp[Dim];
		for(int ii=0;ii<Dim;ii++){temp[ii]=T2();}
		for(int ii=0;ii<rowSizes[i];ii++){
			for(int jj=0;jj<Dim;jj++){temp[jj]+=m_ppElements[i][ii].Value[jj]*V.m_pV[m_ppElements[i][jj].N];}
		}
		for(int ii=0;ii<Dim;ii++){R[i][ii]=temp[ii];}
	}
	return R;
}

template<class T,int Dim>
template<class T2>
Vector<T2> SparseNMatrix<T,Dim>::operator * (const NVector<T2,Dim>& V) const
{
	Vector<T2> R( rows );
	
	for (int i=0; i<rows; i++)
	{
		T2 temp();
		for(int ii=0;ii<rowSizes[i];ii++){
			for(int jj=0;jj<Dim;jj++){temp+=m_ppElements[i][ii].Value[jj]*V.m_pV[m_ppElements[i][ii].N][jj];}
		}
		R(i)=temp;
	}
	return R;
}

///////////////////////////
// SparseSymmetricMatrix //
///////////////////////////
template<class T>
template<class T2>
Vector<T2> SparseSymmetricMatrix<T>::operator * (const Vector<T2>& V) const {return Multiply(V);}
template<class T>
template<class T2>
Vector<T2> SparseSymmetricMatrix<T>::Multiply( const Vector<T2>& V ) const
{
	Vector<T2> R( this->rows );
	
	for (int i=0; i<this->rows; i++){
		for(int ii=0;ii<this->rowSizes[i];ii++){
			int j=this->m_ppElements[i][ii].N;
			R(i)+=this->m_ppElements[i][ii].Value * V.m_pV[j];
			R(j)+=this->m_ppElements[i][ii].Value * V.m_pV[i];
		}
	}
	return R;
}

template<class T>
template<class T2>
void SparseSymmetricMatrix<T>::Multiply( const Vector<T2>& In,Vector<T2>& Out) const
{
	Out.SetZero();
	for (int i=0; i<this->rows; i++){
		MatrixEntry<T>* temp=this->m_ppElements[i];
		T2& in1=In.m_pV[i];
		T2& out1=Out.m_pV[i];
		int rs=this->rowSizes[i];
		for(int ii=0;ii<rs;ii++){
			MatrixEntry<T>& temp2=temp[ii];
			int j=temp2.N;
			T2 v=temp2.Value;
			out1+=v * In.m_pV[j];
			Out.m_pV[j]+=v * in1;
		}
	}
}

template<class T>
template<class T2>
int SparseSymmetricMatrix<T>::Solve(const SparseSymmetricMatrix<T>& M,const Vector<T2>& b,const int& iters,Vector<T2>& solution,const T2 eps,const int& reset){
	Vector<T2> d,r,Md;
	T2 alpha,beta,rDotR,bDotB;
	Md.Resize(b.Dimensions());
	if(reset){
		solution.Resize(b.Dimensions());
		solution.SetZero();
	}
	d=r=b-M.Multiply(solution);
	rDotR=r.Dot(r);
	bDotB=b.Dot(b);
	if(b.Dot(b)<=eps){
		solution.SetZero();
		return 0;
	}
	int i;
	for(i=0;i<iters;i++){
		T2 temp;
		M.Multiply(d,Md);
		temp=d.Dot(Md);
		if(fabs(temp)<=eps){break;}
		alpha=rDotR/temp;
		r.SubtractScaled(Md,alpha);
		temp=r.Dot(r);
		if(temp/bDotB<=eps){break;}
		beta=temp/rDotR;
		solution.AddScaled(d,alpha);
		if(beta<=eps){break;}
		rDotR=temp;
		Vector<T2>::Add(d,beta,r,d);
	}
	return i;
}

template<class T>
template<class T2>
int SparseSymmetricMatrix<T>::Solve(const SparseSymmetricMatrix<T>& M,const Vector<T>& diagonal,const Vector<T2>& b,const int& iters,Vector<T2>& solution,const T2 eps,const int& reset){
	Vector<T2> d,r,Md;

	if(reset){
		solution.Resize(b.Dimensions());
		solution.SetZero();
	}
	Md.Resize(M.rows);
	for(int i=0;i<iters;i++){
		M.Multiply(solution,Md);
		r=b-Md;
		for(int j=0;j<int(M.rows);j++){solution[j]+=r[j]/diagonal[j];}
	}
	return iters;
}

const Real EPSILON=Real(1e-6);
const Real ROUND_EPS=Real(1e-5);
/////////////////////
// SortedTreeNodes //
/////////////////////
SortedTreeNodes::SortedTreeNodes(void){
	nodeCount=NULL;
	treeNodes=NULL;
	maxDepth=0;
}
SortedTreeNodes::~SortedTreeNodes(void){
	if(nodeCount){delete[] nodeCount;}
	if(treeNodes){delete[] treeNodes;}
	nodeCount=NULL;
	treeNodes=NULL;
}
void SortedTreeNodes::Set(TreeOctNode& root,const int& setIndex){
	if(nodeCount){delete[] nodeCount;}
	if(treeNodes){delete[] treeNodes;}
	maxDepth=root.maxDepth()+1;
	nodeCount=new int[maxDepth+1];
	treeNodes=new TreeOctNode*[root.nodes()];

	TreeOctNode* temp=root.nextNode();
	int i,cnt=0;
	while(temp){
		treeNodes[cnt++]=temp;
		temp=root.nextNode(temp);
	}
	qsort(treeNodes,cnt,sizeof(const TreeOctNode*),TreeOctNode::CompareForwardPointerDepths);
	for(i=0;i<=maxDepth;i++){nodeCount[i]=0;}
	for(i=0;i<cnt;i++){
		if(setIndex){treeNodes[i]->nodeData.nodeIndex=i;}
		nodeCount[treeNodes[i]->depth()+1]++;
	}
	for(i=1;i<=maxDepth;i++){nodeCount[i]+=nodeCount[i-1];}
}


//////////////////
// TreeNodeData //
//////////////////
int TreeNodeData::UseIndex=1;
TreeNodeData::TreeNodeData(void){
	if(UseIndex){
		nodeIndex=-1;
		centerWeightContribution=0;
	}
	else{mcIndex=0;}
	value=0;
}
TreeNodeData::~TreeNodeData(void){;}


////////////
// Octree //
////////////
template<int Degree>
double Octree<Degree>::maxMemoryUsage=0;

template<int Degree>
double Octree<Degree>::MemoryUsage(void){
	double mem=MemoryInfo::Usage()/(1<<20);
	if(mem>maxMemoryUsage){maxMemoryUsage=mem;}
	return mem;
}

template<int Degree>
Octree<Degree>::Octree(void){
	radius=0;
	width=0;
	postNormalSmooth=0;
}

template<int Degree>
void Octree<Degree>::setNodeIndices(TreeOctNode& node,int& idx){
	node.nodeData.nodeIndex=idx;
	idx++;
	if(node.children){for(int i=0;i<Cube::CORNERS;i++){setNodeIndices(node.children[i],idx);}}
}
template<int Degree>
int Octree<Degree>::NonLinearSplatOrientedPoint(TreeOctNode* node,const Point3D<Real>& position,const Point3D<Real>& normal){
	double x,dxdy,dxdydz,dx[DIMENSION][3];
	int i,j,k;
	TreeOctNode::Neighbors& neighbors=neighborKey.setNeighbors(node);
	double width;
	Point3D<Real> center;
	Real w;

	node->centerAndWidth(center,w);
	width=w;
	for(i=0;i<3;i++){
		x=(center.coords[i]-position.coords[i]-width)/width;
		dx[i][0]=1.125+1.500*x+0.500*x*x;
		x=(center.coords[i]-position.coords[i])/width;
		dx[i][1]=0.750        -      x*x;
		dx[i][2]=1.0-dx[i][1]-dx[i][0];
	}
	for(i=0;i<3;i++){
		for(j=0;j<3;j++){
			dxdy=dx[0][i]*dx[1][j];
			for(k=0;k<3;k++){
				if(neighbors.neighbors[i][j][k]){
					dxdydz=dxdy*dx[2][k];
					int idx=neighbors.neighbors[i][j][k]->nodeData.nodeIndex;
					if(idx<0){
						Point3D<Real> n;
						n.coords[0]=n.coords[1]=n.coords[2]=0;
						idx=neighbors.neighbors[i][j][k]->nodeData.nodeIndex=int(normals->size());
						normals->push_back(n);
					}
					(*normals)[idx].coords[0]+=Real(normal.coords[0]*dxdydz);
					(*normals)[idx].coords[1]+=Real(normal.coords[1]*dxdydz);
					(*normals)[idx].coords[2]+=Real(normal.coords[2]*dxdydz);
				}
			}
		}
	}
	return 0;
}
template<int Degree>
void Octree<Degree>::NonLinearSplatOrientedPoint(const Point3D<Real>& position,const Point3D<Real>& normal,const int& splatDepth,const Real& samplesPerNode,
												 const int& minDepth,const int& maxDepth){
	double dx;
	Point3D<Real> n;
	TreeOctNode* temp;
	int i,cnt=0;
	double width;
	Point3D<Real> myCenter;
	Real myWidth;
	myCenter.coords[0]=myCenter.coords[1]=myCenter.coords[2]=Real(0.5);
	myWidth=Real(1.0);

	temp=&tree;
	while(temp->depth()<splatDepth){
		if(!temp->children){
			printf("Octree<Degree>::NonLinearSplatOrientedPoint error\n");
			return;
		}
		int cIndex=TreeOctNode::CornerIndex(myCenter,position);
		temp=&temp->children[cIndex];
		myWidth/=2;
		if(cIndex&1){myCenter.coords[0]+=myWidth/2;}
		else		{myCenter.coords[0]-=myWidth/2;}
		if(cIndex&2){myCenter.coords[1]+=myWidth/2;}
		else		{myCenter.coords[1]-=myWidth/2;}
		if(cIndex&4){myCenter.coords[2]+=myWidth/2;}
		else		{myCenter.coords[2]-=myWidth/2;}
	}
	Real alpha,newDepth;
	NonLinearGetSampleDepthAndWeight(temp,position,samplesPerNode,newDepth,alpha);

	if(newDepth<minDepth){newDepth=Real(minDepth);}
	if(newDepth>maxDepth){newDepth=Real(maxDepth);}
	int topDepth=int(ceil(newDepth));

	dx=1.0-(topDepth-newDepth);
	if(topDepth<=minDepth){
		topDepth=minDepth;
		dx=1;
	}
	else if(topDepth>maxDepth){
		topDepth=maxDepth;
		dx=1;
	}
	while(temp->depth()>topDepth){temp=temp->parent;}
	while(temp->depth()<topDepth){
		if(!temp->children){temp->initChildren();}
		int cIndex=TreeOctNode::CornerIndex(myCenter,position);
		temp=&temp->children[cIndex];
		myWidth/=2;
		if(cIndex&1){myCenter.coords[0]+=myWidth/2;}
		else		{myCenter.coords[0]-=myWidth/2;}
		if(cIndex&2){myCenter.coords[1]+=myWidth/2;}
		else		{myCenter.coords[1]-=myWidth/2;}
		if(cIndex&4){myCenter.coords[2]+=myWidth/2;}
		else		{myCenter.coords[2]-=myWidth/2;}
	}
	width=1.0/(1<<temp->depth());
	for(i=0;i<DIMENSION;i++){n.coords[i]=normal.coords[i]*alpha/Real(pow(width,3))*Real(dx);}
	NonLinearSplatOrientedPoint(temp,position,n);
	if(fabs(1.0-dx)>EPSILON){
		dx=Real(1.0-dx);
		temp=temp->parent;
		width=1.0/(1<<temp->depth());

		for(i=0;i<DIMENSION;i++){n.coords[i]=normal.coords[i]*alpha/Real(pow(width,3))*Real(dx);}
		NonLinearSplatOrientedPoint(temp,position,n);
	}
}
template<int Degree>
void Octree<Degree>::NonLinearGetSampleDepthAndWeight(TreeOctNode* node,const Point3D<Real>& position,const Real& samplesPerNode,Real& depth,Real& weight){
	TreeOctNode* temp=node;
	weight=Real(1.0)/NonLinearGetSampleWeight(temp,position);
	if(weight>=samplesPerNode+1){depth=Real(temp->depth()+log(weight/(samplesPerNode+1))/log(double(1<<(DIMENSION-1))));}
	else{
		Real oldAlpha,newAlpha;
		oldAlpha=newAlpha=weight;
		while(newAlpha<(samplesPerNode+1) && temp->parent){
			temp=temp->parent;
			oldAlpha=newAlpha;
			newAlpha=Real(1.0)/NonLinearGetSampleWeight(temp,position);
		}
		depth=Real(temp->depth()+log(newAlpha/(samplesPerNode+1))/log(newAlpha/oldAlpha));
	}
	weight=Real(pow(double(1<<(DIMENSION-1)),-double(depth)));
}

template<int Degree>
Real Octree<Degree>::NonLinearGetSampleWeight(TreeOctNode* node,const Point3D<Real>& position){
	Real weight=0;
	double x,dxdy,dx[DIMENSION][3];
	int i,j,k;
	TreeOctNode::Neighbors& neighbors=neighborKey.setNeighbors(node);
	double width;
	Point3D<Real> center;
	Real w;
	node->centerAndWidth(center,w);
	width=w;

	for(i=0;i<DIMENSION;i++){
		x=(center.coords[i]-position.coords[i]-width)/width;
		dx[i][0]=1.125+1.500*x+0.500*x*x;
		x=(center.coords[i]-position.coords[i])/width;
		dx[i][1]=0.750        -      x*x;
		dx[i][2]=1.0-dx[i][1]-dx[i][0];
	}

	for(i=0;i<3;i++){
		for(j=0;j<3;j++){
			dxdy=dx[0][i]*dx[1][j];
			for(k=0;k<3;k++){
				if(neighbors.neighbors[i][j][k]){
					weight+=Real(dxdy*dx[2][k]*neighbors.neighbors[i][j][k]->nodeData.centerWeightContribution);
				}
			}
		}
	}
	return Real(1.0/weight);
}

template<int Degree>
int Octree<Degree>::NonLinearUpdateWeightContribution(TreeOctNode* node,const Point3D<Real>& position,const Real& weight){
	int i,j,k;
	TreeOctNode::Neighbors& neighbors=neighborKey.setNeighbors(node);
	double x,dxdy,dx[DIMENSION][3];
	double width;
	Point3D<Real> center;
	Real w;
	node->centerAndWidth(center,w);
	width=w;

	for(i=0;i<DIMENSION;i++){
		x=(center.coords[i]-position.coords[i]-width)/width;
		dx[i][0]=1.125+1.500*x+0.500*x*x;
		x=(center.coords[i]-position.coords[i])/width;
		dx[i][1]=0.750        -      x*x;
		dx[i][2]=1.0-dx[i][1]-dx[i][0];
	}

	for(i=0;i<3;i++){
		for(j=0;j<3;j++){
			dxdy=dx[0][i]*dx[1][j]*weight;
			for(k=0;k<3;k++){
				if(neighbors.neighbors[i][j][k]){neighbors.neighbors[i][j][k]->nodeData.centerWeightContribution+=Real(dxdy*dx[2][k]);}
			}
		}
	}
	return 0;
}

template<int Degree>
int Octree<Degree>::setTree(const int& maxDepth,
							const int& kernelDepth,const Real& samplesPerNode,const Real& scaleFactor,Point3D<Real>& center,Real& scale,
							const int& resetSamples,const int& useConfidence){

	Point3D<Real> min,max,position,normal,myCenter;
	Real myWidth;
	int i,cnt=0;
	TreeOctNode* temp;
	int splatDepth=0;

	double c[2*DIMENSION];
  int pid;
  int num_points  = vtk_psr_input->GetNumberOfPoints();


	TreeNodeData::UseIndex=1;
	neighborKey.set(maxDepth);
	splatDepth=kernelDepth;
	if(splatDepth<0){splatDepth=0;}

  for(pid=0;pid<num_points;pid++){
    // Read through once to get the center and scale    
    vtk_psr_input->GetPoint(pid,c);
    for(i=0;i<DIMENSION;i++){
      if(!cnt || c[i]<min.coords[i]){min.coords[i]=c[i];}
      if(!cnt || c[i]>max.coords[i]){max.coords[i]=c[i];}
    }
    cnt++;
  }
	for(i=0;i<DIMENSION;i++){
		if(!i || scale<max.coords[i]-min.coords[i]){scale=Real(max.coords[i]-min.coords[i]);}
		center.coords[i]=Real(max.coords[i]+min.coords[i])/2;
	}

  scale*=scaleFactor;
	for(i=0;i<DIMENSION;i++){center.coords[i]-=scale/2;}
	if(splatDepth>0){

		cnt=0;

    for(pid=0;pid<num_points;pid++){
      // Read through once to get the center and scale    
      vtk_psr_input->GetPoint(pid,c);
      c[3] = vtk_psr_input->GetPointData()->GetNormals()->GetComponent(pid,0);
      c[4] = vtk_psr_input->GetPointData()->GetNormals()->GetComponent(pid,1);
      c[5] = vtk_psr_input->GetPointData()->GetNormals()->GetComponent(pid,2);


			for(i=0;i<DIMENSION;i++){
				position.coords[i]=(c[i]-center.coords[i])/scale;
				normal.coords[i]=c[DIMENSION+i];

			}
			myCenter.coords[0]=myCenter.coords[1]=myCenter.coords[2]=Real(0.5);
			myWidth=Real(1.0);
			for(i=0;i<DIMENSION;i++){if(position.coords[i]<myCenter.coords[i]-myWidth/2 || position.coords[i]>myCenter.coords[i]+myWidth/2){break;}}
			if(i!=DIMENSION){continue;}
			temp=&tree;
			int d=0;
			Real weight=Real(1.0);
			if(useConfidence){weight=Real(Length(normal));}
			while(d<splatDepth){
				NonLinearUpdateWeightContribution(temp,position,weight);
				if(!temp->children){temp->initChildren();}
				int cIndex=TreeOctNode::CornerIndex(myCenter,position);
				temp=&temp->children[cIndex];
				myWidth/=2;
				if(cIndex&1){myCenter.coords[0]+=myWidth/2;}
				else		{myCenter.coords[0]-=myWidth/2;}
				if(cIndex&2){myCenter.coords[1]+=myWidth/2;}
				else		{myCenter.coords[1]-=myWidth/2;}
				if(cIndex&4){myCenter.coords[2]+=myWidth/2;}
				else		{myCenter.coords[2]-=myWidth/2;}
				d++;
			}
			NonLinearUpdateWeightContribution(temp,position,weight);
			cnt++;
		}
	}

	normals=new std::vector<Point3D<Real> >();
	cnt=0;

  for(pid=0;pid<num_points;pid++){
    // Read through once to get the center and scale    
    vtk_psr_input->GetPoint(pid,c);
    c[3] = vtk_psr_input->GetPointData()->GetNormals()->GetComponent(pid,0);
    c[4] = vtk_psr_input->GetPointData()->GetNormals()->GetComponent(pid,1);
    c[5] = vtk_psr_input->GetPointData()->GetNormals()->GetComponent(pid,2);


    for(i=0;i<DIMENSION;i++){
			position.coords[i]=(c[i]-center.coords[i])/scale;
			normal.coords[i]=c[DIMENSION+i];
		}
		myCenter.coords[0]=myCenter.coords[1]=myCenter.coords[2]=Real(0.5);
		myWidth=Real(1.0);
		for(i=0;i<DIMENSION;i++){if(position.coords[i]<myCenter.coords[i]-myWidth/2 || position.coords[i]>myCenter.coords[i]+myWidth/2){break;}}
		if(i!=DIMENSION){continue;}
		Real l=Real(Length(normal));
		if(l<EPSILON){continue;}
		if(!useConfidence){
			normal.coords[0]/=l;
			normal.coords[1]/=l;
			normal.coords[2]/=l;
		}
		l=Real(2<<maxDepth);
		normal.coords[0]*=l;
		normal.coords[1]*=l;
		normal.coords[2]*=l;

		if(resetSamples && samplesPerNode>0 && splatDepth){
			NonLinearSplatOrientedPoint(position,normal,splatDepth,samplesPerNode,1,maxDepth);
		}
		else{
			Real alpha=1;
			temp=&tree;
			if(splatDepth){
				int d=0;
				while(d<splatDepth){
					int cIndex=TreeOctNode::CornerIndex(myCenter,position);
					temp=&temp->children[cIndex];
					myWidth/=2;
					if(cIndex&1){myCenter.coords[0]+=myWidth/2;}
					else		{myCenter.coords[0]-=myWidth/2;}
					if(cIndex&2){myCenter.coords[1]+=myWidth/2;}
					else		{myCenter.coords[1]-=myWidth/2;}
					if(cIndex&4){myCenter.coords[2]+=myWidth/2;}
					else		{myCenter.coords[2]-=myWidth/2;}
					d++;
				}
				alpha=NonLinearGetSampleWeight(temp,position);
			}
			for(i=0;i<DIMENSION;i++){normal.coords[i]*=alpha;}
			int d=0;
			while(d<maxDepth){
				if(!temp->children){temp->initChildren();}
				int cIndex=TreeOctNode::CornerIndex(myCenter,position);
				temp=&temp->children[cIndex];
				myWidth/=2;
				if(cIndex&1){myCenter.coords[0]+=myWidth/2;}
				else		{myCenter.coords[0]-=myWidth/2;}
				if(cIndex&2){myCenter.coords[1]+=myWidth/2;}
				else		{myCenter.coords[1]-=myWidth/2;}
				if(cIndex&4){myCenter.coords[2]+=myWidth/2;}
				else		{myCenter.coords[2]-=myWidth/2;}
				d++;
			}
			NonLinearSplatOrientedPoint(temp,position,normal);
		}
	}
	return cnt;
}

template<int Degree>
void Octree<Degree>::setFunctionData(const PPolynomial<Degree>& ReconstructionFunction,	const int& maxDepth,const int& normalize,const Real& normalSmooth){

	radius=Real(fabs(ReconstructionFunction.polys[0].start));
	width=int(double(radius+0.5-EPSILON)*2);
	if(normalSmooth>0){postNormalSmooth=normalSmooth;}
	fData.Set(maxDepth,ReconstructionFunction,normalize,1);
}

template<int Degree>
void Octree<Degree>::finalize1(const int& refineNeighbors)
{
	TreeOctNode* temp;

	if(refineNeighbors>=0){
		RefineFunction rf;
		temp=tree.nextNode();
		while(temp){
			if(temp->nodeData.nodeIndex>=0 && Length((*normals)[temp->nodeData.nodeIndex])>EPSILON){
				rf.depth=temp->depth()-refineNeighbors;
				TreeOctNode::ProcessMaxDepthNodeAdjacentNodes(fData.depth,temp,2*width,&tree,1,temp->depth()-refineNeighbors,&rf);
			}
			temp=tree.nextNode(temp);
		}
	}
	else if(refineNeighbors==-1234){
		temp=tree.nextLeaf();
		while(temp){
			if(!temp->children && temp->depth()<fData.depth){temp->initChildren();}
			temp=tree.nextLeaf(temp);
		}
	}
}
template<int Degree>
void Octree<Degree>::finalize2(const int& refineNeighbors)
{
	TreeOctNode* temp;

	if(refineNeighbors>=0){
		RefineFunction rf;
		temp=tree.nextNode();
		while(temp){
			if(fabs(temp->nodeData.value)>EPSILON){
				rf.depth=temp->depth()-refineNeighbors;
				TreeOctNode::ProcessMaxDepthNodeAdjacentNodes(fData.depth,temp,2*width,&tree,1,temp->depth()-refineNeighbors,&rf);
			}
			temp=tree.nextNode(temp);
		}
	}
}
template <int Degree>
Real Octree<Degree>::GetDivergence(const int idx[DIMENSION],const Point3D<Real>& normal) const
{
	double dot=fData.dotTable[idx[0]]*fData.dotTable[idx[1]]*fData.dotTable[idx[2]];
	return Real(dot*(fData.dDotTable[idx[0]]*normal.coords[0]+fData.dDotTable[idx[1]]*normal.coords[1]+fData.dDotTable[idx[2]]*normal.coords[2]));
}
template<int Degree>
Real Octree<Degree>::GetLaplacian(const int idx[DIMENSION]) const
{
	return Real(fData.dotTable[idx[0]]*fData.dotTable[idx[1]]*fData.dotTable[idx[2]]*(fData.d2DotTable[idx[0]]+fData.d2DotTable[idx[1]]+fData.d2DotTable[idx[2]]));
}
template<int Degree>
Real Octree<Degree>::GetDotProduct(const int idx[DIMENSION]) const
{
	return Real(fData.dotTable[idx[0]]*fData.dotTable[idx[1]]*fData.dotTable[idx[2]]);
}

template<int Degree>
int Octree<Degree>::GetFixedDepthLaplacian(SparseSymmetricMatrix<float>& matrix,const int& depth,const SortedTreeNodes& sNodes)
{
	LaplacianMatrixFunction mf;
	mf.ot=this;
	mf.offset=sNodes.nodeCount[depth];
	matrix.Resize(sNodes.nodeCount[depth+1]-sNodes.nodeCount[depth]);
	mf.rowElements=(MatrixEntry<float>*)malloc(sizeof(MatrixEntry<float>)*matrix.rows);
	for(int i=sNodes.nodeCount[depth];i<sNodes.nodeCount[depth+1];i++){
		mf.elementCount=0;
		mf.d2=int(sNodes.treeNodes[i]->d);
		mf.x2=int(sNodes.treeNodes[i]->off[0]);
		mf.y2=int(sNodes.treeNodes[i]->off[1]);
		mf.z2=int(sNodes.treeNodes[i]->off[2]);
		mf.index[0]=mf.x2;
		mf.index[1]=mf.y2;
		mf.index[2]=mf.z2;
		TreeOctNode::ProcessTerminatingNodeAdjacentNodes(fData.depth,sNodes.treeNodes[i],2*width-1,&tree,1,&mf);
		matrix.SetRowSize(i-sNodes.nodeCount[depth],mf.elementCount);
		memcpy(matrix.m_ppElements[i-sNodes.nodeCount[depth]],mf.rowElements,sizeof(MatrixEntry<float>)*mf.elementCount);
	}
	free(mf.rowElements);
	return 1;
}
template<int Degree>
int Octree<Degree>::GetRestrictedFixedDepthLaplacian(SparseSymmetricMatrix<float>& matrix,const int& depth,const int* entries,const int& entryCount,
													 const TreeOctNode* rNode,const Real& radius,
													 const SortedTreeNodes& sNodes){
	int i;
	RestrictedLaplacianMatrixFunction mf;
	Real myRadius=int(2*radius-ROUND_EPS)+ROUND_EPS;
	mf.ot=this;
	mf.radius=radius;
	rNode->depthAndOffset(mf.depth,mf.offset);
	matrix.Resize(entryCount);
	mf.rowElements=(MatrixEntry<float>*)malloc(sizeof(MatrixEntry<float>)*matrix.rows);
	for(i=0;i<entryCount;i++){sNodes.treeNodes[entries[i]]->nodeData.nodeIndex=i;}
	for(i=0;i<entryCount;i++){
		mf.elementCount=0;
		mf.index[0]=int(sNodes.treeNodes[entries[i]]->off[0]);
		mf.index[1]=int(sNodes.treeNodes[entries[i]]->off[1]);
		mf.index[2]=int(sNodes.treeNodes[entries[i]]->off[2]);
		TreeOctNode::ProcessTerminatingNodeAdjacentNodes(fData.depth,sNodes.treeNodes[entries[i]],2*width-1,&tree,1,&mf);
		matrix.SetRowSize(i,mf.elementCount);
		memcpy(matrix.m_ppElements[i],mf.rowElements,sizeof(MatrixEntry<float>)*mf.elementCount);
	}
	for(i=0;i<entryCount;i++){sNodes.treeNodes[entries[i]]->nodeData.nodeIndex=entries[i];}
	free(mf.rowElements);
	return 1;
}


template<int Degree>
int Octree<Degree>::LaplacianMatrixIteration(const int& subdivideDepth){
	int i,iter=0;
	SortedTreeNodes sNodes;

	fData.SetDotTables(fData.D2_DOT_FLAG);
	sNodes.Set(tree,1);

	SparseMatrix<float>::SetAllocator(MEMORY_ALLOCATOR_BLOCK_SIZE);

	sNodes.treeNodes[0]->nodeData.value=0;
	for(i=1;i<sNodes.maxDepth;i++){
		if(subdivideDepth>0){iter+=SolveFixedDepthMatrix(i,subdivideDepth,sNodes);}
		else{iter+=SolveFixedDepthMatrix(i,sNodes);}
	}
	SparseMatrix<float>::Allocator.Reset();
	fData.ClearDotTables(fData.DOT_FLAG | fData.D_DOT_FLAG | fData.D2_DOT_FLAG);
	return iter;
}

template<int Degree>
int Octree<Degree>::SolveFixedDepthMatrix(const int& depth,const SortedTreeNodes& sNodes){
	int i,iter=0;
	Vector<double> V,Solution;
	SparseSymmetricMatrix<Real> matrix;
	Real myRadius;
	Real dx,dy,dz;
	int x1,x2,y1,y2,z1,z2;
	Vector<Real> Diagonal;

	V.Resize(sNodes.nodeCount[depth+1]-sNodes.nodeCount[depth]);
	for(i=sNodes.nodeCount[depth];i<sNodes.nodeCount[depth+1];i++){V[i-sNodes.nodeCount[depth]]=sNodes.treeNodes[i]->nodeData.value;}
	SparseSymmetricMatrix<float>::Allocator.RollBack();
	GetFixedDepthLaplacian(matrix,depth,sNodes);

  iter+=SparseSymmetricMatrix<Real>::Solve(matrix,V,int(pow((double)matrix.rows,ITERATION_POWER)),Solution,double(EPSILON),1);

  for(i=sNodes.nodeCount[depth];i<sNodes.nodeCount[depth+1];i++){sNodes.treeNodes[i]->nodeData.value=Real(Solution[i-sNodes.nodeCount[depth]]);}

	myRadius=Real(radius+ROUND_EPS-0.5);
	myRadius /=(1<<depth);

	if(depth<sNodes.maxDepth-1){
		LaplacianProjectionFunction pf;
		TreeOctNode *node1,*node2;
		pf.ot=this;
		int idx1,idx2,off=sNodes.nodeCount[depth];
		// First pass: idx2 is the solution coefficient propogated
		for(i=0;i<matrix.rows;i++){
			idx1=i;
			node1=sNodes.treeNodes[idx1+off];
			if(!node1->children){continue;}
			x1=int(node1->off[0]);
			y1=int(node1->off[1]);
			z1=int(node1->off[2]);
			for(int j=0;j<matrix.rowSizes[i];j++){
				idx2=matrix.m_ppElements[i][j].N;
				node2=sNodes.treeNodes[idx2+off];
				x2=int(node2->off[0]);
				y2=int(node2->off[1]);
				z2=int(node2->off[2]);
				pf.value=Solution[idx2];
				pf.index[0]=x2;
				pf.index[1]=y2;
				pf.index[2]=z2;
				dx=Real(x2-x1)/(1<<depth);
				dy=Real(y2-y1)/(1<<depth);
				dz=Real(z2-z1)/(1<<depth);
				if(fabs(dx)<myRadius && fabs(dy)<myRadius && fabs(dz)<myRadius){node1->processNodeNodes(node2,&pf,0);}
				else{TreeOctNode::ProcessNodeAdjacentNodes(fData.depth,node2,width,node1,width,&pf,0);}
			}
		}
		// Second pass: idx1 is the solution coefficient propogated
		for(i=0;i<matrix.rows;i++){
			idx1=i;
			node1=sNodes.treeNodes[idx1+off];
			x1=int(node1->off[0]);
			y1=int(node1->off[1]);
			z1=int(node1->off[2]);
			pf.value=Solution[idx1];
			pf.index[0]=x1;
			pf.index[1]=y1;
			pf.index[2]=z1;
			for(int j=0;j<matrix.rowSizes[i];j++){
				idx2=matrix.m_ppElements[i][j].N;
				node2=sNodes.treeNodes[idx2+off];
				if(idx1!=idx2 && node2->children){
					x2=int(node2->off[0]);
					y2=int(node2->off[1]);
					z2=int(node2->off[2]);
					dx=Real(x1-x2)/(1<<depth);
					dy=Real(y1-y2)/(1<<depth);
					dz=Real(z1-z2)/(1<<depth);
					if(fabs(dx)<myRadius && fabs(dy)<myRadius && fabs(dz)<myRadius){node2->processNodeNodes(node1,&pf,0);}
					else{TreeOctNode::ProcessNodeAdjacentNodes(fData.depth,node1,width,node2,width,&pf,0);}
				}
			}
		}
	}
	return iter;
}
template<int Degree>
int Octree<Degree>::SolveFixedDepthMatrix(const int& depth,const int& startingDepth,const SortedTreeNodes& sNodes){
	int i,j,d,iter=0;
	SparseSymmetricMatrix<Real> matrix;
	AdjacencySetFunction asf;
	AdjacencyCountFunction acf;
	Vector<Real> Values;
	Vector<double> SubValues,SubSolution;
	Real myRadius,myRadius2;
	Real dx,dy,dz;
	Vector<Real> Diagonal;

	if(startingDepth>=depth){return SolveFixedDepthMatrix(depth,sNodes);}

	Values.Resize(sNodes.nodeCount[depth+1]-sNodes.nodeCount[depth]);

	for(i=sNodes.nodeCount[depth];i<sNodes.nodeCount[depth+1];i++){
		Values[i-sNodes.nodeCount[depth]]=sNodes.treeNodes[i]->nodeData.value;
		sNodes.treeNodes[i]->nodeData.value=0;
	}

	myRadius=2*radius-Real(0.5);
	myRadius=int(myRadius-ROUND_EPS)+ROUND_EPS;
	myRadius2=Real(radius+ROUND_EPS-0.5);
	d=depth-startingDepth;
	for(i=sNodes.nodeCount[d];i<sNodes.nodeCount[d+1];i++){

		TreeOctNode* temp;
		// Get all of the entries associated to the subspace
		acf.adjacencyCount=0;
		temp=sNodes.treeNodes[i]->nextNode();
		while(temp){
			if(temp->depth()==depth){
				acf.Function(temp,temp);
				temp=sNodes.treeNodes[i]->nextBranch(temp);
			}
			else{temp=sNodes.treeNodes[i]->nextNode(temp);}
		}
		for(j=sNodes.nodeCount[d];j<sNodes.nodeCount[d+1];j++){
			if(i==j){continue;}
			TreeOctNode::ProcessFixedDepthNodeAdjacentNodes(fData.depth,sNodes.treeNodes[i],1,sNodes.treeNodes[j],2*width-1,depth,&acf);
		}
		if(!acf.adjacencyCount){continue;}
		asf.adjacencies=new int[acf.adjacencyCount];
		asf.adjacencyCount=0;
		temp=sNodes.treeNodes[i]->nextNode();
		while(temp){
			if(temp->depth()==depth){
				asf.Function(temp,temp);
				temp=sNodes.treeNodes[i]->nextBranch(temp);
			}
			else{temp=sNodes.treeNodes[i]->nextNode(temp);}
		}
		for(j=sNodes.nodeCount[d];j<sNodes.nodeCount[d+1];j++){
			if(i==j){continue;}
			TreeOctNode::ProcessFixedDepthNodeAdjacentNodes(fData.depth,sNodes.treeNodes[i],1,sNodes.treeNodes[j],2*width-1,depth,&asf);
		}

		// Get the associated vector
		SubValues.Resize(asf.adjacencyCount);
		for(j=0;j<asf.adjacencyCount;j++){SubValues[j]=Values[asf.adjacencies[j]-sNodes.nodeCount[depth]];}
		SubSolution.Resize(asf.adjacencyCount);
		for(j=0;j<asf.adjacencyCount;j++){SubSolution[j]=sNodes.treeNodes[asf.adjacencies[j]]->nodeData.value;}
		// Get the associated matrix
		SparseSymmetricMatrix<float>::Allocator.RollBack();
		GetRestrictedFixedDepthLaplacian(matrix,depth,asf.adjacencies,asf.adjacencyCount,sNodes.treeNodes[i],myRadius,sNodes);

		// Solve the matrix
		iter+=SparseSymmetricMatrix<Real>::Solve(matrix,SubValues,int(pow((double)matrix.rows,ITERATION_POWER)),SubSolution,double(EPSILON),0);

    LaplacianProjectionFunction lpf;
		lpf.ot=this;

		// Update the solution for all nodes in the sub-tree
		for(j=0;j<asf.adjacencyCount;j++){
			temp=sNodes.treeNodes[asf.adjacencies[j]];
			while(temp->depth()>sNodes.treeNodes[i]->depth()){temp=temp->parent;}
			if(temp->nodeData.nodeIndex>=sNodes.treeNodes[i]->nodeData.nodeIndex){sNodes.treeNodes[asf.adjacencies[j]]->nodeData.value=Real(SubSolution[j]);}
		}

		// Update the values in the next depth
		int x1,x2,y1,y2,z1,z2;
		if(depth<sNodes.maxDepth-1){
			int idx1,idx2;
			TreeOctNode *node1,*node2;
			// First pass: idx2 is the solution coefficient propogated
			for(j=0;j<matrix.rows;j++){
				idx1=asf.adjacencies[j];
				node1=sNodes.treeNodes[idx1];
				if(!node1->children){continue;}
				x1=int(node1->off[0]);
				y1=int(node1->off[1]);
				z1=int(node1->off[2]);

				for(int k=0;k<matrix.rowSizes[j];k++){
					idx2=asf.adjacencies[matrix.m_ppElements[j][k].N];
					node2=sNodes.treeNodes[idx2];
					temp=node2;
					while(temp->depth()>d){temp=temp->parent;}
					if(temp!=sNodes.treeNodes[i]){continue;}
					lpf.value=Real(SubSolution[matrix.m_ppElements[j][k].N]);
					x2=int(node2->off[0]);
					y2=int(node2->off[1]);
					z2=int(node2->off[2]);
					lpf.index[0]=x2;
					lpf.index[1]=y2;
					lpf.index[2]=z2;
					dx=Real(x2-x1)/(1<<depth);
					dy=Real(y2-y1)/(1<<depth);
					dz=Real(z2-z1)/(1<<depth);
					if(fabs(dx)<myRadius2 && fabs(dy)<myRadius2 && fabs(dz)<myRadius2){node1->processNodeNodes(node2,&lpf,0);}
					else{TreeOctNode::ProcessNodeAdjacentNodes(fData.depth,node2,width,node1,width,&lpf,0);}
				}
			}
			// Second pass: idx1 is the solution coefficient propogated
			for(j=0;j<matrix.rows;j++){
				idx1=asf.adjacencies[j];
				node1=sNodes.treeNodes[idx1];
				temp=node1;
				while(temp->depth()>d){temp=temp->parent;}
				if(temp!=sNodes.treeNodes[i]){continue;}
				x1=int(node1->off[0]);
				y1=int(node1->off[1]);
				z1=int(node1->off[2]);

				lpf.value=Real(SubSolution[j]);
				lpf.index[0]=x1;
				lpf.index[1]=y1;
				lpf.index[2]=z1;
				for(int k=0;k<matrix.rowSizes[j];k++){
					idx2=asf.adjacencies[matrix.m_ppElements[j][k].N];
					node2=sNodes.treeNodes[idx2];
					if(!node2->children){continue;}

					if(idx1!=idx2){
						x2=int(node2->off[0]);
						y2=int(node2->off[1]);
						z2=int(node2->off[2]);
						dx=Real(x1-x2)/(1<<depth);
						dy=Real(y1-y2)/(1<<depth);
						dz=Real(z1-z2)/(1<<depth);
						if(fabs(dx)<myRadius2 && fabs(dy)<myRadius2 && fabs(dz)<myRadius2){node2->processNodeNodes(node1,&lpf,0);}
						else{TreeOctNode::ProcessNodeAdjacentNodes(fData.depth,node1,width,node2,width,&lpf,0);}
					}
				}
			}
		}

		delete[] asf.adjacencies;
	}
	return iter;
}
template<int Degree>
int Octree<Degree>::HasNormals(TreeOctNode* node,const Real& epsilon){
	int hasNormals=0;
	if(node->nodeData.nodeIndex>=0 && Length((*normals)[node->nodeData.nodeIndex])>epsilon){hasNormals=1;}
	if(node->children){for(int i=0;i<Cube::CORNERS && !hasNormals;i++){hasNormals|=HasNormals(&node->children[i],epsilon);}}

	return hasNormals;
}
template<int Degree>
void Octree<Degree>::ClipTree(void){
	TreeOctNode* temp;
	temp=tree.nextNode();
	while(temp){
		if(temp->children){
			int hasNormals=0;
			for(int i=0;i<Cube::CORNERS && !hasNormals;i++){hasNormals=HasNormals(&temp->children[i],EPSILON);}
			if(!hasNormals){temp->children=NULL;}
		}
		temp=tree.nextNode(temp);
	}
}
template<int Degree>
void Octree<Degree>::SetLaplacianWeights(void){
	TreeOctNode* temp;

	fData.SetDotTables(fData.DOT_FLAG | fData.D_DOT_FLAG);
	DivergenceFunction df;
	df.ot=this;
	temp=tree.nextNode();
	while(temp){
		if(temp->nodeData.nodeIndex<0 || Length((*normals)[temp->nodeData.nodeIndex])<=EPSILON){
			temp=tree.nextNode(temp);
			continue;
		}
		int d=temp->depth();
		df.normal=(*normals)[temp->nodeData.nodeIndex];
		df.index[0]=int(temp->off[0]);
		df.index[1]=int(temp->off[1]);
		df.index[2]=int(temp->off[2]);
		TreeOctNode::ProcessNodeAdjacentNodes(fData.depth,temp,width,&tree,width,&df);
		temp=tree.nextNode(temp);
	}
	fData.ClearDotTables(fData.D_DOT_FLAG);
	temp=tree.nextNode();
	while(temp){
		if(temp->nodeData.nodeIndex<0){temp->nodeData.centerWeightContribution=0;}
		else{temp->nodeData.centerWeightContribution=Real(Length((*normals)[temp->nodeData.nodeIndex]));}
		temp=tree.nextNode(temp);
	}
	MemoryUsage();

	delete normals;
	normals=NULL;
}
template<int Degree>
void Octree<Degree>::DivergenceFunction::Function(TreeOctNode* node1,const TreeOctNode* node2){
	Point3D<Real> n=normal;
	if(FunctionData<Degree,Real>::SymmetricIndex(index[0],int(node1->off[0]),scratch[0])){n.coords[0]=-n.coords[0];}
	if(FunctionData<Degree,Real>::SymmetricIndex(index[1],int(node1->off[1]),scratch[1])){n.coords[1]=-n.coords[1];}
	if(FunctionData<Degree,Real>::SymmetricIndex(index[2],int(node1->off[2]),scratch[2])){n.coords[2]=-n.coords[2];}
	double dot=ot->fData.dotTable[scratch[0]]*ot->fData.dotTable[scratch[1]]*ot->fData.dotTable[scratch[2]];
	node1->nodeData.value+=Real(dot*(ot->fData.dDotTable[scratch[0]]*n.coords[0]+ot->fData.dDotTable[scratch[1]]*n.coords[1]+ot->fData.dDotTable[scratch[2]]*n.coords[2]));
}
template<int Degree>
void Octree<Degree>::LaplacianProjectionFunction::Function(TreeOctNode* node1,const TreeOctNode* node2){
	scratch[0]=FunctionData<Degree,Real>::SymmetricIndex(index[0],int(node1->off[0]));
	scratch[1]=FunctionData<Degree,Real>::SymmetricIndex(index[1],int(node1->off[1]));
	scratch[2]=FunctionData<Degree,Real>::SymmetricIndex(index[2],int(node1->off[2]));
	node1->nodeData.value-=Real(ot->GetLaplacian(scratch)*value);
}
template<int Degree>
void Octree<Degree>::AdjacencyCountFunction::Function(const TreeOctNode* node1,const TreeOctNode* node2){adjacencyCount++;}
template<int Degree>
void Octree<Degree>::AdjacencySetFunction::Function(const TreeOctNode* node1,const TreeOctNode* node2){adjacencies[adjacencyCount++]=node1->nodeData.nodeIndex;}
template<int Degree>
void Octree<Degree>::RefineFunction::Function(TreeOctNode* node1,const TreeOctNode* node2){
	if(!node1->children && node1->depth()<depth){node1->initChildren();}
}
template<int Degree>
void Octree<Degree>::FaceEdgesFunction::Function(const TreeOctNode* node1,const TreeOctNode* node2){
	if(!node1->children && MarchingCubes::HasRoots(node1->nodeData.mcIndex)){
		RootInfo ri1,ri2;
		unordered_map<long long,std::pair<RootInfo,int> >::iterator iter;
		int isoTri[DIMENSION*MarchingCubes::MAX_TRIANGLES];
		int count=MarchingCubes::AddTriangleIndices(node1->nodeData.mcIndex,isoTri);

		for(int j=0;j<count;j++){
			for(int k=0;k<3;k++){
				if(fIndex==Cube::FaceAdjacentToEdges(isoTri[j*3+k],isoTri[j*3+((k+1)%3)])){
					if(GetRootIndex(node1,isoTri[j*3+k],maxDepth,ri1) && GetRootIndex(node1,isoTri[j*3+((k+1)%3)],maxDepth,ri2)){
						edges->push_back(std::pair<long long,long long>(ri2.Key,ri1.Key));
						iter=vertexCount->find(ri1.Key);
						if(iter==vertexCount->end()){
							(*vertexCount)[ri1.Key].first=ri1;
							(*vertexCount)[ri1.Key].second=0;
						}
						iter=vertexCount->find(ri2.Key);
						if(iter==vertexCount->end()){
							(*vertexCount)[ri2.Key].first=ri2;
							(*vertexCount)[ri2.Key].second=0;
						}
						(*vertexCount)[ri1.Key].second--;
						(*vertexCount)[ri2.Key].second++;
					}
					else{fprintf(stderr,"Bad Edge 1: %d %d\n",ri1.Key,ri2.Key);}
				}
			}
		}
	}
}
template<int Degree>
void Octree<Degree>::PointIndexValueFunction::Function(const TreeOctNode* node){
	int idx[DIMENSION];
	idx[0]=index[0]+int(node->off[0]);
	idx[1]=index[1]+int(node->off[1]);
	idx[2]=index[2]+int(node->off[2]);
	value+=node->nodeData.value*   Real( valueTables[idx[0]]* valueTables[idx[1]]* valueTables[idx[2]]);
}
template<int Degree>
void Octree<Degree>::PointIndexValueAndNormalFunction::Function(const TreeOctNode* node){
	int idx[DIMENSION];
	idx[0]=index[0]+int(node->off[0]);
	idx[1]=index[1]+int(node->off[1]);
	idx[2]=index[2]+int(node->off[2]);
	value+=				node->nodeData.value*   Real( valueTables[idx[0]]* valueTables[idx[1]]* valueTables[idx[2]]);
	normal.coords[0]+=	node->nodeData.value*   Real(dValueTables[idx[0]]* valueTables[idx[1]]* valueTables[idx[2]]);
	normal.coords[1]+=	node->nodeData.value*   Real( valueTables[idx[0]]*dValueTables[idx[1]]* valueTables[idx[2]]);
	normal.coords[2]+=	node->nodeData.value*   Real( valueTables[idx[0]]* valueTables[idx[1]]*dValueTables[idx[2]]);
}
template<int Degree>
int Octree<Degree>::LaplacianMatrixFunction::Function(const TreeOctNode* node1,const TreeOctNode* node2){
	Real temp;
	int d1=int(node1->d);
	int x1,y1,z1;
	x1=int(node1->off[0]);
	y1=int(node1->off[1]);
	z1=int(node1->off[2]);
	int dDepth=d2-d1;
	int d;
	d=(x2>>dDepth)-x1;
	if(d<0){return 0;}
	if(!dDepth){
		if(!d){
			d=y2-y1;
			if(d<0){return 0;}
			else if(!d){
				d=z2-z1;
				if(d<0){return 0;}
			}
		}
		scratch[0]=FunctionData<Degree,Real>::SymmetricIndex(index[0],x1);
		scratch[1]=FunctionData<Degree,Real>::SymmetricIndex(index[1],y1);
		scratch[2]=FunctionData<Degree,Real>::SymmetricIndex(index[2],z1);
		temp=ot->GetLaplacian(scratch);
		if(node1==node2){temp/=2;}
		if(fabs(temp)>EPSILON){
			rowElements[elementCount].Value=temp;
			rowElements[elementCount].N=node1->nodeData.nodeIndex-offset;
			elementCount++;
		}
		return 0;
	}
	return 1;
}

template<int Degree>
int Octree<Degree>::RestrictedLaplacianMatrixFunction::Function(const TreeOctNode* node1,const TreeOctNode* node2){
	int d1,d2,off1[3],off2[3];
	node1->depthAndOffset(d1,off1);
	node2->depthAndOffset(d2,off2);
	int dDepth=d2-d1;
	int d;
	d=(off2[0]>>dDepth)-off1[0];
	if(d<0){return 0;}

	if(!dDepth){
		if(!d){
			d=off2[1]-off1[1];
			if(d<0){return 0;}
			else if(!d){
				d=off2[2]-off1[2];
				if(d<0){return 0;}
			}
		}
		// Since we are getting the restricted matrix, we don't want to propogate out to terms that don't contribute...
		if(!TreeOctNode::Overlap2(depth,offset,0.5,d1,off1,radius)){return 0;}
		scratch[0]=FunctionData<Degree,Real>::SymmetricIndex(index[0],BinaryNode<Real>::Index(d1,off1[0]));
		scratch[1]=FunctionData<Degree,Real>::SymmetricIndex(index[1],BinaryNode<Real>::Index(d1,off1[1]));
		scratch[2]=FunctionData<Degree,Real>::SymmetricIndex(index[2],BinaryNode<Real>::Index(d1,off1[2]));
		Real temp=ot->GetLaplacian(scratch);
		if(node1==node2){temp/=2;}
		if(fabs(temp)>EPSILON){
			rowElements[elementCount].Value=temp;
			rowElements[elementCount].N=node1->nodeData.nodeIndex;
			elementCount++;
		}
		return 0;
	}
	return 1;
}

template<int Degree>
void Octree<Degree>::GetMCIsoTriangles(const Real& isoValue,CoredMeshData* mesh,const int& fullDepthIso,const int& nonLinearFit){
	double t;
	TreeOctNode* temp;

	unordered_map<long long,int> roots;
	unordered_map<long long,std::pair<Real,Point3D<Real> > > *normalHash=new unordered_map<long long,std::pair<Real,Point3D<Real> > >();

	SetIsoSurfaceCorners(isoValue,0,fullDepthIso);
	// At the point all of the corner values have been set and all nodes are valid. Now it's just a matter
	// of running marching cubes.

	fData.setValueTables(fData.VALUE_FLAG | fData.D_VALUE_FLAG,0,postNormalSmooth);
	temp=tree.nextLeaf();
	while(temp){
		SetMCRootPositions(temp,0,isoValue,roots,NULL,*normalHash,NULL,NULL,mesh,nonLinearFit);
		temp=tree.nextLeaf(temp);
	}
	MemoryUsage();


	fData.clearValueTables();
	delete normalHash;



	// Now get the iso-surfaces, running from finest nodes to coarsest in order to allow for edge propogation from
	// finer faces to coarser ones.
	temp=tree.nextLeaf();
	while(temp){
		GetMCIsoTriangles(temp,mesh,roots,NULL,NULL,0,0);
		temp=tree.nextLeaf(temp);
	}
}
template<int Degree>
void Octree<Degree>::GetMCIsoTriangles(const Real& isoValue,const int& subdivideDepth,CoredMeshData* mesh,const int& fullDepthIso,const int& nonLinearFit){
	TreeOctNode* temp;
	unordered_map<long long,int> boundaryRoots,*interiorRoots;
	unordered_map<long long,std::pair<Real,Point3D<Real> > > *boundaryNormalHash,*interiorNormalHash;
	std::vector<Point3D<float> >* interiorPoints;

	int sDepth;
	if(subdivideDepth<=0){sDepth=0;}
	else{sDepth=fData.depth-subdivideDepth;}
	if(sDepth<0){sDepth=0;}

	SetIsoSurfaceCorners(isoValue,sDepth,fullDepthIso);
	// At this point all of the corner values have been set and all nodes are valid. Now it's just a matter
	// of running marching cubes.

	boundaryNormalHash=new unordered_map<long long,std::pair<Real,Point3D<Real> > >();
	int offSet=0;
	SortedTreeNodes sNodes;
	sNodes.Set(tree,0);
	fData.SetValueTables(fData.VALUE_FLAG | fData.D_VALUE_FLAG,0,postNormalSmooth);

	// Set the root positions for all leaf nodes below the subdivide threshold
	SetBoundaryMCRootPositions(sDepth,isoValue,boundaryRoots,*boundaryNormalHash,mesh,nonLinearFit);

	for(int i=sNodes.nodeCount[sDepth];i<sNodes.nodeCount[sDepth+1];i++){
		interiorRoots=new unordered_map<long long,int>();
		interiorNormalHash=new unordered_map<long long,std::pair<Real,Point3D<Real> > >();
		interiorPoints=new std::vector<Point3D<float> >();

		temp=sNodes.treeNodes[i]->nextLeaf();
		while(temp){
			if(MarchingCubes::HasRoots(temp->nodeData.mcIndex)){
				SetMCRootPositions(temp,sDepth,isoValue,boundaryRoots,interiorRoots,*boundaryNormalHash,interiorNormalHash,interiorPoints,mesh,nonLinearFit);
			}
			temp=sNodes.treeNodes[i]->nextLeaf(temp);
		}
		delete interiorNormalHash;

		temp=sNodes.treeNodes[i]->nextLeaf();
		while(temp){
			GetMCIsoTriangles(temp,mesh,boundaryRoots,interiorRoots,interiorPoints,offSet,sDepth);
			temp=sNodes.treeNodes[i]->nextLeaf(temp);
		}
		delete interiorRoots;
		delete interiorPoints;
		offSet=mesh->OutOfCorePointCount();
	}
	delete boundaryNormalHash;

	temp=tree.nextLeaf();
	while(temp){
		if(temp->depth()<sDepth){GetMCIsoTriangles(temp,mesh,boundaryRoots,NULL,NULL,0,0);}
		temp=tree.nextLeaf(temp);
	}
}
template<int Degree>
Real Octree<Degree>::getCenterValue(const TreeOctNode* node){
	int idx[3];
	Real value=0;

	neighborKey2.getNeighbors(node);
	VertexData::CenterIndex(node,fData.depth,idx);
	idx[0]*=fData.res;
	idx[1]*=fData.res;
	idx[2]*=fData.res;
	for(int i=0;i<=node->depth();i++){
		for(int j=0;j<3;j++){
			for(int k=0;k<3;k++){
				for(int l=0;l<3;l++){
					const TreeOctNode* n=neighborKey2.neighbors[i].neighbors[j][k][l];
					if(n){
						Real temp=n->nodeData.value;
						value+=temp*Real(
							fData.valueTables[idx[0]+int(n->off[0])]*
							fData.valueTables[idx[1]+int(n->off[1])]*
							fData.valueTables[idx[2]+int(n->off[2])]);
					}
				}
			}
		}
	}
	if(node->children){
		for(int i=0;i<Cube::CORNERS;i++){
			int ii=Cube::AntipodalCornerIndex(i);
			const TreeOctNode* n=&node->children[i];
			while(1){
				value+=n->nodeData.value*Real(
					fData.valueTables[idx[0]+int(n->off[0])]*
					fData.valueTables[idx[1]+int(n->off[1])]*
					fData.valueTables[idx[2]+int(n->off[2])]);
				if(n->children){n=&n->children[ii];}
				else{break;}
			}
		}
	}
	return value;
}
template<int Degree>
Real Octree<Degree>::getCornerValue(const TreeOctNode* node,const int& corner){
	int idx[3];
	Real value=0;

	neighborKey2.getNeighbors(node);
	VertexData::CornerIndex(node,corner,fData.depth,idx);
	idx[0]*=fData.res;
	idx[1]*=fData.res;
	idx[2]*=fData.res;
	for(int i=0;i<=node->depth();i++){
		for(int j=0;j<3;j++){
			for(int k=0;k<3;k++){
				for(int l=0;l<3;l++){
					const TreeOctNode* n=neighborKey2.neighbors[i].neighbors[j][k][l];
					if(n){
						Real temp=n->nodeData.value;
						value+=temp*Real(
							fData.valueTables[idx[0]+int(n->off[0])]*
							fData.valueTables[idx[1]+int(n->off[1])]*
							fData.valueTables[idx[2]+int(n->off[2])]);
					}
				}
			}
		}
	}
	int x,y,z,d=node->depth();
	Cube::FactorCornerIndex(corner,x,y,z);
	for(int i=0;i<2;i++){
		for(int j=0;j<2;j++){
			for(int k=0;k<2;k++){
				const TreeOctNode* n=neighborKey2.neighbors[d].neighbors[x+i][y+j][z+k];
				if(n){
					int ii=Cube::AntipodalCornerIndex(Cube::CornerIndex(i,j,k));
					while(n->children){
						n=&n->children[ii];
						value+=n->nodeData.value*Real(
							fData.valueTables[idx[0]+int(n->off[0])]*
							fData.valueTables[idx[1]+int(n->off[1])]*
							fData.valueTables[idx[2]+int(n->off[2])]);
					}
				}
			}
		}
	}
	return value;
}
template<int Degree>
void Octree<Degree>::getCornerValueAndNormal(const TreeOctNode* node,const int& corner,Real& value,Point3D<Real>& normal){
	int idx[3],index[3];
	value=normal.coords[0]=normal.coords[1]=normal.coords[2]=0;

	neighborKey2.getNeighbors(node);
	VertexData::CornerIndex(node,corner,fData.depth,idx);
	idx[0]*=fData.res;
	idx[1]*=fData.res;
	idx[2]*=fData.res;
	for(int i=0;i<=node->depth();i++){
		for(int j=0;j<3;j++){
			for(int k=0;k<3;k++){
				for(int l=0;l<3;l++){
					const TreeOctNode* n=neighborKey2.neighbors[i].neighbors[j][k][l];
					if(n){
						Real temp=n->nodeData.value;
						index[0]=idx[0]+int(n->off[0]);
						index[1]=idx[1]+int(n->off[1]);
						index[2]=idx[2]+int(n->off[2]);
						value+=temp*Real(fData.valueTables[index[0]]*fData.valueTables[index[1]]*fData.valueTables[index[2]]);
						normal.coords[0]+=temp*Real(fData.dValueTables[index[0]]* fData.valueTables[index[1]]* fData.valueTables[index[2]]);
						normal.coords[1]+=temp*Real( fData.valueTables[index[0]]*fData.dValueTables[index[1]]* fData.valueTables[index[2]]);
						normal.coords[2]+=temp*Real( fData.valueTables[index[0]]* fData.valueTables[index[1]]*fData.dValueTables[index[2]]);
					}
				}
			}
		}
	}
	int x,y,z,d=node->depth();
	Cube::FactorCornerIndex(corner,x,y,z);
	for(int i=0;i<2;i++){
		for(int j=0;j<2;j++){
			for(int k=0;k<2;k++){
				const TreeOctNode* n=neighborKey2.neighbors[d].neighbors[x+i][y+j][z+k];
				if(n){
					int ii=Cube::AntipodalCornerIndex(Cube::CornerIndex(i,j,k));
					while(n->children){
						n=&n->children[ii];
						Real temp=n->nodeData.value;
						index[0]=idx[0]+int(n->off[0]);
						index[1]=idx[1]+int(n->off[1]);
						index[2]=idx[2]+int(n->off[2]);
						value+=temp*Real(fData.valueTables[index[0]]*fData.valueTables[index[1]]*fData.valueTables[index[2]]);
						normal.coords[0]+=temp*Real(fData.dValueTables[index[0]]* fData.valueTables[index[1]]* fData.valueTables[index[2]]);
						normal.coords[1]+=temp*Real( fData.valueTables[index[0]]*fData.dValueTables[index[1]]* fData.valueTables[index[2]]);
						normal.coords[2]+=temp*Real( fData.valueTables[index[0]]* fData.valueTables[index[1]]*fData.dValueTables[index[2]]);
					}
				}
			}
		}
	}
}
template<int Degree>
Real Octree<Degree>::GetIsoValue(void){
	if(this->width<=3){
		TreeOctNode* temp;
		Real isoValue,weightSum,w;

		neighborKey2.set(fData.depth);
		fData.SetValueTables(fData.VALUE_FLAG,0);

		isoValue=weightSum=0;
		temp=tree.nextNode();
		while(temp){
			w=temp->nodeData.centerWeightContribution;
			if(w>EPSILON){
				isoValue+=getCenterValue(temp)*w;
				weightSum+=w;
			}
			temp=tree.nextNode(temp);
		}
		return isoValue/weightSum;
	}
	else{
		const TreeOctNode* temp;
		Real isoValue,weightSum,w;
		Real myRadius;
		PointIndexValueFunction cf;

		fData.SetValueTables(fData.VALUE_FLAG,0);
		cf.valueTables=fData.valueTables;
		cf.res2=fData.res2;
		myRadius=radius;
		isoValue=weightSum=0;
		temp=tree.nextNode();
		while(temp){
			w=temp->nodeData.centerWeightContribution;
			if(w>EPSILON){
				cf.value=0;
				int idx[3];
				VertexData::CenterIndex(temp,fData.depth,idx);
				cf.index[0]=idx[0]*fData.res;
				cf.index[1]=idx[1]*fData.res;
				cf.index[2]=idx[2]*fData.res;
				TreeOctNode::ProcessPointAdjacentNodes(fData.depth,idx,&tree,width,&cf);
				isoValue+=cf.value*w;
				weightSum+=w;
			}
			temp=tree.nextNode(temp);
		}
		return isoValue/weightSum;
	}
}
template<int Degree>
void Octree<Degree>::SetIsoSurfaceCorners(const Real& isoValue,const int& subdivideDepth,const int& fullDepthIso){
	int i,j;
	unordered_map<long long,Real> values;
	Real cornerValues[Cube::CORNERS];
	PointIndexValueFunction cf;
	TreeOctNode* temp;
	int leafCount=tree.leaves();
	long long key;
	SortedTreeNodes *sNodes=new SortedTreeNodes();
	sNodes->Set(tree,0);
	temp=tree.nextNode();
	while(temp){
		temp->nodeData.mcIndex=0;
		temp=tree.nextNode(temp);
	}
	TreeNodeData::UseIndex=0;	
	// Start by setting the corner values of all the nodes
	cf.valueTables=fData.valueTables;
	cf.res2=fData.res2;
	for(i=0;i<sNodes->nodeCount[subdivideDepth];i++){
		temp=sNodes->treeNodes[i];
		if(!temp->children){
			for(j=0;j<Cube::CORNERS;j++){
				if(this->width<=3){cornerValues[j]=getCornerValue(temp,j);}
				else{
					cf.value=0;
					int idx[3];
					VertexData::CornerIndex(temp,j,fData.depth,idx);
					cf.index[0]=idx[0]*fData.res;
					cf.index[1]=idx[1]*fData.res;
					cf.index[2]=idx[2]*fData.res;
					TreeOctNode::ProcessPointAdjacentNodes(fData.depth,idx,&tree,width,&cf);
					cornerValues[j]=cf.value;
				}
			}
			temp->nodeData.mcIndex=MarchingCubes::GetIndex(cornerValues,isoValue);

			if(temp->parent){
				TreeOctNode* parent=temp->parent;
				int c=int(temp-temp->parent->children);
				int mcid=temp->nodeData.mcIndex&(1<<MarchingCubes::cornerMap[c]);
				
				if(mcid){
					parent->nodeData.mcIndex|=mcid;
					while(1){
						if(parent->parent && (parent-parent->parent->children)==c){
							parent->parent->nodeData.mcIndex|=mcid;
							parent=parent->parent;
						}
						else{break;}
					}
				}
			}
		}
	}

	MemoryUsage();

	for(i=sNodes->nodeCount[subdivideDepth];i<sNodes->nodeCount[subdivideDepth+1];i++){
		temp=sNodes->treeNodes[i]->nextLeaf();
		while(temp){
			for(j=0;j<Cube::CORNERS;j++){
				int idx[3];
				key=VertexData::CornerIndex(temp,j,fData.depth,idx);
				cf.index[0]=idx[0]*fData.res;
				cf.index[1]=idx[1]*fData.res;
				cf.index[2]=idx[2]*fData.res;
				if(values.find(key)!=values.end()){cornerValues[j]=values[key];}
				else{
					if(this->width<=3){values[key]=cornerValues[j]=getCornerValue(temp,j);}
					else{
						cf.value=0;
						TreeOctNode::ProcessPointAdjacentNodes(fData.depth,idx,&tree,width,&cf);
						values[key]=cf.value;
						cornerValues[j]=cf.value;
					}
				}
			}
			temp->nodeData.mcIndex=MarchingCubes::GetIndex(cornerValues,isoValue);

			if(temp->parent){
				TreeOctNode* parent=temp->parent;
				int c=int(temp-temp->parent->children);
				int mcid=temp->nodeData.mcIndex&(1<<MarchingCubes::cornerMap[c]);
				
				if(mcid){
					parent->nodeData.mcIndex|=mcid;
					while(1){
						if(parent->parent && (parent-parent->parent->children)==c){
							parent->parent->nodeData.mcIndex|=mcid;
							parent=parent->parent;
						}
						else{break;}
					}
				}
			}

			temp=sNodes->treeNodes[i]->nextLeaf(temp);
		}
		MemoryUsage();
		values.clear();
	}
	delete sNodes;

	if(subdivideDepth){PreValidate(isoValue,fData.depth,subdivideDepth);}
}
template<int Degree>
void Octree<Degree>::Subdivide(TreeOctNode* node,const Real& isoValue,const int& maxDepth){
	int i,j,c[4];
	Real value;
	int cornerIndex2[Cube::CORNERS];
	PointIndexValueFunction cf;
	cf.valueTables=fData.valueTables;
	cf.res2=fData.res2;
	node->initChildren();
	// Since we are allocating blocks, it is possible that some of the memory was pre-allocated with
	// the wrong initialization

	// Now set the corner values for the new children
	// Copy old corner values
	for(i=0;i<Cube::CORNERS;i++){cornerIndex2[i]=node->nodeData.mcIndex&(1<<MarchingCubes::cornerMap[i]);}
	// 8 of 27 corners set

	// Set center corner
	cf.value=0;
	int idx[3];
	VertexData::CenterIndex(node,maxDepth,idx);
	cf.index[0]=idx[0]*fData.res;
	cf.index[1]=idx[1]*fData.res;
	cf.index[2]=idx[2]*fData.res;
	if(this->width<=3){value=getCenterValue(node);}
	else{
		TreeOctNode::ProcessPointAdjacentNodes(fData.depth,idx,&tree,width,&cf);
		value=cf.value;
	}
	if(value<isoValue){for(i=0;i<Cube::CORNERS;i++){cornerIndex2[i]|=1<<MarchingCubes::cornerMap[Cube::AntipodalCornerIndex(i)];}}
	// 9 of 27 set

	// Set face corners
	for(i=0;i<Cube::NEIGHBORS;i++){
		int dir,offset,e;
		Cube::FactorFaceIndex(i,dir,offset);
		cf.value=0;
		int idx[3];
		VertexData::FaceIndex(node,i,maxDepth,idx);
		cf.index[0]=idx[0]*fData.res;
		cf.index[1]=idx[1]*fData.res;
		cf.index[2]=idx[2]*fData.res;
		TreeOctNode::ProcessPointAdjacentNodes(fData.depth,idx,&tree,width,&cf);
		value=cf.value;
		Cube::FaceCorners(i,c[0],c[1],c[2],c[3]);
		e=Cube::EdgeIndex(dir,0,0);
		if(value<isoValue){for(j=0;j<4;j++){cornerIndex2[c[j]]|=1<<MarchingCubes::cornerMap[Cube::EdgeReflectCornerIndex(c[j],e)];}}
	}
	// 15 of 27 set

	// Set edge corners
	for(i=0;i<Cube::EDGES;i++){
		int o,i1,i2,f;
		Cube::FactorEdgeIndex(i,o,i1,i2);
		cf.value=0;
		int idx[3];
		VertexData::EdgeIndex(node,i,maxDepth,idx);
		cf.index[0]=idx[0]*fData.res;
		cf.index[1]=idx[1]*fData.res;
		cf.index[2]=idx[2]*fData.res;
		TreeOctNode::ProcessPointAdjacentNodes(fData.depth,idx,&tree,width,&cf);
		value=cf.value;
		Cube::EdgeCorners(i,c[0],c[1]);
		f=Cube::FaceIndex(o,0);
		if(value<isoValue){for(j=0;j<2;j++){cornerIndex2[c[j]]|=1<<MarchingCubes::cornerMap[Cube::FaceReflectCornerIndex(c[j],f)];}}
	}
	// 27 of 27 set

	for(i=0;i<Cube::CORNERS;i++){node->children[i].nodeData.mcIndex=cornerIndex2[i];}
}

template<int Degree>
int Octree<Degree>::InteriorFaceRootCount(const TreeOctNode* node,const int &faceIndex,const int& maxDepth){
	int c1,c2,e1,e2,dir,off,cnt=0;
	int corners[Cube::CORNERS/2];
	if(node->children){
		Cube::FaceCorners(faceIndex,corners[0],corners[1],corners[2],corners[3]);
		Cube::FactorFaceIndex(faceIndex,dir,off);
		c1=corners[0];
		c2=corners[3];
		switch(dir){
			case 0:
				e1=Cube::EdgeIndex(1,off,1);
				e2=Cube::EdgeIndex(2,off,1);
				break;
			case 1:
				e1=Cube::EdgeIndex(0,off,1);
				e2=Cube::EdgeIndex(2,1,off);
				break;
			case 2:
				e1=Cube::EdgeIndex(0,1,off);
				e2=Cube::EdgeIndex(1,1,off);
				break;
		};
		cnt+=EdgeRootCount(&node->children[c1],e1,maxDepth)+EdgeRootCount(&node->children[c1],e2,maxDepth);
		switch(dir){
			case 0:
				e1=Cube::EdgeIndex(1,off,0);
				e2=Cube::EdgeIndex(2,off,0);
				break;
			case 1:
				e1=Cube::EdgeIndex(0,off,0);
				e2=Cube::EdgeIndex(2,0,off);
				break;
			case 2:
				e1=Cube::EdgeIndex(0,0,off);
				e2=Cube::EdgeIndex(1,0,off);
				break;
		};
		cnt+=EdgeRootCount(&node->children[c2],e1,maxDepth)+EdgeRootCount(&node->children[c2],e2,maxDepth);
		for(int i=0;i<Cube::CORNERS/2;i++){if(node->children[corners[i]].children){cnt+=InteriorFaceRootCount(&node->children[corners[i]],faceIndex,maxDepth);}}
	}
	return cnt;
}

template<int Degree>
int Octree<Degree>::EdgeRootCount(const TreeOctNode* node,const int& edgeIndex,const int& maxDepth){
	int f1,f2,c1,c2;
	const TreeOctNode* temp;
	Cube::FacesAdjacentToEdge(edgeIndex,f1,f2);

	int eIndex;
	const TreeOctNode* finest=node;
	eIndex=edgeIndex;
	if(node->depth()<maxDepth){
		temp=node->faceNeighbor(f1);
		if(temp && temp->children){
			finest=temp;
			eIndex=Cube::FaceReflectEdgeIndex(edgeIndex,f1);
		}
		else{
			temp=node->faceNeighbor(f2);
			if(temp && temp->children){
				finest=temp;
				eIndex=Cube::FaceReflectEdgeIndex(edgeIndex,f2);
			}
			else{
				temp=node->edgeNeighbor(edgeIndex);
				if(temp && temp->children){
					finest=temp;
					eIndex=Cube::EdgeReflectEdgeIndex(edgeIndex);
				}
			}
		}
	}

	Cube::EdgeCorners(eIndex,c1,c2);
	if(finest->children){return EdgeRootCount(&finest->children[c1],eIndex,maxDepth)+EdgeRootCount(&finest->children[c2],eIndex,maxDepth);}
	else{return MarchingCubes::HasEdgeRoots(finest->nodeData.mcIndex,eIndex);}
}
template<int Degree>
int Octree<Degree>::IsBoundaryFace(const TreeOctNode* node,const int& faceIndex,const int& subdivideDepth){
	int dir,offset,d,o[3],idx;

	if(subdivideDepth<0){return 0;}
	if(node->d<=subdivideDepth){return 1;}
	Cube::FactorFaceIndex(faceIndex,dir,offset);
	node->depthAndOffset(d,o);

	idx=(int(o[dir])<<1) + (offset<<1);
	return !(idx%(2<<(int(node->d)-subdivideDepth)));
}
template<int Degree>
int Octree<Degree>::IsBoundaryEdge(const TreeOctNode* node,const int& edgeIndex,const int& subdivideDepth){
	int dir,x,y;
	Cube::FactorEdgeIndex(edgeIndex,dir,x,y);
	return IsBoundaryEdge(node,dir,x,y,subdivideDepth);
}
template<int Degree>
int Octree<Degree>::IsBoundaryEdge(const TreeOctNode* node,const int& dir,const int& x,const int& y,const int& subdivideDepth){
	int d,o[3],idx1,idx2,mask;

	if(subdivideDepth<0){return 0;}
	if(node->d<=subdivideDepth){return 1;}
	node->depthAndOffset(d,o);

	switch(dir){
		case 0:
			idx1=(int(o[1])<<1) + (x<<1);
			idx2=(int(o[2])<<1) + (y<<1);
			break;
		case 1:
			idx1=(int(o[0])<<1) + (x<<1);
			idx2=(int(o[2])<<1) + (y<<1);
			break;
		case 2:
			idx1=(int(o[0])<<1) + (x<<1);
			idx2=(int(o[1])<<1) + (y<<1);
			break;
	}
	mask=2<<(int(node->d)-subdivideDepth);
	return !(idx1%(mask)) || !(idx2%(mask));
}

template<int Degree>
void Octree<Degree>::PreValidate(TreeOctNode* node,const Real& isoValue,const int& maxDepth,const int& subdivideDepth){
	int sub=0;
	if(node->children){printf("Bad Pre-Validate\n");}
//	if(int(node->d)<subdivideDepth){sub=1;}
	for(int i=0;i<Cube::NEIGHBORS && !sub;i++){
		TreeOctNode* neighbor=node->faceNeighbor(i);
		if(neighbor && neighbor->children){
			if(IsBoundaryFace(node,i,subdivideDepth) && InteriorFaceRootCount(neighbor,Cube::FaceReflectFaceIndex(i,i),maxDepth)){sub=1;}
		}
	}
	if(sub){
		Subdivide(node,isoValue,maxDepth);
		for(int i=0;i<Cube::NEIGHBORS;i++){
			if(IsBoundaryFace(node,i,subdivideDepth) && InteriorFaceRootCount(node,i,maxDepth)){
				TreeOctNode* neighbor=node->faceNeighbor(i);
				while(neighbor && !neighbor->children){
					PreValidate(neighbor,isoValue,maxDepth,subdivideDepth);
					neighbor=node->faceNeighbor(i);
				}
			}
		}
	}
}

template<int Degree>
void Octree<Degree>::PreValidate(const Real& isoValue,const int& maxDepth,const int& subdivideDepth){
	TreeOctNode* temp;

	temp=tree.nextLeaf();
	while(temp){
		PreValidate(temp,isoValue,maxDepth,subdivideDepth);
		temp=tree.nextLeaf(temp);
	}
}
template<int Degree>
void Octree<Degree>::Validate(TreeOctNode* node,const Real& isoValue,const int& maxDepth,const int& fullDepthIso){
	int i,sub=0;
	TreeOctNode* treeNode=node;
	TreeOctNode* neighbor;
	if(node->depth()>=maxDepth || node->children){return;}

	// Check if full-depth extraction is enabled and we have an iso-node that is not at maximum depth
	if(!sub && fullDepthIso && MarchingCubes::HasRoots(node->nodeData.mcIndex)){sub=1;}

	// Check if the node has faces that are ambiguous and are adjacent to finer neighbors
	for(i=0;i<Cube::NEIGHBORS && !sub;i++){
		neighbor=treeNode->faceNeighbor(i);
		if(neighbor && neighbor->children){if(MarchingCubes::IsAmbiguous(node->nodeData.mcIndex,i)){sub=1;}}
	}

	// Check if the node has edges with more than one root
	for(i=0;i<Cube::EDGES && !sub;i++){if(EdgeRootCount(node,i,maxDepth)>1){sub=1;}}

	for(i=0;i<Cube::NEIGHBORS && !sub;i++){
		neighbor=node->faceNeighbor(i);
		if(	neighbor && neighbor->children &&
			!MarchingCubes::HasFaceRoots(node->nodeData.mcIndex,i) &&
			InteriorFaceRootCount(neighbor,Cube::FaceReflectFaceIndex(i,i),maxDepth)){sub=1;}
	}
	if(sub){
		Subdivide(node,isoValue,maxDepth);
		for(i=0;i<Cube::NEIGHBORS;i++){
			neighbor=treeNode->faceNeighbor(i);
			if(neighbor && !neighbor->children){Validate(neighbor,isoValue,maxDepth,fullDepthIso);}
		}
		for(i=0;i<Cube::EDGES;i++){
			neighbor=treeNode->edgeNeighbor(i);
			if(neighbor && !neighbor->children){Validate(neighbor,isoValue,maxDepth,fullDepthIso);}
		}
		for(i=0;i<Cube::CORNERS;i++){if(!node->children[i].children){Validate(&node->children[i],isoValue,maxDepth,fullDepthIso);}}
	}
}
template<int Degree>
void Octree<Degree>::Validate(TreeOctNode* node,const Real& isoValue,const int& maxDepth,const int& fullDepthIso,const int& subdivideDepth){
	int i,sub=0;
	TreeOctNode* treeNode=node;
	TreeOctNode* neighbor;
	if(node->depth()>=maxDepth || node->children){return;}

	// Check if full-depth extraction is enabled and we have an iso-node that is not at maximum depth
	if(!sub && fullDepthIso && MarchingCubes::HasRoots(node->nodeData.mcIndex)){sub=1;}

	// Check if the node has faces that are ambiguous and are adjacent to finer neighbors
	for(i=0;i<Cube::NEIGHBORS && !sub;i++){
		neighbor=treeNode->faceNeighbor(i);
		if(neighbor && neighbor->children){if(MarchingCubes::IsAmbiguous(node->nodeData.mcIndex,i) || IsBoundaryFace(node,i,subdivideDepth)){sub=1;}}
	}

	// Check if the node has edges with more than one root
	for(i=0;i<Cube::EDGES && !sub;i++){if(EdgeRootCount(node,i,maxDepth)>1){sub=1;}}

	for(i=0;i<Cube::NEIGHBORS && !sub;i++){
		neighbor=node->faceNeighbor(i);
		if(	neighbor && neighbor->children && !MarchingCubes::HasFaceRoots(node->nodeData.mcIndex,i) &&
			InteriorFaceRootCount(neighbor,Cube::FaceReflectFaceIndex(i,i),maxDepth)){sub=1;}
	}
	if(sub){
		Subdivide(node,isoValue,maxDepth);
		for(i=0;i<Cube::NEIGHBORS;i++){
			neighbor=treeNode->faceNeighbor(i);
			if(neighbor && !neighbor->children){Validate(neighbor,isoValue,maxDepth,fullDepthIso,subdivideDepth);}
		}
		for(i=0;i<Cube::EDGES;i++){
			neighbor=treeNode->edgeNeighbor(i);
			if(neighbor && !neighbor->children){Validate(neighbor,isoValue,maxDepth,fullDepthIso,subdivideDepth);}
		}
		for(i=0;i<Cube::CORNERS;i++){if(!node->children[i].children){Validate(&node->children[i],isoValue,maxDepth,fullDepthIso,subdivideDepth);}}
	}
}
//////////////////////////////////////////////////////////////////////////////////////
// The assumption made when calling this code is that the edge has at most one root //
//////////////////////////////////////////////////////////////////////////////////////
template<int Degree>
int Octree<Degree>::GetRoot(const RootInfo& ri,const Real& isoValue,Point3D<Real> & position,unordered_map<long long,std::pair<Real,Point3D<Real> > >& normalHash,const int& nonLinearFit){
	int c1,c2;
	Cube::EdgeCorners(ri.EdgeIndex,c1,c2);
	if(!MarchingCubes::HasEdgeRoots(ri.Node->nodeData.mcIndex,ri.EdgeIndex)){return 0;}

	long long key;
	Point3D<Real> n[2];
	PointIndexValueAndNormalFunction cnf;
	cnf.valueTables=fData.valueTables;
	cnf.dValueTables=fData.dValueTables;
	cnf.res2=fData.res2;

	int i,o,i1,i2,rCount=0;
	Polynomial<2> P;
	std::vector<double> roots;
	double x0,x1;
	Real center,width;
	Real averageRoot=0;
	Cube::FactorEdgeIndex(ri.EdgeIndex,o,i1,i2);
	int idx[3];
	key=VertexData::CornerIndex(ri.Node,c1,fData.depth,idx);
	cnf.index[0]=idx[0]*fData.res;
	cnf.index[1]=idx[1]*fData.res;
	cnf.index[2]=idx[2]*fData.res;

	if(normalHash.find(key)==normalHash.end()){
		cnf.value=0;
		cnf.normal.coords[0]=cnf.normal.coords[1]=cnf.normal.coords[2]=0;
		// Careful here as the normal isn't quite accurate... (i.e. postNormalSmooth is ignored)
		if(this->width<=3){getCornerValueAndNormal(ri.Node,c1,cnf.value,cnf.normal);}
		else{TreeOctNode::ProcessPointAdjacentNodes(fData.depth,idx,&tree,this->width,&cnf);}
		normalHash[key]=std::pair<Real,Point3D<Real> >(cnf.value,cnf.normal);
	}
	x0=normalHash[key].first;
	n[0]=normalHash[key].second;

	key=VertexData::CornerIndex(ri.Node,c2,fData.depth,idx);
	cnf.index[0]=idx[0]*fData.res;
	cnf.index[1]=idx[1]*fData.res;
	cnf.index[2]=idx[2]*fData.res;
	if(normalHash.find(key)==normalHash.end()){
		cnf.value=0;
		cnf.normal.coords[0]=cnf.normal.coords[1]=cnf.normal.coords[2]=0;
		if(this->width<=3){getCornerValueAndNormal(ri.Node,c2,cnf.value,cnf.normal);}
		else{TreeOctNode::ProcessPointAdjacentNodes(fData.depth,idx,&tree,this->width,&cnf);}
		normalHash[key]=std::pair<Real,Point3D<Real> >(cnf.value,cnf.normal);
	}
	x1=normalHash[key].first;
	n[1]=normalHash[key].second;

	Point3D<Real> c;
	ri.Node->centerAndWidth(c,width);
	center=c.coords[o];
	for(i=0;i<DIMENSION;i++){
		n[0].coords[i]*=width;
		n[1].coords[i]*=width;
	}

	switch(o){
				case 0:
					position.coords[1]=c.coords[1]-width/2+width*i1;
					position.coords[2]=c.coords[2]-width/2+width*i2;
					break;
				case 1:
					position.coords[0]=c.coords[0]-width/2+width*i1;
					position.coords[2]=c.coords[2]-width/2+width*i2;
					break;
				case 2:
					position.coords[0]=c.coords[0]-width/2+width*i1;
					position.coords[1]=c.coords[1]-width/2+width*i2;
					break;
	}
	double dx0,dx1;
	dx0=n[0].coords[o];
	dx1=n[1].coords[o];

	// The scaling will turn the Hermite Spline into a quadratic
	double scl=(x1-x0)/((dx1+dx0)/2);
	dx0*=scl;
	dx1*=scl;

	// Hermite Spline
	P.coefficients[0]=x0;
	P.coefficients[1]=dx0;
	P.coefficients[2]=3*(x1-x0)-dx1-2*dx0;

	P.GetSolutions(isoValue,roots,EPSILON);
	for(i=0;i<int(roots.size());i++){
		if(roots[i]>=0 && roots[i]<=1){
			averageRoot+=Real(roots[i]);
			rCount++;
		}
	}
	if(rCount && nonLinearFit)	{averageRoot/=rCount;}
	else						{averageRoot=Real((x0-isoValue)/(x0-x1));}

	position.coords[o]=Real(center-width/2+width*averageRoot);
	return 1;
}

template<int Degree>
int Octree<Degree>::GetRoot(const RootInfo& ri,const Real& isoValue,const int& maxDepth,Point3D<Real>& position,unordered_map<long long,std::pair<Real,Point3D<Real> > >& normals,
							Point3D<Real>* normal,const int& nonLinearFit){
	if(!MarchingCubes::HasRoots(ri.Node->nodeData.mcIndex)){return 0;}
	return GetRoot(ri,isoValue,position,normals,nonLinearFit);
}
template<int Degree>
int Octree<Degree>::GetRootIndex(const TreeOctNode* node,const int& edgeIndex,const int& maxDepth,const int& sDepth,RootInfo& ri){
	int c1,c2,f1,f2;
	const TreeOctNode *temp,*finest;
	int finestIndex;

	Cube::FacesAdjacentToEdge(edgeIndex,f1,f2);

	finest=node;
	finestIndex=edgeIndex;
	if(node->depth()<maxDepth){
		if(IsBoundaryFace(node,f1,sDepth)){temp=NULL;}
		else{temp=node->faceNeighbor(f1);}
		if(temp && temp->children){
			finest=temp;
			finestIndex=Cube::FaceReflectEdgeIndex(edgeIndex,f1);
		}
		else{
			if(IsBoundaryFace(node,f2,sDepth)){temp=NULL;}
			else{temp=node->faceNeighbor(f2);}
			if(temp && temp->children){
				finest=temp;
				finestIndex=Cube::FaceReflectEdgeIndex(edgeIndex,f2);
			}
			else{
				if(IsBoundaryEdge(node,edgeIndex,sDepth)){temp=NULL;}
				else{temp=node->edgeNeighbor(edgeIndex);}
				if(temp && temp->children){
					finest=temp;
					finestIndex=Cube::EdgeReflectEdgeIndex(edgeIndex);
				}
			}
		}
	}

	Cube::EdgeCorners(finestIndex,c1,c2);
	if(finest->children){
		if		(GetRootIndex(&finest->children[c1],finestIndex,maxDepth,sDepth,ri))	{return 1;}
		else if	(GetRootIndex(&finest->children[c2],finestIndex,maxDepth,sDepth,ri))	{return 1;}
		else																							{return 0;}
	}
	else{
		if(!(MarchingCubes::edgeMask[finest->nodeData.mcIndex] & (1<<finestIndex))){return 0;}

		int o,i1,i2;
		Cube::FactorEdgeIndex(finestIndex,o,i1,i2);
		int d,off[3];
		finest->depthAndOffset(d,off);
		ri.Node=finest;
		ri.EdgeIndex=finestIndex;
		int eIndex[2],offset;
		offset=BinaryNode<Real>::Index(d,off[o]);
		switch(o){
				case 0:
					eIndex[0]=BinaryNode<Real>::CornerIndex(maxDepth+1,d,off[1],i1);
					eIndex[1]=BinaryNode<Real>::CornerIndex(maxDepth+1,d,off[2],i2);
					break;
				case 1:
					eIndex[0]=BinaryNode<Real>::CornerIndex(maxDepth+1,d,off[0],i1);
					eIndex[1]=BinaryNode<Real>::CornerIndex(maxDepth+1,d,off[2],i2);
					break;
				case 2:
					eIndex[0]=BinaryNode<Real>::CornerIndex(maxDepth+1,d,off[0],i1);
					eIndex[1]=BinaryNode<Real>::CornerIndex(maxDepth+1,d,off[1],i2);
					break;
		}
		ri.Key = (long long)(o) | (long long)(eIndex[0])<<5 | (long long)(eIndex[1])<<25 | (long long)(offset)<<45;
		return 1;
	}
}
template<int Degree>
int Octree<Degree>::GetRootIndex(const TreeOctNode* node,const int& edgeIndex,const int& maxDepth,RootInfo& ri){
	int c1,c2,f1,f2;
	const TreeOctNode *temp,*finest;
	int finestIndex;


	// The assumption is that the super-edge has a root along it. 
	if(!(MarchingCubes::edgeMask[node->nodeData.mcIndex] & (1<<edgeIndex))){return 0;}

	Cube::FacesAdjacentToEdge(edgeIndex,f1,f2);

	finest=node;
	finestIndex=edgeIndex;
	if(node->depth()<maxDepth){
		temp=node->faceNeighbor(f1);
		if(temp && temp->children){
			finest=temp;
			finestIndex=Cube::FaceReflectEdgeIndex(edgeIndex,f1);
		}
		else{
			temp=node->faceNeighbor(f2);
			if(temp && temp->children){
				finest=temp;
				finestIndex=Cube::FaceReflectEdgeIndex(edgeIndex,f2);
			}
			else{
				temp=node->edgeNeighbor(edgeIndex);
				if(temp && temp->children){
					finest=temp;
					finestIndex=Cube::EdgeReflectEdgeIndex(edgeIndex);
				}
			}
		}
	}

	Cube::EdgeCorners(finestIndex,c1,c2);
	if(finest->children){
		if		(GetRootIndex(&finest->children[c1],finestIndex,maxDepth,ri))				{return 1;}
		else if	(GetRootIndex(&finest->children[c2],finestIndex,maxDepth,ri))				{return 1;}
		else																				{return 0;}
	}
	else{
		int o,i1,i2;
		Cube::FactorEdgeIndex(finestIndex,o,i1,i2);
		int d,off[3];
		finest->depthAndOffset(d,off);
		ri.Node=finest;
		ri.EdgeIndex=finestIndex;
		int offset,eIndex[2];
		offset=BinaryNode<Real>::Index(d,off[o]);
		switch(o){
				case 0:
					eIndex[0]=BinaryNode<Real>::CornerIndex(maxDepth+1,d,off[1],i1);
					eIndex[1]=BinaryNode<Real>::CornerIndex(maxDepth+1,d,off[2],i2);
					break;
				case 1:
					eIndex[0]=BinaryNode<Real>::CornerIndex(maxDepth+1,d,off[0],i1);
					eIndex[1]=BinaryNode<Real>::CornerIndex(maxDepth+1,d,off[2],i2);
					break;
				case 2:
					eIndex[0]=BinaryNode<Real>::CornerIndex(maxDepth+1,d,off[0],i1);
					eIndex[1]=BinaryNode<Real>::CornerIndex(maxDepth+1,d,off[1],i2);
					break;
		}
		ri.Key= (long long)(o) | (long long)(eIndex[0])<<5 | (long long)(eIndex[1])<<25 | (long long)(offset)<<45;
		return 1;
	}
}
template<int Degree>
int Octree<Degree>::GetRootPair(const RootInfo& ri,const int& maxDepth,RootInfo& pair){
	const TreeOctNode* node=ri.Node;
	int c1,c2,c;
	Cube::EdgeCorners(ri.EdgeIndex,c1,c2);
	while(node->parent){
		c=int(node-node->parent->children);
		if(c!=c1 && c!=c2){return 0;}
		if(!MarchingCubes::HasEdgeRoots(node->parent->nodeData.mcIndex,ri.EdgeIndex)){
			if(c==c1){return GetRootIndex(&node->parent->children[c2],ri.EdgeIndex,maxDepth,pair);}
			else{return GetRootIndex(&node->parent->children[c1],ri.EdgeIndex,maxDepth,pair);}
		}
		node=node->parent;
	}
	return 0;

}
template<int Degree>
int Octree<Degree>::GetRootIndex(const long long& key,unordered_map<long long,int>& boundaryRoots,unordered_map<long long,int>* interiorRoots,CoredPointIndex& index){
	unordered_map<long long,int>::iterator rootIter=boundaryRoots.find(key);
	if(rootIter!=boundaryRoots.end()){
		index.inCore=1;
		index.index=rootIter->second;
		return 1;
	}
	else if(interiorRoots){
		rootIter=interiorRoots->find(key);
		if(rootIter!=interiorRoots->end()){
			index.inCore=0;
			index.index=rootIter->second;
			return 1;
		}
	}
	return 0;
}
template<int Degree>
int Octree<Degree>::SetMCRootPositions(TreeOctNode* node,const int& sDepth,const Real& isoValue,
									   unordered_map<long long,int>& boundaryRoots,unordered_map<long long,int>* interiorRoots,
									   unordered_map<long long,std::pair<Real,Point3D<Real> > >& boundaryNormalHash,unordered_map<long long,std::pair<Real,Point3D<Real> > >* interiorNormalHash,
									   std::vector<Point3D<float> >* interiorPositions,
									   CoredMeshData* mesh,const int& nonLinearFit){
	Point3D<Real> position;
	int i,j,k,eIndex;
	RootInfo ri;
	int count=0;
	if(!MarchingCubes::HasRoots(node->nodeData.mcIndex)){return 0;}
	for(i=0;i<DIMENSION;i++){
		for(j=0;j<2;j++){
			for(k=0;k<2;k++){
				long long key;
				eIndex=Cube::EdgeIndex(i,j,k);
				if(GetRootIndex(node,eIndex,fData.depth,ri)){
					key=ri.Key;
					if(!interiorRoots || IsBoundaryEdge(node,i,j,k,sDepth)){
						if(boundaryRoots.find(key)==boundaryRoots.end()){
							GetRoot(ri,isoValue,fData.depth,position,boundaryNormalHash,NULL,nonLinearFit);
							mesh->inCorePoints.push_back(position);
							boundaryRoots[key]=int(mesh->inCorePoints.size())-1;
							count++;
						}
					}
					else{
						if(interiorRoots->find(key)==interiorRoots->end()){
							GetRoot(ri,isoValue,fData.depth,position,*interiorNormalHash,NULL,nonLinearFit);
							(*interiorRoots)[key]=mesh->AddOutOfCorePoint(position);
							interiorPositions->push_back(position);
							count++;
						}
					}
				}
			}
		}
	}
	return count;
}
template<int Degree>
int Octree<Degree>::SetBoundaryMCRootPositions(const int& sDepth,const Real& isoValue,
											   unordered_map<long long,int>& boundaryRoots,unordered_map<long long,std::pair<Real,Point3D<Real> > >& boundaryNormalHash,
											   CoredMeshData* mesh,const int& nonLinearFit){
	Point3D<Real> position;
	int i,j,k,eIndex,hits;
	RootInfo ri;
	int count=0;
	TreeOctNode* node;

	node=tree.nextLeaf();
	while(node){
		if(MarchingCubes::HasRoots(node->nodeData.mcIndex)){
			hits=0;
			for(i=0;i<DIMENSION;i++){
				for(j=0;j<2;j++){
					for(k=0;k<2;k++){
						if(IsBoundaryEdge(node,i,j,k,sDepth)){
							hits++;
							long long key;
							eIndex=Cube::EdgeIndex(i,j,k);
							if(GetRootIndex(node,eIndex,fData.depth,ri)){
								key=ri.Key;
								if(boundaryRoots.find(key)==boundaryRoots.end()){
									GetRoot(ri,isoValue,fData.depth,position,boundaryNormalHash,NULL,nonLinearFit);
									mesh->inCorePoints.push_back(position);
									boundaryRoots[key]=int(mesh->inCorePoints.size())-1;
									count++;
								}
							}
						}
					}
				}
			}
		}
		if(hits){node=tree.nextLeaf(node);}
		else{node=tree.nextBranch(node);}
	}
	return count;
}
template<int Degree>
void Octree<Degree>::GetMCIsoEdges(TreeOctNode* node,unordered_map<long long,int>& boundaryRoots,unordered_map<long long,int>* interiorRoots,const int& sDepth,
								   std::vector<std::pair<long long,long long> >& edges){
	TreeOctNode* temp;
	int count=0,tris=0;
	int isoTri[DIMENSION*MarchingCubes::MAX_TRIANGLES];
	FaceEdgesFunction fef;
	int ref,fIndex;
	unordered_map<long long,std::pair<RootInfo,int> >::iterator iter;
	unordered_map<long long,std::pair<RootInfo,int> > vertexCount;

	fef.edges=&edges;
	fef.maxDepth=fData.depth;
	fef.vertexCount=&vertexCount;
	count=MarchingCubes::AddTriangleIndices(node->nodeData.mcIndex,isoTri);
	for(fIndex=0;fIndex<Cube::NEIGHBORS;fIndex++){
		ref=Cube::FaceReflectFaceIndex(fIndex,fIndex);
		fef.fIndex=ref;
		temp=node->faceNeighbor(fIndex);
		// If the face neighbor exists and has higher resolution than the current node,
		// get the iso-curve from the neighbor
		if(temp && temp->children && !IsBoundaryFace(node,fIndex,sDepth)){temp->processNodeFaces(temp,&fef,ref);}
		// Otherwise, get it from the node
		else{
			RootInfo ri1,ri2;
			for(int j=0;j<count;j++){
				for(int k=0;k<3;k++){
					if(fIndex==Cube::FaceAdjacentToEdges(isoTri[j*3+k],isoTri[j*3+((k+1)%3)])){
						if(GetRootIndex(node,isoTri[j*3+k],fData.depth,ri1) && GetRootIndex(node,isoTri[j*3+((k+1)%3)],fData.depth,ri2)){
							edges.push_back(std::pair<long long,long long>(ri1.Key,ri2.Key));
							iter=vertexCount.find(ri1.Key);
							if(iter==vertexCount.end()){
								vertexCount[ri1.Key].first=ri1;
								vertexCount[ri1.Key].second=0;
							}
							iter=vertexCount.find(ri2.Key);
							if(iter==vertexCount.end()){
								vertexCount[ri2.Key].first=ri2;
								vertexCount[ri2.Key].second=0;
							}
							vertexCount[ri1.Key].second++;
							vertexCount[ri2.Key].second--;
						}
						else{fprintf(stderr,"Bad Edge 1: %d %d\n",ri1.Key,ri2.Key);}
					}
				}
			}
		}
	}
	for(int i=0;i<int(edges.size());i++){
		iter=vertexCount.find(edges[i].first);
		if(iter==vertexCount.end()){printf("Could not find vertex: %lld\n",edges[i].first);}
		else if(vertexCount[edges[i].first].second){
			RootInfo ri;
			GetRootPair(vertexCount[edges[i].first].first,fData.depth,ri);
			iter=vertexCount.find(ri.Key);
			if(iter==vertexCount.end()){printf("Vertex pair not in list\n");}
			else{
				edges.push_back(std::pair<long long,long long>(ri.Key,edges[i].first));
				vertexCount[ri.Key].second++;
				vertexCount[edges[i].first].second--;
			}
		}

		iter=vertexCount.find(edges[i].second);
		if(iter==vertexCount.end()){printf("Could not find vertex: %lld\n",edges[i].second);}
		else if(vertexCount[edges[i].second].second){
			RootInfo ri;
			GetRootPair(vertexCount[edges[i].second].first,fData.depth,ri);
			iter=vertexCount.find(ri.Key);
			if(iter==vertexCount.end()){printf("Vertex pair not in list\n");}
			else{
				edges.push_back(std::pair<long long,long long>(edges[i].second,ri.Key));
				vertexCount[edges[i].second].second++;
				vertexCount[ri.Key].second--;
			}
		}
	}
}
template<int Degree>
int Octree<Degree>::GetMCIsoTriangles(TreeOctNode* node,CoredMeshData* mesh,unordered_map<long long,int>& boundaryRoots,
									  unordered_map<long long,int>* interiorRoots,std::vector<Point3D<float> >* interiorPositions,const int& offSet,const int& sDepth)
{
	int tris=0;
	std::vector<std::pair<long long,long long> > edges;
	std::vector<std::vector<std::pair<long long,long long> > > edgeLoops;
	GetMCIsoEdges(node,boundaryRoots,interiorRoots,sDepth,edges);

	GetEdgeLoops(edges,edgeLoops);
	for(int i=0;i<int(edgeLoops.size());i++){
		CoredPointIndex p;
		std::vector<CoredPointIndex> edgeIndices;
		for(int j=0;j<int(edgeLoops[i].size());j++){
			if(!GetRootIndex(edgeLoops[i][j].first,boundaryRoots,interiorRoots,p)){printf("Bad Point Index\n");}
			else{edgeIndices.push_back(p);}
		}
		tris+=AddTriangles(mesh,edgeIndices,interiorPositions,offSet);
	}
	return tris;
}

template<int Degree>
int Octree<Degree>::GetEdgeLoops(std::vector<std::pair<long long,long long> >& edges,std::vector<std::vector<std::pair<long long,long long> > >& loops){
	int loopSize=0;
	long long frontIdx,backIdx;
	std::pair<long long,long long> e,temp;
	loops.clear();

	while(edges.size()){
		std::vector<std::pair<long long,long long> > front,back;
		e=edges[0];
		loops.resize(loopSize+1);
		edges[0]=edges[edges.size()-1];
		edges.pop_back();
		frontIdx=e.second;
		backIdx=e.first;
		for(int j=int(edges.size())-1;j>=0;j--){
			if(edges[j].first==frontIdx || edges[j].second==frontIdx){
				if(edges[j].first==frontIdx)	{temp=edges[j];}
				else							{temp.first=edges[j].second;temp.second=edges[j].first;}
				frontIdx=temp.second;
				front.push_back(temp);
				edges[j]=edges[edges.size()-1];
				edges.pop_back();
				j=int(edges.size());
			}
			else if(edges[j].first==backIdx || edges[j].second==backIdx){
				if(edges[j].second==backIdx)	{temp=edges[j];}
				else							{temp.first=edges[j].second;temp.second=edges[j].first;}
				backIdx=temp.first;
				back.push_back(temp);
				edges[j]=edges[edges.size()-1];
				edges.pop_back();
				j=int(edges.size());
			}
		}
		for(int j=int(back.size())-1;j>=0;j--){loops[loopSize].push_back(back[j]);}
		loops[loopSize].push_back(e);
		for(int j=0;j<int(front.size());j++){loops[loopSize].push_back(front[j]);}
		loopSize++;
	}
	return int(loops.size());
}
template<int Degree>
int Octree<Degree>::AddTriangles(CoredMeshData* mesh,std::vector<CoredPointIndex> edges[3],std::vector<Point3D<float> >* interiorPositions,const int& offSet){
	std::vector<CoredPointIndex> e;
	for(int i=0;i<3;i++){for(size_t j=0;j<edges[i].size();j++){e.push_back(edges[i][j]);}}
	return AddTriangles(mesh,e,interiorPositions,offSet);
}
template<int Degree>
int Octree<Degree>::AddTriangles(CoredMeshData* mesh,std::vector<CoredPointIndex>& edges,std::vector<Point3D<float> >* interiorPositions,const int& offSet){
	if(edges.size()>3){
		Triangulation<float> t;

		// Add the points to the triangulation
		for(int i=0;i<int(edges.size());i++){
			Point3D<Real> p;
			if(edges[i].inCore)	{for(int j=0;j<3;j++){p.coords[j]=mesh->inCorePoints[edges[i].index].coords[j];}}
			else				{for(int j=0;j<3;j++){p.coords[j]=(*interiorPositions)[edges[i].index-offSet].coords[j];}}
			t.points.push_back(p);
		}

		// Create a fan triangulation
		for(int i=1;i<int(edges.size())-1;i++){t.AddTriangle(0,i,i+1);}

		// Minimize
		while(1){
			int i;
			for(i=0;i<int(t.edges.size());i++){if(t.FlipMinimize(i)){break;}}
			if(i==t.edges.size()){break;}
		}
		// Add the triangles to the mesh
		for(int i=0;i<int(t.triangles.size());i++){
			TriangleIndex tri;
			int idx[3];
			int inCoreFlag=0;
			t.Factor(i,idx[0],idx[1],idx[2]);
			for(int j=0;j<3;j++){
				tri.idx[j]=edges[idx[j]].index;
				if(edges[idx[j]].inCore){inCoreFlag|=CoredMeshData::IN_CORE_FLAG[j];}
			}
			mesh->AddTriangle(tri,inCoreFlag);
		}
	}
	else if(edges.size()==3){
		TriangleIndex tri;
		int inCoreFlag=0;
		for(int i=0;i<3;i++){
			tri.idx[i]=edges[i].index;
			if(edges[i].inCore){inCoreFlag|=CoredMeshData::IN_CORE_FLAG[i];}
		}
		mesh->AddTriangle(tri,inCoreFlag);
	}
	return int(edges.size())-2;
}
////////////////
// VertexData //
////////////////
long long VertexData::CenterIndex(const TreeOctNode* node,const int& maxDepth){
	int idx[DIMENSION];
	return CenterIndex(node,maxDepth,idx);
}
long long VertexData::CenterIndex(const TreeOctNode* node,const int& maxDepth,int idx[DIMENSION]){
	int d,o[3];
	node->depthAndOffset(d,o);
	for(int i=0;i<DIMENSION;i++){idx[i]=BinaryNode<Real>::CornerIndex(maxDepth+1,d+1,o[i]<<1,1);}
	return (long long)(idx[0]) | (long long)(idx[1])<<15 | (long long)(idx[2])<<30;
}
long long VertexData::CenterIndex(const int& depth,const int offSet[DIMENSION],const int& maxDepth,int idx[DIMENSION]){
	for(int i=0;i<DIMENSION;i++){idx[i]=BinaryNode<Real>::CornerIndex(maxDepth+1,depth+1,offSet[i]<<1,1);}
	return (long long)(idx[0]) | (long long)(idx[1])<<15 | (long long)(idx[2])<<30;
}
long long VertexData::CornerIndex(const TreeOctNode* node,const int& cIndex,const int& maxDepth){
	int idx[DIMENSION];
	return CornerIndex(node,cIndex,maxDepth,idx);
}
long long VertexData::CornerIndex(const TreeOctNode* node,const int& cIndex,const int& maxDepth,int idx[DIMENSION]){
	int x[DIMENSION];
	Cube::FactorCornerIndex(cIndex,x[0],x[1],x[2]);
	int d,o[3];
	node->depthAndOffset(d,o);
	for(int i=0;i<DIMENSION;i++){idx[i]=BinaryNode<Real>::CornerIndex(maxDepth+1,d,o[i],x[i]);}
	return (long long)(idx[0]) | (long long)(idx[1])<<15 | (long long)(idx[2])<<30;
}
long long VertexData::CornerIndex(const int& depth,const int offSet[DIMENSION],const int& cIndex,const int& maxDepth,int idx[DIMENSION]){
	int x[DIMENSION];
	Cube::FactorCornerIndex(cIndex,x[0],x[1],x[2]);
	for(int i=0;i<DIMENSION;i++){idx[i]=BinaryNode<Real>::CornerIndex(maxDepth+1,depth,offSet[i],x[i]);}
	return (long long)(idx[0]) | (long long)(idx[1])<<15 | (long long)(idx[2])<<30;
}
long long VertexData::FaceIndex(const TreeOctNode* node,const int& fIndex,const int& maxDepth){
	int idx[DIMENSION];
	return FaceIndex(node,fIndex,maxDepth,idx);
}
long long VertexData::FaceIndex(const TreeOctNode* node,const int& fIndex,const int& maxDepth,int idx[DIMENSION]){
	int dir,offset;
	Cube::FactorFaceIndex(fIndex,dir,offset);
	int d,o[3];
	node->depthAndOffset(d,o);
	for(int i=0;i<DIMENSION;i++){idx[i]=BinaryNode<Real>::CornerIndex(maxDepth+1,d+1,o[i]<<1,1);}
	idx[dir]=BinaryNode<Real>::CornerIndex(maxDepth+1,d,o[dir],offset);
	return (long long)(idx[0]) | (long long)(idx[1])<<15 | (long long)(idx[2])<<30;
}
long long VertexData::EdgeIndex(const TreeOctNode* node,const int& eIndex,const int& maxDepth){
	int idx[DIMENSION];
	return EdgeIndex(node,eIndex,maxDepth,idx);
}
long long VertexData::EdgeIndex(const TreeOctNode* node,const int& eIndex,const int& maxDepth,int idx[DIMENSION]){
	int o,i1,i2;
	int d,off[3];
	node->depthAndOffset(d,off);
	for(int i=0;i<DIMENSION;i++){idx[i]=BinaryNode<Real>::CornerIndex(maxDepth+1,d+1,off[i]<<1,1);}
	Cube::FactorEdgeIndex(eIndex,o,i1,i2);
	switch(o){
		case 0:
			idx[1]=BinaryNode<Real>::CornerIndex(maxDepth+1,d,off[1],i1);
			idx[2]=BinaryNode<Real>::CornerIndex(maxDepth+1,d,off[2],i2);
			break;
		case 1:
			idx[0]=BinaryNode<Real>::CornerIndex(maxDepth+1,d,off[0],i1);
			idx[2]=BinaryNode<Real>::CornerIndex(maxDepth+1,d,off[2],i2);
			break;
		case 2:
			idx[0]=BinaryNode<Real>::CornerIndex(maxDepth+1,d,off[0],i1);
			idx[1]=BinaryNode<Real>::CornerIndex(maxDepth+1,d,off[1],i2);
			break;
	};
	return (long long)(idx[0]) | (long long)(idx[1])<<15 | (long long)(idx[2])<<30;
}



////////////////////////
//  factor.cpp
////////////////////////

int Factor(double a1,double a0,double roots[1][2],const double& EPS){
	if(fabs(a1)<=EPS){return 0;}
	roots[0][0]=-a0/a1;
	roots[0][1]=0;
	return 1;
}
int Factor(double a2,double a1,double a0,double roots[2][2],const double& EPS){
	double d;
	if(fabs(a2)<=EPS){return Factor(a1,a0,roots,EPS);}

	d=a1*a1-4*a0*a2;
	a1/=(2*a2);
	if(d<0){
		d=sqrt(-d)/(2*a2);
		roots[0][0]=roots[1][0]=-a1;
		roots[0][1]=-d;
		roots[1][1]= d;
	}
	else{
		d=sqrt(d)/(2*a2);
		roots[0][1]=roots[1][1]=0;
		roots[0][0]=-a1-d;
		roots[1][0]=-a1+d;
	}
	return 2;
}
// Solution taken from: http://mathworld.wolfram.com/CubicFormula.html
// and http://www.csit.fsu.edu/~burkardt/f_src/subpak/subpak.f90
int Factor(double a3,double a2,double a1,double a0,double roots[3][2],const double& EPS){
	double q,r,r2,q3;

	if(fabs(a3)<=EPS){return Factor(a2,a1,a0,roots,EPS);}
	a2/=a3;
	a1/=a3;
	a0/=a3;

	q=-(3*a1-a2*a2)/9;
	r=-(9*a2*a1-27*a0-2*a2*a2*a2)/54;
	r2=r*r;
	q3=q*q*q;

	if(r2<q3){
		double sqrQ=sqrt(q);
		double theta = acos ( r / (sqrQ*q) );
		double cTheta=cos(theta/3)*sqrQ;
		double sTheta=sin(theta/3)*sqrQ*SQRT_3/2;
		roots[0][1]=roots[1][1]=roots[2][1]=0;
		roots[0][0]=-2*cTheta;
		roots[1][0]=-2*(-cTheta*0.5-sTheta);
		roots[2][0]=-2*(-cTheta*0.5+sTheta);
	}
	else{
		double s1,s2,sqr=sqrt(r2-q3);
		double t;
		t=-r+sqr;
		if(t<0){s1=-pow(-t,1.0/3);}
		else{s1=pow(t,1.0/3);}
		t=-r-sqr;
		if(t<0){s2=-pow(-t,1.0/3);}
		else{s2=pow(t,1.0/3);}
		roots[0][1]=0;
		roots[0][0]=s1+s2;
		s1/=2;
		s2/=2;
		roots[1][0]= roots[2][0]=-s1-s2;
		roots[1][1]= SQRT_3*(s1-s2);
		roots[2][1]=-roots[1][1];
	}
	roots[0][0]-=a2/3;
	roots[1][0]-=a2/3;
	roots[2][0]-=a2/3;
	return 3;
}
double ArcTan2(const double& y,const double& x){
	/* This first case should never happen */
	if(y==0 && x==0){return 0;}
	if(x==0){
		if(y>0){return PI/2.0;}
		else{return -PI/2.0;}
	}
	if(x>=0){return atan(y/x);}
	else{
		if(y>=0){return atan(y/x)+PI;}
		else{return atan(y/x)-PI;}
	}
}
double Angle(const double in[2]){
	if((in[0]*in[0]+in[1]*in[1])==0.0){return 0;}
	else{return ArcTan2(in[1],in[0]);}
}
void Sqrt(const double in[2],double out[2]){
	double r=sqrt(sqrt(in[0]*in[0]+in[1]*in[1]));
	double a=Angle(in)*0.5;
	out[0]=r*cos(a);
	out[1]=r*sin(a);
}
void Add(const double in1[2],const double in2[2],double out[2]){
	out[0]=in1[0]+in2[0];
	out[1]=in1[1]+in2[1];
}
void Subtract(const double in1[2],const double in2[2],double out[2]){
	out[0]=in1[0]-in2[0];
	out[1]=in1[1]-in2[1];
}
void Multiply(const double in1[2],const double in2[2],double out[2]){
	out[0]=in1[0]*in2[0]-in1[1]*in2[1];
	out[1]=in1[0]*in2[1]+in1[1]*in2[0];
}
void Divide(const double in1[2],const double in2[2],double out[2]){
	double temp[2];
	double l=in2[0]*in2[0]+in2[1]*in2[1];
	temp[0]= in2[0]/l;
	temp[1]=-in2[1]/l;
	Multiply(in1,temp,out);
}
// Solution taken from: http://mathworld.wolfram.com/QuarticEquation.html
// and http://www.csit.fsu.edu/~burkardt/f_src/subpak/subpak.f90
int Factor(double a4,double a3,double a2,double a1,double a0,double roots[4][2],const double& EPS){
	double R[2],D[2],E[2],R2[2];

	if(fabs(a4)<EPS){return Factor(a3,a2,a1,a0,roots,EPS);}
	a3/=a4;
	a2/=a4;
	a1/=a4;
	a0/=a4;

	Factor(1.0,-a2,a3*a1-4.0*a0,-a3*a3*a0+4.0*a2*a0-a1*a1,roots,EPS);

	R2[0]=a3*a3/4.0-a2+roots[0][0];
	R2[1]=0;
	Sqrt(R2,R);
	if(fabs(R[0])>10e-8){
		double temp1[2],temp2[2];
		double p1[2],p2[2];

		p1[0]=a3*a3*0.75-2.0*a2-R2[0];
		p1[1]=0;

		temp2[0]=((4.0*a3*a2-8.0*a1-a3*a3*a3)/4.0);
		temp2[1]=0;
		Divide(temp2,R,p2);

		Add     (p1,p2,temp1);
		Subtract(p1,p2,temp2);

		Sqrt(temp1,D);
		Sqrt(temp2,E);
	}
	else{
		R[0]=R[1]=0;
		double temp1[2],temp2[2];
		temp1[0]=roots[0][0]*roots[0][0]-4.0*a0;
		temp1[1]=0;
		Sqrt(temp1,temp2);
		temp1[0]=a3*a3*0.75-2.0*a2+2.0*temp2[0];
		temp1[1]=                  2.0*temp2[1];
		Sqrt(temp1,D);
		temp1[0]=a3*a3*0.75-2.0*a2-2.0*temp2[0];
		temp1[1]=                 -2.0*temp2[1];
		Sqrt(temp1,E);
	}

	roots[0][0]=-a3/4.0+R[0]/2.0+D[0]/2.0;
	roots[0][1]=        R[1]/2.0+D[1]/2.0;

	roots[1][0]=-a3/4.0+R[0]/2.0-D[0]/2.0;
	roots[1][1]=        R[1]/2.0-D[1]/2.0;

	roots[2][0]=-a3/4.0-R[0]/2.0+E[0]/2.0;
	roots[2][1]=       -R[1]/2.0+E[1]/2.0;

	roots[3][0]=-a3/4.0-R[0]/2.0-E[0]/2.0;
	roots[3][1]=       -R[1]/2.0-E[1]/2.0;
	return 4;
}

int Solve(const double* eqns,const double* values,double* solutions,const int& dim){
	int i,j,eIndex;
	double v,m;
	int *index=new int[dim];
	int *set=new int[dim];
	double* myEqns=new double[dim*dim];
	double* myValues=new double[dim];

	for(i=0;i<dim*dim;i++){myEqns[i]=eqns[i];}
	for(i=0;i<dim;i++){
		myValues[i]=values[i];
		set[i]=0;
	}
	for(i=0;i<dim;i++){
		// Find the largest equation that has a non-zero entry in the i-th index
		m=-1;
		eIndex=-1;
		for(j=0;j<dim;j++){
			if(set[j]){continue;}
			if(myEqns[j*dim+i]!=0 && fabs(myEqns[j*dim+i])>m){
				m=fabs(myEqns[j*dim+i]);
				eIndex=j;
			}
		}
		if(eIndex==-1){
			delete[] index;
			delete[] myValues;
			delete[] myEqns;
			delete[] set;
			return 0;
		}
		// The position in which the solution for the i-th variable can be found
		index[i]=eIndex;
		set[eIndex]=1;

		// Normalize the equation
		v=myEqns[eIndex*dim+i];
		for(j=0;j<dim;j++){myEqns[eIndex*dim+j]/=v;}
		myValues[eIndex]/=v;

		// Subtract it off from everything else
		for(j=0;j<dim;j++){
			if(j==eIndex){continue;}
			double vv=myEqns[j*dim+i];
			for(int k=0;k<dim;k++){myEqns[j*dim+k]-=myEqns[eIndex*dim+k]*vv;}
			myValues[j]-=myValues[eIndex]*vv;
		}
	}
	for(i=0;i<dim;i++){solutions[i]=myValues[index[i]];}
	delete[] index;
	delete[] myValues;
	delete[] myEqns;
	delete[] set;
	return 1;
}




///////////////////
// CoredMeshData //
///////////////////
const int CoredMeshData::IN_CORE_FLAG[]={1,2,4};

TriangulationEdge::TriangulationEdge(void){pIndex[0]=pIndex[1]=tIndex[0]=tIndex[1]=-1;}
TriangulationTriangle::TriangulationTriangle(void){EIndex[0]=EIndex[1]=EIndex[2]=-1;}

/////////////////////////
// CoredVectorMeshData //
/////////////////////////
CoredVectorMeshData::CoredVectorMeshData(void){oocPointIndex=triangleIndex=0;}
void CoredVectorMeshData::ResetIterator(void){oocPointIndex=triangleIndex=0;}
int CoredVectorMeshData::AddOutOfCorePoint(const Point3D<float>& p){
	oocPoints.push_back(p);
	return int(oocPoints.size())-1;
}
int CoredVectorMeshData::AddTriangle(const TriangleIndex& t,const int& coreFlag){
	TriangleIndex tt;
	if(coreFlag & CoredMeshData::IN_CORE_FLAG[0])	{tt.idx[0]= t.idx[0];}
	else											{tt.idx[0]=-t.idx[0]-1;}
	if(coreFlag & CoredMeshData::IN_CORE_FLAG[1])	{tt.idx[1]= t.idx[1];}
	else											{tt.idx[1]=-t.idx[1]-1;}
	if(coreFlag & CoredMeshData::IN_CORE_FLAG[2])	{tt.idx[2]= t.idx[2];}
	else											{tt.idx[2]=-t.idx[2]-1;}
	triangles.push_back(tt);
	return int(triangles.size())-1;
}
int CoredVectorMeshData::NextOutOfCorePoint(Point3D<float>& p){
	if(oocPointIndex<int(oocPoints.size())){
		p=oocPoints[oocPointIndex++];
		return 1;
	}
	else{return 0;}
}
int CoredVectorMeshData::NextTriangle(TriangleIndex& t,int& inCoreFlag){
	inCoreFlag=0;
	if(triangleIndex<int(triangles.size())){
		t=triangles[triangleIndex++];
		if(t.idx[0]<0)	{t.idx[0]=-t.idx[0]-1;}
		else			{inCoreFlag|=CoredMeshData::IN_CORE_FLAG[0];}
		if(t.idx[1]<0)	{t.idx[1]=-t.idx[1]-1;}
		else			{inCoreFlag|=CoredMeshData::IN_CORE_FLAG[1];}
		if(t.idx[2]<0)	{t.idx[2]=-t.idx[2]-1;}
		else			{inCoreFlag|=CoredMeshData::IN_CORE_FLAG[2];}
		return 1;
	}
	else{return 0;}
}
int CoredVectorMeshData::OutOfCorePointCount(void){return int(oocPoints.size());}
int CoredVectorMeshData::TriangleCount(void){return int(triangles.size());}


////////////
// Square //
////////////
int Square::CornerIndex(const int& x,const int& y){return (y<<1)|x;}
void Square::FactorCornerIndex(const int& idx,int& x,int& y){
	x=(idx>>0)%2;
	y=(idx>>1)%2;
}
int Square::EdgeIndex(const int& orientation,const int& i){
	switch(orientation){
		case 0: // x
			if(!i)	{return  0;} // (0,0) -> (1,0)
			else	{return  2;} // (0,1) -> (1,1)
		case 1: // y
			if(!i)	{return  3;} // (0,0) -> (0,1)
			else	{return  1;} // (1,0) -> (1,1)
	};
	return -1;
}
void Square::FactorEdgeIndex(const int& idx,int& orientation,int& i){
	switch(idx){
		case 0: case 2:
			orientation=0;
			i=idx/2;
			return;
		case 1: case 3:
			orientation=1;
			i=((idx/2)+1)%2;
			return;
	};
}
void Square::EdgeCorners(const int& idx,int& c1,int& c2){
	int orientation,i;
	FactorEdgeIndex(idx,orientation,i);
	switch(orientation){
		case 0:
			c1=CornerIndex(0,i);
			c2=CornerIndex(1,i);
			break;
		case 1:
			c1=CornerIndex(i,0);
			c2=CornerIndex(i,1);
			break;
	};
}
int Square::ReflectEdgeIndex(const int& idx,const int& edgeIndex){
	int orientation=edgeIndex%2;
	int o,i;
	FactorEdgeIndex(idx,o,i);
	if(o!=orientation){return idx;}
	else{return EdgeIndex(o,(i+1)%2);}
}
int Square::ReflectCornerIndex(const int& idx,const int& edgeIndex){
	int orientation=edgeIndex%2;
	int x,y;
	FactorCornerIndex(idx,x,y);
	switch(orientation){
		case 0:	return CornerIndex((x+1)%2,y);
		case 1:	return CornerIndex(x,(y+1)%2);
	};
	return -1;
}



//////////
// Cube //
//////////
int Cube::CornerIndex(const int& x,const int& y,const int& z){return (z<<2)|(y<<1)|x;}
void Cube::FactorCornerIndex(const int& idx,int& x,int& y,int& z){
	x=(idx>>0)%2;
	y=(idx>>1)%2;
	z=(idx>>2)%2;
}
int Cube::EdgeIndex(const int& orientation,const int& i,const int& j){return (i | (j<<1))|(orientation<<2);}
void Cube::FactorEdgeIndex(const int& idx,int& orientation,int& i,int &j){
	orientation=idx>>2;
	i=idx&1;
	j=(idx&2)>>1;
}
int Cube::FaceIndex(const int& x,const int& y,const int& z){
	if		(x<0)	{return  0;}
	else if	(x>0)	{return  1;}
	else if	(y<0)	{return  2;}
	else if	(y>0)	{return  3;}
	else if	(z<0)	{return  4;}
	else if	(z>0)	{return  5;}
	else			{return -1;}
}
int Cube::FaceIndex(const int& dir,const int& offSet){return (dir<<1)|offSet;}

void Cube::FactorFaceIndex(const int& idx,int& x,int& y,int& z){
	x=y=z=0;
	switch(idx){
		case 0:		x=-1;	break;
		case 1:		x= 1;	break;
		case 2:		y=-1;	break;
		case 3:		y= 1;	break;
		case 4:		z=-1;	break;
		case 5:		z= 1;	break;
	};
}
void Cube::FactorFaceIndex(const int& idx,int& dir,int& offSet){
	dir  = idx>>1;
	offSet=idx &1;
}

int Cube::FaceAdjacentToEdges(const int& eIndex1,const int& eIndex2){
	int f1,f2,g1,g2;
	FacesAdjacentToEdge(eIndex1,f1,f2);
	FacesAdjacentToEdge(eIndex2,g1,g2);
	if(f1==g1 || f1==g2){return f1;}
	if(f2==g1 || f2==g2){return f2;}
	return -1;
}

void Cube::FacesAdjacentToEdge(const int& eIndex,int& f1Index,int& f2Index){
	int orientation,i1,i2;
	FactorEdgeIndex(eIndex,orientation,i1,i2);
	i1<<=1;
	i2<<=1;
	i1--;
	i2--;
	switch(orientation){
		case 0:
			f1Index=FaceIndex( 0,i1, 0);
			f2Index=FaceIndex( 0, 0,i2);
			break;
		case 1:
			f1Index=FaceIndex(i1, 0, 0);
			f2Index=FaceIndex( 0, 0,i2);
			break;
		case 2:
			f1Index=FaceIndex(i1, 0, 0);
			f2Index=FaceIndex( 0,i2, 0);
			break;
	};
}
void Cube::EdgeCorners(const int& idx,int& c1,int& c2){
	int orientation,i1,i2;
	FactorEdgeIndex(idx,orientation,i1,i2);
	switch(orientation){
		case 0:
			c1=CornerIndex(0,i1,i2);
			c2=CornerIndex(1,i1,i2);
			break;
		case 1:
			c1=CornerIndex(i1,0,i2);
			c2=CornerIndex(i1,1,i2);
			break;
		case 2:
			c1=CornerIndex(i1,i2,0);
			c2=CornerIndex(i1,i2,1);
			break;
	};
}
void Cube::FaceCorners(const int& idx,int& c1,int& c2,int& c3,int& c4){
	int i=idx%2;
	switch(idx/2){
	case 0:
		c1=CornerIndex(i,0,0);
		c2=CornerIndex(i,1,0);
		c3=CornerIndex(i,0,1);
		c4=CornerIndex(i,1,1);
		return;
	case 1:
		c1=CornerIndex(0,i,0);
		c2=CornerIndex(1,i,0);
		c3=CornerIndex(0,i,1);
		c4=CornerIndex(1,i,1);
		return;
	case 2:
		c1=CornerIndex(0,0,i);
		c2=CornerIndex(1,0,i);
		c3=CornerIndex(0,1,i);
		c4=CornerIndex(1,1,i);
		return;
	}
}
int Cube::AntipodalCornerIndex(const int& idx){
	int x,y,z;
	FactorCornerIndex(idx,x,y,z);
	return CornerIndex((x+1)%2,(y+1)%2,(z+1)%2);
}
int Cube::FaceReflectFaceIndex(const int& idx,const int& faceIndex){
	if(idx/2!=faceIndex/2){return idx;}
	else{
		if(idx%2)	{return idx-1;}
		else		{return idx+1;}
	}
}
int Cube::FaceReflectEdgeIndex(const int& idx,const int& faceIndex){
	int orientation=faceIndex/2;
	int o,i,j;
	FactorEdgeIndex(idx,o,i,j);
	if(o==orientation){return idx;}
	switch(orientation){
		case 0:	return EdgeIndex(o,(i+1)%2,j);
		case 1:
			switch(o){
				case 0:	return EdgeIndex(o,(i+1)%2,j);
				case 2:	return EdgeIndex(o,i,(j+1)%2);
			};
		case 2:	return EdgeIndex(o,i,(j+1)%2);
	};
	return -1;
}
int Cube::FaceReflectCornerIndex(const int& idx,const int& faceIndex){
	int orientation=faceIndex/2;
	int x,y,z;
	FactorCornerIndex(idx,x,y,z);
	switch(orientation){
		case 0:	return CornerIndex((x+1)%2,y,z);
		case 1:	return CornerIndex(x,(y+1)%2,z);
		case 2: return CornerIndex(x,y,(z+1)%2);
	};
	return -1;
}
int Cube::EdgeReflectCornerIndex(const int& idx,const int& edgeIndex){
	int orientation,x,y,z;
	FactorEdgeIndex(edgeIndex,orientation,x,y);
	FactorCornerIndex(idx,x,y,z);
	switch(orientation){
		case 0:	return CornerIndex( x     ,(y+1)%2,(z+1)%2);
		case 1:	return CornerIndex((x+1)%2, y     ,(z+1)%2);
		case 2:	return CornerIndex((x+1)%2,(y+1)%2, z     );
	};
	return -1;
}
int	Cube::EdgeReflectEdgeIndex(const int& edgeIndex){
	int o,i1,i2;
	FactorEdgeIndex(edgeIndex,o,i1,i2);
	return Cube::EdgeIndex(o,(i1+1)%2,(i2+1)%2);
}


/////////////////////
// MarchingSquares //
/////////////////////
/*
0} // (0,0) -> (1,0)
1} // (1,0) -> (1,1)
2} // (0,1) -> (1,1)
3} // (0,0) -> (0,1)
*/
const int MarchingSquares::edgeMask[1<<Square::CORNERS]={
	    0, //  0 ->         ->                         ->
	    9, //  1 -> 0       -> (0,0)                   -> 0,3     ->  9 
	    3, //  2 -> 1       -> (1,0)                   -> 0,1     ->  3
	   10, //  3 -> 0,1     -> (0,0) (1,0)             -> 1,3     -> 10
	   12, //  4 -> 2       -> (0,1)                   -> 2,3     -> 12
	    5, //  5 -> 0,2     -> (0,0) (0,1)             -> 0,2     ->  5
	   15, //  6 -> 1,2     -> (1,0) (0,1)             -> 0,1,2,3 -> 15
	    6, //  7 -> 0,1,2   -> (0,0) (1,0) (0,1)       -> 1,2     ->  6
	    6, //  8 -> 3       -> (1,1)                   -> 1,2     ->  6
	   15, //  9 -> 0,3     -> (0,0) (1,1)             -> 0,1,2,3 -> 15 
	    5, // 10 -> 1,3     -> (1,0) (1,1)             -> 0,2     ->  5
	   12, // 11 -> 0,1,3   -> (0,0) (1,0) (1,1)       -> 2,3     -> 12
	   10, // 12 -> 2,3     -> (0,1) (1,1)             -> 1,3     -> 10
	    3, // 13 -> 0,2,3   -> (0,0) (0,1) (1,1)       -> 0,1     ->  3
	    9, // 14 -> 1,2,3   -> (1,0) (0,1) (1,1)       -> 0,3     ->  9
	    0, // 15 -> 0,1,2,3 -> (0,0) (1,0) (0,1) (1,1) -> 
};
const int MarchingSquares::edges[1<<Square::CORNERS][MAX_EDGES*2+1] = {
	{ -1,  -1,  -1,  -1,  -1}, //
	{  3,   0,  -1,  -1,  -1}, // (0,0)
	{  0,   1,  -1,  -1,  -1}, // (1,0)
	{  3,   1,  -1,  -1,  -1}, // (0,0) (1,0)
	{  2,   3,  -1,  -1,  -1}, // (0,1)
	{  2,   0,  -1,  -1,  -1}, // (0,0) (0,1)
	{  0,   1,   2,   3,  -1}, // (1,0) (0,1)
	{  1,   2,  -1,  -1,  -1}, // (0,0) (1,0) (0,1)
	{  2,   1,  -1,  -1,  -1}, // (1,1)
	{  3,   0,   1,   2,  -1}, // (0,0) (1,1)
	{  0,   2,  -1,  -1,  -1}, // (1,0) (1,1)
	{  3,   2,  -1,  -1,  -1}, // (0,0) (1,0) (1,1)
	{  1,   3,  -1,  -1,  -1}, // (0,1) (1,1)
	{  1,   0,  -1,  -1,  -1}, // (0,0) (0,1) (1,1)
	{  0,   3,  -1,  -1,  -1}, // (1,0) (0,1) (1,1)
	{ -1,  -1,  -1,  -1,  -1}, // (0,0) (1,0) (0,1) (1,1)
};

double MarchingSquares::vertexList[Square::EDGES][2];
int MarchingSquares::GetIndex(const double v[Square::CORNERS],const double& iso){
	int idx=0;
	for(int i=0;i<Square::CORNERS;i++){if(v[i]<iso){idx|=(1<<i);}}
	return idx;
}

int MarchingSquares::IsAmbiguous(const double v[Square::CORNERS],const double& isoValue){
	int idx=GetIndex(v,isoValue);
	return (idx==5) || (idx==10);
}
int MarchingSquares::AddEdges(const double v[Square::CORNERS],const double& iso,Edge* isoEdges){
	int idx,nEdges=0;
	Edge e;

	idx=GetIndex(v,iso);

	/* Cube is entirely in/out of the surface */
	if (!edgeMask[idx]) return 0;

	/* Find the vertices where the surface intersects the cube */
	int i,j,ii=1;
	for(i=0;i<12;i++){
		if(edgeMask[idx] & ii){SetVertex(i,v,iso);}
		ii<<=1;
	}
	/* Create the triangle */
	for (i=0;edges[idx][i]!=-1;i+=2) {
		for(j=0;j<2;j++){
			e.p[0][j]=vertexList[edges[idx][i+0]][j];
			e.p[1][j]=vertexList[edges[idx][i+1]][j];
		}
		isoEdges[nEdges++]=e;
	}
	return nEdges;
}

int MarchingSquares::AddEdgeIndices(const double v[Square::CORNERS],const double& iso,int* isoIndices){
	int idx,nEdges=0;

	idx=GetIndex(v,iso);

	/* Cube is entirely in/out of the surface */
	if (!edgeMask[idx]) return 0;

	/* Create the triangle */
	for(int i=0;edges[idx][i]!=-1;i+=2){
		for(int j=0;j<2;j++){isoIndices[i+j]=edges[idx][i+j];}
		nEdges++;
	}
	return nEdges;
}
void MarchingSquares::SetVertex(const int& e,const double values[Square::CORNERS],const double& iso){
	int o,i,c1,c2;
	Square::FactorEdgeIndex(e,o,i);
	Square::EdgeCorners(e,c1,c2);
	switch(o){
		case 0:
			vertexList[e][0]=Interpolate(values[c1]-iso,values[c2]-iso);
			vertexList[e][1]=i;
			break;
		case 1:
			vertexList[e][1]=Interpolate(values[c1]-iso,values[c2]-iso);
			vertexList[e][0]=i;
			break;
	}
}
double MarchingSquares::Interpolate(const double& v1,const double& v2){return v1/(v1-v2);}


///////////////////
// MarchingCubes //
///////////////////
const int MarchingCubes::edgeMask[1<<Cube::CORNERS]={
	    0,  273,  545,  816, 2082, 2355, 2563, 2834,
	 1042, 1283, 1587, 1826, 3120, 3361, 3601, 3840,
	  324,   85,  869,  628, 2406, 2167, 2887, 2646,
	 1366, 1095, 1911, 1638, 3444, 3173, 3925, 3652,
	  644,  917,  165,  436, 2726, 2999, 2183, 2454,
	 1686, 1927, 1207, 1446, 3764, 4005, 3221, 3460,
	  960,  721,  481,  240, 3042, 2803, 2499, 2258,
	 2002, 1731, 1523, 1250, 4080, 3809, 3537, 3264,
	 2184, 2457, 2729, 3000,  170,  443,  651,  922,
	 3226, 3467, 3771, 4010, 1208, 1449, 1689, 1928,
	 2508, 2269, 3053, 2812,  494,  255,  975,  734,
	 3550, 3279, 4095, 3822, 1532, 1261, 2013, 1740,
	 2572, 2845, 2093, 2364,  558,  831,   15,  286,
	 3614, 3855, 3135, 3374, 1596, 1837, 1053, 1292,
	 2888, 2649, 2409, 2168,  874,  635,  331,   90,
	 3930, 3659, 3451, 3178, 1912, 1641, 1369, 1096,
	 1096, 1369, 1641, 1912, 3178, 3451, 3659, 3930,
	   90,  331,  635,  874, 2168, 2409, 2649, 2888,
	 1292, 1053, 1837, 1596, 3374, 3135, 3855, 3614,
	  286,   15,  831,  558, 2364, 2093, 2845, 2572,
	 1740, 2013, 1261, 1532, 3822, 4095, 3279, 3550,
	  734,  975,  255,  494, 2812, 3053, 2269, 2508,
	 1928, 1689, 1449, 1208, 4010, 3771, 3467, 3226,
	  922,  651,  443,  170, 3000, 2729, 2457, 2184,
	 3264, 3537, 3809, 4080, 1250, 1523, 1731, 2002,
	 2258, 2499, 2803, 3042,  240,  481,  721,  960,
	 3460, 3221, 4005, 3764, 1446, 1207, 1927, 1686,
	 2454, 2183, 2999, 2726,  436,  165,  917,  644,
	 3652, 3925, 3173, 3444, 1638, 1911, 1095, 1366,
	 2646, 2887, 2167, 2406,  628,  869,   85,  324,
	 3840, 3601, 3361, 3120, 1826, 1587, 1283, 1042,
	 2834, 2563, 2355, 2082,  816,  545,  273,    0
};
const int MarchingCubes::triangles[1<<Cube::CORNERS][MAX_TRIANGLES*3+1] = {
	{  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   0,   4,   8,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   5,   0,   9,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   8,   9,   5,   8,   5,   4,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   1,   5,  11,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   0,   4,   8,   1,   5,  11,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   9,  11,   1,   9,   1,   0,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   8,   9,  11,   8,  11,   1,   8,   1,   4,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   4,   1,  10,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{  10,   8,   0,  10,   0,   1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   5,   0,   9,   4,   1,  10,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{  10,   8,   9,  10,   9,   5,  10,   5,   1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{  11,  10,   4,  11,   4,   5,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{  11,  10,   8,  11,   8,   0,  11,   0,   5,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   9,  11,  10,   9,  10,   4,   9,   4,   0,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   8,   9,  11,   8,  11,  10,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   8,   6,   2,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   6,   2,   0,   4,   6,   0,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   6,   2,   8,   5,   0,   9,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   5,   4,   6,   9,   5,   6,   2,   9,   6,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   1,   5,  11,   8,   6,   2,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   1,   5,  11,   6,   2,   0,   4,   6,   0,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   6,   2,   8,   9,  11,   1,   9,   1,   0,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   9,  11,   2,   2,  11,   1,   2,   1,   6,   6,   1,   4,  -1,  -1,  -1,  -1},
	{   1,  10,   4,   2,   8,   6,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   2,   0,   1,   6,   2,   1,  10,   6,   1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   5,   0,   9,   4,   1,  10,   8,   6,   2,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   5,   2,   9,   5,   6,   2,   5,   1,   6,   1,  10,   6,  -1,  -1,  -1,  -1},
	{   2,   8,   6,   4,   5,  11,   4,  11,  10,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   5,   2,   0,   6,   2,   5,  11,   6,   5,  10,   6,  11,  -1,  -1,  -1,  -1},
	{   9,  11,  10,   9,  10,   4,   9,   4,   0,   8,   6,   2,  -1,  -1,  -1,  -1},
	{   9,  11,   2,   2,  11,   6,  10,   6,  11,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   9,   2,   7,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   7,   9,   2,   4,   8,   0,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   0,   2,   7,   0,   7,   5,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   7,   5,   4,   2,   7,   4,   8,   2,   4,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   7,   9,   2,   5,  11,   1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   1,   5,  11,   0,   4,   8,   9,   2,   7,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   1,   0,   2,   1,   2,   7,   1,   7,  11,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   1,   7,  11,   1,   2,   7,   1,   4,   2,   4,   8,   2,  -1,  -1,  -1,  -1},
	{   4,   1,  10,   9,   2,   7,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   7,   9,   2,   0,   1,  10,   0,  10,   8,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   4,   1,  10,   2,   7,   5,   0,   2,   5,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   2,  10,   8,   1,  10,   2,   7,   1,   2,   5,   1,   7,  -1,  -1,  -1,  -1},
	{   7,   9,   2,  10,   4,   5,  11,  10,   5,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{  11,  10,   8,  11,   8,   0,  11,   0,   5,   9,   2,   7,  -1,  -1,  -1,  -1},
	{  11,  10,   7,   7,  10,   4,   7,   4,   2,   2,   4,   0,  -1,  -1,  -1,  -1},
	{  11,  10,   7,   7,  10,   2,   8,   2,  10,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   7,   9,   8,   6,   7,   8,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   4,   6,   7,   0,   4,   7,   9,   0,   7,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   6,   7,   5,   8,   6,   5,   0,   8,   5,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   4,   6,   7,   5,   4,   7,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   5,  11,   1,   8,   6,   7,   9,   8,   7,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   4,   6,   7,   0,   4,   7,   9,   0,   7,  11,   1,   5,  -1,  -1,  -1,  -1},
	{   8,   1,   0,  11,   1,   8,   6,  11,   8,   7,  11,   6,  -1,  -1,  -1,  -1},
	{  11,   6,   7,   1,   6,  11,   6,   1,   4,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   1,  10,   4,   6,   7,   9,   6,   9,   8,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   0,   1,   9,   9,   1,  10,   9,  10,   7,   7,  10,   6,  -1,  -1,  -1,  -1},
	{   6,   7,   5,   8,   6,   5,   0,   8,   5,   1,  10,   4,  -1,  -1,  -1,  -1},
	{   1,   7,   5,  10,   7,   1,   7,  10,   6,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{  11,  10,   4,  11,   4,   5,   7,   9,   8,   6,   7,   8,  -1,  -1,  -1,  -1},
	{   0,   6,   9,   9,   6,   7,   6,   0,   5,   5,  11,  10,   5,  10,   6,  -1},
	{   8,   7,   0,   6,   7,   8,   4,   0,   7,  11,  10,   4,   7,  11,   4,  -1},
	{  11,  10,   6,  11,   6,   7,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{  11,   7,   3,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   0,   4,   8,  11,   7,   3,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   9,   5,   0,  11,   7,   3,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{  11,   7,   3,   4,   8,   9,   5,   4,   9,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   3,   1,   5,   3,   5,   7,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   0,   4,   8,   7,   3,   1,   5,   7,   1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   3,   1,   0,   3,   0,   9,   3,   9,   7,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   7,   8,   9,   4,   8,   7,   3,   4,   7,   1,   4,   3,  -1,  -1,  -1,  -1},
	{   1,  10,   4,   3,  11,   7,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   3,  11,   7,   8,   0,   1,  10,   8,   1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   4,   1,  10,   5,   0,   9,  11,   7,   3,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{  10,   8,   9,  10,   9,   5,  10,   5,   1,  11,   7,   3,  -1,  -1,  -1,  -1},
	{   4,   5,   7,   4,   7,   3,   4,   3,  10,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{  10,   8,   3,   3,   8,   0,   3,   0,   7,   7,   0,   5,  -1,  -1,  -1,  -1},
	{   4,   3,  10,   4,   7,   3,   4,   0,   7,   0,   9,   7,  -1,  -1,  -1,  -1},
	{  10,   8,   3,   3,   8,   7,   9,   7,   8,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{  11,   7,   3,   8,   6,   2,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{  11,   7,   3,   2,   0,   4,   2,   4,   6,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{  11,   7,   3,   8,   6,   2,   5,   0,   9,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   5,   4,   6,   9,   5,   6,   2,   9,   6,   3,  11,   7,  -1,  -1,  -1,  -1},
	{   8,   6,   2,   3,   1,   5,   3,   5,   7,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   3,   1,   5,   3,   5,   7,   6,   2,   0,   4,   6,   0,  -1,  -1,  -1,  -1},
	{   3,   1,   0,   3,   0,   9,   3,   9,   7,   2,   8,   6,  -1,  -1,  -1,  -1},
	{   9,   4,   2,   2,   4,   6,   4,   9,   7,   7,   3,   1,   7,   1,   4,  -1},
	{   8,   6,   2,  11,   7,   3,   4,   1,  10,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   2,   0,   1,   6,   2,   1,  10,   6,   1,  11,   7,   3,  -1,  -1,  -1,  -1},
	{   5,   0,   9,   4,   1,  10,   8,   6,   2,  11,   7,   3,  -1,  -1,  -1,  -1},
	{  11,   7,   3,   5,   2,   9,   5,   6,   2,   5,   1,   6,   1,  10,   6,  -1},
	{   4,   5,   7,   4,   7,   3,   4,   3,  10,   6,   2,   8,  -1,  -1,  -1,  -1},
	{  10,   5,   3,   3,   5,   7,   5,  10,   6,   6,   2,   0,   6,   0,   5,  -1},
	{   8,   6,   2,   4,   3,  10,   4,   7,   3,   4,   0,   7,   0,   9,   7,  -1},
	{   9,   7,  10,  10,   7,   3,  10,   6,   9,   6,   2,   9,  -1,  -1,  -1,  -1},
	{   3,  11,   9,   2,   3,   9,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   4,   8,   0,   2,   3,  11,   2,  11,   9,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   0,   2,   3,   0,   3,  11,   0,  11,   5,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   2,   3,   8,   8,   3,  11,   8,  11,   4,   4,  11,   5,  -1,  -1,  -1,  -1},
	{   2,   3,   1,   2,   1,   5,   2,   5,   9,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   2,   3,   1,   2,   1,   5,   2,   5,   9,   0,   4,   8,  -1,  -1,  -1,  -1},
	{   0,   2,   3,   0,   3,   1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   2,   3,   8,   8,   3,   4,   1,   4,   3,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   1,  10,   4,   9,   2,   3,  11,   9,   3,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{  10,   8,   0,  10,   0,   1,   3,  11,   9,   2,   3,   9,  -1,  -1,  -1,  -1},
	{   0,   2,   3,   0,   3,  11,   0,  11,   5,   1,  10,   4,  -1,  -1,  -1,  -1},
	{   5,   2,  11,  11,   2,   3,   2,   5,   1,   1,  10,   8,   1,   8,   2,  -1},
	{  10,   2,   3,   9,   2,  10,   4,   9,  10,   5,   9,   4,  -1,  -1,  -1,  -1},
	{   5,  10,   0,   0,  10,   8,  10,   5,   9,   9,   2,   3,   9,   3,  10,  -1},
	{   0,   2,   4,   4,   2,  10,   3,  10,   2,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{  10,   8,   2,  10,   2,   3,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{  11,   9,   8,   3,  11,   8,   6,   3,   8,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   0,  11,   9,   3,  11,   0,   4,   3,   0,   6,   3,   4,  -1,  -1,  -1,  -1},
	{  11,   5,   3,   5,   0,   3,   0,   6,   3,   0,   8,   6,  -1,  -1,  -1,  -1},
	{   3,   4,   6,  11,   4,   3,   4,  11,   5,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   3,   1,   6,   6,   1,   5,   6,   5,   8,   8,   5,   9,  -1,  -1,  -1,  -1},
	{   0,   6,   9,   4,   6,   0,   5,   9,   6,   3,   1,   5,   6,   3,   5,  -1},
	{   3,   1,   6,   6,   1,   8,   0,   8,   1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   3,   1,   4,   3,   4,   6,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{  11,   9,   8,   3,  11,   8,   6,   3,   8,   4,   1,  10,  -1,  -1,  -1,  -1},
	{   3,   9,   6,  11,   9,   3,  10,   6,   9,   0,   1,  10,   9,   0,  10,  -1},
	{   4,   1,  10,  11,   5,   3,   5,   0,   3,   0,   6,   3,   0,   8,   6,  -1},
	{   5,  10,   6,   1,  10,   5,   6,  11,   5,   6,   3,  11,  -1,  -1,  -1,  -1},
	{  10,   5,   3,   4,   5,  10,   6,   3,   5,   9,   8,   6,   5,   9,   6,  -1},
	{   6,   3,  10,   9,   0,   5,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   3,  10,   0,   0,  10,   4,   0,   8,   3,   8,   6,   3,  -1,  -1,  -1,  -1},
	{   6,   3,  10,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{  10,   3,   6,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   3,   6,  10,   0,   4,   8,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   5,   0,   9,  10,   3,   6,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   3,   6,  10,   8,   9,   5,   8,   5,   4,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{  11,   1,   5,  10,   3,   6,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   0,   4,   8,   1,   5,  11,  10,   3,   6,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{  10,   3,   6,   0,   9,  11,   1,   0,  11,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   8,   9,  11,   8,  11,   1,   8,   1,   4,  10,   3,   6,  -1,  -1,  -1,  -1},
	{   4,   1,   3,   6,   4,   3,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   0,   1,   3,   8,   0,   3,   6,   8,   3,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   5,   0,   9,   3,   6,   4,   1,   3,   4,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   8,   9,   6,   6,   9,   5,   6,   5,   3,   3,   5,   1,  -1,  -1,  -1,  -1},
	{   6,   4,   5,   6,   5,  11,   6,  11,   3,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   0,   6,   8,   0,   3,   6,   0,   5,   3,   5,  11,   3,  -1,  -1,  -1,  -1},
	{   3,   9,  11,   0,   9,   3,   6,   0,   3,   4,   0,   6,  -1,  -1,  -1,  -1},
	{   8,   9,   6,   6,   9,   3,  11,   3,   9,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   2,   8,  10,   3,   2,  10,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   3,   2,   0,  10,   3,   0,   4,  10,   0,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   5,   0,   9,   8,  10,   3,   8,   3,   2,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   9,   3,   2,  10,   3,   9,   5,  10,   9,   4,  10,   5,  -1,  -1,  -1,  -1},
	{  11,   1,   5,   2,   8,  10,   3,   2,  10,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   3,   2,   0,  10,   3,   0,   4,  10,   0,   5,  11,   1,  -1,  -1,  -1,  -1},
	{   9,  11,   1,   9,   1,   0,   2,   8,  10,   3,   2,  10,  -1,  -1,  -1,  -1},
	{  10,   2,   4,   3,   2,  10,   1,   4,   2,   9,  11,   1,   2,   9,   1,  -1},
	{   1,   3,   2,   4,   1,   2,   8,   4,   2,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   0,   1,   3,   2,   0,   3,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   1,   3,   2,   4,   1,   2,   8,   4,   2,   9,   5,   0,  -1,  -1,  -1,  -1},
	{   9,   3,   2,   5,   3,   9,   3,   5,   1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   3,   2,  11,  11,   2,   8,  11,   8,   5,   5,   8,   4,  -1,  -1,  -1,  -1},
	{   5,   2,   0,  11,   2,   5,   2,  11,   3,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   4,   3,   8,   8,   3,   2,   3,   4,   0,   0,   9,  11,   0,  11,   3,  -1},
	{   9,  11,   3,   9,   3,   2,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{  10,   3,   6,   9,   2,   7,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   9,   2,   7,  10,   3,   6,   0,   4,   8,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{  10,   3,   6,   7,   5,   0,   7,   0,   2,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   7,   5,   4,   2,   7,   4,   8,   2,   4,  10,   3,   6,  -1,  -1,  -1,  -1},
	{  10,   3,   6,   9,   2,   7,   1,   5,  11,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{  10,   3,   6,   9,   2,   7,   1,   5,  11,   0,   4,   8,  -1,  -1,  -1,  -1},
	{   1,   0,   2,   1,   2,   7,   1,   7,  11,   3,   6,  10,  -1,  -1,  -1,  -1},
	{  10,   3,   6,   1,   7,  11,   1,   2,   7,   1,   4,   2,   4,   8,   2,  -1},
	{   9,   2,   7,   6,   4,   1,   6,   1,   3,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   0,   1,   3,   8,   0,   3,   6,   8,   3,   7,   9,   2,  -1,  -1,  -1,  -1},
	{   0,   2,   7,   0,   7,   5,   4,   1,   3,   6,   4,   3,  -1,  -1,  -1,  -1},
	{   2,   5,   8,   7,   5,   2,   6,   8,   5,   1,   3,   6,   5,   1,   6,  -1},
	{   6,   4,   5,   6,   5,  11,   6,  11,   3,   7,   9,   2,  -1,  -1,  -1,  -1},
	{   9,   2,   7,   0,   6,   8,   0,   3,   6,   0,   5,   3,   5,  11,   3,  -1},
	{   3,   4,  11,   6,   4,   3,   7,  11,   4,   0,   2,   7,   4,   0,   7,  -1},
	{  11,   3,   8,   8,   3,   6,   8,   2,  11,   2,   7,  11,  -1,  -1,  -1,  -1},
	{   9,   8,  10,   7,   9,  10,   3,   7,  10,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   9,   0,   7,   0,   4,   7,   4,   3,   7,   4,  10,   3,  -1,  -1,  -1,  -1},
	{   8,  10,   0,   0,  10,   3,   0,   3,   5,   5,   3,   7,  -1,  -1,  -1,  -1},
	{  10,   5,   4,   3,   5,  10,   5,   3,   7,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   9,   8,  10,   7,   9,  10,   3,   7,  10,   1,   5,  11,  -1,  -1,  -1,  -1},
	{   1,   5,  11,   9,   0,   7,   0,   4,   7,   4,   3,   7,   4,  10,   3,  -1},
	{  11,   0,   7,   1,   0,  11,   3,   7,   0,   8,  10,   3,   0,   8,   3,  -1},
	{   7,   1,   4,  11,   1,   7,   4,   3,   7,   4,  10,   3,  -1,  -1,  -1,  -1},
	{   4,   9,   8,   7,   9,   4,   1,   7,   4,   3,   7,   1,  -1,  -1,  -1,  -1},
	{   7,   1,   3,   9,   1,   7,   1,   9,   0,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   8,   7,   0,   0,   7,   5,   7,   8,   4,   4,   1,   3,   4,   3,   7,  -1},
	{   5,   1,   3,   7,   5,   3,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   3,   4,  11,  11,   4,   5,   4,   3,   7,   7,   9,   8,   7,   8,   4,  -1},
	{   3,   9,   0,   7,   9,   3,   0,  11,   3,   0,   5,  11,  -1,  -1,  -1,  -1},
	{   3,   7,  11,   8,   4,   0,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   3,   7,  11,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   6,  10,  11,   7,   6,  11,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   0,   4,   8,  10,  11,   7,  10,   7,   6,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   9,   5,   0,   6,  10,  11,   7,   6,  11,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   8,   9,   5,   8,   5,   4,   6,  10,  11,   7,   6,  11,  -1,  -1,  -1,  -1},
	{   5,   7,   6,   5,   6,  10,   5,  10,   1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   5,   7,   6,   5,   6,  10,   5,  10,   1,   4,   8,   0,  -1,  -1,  -1,  -1},
	{   1,   0,  10,  10,   0,   9,  10,   9,   6,   6,   9,   7,  -1,  -1,  -1,  -1},
	{   1,   7,  10,  10,   7,   6,   7,   1,   4,   4,   8,   9,   4,   9,   7,  -1},
	{   7,   6,   4,   7,   4,   1,   7,   1,  11,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{  11,   0,   1,   8,   0,  11,   7,   8,  11,   6,   8,   7,  -1,  -1,  -1,  -1},
	{   7,   6,   4,   7,   4,   1,   7,   1,  11,   5,   0,   9,  -1,  -1,  -1,  -1},
	{  11,   6,   1,   7,   6,  11,   5,   1,   6,   8,   9,   5,   6,   8,   5,  -1},
	{   4,   5,   7,   4,   7,   6,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   5,   7,   0,   0,   7,   8,   6,   8,   7,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   7,   6,   9,   9,   6,   0,   4,   0,   6,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   8,   9,   7,   8,   7,   6,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   8,  10,  11,   2,   8,  11,   7,   2,  11,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{  10,  11,   4,   4,  11,   7,   4,   7,   0,   0,   7,   2,  -1,  -1,  -1,  -1},
	{   8,  10,  11,   2,   8,  11,   7,   2,  11,   5,   0,   9,  -1,  -1,  -1,  -1},
	{   9,   4,   2,   5,   4,   9,   7,   2,   4,  10,  11,   7,   4,  10,   7,  -1},
	{   1,   8,  10,   2,   8,   1,   5,   2,   1,   7,   2,   5,  -1,  -1,  -1,  -1},
	{   1,   7,  10,   5,   7,   1,   4,  10,   7,   2,   0,   4,   7,   2,   4,  -1},
	{   7,   1,   9,   9,   1,   0,   1,   7,   2,   2,   8,  10,   2,  10,   1,  -1},
	{   7,   2,   9,  10,   1,   4,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   8,   4,   2,   4,   1,   2,   1,   7,   2,   1,  11,   7,  -1,  -1,  -1,  -1},
	{  11,   0,   1,   7,   0,  11,   0,   7,   2,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   5,   0,   9,   8,   4,   2,   4,   1,   2,   1,   7,   2,   1,  11,   7,  -1},
	{   2,   5,   1,   9,   5,   2,   1,   7,   2,   1,  11,   7,  -1,  -1,  -1,  -1},
	{   4,   5,   8,   8,   5,   2,   7,   2,   5,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   7,   2,   0,   5,   7,   0,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   7,   2,   4,   4,   2,   8,   4,   0,   7,   0,   9,   7,  -1,  -1,  -1,  -1},
	{   7,   2,   9,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{  10,  11,   9,   6,  10,   9,   2,   6,   9,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{  10,  11,   9,   6,  10,   9,   2,   6,   9,   0,   4,   8,  -1,  -1,  -1,  -1},
	{   5,  10,  11,   6,  10,   5,   0,   6,   5,   2,   6,   0,  -1,  -1,  -1,  -1},
	{   2,   5,   8,   8,   5,   4,   5,   2,   6,   6,  10,  11,   6,  11,   5,  -1},
	{  10,   1,   6,   1,   5,   6,   5,   2,   6,   5,   9,   2,  -1,  -1,  -1,  -1},
	{   0,   4,   8,  10,   1,   6,   1,   5,   6,   5,   2,   6,   5,   9,   2,  -1},
	{   1,   0,  10,  10,   0,   6,   2,   6,   0,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   2,   6,   1,   1,   6,  10,   1,   4,   2,   4,   8,   2,  -1,  -1,  -1,  -1},
	{  11,   9,   1,   1,   9,   2,   1,   2,   4,   4,   2,   6,  -1,  -1,  -1,  -1},
	{   8,   1,   6,   0,   1,   8,   2,   6,   1,  11,   9,   2,   1,  11,   2,  -1},
	{  11,   6,   1,   1,   6,   4,   6,  11,   5,   5,   0,   2,   5,   2,   6,  -1},
	{   2,   6,   8,  11,   5,   1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   6,   4,   2,   2,   4,   9,   5,   9,   4,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   5,   9,   6,   6,   9,   2,   6,   8,   5,   8,   0,   5,  -1,  -1,  -1,  -1},
	{   0,   2,   6,   0,   6,   4,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   2,   6,   8,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   8,  10,  11,   9,   8,  11,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   0,  11,   9,   4,  11,   0,  11,   4,  10,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   5,  10,  11,   0,  10,   5,  10,   0,   8,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   4,  10,  11,   5,   4,  11,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   1,   8,  10,   5,   8,   1,   8,   5,   9,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   9,   4,  10,   0,   4,   9,  10,   5,   9,  10,   1,   5,  -1,  -1,  -1,  -1},
	{   0,   8,  10,   1,   0,  10,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{  10,   1,   4,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   4,   9,   8,   1,   9,   4,   9,   1,  11,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   1,  11,   9,   0,   1,   9,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{  11,   0,   8,   5,   0,  11,   8,   1,  11,   8,   4,   1,  -1,  -1,  -1,  -1},
	{  11,   5,   1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   5,   9,   8,   4,   5,   8,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   9,   0,   5,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{   8,   4,   0,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
	{  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1}
};
const int MarchingCubes::cornerMap[Cube::CORNERS]={0,1,3,2,4,5,7,6};
double MarchingCubes::vertexList[Cube::EDGES][3];

int MarchingCubes::GetIndex(const double v[Cube::CORNERS],const double& iso){
	int idx=0;
	if (v[Cube::CornerIndex(0,0,0)] < iso) idx |=   1;
	if (v[Cube::CornerIndex(1,0,0)] < iso) idx |=   2;
	if (v[Cube::CornerIndex(1,1,0)] < iso) idx |=   4;
	if (v[Cube::CornerIndex(0,1,0)] < iso) idx |=   8;
	if (v[Cube::CornerIndex(0,0,1)] < iso) idx |=  16;
	if (v[Cube::CornerIndex(1,0,1)] < iso) idx |=  32;
	if (v[Cube::CornerIndex(1,1,1)] < iso) idx |=  64;
	if (v[Cube::CornerIndex(0,1,1)] < iso) idx |= 128;
	return idx;
}
int MarchingCubes::GetFaceIndex(const double values[Cube::CORNERS],const double& iso,const int& faceIndex){
	int i,j,x,y,z,idx=0;
	double v[2][2];
	Cube::FactorFaceIndex(faceIndex,x,y,z);
	if		(x<0){for(i=0;i<2;i++){for(j=0;j<2;j++){v[i][j]=values[Cube::CornerIndex(0,i,j)];}}}
	else if	(x>0){for(i=0;i<2;i++){for(j=0;j<2;j++){v[i][j]=values[Cube::CornerIndex(1,i,j)];}}}
	else if	(y<0){for(i=0;i<2;i++){for(j=0;j<2;j++){v[i][j]=values[Cube::CornerIndex(i,0,j)];}}}
	else if	(y>0){for(i=0;i<2;i++){for(j=0;j<2;j++){v[i][j]=values[Cube::CornerIndex(i,1,j)];}}}
	else if	(z<0){for(i=0;i<2;i++){for(j=0;j<2;j++){v[i][j]=values[Cube::CornerIndex(i,j,0)];}}}
	else if	(z>0){for(i=0;i<2;i++){for(j=0;j<2;j++){v[i][j]=values[Cube::CornerIndex(i,j,1)];}}}
	if (v[0][0] < iso) idx |=   1;
	if (v[1][0] < iso) idx |=   2;
	if (v[1][1] < iso) idx |=   4;
	if (v[0][1] < iso) idx |=   8;
	return idx;
}
int MarchingCubes::IsAmbiguous(const double v[Cube::CORNERS],const double& isoValue,const int& faceIndex){
	int idx=GetFaceIndex(v,isoValue,faceIndex);
	return (idx==5) || (idx==10);
}
int MarchingCubes::HasRoots(const double v[Cube::CORNERS],const double& isoValue,const int& faceIndex){
	int idx=GetFaceIndex(v,isoValue,faceIndex);
	return (idx!=0) && (idx !=15);
}
int MarchingCubes::HasRoots(const double v[Cube::CORNERS],const double& isoValue){
	int idx=GetIndex(v,isoValue);
	if(idx==0 || idx==255){return 0;}
	else{return 1;}
}
int MarchingCubes::HasRoots(const int& mcIndex){
	if(mcIndex==0 || mcIndex==255){return 0;}
	else{return 1;}
}
int MarchingCubes::AddTriangles(const double v[Cube::CORNERS],const double& iso,Triangle* isoTriangles){
	int idx,ntriang=0;
	Triangle tri;

	idx=GetIndex(v,iso);

	/* Cube is entirely in/out of the surface */
	if (!edgeMask[idx]) return 0;

	/* Find the vertices where the surface intersects the cube */
	int i,j,ii=1;
	for(i=0;i<12;i++){
		if(edgeMask[idx] & ii){SetVertex(i,v,iso);}
		ii<<=1;
	}
	/* Create the triangle */
	for (i=0;triangles[idx][i]!=-1;i+=3) {
		for(j=0;j<3;j++){
			tri.p[0][j]=vertexList[triangles[idx][i+0]][j];
			tri.p[1][j]=vertexList[triangles[idx][i+1]][j];
			tri.p[2][j]=vertexList[triangles[idx][i+2]][j];
		}
		isoTriangles[ntriang++]=tri;
	}
	return ntriang;
}

int MarchingCubes::AddTriangleIndices(const double v[Cube::CORNERS],const double& iso,int* isoIndices){
	int idx,ntriang=0;

	idx=GetIndex(v,iso);

	/* Cube is entirely in/out of the surface */
	if (!edgeMask[idx]) return 0;

	/* Create the triangle */
	for(int i=0;triangles[idx][i]!=-1;i+=3){
		for(int j=0;j<3;j++){isoIndices[i+j]=triangles[idx][i+j];}
		ntriang++;
	}
	return ntriang;
}

void MarchingCubes::SetVertex(const int& e,const double values[Cube::CORNERS],const double& iso){
	double t;
	switch(e){
		case 0:
			t=Interpolate(values[Cube::CornerIndex(0,0,0)]-iso,values[Cube::CornerIndex(1,0,0)]-iso);
			vertexList[e][0]=t;			vertexList[e][1]=0.0;		vertexList[e][2]=0.0;
			break;
		case 1:
			t=Interpolate(values[Cube::CornerIndex(1,0,0)]-iso,values[Cube::CornerIndex(1,1,0)]-iso);
			vertexList[e][0]=1.0;		vertexList[e][1]=t;			vertexList[e][2]=0.0;
			break;
		case 2:
			t=Interpolate(values[Cube::CornerIndex(1,1,0)]-iso,values[Cube::CornerIndex(0,1,0)]-iso);
			vertexList[e][0]=(1.0-t);	vertexList[e][1]=1.0;		vertexList[e][2]=0.0;
			break;
		case 3:
			t=Interpolate(values[Cube::CornerIndex(0,1,0)]-iso,values[Cube::CornerIndex(0,0,0)]-iso);
			vertexList[e][0]=0.0;		vertexList[e][1]=(1.0-t);	vertexList[e][2]=0.0;
			break;
		case 4:
			t=Interpolate(values[Cube::CornerIndex(0,0,1)]-iso,values[Cube::CornerIndex(1,0,1)]-iso);
			vertexList[e][0]=t;			vertexList[e][1]=0.0;		vertexList[e][2]=1.0;
			break;
		case 5:
			t=Interpolate(values[Cube::CornerIndex(1,0,1)]-iso,values[Cube::CornerIndex(1,1,1)]-iso);
			vertexList[e][0]=1.0;		vertexList[e][1]=t;			vertexList[e][2]=1.0;
			break;
		case 6:
			t=Interpolate(values[Cube::CornerIndex(1,1,1)]-iso,values[Cube::CornerIndex(0,1,1)]-iso);
			vertexList[e][0]=(1.0-t);	vertexList[e][1]=1.0;		vertexList[e][2]=1.0;
			break;
		case 7:
			t=Interpolate(values[Cube::CornerIndex(0,1,1)]-iso,values[Cube::CornerIndex(0,0,1)]-iso);
			vertexList[e][0]=0.0;		vertexList[e][1]=(1.0-t);	vertexList[e][2]=1.0;
			break;
		case 8:
			t=Interpolate(values[Cube::CornerIndex(0,0,0)]-iso,values[Cube::CornerIndex(0,0,1)]-iso);
			vertexList[e][0]=0.0;		vertexList[e][1]=0.0;		vertexList[e][2]=t;
			break;
		case 9:
			t=Interpolate(values[Cube::CornerIndex(1,0,0)]-iso,values[Cube::CornerIndex(1,0,1)]-iso);
			vertexList[e][0]=1.0;		vertexList[e][1]=0.0;		vertexList[e][2]=t;
			break;
		case 10:
			t=Interpolate(values[Cube::CornerIndex(1,1,0)]-iso,values[Cube::CornerIndex(1,1,1)]-iso);
			vertexList[e][0]=1.0;		vertexList[e][1]=1.0;		vertexList[e][2]=t;
			break;
		case 11:
			t=Interpolate(values[Cube::CornerIndex(0,1,0)]-iso,values[Cube::CornerIndex(0,1,1)]-iso);
			vertexList[e][0]=0.0;		vertexList[e][1]=1.0;		vertexList[e][2]=t;
			break;
	};
}
double MarchingCubes::Interpolate(const double& v1,const double& v2){return v1/(v1-v2);}


///////////////////////////////////
int MarchingCubes::GetIndex(const float v[Cube::CORNERS],const float& iso){
	int idx=0;
	if (v[Cube::CornerIndex(0,0,0)] < iso) idx |=   1;
	if (v[Cube::CornerIndex(1,0,0)] < iso) idx |=   2;
	if (v[Cube::CornerIndex(1,1,0)] < iso) idx |=   4;
	if (v[Cube::CornerIndex(0,1,0)] < iso) idx |=   8;
	if (v[Cube::CornerIndex(0,0,1)] < iso) idx |=  16;
	if (v[Cube::CornerIndex(1,0,1)] < iso) idx |=  32;
	if (v[Cube::CornerIndex(1,1,1)] < iso) idx |=  64;
	if (v[Cube::CornerIndex(0,1,1)] < iso) idx |= 128;
	return idx;
}
int MarchingCubes::GetFaceIndex(const float values[Cube::CORNERS],const float& iso,const int& faceIndex){
	int i,j,x,y,z,idx=0;
	double v[2][2];
	Cube::FactorFaceIndex(faceIndex,x,y,z);
	if		(x<0){for(i=0;i<2;i++){for(j=0;j<2;j++){v[i][j]=values[Cube::CornerIndex(0,i,j)];}}}
	else if	(x>0){for(i=0;i<2;i++){for(j=0;j<2;j++){v[i][j]=values[Cube::CornerIndex(1,i,j)];}}}
	else if	(y<0){for(i=0;i<2;i++){for(j=0;j<2;j++){v[i][j]=values[Cube::CornerIndex(i,0,j)];}}}
	else if	(y>0){for(i=0;i<2;i++){for(j=0;j<2;j++){v[i][j]=values[Cube::CornerIndex(i,1,j)];}}}
	else if	(z<0){for(i=0;i<2;i++){for(j=0;j<2;j++){v[i][j]=values[Cube::CornerIndex(i,j,0)];}}}
	else if	(z>0){for(i=0;i<2;i++){for(j=0;j<2;j++){v[i][j]=values[Cube::CornerIndex(i,j,1)];}}}
	if (v[0][0] < iso) idx |=   1;
	if (v[1][0] < iso) idx |=   2;
	if (v[1][1] < iso) idx |=   4;
	if (v[0][1] < iso) idx |=   8;
	return idx;
}
int MarchingCubes::GetFaceIndex(const int& mcIndex,const int& faceIndex){
	int i,j,x,y,z,idx=0;
	int v[2][2];
	Cube::FactorFaceIndex(faceIndex,x,y,z);
	if		(x<0){for(i=0;i<2;i++){for(j=0;j<2;j++){v[i][j]=mcIndex&(1<<MarchingCubes::cornerMap[Cube::CornerIndex(0,i,j)]);}}}
	else if	(x>0){for(i=0;i<2;i++){for(j=0;j<2;j++){v[i][j]=mcIndex&(1<<MarchingCubes::cornerMap[Cube::CornerIndex(1,i,j)]);}}}
	else if	(y<0){for(i=0;i<2;i++){for(j=0;j<2;j++){v[i][j]=mcIndex&(1<<MarchingCubes::cornerMap[Cube::CornerIndex(i,0,j)]);}}}
	else if	(y>0){for(i=0;i<2;i++){for(j=0;j<2;j++){v[i][j]=mcIndex&(1<<MarchingCubes::cornerMap[Cube::CornerIndex(i,1,j)]);}}}
	else if	(z<0){for(i=0;i<2;i++){for(j=0;j<2;j++){v[i][j]=mcIndex&(1<<MarchingCubes::cornerMap[Cube::CornerIndex(i,j,1)]);}}}
	else if	(z>0){for(i=0;i<2;i++){for(j=0;j<2;j++){v[i][j]=mcIndex&(1<<MarchingCubes::cornerMap[Cube::CornerIndex(i,j,1)]);}}}
	if (v[0][0]) idx |=   1;
	if (v[1][0]) idx |=   2;
	if (v[1][1]) idx |=   4;
	if (v[0][1]) idx |=   8;
	return idx;
}
int MarchingCubes::IsAmbiguous(const float v[Cube::CORNERS],const float& isoValue,const int& faceIndex){
	int idx=GetFaceIndex(v,isoValue,faceIndex);
	return (idx==5) || (idx==10);
}
int MarchingCubes::IsAmbiguous(const int& mcIndex,const int& faceIndex){
	int idx=GetFaceIndex(mcIndex,faceIndex);
	return (idx==5) || (idx==10);
}
int MarchingCubes::HasRoots(const float v[Cube::CORNERS],const float& isoValue){
	int idx=GetIndex(v,isoValue);
	if(idx==0 || idx==255){return 0;}
	else{return 1;}
}
int MarchingCubes::HasRoots(const float v[Cube::CORNERS],const float& isoValue,const int& faceIndex){
	int idx=GetFaceIndex(v,isoValue,faceIndex);
	return (idx!=0) && (idx!=15);
}
int MarchingCubes::HasFaceRoots(const int& mcIndex,const int& faceIndex){
	int idx=GetFaceIndex(mcIndex,faceIndex);
	return (idx!=0) && (idx!=15);
}
int MarchingCubes::HasEdgeRoots(const int& mcIndex,const int& edgeIndex){
	int c1,c2;
	Cube::EdgeCorners(edgeIndex,c1,c2);
	if(	( (mcIndex&(1<<MarchingCubes::cornerMap[c1])) &&  (mcIndex&(1<<MarchingCubes::cornerMap[c2]))) ||
		(!(mcIndex&(1<<MarchingCubes::cornerMap[c1])) && !(mcIndex&(1<<MarchingCubes::cornerMap[c2])))){return 0;}
	else{return 1;}
}
int MarchingCubes::AddTriangles(const float v[Cube::CORNERS],const float& iso,Triangle* isoTriangles){
	int idx,ntriang=0;
	Triangle tri;

	idx=GetIndex(v,iso);

	/* Cube is entirely in/out of the surface */
	if (!edgeMask[idx]) return 0;

	/* Find the vertices where the surface intersects the cube */
	int i,j,ii=1;
	for(i=0;i<12;i++){
		if(edgeMask[idx] & ii){SetVertex(i,v,iso);}
		ii<<=1;
	}
	/* Create the triangle */
	for (i=0;triangles[idx][i]!=-1;i+=3) {
		for(j=0;j<3;j++){
			tri.p[0][j]=vertexList[triangles[idx][i+0]][j];
			tri.p[1][j]=vertexList[triangles[idx][i+1]][j];
			tri.p[2][j]=vertexList[triangles[idx][i+2]][j];
		}
		isoTriangles[ntriang++]=tri;
	}
	return ntriang;
}

int MarchingCubes::AddTriangleIndices(const float v[Cube::CORNERS],const float& iso,int* isoIndices){
	int idx,ntriang=0;
	idx=GetIndex(v,iso);
	/* Cube is entirely in/out of the surface */
	if (!edgeMask[idx]) return 0;
	/* Create the triangle */
	for(int i=0;triangles[idx][i]!=-1;i+=3){
		for(int j=0;j<3;j++){isoIndices[i+j]=triangles[idx][i+j];}
		ntriang++;
	}
	return ntriang;
}
int MarchingCubes::AddTriangleIndices(const int& idx,int* isoIndices){
	int ntriang=0;

	/* Cube is entirely in/out of the surface */
	if (!edgeMask[idx]) return 0;

	/* Create the triangle */
	for(int i=0;triangles[idx][i]!=-1;i+=3){
		for(int j=0;j<3;j++){isoIndices[i+j]=triangles[idx][i+j];}
		ntriang++;
	}
	return ntriang;
}

void MarchingCubes::SetVertex(const int& e,const float values[Cube::CORNERS],const float& iso){
	double t;
	switch(e){
		case 0:
			t=Interpolate(values[Cube::CornerIndex(0,0,0)]-iso,values[Cube::CornerIndex(1,0,0)]-iso);
			vertexList[e][0]=t;			vertexList[e][1]=0.0;		vertexList[e][2]=0.0;
			break;
		case 1:
			t=Interpolate(values[Cube::CornerIndex(1,0,0)]-iso,values[Cube::CornerIndex(1,1,0)]-iso);
			vertexList[e][0]=1.0;		vertexList[e][1]=t;			vertexList[e][2]=0.0;
			break;
		case 2:
			t=Interpolate(values[Cube::CornerIndex(1,1,0)]-iso,values[Cube::CornerIndex(0,1,0)]-iso);
			vertexList[e][0]=(1.0-t);	vertexList[e][1]=1.0;		vertexList[e][2]=0.0;
			break;
		case 3:
			t=Interpolate(values[Cube::CornerIndex(0,1,0)]-iso,values[Cube::CornerIndex(0,0,0)]-iso);
			vertexList[e][0]=0.0;		vertexList[e][1]=(1.0-t);	vertexList[e][2]=0.0;
			break;
		case 4:
			t=Interpolate(values[Cube::CornerIndex(0,0,1)]-iso,values[Cube::CornerIndex(1,0,1)]-iso);
			vertexList[e][0]=t;			vertexList[e][1]=0.0;		vertexList[e][2]=1.0;
			break;
		case 5:
			t=Interpolate(values[Cube::CornerIndex(1,0,1)]-iso,values[Cube::CornerIndex(1,1,1)]-iso);
			vertexList[e][0]=1.0;		vertexList[e][1]=t;			vertexList[e][2]=1.0;
			break;
		case 6:
			t=Interpolate(values[Cube::CornerIndex(1,1,1)]-iso,values[Cube::CornerIndex(0,1,1)]-iso);
			vertexList[e][0]=(1.0-t);	vertexList[e][1]=1.0;		vertexList[e][2]=1.0;
			break;
		case 7:
			t=Interpolate(values[Cube::CornerIndex(0,1,1)]-iso,values[Cube::CornerIndex(0,0,1)]-iso);
			vertexList[e][0]=0.0;		vertexList[e][1]=(1.0-t);	vertexList[e][2]=1.0;
			break;
		case 8:
			t=Interpolate(values[Cube::CornerIndex(0,0,0)]-iso,values[Cube::CornerIndex(0,0,1)]-iso);
			vertexList[e][0]=0.0;		vertexList[e][1]=0.0;		vertexList[e][2]=t;
			break;
		case 9:
			t=Interpolate(values[Cube::CornerIndex(1,0,0)]-iso,values[Cube::CornerIndex(1,0,1)]-iso);
			vertexList[e][0]=1.0;		vertexList[e][1]=0.0;		vertexList[e][2]=t;
			break;
		case 10:
			t=Interpolate(values[Cube::CornerIndex(1,1,0)]-iso,values[Cube::CornerIndex(1,1,1)]-iso);
			vertexList[e][0]=1.0;		vertexList[e][1]=1.0;		vertexList[e][2]=t;
			break;
		case 11:
			t=Interpolate(values[Cube::CornerIndex(0,1,0)]-iso,values[Cube::CornerIndex(0,1,1)]-iso);
			vertexList[e][0]=0.0;		vertexList[e][1]=1.0;		vertexList[e][2]=t;
			break;
	};
}
float MarchingCubes::Interpolate(const float& v1,const float& v2){return v1/(v1-v2);}


template<int Degree>
int Execute()
{
  TreeNodeData::UseIndex = 1;

	int i=0;

  int Binary=0,Verbose=0,NoResetSamples=0,NoClipTree=0,Confidence=0;
  int Depth=7,SolverDivide=8,IsoDivide=8,Refine=3;
  int KernelDepth=0;
  float SamplesPerNode=1.0f,Scale=1.25f;

  Point3D<float> center;
	Real scale=1.0;
	Real isoValue=0;
	Octree<Degree> tree;
	PPolynomial<Degree> ReconstructionFunction=PPolynomial<Degree>::GaussianApproximation();

	center.coords[0]=center.coords[1]=center.coords[2]=0;
	
	TreeOctNode::SetAllocator(MEMORY_ALLOCATOR_BLOCK_SIZE);

	int kernelDepth=Depth-2;

	tree.setFunctionData(ReconstructionFunction,Depth,0,Real(1.0)/(1<<Depth));

	tree.setTree(Depth,kernelDepth,Real(SamplesPerNode),Scale,center,scale,!NoResetSamples,Confidence);

	tree.ClipTree();

	tree.finalize1(Refine);
	tree.maxMemoryUsage=0;
	tree.SetLaplacianWeights();

  tree.finalize2(Refine);

	tree.maxMemoryUsage=0;
	tree.LaplacianMatrixIteration(SolverDivide);

	CoredVectorMeshData mesh;
	tree.maxMemoryUsage=0;
	isoValue=tree.GetIsoValue();

  tree.GetMCIsoTriangles(isoValue,IsoDivide,&mesh);

  //////////////////////////////////////////////////////////////////////////////////////
  //output the reconstructed mesh.
  int nr_vertices=int(mesh.OutOfCorePointCount()+mesh.inCorePoints.size());
  int nr_faces=mesh.TriangleCount();

  mesh.ResetIterator();
	Point3D<float> p;
	float ply_vertex[3];
	for (i=0; i < (int)(mesh.inCorePoints.size()); i++){
		p=mesh.inCorePoints[i];
		ply_vertex[0] = p.coords[0]*scale+center.coords[0];
		ply_vertex[1] = p.coords[1]*scale+center.coords[1];
		ply_vertex[2] = p.coords[2]*scale+center.coords[2];
    vtk_psr_output->GetPoints()->InsertNextPoint(ply_vertex);
	}
	for (i=0; i < mesh.OutOfCorePointCount(); i++){
		mesh.NextOutOfCorePoint(p);
		ply_vertex[0] = p.coords[0]*scale+center.coords[0];
		ply_vertex[1] = p.coords[1]*scale+center.coords[1];
		ply_vertex[2] = p.coords[2]*scale+center.coords[2];
    vtk_psr_output->GetPoints()->InsertNextPoint(ply_vertex);
	}  // for, write vertices
	
	// write faces
	TriangleIndex tIndex;
	int inCoreFlag;

  vtkIdType ply_face[3];

	for (i=0; i < nr_faces; i++){
		//
		// create and fill a struct that the ply code can handle
		//
    mesh.NextTriangle(tIndex,inCoreFlag);
		if(!(inCoreFlag & CoredMeshData::IN_CORE_FLAG[0])){tIndex.idx[0]+=int(mesh.inCorePoints.size());}
		if(!(inCoreFlag & CoredMeshData::IN_CORE_FLAG[1])){tIndex.idx[1]+=int(mesh.inCorePoints.size());}
		if(!(inCoreFlag & CoredMeshData::IN_CORE_FLAG[2])){tIndex.idx[2]+=int(mesh.inCorePoints.size());}
	
    for(int j=0; j<3; j++)  ply_face[j] = tIndex.idx[j];
    
    vtk_psr_output->GetPolys()->InsertNextCell(3,ply_face);

	}  // for, write faces

	return 1;
}

void PSR_main()
{
  Execute<2>();
}