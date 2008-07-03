/*=========================================================================
ToDo:
-	Needs to be restructured in line with vtk principles of on demand
    execution. Eg BuildLinks and BuildLocator on first call to Evaluate*?.
    Handle input modification correctly etc.
-   Drop internal triangle filter and just check cell type as encountered?.
-   Get working with CellLocator. Currently crashes every time, although
    a large number of cells are located successfully. Too hard to debug,
    maybe try a tiny dataset.
=========================================================================*/

#include "vtkObjectFactory.h"
#include "vtkMAFImplicitPolyData.h"
#include "vtkPolygon.h"

vtkCxxRevisionMacro(vtkMAFImplicitPolyData, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkMAFImplicitPolyData);

// Constructor
//----------------------------------------------------------------------------
vtkMAFImplicitPolyData::vtkMAFImplicitPolyData()
//----------------------------------------------------------------------------
{
	this->NoGradient[0] = 0.0;
	this->NoGradient[1] = 0.0;
	this->NoGradient[2] = 1.0;

	this->tri     = NULL;
	this->input   = NULL;
  this->locator = NULL;
  this->poly    = NULL;
  this->cells   = NULL;

  this->EvaluateBoundsSet=0;
}
//----------------------------------------------------------------------------
void vtkMAFImplicitPolyData::SetInput(vtkPolyData *input) 
//----------------------------------------------------------------------------
{
  if( this->input != input ) 
  {
    vtkDebugMacro( <<" setting Input to " << (void *)input );

	  // use a tringle filter on the polydata input
    // this is done to filter out lines and vertices to leave only
    // polygons which are required by this algorithm for cell normals
    if( this->tri == NULL ) 
    {
		  this->tri = vtkTriangleFilter::New();
		  this->tri->PassVertsOff();
		  this->tri->PassLinesOff();
    }
	  this->tri->SetInput( input );
    this->tri->Update();

    this->input = this->tri->GetOutput();
    this->input->BuildLinks();	// to enable calls to GetPointCells
	  this->NoValue = this->input->GetLength();

    if( this->locator != NULL ) this->locator->Delete();
    locator = PointLocator::New();
    this->locator->SetDataSet( this->input );
    this->locator->BuildLocator();

    if( this->cells   != NULL ) this->cells->Delete();
	  this->cells = vtkIdList::New();
    this->cells->SetNumberOfIds( this->input->GetNumberOfCells() );

    this->poly = vtkPolygon::New();
    this->Modified();
  }
}
// used to extend bounds of point locator
//----------------------------------------------------------------------------
void vtkMAFImplicitPolyData::SetEvaluateBounds( double eBounds[6] )
//----------------------------------------------------------------------------
{
  int i;
  for( i=0; i<6; i++ ) 
    this->EvaluateBounds[i] = eBounds[i];
  this->EvaluateBoundsSet = 1;
}
//----------------------------------------------------------------------------
unsigned long vtkMAFImplicitPolyData::GetMTime()
//----------------------------------------------------------------------------
{
  unsigned long mTime = this->vtkImplicitFunction::GetMTime();
  unsigned long inputMTime;

  if ( this->input != NULL )
  {
    this->input->Update ();
    inputMTime = this->input->GetMTime();
    mTime = ( inputMTime > mTime ? inputMTime : mTime );
  }

  return mTime;
}
// Destructor
//----------------------------------------------------------------------------
vtkMAFImplicitPolyData::~vtkMAFImplicitPolyData()
//----------------------------------------------------------------------------
{
  if( this->tri     != NULL ) this->tri->Delete();
  if( this->locator != NULL ) this->locator->Delete();
  if( this->poly    != NULL ) this->poly->Delete();
  if( this->cells   != NULL ) this->cells->Delete();
}
// Evaluate for point x[3].
// Method using combination of implicit planes
//----------------------------------------------------------------------------
double vtkMAFImplicitPolyData::EvaluateFunction(double x[3])
//----------------------------------------------------------------------------
{
	// See if data set with polygons has been specified
	if( this->input == NULL || input->GetNumberOfCells() == 0 ) 
  {
	  vtkErrorMacro(<<"No polygons to evaluate function!");
    return this->NoValue;
  }

	int cellNum, pid;
	vtkCell *cell;
	double dot, ret = -VTK_LARGE_FLOAT, cNormal[3], closestPoint[3];

    // get point id of closest point in data set
	pid = this->locator->FindClosestPoint( x );
	if( pid != -1 ) 
  {
    this->input->GetPoint( pid, closestPoint );
    // get cells it belongs to
		this->input->GetPointCells( pid, cells );
    // for each cell
  	for( cellNum=0; cellNum<cells->GetNumberOfIds(); cellNum++ ) 
    {
    	cell = this->input->GetCell( cells->GetId( cellNum ) );
      // get cell normal
      poly->ComputeNormal( cell->GetPoints(), cNormal );
      dot = ( cNormal[0]*(x[0]-closestPoint[0]) +
              cNormal[1]*(x[1]-closestPoint[1]) +
              cNormal[2]*(x[2]-closestPoint[2]) );

      if( dot > ret ) ret = dot;
    }
  }
  if( ret == -VTK_LARGE_FLOAT ) ret = NoValue;
  return ret;
}
// Evaluate function gradient at point x[3].
//----------------------------------------------------------------------------
void vtkMAFImplicitPolyData::EvaluateGradient( double x[3], double n[3] )
//----------------------------------------------------------------------------
{
	int i;
	// See if data set with polygons has been specified
	if( this->input == NULL || input->GetNumberOfCells() == 0 ) 
  {
	  vtkErrorMacro(<<"No polygons to evaluate gradient!");
    for( i=0; i<3; i++ ) n[i] = this->NoGradient[i];
    return;
  }

	int cellNum, pid;
	vtkCell *cell;
	double dot, ret = -VTK_LARGE_FLOAT, cNormal[3], closestPoint[3];

    // get point id of closest point in data set
	pid = this->locator->FindClosestPoint( x );
	if( pid != -1 ) 
  {
   	this->input->GetPoint( pid, closestPoint );
   	// get cells it belongs to
		this->input->GetPointCells( pid, cells );
   	// for each cell
 		for( cellNum=0; cellNum<cells->GetNumberOfIds(); cellNum++ ) 
    {
    	cell = this->input->GetCell( cells->GetId( cellNum ) );
      // get cell normal
      poly->ComputeNormal( cell->GetPoints(), cNormal );
      dot = ( cNormal[0]*(x[0]-closestPoint[0]) +
              cNormal[1]*(x[1]-closestPoint[1]) +
              cNormal[2]*(x[2]-closestPoint[2]) );

      if( dot > ret ) 
        for( i=0; i<3; i++ ) n[i] = cNormal[i];
    }
  }
  if( ret == -VTK_LARGE_FLOAT ) 
    for( i=0; i<3; i++ ) n[i] = this->NoGradient[i];
}
//----------------------------------------------------------------------------
void vtkMAFImplicitPolyData::PrintSelf(ostream& os, vtkIndent indent)
//----------------------------------------------------------------------------
{
  vtkImplicitFunction::PrintSelf(os,indent);

  os << indent << "No polydata Value: " << this->NoValue << "\n";
  os << indent << "No polydata Gradient: (" << this->NoGradient[0] << ", "
     << this->NoGradient[1] << ", " << this->NoGradient[2] << ")\n";

  if ( this->input )
    os << indent << "Input : " << this->input << "\n";
  else
    os << indent << "Input : (none)\n";
}
