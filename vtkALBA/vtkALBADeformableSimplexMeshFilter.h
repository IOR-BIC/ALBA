
#ifndef __vtkALBADeformableSimplexMesh3DFilter_h 
#define __vtkALBADeformableSimplexMesh3DFilter_h

//-------------------------------
// By Xiangyin Ma, Nov 2012
//-----------------------------------

//-----------------------------------------------------------------------------
/// vtkALBADeformableSimplexMesh3DFilter \n
/// The deforming filter of simplex mesh. \n
/// 1. inherited from itk::DeformableSimplexMesh3DFilter \n
/// 2. Keep the internal force/smoothing force unchanged \n
/// 3. Override External force, compute it as displacement vectors \n 
///    along the direction normal to the simplex surface \n
/// 4. Add new force called 'Strut and link length force',  \n
/// to keep the length of the struts and links \n
/// 5. Override the ComputeDisplacement() function \n
///    5.1. Use KD tree to calculate the distance from the simplex vertex to \n
///         the vessel wall \n
///         This distance is used to weight the external force and total \n
///         displacement \n
///    5.2. Synchronize the catheter's pulling over  \n
///         with the expansion of a certain circle of simplex vertices \n
//-----------------------------------------------------------------------------


#include "itkDeformableSimplexMesh3DFilter.h"
#include "itkMesh.h"
#include "itkConstNeighborhoodIterator.h"
#include "itkCovariantVector.h"
#include "itkCellInterface.h"
#include "kdtree.h"
#include <assert.h>
#include "vtkCellArray.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"

#include "vtkALBAStentModelSource.h"

#include <ostream>
#include <vector>


