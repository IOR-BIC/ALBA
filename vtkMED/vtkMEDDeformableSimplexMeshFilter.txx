

#ifndef __vtkMEDDeformableSimplexMesh3DFilter_txx
#define __vtkMEDDeformableSimplexMesh3DFilter_txx

#include "itkDeformableSimplexMesh3DGradientConstraintForceFilter.h"
#include "itkNumericTraits.h"
#include "vnl/vnl_math.h"
#include "vtkMath.h"
#include "vtkIdList.h"
#include "vtkCell.h"
#include "vtkCellArray.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkIdType.h"



namespace itk
{
  //----------------------------------------------------------------------------
  // Constructor
  //----------------------------------------------------------------------------
  template< typename TInputMesh, typename TOutputMesh >
  vtkMEDDeformableSimplexMeshFilter< TInputMesh, TOutputMesh >
    ::vtkMEDDeformableSimplexMeshFilter()
    : m_Epsilon(0.3), m_StrutNeighbors(NULL)
  {
    m_DistanceCoefficient = 1;
    m_CurrentStepNum = 0;

    m_CatheterCalculator = new CatheterCalculator ;
  }



  //----------------------------------------------------------------------------
  // destructor
  //----------------------------------------------------------------------------
  template< typename TInputMesh, typename TOutputMesh >
  vtkMEDDeformableSimplexMeshFilter< TInputMesh, TOutputMesh >
    ::~vtkMEDDeformableSimplexMeshFilter()
  {
    if (m_StrutNeighbors != NULL)
      delete[] m_StrutNeighbors;

    delete m_CatheterCalculator ;
  }



  //----------------------------------------------------------------------------
  // The difference from SetStrutLinkIter is parameter
  // setting m_StrutNeighbors: [0],[1] strut neighbor; [2] link neighbor
  // record for every simplex vertex
  // prepare for calculating strut&link length force
  // if the value equals -1, means not applicable
  // here, if the simplex vertex serves as extremity of stent structure, 
  // then it as least has the first two strut neighbor
  // if not, no neighbors
  // if this vertex is also connected to next crown with links, 
  // then it has the third link neighbor
  //----------------------------------------------------------------------------
  template< typename TInputMesh, typename TOutputMesh >
  void vtkMEDDeformableSimplexMeshFilter< TInputMesh, TOutputMesh >
    ::SetStrutLinkFromCellArray(vtkCellArray *strut,vtkCellArray *link)
  {
      //initialization
      int VertexNumber = this->GetInput(0)->GetPoints()->size();
      m_StrutNeighbors = new int[VertexNumber][3];
      for(int i=0;i<VertexNumber;i++){
        m_StrutNeighbors[i][0] = -1;
        m_StrutNeighbors[i][1] = -1;
        m_StrutNeighbors[i][2] = -1;
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
        if(m_StrutNeighbors[start][0] == -1)
          m_StrutNeighbors[start][0] = end;
        else 
          if(m_StrutNeighbors[start][0]!= end && m_StrutNeighbors[start][1] == -1)
            m_StrutNeighbors[start][1] = end;

        if(m_StrutNeighbors[end][0] == -1)
          m_StrutNeighbors[end][0] = start;
        else 
          if(m_StrutNeighbors[end][0]!= start && m_StrutNeighbors[end][1] == -1)
            m_StrutNeighbors[end][1] = start;

      }
      /*loop link next*/
      // set link neighbor: [2]
      for ( link->InitTraversal(); link->GetNextCell(npts,pts); ){
        start = pts[0];
        end = pts[1];

        if(m_StrutNeighbors[start][2] == -1)
          m_StrutNeighbors[start][2] = end;
        if(m_StrutNeighbors[end][2] == -1)
          m_StrutNeighbors[end][2] = start;		
      }

  }



