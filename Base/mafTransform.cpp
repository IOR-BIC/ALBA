/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafTransform.cpp,v $
  Language:  C++
  Date:      $Date: 2005-02-20 23:33:18 $
  Version:   $Revision: 1.2 $
  Authors:   Marco Petrone, Stefano Perticoni,Stefania Paperini
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#include "mafTransform.h"
#include "mafMatrix.h"
#include "mafMatrix3x3.h"
#include "mafIndent.h"

#include <math.h>
#include <assert.h>
#include <ostream>

mafCxxTypeMacro(mafTransform)

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
//MFL_EVT_IMP(mafTransform::UpdateEvent); // Event rised by updates of the internal matrix

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafTransform::mafTransform()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
mafTransform::~mafTransform()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
mafTransform::mafTransform(const mafTransform& copy)
//----------------------------------------------------------------------------
{
  m_Matrix=copy.m_Matrix;
  Modified();
}

//----------------------------------------------------------------------------
void mafTransform::Identity()
//----------------------------------------------------------------------------
{
  m_Matrix.Identity();
  Modified();
}

//----------------------------------------------------------------------------
// Copied from vtkTransformConcatenation::Rotate(...)
void mafTransform::RotateWXYZ(const mafMatrix &source,mafMatrix &target,double angle,double x, double y, double z,int premultiply)
//----------------------------------------------------------------------------
{
  if (angle == 0.0 || (x == 0.0 && y == 0.0 && z == 0.0)) 
  {
    return;
  }

  // convert to radians
  angle = angle*mafMatrix3x3::DegreesToRadians();

  // make a normalized quaternion
  double w = cos(0.5*angle);
  double f = sin(0.5*angle)/sqrt(x*x+y*y+z*z);
  x *= f;
  y *= f;
  z *= f;

  
  // temporary matrix
  mafMatrix mat;
  mafMatrixElements matrix=mat.GetElements();

  // convert the quaternion to a matrix
  double ww = w*w;
  double wx = w*x;
  double wy = w*y;
  double wz = w*z;

  double xx = x*x;
  double yy = y*y;
  double zz = z*z;

  double xy = x*y;
  double xz = x*z;
  double yz = y*z;

  double s = ww - xx - yy - zz;

  matrix[0][0] = xx*2 + s;
  matrix[1][0] = (xy + wz)*2;
  matrix[2][0] = (xz - wy)*2;

  matrix[0][1] = (xy - wz)*2;
  matrix[1][1] = yy*2 + s;
  matrix[2][1] = (yz + wx)*2;

  matrix[0][2] = (xz + wy)*2;
  matrix[1][2] = (yz - wx)*2;
  matrix[2][2] = zz*2 + s;

  if (premultiply)
  {
    mafMatrix::Multiply4x4(source,mat,target);
  }
  else
  {
    mafMatrix::Multiply4x4(mat,source,target);
  }
  target.Modified();
}

//----------------------------------------------------------------------------
void mafTransform::Concatenate(const mafMatrix &matrix, int premultiply)
//----------------------------------------------------------------------------
{
  if(premultiply)
  {
    mafMatrix::Multiply4x4(m_Matrix, matrix, m_Matrix);
  }
  else
  {
    mafMatrix::Multiply4x4(matrix, m_Matrix, m_Matrix);
  }
  
  Modified();
}

//----------------------------------------------------------------------------
void mafTransform::SetOrientation(mafMatrix &matrix,double orientation[3])
//----------------------------------------------------------------------------
{
  double pos[3];

  mafTransform::GetPosition(matrix,pos);
  matrix.Identity();
  mafTransform::RotateZ(matrix,orientation[2], PRE_MULTIPLY);
  mafTransform::RotateX(matrix,orientation[0], PRE_MULTIPLY);
  mafTransform::RotateY(matrix,orientation[1], PRE_MULTIPLY);  
  mafTransform::SetPosition(matrix,pos);
}

//----------------------------------------------------------------------------
void mafTransform::SetPosition(mafMatrix &matrix,double position[3])
//----------------------------------------------------------------------------
{
	for (int i = 0; i < 3; i++)
	{
	  matrix.SetElement(i, 3, position[i]);
	}	
  matrix.Modified();
}

//----------------------------------------------------------------------------
void mafTransform::Translate(mafMatrix &matrix,double translation[3],int premultiply)
//----------------------------------------------------------------------------
{
  mafMatrix trans_matrix;
  SetPosition(trans_matrix,translation);
  if(premultiply)
  {
    mafMatrix::Multiply4x4(matrix, trans_matrix, matrix);
  }
  else
  {
    mafMatrix::Multiply4x4(trans_matrix, matrix, matrix);
  }
}

//----------------------------------------------------------------------------
void mafTransform::CopyRotation(const mafMatrix &source, mafMatrix &target)
//----------------------------------------------------------------------------
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
		  target.SetElement(i,j, source.GetElement(i,j));
		}
	}
}

//----------------------------------------------------------------------------
void mafTransform::CopyTranslation(const mafMatrix &source, mafMatrix &target)
//----------------------------------------------------------------------------
{
  for (int i = 0; i < 3; i++)
	{
		target.SetElement(i,3, source.GetElement(i,3));
	}
}

//----------------------------------------------------------------------------
double mafTransform::PolarDecomp(const mafMatrix &v_M, mafMatrix &v_Q, mafMatrix &v_S,
																double translation[3])
//----------------------------------------------------------------------------
{
double det;
int i, j;

for (i = 0; i < 3; i++)
{
translation[i] = v_M.GetElement(i, 3);
}

mafTransform::HMatrix M, Q, S;
for (i = 0; i < 3; i++)
	for (j = 0;j < 3; j++)
	M[i][j] = v_M.GetElement(i,j);

	det = PolarDecomp(M, Q, S);

for (i = 0; i < 4; i++)
	for (j = 0;j < 4; j++)
	{
	v_Q.SetElement(i, j, Q[i][j]);
	v_S.SetElement(i, j, S[i][j]);
	}

return det;
}


//----------------------------------------------------------------------------	
// Fill out 3x3 matrix to 4x4
#define mat_pad(A) (A[W][X]=A[X][W]=A[W][Y]=A[Y][W]=A[W][Z]=A[Z][W]=0,A[W][W]=1)
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Copy nxn matrix A to C using "gets" for assignment
#define mat_copy(C,gets,A,n) {int i,j; for(i=0;i<n;i++) for(j=0;j<n;j++)\
    C[i][j] gets (A[i][j]);}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Copy transpose of nxn matrix A to C using "gets" for assignment
#define mat_tpose(AT,gets,A,n) {int i,j; for(i=0;i<n;i++) for(j=0;j<n;j++)\
    AT[i][j] gets (A[j][i]);}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Assign nxn matrix C the element-wise combination of A and B using "op"
#define mat_binop(C,gets,A,op,B,n) {int i,j; for(i=0;i<n;i++) for(j=0;j<n;j++)\
    C[i][j] gets (A[i][j]) op (B[i][j]);}

//----------------------------------------------------------------------------
// Multiply the upper left 3x3 parts of A and B to get AB 
void mat_mult(mafTransform::HMatrix A, mafTransform::HMatrix B, mafTransform::HMatrix AB)
{
    int i, j;
    for (i=0; i<3; i++) for (j=0; j<3; j++)
	AB[i][j] = A[i][0]*B[0][j] + A[i][1]*B[1][j] + A[i][2]*B[2][j];
}

//----------------------------------------------------------------------------
// Return dot product of length 3 vectors va and vb
double vdot(double *va, double *vb)
//----------------------------------------------------------------------------
{
    return (va[0]*vb[0] + va[1]*vb[1] + va[2]*vb[2]);
}

//----------------------------------------------------------------------------
// Set v to cross product of length 3 vectors va and vb
void vcross(double *va, double *vb, double *v)
{
    v[0] = va[1]*vb[2] - va[2]*vb[1];
    v[1] = va[2]*vb[0] - va[0]*vb[2];
    v[2] = va[0]*vb[1] - va[1]*vb[0];
}