namespace itk
{
  template< class TInputMesh, class TOutputMesh >
  class vtkALBADeformableSimplexMeshFilter :
    public DeformableSimplexMesh3DFilter< TInputMesh, TOutputMesh >
  {
  public:
    /** Standard "Self" typedefs. */
    typedef vtkALBADeformableSimplexMeshFilter Self;

    /** Standard "Superclass" typedef. */
    typedef  DeformableSimplexMesh3DFilter< TInputMesh, TOutputMesh > Superclass;

    /** Smart pointer typedef support */
    typedef SmartPointer< Self >       Pointer;
    typedef SmartPointer< const Self > ConstPointer;

    /** Method for creation through the object factory. */
    itkNewMacro(Self);

    /** Run-time type information (and related methods). */
    itkTypeMacro(vtkALBADeformableSimplexMeshFilter, DeformableSimplexMesh3DFilter);

    /** Some typedefs. */
    typedef TInputMesh  InputMeshType;
    typedef TOutputMesh OutputMeshType;

    typedef typename Superclass::PointType              PointType;
    typedef typename Superclass::GradientIndexType      GradientIndexType;
    typedef typename Superclass::GradientIndexValueType GradientIndexValueType;
    typedef typename Superclass::GradientType           GradientType;
    typedef typename Superclass::GradientImageType      GradientImageType;

    /** Mesh pointer definition. */
    typedef typename InputMeshType::Pointer  InputMeshPointer;
    typedef typename OutputMeshType::Pointer OutputMeshPointer;

    typedef typename InputMeshType::PixelType PixelType;

    typedef Image< PixelType, 3 >                        GradientIntensityImageType;
    typedef typename GradientIntensityImageType::Pointer GradientIntensityImagePointer;

    typedef Image< float, 3 >                               OriginalImageType;
    typedef typename OriginalImageType::IndexType           OriginalImageIndexType;
    typedef typename OriginalImageIndexType::IndexValueType ImageIndexValueType;
    typedef typename OriginalImageType::ConstPointer        OriginalImagePointer;

    typedef vtkALBAStentModelSource::Strut          Strut;
    typedef std::vector<Strut>::const_iterator    StrutIterator;
    typedef itk::Vector< double, 3 >         VectorType;

    //typedef VesselMeshForTesting::Point      Point;
    //typedef std::vector<Point>::const_iterator    PointIterator;

    /// Nested helper classes
    class CatheterCalculator ;
    class DeformationHistory ;

    void SetStrutLength(double len) {m_StrutLength = len ;}
    double GetStrutLength() const {return m_StrutLength ;}

    void SetLinkLength(double len) {m_LinkLength = len ;}
    double GetLinkLength() const {return m_LinkLength ;}

    void SetCurrentStepNum(double n) {m_CurrentStepNum = n ;}
    int GetCurrentStepNum() const {return m_CurrentStepNum ;}

    void SetEpsilon(double len) {m_Epsilon = len ;}
    double GetEpsilon() const {return m_Epsilon ;}

    void SetStrutLinkIter(StrutIterator StrutStart, StrutIterator StrutEnd,
      StrutIterator LinkStart, StrutIterator LinkEnd);

    void SetStrutLinkFromCellArray(vtkCellArray *strut,vtkCellArray *link);

    // void SetVesselPointsKDTree(PointIterator PointStart, PointIterator PointEnd);
    void SetVesselPointsKDTreeFromPolyData(vtkPolyData *surface);

    /// Check if point is inside the vessel: 1 inside, outside \n
    /// This version uses the surrounding cells of the nearest point.
    int IsPointInsideVessel1(const double *stentPoint, double *surfacePoint) const ;

    /// Check if point is inside the vessel: 1 inside, outside. \n
    /// This version uses the polydata normals.
    int IsPointInsideVessel2(const double *stentPoint, double *surfacePoint) const ;

    /// Check if point is inside the vessel: 1 inside, outside. \n
    /// This version uses the polydata normals.
    int IsPointInsideVessel2(const double *stentPoint, int surfacePtId) const ;

    double computeDirectionFromPoint2Face(double *stentPoint,vtkIdType p1,vtkIdType p2,vtkIdType p3);
    void SetCenterLocationIdx(std::vector<int>::const_iterator centerLocationIndex);
    void SetCenterLocationIdxRef(std::vector<int> const&ve);
    void SetTestValue(int value){this->m_TestValue = value;}
    void vectorSubtract(double *endPoint,double *startPoint,double *result);


    /// Get catheter position calculator
    const CatheterCalculator* GetCatheterCalculator() const {return m_CatheterCalculator ;}

    /// Get catheter position calculator
    CatheterCalculator* GetCatheterCalculator() {return m_CatheterCalculator ;}

    /// Get deformation info
    const DeformationHistory* GetDeformationHistory() const {return m_DeformationHistory ;}
    DeformationHistory* GetDeformationHistory() {return m_DeformationHistory ;}

    void SetMinDistanceToVessel(double dist) {m_MinDistanceToVessel = dist ;}

  protected:
    vtkALBADeformableSimplexMeshFilter();
    ~vtkALBADeformableSimplexMeshFilter();
    vtkALBADeformableSimplexMeshFilter(const Self &) {}
    void operator=(const Self &){}

    virtual void ComputeDisplacement();
    virtual void ComputeExternalForce(SimplexMeshGeometry *data);

    double m_StrutLength;
    double m_LinkLength;

  private:
    VectorType ComputeStrutLengthForce(SimplexMeshGeometry *data, int index);
    VectorType ComputeLinkLengthForce(SimplexMeshGeometry *data, int index);

    /// Is point x above cell (triangle only). \n
    /// 1 for above, 0 for in, -1 for below
    int IsPointAboveCell(vtkPolyData *polydata, int cellId, const double x[3]) const ;

    int m_CurrentStepNum;
    vtkPolyData *m_SurfacePoly; // pointer set from outside - not allocated here

    double m_Epsilon ; // distance parameter, defines "near distance" when approaching target

    double m_DistanceCoefficient;

    // [0],[1] strut neighbor; [2] link neighbor
    int (*m_StrutNeighbors)[3] ;

    kdtree *m_KDTree;

    std::vector<int>::const_iterator m_CenterLocationIdx;
    int m_IsDataChanged ;

    int m_TestValue;

    double m_MinDistanceToVessel ; // buffer distance to vessel wall at which stent should stop

    CatheterCalculator* m_CatheterCalculator ;
    DeformationHistory* m_DeformationHistory ;

  }; // end of class




