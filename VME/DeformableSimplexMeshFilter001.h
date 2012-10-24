#include "itkDeformableSimplexMesh3DFilter.h"
#include "itkMesh.h"
#include "itkConstNeighborhoodIterator.h"
#include "itkCovariantVector.h"
#include "StentModelSource.h"
//#include "VesselMeshForTesting.h" 
#include "kdtree.h"
#include <assert.h>


#include <set>
#include <vector>
using std::vector;

namespace itk
{
template< class TInputMesh, class TOutputMesh >
class DeformableSimplexMeshFilter001 :
  public DeformableSimplexMesh3DFilter< TInputMesh, TOutputMesh >
{
public:
  /** Standard "Self" typedefs. */
  typedef DeformableSimplexMeshFilter001 Self;

  /** Standard "Superclass" typedef. */
  typedef  DeformableSimplexMesh3DFilter< TInputMesh, TOutputMesh > Superclass;

  /** Smart pointer typedef support */
  typedef SmartPointer< Self >       Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(DeformableSimplexMeshFilter001, DeformableSimplexMesh3DFilter);

  /** Some typedefs. */
  typedef TInputMesh  InputMeshType;
  typedef TOutputMesh OutputMeshType;

  typedef typename Superclass::PointType              PointType;
  typedef typename Superclass::GradientIndexType      GradientIndexType;
  typedef typename Superclass::GradientIndexValueType GradientIndexValueType;
  typedef typename Superclass::GradientType           GradientType;
  typedef typename Superclass::GradientImageType      GradientImageType;

  /* Mesh pointer definition. */
  typedef typename InputMeshType::Pointer  InputMeshPointer;
  typedef typename OutputMeshType::Pointer OutputMeshPointer;

  typedef typename InputMeshType::PixelType PixelType;

  typedef Image< PixelType, 3 >                        GradientIntensityImageType;
  typedef typename GradientIntensityImageType::Pointer GradientIntensityImagePointer;

  typedef Image< float, 3 >                               OriginalImageType;
  typedef typename OriginalImageType::IndexType           OriginalImageIndexType;
  typedef typename OriginalImageIndexType::IndexValueType ImageIndexValueType;
  typedef typename OriginalImageType::ConstPointer        OriginalImagePointer;

  typedef StentModelSource::Strut          Strut;
  typedef vector<Strut>::const_iterator    StrutIterator;
  typedef itk::Vector< double, 3 >         VectorType;

  //typedef VesselMeshForTesting::Point      Point;
  //typedef vector<Point>::const_iterator    PointIterator;

  /** control the range of search for Bresenham at normal line */
  itkSetMacro(Range, int);
  itkGetConstMacro(Range, int);

  itkSetMacro(StrutLength, double);
  itkGetConstMacro(StrutLength, double);

  itkSetMacro(LinkLength, double);
  itkGetConstMacro(LinkLength, double);

  itkSetMacro(CurIterationNum, int);
  itkGetConstMacro(CurIterationNum, int);
  
  void SetStrutLinkIter(StrutIterator StrutStart, StrutIterator StrutEnd,
	                    StrutIterator LinkStart, StrutIterator LinkEnd);

  //void SetVesselPointsKDTree(PointIterator PointStart, PointIterator PointEnd);


  // full segment or half segment direction
  enum SIDE {
    // half segment in direction
    NORMAL,
    // half segment in -direction
    INVERSE,
    // complete segment
    BOTH
    };

protected:
  DeformableSimplexMeshFilter001();
  ~DeformableSimplexMeshFilter001();
  DeformableSimplexMeshFilter001(const Self &) {}
  void operator=(const Self &){}
  void PrintSelf(std::ostream & os, Indent indent) const;

  virtual void Initialize();
  virtual void ComputeDisplacement();

  /**
   * Compute the external force component
   */
  virtual void ComputeExternalForce(SimplexMeshGeometry *data);

  /**
   * Range of search for Bresenham algorithm (normal line at each vertex)
   */
  int m_Range;
  double m_StrutLength;
  double m_LinkLength;

 

private:
  VectorType ComputeStrutLengthForce(SimplexMeshGeometry *data, int index);
	//double dist_sq( double *a1, double *a2, int dims );
  //bool distanceFlag;

  double distanceCoefficient;

  // [0],[1] strut neighbor; [2] link neighbor
  int (*StrutNeighbors)[3] ;
  //double (*VesselPoints)[3];
  kdtree *KDTree;
  double (*closetPoint)[3];
  double *distance;

  int m_CurIterationNum;

}; // end of class
} // namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "DeformableSimplexMeshFilter001.hxx"
#endif