//----------------------------------------------------------------------------
// Set MadjT to transpose of inverse of M times determinant of M
void adjoint_transpose(mafTransform::HMatrix M, mafTransform::HMatrix MadjT)
//----------------------------------------------------------------------------
{
    vcross(M[1], M[2], MadjT[0]);
    vcross(M[2], M[0], MadjT[1]);
    vcross(M[0], M[1], MadjT[2]);
}

//----------------------------------------------------------------------------
//                ******* mmuQuaternion Preliminaries *******
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Construct a (possibly non-unit) quaternion from real components.
mafTransform::mmuQuat Qt_(double x, double y, double z, double w)
//----------------------------------------------------------------------------
{
    mafTransform::mmuQuat qq;
    qq.x = x; qq.y = y; qq.z = z; qq.w = w;
    return (qq);
}

//----------------------------------------------------------------------------
// Return conjugate of quaternion.
mafTransform::mmuQuat Qt_Conj(mafTransform::mmuQuat q)
//----------------------------------------------------------------------------
{
    mafTransform::mmuQuat qq;
    qq.x = -q.x; qq.y = -q.y; qq.z = -q.z; qq.w = q.w;
    return (qq);
}

//----------------------------------------------------------------------------
// Return quaternion product qL * qR.  Note: order is important!
// To combine rotations, use the product Mul(qSecond, qFirst),
// which gives the effect of rotating by qFirst then qSecond.
mafTransform::mmuQuat Qt_Mul(mafTransform::mmuQuat qL, mafTransform::mmuQuat qR)
//----------------------------------------------------------------------------
{
    mafTransform::mmuQuat qq;
    qq.w = qL.w*qR.w - qL.x*qR.x - qL.y*qR.y - qL.z*qR.z;
    qq.x = qL.w*qR.x + qL.x*qR.w + qL.y*qR.z - qL.z*qR.y;
    qq.y = qL.w*qR.y + qL.y*qR.w + qL.z*qR.x - qL.x*qR.z;
    qq.z = qL.w*qR.z + qL.z*qR.w + qL.x*qR.y - qL.y*qR.x;
    return (qq);
}

//----------------------------------------------------------------------------
// Return product of quaternion q by scalar w.
mafTransform::mmuQuat Qt_Scale(mafTransform::mmuQuat q, double w)
//----------------------------------------------------------------------------
{
    mafTransform::mmuQuat qq;
    qq.w = q.w*w; qq.x = q.x*w; qq.y = q.y*w; qq.z = q.z*w;
    return (qq);
}

//----------------------------------------------------------------------------
// Construct a unit quaternion from rotation matrix.  Assumes matrix is
// used to multiply column vector on the left: vnew = mat vold.	 Works
// correctly for right-handed coordinate system and right-handed rotations.
// Translation and perspective components ignored.
mafTransform::mmuQuat mafTransform::mmuQuaternionFromMatrix(HMatrix mat)
//----------------------------------------------------------------------------
{
    /* This algorithm avoids near-zero divides by looking for a large component
     * - first w, then x, y, or z.  When the trace is greater than zero,
     * |w| is greater than 1/2, which is as small as a largest component can be.
     * Otherwise, the largest diagonal entry corresponds to the largest of |x|,
     * |y|, or |z|, one of which must be larger than |w|, and at least 1/2. */
    mmuQuat qu;
    register double tr, s;

    tr = mat[X][X] + mat[Y][Y]+ mat[Z][Z];
    if (tr >= 0.0) {
	    s = sqrt(tr + mat[W][W]);
	    qu.w = s*0.5;
	    s = 0.5 / s;
	    qu.x = (mat[Z][Y] - mat[Y][Z]) * s;
	    qu.y = (mat[X][Z] - mat[Z][X]) * s;
	    qu.z = (mat[Y][X] - mat[X][Y]) * s;
	} else {
	    int h = X;
	    if (mat[Y][Y] > mat[X][X]) h = Y;
	    if (mat[Z][Z] > mat[h][h]) h = Z;
	    switch (h) {
#define caseMacro(i,j,k,I,J,K) \
	    case I:\
		s = sqrt( (mat[I][I] - (mat[J][J]+mat[K][K])) + mat[W][W] );\
		qu.i = s*0.5;\
		s = 0.5 / s;\
		qu.j = (mat[I][J] + mat[J][I]) * s;\
		qu.k = (mat[K][I] + mat[I][K]) * s;\
		qu.w = (mat[K][J] - mat[J][K]) * s;\
		break
	    caseMacro(x,y,z,X,Y,Z);
	    caseMacro(y,z,x,Y,Z,X);
	    caseMacro(z,x,y,Z,X,Y);
	    }
	}
    if (mat[W][W] != 1.0) qu = Qt_Scale(qu, 1/sqrt(mat[W][W]));
    return (qu);
}

//----------------------------------------------------------------------------
//                  ******* Decomp Auxiliaries *******
//----------------------------------------------------------------------------

static mafTransform::HMatrix mat_id = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};

/** Compute either the 1 or infinity norm of M, depending on tpose **/
double mat_norm(mafTransform::HMatrix M, int tpose)
{
    int i;
    double sum, max;
    max = 0.0;
    for (i=0; i<3; i++) {
	if (tpose) sum = fabs(M[0][i])+fabs(M[1][i])+fabs(M[2][i]);
	else	   sum = fabs(M[i][0])+fabs(M[i][1])+fabs(M[i][2]);
	if (max<sum) max = sum;
    }
    return max;
}

double norm_inf(mafTransform::HMatrix M) {return mat_norm(M, 0);}
double norm_one(mafTransform::HMatrix M) {return mat_norm(M, 1);}

//----------------------------------------------------------------------------
// Return index of column of M containing maximum abs entry, or -1 if M=0
//----------------------------------------------------------------------------
int find_max_col(mafTransform::HMatrix M)
//----------------------------------------------------------------------------
{
    double abs, max;
    int i, j, col;
    max = 0.0; col = -1;
    for (i=0; i<3; i++) for (j=0; j<3; j++) {
	abs = M[i][j]; if (abs<0.0) abs = -abs;
	if (abs>max) {max = abs; col = j;}
    }
    return col;
}

//----------------------------------------------------------------------------
// Setup u for Household reflection to zero all v components but first
void make_reflector(double *v, double *u)
//----------------------------------------------------------------------------
{
    double s = sqrt(vdot(v, v));
    u[0] = v[0]; u[1] = v[1];
    u[2] = v[2] + ((v[2]<0.0) ? -s : s);
    s = sqrt(2.0/vdot(u, u));
    u[0] = u[0]*s; u[1] = u[1]*s; u[2] = u[2]*s;
}

//----------------------------------------------------------------------------
// Apply Householder reflection represented by u to column vectors of M 
void reflect_cols(mafTransform::HMatrix M, double *u)
//----------------------------------------------------------------------------
{
    int i, j;
    for (i=0; i<3; i++) {
	double s = u[0]*M[0][i] + u[1]*M[1][i] + u[2]*M[2][i];
	for (j=0; j<3; j++) M[j][i] -= u[j]*s;
    }
}

//----------------------------------------------------------------------------
// Apply Householder reflection represented by u to row vectors of M
void reflect_rows(mafTransform::HMatrix M, double *u)
//----------------------------------------------------------------------------
{
    int i, j;
    for (i=0; i<3; i++) {
	double s = vdot(u, M[i]);
	for (j=0; j<3; j++) M[i][j] -= u[j]*s;
    }
}

//----------------------------------------------------------------------------
// Find orthogonal factor Q of rank 1 (or less) M
void do_rank1(mafTransform::HMatrix M, mafTransform::HMatrix Q)
//----------------------------------------------------------------------------
{
    double v1[3], v2[3], s;
    int col;
    mat_copy(Q,=,mat_id,4);
    /* If rank(M) is 1, we should find a non-zero column in M */
    col = find_max_col(M);
    if (col<0) return; /* Rank is 0 */
    v1[0] = M[0][col]; v1[1] = M[1][col]; v1[2] = M[2][col];
    make_reflector(v1, v1); reflect_cols(M, v1);
    v2[0] = M[2][0]; v2[1] = M[2][1]; v2[2] = M[2][2];
    make_reflector(v2, v2); reflect_rows(M, v2);
    s = M[2][2];
    if (s<0.0) Q[2][2] = -1.0;
    reflect_cols(Q, v1); reflect_rows(Q, v2);
}


