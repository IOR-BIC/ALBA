
#ifndef __vtkMEDDeformableSimplexMesh3DFilter_h 
#define __vtkMEDDeformableSimplexMesh3DFilter_h

//-------------------------------
// By Xiangyin Ma, Nov 2012
//-----------------------------------

//-----------------------------------------------------------------------------
/// vtkMEDDeformableSimplexMesh3DFilter \n
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
#include "vtkMEDStentModelSource.h"
//#include "VesselMeshForTesting.h" 
#include "kdtree.h"
#include <assert.h>
#include "vtkCellArray.h"
#include "vtkPolyData.h"
#include "vtkIdType.h"


namespace itk
{
  template< class TInputMesh, class TOutputMesh >
  class vtkMEDDeformableSimplexMeshFilter :
    public DeformableSimplexMesh3DFilter< TInputMesh, TOutputMesh >
  {
  public:
    /** Standard "Self" typedefs. */
    typedef vtkMEDDeformableSimplexMeshFilter Self;

    /** Standard "Superclass" typedef. */
    typedef  DeformableSimplexMesh3DFilter< TInputMesh, TOutputMesh > Superclass;

    /** Smart pointer typedef support */
    typedef SmartPointer< Self >       Pointer;
    typedef SmartPointer< const Self > ConstPointer;

    /** Method for creation through the object factory. */
    itkNewMacro(Self);

    /** Run-time type information (and related methods). */
    itkTypeMacro(vtkMEDDeformableSimplexMeshFilter, DeformableSimplexMesh3DFilter);

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

    typedef vtkMEDStentModelSource::Strut          Strut;
    typedef vector<Strut>::const_iterator    StrutIterator;
    typedef itk::Vector< double, 3 >         VectorType;

    //typedef VesselMeshForTesting::Point      Point;
    //typedef vector<Point>::const_iterator    PointIterator;

    itkSetMacro(StrutLength, double);
    itkGetConstMacro(StrutLength, double);

    itkSetMacro(LinkLength, double);
    itkGetConstMacro(LinkLength, double);

    itkSetMacro(CurIterationNum, int);
    itkGetConstMacro(CurIterationNum, int);

    itkSetMacro(Epsilon, double);
    itkGetConstMacro(Epsilon, double);

    void SetStrutLinkIter(StrutIterator StrutStart, StrutIterator StrutEnd,
      StrutIterator LinkStart, StrutIterator LinkEnd);

    void SetStrutLinkFromCellArray(vtkCellArray *strut,vtkCellArray *link);

    // void SetVesselPointsKDTree(PointIterator PointStart, PointIterator PointEnd);
    void SetVesselPointsKDTreeFromPolyData(vtkPolyData *surface);
    /*to check if point touch vessel wall from inside,1 inside ,0,outside */
    int isPointInsideVessel(double *stentPoint, double *surfacePoint) ;
    double computeDirectionFromPoint2Face(double *stentPoint,vtkIdType p1,vtkIdType p2,vtkIdType p3);
    void SetCenterLocationIdx(vector<int>::const_iterator centerLocationIndex);
    void SetCenterLocationIdxRef(vector<int> const&ve);
    void SetTestValue(int value){this->m_TestValue = value;}
    void vectorSubtract(double *endPoint,double *startPoint,double *result);

  protected:
    vtkMEDDeformableSimplexMeshFilter();
    ~vtkMEDDeformableSimplexMeshFilter();
    vtkMEDDeformableSimplexMeshFilter(const Self &) {}
    void operator=(const Self &){}

    virtual void ComputeDisplacement();
    virtual void ComputeExternalForce(SimplexMeshGeometry *data);

    double m_StrutLength;
    double m_LinkLength;


  private:
    VectorType ComputeStrutLengthForce(SimplexMeshGeometry *data, int index);
    VectorType ComputeLinkLengthForce(SimplexMeshGeometry *data, int index);

    int m_CurIterationNum;
    vtkPolyData *m_SurfacePoly; // pointer set from outside - not allocated here

    double m_Epsilon ; // distance parameter, defines "near distance" when approaching target

    double m_DistanceCoefficient;

    // [0],[1] strut neighbor; [2] link neighbor
    int (*m_StrutNeighbours)[3] ;

    kdtree *m_KDTree;

    vector<int>::const_iterator m_CenterLocationIdx;
    int m_IsDataChanged ;

    int m_TestValue;

  }; // end of class
} // namespace itk

#include "vtkMEDDeformableSimplexMeshFilter.txx"

#endif
