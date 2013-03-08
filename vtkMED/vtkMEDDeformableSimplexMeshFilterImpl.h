#include "itkDeformableSimplexMesh3DGradientConstraintForceFilter.h"
#include "itkNumericTraits.h"
#include "vnl/vnl_math.h"
#include <time.h>
#include "vtkDoubleArray.h"
#include "vtkCellData.h"
#include "vtkMath.h"
#include <set>




namespace itk
{
/* Constructor  */
template< typename TInputMesh, typename TOutputMesh >
DeformableSimplexMeshFilterImpl< TInputMesh, TOutputMesh >
::DeformableSimplexMeshFilterImpl()
{
  distanceCoefficient = 1;
  m_CurIterationNum = 0;
}
//----------------------------

/*destructor */
template< typename TInputMesh, typename TOutputMesh >
DeformableSimplexMeshFilterImpl< TInputMesh, TOutputMesh >
::~DeformableSimplexMeshFilterImpl()
{
	delete[] StrutNeighbors;
}
//---------------------
/* the difference from SetStrutLinkIter is parameter
* setting StrutNeighbors: [0],[1] strut neighbor; [2] link neighbor
* record for every simplex vertex
* prepare for calculating strut&link length force
* if the value equals -1, means not applicable
* here, if the simplex vertex serves as extremity of stent structure, 
      then it as least has the first two strut neighbor
* if not, no neighbors
* if this vertex is also connected to next crown with links, 
      then it has the third link neighbor
*/
template< typename TInputMesh, typename TOutputMesh >
void DeformableSimplexMeshFilterImpl< TInputMesh, TOutputMesh >
	::SetStrutLinkFromCellArray(vtkCellArray *strut,vtkCellArray *link){

		//initialization
		int VertexNumber = this->GetInput(0)->GetPoints()->size();
		StrutNeighbors = new int[VertexNumber][3];
		for(int i=0;i<VertexNumber;i++){
			StrutNeighbors[i][0] = -1;
			StrutNeighbors[i][1] = -1;
			StrutNeighbors[i][2] = -1;
		}
		
		
		//vtkIdType sNum = strut->GetNumberOfCells();
	    //vtkCell* pCell;

		vtkIdType npts = 0;
		vtkIdType *pts=0;

		vtkIdType start,end;


		/*loop strut first */
		//set strut neighbor: [0] & [1]
		for ( strut->InitTraversal(); strut->GetNextCell(npts,pts); ){

			start = pts[0];
			end = pts[1];
			if(StrutNeighbors[start][0] == -1)
				StrutNeighbors[start][0] = end;
			else 
				if(StrutNeighbors[start][0]!= end && StrutNeighbors[start][1] == -1)
					StrutNeighbors[start][1] = end;

			if(StrutNeighbors[end][0] == -1)
				StrutNeighbors[end][0] = start;
			else 
				if(StrutNeighbors[end][0]!= start && StrutNeighbors[end][1] == -1)
					StrutNeighbors[end][1] = start;

		}
		/*loop link next*/
		// set link neighbor: [2]
		for ( link->InitTraversal(); link->GetNextCell(npts,pts); ){
			start = pts[0];
			end = pts[1];

			if(StrutNeighbors[start][2] == -1)
				StrutNeighbors[start][2] = end;
			if(StrutNeighbors[end][2] == -1)
				StrutNeighbors[end][2] = start;		
		}
		
}

/*
* setting StrutNeighbors: [0],[1] strut neighbor; [2] link neighbor
* record for every simplex vertex
* prepare for calculating strut&link length force
* if the value equals -1, means not applicable
* here, if the simplex vertex serves as extremity of stent structure, 
      then it as least has the first two strut neighbor
* if not, no neighbors
* if this vertex is also connected to next crown with links, 
      then it has the third link neighbor
*/
template< typename TInputMesh, typename TOutputMesh >
void DeformableSimplexMeshFilterImpl< TInputMesh, TOutputMesh >
::SetStrutLinkIter(StrutIterator StrutStart, StrutIterator StrutEnd,
	                    StrutIterator LinkStart, StrutIterator LinkEnd){

	//initialization
	int VertexNumber = this->GetInput(0)->GetPoints()->size();
	StrutNeighbors = new int[VertexNumber][3];
	for(int i=0;i<VertexNumber;i++){
		StrutNeighbors[i][0] = -1;
		StrutNeighbors[i][1] = -1;
		StrutNeighbors[i][2] = -1;
	}

	// set strut neighbor: [0] & [1]
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
  // set link neighbor: [2]
 for(StrutIterator iter = LinkStart; iter !=LinkEnd; iter++){
	if(StrutNeighbors[iter->startVertex][2] == -1)
		StrutNeighbors[iter->startVertex][2] = iter->endVertex;
	if(StrutNeighbors[iter->endVertex][2] == -1)
		StrutNeighbors[iter->endVertex][2] = iter->startVertex;
 } 
 }
//-----------------------

/*
* import vertices of vessel wall , and create a KD-tree 
* for accuracy, we need the vessel wall to be represented with dense point cloud
* if not dense enough, sampling before creating the KD-tree
*/
/*
template< typename TInputMesh, typename TOutputMesh >
void DeformableSimplexMeshFilterImpl< TInputMesh, TOutputMesh >
::SetVesselPointsKDTree(PointIterator PointStart, PointIterator PointEnd){
	KDTree = kd_create(3);
	for(PointIterator iter = PointStart; iter != PointEnd; iter++){
		assert(kd_insert3(KDTree,iter->x,iter->y,iter->z,0)==0);
	}
}
*/
template< typename TInputMesh, typename TOutputMesh >
void DeformableSimplexMeshFilterImpl< TInputMesh, TOutputMesh >
	::SetVesselPointsKDTreeFromPolyData(vtkPolyData *surface){

		m_SurfacePoly = surface;
		vtkPoints *points = surface->GetPoints();
		

		
	
		vtkIdType num = surface->GetNumberOfPoints();
		
		double p[3];
		KDTree = kd_create(3);
		for(vtkIdType i=0;i<num;i++){
			points->GetPoint(i,p);
			assert(kd_insert3(KDTree,p[0],p[1],p[2],0)==0);
		}
}


//1 inside ,0,outside 
template< typename TInputMesh, typename TOutputMesh >
int DeformableSimplexMeshFilterImpl< TInputMesh, TOutputMesh >
	:: isPointInsideVessel(double *stentPoint, double *surfacePoint) 
{
	int rtnFlag = 1;//inside
	//----to have a test-
	double outPoint[3];
	//double aPoint[3];
	//m_SurfacePoly->GetPoint(0,aPoint); X                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
	vtkIdType surPointIndex = m_SurfacePoly->FindPoint(surfacePoint);
	vtkIdList *apCellIds = vtkIdList::New();
	vtkIdList *apNPointIds  = vtkIdList::New();
	m_SurfacePoly->GetPointCells (surPointIndex, apCellIds);
	vtkIdType cellNumber = apCellIds->GetNumberOfIds();

	vtkIdType cellId,pointId1,pointId2;
	vtkCell *aCell;
	vtkIdType pointNumber ;
	double flag; //flag>0 in surface or <0 outside
	for(int i=0;i<cellNumber;i++){
		cellId = apCellIds->GetId(i);
		aCell = m_SurfacePoly->GetCell(cellId);
		apNPointIds = aCell->GetPointIds();
		pointNumber = apNPointIds->GetNumberOfIds();
		if(pointNumber ==3){
			flag = computeDirectionFromPoint2Face(stentPoint,apNPointIds->GetId(0),apNPointIds->GetId(1),apNPointIds->GetId(2));
			if(flag>0){
				rtnFlag = -1;
				break;
			}
		}
	}

	//double n1[3],n2[3],point1Coord[3],point2Coord[3];
	//find cells of a point
	/*for(int i=0;i<cellNumber;i++){
		cellId = apCellIds->GetId(i);
		aCell = surface->GetCell(cellId);
		vtkIdaCell->GetPointIds(apNPointIds);
		pointNumber = apNPointIds->GetNumberOfIds();
		//find points of a cell
		for(int j=0;j<pointNumber-1;j++){
			pointId1 = apNPointIds->GetId(j);
			pointId2 = apNPointIds->GetId(j+1)
				//computeFacePointDistance(outPoint ,aPoint,pointId1,pointId2);
			m_SurfacePoly->GetPoint(pointId1,point1Coord);
			m_SurfacePoly->GetPoint(pointId2,point2Coord);
			//---------compute distance from stentPoint to face (aPoint,point1Coord, point2Coord);
		
		}
		//last cell
		pointId1 = apNPointIds->GetId(j);
		pointId2 = apNPointIds->GetId(0);

	}*/
		return rtnFlag;
	//------test finish-----
}
template< typename TInputMesh, typename TOutputMesh >
void DeformableSimplexMeshFilterImpl< TInputMesh, TOutputMesh >
	::vectorSubtract(double *endPoint,double *startPoint,double *result){
		result[0] = endPoint[0]-startPoint[0];
		result[1] = endPoint[1]-startPoint[1];
		result[2] = endPoint[2]-startPoint[2];

}

template< typename TInputMesh, typename TOutputMesh >
double DeformableSimplexMeshFilterImpl< TInputMesh, TOutputMesh >
	::computeDirectionFromPoint2Face(double *stentPoint,vtkIdType p1,vtkIdType p2,vtkIdType p3){
		double rtn = 0.0;
		
		double point1Coord[3],point2Coord[3],point3Coord[3],p1p2[3],p1p3[3],normal[3],p1Stent[3];
		m_SurfacePoly->GetPoint(p1,point1Coord);
		m_SurfacePoly->GetPoint(p2,point2Coord);
		m_SurfacePoly->GetPoint(p3,point3Coord);
		vectorSubtract(point2Coord, point1Coord, p1p2);
		vectorSubtract(point3Coord, point1Coord, p1p3);
		vtkMath::Cross(p1p2,p1p3,normal);
		vectorSubtract(stentPoint,point1Coord,p1Stent);
		rtn = vtkMath::Dot(normal,p1Stent);
		
		//-----------test------------
//		| N /p3
//		|  /
//	p1	| /_______ p2
//      \
//       \
//        \ sp
		double n[3],p1S[3];//*p1c,*p2c,*p3c,
		double p1c[3] = {0,0,0};
		double p2c[3] =  {2,0,0};
		double p3c[3] = {0,2,0};
		double sp[3] =  {2,0,-1};
		;
		vectorSubtract(p2c, p1c, p1p2);
		vectorSubtract(p3c, p1c, p1p3);
		vtkMath::Cross(p1p2,p1p3,n);
		vectorSubtract(stentPoint,p1c,p1S);
		double dot = vtkMath::Dot(n,p1S);

		return rtn;
}
//--------------------------

/*
* for every simplex vertex,
* centerLocationIdx stores the corresponding centerline position
*
* this information is recorded during the simplex mesh & stent model creation
* and imported here using setter
*/
template< typename TInputMesh, typename TOutputMesh >
void
DeformableSimplexMeshFilterImpl< TInputMesh, TOutputMesh >
::SetCenterLocationIdx(vector<int>::const_iterator centerLocationIndex){
	centerLocationIdx = centerLocationIndex;
}
template< typename TInputMesh, typename TOutputMesh >
void
	DeformableSimplexMeshFilterImpl< TInputMesh, TOutputMesh >
::SetCenterLocationIdxRef(vector<int> const&ve){
	centerLocationIdx = ve.begin();
}
//-------------------------

/*
* Override the ComputeDisplacement() function
*   1. Use KD tree to calculate the distance from the simplex vertex to the vessel wall
*        This distance is used to weight the external force and total dispalcement
*   2. Sysnchronize the catheter's pulling over with the expansion of a certain circle of simplex vertices
*/
template< typename TInputMesh, typename TOutputMesh >
void
DeformableSimplexMeshFilterImpl< TInputMesh, TOutputMesh >
::ComputeDisplacement(){
  //-------------timer--------
	 time_t t1,t2;
//-------------timer--------
	const InputMeshType *inputMesh = this->GetInput(0);

  // Filters should not modify their input...
  // There is a design flaw here.
  InputPointsContainer *nonConstPoints =
    const_cast< InputPointsContainer * >( inputMesh->GetPoints() );

  typename GeometryMapType::Iterator dataIt = this->m_Data->Begin();
  
  SimplexMeshGeometry *data;
  VectorType displacement;
  
  int i = 0;
  double pt[3]; //point on surface
  double pos[3];
  VectorType StrutLengthForce;
  VectorType LinkLengthForce;
  kdres *nearestPointSet;
  vector<int>::const_iterator centerIdx = centerLocationIdx;
  int catheterConstraint = 1;
  isDataChanged = 0;
  //-------------timer--------
  (void) time(&t1);
  
  //-------------timer--------
  //for every simplex vertex
  while ( dataIt != this->m_Data->End() )
  {
    data = dataIt.Value();

	/*Sysnchronize the catheter's pulling over with the expansion of a certain circle of simplex vertices
	*centerIdx indicates the centerline position of the current vertex
	*(*centerIdx)*x > m_CurIterationNum
	* here the x should be set as the same as the catheter's pulling speed
	* the speed we use is after x iteration the catheter's start position is located on the next center point
	*
	* ((*centerIdx)*x + y )< m_CurIterationNum)
	* here the number means for a vertex, 
	* a total of y iterations will be carried out on it since it start to expansion
	* as it is a progressive expansion, 
	* vertice on one side will reach the vessel wall far earlier that another side
	*
	* choose your own x and y parameters here
	*/


	if (((*centerIdx)*2+75)< m_CurIterationNum){//y:75 can not expend fully //my value:25 too fast//change parameter here
		dataIt++;
		centerIdx++;
		continue;
	}
	if((*centerIdx)*2 > m_CurIterationNum ){
		catheterConstraint = 0;
	}else{
		catheterConstraint = 1;
	}	
	
    //compute internal,external and length force
	this->ComputeInternalForce(data);
    this->ComputeExternalForce(data);	
	StrutLengthForce = this->ComputeStrutLengthForce(data,dataIt.Index());
	LinkLengthForce = this->ComputeLinkLengthForce(data,dataIt.Index());

	//Use KD tree to calculate the distance from the simplex vertex to the vessel wall
    //This distance is used to weight the external force and total dispalcement
	pos[0] = data->pos[0];
	pos[1] = data->pos[1];
	pos[2] = data->pos[2];
	nearestPointSet = kd_nearest3(KDTree, pos[0], pos[1], pos[2]);//pos from simplex mesh
	kd_res_item( nearestPointSet, pt ); //pt from surface

	int inFlag = isPointInsideVessel(pos,pt);//a point from simplex vertex and a point from surface.
	double changeFlag =0 ;
	
	double dis = sqrt((pt[0]-data->pos[0])*(pt[0]-data->pos[0])
					+ (pt[1]-data->pos[1])*(pt[1]-data->pos[1])
					+ (pt[2]-data->pos[2])*(pt[2]-data->pos[2]));
	distanceCoefficient = dis;
	//if(dis < 0.35)  dis = 0;
    //if(dis >= 0.7) distanceCoefficient = 1;
	if(dis < 0.1)  dis = 0;
	if(dis >= 0.3) distanceCoefficient = 1;


	//compose the displacement using all the forces together
	displacement.Set_vnl_vector(LinkLengthForce.Get_vnl_vector());
	data->pos += displacement;

    displacement.Set_vnl_vector( m_Alpha * ( data->internalForce).Get_vnl_vector()
		                      +  distanceCoefficient * (data->externalForce).Get_vnl_vector() 
							  +  StrutLengthForce.Get_vnl_vector());
							  //+  LinkLengthForce.Get_vnl_vector());
	changeFlag = inFlag*catheterConstraint*distanceCoefficient;
	if(changeFlag){
		isDataChanged ++;
		data->pos +=  changeFlag * displacement;
	}
	nonConstPoints->InsertElement(dataIt.Index(), data->pos);
  
    dataIt++;
	centerIdx++;
   }//end of while
  //-------------timer--------
  (void) time(&t2);
  //-------------timer--------
  int diff =(int) t2 -t1;

}
//-------------------------

/*
* Override External force, 
* compute it as displacement vectors along the direction normal to the simplex surface
*/
template< typename TInputMesh, typename TOutputMesh >
void
DeformableSimplexMeshFilterImpl< TInputMesh, TOutputMesh >
::ComputeExternalForce(SimplexMeshGeometry *data)
{
  data->externalForce[0] = m_Beta * ( data->normal )[0];
  data->externalForce[1] = m_Beta * ( data->normal )[1];
  data->externalForce[2] = m_Beta * ( data->normal )[2];
}
//---------------------------------------

/*
* Add new force to keep the length of the struts and links
*/
template< typename TInputMesh, typename TOutputMesh >
itk::Vector< double, 3 > DeformableSimplexMeshFilterImpl< TInputMesh, TOutputMesh >
::ComputeStrutLengthForce(SimplexMeshGeometry *data, int index){

	VectorType lengthForce;
	lengthForce.Fill(0);
	PointType neighbor;
	double curLength;

	int n = 0;
	//m_StrutLength is the reference length of the strut
	if(StrutNeighbors[index][0] != -1 ) { //&& StrutNeighbors[index][0] < index ){ //!= -1){
		neighbor = this->m_Data->GetElement(StrutNeighbors[index][0])->pos;
		curLength= sqrt((data->pos[0]-neighbor[0])*(data->pos[0]-neighbor[0])
			     +(data->pos[1]-neighbor[1])*(data->pos[1]-neighbor[1])
				 +(data->pos[2]-neighbor[2])*(data->pos[2]-neighbor[2]));
		lengthForce[0] -= (curLength - m_StrutLength)*(data->pos[0] - neighbor[0])/curLength;
		lengthForce[1] -= (curLength - m_StrutLength)*(data->pos[1] - neighbor[1])/curLength;
		lengthForce[2] -= (curLength - m_StrutLength)*(data->pos[2] - neighbor[2])/curLength;
		n++;
	}

	if(StrutNeighbors[index][1] != -1 ) { //&& StrutNeighbors[index][1] < index ) { //!= -1){
		neighbor = this->m_Data->GetElement(StrutNeighbors[index][1])->pos;
		curLength= sqrt((data->pos[0]-neighbor[0])*(data->pos[0]-neighbor[0])
			     +(data->pos[1]-neighbor[1])*(data->pos[1]-neighbor[1])
				 +(data->pos[2]-neighbor[2])*(data->pos[2]-neighbor[2]));
		lengthForce[0] -= (curLength - m_StrutLength)*(data->pos[0] - neighbor[0])/curLength;
		lengthForce[1] -= (curLength - m_StrutLength)*(data->pos[1] - neighbor[1])/curLength;
		lengthForce[2] -= (curLength - m_StrutLength)*(data->pos[2] - neighbor[2])/curLength;
		n++;
	}

	if(n>0){
	lengthForce[0] /= 2;
	lengthForce[1] /= 2;
	lengthForce[2] /= 2;
	}

	return lengthForce;
}
//----------------

template< typename TInputMesh, typename TOutputMesh >
itk::Vector< double, 3 > DeformableSimplexMeshFilterImpl< TInputMesh, TOutputMesh >
::ComputeLinkLengthForce(SimplexMeshGeometry *data, int index){
			//m_LinkLength is the reference length of link
	VectorType lengthForce;
	lengthForce.Fill(0);
	PointType neighbor;
	double curLength;
	//if(StrutNeighbors[index][2] != -1 && StrutNeighbors[index][2] < index ) {
	if(StrutNeighbors[index][2] != -1 ) {
		neighbor = this->m_Data->GetElement(StrutNeighbors[index][2])->pos;
		curLength= sqrt((data->pos[0]-neighbor[0])*(data->pos[0]-neighbor[0])
			     +(data->pos[1]-neighbor[1])*(data->pos[1]-neighbor[1])
				 +(data->pos[2]-neighbor[2])*(data->pos[2]-neighbor[2]));
		lengthForce[0] -= (curLength - m_LinkLength)*(data->pos[0] - neighbor[0])/curLength;
		lengthForce[1] -= (curLength - m_LinkLength)*(data->pos[1] - neighbor[1])/curLength;
		lengthForce[2] -= (curLength - m_LinkLength)*(data->pos[2] - neighbor[2])/curLength;
	}
	return lengthForce;
}
//------------------

}