//----------------------------------------------------------------------------
// Find orthogonal factor Q of rank 2 (or less) M using adjoint transpose
void do_rank2(mafTransform::HMatrix M, mafTransform::HMatrix MadjT, mafTransform::HMatrix Q)
//----------------------------------------------------------------------------
{
    double v1[3], v2[3];
    double w, x, y, z, c, s, d;
    int col;
    /* If rank(M) is 2, we should find a non-zero column in MadjT */
    col = find_max_col(MadjT);
    if (col<0) {do_rank1(M, Q); return;} /* Rank<2 */
    v1[0] = MadjT[0][col]; v1[1] = MadjT[1][col]; v1[2] = MadjT[2][col];
    make_reflector(v1, v1); reflect_cols(M, v1);
    vcross(M[0], M[1], v2);
    make_reflector(v2, v2); reflect_rows(M, v2);
    w = M[0][0]; x = M[0][1]; y = M[1][0]; z = M[1][1];
    if (w*z>x*y) {
	c = z+w; s = y-x; d = sqrt(c*c+s*s); c = c/d; s = s/d;
	Q[0][0] = Q[1][1] = c; Q[0][1] = -(Q[1][0] = s);
    } else {
	c = z-w; s = y+x; d = sqrt(c*c+s*s); c = c/d; s = s/d;
	Q[0][0] = -(Q[1][1] = c); Q[0][1] = Q[1][0] = s;
    }
    Q[0][2] = Q[2][0] = Q[1][2] = Q[2][1] = 0.0; Q[2][2] = 1.0;
    reflect_cols(Q, v1); reflect_rows(Q, v2);
}


//----------------------------------------------------------------------------
//                ******* Polar Decomposition *******
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Polar Decomposition of 3x3 matrix in 4x4,
// M = QS.  See Nicholas Higham and Robert S. Schreiber,
// Fast Polar Decomposition of An Arbitrary Matrix,
// Technical Report 88-942, October 1988,
// Department of Computer Science, Cornell University.
//----------------------------------------------------------------------------
double mafTransform::PolarDecomp(HMatrix M, HMatrix Q, HMatrix S)
//----------------------------------------------------------------------------
{
#define TOL 1.0e-6
    HMatrix Mk, MadjTk, Ek;
    double det, M_one, M_inf, MadjT_one, MadjT_inf, E_one, gamma, g1, g2;
    int i, j;
    mat_tpose(Mk,=,M,3);
    M_one = norm_one(Mk);  M_inf = norm_inf(Mk);
    do {
	adjoint_transpose(Mk, MadjTk);
	det = vdot(Mk[0], MadjTk[0]);
	if (det==0.0) {do_rank2(Mk, MadjTk, Mk); break;}
	MadjT_one = norm_one(MadjTk); MadjT_inf = norm_inf(MadjTk);
	gamma = sqrt(sqrt((MadjT_one*MadjT_inf)/(M_one*M_inf))/fabs(det));
	g1 = gamma*0.5;
	g2 = 0.5/(gamma*det);
	mat_copy(Ek,=,Mk,3);
	mat_binop(Mk,=,g1*Mk,+,g2*MadjTk,3);
	mat_copy(Ek,-=,Mk,3);
	E_one = norm_one(Ek);
	M_one = norm_one(Mk);  M_inf = norm_inf(Mk);
    } while (E_one>(M_one*TOL));
    mat_tpose(Q,=,Mk,3); mat_pad(Q);
    mat_mult(Mk, M, S);	 mat_pad(S);
    for (i=0; i<3; i++) for (j=i; j<3; j++)
	S[i][j] = S[j][i] = 0.5*(S[i][j]+S[j][i]);
    return (det);
}


//----------------------------------------------------------------------------
//            ******* Spectral Decomposition *******
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Compute the spectral decomposition of symmetric positive semi-definite S.
// Returns rotation in U and scale factors in result, so that if K is a diagonal
// matrix of the scale factors, then S = U K (U transpose). Uses Jacobi method.
// See Gene H. Golub and Charles F. Van Loan. Matrix Computations. Hopkins 1983.
//----------------------------------------------------------------------------
mafTransform::HVect mafTransform::SpectDecomp(HMatrix S, HMatrix U)
//----------------------------------------------------------------------------
{
    HVect kv;
    double Diag[3],OffD[3]; /* OffD is off-diag (by omitted index) */
    double g,h,fabsh,fabsOffDi,t,theta,c,s,tau,ta,OffDq,a,b;
    static char nxt[] = {Y,Z,X};
    int sweep, i, j;
    mat_copy(U,=,mat_id,4);
    Diag[X] = S[X][X]; Diag[Y] = S[Y][Y]; Diag[Z] = S[Z][Z];
    OffD[X] = S[Y][Z]; OffD[Y] = S[Z][X]; OffD[Z] = S[X][Y];
    for (sweep=20; sweep>0; sweep--) {
	double sm = fabs(OffD[X])+fabs(OffD[Y])+fabs(OffD[Z]);
	if (sm==0.0) break;
	for (i=Z; i>=X; i--) {
	    int p = nxt[i]; int q = nxt[p];
	    fabsOffDi = fabs(OffD[i]);
	    g = 100.0*fabsOffDi;
	    if (fabsOffDi>0.0) {
		h = Diag[q] - Diag[p];
		fabsh = fabs(h);
		if (fabsh+g==fabsh) {
		    t = OffD[i]/h;
		} else {
		    theta = 0.5*h/OffD[i];
		    t = 1.0/(fabs(theta)+sqrt(theta*theta+1.0));
		    if (theta<0.0) t = -t;
		}
		c = 1.0/sqrt(t*t+1.0); s = t*c;
		tau = s/(c+1.0);
		ta = t*OffD[i]; OffD[i] = 0.0;
		Diag[p] -= ta; Diag[q] += ta;
		OffDq = OffD[q];
		OffD[q] -= s*(OffD[p] + tau*OffD[q]);
		OffD[p] += s*(OffDq   - tau*OffD[p]);
		for (j=Z; j>=X; j--) {
		    a = U[j][p]; b = U[j][q];
		    U[j][p] -= s*(b + tau*a);
		    U[j][q] += s*(a - tau*b);
		}
	    }
	}
    }
    kv.x = Diag[X]; kv.y = Diag[Y]; kv.z = Diag[Z]; kv.w = 1.0;
    return (kv);
}

