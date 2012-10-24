#include "itkDeformableSimplexMesh3DGradientConstraintForceFilter.h"
#include "itkNumericTraits.h"
#include "vnl/vnl_math.h"

#include <set>

namespace itk
{
/* Constructore  */
template< typename TInputMesh, typename TOutputMesh >
DeformableSimplexMeshFilter001< TInputMesh, TOutputMesh >
::DeformableSimplexMeshFilter001()
{
  m_Range = 1;
  distanceCoefficient = 1;
  m_CurIterationNum = 0;
//  distanceFlag = true;
  closetPoint = NULL;

}

template< typename TInputMesh, typename TOutputMesh >
DeformableSimplexMeshFilter001< TInputMesh, TOutputMesh >
::~DeformableSimplexMeshFilter001()
{
	//delete[] closetPoint;
	delete[] StrutNeighbors;
	//delete distance;

}

template< typename TInputMesh, typename TOutputMesh >
void
DeformableSimplexMeshFilter001< TInputMesh, TOutputMesh >
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Range = " << m_Range << std::endl;
}

template< typename TInputMesh, typename TOutputMesh >
void DeformableSimplexMeshFilter001< TInputMesh, TOutputMesh >
::SetStrutLinkIter(StrutIterator StrutStart, StrutIterator StrutEnd,
	                    StrutIterator LinkStart, StrutIterator LinkEnd){

 int VertexNumber = this->GetInput(0)->GetPoints()->size();
 StrutNeighbors = new int[VertexNumber][3];
 for(int i=0;i<VertexNumber;i++){
	StrutNeighbors[i][0] = -1;
	StrutNeighbors[i][1] = -1;
	StrutNeighbors[i][2] = -1;
 }
  // [0],[1] strut neighbor; [2] link neighbor
 for(StrutIterator iter = StrutStart; iter !=StrutEnd; iter++){
	if(StrutNeighbors[iter->startVertex][0] == -1)
		StrutNeighbors[iter->startVertex][0] = iter->endVertex;
	else 
		if(StrutNeighbors[iter->startVertex][0]!= iter->endVertex
			&&StrutNeighbors[iter->startVertex][1] == -1)
			StrutNeighbors[iter->startVertex][1] = iter->endVertex;
	
	if(StrutNeighbors[iter->endVertex][0] == -1)
		StrutNeighbors[iter->endVertex][0] = iter->startVertex;
	else 
		if(StrutNeighbors[iter->endVertex][0]!= iter->startVertex
			&&StrutNeighbors[iter->endVertex][1] == -1)
			StrutNeighbors[iter->endVertex][1] = iter->startVertex;
 }
  // [0],[1] strut neighbor; [2] link neighbor
 for(StrutIterator iter = LinkStart; iter !=LinkEnd; iter++){
	if(StrutNeighbors[iter->startVertex][2] == -1)
		StrutNeighbors[iter->startVertex][2] = iter->endVertex;
	if(StrutNeighbors[iter->endVertex][2] == -1)
		StrutNeighbors[iter->endVertex][2] = iter->startVertex;
 } 
 }

/*template< typename TInputMesh, typename TOutputMesh >
void DeformableSimplexMeshFilter001< TInputMesh, TOutputMesh >
::SetVesselPointsKDTree(PointIterator PointStart, PointIterator PointEnd){
	KDTree = kd_create(3);
	for(PointIterator iter = PointStart; iter != PointEnd; iter++){
		assert(kd_insert3(KDTree,iter->x,iter->y,iter->z,0)==0);
	}
}
*/
template< typename TInputMesh, typename TOutputMesh >
void
DeformableSimplexMeshFilter001< TInputMesh, TOutputMesh >
::Initialize(){
  Superclass::Initialize(); 
}