  //-----------------------------------------------------------------------------
  /// CatheterCalculator. \n
  /// Helper class which calculates catheter position per step.
  //-----------------------------------------------------------------------------
  template< class TInputMesh, class TOutputMesh >
  class vtkALBADeformableSimplexMeshFilter<TInputMesh, TOutputMesh > 
    ::CatheterCalculator
  {
  public:
    /// Constructor
    CatheterCalculator() : m_StartPos(0.0), m_Speed(1.0), m_PauseType(PAUSE_NONE), m_CenterLine(NULL) {}

    /// Set start position of catheter
    void SetStartPos(double pos) {m_StartPos = pos;}

    /// Set speed of catheter withdrawal in mm/step
    void SetSpeed(double speed) {m_Speed = speed ;}

    /// Set no pause
    void SetPauseToNone() {m_PauseType = PAUSE_NONE ;}

    /// Set pause of length len at position start + p
    void SetPauseAtPosition(double p, double len) ;

    /// Set pause of length len at time t
    void SetPauseAtTime(double t, double len) ;

    /// Calculate position at time t
    double CalculatePosition(double t) const ;

    /// Set center line
    void SetCenterLine(vtkPolyData* centerLine) ;

    /// Get position of vertex along center line. \n
    /// If the index is outside the range, the position is extrapolated.
    double GetVertexPosition(int idx) const ;

    /// Is center line vertex still inside the catheter at time t.
    bool IsVertexInsideCatheter(int idx, double t) const ;

    /// Get index of first vertex which is still inside the catheter at time t. \n
    /// ie return the vertex which corresponds to the catheter position. \n
    /// Returns number larger than range if none found.
    int GetFirstVertexInsideCatheter(double t) const ;

    /// Create truncated center line, ie the part which is inside the catheter \n
    /// at time t, and which generates the catheter polydata. \n
    /// Returns true if successful.
    bool CreateTruncatedCenterLine(vtkPolyData* truncLine, double t) const ;

    /// Create subset of center line between vertices idFirst and idLast inclusive. \n
    /// Static utility which does not require any parameters set. \n
    /// Returns true if successful.
    bool static CreateTruncatedCenterLine(vtkPolyData* inputLine, vtkPolyData* truncLine, int idFirst, int idLast) ;

  private:
    double m_StartPos ;
    double m_Speed ;

    enum{
      PAUSE_NONE = 0,
      PAUSE_AT_POSITION,
      PAUSE_AT_TIME
    };

    int m_PauseType ;
    double m_PauseLength ;
    double m_PauseAtPosition ; // pause position relative to start
    double m_PauseAtTime ;

    vtkPolyData* m_CenterLine ;
    std::vector<double> m_VertexPositions ;  // cache distances of vertices along center line
  } ;




  //-----------------------------------------------------------------------------
  /// DeformationHistory. \n
  /// Helper class which saves deformation history for debugging purposes.
  //-----------------------------------------------------------------------------
  template< class TInputMesh, class TOutputMesh >
  class vtkALBADeformableSimplexMeshFilter<TInputMesh, TOutputMesh > 
    ::DeformationHistory
  {
  public:
    typedef std::vector<int> intList ;
    typedef std::vector<double> doubleList ;

    /// Constructor
    DeformationHistory() {} 

    /// Clear data
    void Clear() {m_IsInside.clear() ;  m_DistToTarget.clear() ;  m_Length.clear() ;}

    /// Save inside/outside status of point. \n
    /// outside = 0, inside = 1
    void SaveInside(int step, int isInside) ; 

    /// Save target distance of point
    void SaveDistToTarget(int step, double dist) ;

    /// Save lengths of cells
    void SaveLengthsOfCells(int step, const vtkALBAStentModelSource::SimplexMeshType *mesh) ;

    /// Print self
    void PrintSelf(ostream& os, int offset = 0) const ;


  private:
    /// Save cell length
    void SaveCellLength(int step, double length) ;

    // indexing is vector[ptId][step] or vector[cellId][step]
    std::vector<intList> m_IsInside ; // for each point: outside (0) or inside (1).  0 = inside, 1 = outside
    std::vector<doubleList> m_DistToTarget ; // for each point: distance to target
    std::vector<doubleList> m_Length ; // for each cell: length
  } ;