//----------------------------------------------------------------------------
//          ******* Spectral Axis Adjustment *******
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Given a unit quaternion, q, and a scale vector, k, find a unit quaternion, p,
// which permutes the axes and turns freely in the plane of duplicate scale
// factors, such that q p has the largest possible w component, i.e. the
// smallest possible angle. Permutes k's components to go with q p instead of q.
// See Ken Shoemake and Tom Duff. Matrix Animation and Polar Decomposition.
// Proceedings of Graphics Interface 1992. Details on p. 262-263.
//----------------------------------------------------------------------------
mafTransform::mmuQuat mafTransform::Snuggle(mmuQuat q, HVect *k)
//----------------------------------------------------------------------------
{
#define SQRTHALF (0.7071067811865475244)
#define sgn(n,v)    ((n)?-(v):(v))
#define swap(a,i,j) {a[3]=a[i]; a[i]=a[j]; a[j]=a[3];}
#define cycle(a,p)  if (p) {a[3]=a[0]; a[0]=a[1]; a[1]=a[2]; a[2]=a[3];}\
		    else   {a[3]=a[2]; a[2]=a[1]; a[1]=a[0]; a[0]=a[3];}
    mmuQuat p;
    double ka[4];
    int i, turn = -1;
    ka[X] = k->x; ka[Y] = k->y; ka[Z] = k->z;
    if (ka[X]==ka[Y]) {if (ka[X]==ka[Z]) turn = W; else turn = Z;}
    else {if (ka[X]==ka[Z]) turn = Y; else if (ka[Y]==ka[Z]) turn = X;}
    if (turn>=0) {
	mmuQuat qtoz, qp;
	unsigned neg[3], win;
	double mag[3], t;
	static mmuQuat qxtoz = {0,SQRTHALF,0,SQRTHALF};
	static mmuQuat qytoz = {SQRTHALF,0,0,SQRTHALF};
	static mmuQuat qppmm = { 0.5, 0.5,-0.5,-0.5};
	static mmuQuat qpppp = { 0.5, 0.5, 0.5, 0.5};
	static mmuQuat qmpmm = {-0.5, 0.5,-0.5,-0.5};
	static mmuQuat qpppm = { 0.5, 0.5, 0.5,-0.5};
	static mmuQuat q0001 = { 0.0, 0.0, 0.0, 1.0};
	static mmuQuat q1000 = { 1.0, 0.0, 0.0, 0.0};
	switch (turn) {
	default: return (Qt_Conj(q));
	case X: q = Qt_Mul(q, qtoz = qxtoz); swap(ka,X,Z) break;
	case Y: q = Qt_Mul(q, qtoz = qytoz); swap(ka,Y,Z) break;
	case Z: qtoz = q0001; break;
	}
	q = Qt_Conj(q);
	mag[0] = (double)q.z*q.z+(double)q.w*q.w-0.5;
	mag[1] = (double)q.x*q.z-(double)q.y*q.w;
	mag[2] = (double)q.y*q.z+(double)q.x*q.w;
	for (i=0; i<3; i++) if (neg[i] = (mag[i]<0.0)) mag[i] = -mag[i];
	if (mag[0]>mag[1]) {if (mag[0]>mag[2]) win = 0; else win = 2;}
	else		   {if (mag[1]>mag[2]) win = 1; else win = 2;}
	switch (win) {
	case 0: if (neg[0]) p = q1000; else p = q0001; break;
	case 1: if (neg[1]) p = qppmm; else p = qpppp; cycle(ka,0) break;
	case 2: if (neg[2]) p = qmpmm; else p = qpppm; cycle(ka,1) break;
	}
	qp = Qt_Mul(q, p);
	t = sqrt(mag[win]+0.5);
	p = Qt_Mul(p, Qt_(0.0,0.0,-qp.z/t,qp.w/t));
	p = Qt_Mul(qtoz, Qt_Conj(p));
    } else {
	double qa[4], pa[4];
	unsigned lo, hi, neg[4], par = 0;
	double all, big, two;
	qa[0] = q.x; qa[1] = q.y; qa[2] = q.z; qa[3] = q.w;
	for (i=0; i<4; i++) {
	    pa[i] = 0.0;
	    if (neg[i] = (qa[i]<0.0)) qa[i] = -qa[i];
	    par ^= neg[i];
	}
	/* Find two largest components, indices in hi and lo */
	if (qa[0]>qa[1]) lo = 0; else lo = 1;
	if (qa[2]>qa[3]) hi = 2; else hi = 3;
	if (qa[lo]>qa[hi]) {
	    if (qa[lo^1]>qa[hi]) {hi = lo; lo ^= 1;}
	    else {hi ^= lo; lo ^= hi; hi ^= lo;}
	} else {if (qa[hi^1]>qa[lo]) lo = hi^1;}
	all = (qa[0]+qa[1]+qa[2]+qa[3])*0.5;
	two = (qa[hi]+qa[lo])*SQRTHALF;
	big = qa[hi];
	if (all>two) {
	    if (all>big) {/*all*/
		{int i; for (i=0; i<4; i++) pa[i] = sgn(neg[i], 0.5);}
		cycle(ka,par)
	    } else {/*big*/ pa[hi] = sgn(neg[hi],1.0);}
	} else {
	    if (two>big) {/*two*/
		pa[hi] = sgn(neg[hi],SQRTHALF); pa[lo] = sgn(neg[lo], SQRTHALF);
		if (lo>hi) {hi ^= lo; lo ^= hi; hi ^= lo;}
		if (hi==W) {hi = "\001\002\000"[lo]; lo = 3-hi-lo;}
		swap(ka,hi,lo)
	    } else {/*big*/ pa[hi] = sgn(neg[hi],1.0);}
	}
	p.x = -pa[0]; p.y = -pa[1]; p.z = -pa[2]; p.w = pa[3];
    }
    k->x = ka[X]; k->y = ka[Y]; k->z = ka[Z];
    return (p);
}

//----------------------------------------------------------------------------
//          ******* Decompose Affine Matrix *******
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Decompose 4x4 affine matrix A as TFRUK(U transpose), where t contains the
// translation components, q contains the rotation R, u contains U, k contains
// scale factors, and f contains the sign of the determinant.
// Assumes A transforms column vectors in right-handed coordinates.
// See Ken Shoemake and Tom Duff. Matrix Animation and Polar Decomposition.
// Proceedings of Graphics Interface 1992.
//----------------------------------------------------------------------------
void mafTransform::DecompAffine(HMatrix A, mmuAffineParts *parts)
//----------------------------------------------------------------------------
{
    mafTransform::HMatrix Q, S, U;
    mmuQuat p;
    double det;
    parts->t = Qt_(A[X][W], A[Y][W], A[Z][W], 0);
    det = PolarDecomp(A, Q, S);
    if (det<0.0) {
	mat_copy(Q,=,-Q,3);
	parts->f = -1;
    } else parts->f = 1;
    parts->q = mmuQuaternionFromMatrix(Q);
    parts->k = SpectDecomp(S, U);
    parts->u = mmuQuaternionFromMatrix(U);
    p = Snuggle(parts->u, &parts->k);
    parts->u = Qt_Mul(parts->u, p);
}

//----------------------------------------------------------------------------
//            ******* Invert Affine Decomposition *******
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Compute inverse of affine decomposition.
void mafTransform::InvertAffine(mmuAffineParts *parts, mmuAffineParts *inverse)
//----------------------------------------------------------------------------
{
    mmuQuat t, p;
    inverse->f = parts->f;
    inverse->q = Qt_Conj(parts->q);
    inverse->u = Qt_Mul(parts->q, parts->u);
    inverse->k.x = (parts->k.x==0.0) ? 0.0 : 1.0/parts->k.x;
    inverse->k.y = (parts->k.y==0.0) ? 0.0 : 1.0/parts->k.y;
    inverse->k.z = (parts->k.z==0.0) ? 0.0 : 1.0/parts->k.z;
    inverse->k.w = parts->k.w;
    t = Qt_(-parts->t.x, -parts->t.y, -parts->t.z, 0);
    t = Qt_Mul(Qt_Conj(inverse->u), Qt_Mul(t, inverse->u));
    t = Qt_(inverse->k.x*t.x, inverse->k.y*t.y, inverse->k.z*t.z, 0);
    p = Qt_Mul(inverse->q, inverse->u);
    t = Qt_Mul(p, Qt_Mul(t, Qt_Conj(p)));
    inverse->t = (inverse->f>0.0) ? t : Qt_(-t.x, -t.y, -t.z, 0);
}

