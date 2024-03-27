

#ifndef __vtkALBADeformableSimplexMesh3DFilter_Impl_h
#define __vtkALBADeformableSimplexMesh3DFilter_Impl_h

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
  vtkALBADeformableSimplexMeshFilter< TInputMesh, TOutputMesh >
    ::vtkALBADeformableSimplexMeshFilter()
    : m_Epsilon(0.3), m_StrutNeighbors(NULL), m_MinDistanceToVessel(0.0)
  {
    m_DistanceCoefficient = 1;
    m_CurrentStepNum = 0;

    m_CatheterCalculator = new CatheterCalculator ;
    m_DeformationHistory = new DeformationHistory ;
  }



  //----------------------------------------------------------------------------
  // destructor
  //----------------------------------------------------------------------------
  template< typename TInputMesh, typename TOutputMesh >
  vtkALBADeformableSimplexMeshFilter< TInputMesh, TOutputMesh >
    ::~vtkALBADeformableSimplexMeshFilter()
  {
    if (m_StrutNeighbors != NULL)
      delete[] m_StrutNeighbors;

    delete m_CatheterCalculator ;
    delete m_DeformationHistory ;
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
  void vtkALBADeformableSimplexMeshFilter< TInputMesh, TOutputMesh >
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
  void vtkALBADeformableSimplexMeshFilter< TInputMesh, TOutputMesh >
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
  void vtkALBADeformableSimplexMeshFilter< TInputMesh, TOutputMesh >
  ::SetVesselPointsKDTree(PointIterator PointStart, PointIterator PointEnd)
  {
  m_KDTree = kd_create(3);
  for(PointIterator iter = PointStart; iter != PointEnd; iter++){
  assert(kd_insert3(m_KDTree,iter->x,iter->y,iter->z,0)==0);
  }
  }
  */
  template< typename TInputMesh, typename TOutputMesh >
  void vtkALBADeformableSimplexMeshFilter< TInputMesh, TOutputMesh >
    ::SetVesselPointsKDTreeFromPolyData(vtkPolyData *surface)
  {
    m_SurfacePoly = surface;
    vtkPoints *points = surface->GetPoints();

    vtkIdType num = surface->GetNumberOfPoints();

    double p[3];
    m_KDTree = kd_create(3);
    for(vtkIdType i=0;i<num;i++){
      points->GetPoint(i,p);
      bool ok = (kd_insert3(m_KDTree,p[0],p[1],p[2],0)==0) ;
      assert(ok);
    }
  }




  //----------------------------------------------------------------------------
  //----------------------------------------------------------------------------
  template< typename TInputMesh, typename TOutputMesh >
  void vtkALBADeformableSimplexMeshFilter< TInputMesh, TOutputMesh >
    ::vectorSubtract(double *endPoint,double *startPoint,double *result)
  {
    result[0] = endPoint[0]-startPoint[0];
    result[1] = endPoint[1]-startPoint[1];
    result[2] = endPoint[2]-startPoint[2];
  }



  //----------------------------------------------------------------------------
  //----------------------------------------------------------------------------
  template< typename TInputMesh, typename TOutputMesh >
  double vtkALBADeformableSimplexMeshFilter< TInputMesh, TOutputMesh >
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
  void vtkALBADeformableSimplexMeshFilter< TInputMesh, TOutputMesh >
    ::SetCenterLocationIdx(std::vector<int>::const_iterator centerLocationIndex){
      m_CenterLocationIdx = centerLocationIndex;
  }

  template< typename TInputMesh, typename TOutputMesh >
  void
    vtkALBADeformableSimplexMeshFilter< TInputMesh, TOutputMesh >
    ::SetCenterLocationIdxRef(std::vector<int> const&ve){
      m_CenterLocationIdx = ve.begin();
  }





  //----------------------------------------------------------------------------
  // Override the ComputeDisplacement() function
  //   1. Use KD tree to calculate the distance from the simplex vertex to the vessel wall
  //        This distance is used to weight the external force and total displacement
  //   2. Synchronize the catheter's pulling over with the expansion of a certain circle of simplex vertices
  //----------------------------------------------------------------------------
  template< typename TInputMesh, typename TOutputMesh >
  void vtkALBADeformableSimplexMeshFilter< TInputMesh, TOutputMesh >
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

    VectorType StrutLengthForce;
    VectorType LinkLengthForce;
    kdres *nearestPointSet;
    m_IsDataChanged = 0;

    double surfacePt[5000][3]; //point on surface
    static int surfacePtId[5000] ; // static cache of surface points

    static double pos[3];


    //-------------timer--------
    (void) time(&t1);
    //-------------timer--------


    //--------------------------------------------------------------------------
    // Determine for each vertex whether it is currently constrained by the catheter.
    // Constrained vertices are those inside the catheter and their neighbors.
    //--------------------------------------------------------------------------
    int constrained[5000] ;
    int i ;
    for (i = 0 ;  i < 5000 ;  i++)
      constrained[i] = 0 ;

    std::vector<int>::const_iterator centerIdx = m_CenterLocationIdx ;
    GeometryMapType::Iterator dataIt ;
    for (dataIt = m_Data->Begin(), centerIdx = m_CenterLocationIdx, i = 0 ;  dataIt != m_Data->End() ;  dataIt++, centerIdx++, i++){
      data = dataIt.Value();

      // Create table showing whether vertices are constrained by catheter
      if (m_CatheterCalculator->IsVertexInsideCatheter(*centerIdx, m_CurrentStepNum)){
        // constrain point and also its neighbors
        constrained[i] = 1 ;
        for (int j = 0 ;  j < 3 ;  j++){
          int id = m_StrutNeighbors[i][j] ;
          if (id > 0)
            constrained[id] = 1 ;
        }
      }      
    }

    //--------------------------------------------------------------------------
    // Loop for every simplex vertex
    //--------------------------------------------------------------------------
    for (dataIt = m_Data->Begin(), centerIdx = m_CenterLocationIdx, i = 0 ;  dataIt != m_Data->End() ;  dataIt++, centerIdx++, i++){
      data = dataIt.Value();

      //Use KD tree to calculate the distance from the simplex vertex to the vessel wall
      //This distance is used to weight the external force and total displacement
      pos[0] = data->pos[0];
      pos[1] = data->pos[1];
      pos[2] = data->pos[2];

      if (m_CurrentStepNum % 5 == 0){
        nearestPointSet = kd_nearest3(m_KDTree, pos[0], pos[1], pos[2]); //pos from simplex mesh
        kd_res_item( nearestPointSet, surfacePt[i] ); // nearest pt from surface
        surfacePtId[i] = m_SurfacePoly->FindPoint(surfacePt[i]) ; // cache the id of the point
      }

      // set the distance-dependent weight
      double dist = sqrt((surfacePt[i][0]-data->pos[0])*(surfacePt[i][0]-data->pos[0])
        + (surfacePt[i][1]-data->pos[1])*(surfacePt[i][1]-data->pos[1])
        + (surfacePt[i][2]-data->pos[2])*(surfacePt[i][2]-data->pos[2]));
      if (dist >= m_Epsilon)
        m_DistanceCoefficient = 1.0 ;
      else
        m_DistanceCoefficient = dist/m_Epsilon ;

      int inFlag = IsPointInsideVessel2(pos, surfacePtId[i]); //a point from simplex vertex and a point from surface.

      //compute internal,external and length force
      this->ComputeInternalForce(data);
      this->ComputeExternalForce(data);	
      StrutLengthForce = this->ComputeStrutLengthForce(data,dataIt.Index());
      LinkLengthForce = this->ComputeLinkLengthForce(data,dataIt.Index());

      // calculate the displacement, equal to the total force
      displacement.Set_vnl_vector( 
        m_Alpha * ( data->internalForce).Get_vnl_vector()
        +  m_DistanceCoefficient * (data->externalForce).Get_vnl_vector() 
        +  StrutLengthForce.Get_vnl_vector()
        +  LinkLengthForce.Get_vnl_vector());

      int changeFlag = inFlag*(1-constrained[i]) ;
      if(changeFlag){
        m_IsDataChanged++;
        data->pos +=  m_DistanceCoefficient * displacement;
      }

      nonConstPoints->InsertElement(dataIt.Index(), data->pos);

      // save deformation info
      m_DeformationHistory->SaveDistToTarget(m_CurrentStepNum, dist) ;
      m_DeformationHistory->SaveInside(m_CurrentStepNum, inFlag) ;

    }//end of loop over vertices

    // save deformation info
    //m_DeformationHistory->SaveLengthsOfCells(m_CurrentStepNum, inputMesh) ;
    int n = inputMesh->GetNumberOfCells() ;


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
  void vtkALBADeformableSimplexMeshFilter< TInputMesh, TOutputMesh >
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
  itk::Vector< double, 3 > vtkALBADeformableSimplexMeshFilter< TInputMesh, TOutputMesh >
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
  itk::Vector< double, 3 > vtkALBADeformableSimplexMeshFilter< TInputMesh, TOutputMesh >
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
  void vtkALBADeformableSimplexMeshFilter<TInputMesh, TOutputMesh >::
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
  void vtkALBADeformableSimplexMeshFilter<TInputMesh, TOutputMesh >::
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
  double vtkALBADeformableSimplexMeshFilter<TInputMesh, TOutputMesh >::
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
  void vtkALBADeformableSimplexMeshFilter<TInputMesh, TOutputMesh >::
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
  double vtkALBADeformableSimplexMeshFilter<TInputMesh, TOutputMesh >::
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
  bool vtkALBADeformableSimplexMeshFilter<TInputMesh, TOutputMesh >::
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
  int vtkALBADeformableSimplexMeshFilter<TInputMesh, TOutputMesh >::
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
  // Catheter calculator:
  // Create truncated center line, ie the part which is
  // inside the catheter at time t.
  // Returns true if successful.
  //-----------------------------------------------------------------------------
  template< typename TInputMesh, typename TOutputMesh >
  bool vtkALBADeformableSimplexMeshFilter<TInputMesh, TOutputMesh >::
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
  // Catheter calculator:
  // Create subset of center line between vertices idFirst and idLast inclusive.
  // Static utility which does not require any parameters set.
  // Returns true if successful.
  //-----------------------------------------------------------------------------
  template< typename TInputMesh, typename TOutputMesh >
  bool vtkALBADeformableSimplexMeshFilter<TInputMesh, TOutputMesh >::
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



  

  //-----------------------------------------------------------------------------
  // DeformationHistory:
  // Save inside/outside status of point.
  // outside = 0, inside = 1
  //-----------------------------------------------------------------------------
  template< typename TInputMesh, typename TOutputMesh >
  void vtkALBADeformableSimplexMeshFilter<TInputMesh, TOutputMesh >::
    DeformationHistory::SaveInside(int step, int isInside)
  {
    int n = (int)m_IsInside.size() ;
    for (int j = 0 ; j < step-(n-1) ; j++){
      intList list ;
      m_IsInside.push_back(list) ;
    }
    m_IsInside[step].push_back(isInside) ;
  }



  //-----------------------------------------------------------------------------
  // DeformationHistory:
  // Save target distance of point
  //-----------------------------------------------------------------------------
  template< typename TInputMesh, typename TOutputMesh >
  void vtkALBADeformableSimplexMeshFilter<TInputMesh, TOutputMesh >::
    DeformationHistory::SaveDistToTarget(int step, double dist)
  {
    int n = (int)m_DistToTarget.size() ;
    for (int j = 0 ; j < step-(n-1) ; j++){
      doubleList list ;
      m_DistToTarget.push_back(list) ;
    }
    m_DistToTarget[step].push_back(dist) ;
  }



  //-----------------------------------------------------------------------------
  // DeformationHistory:
  // Save cell length
  //-----------------------------------------------------------------------------
  template< typename TInputMesh, typename TOutputMesh >
  void vtkALBADeformableSimplexMeshFilter<TInputMesh, TOutputMesh >::
    DeformationHistory::SaveCellLength(int step, double length)
  {
    int n = (int)m_Length.size() ;
    for (int j = 0 ; j < step-(n-1) ; j++){
      doubleList list ;
      m_Length.push_back(list) ;
    }
    m_Length[step].push_back(length) ;
  }



  //-----------------------------------------------------------------------------
  // DeformationHistory:
  // Save lengths of cells
  //-----------------------------------------------------------------------------
  template< typename TInputMesh, typename TOutputMesh >
  void vtkALBADeformableSimplexMeshFilter<TInputMesh, TOutputMesh >::
    DeformationHistory::SaveLengthsOfCells(int step, const vtkALBAStentModelSource::SimplexMeshType *mesh)
  {
    int n = mesh->GetNumberOfCells() ;

    for (int i = 0 ;  i < n ;  i++){
      vtkALBAStentModelSource::CellAutoPointer cellPtr ;
      mesh->GetCell(i, cellPtr) ;
      CellInterface<PixelType, vtkALBAStentModelSource::SimplexMeshType::CellTraits>::PointIdConstIterator ptIdIterator = cellPtr->GetPointIds() ;
      int id0 = *ptIdIterator ;
      ptIdIterator++ ;
      int id1 = *ptIdIterator ;

      PointType pt ;
      double *x ;
      double x0[3], x1[3], dx[3] ;
      mesh->GetPoint(id0,&pt) ;
      x = pt.GetDataPointer() ;
      for (int j = 0 ;  j < 3 ;  j++)
        x0[j] = x[j] ;
      mesh->GetPoint(id1,&pt) ;
      x = pt.GetDataPointer() ;
      for (int j = 0 ;  j < 3 ;  j++)
        x1[j] = x[j] ;

      double r = 0.0 ;
      for (int j = 0 ;  j < 3 ;  j++){
        dx[j] = x1[j] - x0[j] ;
        r += dx[j]*dx[j] ;
      }
      r = sqrt(r) ;

      SaveCellLength(step, r) ;
    }
  }



  //-----------------------------------------------------------------------------
  // DeformationHistory:
  // Print Self
  //-----------------------------------------------------------------------------
  template< typename TInputMesh, typename TOutputMesh >
  void vtkALBADeformableSimplexMeshFilter<TInputMesh, TOutputMesh >::
  DeformationHistory::PrintSelf(ostream& os, int offset) const
  {
    int nsteps = (int)m_DistToTarget.size() ;
    assert((int)m_IsInside.size() == nsteps);
    //assert((int)m_Length.size() == nsteps);
    if (nsteps == 0){
      os << "Deformation info: no steps\n" ;
      return ;
    }

    int nPtIds = (int)m_DistToTarget[0].size() ;
    if (nPtIds == 0){
      os << "Deformation info: no pt ids\n" ;
      return ;
    }

    //int nCellIds = (int)m_Length[0].size() ;
    //if (nCellIds == 0){
    //  os << "Deformation info: no cell ids\n" ;
    //  return ;
    //}

    os << "Distance to target and in/out\n" ;
    os << " , " ;
    for (int id = 0 ;  id < nPtIds ;  id++)
      os << id << ",,, " ;
    os << "\n" ;
    for (int step = 0 ;  step < (int)nsteps ;  step++){
      os << step << ", " ;
      for (int id = 0 ;  id < nPtIds ;  id++)
        os << m_DistToTarget[step][id] << ", " << m_IsInside[step][id] << ",, " ;
      os << "\n" ;
    }
    os << "\n" ;

    os << "Distance to target\n" ;
    os << " , " ;
    for (int id = 0 ;  id < nPtIds ;  id++)
      os << id << ", " ;
    os << "\n" ;
    for (int step = 0 ;  step < (int)nsteps ;  step++){
      os << step << ", " ;
      for (int id = 0 ;  id < nPtIds ;  id++)
        os << m_DistToTarget[step][id] << ", ";
      os << "\n" ;
    }
    os << "\n" ;

    
    /* os << "Length of cell\n" ;
    os << " , " ;
    for (int id = 0 ;  id < nCellIds ;  id++)
      os << id << ", " ;
    os << "\n" ;
    for (int step = 0 ;  step < (int)nsteps ;  step++){
      os << step << ", " ;
      for (int id = 0 ;  id < nCellIds ;  id++)
        os << m_Length[step][id] << ", " ;
      os << "\n" ;
    }
    os << "\n" ; */
  }

} // itk namespace

#endif