template< typename TInputMesh, typename TOutputMesh >
void
DeformableSimplexMeshFilter001< TInputMesh, TOutputMesh >
::ComputeDisplacement(){
  const InputMeshType *inputMesh = this->GetInput(0);

  // Filters should not modify their input...
  // There is a design flaw here.
  InputPointsContainer *nonConstPoints =
    const_cast< InputPointsContainer * >( inputMesh->GetPoints() );

  typename GeometryMapType::Iterator dataIt = this->m_Data->Begin();
  SimplexMeshGeometry *data;
  VectorType           displacement;
  int i = 0;

  VectorType StrutLengthForce;
  while ( dataIt != this->m_Data->End() )
    {
    data = dataIt.Value();

	
    this->ComputeInternalForce(data);

    this->ComputeExternalForce(data);
	
	StrutLengthForce = this->ComputeStrutLengthForce(data,dataIt.Index());

	 displacement.Set_vnl_vector( m_Alpha * ( data->internalForce).Get_vnl_vector()
		                     + ( data->externalForce).Get_vnl_vector() 
							 +  StrutLengthForce.Get_vnl_vector());
	 data->pos+=displacement;
	nonConstPoints->InsertElement(dataIt.Index(), data->pos);
    dataIt++;
	i++;
    }
}

template< typename TInputMesh, typename TOutputMesh >
void
DeformableSimplexMeshFilter001< TInputMesh, TOutputMesh >
::ComputeExternalForce(SimplexMeshGeometry *data)
{
  double pos[3];
  pos[0] = data->pos[0];
  pos[1] = data->pos[1];
  pos[2] = data->pos[2];

  data->externalForce[0] = m_Beta * ( data->normal )[0];
  data->externalForce[1] = m_Beta * ( data->normal )[1];
  data->externalForce[2] = m_Beta * ( data->normal )[2];
}


template< typename TInputMesh, typename TOutputMesh >
itk::Vector< double, 3 > DeformableSimplexMeshFilter001< TInputMesh, TOutputMesh >
::ComputeStrutLengthForce(SimplexMeshGeometry *data, int index){

	VectorType lengthForce;
	lengthForce.Fill(0);
	PointType neighbor;
	double curLength;
	int c = 0;
	if(StrutNeighbors[index][0] != -1){
		c++;
		neighbor = this->m_Data->GetElement(StrutNeighbors[index][0])->pos;
		curLength= sqrt((data->pos[0]-neighbor[0])*(data->pos[0]-neighbor[0])
			     +(data->pos[1]-neighbor[1])*(data->pos[1]-neighbor[1])
				 +(data->pos[2]-neighbor[2])*(data->pos[2]-neighbor[2]));
		lengthForce[0] -= (curLength - m_StrutLength)*(data->pos[0] - neighbor[0])/curLength;
		lengthForce[1] -= (curLength - m_StrutLength)*(data->pos[1] - neighbor[1])/curLength;
		lengthForce[2] -= (curLength - m_StrutLength)*(data->pos[2] - neighbor[2])/curLength;
	}
	if(StrutNeighbors[index][1] != -1){
		c++;
		neighbor = this->m_Data->GetElement(StrutNeighbors[index][1])->pos;
		curLength= sqrt((data->pos[0]-neighbor[0])*(data->pos[0]-neighbor[0])
			     +(data->pos[1]-neighbor[1])*(data->pos[1]-neighbor[1])
				 +(data->pos[2]-neighbor[2])*(data->pos[2]-neighbor[2]));
		lengthForce[0] -= (curLength - m_StrutLength)*(data->pos[0] - neighbor[0])/curLength;
		lengthForce[1] -= (curLength - m_StrutLength)*(data->pos[1] - neighbor[1])/curLength;
		lengthForce[2] -= (curLength - m_StrutLength)*(data->pos[2] - neighbor[2])/curLength;
	}

	if(StrutNeighbors[index][2] != -1){
		c++;
		neighbor = this->m_Data->GetElement(StrutNeighbors[index][2])->pos;
		curLength= sqrt((data->pos[0]-neighbor[0])*(data->pos[0]-neighbor[0])
			     +(data->pos[1]-neighbor[1])*(data->pos[1]-neighbor[1])
				 +(data->pos[2]-neighbor[2])*(data->pos[2]-neighbor[2]));
		lengthForce[0] -= (curLength - m_LinkLength)*(data->pos[0] - neighbor[0])*1.2/curLength;
		lengthForce[1] -= (curLength - m_LinkLength)*(data->pos[1] - neighbor[1])*1.2/curLength;
		lengthForce[2] -= (curLength - m_LinkLength)*(data->pos[2] - neighbor[2])*1.2/curLength;
	}
	
	lengthForce[0] /= 1.2;
	lengthForce[1] /= 1.2;
	lengthForce[2] /= 1.2;

	return lengthForce;
}
}