//----------------------------------------------------------------------------
// Conversion functions:
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// copied from vtkTransform::GetOrientation
// Get the x, y, z orientation angles from the transformation matrix as an
// array of three floating point values.
void mafTransform::GetOrientation(const mafMatrix &in_matrix,double orientation[3])
//----------------------------------------------------------------------------
{
#define MAF_AXIS_EPSILON 0.001
  int i;

  // convenient access to matrix
  double (*matrix)[4] = in_matrix.GetElements();
  double ortho[3][3];

  for (i = 0; i < 3; i++)
    {
    ortho[0][i] = matrix[0][i];
    ortho[1][i] = matrix[1][i];
    ortho[2][i] = matrix[2][i];
    }
  if (mafMatrix3x3::Determinant(ortho) < 0)
    {
    ortho[0][2] = -ortho[0][2];
    ortho[1][2] = -ortho[1][2];
    ortho[2][2] = -ortho[2][2];
    }

  mafMatrix3x3::Orthogonalize(ortho, ortho);

  // first rotate about y axis
  double x2 = ortho[2][0];
  double y2 = ortho[2][1];
  double z2 = ortho[2][2];

  double x3 = ortho[1][0];
  double y3 = ortho[1][1];
  double z3 = ortho[1][2];

  double d1 = sqrt(x2*x2 + z2*z2);

  double cosTheta, sinTheta;
  if (d1 < MAF_AXIS_EPSILON) 
    {
    cosTheta = 1.0;
    sinTheta = 0.0;
    }
  else 
    {
    cosTheta = z2/d1;
    sinTheta = x2/d1;
    }

  double theta = atan2(sinTheta, cosTheta);
  orientation[1] = -theta/mafMatrix3x3::DegreesToRadians();

  // now rotate about x axis
  double d = sqrt(x2*x2 + y2*y2 + z2*z2);

  double sinPhi, cosPhi;
  if (d < MAF_AXIS_EPSILON) 
    {
    sinPhi = 0.0;
    cosPhi = 1.0;
    }
  else if (d1 < MAF_AXIS_EPSILON) 
    {
    sinPhi = y2/d;
    cosPhi = z2/d;
    }
  else 
    {
    sinPhi = y2/d;
    cosPhi = (x2*x2 + z2*z2)/(d1*d);
    }

  double phi = atan2(sinPhi, cosPhi);
  orientation[0] = phi/mafMatrix3x3::DegreesToRadians();

  // finally, rotate about z
  double x3p = x3*cosTheta - z3*sinTheta;
  double y3p = - sinPhi*sinTheta*x3 + cosPhi*y3 - sinPhi*cosTheta*z3;
  double d2 = sqrt(x3p*x3p + y3p*y3p);

  double cosAlpha, sinAlpha;
  if (d2 < MAF_AXIS_EPSILON) 
    {
    cosAlpha = 1.0;
    sinAlpha = 0.0;
    }
  else 
    {
    cosAlpha = y3p/d2;
    sinAlpha = x3p/d2;
    }

  double alpha = atan2(sinAlpha, cosAlpha);
  orientation[2] = alpha/mafMatrix3x3::DegreesToRadians();
}

//----------------------------------------------------------------------------
void mafTransform::GetOrientation(const mafMatrix &in_matrix,float orientation[3])
//----------------------------------------------------------------------------
{
  double temp[3];
  GetOrientation(in_matrix,temp);
  orientation[0]=temp[0];
  orientation[1]=temp[1];
  orientation[2]=temp[2];
}

//----------------------------------------------------------------------------
// copied from vtkTransform::GetOrientationWXYZ 
void mafTransform::GetOrientationWXYZ(const mafMatrix &in_matrix, double wxyz[4])
//----------------------------------------------------------------------------
{
  int i;

  // convenient access to matrix
  double (*matrix)[4] = in_matrix.GetElements();
  double ortho[3][3];

  for (i = 0; i < 3; i++)
    {
    ortho[0][i] = matrix[0][i];
    ortho[1][i] = matrix[1][i];
    ortho[2][i] = matrix[2][i];
    }
  if (mafMatrix3x3::Determinant(ortho) < 0)
    {
    ortho[0][i] = -ortho[0][i];
    ortho[1][i] = -ortho[1][i];
    ortho[2][i] = -ortho[2][i];
    }

  mafMatrix3x3::MatrixTommuQuaternion(ortho, wxyz);

  // calc the return value wxyz
 double mag = sqrt(wxyz[1]*wxyz[1] + wxyz[2]*wxyz[2] + wxyz[3]*wxyz[3]);

  if (mag)
    {
    wxyz[0] = 2.0*acos(wxyz[0])/mafMatrix3x3::DegreesToRadians();
    wxyz[1] /= mag;
    wxyz[2] /= mag;
    wxyz[3] /= mag;
    }
  else
    {
    wxyz[0] = 0.0;
    wxyz[1] = 0.0;
    wxyz[2] = 0.0;
    wxyz[3] = 1.0;
    }
}

//----------------------------------------------------------------------------
void mafTransform::GetPosition(const mafMatrix &matrix,double position[3])
//----------------------------------------------------------------------------
{
  position[0] = matrix.GetElements()[0][3];
  position[1] = matrix.GetElements()[1][3];
  position[2] = matrix.GetElements()[2][3];
}

//----------------------------------------------------------------------------
void mafTransform::GetPosition(const mafMatrix &matrix,float position[3])
//----------------------------------------------------------------------------
{
  position[0] = matrix.GetElements()[0][3];
  position[1] = matrix.GetElements()[1][3];
  position[2] = matrix.GetElements()[2][3];
}

//----------------------------------------------------------------------------
void mafTransform::GetScale(const mafMatrix &in_matrix,double scale[3])
//----------------------------------------------------------------------------
{
  // convenient access to matrix
  double (*matrix)[4] = in_matrix.GetElements();
  double U[3][3], VT[3][3];

  for (int i = 0; i < 3; i++) 
    {
    U[0][i] = matrix[0][i];
    U[1][i] = matrix[1][i];
    U[2][i] = matrix[2][i];
    }

  mafMatrix3x3::SingularValueDecomposition(U, U, scale, VT);
}

//----------------------------------------------------------------------------
void mafTransform::Scale(mafMatrix &matrix,double scalex,double scaley,double scalez,int premult)
//----------------------------------------------------------------------------
{
  mafMatrix tmp;
  if (scalex == 1.0 && scaley == 1.0 && scalez == 1.0) 
  {
    return;
  }

  tmp.SetElement(0,0,scalex);
  tmp.SetElement(1,1,scaley);
  tmp.SetElement(2,2,scalez);
  
  if (premult)
    mafMatrix::Multiply4x4(matrix,tmp,matrix);
  else
    mafMatrix::Multiply4x4(tmp,matrix,matrix);
}

//----------------------------------------------------------------------------
inline double sign(double a) 
//----------------------------------------------------------------------------
{
  return (a>0) ? 1.0 : ((a<0) ? -1.0 : 0.0);
}

//----------------------------------------------------------------------------
int mafTransform::MatrixToAttitudeVector(const mafMatrix &matrix,
											  double attitude_vector[3])
//----------------------------------------------------------------------------
{
	double c,s,first_diagonal,second_diagonal,third_diagonal,alpha;
	
	c =	(matrix.GetElement(0,0)+matrix.GetElement(1,1)+matrix.GetElement(2,2)-1)/2.0;
	
	third_diagonal	= matrix.GetElement(2,1)-matrix.GetElement(1,2);
	second_diagonal = matrix.GetElement(2,0)-matrix.GetElement(0,2);
	first_diagonal	= matrix.GetElement(1,0)-matrix.GetElement(0,1);


	s =	sqrt((third_diagonal*third_diagonal)+(second_diagonal*second_diagonal)+(first_diagonal*first_diagonal))/2.0;

	alpha = atan2(s,c);

	// attitude_vector is the Orientation Vector [rad] 
	attitude_vector[0] = (matrix.GetElement(2,1)-matrix.GetElement(1,2))*alpha/(2*sin(alpha));
	attitude_vector[1] = (matrix.GetElement(0,2)-matrix.GetElement(2,0))*alpha/(2*sin(alpha));
	attitude_vector[2] = (matrix.GetElement(1,0)-matrix.GetElement(0,1))*alpha/(2*sin(alpha));

	
	//modified by Stefano. 1-7-2003 (beg)
	//conversion to degrees
	attitude_vector[0] = attitude_vector[0] * mafMatrix3x3::RadiansToDegrees (); 
	attitude_vector[1] = attitude_vector[1] * mafMatrix3x3::RadiansToDegrees ();
	attitude_vector[2] = attitude_vector[2] * mafMatrix3x3::RadiansToDegrees ();
	//modified by Stefano. 1-7-2003 (end)

	return 1;
	
}		

//----------------------------------------------------------------------------
int mafTransform::MatrixToEulerCardanicAngle(const mafMatrix &matrix,
												 int i,int j,int k,
												 double euler_cardan[3],
												 double tentative_euler_cardan_first = 0,
												 double tentative_euler_cardan_second = 0,
												 double tentative_euler_cardan_third = 0)