  //----------------------------------------------------------------------------
  // setting m_StrutNeighbors: [0],[1] strut neighbor; [2] link neighbor
  // record for every simplex vertex
  // prepare for calculating strut&link length force
  // if the value equals -1, means not applicable
  // here, if the simplex vertex serves as extremity of stent structure, 
  // then it as least has the first two strut neighbor
  // if not, no neighbors
  // if this vertex is also connected to next crown with links, 
  // then it has the third link neighbor
  //----------------------------------------------------------------------------
  template< typename TInputMesh, typename TOutputMesh >
  void vtkMEDDeformableSimplexMeshFilter< TInputMesh, TOutputMesh >
    ::SetStrutLinkIter(StrutIterator StrutStart, StrutIterator StrutEnd,
    StrutIterator LinkStart, StrutIterator LinkEnd)
  {
      //initialization
      int VertexNumber = this->GetInput(0)->GetPoints()->size();
      m_StrutNeighbors = new int[VertexNumber][3];
      for(int i=0;i<VertexNumber;i++){
        m_StrutNeighbors[i][0] = -1;
        m_StrutNeighbors[i][1] = -1;
        m_StrutNeighbors[i][2] = -1;
      }

      // set strut neighbor: [0] & [1]
      for(StrutIterator iter = StrutStart; iter !=StrutEnd; iter++){
        if(m_StrutNeighbors[iter->startVertex][0] == -1)
          m_StrutNeighbors[iter->startVertex][0] = iter->endVertex;
        else 
          if(m_StrutNeighbors[iter->startVertex][0]!= iter->endVertex
            &&m_StrutNeighbors[iter->startVertex][1] == -1)
            m_StrutNeighbors[iter->startVertex][1] = iter->endVertex;

        if(m_StrutNeighbors[iter->endVertex][0] == -1)
          m_StrutNeighbors[iter->endVertex][0] = iter->startVertex;
        else 
          if(m_StrutNeighbors[iter->endVertex][0]!= iter->startVertex
            &&m_StrutNeighbors[iter->endVertex][1] == -1)
            m_StrutNeighbors[iter->endVertex][1] = iter->startVertex;
      }
      // set link neighbor: [2]
      for(StrutIterator iter = LinkStart; iter !=LinkEnd; iter++){
        if(m_StrutNeighbors[iter->startVertex][2] == -1)
          m_StrutNeighbors[iter->startVertex][2] = iter->endVertex;
        if(m_StrutNeighbors[iter->endVertex][2] == -1)
          m_StrutNeighbors[iter->endVertex][2] = iter->startVertex;
      } 
  }



  //----------------------------------------------------------------------------
  // Import vertices of vessel wall , and create a KD-tree 
  // for accuracy, we need the vessel wall to be represented with dense point cloud
  // if not dense enough, sampling before creating the KD-tree
  //----------------------------------------------------------------------------
  /*
  template< typename TInputMesh, typename TOutputMesh >
  void vtkMEDDeformableSimplexMeshFilter< TInputMesh, TOutputMesh >
  ::SetVesselPointsKDTree(PointIterator PointStart, PointIterator PointEnd)
  {
  m_KDTree = kd_create(3);
  for(PointIterator iter = PointStart; iter != PointEnd; iter++){
  assert(kd_insert3(m_KDTree,iter->x,iter->y,iter->z,0)==0);
  }
  }
  */
  template< typename TInputMesh, typename TOutputMesh >
  void vtkMEDDeformableSimplexMeshFilter< TInputMesh, TOutputMesh >
    ::SetVesselPointsKDTreeFromPolyData(vtkPolyData *surface)
  {
      m_SurfacePoly = surface;
      vtkPoints *points = surface->GetPoints();

      vtkIdType num = surface->GetNumberOfPoints();

      double p[3];
      m_KDTree = kd_create(3);
      for(vtkIdType i=0;i<num;i++){
        points->GetPoint(i,p);
        assert(kd_insert3(m_KDTree,p[0],p[1],p[2],0)==0);
      }
  }