  //---------------------------------------------------------------------------
  // Is point x above cell (triangle only). \n
  // 1 for above, 0 for in, -1 for below
  //---------------------------------------------------------------------------
  template< typename TInputMesh, typename TOutputMesh >
  int vtkALBADeformableSimplexMeshFilter<TInputMesh, TOutputMesh >
    ::IsPointAboveCell(vtkPolyData *polydata, int cellId, const double x[3]) const
  {
    // check that cell is triangle
    vtkIdList *ids = vtkIdList::New() ;
    polydata->GetCellPoints(cellId, ids) ;
    int n = ids->GetNumberOfIds() ;
    assert(n == 3) ;
    
    // get normal of cell
    double p0[3], p1[3], p2[3], a[3], b[3], xp[3], s[3] ;
    polydata->GetPoint(ids->GetId(0), p0) ;
    polydata->GetPoint(ids->GetId(1), p1) ;
    polydata->GetPoint(ids->GetId(2), p2) ;
    for (int j = 0 ;  j < 3 ;  j++){
      a[j] = p1[j]-p0[j] ;
      b[j] = p2[j]-p0[j] ;
      xp[j] = x[j] - p0[j] ;
      vtkMath::Cross(a,b,s) ;
    }

    ids->Delete() ;

    // get dot product with normal and return sign
    double z = vtkMath::Dot(xp,s) ;
    if (z > m_MinDistanceToVessel)
      return 1 ;
    else if (z < -m_MinDistanceToVessel)
      return -1 ;
    else
      return 0 ;
  }



  //---------------------------------------------------------------------------
  // Is point inside the vessel.  1 inside, 0 outside 
  //---------------------------------------------------------------------------
  template< typename TInputMesh, typename TOutputMesh >
  int vtkALBADeformableSimplexMeshFilter< TInputMesh, TOutputMesh >
    ::IsPointInsideVessel1(const double *stentPoint, double *surfacePoint) const
  {
    int ptId = m_SurfacePoly->FindPoint(surfacePoint) ;
    vtkIdList *cellNeighbours = vtkIdList::New() ;
    m_SurfacePoly->GetPointCells(ptId, cellNeighbours) ;
    int inside = 1 ;
    for (int i = 0 ;  i < cellNeighbours->GetNumberOfIds() && inside == 1 ;  i++){
      int cellId = cellNeighbours->GetId(i) ;
      int aboveCell = IsPointAboveCell(m_SurfacePoly, cellId, stentPoint) ;
      // Point is outside if it is outside with respect to any one of the cells.
      // This should be ok as long as the vessel surface is concave.
      if ((aboveCell == 0) || (aboveCell == 1))
        inside = 0 ;
    }

    cellNeighbours->Delete() ;
    return inside ;
  }



  //---------------------------------------------------------------------------
  // Is point inside the vessel.  1 inside, 0 outside 
  //---------------------------------------------------------------------------
  template< typename TInputMesh, typename TOutputMesh >
  int vtkALBADeformableSimplexMeshFilter< TInputMesh, TOutputMesh >
    ::IsPointInsideVessel2(const double *stentPoint, double *surfacePoint) const
  {
    int ptId = m_SurfacePoly->FindPoint(surfacePoint) ;
    double *norm = m_SurfacePoly->GetPointData()->GetNormals()->GetTuple3(ptId) ;
    double dprod = 0.0 ;
    for (int j = 0 ;  j < 3 ;  j++)
      dprod += (surfacePoint[j] - stentPoint[j]) * norm[j] ;

    if (dprod < -m_MinDistanceToVessel)
      return 1 ;
    else
      return 0 ;
  }




  //---------------------------------------------------------------------------
  // Is point inside the vessel.  1 inside, 0 outside 
  //---------------------------------------------------------------------------
  template< typename TInputMesh, typename TOutputMesh >
  int vtkALBADeformableSimplexMeshFilter< TInputMesh, TOutputMesh >
    ::IsPointInsideVessel2(const double *stentPoint, int surfacePtId) const
  {
    double surfacePoint[3] ;
    m_SurfacePoly->GetPoint(surfacePtId, surfacePoint) ;
    double *norm = m_SurfacePoly->GetPointData()->GetNormals()->GetTuple3(surfacePtId) ;
    double dprod = 0.0 ;
    for (int j = 0 ;  j < 3 ;  j++)
      dprod += (surfacePoint[j] - stentPoint[j]) * norm[j] ;

    if (dprod < -m_MinDistanceToVessel)
      return 1 ;
    else
      return 0 ;
  }

} // namespace itk




#include "vtkALBADeformableSimplexMeshFilter_Impl.h"






#endif