//----------------------------------------------------------------------------
{

	// Extracts the Euler and Cardan angles from a rotation matrix.
	// The  parameters  i, j, k  specify the sequence of the rotation axes 
	// their value must be the constant:X,Y or Z. 
	// j must be different from i and k, 
	// if k is equal to i ->Euler angles
	// if k is different from i ->Cardan angles


	// The two solutions are stored in the three-element vectors 
	// m_first_euler_cardan and m_second_euler_cardan

	//modified by Stefano. 1-7-2003 (beg)
	//converison from degree to radians
	tentative_euler_cardan_first = tentative_euler_cardan_first * mafMatrix3x3::DegreesToRadians();
	tentative_euler_cardan_second = tentative_euler_cardan_second * mafMatrix3x3::DegreesToRadians();	
	tentative_euler_cardan_third =	tentative_euler_cardan_third * mafMatrix3x3::DegreesToRadians();
	//modified by Stefano. 1-7-2003 (end)	

	double PI = mafMatrix3x3::Pi();

	double first_euler_cardan[3];
	double second_euler_cardan[3];

	if ( i<X || i>Z || j<X || j>Z || k<X || k>Z || i==j || j==k )
	{
		//vtkErrorMacro(<<"Illegal rotation axis");
		return 0;

	}	


	int sig;

	if ((( (j-i+3) % 3 ) ) == 1  && ((j-i+3) > 0) )                  
	{
			sig=1;  // ciclic 
	}	

	else		sig=-1;	// anti ciclic



	if (i!=k)	// Cardan Angles 
	{		
		first_euler_cardan[0]= atan2(-sig*matrix.GetElement(j,k),matrix.GetElement(k,k));
		first_euler_cardan[1]= asin(sig*matrix.GetElement(i,k));
		first_euler_cardan[2]= atan2(-sig*matrix.GetElement(i,j),matrix.GetElement(i,i));

		second_euler_cardan[0]= atan2(sig*matrix.GetElement(j,k),-matrix.GetElement(k,k));
		int division = (double)((PI-asin(sig*matrix.GetElement(i,k)) + PI) / (2.0*PI));
		double remainder = (PI-asin(sig*matrix.GetElement(i,k)) + PI)-(division * (2*PI));
		second_euler_cardan[1]= remainder - PI; 
		second_euler_cardan[2]= atan2(sig*matrix.GetElement(i,j),-matrix.GetElement(i,i));

	}



	else		// Euler Angles
	{

		int l=3-i-j;

		first_euler_cardan[0]= atan2(matrix.GetElement(j,i),-sig*matrix.GetElement(l,i));
		first_euler_cardan[1]= acos(matrix.GetElement(i,i));
		first_euler_cardan[2]= atan2(matrix.GetElement(i,j),sig*matrix.GetElement(i,l));

		second_euler_cardan[0]= atan2(-matrix.GetElement(j,i),sig*matrix.GetElement(l,i));
		second_euler_cardan[1]= -acos(matrix.GetElement(i,i));
		second_euler_cardan[2]= atan2(-matrix.GetElement(i,j),-sig*matrix.GetElement(i,l));

	}

  //modified by STEFY 18-6-2003 (begin)

	double a = (first_euler_cardan[0]-tentative_euler_cardan_first); 
	double b = (first_euler_cardan[1]-tentative_euler_cardan_second); 
	double c = (first_euler_cardan[2]-tentative_euler_cardan_third); 

	double first_distance = sqrt(a*a+b*b+c*c);

	double d = (second_euler_cardan[0]-tentative_euler_cardan_first); 
	double e = (second_euler_cardan[1]-tentative_euler_cardan_second); 
	double f = (second_euler_cardan[2]-tentative_euler_cardan_third); 

	double second_distance = sqrt(d*d+e*e+f*f);

	if (first_distance < second_distance)
		{
			euler_cardan[0] = first_euler_cardan[0];
			euler_cardan[1] = first_euler_cardan[1];
			euler_cardan[2] = first_euler_cardan[2];
		}
	else 
		{
			euler_cardan[0] = second_euler_cardan[0];
			euler_cardan[1] = second_euler_cardan[1];
			euler_cardan[2] = second_euler_cardan[2];
		}

	//modified by STEFY 18-6-2003 (end)

	//modified by Stefano. 1-7-2003 (beg)
	//converison to degree
	euler_cardan[0] = euler_cardan[0] * mafMatrix3x3::RadiansToDegrees ();	
	euler_cardan[1] = euler_cardan[1] * mafMatrix3x3::RadiansToDegrees ();
	euler_cardan[2] = euler_cardan[2] * mafMatrix3x3::RadiansToDegrees ();
	//modified by Stefano. 1-7-2003 (end)	

	return 1;
  
}	
		
//----------------------------------------------------------------------------
int mafTransform::MatrixTommuQuaternion(const mafMatrix &matrix,
										  double quaternion[4])
//----------------------------------------------------------------------------
{

	// Scalar coordinate of the quaternion:							q0 = quaternion[0]
  	// First coordinate of the vectorial part of the quaternion:	q1 = quaternion[1]
  	// Second coordinate of the vectorial part of the quaternion:	q2 = quaternion[2]
  	// Third coordinate of the vectorial part of the quaternion:	q3 = quaternion[3] 


	double s = matrix.GetElement(0,0) + matrix.GetElement(1,1) + matrix.GetElement(2,2);
	if (s > -0.19) 
		{
			// compute quaternion[0] and deduce quaternion[1], quaternion[2] and quaternion[3]
			quaternion[0] = 0.5 * sqrt(s + 1.0);
			double inv = 0.25 / quaternion[0];
			quaternion[1] = inv * (matrix.GetElement(1,2) - matrix.GetElement(2,1));
			quaternion[2] = inv * (matrix.GetElement(2,0) - matrix.GetElement(0,2));
			quaternion[3] = inv * (matrix.GetElement(0,1) - matrix.GetElement(1,0));
		} 
	else 
		{
			s = matrix.GetElement(0,0) - matrix.GetElement(1,1) - matrix.GetElement(2,2);
			if (s > -0.19) 
				{
					// compute quaternion[1] and deduce quaternion[0], quaternion[2] and quaternion[3]
					quaternion[1] = 0.5 * sqrt(s + 1.0);
					double inv = 0.25 / quaternion[1];
					quaternion[0] = inv * (matrix.GetElement(1,2) - matrix.GetElement(2,1));
					quaternion[2] = inv * (matrix.GetElement(0,1) + matrix.GetElement(1,0));
					quaternion[3] = inv * (matrix.GetElement(0,2) + matrix.GetElement(2,0));
				} 
			else 
				{
					s = matrix.GetElement(1,1) - matrix.GetElement(0,0) - matrix.GetElement(2,2);
					if (s > -0.19) 
						{
							// compute quaternion[2] and deduce quaternion[0], quaternion[1] and quaternion[3]
							quaternion[2] = 0.5 * sqrt(s + 1.0);
							double inv = 0.25 / quaternion[2];
							quaternion[0] = inv * (matrix.GetElement(2,0) - matrix.GetElement(0,2));
							quaternion[1] = inv * (matrix.GetElement(0,1) + matrix.GetElement(1,0));
							quaternion[3] = inv * (matrix.GetElement(2,1) + matrix.GetElement(1,2));
						} 
					else 
						{
							// compute quaternion[3] and deduce quaternion[0], quaternion[1] and quaternion[2]
							s = matrix.GetElement(2,2) - matrix.GetElement(0,0) - matrix.GetElement(1,1);
							quaternion[3] = 0.5 * sqrt(s + 1.0);
							double inv = 0.25 / quaternion[3];
							quaternion[0] = inv * (matrix.GetElement(0,1) - matrix.GetElement(1,0));
							quaternion[1] = inv * (matrix.GetElement(0,2) + matrix.GetElement(2,0));
							quaternion[2] = inv * (matrix.GetElement(2,1) + matrix.GetElement(1,2));
						}
				 }
			}

	return 1;

}