  //1 inside ,0,outside 
  template< typename TInputMesh, typename TOutputMesh >
  int vtkMEDDeformableSimplexMeshFilter< TInputMesh, TOutputMesh >
    :: isPointInsideVessel(double *stentPoint, double *surfacePoint) 
  {
    int rtnFlag = 1;//inside
    //----to have a test-
    //double outPoint[3];
    //double aPoint[3];
    //m_SurfacePoly->GetPoint(0,aPoint); X                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
    vtkIdType surPointIndex = m_SurfacePoly->FindPoint(surfacePoint);
    vtkIdList *apCellIds = vtkIdList::New();
    vtkIdList *apNPointIds  = vtkIdList::New();
    m_SurfacePoly->GetPointCells (surPointIndex, apCellIds);
    vtkIdType cellNumber = apCellIds->GetNumberOfIds();

    //vtkIdType pointId1, pointId2 ;
    vtkIdType cellId ;
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


  //----------------------------------------------------------------------------
  //----------------------------------------------------------------------------
  template< typename TInputMesh, typename TOutputMesh >
  void vtkMEDDeformableSimplexMeshFilter< TInputMesh, TOutputMesh >
    ::vectorSubtract(double *endPoint,double *startPoint,double *result)
  {
      result[0] = endPoint[0]-startPoint[0];
      result[1] = endPoint[1]-startPoint[1];
      result[2] = endPoint[2]-startPoint[2];
  }



  //----------------------------------------------------------------------------
  //----------------------------------------------------------------------------
  template< typename TInputMesh, typename TOutputMesh >
  double vtkMEDDeformableSimplexMeshFilter< TInputMesh, TOutputMesh >
    ::computeDirectionFromPoint2Face(double *stentPoint,vtkIdType p1,vtkIdType p2,vtkIdType p3)
  {
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



  //----------------------------------------------------------------------------
  // for every simplex vertex,
  // m_CenterLocationIdx stores the corresponding centerline position
  // this information is recorded during the simplex mesh & stent model creation
  // and imported here using setter
  //----------------------------------------------------------------------------
  template< typename TInputMesh, typename TOutputMesh >
  void vtkMEDDeformableSimplexMeshFilter< TInputMesh, TOutputMesh >
    ::SetCenterLocationIdx(vector<int>::const_iterator centerLocationIndex){
      m_CenterLocationIdx = centerLocationIndex;
  }

  template< typename TInputMesh, typename TOutputMesh >
  void
    vtkMEDDeformableSimplexMeshFilter< TInputMesh, TOutputMesh >
    ::SetCenterLocationIdxRef(vector<int> const&ve){
      m_CenterLocationIdx = ve.begin();
  }





  //----------------------------------------------------------------------------
  // Override the ComputeDisplacement() function
  //   1. Use KD tree to calculate the distance from the simplex vertex to the vessel wall
  //        This distance is used to weight the external force and total displacement
  //   2. Synchronize the catheter's pulling over with the expansion of a certain circle of simplex vertices
  //----------------------------------------------------------------------------
  template< typename TInputMesh, typename TOutputMesh >
  void vtkMEDDeformableSimplexMeshFilter< TInputMesh, TOutputMesh >
    ::ComputeDisplacement()
  {
    //-------------timer--------
    time_t t1,t2;
    //-------------timer--------

    const InputMeshType *inputMesh = this->GetInput(0);

    // Filters should not modify their input...
    // There is a design flaw here.
    InputPointsContainer *nonConstPoints = const_cast< InputPointsContainer * >( inputMesh->GetPoints() );

    SimplexMeshGeometry *data;
    VectorType displacement;

    double pt[3]; //point on surface
    double pos[3];
    VectorType StrutLengthForce;
    VectorType LinkLengthForce;
    kdres *nearestPointSet;
    m_IsDataChanged = 0;

    //-------------timer--------
    (void) time(&t1);
    //-------------timer--------


    //--------------------------------------------------------------------------
    // Determine for each vertex whether it is constrained by the catheter.
    // Constrained vertices are those inside the catether and their neighbors.
    //--------------------------------------------------------------------------
    int constrained[10000] ;
    vector<int>::const_iterator centerIdx = m_CenterLocationIdx ;
    GeometryMapType::Iterator dataIt ;
    int i ;
    for (dataIt = m_Data->Begin(), centerIdx = m_CenterLocationIdx, i = 0 ;  dataIt != m_Data->End() ;  dataIt++, centerIdx++, i++){
      data = dataIt.Value();

      // Create table showing whether vertices are constrained by catheter
      if (m_CatheterCalculator->IsVertexInsideCatheter(*centerIdx, m_CurrentStepNum)){
        // constrain point and its neighbors
        constrained[i] = 1 ;
        for (int j = 0 ;  j < 3 ;  j++){
          int id = m_StrutNeighbors[i][j] ;
          if (id > 0)
            constrained[id] = 1 ;
        }
      }      
      else
        constrained[i] = 0 ;
    }


    //--------------------------------------------------------------------------
    // Loop for every simplex vertex
    //--------------------------------------------------------------------------
    for (dataIt = m_Data->Begin(), centerIdx = m_CenterLocationIdx, i = 0 ;  dataIt != m_Data->End() ;  dataIt++, centerIdx++, i++){
      data = dataIt.Value();

      int constraintFlag = constrained[i] ? 0 : 1 ;  // 0 if contrained, else 1

      //compute internal,external and length force
      this->ComputeInternalForce(data);
      this->ComputeExternalForce(data);	
      StrutLengthForce = this->ComputeStrutLengthForce(data,dataIt.Index());
      LinkLengthForce = this->ComputeLinkLengthForce(data,dataIt.Index());

      //Use KD tree to calculate the distance from the simplex vertex to the vessel wall
      //This distance is used to weight the external force and total displacement
      pos[0] = data->pos[0];
      pos[1] = data->pos[1];
      pos[2] = data->pos[2];
      nearestPointSet = kd_nearest3(m_KDTree, pos[0], pos[1], pos[2]);//pos from simplex mesh
      kd_res_item( nearestPointSet, pt ); //pt from surface

      int inFlag = isPointInsideVessel(pos,pt);//a point from simplex vertex and a point from surface.
      double changeFlag =0 ;

      double dis = sqrt((pt[0]-data->pos[0])*(pt[0]-data->pos[0])
        + (pt[1]-data->pos[1])*(pt[1]-data->pos[1])
        + (pt[2]-data->pos[2])*(pt[2]-data->pos[2]));
      if (dis >= m_Epsilon)
        m_DistanceCoefficient = 1.0 ;
      else{
        double rat = dis/m_Epsilon ;
        m_DistanceCoefficient = rat*rat ;
      }

      //compose the displacement using all the forces together
      displacement.Set_vnl_vector(LinkLengthForce.Get_vnl_vector());
      data->pos += displacement;

      displacement.Set_vnl_vector( m_Alpha * ( data->internalForce).Get_vnl_vector()
        +  m_DistanceCoefficient * (data->externalForce).Get_vnl_vector() 
        +  StrutLengthForce.Get_vnl_vector());
      //+  LinkLengthForce.Get_vnl_vector());

      changeFlag = inFlag*constraintFlag*m_DistanceCoefficient;
      if(changeFlag){
        m_IsDataChanged++;
        data->pos +=  changeFlag * displacement;
      }
      nonConstPoints->InsertElement(dataIt.Index(), data->pos);

    }//end of loop over vertices
    //-------------timer--------
    (void) time(&t2);
    //-------------timer--------
    int diff =(int) t2 -t1;

  }



  //----------------------------------------------------------------------------
  // Override External force, 
  // compute it as displacement vectors along the direction normal to the simplex surface
  //----------------------------------------------------------------------------
  template< typename TInputMesh, typename TOutputMesh >
  void vtkMEDDeformableSimplexMeshFilter< TInputMesh, TOutputMesh >
    ::ComputeExternalForce(SimplexMeshGeometry *data)
  {
    data->externalForce[0] = m_Beta * ( data->normal )[0];
    data->externalForce[1] = m_Beta * ( data->normal )[1];
    data->externalForce[2] = m_Beta * ( data->normal )[2];
  }



  //----------------------------------------------------------------------------
  // Add new force to keep the length of the struts and links
  //----------------------------------------------------------------------------
  template< typename TInputMesh, typename TOutputMesh >
  itk::Vector< double, 3 > vtkMEDDeformableSimplexMeshFilter< TInputMesh, TOutputMesh >
    ::ComputeStrutLengthForce(SimplexMeshGeometry *data, int index)
  {
      VectorType lengthForce;
      lengthForce.Fill(0);
      PointType neighbor;
      double curLength;

      int n = 0;
      //m_StrutLength is the reference length of the strut
      if(m_StrutNeighbors[index][0] != -1 ) { //&& m_StrutNeighbors[index][0] < index ){ //!= -1){
        neighbor = this->m_Data->GetElement(m_StrutNeighbors[index][0])->pos;
        curLength= sqrt((data->pos[0]-neighbor[0])*(data->pos[0]-neighbor[0])
          +(data->pos[1]-neighbor[1])*(data->pos[1]-neighbor[1])
          +(data->pos[2]-neighbor[2])*(data->pos[2]-neighbor[2]));
        lengthForce[0] -= (curLength - m_StrutLength)*(data->pos[0] - neighbor[0])/curLength;
        lengthForce[1] -= (curLength - m_StrutLength)*(data->pos[1] - neighbor[1])/curLength;
        lengthForce[2] -= (curLength - m_StrutLength)*(data->pos[2] - neighbor[2])/curLength;
        n++;
      }

      if(m_StrutNeighbors[index][1] != -1 ) { //&& m_StrutNeighbors[index][1] < index ) { //!= -1){
        neighbor = this->m_Data->GetElement(m_StrutNeighbors[index][1])->pos;
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



  //----------------------------------------------------------------------------
  //----------------------------------------------------------------------------
  template< typename TInputMesh, typename TOutputMesh >
  itk::Vector< double, 3 > vtkMEDDeformableSimplexMeshFilter< TInputMesh, TOutputMesh >
    ::ComputeLinkLengthForce(SimplexMeshGeometry *data, int index)
  {
    //m_LinkLength is the reference length of link
    VectorType lengthForce;
    lengthForce.Fill(0);
    PointType neighbor;
    double curLength;
    //if(m_StrutNeighbors[index][2] != -1 && m_StrutNeighbors[index][2] < index ) {
    if(m_StrutNeighbors[index][2] != -1 ) {
      neighbor = this->m_Data->GetElement(m_StrutNeighbors[index][2])->pos;
      curLength= sqrt((data->pos[0]-neighbor[0])*(data->pos[0]-neighbor[0])
        +(data->pos[1]-neighbor[1])*(data->pos[1]-neighbor[1])
        +(data->pos[2]-neighbor[2])*(data->pos[2]-neighbor[2]));
      lengthForce[0] -= (curLength - m_LinkLength)*(data->pos[0] - neighbor[0])/curLength;
      lengthForce[1] -= (curLength - m_LinkLength)*(data->pos[1] - neighbor[1])/curLength;
      lengthForce[2] -= (curLength - m_LinkLength)*(data->pos[2] - neighbor[2])/curLength;
    }
    return lengthForce;
  }


  //----------------------------------------------------------------------------
  // Catheter calculator:
  // Set pause at position
  //----------------------------------------------------------------------------
  template< typename TInputMesh, typename TOutputMesh >
  void vtkMEDDeformableSimplexMeshFilter<TInputMesh, TOutputMesh >::
    CatheterCalculator::SetPauseAtPosition(double p, double len)
  {
    m_PauseType = PAUSE_AT_POSITION;
    m_PauseLength = len ;  
    m_PauseAtPosition = p;
    m_PauseAtTime = p / m_Speed ;
  }
 

  //----------------------------------------------------------------------------
  // Catheter calculator:
  // Set pause at time
  //----------------------------------------------------------------------------
  template< typename TInputMesh, typename TOutputMesh >
  void vtkMEDDeformableSimplexMeshFilter<TInputMesh, TOutputMesh >::
    CatheterCalculator::SetPauseAtTime(double t, double len)
  {
    m_PauseType = PAUSE_AT_POSITION;
    m_PauseLength = len ;  
    m_PauseAtTime = t ;
    m_PauseAtPosition = m_Speed * t ;
  }
 

  //----------------------------------------------------------------------------
  // Catheter calculator:
  // Calculate position at time t
  //----------------------------------------------------------------------------
  template< typename TInputMesh, typename TOutputMesh >
  double vtkMEDDeformableSimplexMeshFilter<TInputMesh, TOutputMesh >::
    CatheterCalculator::CalculatePosition(double t) const
  {
    switch(m_PauseType){
    case PAUSE_NONE:
      return m_StartPos + m_Speed * t ;
      break ;
    case PAUSE_AT_TIME:
    case PAUSE_AT_POSITION:
      if (t < m_PauseAtTime)
        return m_StartPos + m_Speed * t ;
      else if (t < m_PauseAtTime + m_PauseLength)
        return m_StartPos + m_PauseAtPosition ;
      else
        return m_StartPos + m_Speed * (t - m_PauseLength) ;
      break ;
    default:
      // can't happen
      return -10000.0 ;
    }
  }


  //----------------------------------------------------------------------------
  // Catheter calculator:
  // Set the center line and cache the positions of the vertices along the line
  //----------------------------------------------------------------------------
  template< typename TInputMesh, typename TOutputMesh >
  void vtkMEDDeformableSimplexMeshFilter<TInputMesh, TOutputMesh >::
    CatheterCalculator::SetCenterLine(vtkPolyData* centerLine) 
  {
    m_CenterLine = centerLine ;

    m_VertexPositions.clear() ;
    int n = m_CenterLine->GetPoints()->GetNumberOfPoints() ;
    for (int i = 0 ;  i < n ;  i++){
      double xold[3], x[3] ;
      if (i == 0){
        m_CenterLine->GetPoint(i,xold) ;
        m_VertexPositions.push_back(0.0) ;
      }
      else{
        m_CenterLine->GetPoint(i,x) ;
        double r2 = 0.0 ;
        for (int j = 0 ;  j < 3 ;  j++){
          r2 += (x[j]-xold[j])*(x[j]-xold[j]) ;
          xold[j] = x[j] ;
        }
        double dist = m_VertexPositions[i-1] + sqrt(r2) ;
        m_VertexPositions.push_back(dist) ;       
      }
    }
  }



  //----------------------------------------------------------------------------
  // Catheter calculator:
  // Get position of vertex along center line
  //----------------------------------------------------------------------------
  template< typename TInputMesh, typename TOutputMesh >
  double vtkMEDDeformableSimplexMeshFilter<TInputMesh, TOutputMesh >::
    CatheterCalculator::GetVertexPosition(int idx) const
  {
    int n = m_CenterLine->GetPoints()->GetNumberOfPoints() ;

    if (idx < 0){
      double dv = m_VertexPositions[0] - m_VertexPositions[1] ;
      return m_VertexPositions[0] - (double)idx * dv ;
    }
    else if (idx >= n){
      double dv = m_VertexPositions[n-1] - m_VertexPositions[n-2] ;
      return m_VertexPositions[n-1] + (double)idx * dv ;
    }
    else
      return m_VertexPositions.at(idx) ;
  }



  //----------------------------------------------------------------------------
  // Catheter calculator:
  // Is center line vertex still inside the catheter at time t.
  //----------------------------------------------------------------------------
  template< typename TInputMesh, typename TOutputMesh >
  bool vtkMEDDeformableSimplexMeshFilter<TInputMesh, TOutputMesh >::
    CatheterCalculator::IsVertexInsideCatheter(int idx, double t) const
  {
    double cathPos = CalculatePosition(t) ;
    double vertPos = GetVertexPosition(idx) ;
    return cathPos <= vertPos ;
  }



  //----------------------------------------------------------------------------
  // Catheter calculator:
  // Get index of first vertex which is still inside the catheter at time t.
  // ie return the vertex which corresponds to the catheter position.
  //----------------------------------------------------------------------------
  template< typename TInputMesh, typename TOutputMesh >
  int vtkMEDDeformableSimplexMeshFilter<TInputMesh, TOutputMesh >::
    CatheterCalculator::GetFirstVertexInsideCatheter(double t) const
  {
    int n = m_CenterLine->GetPoints()->GetNumberOfPoints() ;
    for (int i = 0 ;  i < n ;  i++){
      if (IsVertexInsideCatheter(i,t))
        return i ;
    }

    return n+1000 ;
  }




  //-----------------------------------------------------------------------------
  // Create truncated center line, ie the part which is
  // inside the catheter at time t.
  // Returns true if successful.
  //-----------------------------------------------------------------------------
  template< typename TInputMesh, typename TOutputMesh >
  bool vtkMEDDeformableSimplexMeshFilter<TInputMesh, TOutputMesh >::
    CatheterCalculator::CreateTruncatedCenterLine(vtkPolyData* truncLine, double t) const 
  {
    if (m_CenterLine == NULL)
      return false ;

    int n = m_CenterLine->GetPoints()->GetNumberOfPoints() ;
    int istart = GetFirstVertexInsideCatheter(t) ;

    bool ok = CreateTruncatedCenterLine(m_CenterLine, truncLine, istart, n-1) ;
    return ok ;
  }



  //-----------------------------------------------------------------------------
  // Create subset of center line between vertices idFirst and idLast inclusive.
  // Static utility which does not require any parameters set.
  // Returns true if successful.
  //-----------------------------------------------------------------------------
  template< typename TInputMesh, typename TOutputMesh >
  bool vtkMEDDeformableSimplexMeshFilter<TInputMesh, TOutputMesh >::
    CatheterCalculator::CreateTruncatedCenterLine(vtkPolyData* inputLine, vtkPolyData* truncLine, int idFirst, int idLast)
  {
    truncLine->Initialize() ;

    if (inputLine == NULL)
      return false ;

    int n = inputLine->GetPoints()->GetNumberOfPoints() ;
    int i0 = std::max(0, idFirst) ;
    int i1 = std::min(n-1, idLast) ;
    int m = i1-i0+1 ;
    if (m < 2)
      return false ;

    vtkPoints *points = vtkPoints::New() ;
    vtkCellArray *lines = vtkCellArray::New() ;

    // copy points
    for (int i = i0 ;  i <= i1 ;  i++){
      double x[3] ;
      inputLine->GetPoint(i, x) ;
      points->InsertNextPoint(x) ;
    }

    // create polyline cell
    int *ids = new int[m] ;
    for (int i = 0 ;  i < m ;  i++)
      ids[i] = i ;
    lines->InsertNextCell(m, ids) ;
    delete [] ids ;

    truncLine->SetPoints(points) ;
    truncLine->SetLines(lines) ;
    points->Delete() ;
    lines->Delete() ;

    return true ;
  }

} // itk namespace

#endif