//----------------------------------------------------------------------------
int mafTransform::mmuQuaternionToMatrix(double quaternion[4],
										  mafMatrix &matrix)
//----------------------------------------------------------------------------
{


	double q0 = quaternion[0];
    double q1 = quaternion[1];
    double q2 = quaternion[2];
	double q3 = quaternion[3];



    // products
    double q0q0  = q0 * q0;
    double q0q1  = q0 * q1;
    double q0q2  = q0 * q2;
    double q0q3  = q0 * q3;
    double q1q1  = q1 * q1;
    double q1q2  = q1 * q2;
    double q1q3  = q1 * q3;
    double q2q2  = q2 * q2;
    double q2q3  = q2 * q3;
    double q3q3  = q3 * q3;

    // create the matrix
    
    double m00  = 2.0 * (q0q0 + q1q1) - 1.0;
    double m10  = 2.0 * (q1q2 - q0q3);
    double m20  = 2.0 * (q1q3 + q0q2);

    double m01  = 2.0 * (q1q2 + q0q3);
    double m11  = 2.0 * (q0q0 + q2q2) - 1.0;
    double m21  = 2.0 * (q2q3 - q0q1);

    double m02  = 2.0 * (q1q3 - q0q2);
    double m12  = 2.0 * (q2q3 + q0q1);
    double m22  = 2.0 * (q0q0 + q3q3) - 1.0;

    


	matrix.SetElement(0, 0, m00); 
	matrix.SetElement(0, 1, m01); 
	matrix.SetElement(0, 2, m02);

	matrix.SetElement(0, 3, 0);

	
	matrix.SetElement(1, 0, m10);
	matrix.SetElement(1, 1, m11);
	matrix.SetElement(1, 2, m12);

	matrix.SetElement(1, 3, 0);



	matrix.SetElement(2, 0, m20);
	matrix.SetElement(2, 1, m21);
	matrix.SetElement(2, 2, m22);

	matrix.SetElement(2, 3, 0);




	matrix.SetElement(3, 0, 0);
	matrix.SetElement(3, 1, 0);
	matrix.SetElement(3, 2, 0);
	matrix.SetElement(3, 3, 1);

	
	return 1;
}

//----------------------------------------------------------------------------
int mafTransform::HelicalAxisToMatrix(double helical_axis[3],double angle,
										   mafMatrix &matrix)
{
	//modified by Stefano. 1-7-2003 (beg)
	//conversion to radians
	angle = angle * mafMatrix3x3::DegreesToRadians();
	//modified by Stefano. 1-7-2003 (end)

	double kx = helical_axis[0], ky = helical_axis[1], kz = helical_axis[2];
	double ks = kx * kx + ky * ky + kz * kz;
	double ca = cos(angle), sa = sin(angle);
	double ica = 1. - ca;

	//attention: for ica = 0 there is a singolarity!
	// assert(ica != 0);
   
	if ((ks - 1.) != 0) {
		ks = sqrt(ks);
		kx /= ks; ky /= ks; kz /= ks;
		}

 
	matrix.SetElement(X, 0, kx * kx * ica + ca); 
	matrix.SetElement(X, 1, ky * kx * ica - kz * sa); 
	matrix.SetElement(X, 2, kz * kx * ica + ky * sa);

	matrix.SetElement(0, 3, 0);


	matrix.SetElement(Y, 0, kx * ky * ica + kz * sa);
	matrix.SetElement(Y, 1, ky * ky * ica + ca);
	matrix.SetElement(Y, 2, kz * ky * ica - kx * sa);

	matrix.SetElement(1, 3, 0);


	matrix.SetElement(Z, 0, kx * kz * ica - ky * sa);
	matrix.SetElement(Z, 1, ky * kz * ica + kx * sa);
	matrix.SetElement(Z, 2, kz * kz * ica + ca);

	matrix.SetElement(2, 3, 0);


	matrix.SetElement(3, 0, 0);
	matrix.SetElement(3, 1, 0);
	matrix.SetElement(3, 2, 0);
	matrix.SetElement(3, 3, 1);


	return 1;

}
//----------------------------------------------------------------------------
int mafTransform::AttitudeVectorToMatrix(double attitude_vector[3],
											  mafMatrix &matrix)
//----------------------------------------------------------------------------
{

	double fi = sqrt(attitude_vector[0]*attitude_vector[0] + attitude_vector[1]*attitude_vector[1] + 
					 attitude_vector[2]*attitude_vector[2]);


	/* if abs(fi)> PI
		{
			return 0;
		}
	*/

	double sinc = sin(fi)/fi; 
	double cosc =(1-cos(fi))/(fi*fi);

	

	matrix.SetElement(0,0,cos(fi) + cosc*attitude_vector[0]*attitude_vector[0]);
	matrix.SetElement(0,1,sinc*(-attitude_vector[2]) + cosc*attitude_vector[0]*attitude_vector[1]);	
	matrix.SetElement(0,2,sinc*attitude_vector[1] + cosc*attitude_vector[0]*attitude_vector[2]);

	matrix.SetElement(1,0,sinc*attitude_vector[2] + cosc*attitude_vector[1]*attitude_vector[0]);
	matrix.SetElement(1,1,cos(fi) + cosc*attitude_vector[1]*attitude_vector[1]);
	matrix.SetElement(1,2,sinc*(-attitude_vector[0]) + cosc*attitude_vector[1]*attitude_vector[2]);

	matrix.SetElement(2,0,sinc*(-attitude_vector[1]) + cosc*attitude_vector[2]*attitude_vector[0]);
	matrix.SetElement(2,1,sinc*attitude_vector[0] + cosc*attitude_vector[2]*attitude_vector[1]);
	matrix.SetElement(2,2,cos(fi) + cosc*attitude_vector[2]*attitude_vector[2]);


	matrix.SetElement(0, 3, 0);

	matrix.SetElement(1, 3, 0);

	matrix.SetElement(2, 3, 0);

	matrix.SetElement(3, 0, 0);
	matrix.SetElement(3, 1, 0);
	matrix.SetElement(3, 2, 0);
	matrix.SetElement(3, 3, 1);
	
	return 1;
}

//----------------------------------------------------------------------------
int mafTransform::EulerCardanicAngleToMatrix(double euler_cardan[3],
												  int i,int j,int k,
												  mafMatrix &matrix)
//----------------------------------------------------------------------------
{

	//modified by Stefano. 1-7-2003 (beg)
	//converson to radians
	euler_cardan[0] = euler_cardan[0] * mafMatrix3x3::DegreesToRadians ();	
	euler_cardan[1] = euler_cardan[1] * mafMatrix3x3::DegreesToRadians ();
	euler_cardan[2] = euler_cardan[2] * mafMatrix3x3::DegreesToRadians ();	
	//modified by Stefano. 1-7-2003 (end)	

	double alfa		= euler_cardan[0];
	double beta		= euler_cardan[1];
	double gamma	= euler_cardan[2];



	double sa = sin(alfa);	
	double sb = sin(beta);
	double sc = sin(gamma);
	double ca = cos(alfa);
	double cb = cos(beta);	
	double cc = cos(gamma);

	if ( i<X || i>Z || j<X || j>Z || k<X || k>Z || i==j || j==k )
		{
			//vtkErrorMacro(<<"Illegal rotation axis");
			return 0;
		}	
	

	int sig;

	if ((( (j-i+3) % 3 ) ) == 1  && ((j-i+3) > 0) )                  
	{
			sig=1;  // ciclic 
	}	

	else		sig=-1;	// anti ciclic



	if (i!=k)	// Cardan Angles 
		{
			
			matrix.SetElement(i,i,cb*cc);
			matrix.SetElement(i,j,-sig*cb*sc);
			matrix.SetElement(i,k,sig*sb);

			
			matrix.SetElement(j,i,sa*sb*cc + sig*ca*sc);
			matrix.SetElement(j,j,-sig*sa*sb*sc + cc*ca);
			matrix.SetElement(j,k,-sig*sa*cb);

			matrix.SetElement(k,i,-sig*ca*sb*cc + sa*sc);
			matrix.SetElement(k,j,ca*sb*sc + sig*sa*cc);
			matrix.SetElement(k,k,ca*cb);
			

			matrix.SetElement(0, 3, 0);

			matrix.SetElement(1, 3, 0);

			matrix.SetElement(2, 3, 0);

			matrix.SetElement(3, 0, 0);
			matrix.SetElement(3, 1, 0);
			matrix.SetElement(3, 2, 0);
			matrix.SetElement(3, 3, 1);

			
		}	
	
	

	else		// Euler Angles
		{

			int l=3-i-j;

			matrix.SetElement(i,i,cb);
			matrix.SetElement(i,j,sb*sc);
			matrix.SetElement(i,l,sig*sb*cc);

			
			matrix.SetElement(j,i,sa*sb);
			matrix.SetElement(j,j,-sa*cb*sc + ca*cc);
			matrix.SetElement(j,l,-sig*(ca*sc + sa*cb*cc));

			matrix.SetElement(l,i,-sig*ca*sb);
			matrix.SetElement(l,j,sig*(ca*cb*sc + sa*cc));
			matrix.SetElement(l,l,-sa*sc + ca*cb*cc);


			matrix.SetElement(0, 3, 0);

			matrix.SetElement(1, 3, 0);

			matrix.SetElement(2, 3, 0);

			matrix.SetElement(3, 0, 0);
			matrix.SetElement(3, 1, 0);
			matrix.SetElement(3, 2, 0);
			matrix.SetElement(3, 3, 1);

		}




	return 1;

}
												
													   
//----------------------------------------------------------------------------
//modified by STEFY 10-7-2003(begin)//modified because Helical Axis conversion must be made 
//from a translation matrix (not from a rotation matrix)
int mafTransform::MatrixToHelicalAxis(const mafMatrix &matrix,
                                      double helical_axis[3],double point[3],
                                      double& phi,double& t, int intersect = Z)
//----------------------------------------------------------------------------
{
	// input: 
	// mafMatrix &matrix (translation matrix)
	// int intersect:location of the screw axis where it intersects either the X, Y, or the Z plane
	// default: intersect = Z

	// output:
	// helical_axis[3] is the unit vector with direction of helical axis
	// point[3] is the point on helical axis 
	// phi is the rotation angle (in deg)
	// t is the amount of translation along screw axis


	double tmp[3];

	tmp[0] = matrix.GetElement(2,1) - matrix.GetElement(1,2);
	tmp[1] = matrix.GetElement(0,2) - matrix.GetElement(2,0);
	tmp[2] = matrix.GetElement(1,0) - matrix.GetElement(0,1);

	double quad_sum;
	quad_sum = 0;
        int i;
	for (i=0; i<3; i++)
		{
			
				quad_sum = (tmp[i] * tmp[i]);
		}


	quad_sum = sqrt(quad_sum);

	for (i=0; i<3; i++)
		{
			tmp[i] = tmp[i]/quad_sum;
				
		}
	
	

	helical_axis[0] = tmp[0];
	helical_axis[1] = tmp[1];
	helical_axis[2] = tmp[2];


	
	if (quad_sum <= sqrt(2.0)) 
		{
      phi=asin(0.5*quad_sum);
			phi = phi * mafMatrix3x3::RadiansToDegrees();

		}

	else  
		{
			double sum = matrix.GetElement(0,0)+matrix.GetElement(1,1)+matrix.GetElement(2,2)-1;
			phi=acos(0.5*sum);
			phi = phi * mafMatrix3x3::RadiansToDegrees();
		}


	
	//if phi approaches 180 deg it is better to use the following:
	if (phi>135)
		{
			mafMatrix b_mat;

			double phi_rad = phi * mafMatrix3x3::DegreesToRadians();

			double b00 = 0.5 * (matrix.GetElement(0,0)+matrix.GetElement(0,0)) - cos(phi_rad); 
			double b01 = 0.5 * (matrix.GetElement(0,1)+matrix.GetElement(1,0)); 
			double b02 = 0.5 * (matrix.GetElement(0,2)+matrix.GetElement(2,0)); 

			double b10 = 0.5 * (matrix.GetElement(1,0)+matrix.GetElement(0,1)); 
			double b11 = 0.5 * (matrix.GetElement(1,1)+matrix.GetElement(1,1)) - cos(phi_rad); 
			double b12 = 0.5 * (matrix.GetElement(1,2)+matrix.GetElement(2,1)); 

			double b20 = 0.5 * (matrix.GetElement(2,0)+matrix.GetElement(0,2)); 
			double b21 = 0.5 * (matrix.GetElement(2,1)+matrix.GetElement(1,2)); 
			double b22 = 0.5 * (matrix.GetElement(2,2)+matrix.GetElement(2,2)) - cos(phi_rad); 



			b_mat.SetElement(0,0,b00);
			b_mat.SetElement(0,1,b01);
			b_mat.SetElement(0,2,b02);

			b_mat.SetElement(1,0,b10);
			b_mat.SetElement(1,1,b11);
			b_mat.SetElement(1,2,b12);

			b_mat.SetElement(2,0,b20);
			b_mat.SetElement(2,1,b21);
			b_mat.SetElement(2,2,b22);



			double btmp[3];

			btmp[0] = b00*b00 + b10*b10 + b20*b20;
			btmp[1] = b01*b01 + b11*b11 + b21*b21;
			btmp[2] = b02*b02 + b12*b12 + b22*b22;


			double bmax = 0.0;
			int index = 0;

			for (i=0; i<3; i++)
				{

					if (btmp[i] > bmax)
						{

							bmax = btmp[i];
							index = i;

						}

				}


			helical_axis[0] = (b_mat.GetElement(0,index))/sqrt(bmax);
			helical_axis[1] = (b_mat.GetElement(1,index))/sqrt(bmax);
			helical_axis[2] = (b_mat.GetElement(2,index))/sqrt(bmax);


			if ( sign(matrix.GetElement(2,1)- matrix.GetElement(1,2)) != sign(helical_axis[0]) )
				{
						helical_axis[0] = (-1) * helical_axis[0];
						helical_axis[1]	= (-1) * helical_axis[1];
						helical_axis[2]	= (-1) * helical_axis[2];
					
				}

	}


	// calculation of t:amount of translation along screw axis

	t = helical_axis[0] * matrix.GetElement(0,3) + helical_axis[1] * matrix.GetElement(1,3)	+ 
			helical_axis[2] * matrix.GetElement(2,3);

	//calculating where the screw axis intersects the plane as defined in 'intersect'

	double q_mat[3][3];
	double q_inv[3][3];

	q_mat[0][0] = matrix.GetElement(0,0) - 1;
	q_mat[0][1] = matrix.GetElement(0,1);
	q_mat[0][2] = matrix.GetElement(0,2);

	q_mat[1][0] = matrix.GetElement(1,0);
	q_mat[1][1] = matrix.GetElement(1,1) - 1;
	q_mat[1][2] = matrix.GetElement(1,2);

	q_mat[2][0] = matrix.GetElement(2,0);
	q_mat[2][1] = matrix.GetElement(2,1);
	q_mat[2][2] = matrix.GetElement(2,2) - 1;


	
	q_mat[0][intersect] = (-1)*helical_axis[0];
	q_mat[1][intersect] = (-1)*helical_axis[1];
	q_mat[2][intersect] = (-1)*helical_axis[2];


	mafMatrix3x3::Invert(q_mat, q_inv);
	

	double v03 = (-1) * (matrix.GetElement(0,3));
	double v13 = (-1) * (matrix.GetElement(1,3));
	double v23 = (-1) * (matrix.GetElement(2,3));


	// calculting the point on helical axis 

	point[0] = q_mat[0][0]*v03 + q_mat[0][1]*v13 + q_mat[0][2]*v23;  
	point[1] = q_mat[1][0]*v03 + q_mat[1][1]*v13 + q_mat[1][2]*v23;
	point[2] = q_mat[2][0]*v03 + q_mat[2][1]*v13 + q_mat[2][2]*v23;

	point[intersect] = 0;

	return 1;

}

