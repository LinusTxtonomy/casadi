/*
 *    This file is part of CasADi.
 *
 *    CasADi -- A symbolic framework for dynamic optimization.
 *    Copyright (C) 2010-2014 Joel Andersson, Joris Gillis, Moritz Diehl,
 *                            K.U. Leuven. All rights reserved.
 *    Copyright (C) 2011-2014 Greg Horn
 *
 *    CasADi is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License as published by the Free Software Foundation; either
 *    version 3 of the License, or (at your option) any later version.
 *
 *    CasADi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with CasADi; if not, write to the Free Software
 *    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#ifndef CASADI_CALCULUS_HPP
#define CASADI_CALCULUS_HPP

#include <iostream>
#include <string>
#include <cmath>

#include <limits>
#include <algorithm>
#include "casadi_exception.hpp"

// Define pi if the compiler fails to do so
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif // M_PI

/// \cond INTERNAL

namespace casadi {

  /// Enum for quick access to any node
  enum Operation {
    // Simple assignment
    OP_ASSIGN,

    // Standard unary and binary functions
    OP_ADD,  OP_SUB,  OP_MUL,  OP_DIV,
    OP_NEG,  OP_EXP,  OP_LOG,  OP_POW, OP_CONSTPOW,
    OP_SQRT,  OP_SQ,  OP_TWICE,
    OP_SIN,  OP_COS,  OP_TAN,
    OP_ASIN,  OP_ACOS,  OP_ATAN,
    OP_LT, OP_LE, OP_EQ, OP_NE, OP_NOT, OP_AND, OP_OR,
    OP_FLOOR,  OP_CEIL,  OP_FMOD, OP_FABS, OP_SIGN, OP_COPYSIGN, OP_IF_ELSE_ZERO,
    OP_ERF,  OP_FMIN,  OP_FMAX,
    OP_INV,
    OP_SINH,  OP_COSH,  OP_TANH,
    OP_ASINH, OP_ACOSH, OP_ATANH,
    OP_ATAN2,

    // Double constant
    OP_CONST,

    // Function input and output
    OP_INPUT, OP_OUTPUT,

    // Free parameter
    OP_PARAMETER,

    // Embedded function call
    OP_CALL,

    // Find first nonzero in a vector
    OP_FIND,

    // Embedded function call in parallel
    OP_MAP,

    // Matrix multiplication
    OP_MATMUL,

    // Solve linear system of equations
    OP_SOLVE,

    // Matrix transpose
    OP_TRANSPOSE,

    // Matrix determinant
    OP_DETERMINANT,

    // Matrix inverse
    OP_INVERSE,

    // Inner product
    OP_INNER_PROD,

    // Horizontal concatenation
    OP_HORZCAT,

    // Vertical concatenation of vectors
    OP_VERTCAT,

    // Diagonal concatenation
    OP_DIAGCAT,

    // Horizontal split
    OP_HORZSPLIT,

    // Vertical split of vectors
    OP_VERTSPLIT,

    // Diagonal split
    OP_DIAGSPLIT,

    // Reshape an expression
    OP_RESHAPE,

    // Submatrix reference
    OP_SUBREF,

    // Submatrix assignment
    OP_SUBASSIGN,

    // Nonzero reference
    OP_GETNONZEROS,

    // Nonzero addition
    OP_ADDNONZEROS,

    // Nonzero assignment
    OP_SETNONZEROS,

    // Set sparse
    OP_PROJECT,

    // Assertion
    OP_ASSERTION,

    // Monitor
    OP_MONITOR,

    // Norms
    OP_NORM2, OP_NORM1, OP_NORMINF, OP_NORMF,

    OP_ERFINV,
    OP_PRINTME,
    OP_LIFT,
    NUM_BUILT_IN_OPS
  };

#ifndef SWIG

  // Get GCC version if GCC is used
#ifdef __GNUC__
#ifdef __GNUC_MINOR__
#ifdef __GNUC_PATCHLEVEL__
#define GCC_VERSION (__GNUC__ * 10000 +__GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#endif // __GNUC_PATCHLEVEL__
#endif // __GNUC_MINOR__
#endif // __GNUC__

  // Disable some Visual studio warnings
#ifdef _MSC_VER

#pragma warning(disable:4996)

  // warning C4018: '<' : signed/unsigned mismatch
#pragma warning(disable:4018)

  // warning C4800: 'int' : forcing value to bool 'true'or 'false'(performance warning)
#pragma warning(disable:4800)
#endif

  ///@{
  /** \brief Enable using elementary numerical operations without std:: prefix */
  using std::sqrt;
  using std::sin;
  using std::cos;
  using std::tan;
  using std::atan;
  using std::asin;
  using std::acos;
  using std::sinh;
  using std::cosh;
  using std::tanh;
  using std::exp;
  using std::log;
  using std::log10;
  using std::abs;
  using std::fabs;
  using std::floor;
  using std::ceil;
  using std::pow;
  using std::fmod;
  using std::atan2;
  ///@}

  ///@{
  // Implement "missing" operations
  inline double atanh(double x) throw() {
    if (x==-1) return -std::numeric_limits<double>::infinity();
    if (x==1) return std::numeric_limits<double>::infinity();
    return 0.5*log((1+x)/(1-x));
  }

  inline double asinh(double x) throw() {
    return log(x + sqrt(1+x*x));
  }

  inline double acosh(double x) throw() {
    return log(x + sqrt(1+x)*sqrt(x-1));
  }

  inline int isnan(double x) throw() { return x!=x;}
  inline int isinf(double x) throw() { return isnan(x-x);}

  /// Sign function, note that sign(nan) == nan
  inline double sign(double x) { return x<0 ? -1 : x>0 ? 1 : x;}

  /// Compare if two values are equal, up to a depth
  inline bool isEqual(double x, double y, int depth=0) { return x==y;}

  /// fmin, fmax and erf should be available if C99 and/or C++11 required
  inline double fmin(double x, double y) throw() { return std::min(x, y);}
  inline int fmin(int x, int y) throw() { return std::min(x, y);}
  inline double fmax(double x, double y) throw() { return std::max(x, y);}
  inline int fmax(int x, int y) throw() { return std::max(x, y);}

  /// fabs(int) was added in C++11
  inline int fabs(int x) throw() { return std::abs(x);}
  ///@}

#ifdef HAS_ERF
  using ::erf;
#else // HAS ERF
  inline double erf(double x) throw() {
    // Approximation found in Sourceforge and modified,
    // originally from numerical recipes in Fortran
    double sx = x<0 ? -1 : x>0 ? 1 : x;
    double z = sx*x;
    double t = 1.0/(1.0+0.5*z);
    return 1.-sx*(t*exp(-z*z-1.26551223+t*(1.00002368+t*(0.37409196+t*(0.09678418+
                                       t*(-0.18628806+t*(0.27886807+t*(-1.13520398+t*(1.48851587+
                                                           t*(-0.82215223+t*0.17087277))))))))));
  }
#endif // HAS ERF
  ///@}

  ///@{
  /** \brief  CasADi additions */
  template<class T> T constpow(const T &x, const T &n) { return x.constpow(n);}
  template<class T> T printme(const T &x, const T &y) { return x.printme(y);}
  inline double printme(double x, double y) {
    std::cout << "|> " << y << " : " << x << std::endl;
    return x;
  }

  /// __copysign__ function
  template<class T> T copysign(const T &x, const T &y) {return x.__copysign__(y);}

  #ifdef HAS_COPYSIGN
  using std::copysign;
  #else
  /// copysign function
  inline double copysign(double x, double y) { return y>=0 ? fabs(x) : -fabs(x);}
  #endif //HAS_COPYSIGN

  /// Conditional assignment
  template<class T> T if_else_zero(const T &x, const T &y) {return x.zz_if_else_zero(y);}

  /// Conditional assignment
  inline double if_else_zero(double x, double y) { return x ? y : 0;}

  /// Inverse of the error function
  template<class T> T erfinv(const T &x) {return x.zz_erfinv();}
#ifdef HAS_ERFINV
  using ::erfinv;
#else // HAS ERFINV
  inline double erfinv(double x) throw() {
    // Approximation found in Sourceforge and modified: Not very efficient
    if (x>=1) {
      return x==1 ? std::numeric_limits<double>::infinity() :
          std::numeric_limits<double>::quiet_NaN();
    } else if (x<=-1) {
      return x==-1 ? -std::numeric_limits<double>::infinity() :
          std::numeric_limits<double>::quiet_NaN();
    } else if (x<-0.7) {
      double z = sqrt(-log((1.0+x)/2.0));
      return -(((1.641345311*z+3.429567803)*z-1.624906493)*z-1.970840454)/
          ((1.637067800*z+3.543889200)*z+1.0);
    } else {
      double y;
      if (x<0.7) {
        double z = x*x;
        y = x*(((-0.140543331*z+0.914624893)*z-1.645349621)*z+0.886226899)/
            ((((-0.329097515*z+0.012229801)*z+1.442710462)*z-2.118377725)*z+1.0);
      } else {
        double z = sqrt(-log((1.0-x)/2.0));
        y = (((1.641345311*z+3.429567803)*z-1.624906493)*z-1.970840454)/
            ((1.637067800*z+3.543889200)*z+1.0);
      }

      //polish x to full accuracy
      y = y - (erf(y) - x) / (2.0/sqrt(M_PI) * exp(-y*y));
      y = y - (erf(y) - x) / (2.0/sqrt(M_PI) * exp(-y*y));
      return y;
    }
  }
#endif // HAS_ERFINV
  ///@}

  template<typename T>
  T twice(const T& x) {
    return x+x;
  }

  template<typename T>
  T sq(const T& x) {
    return x*x;
  }

  template<int I>
  struct UnaryOperation {
    /// Function evaluation
    template<typename T> static inline void fcn(const T& x, T& f);

    /// Partial derivatives
    template<typename T> static inline void der(const T& x, const T& f, T* d);
  };

  template<int I>
  struct BinaryOperation {
    /// Function evaluation
    template<typename T> static inline void fcn(const T& x, const T& y, T& f) {
        UnaryOperation<I>::fcn(x, f);}

    /// Partial derivatives - binary function
    template<typename T> static inline void der(const T& x, const T& y, const T& f, T* d) {
        UnaryOperation<I>::der(x, f, d); d[1]=0; }
  };

  template<int I>
  struct BinaryOperationE {
    /// Function evaluation
    template<typename T> static inline T fcn(const T& x, const T& y) {
      T ret;
      BinaryOperation<I>::fcn(x, y, ret);
      return ret;
    }
  };

  /// Calculate function and derivative
  template<int I>
  struct DerBinaryOpertion {
    /// Perform the operation
    template<typename T> static inline void derf(const T& x, const T& y, T& f, T* d) {

      /** First save to temp since f might have the same address as x or y,
      * in which case it will be incorrect in the second call
      */
      T tmp;

      /// Evaluate the function
      BinaryOperation<I>::fcn(x, y, tmp);

      /// Evaluate the partial derivatives
      BinaryOperation<I>::der(x, y, tmp, d);

      /// Now save f
      f = tmp;
    }
  };

  /// Perform a binary operation on two scalars
  template<int I>
  struct BinaryOperationSS {
    /// Function evaluation
    template<typename T> static inline void fcn(const T& x, const T& y, T& f, int n) {
      BinaryOperation<I>::fcn(x, y, f);
    }

    /// Partial derivatives - binary function
    template<typename T> static inline void der(const T& x, const T& y, const T& f, T* d, int n) {
      BinaryOperation<I>::der(x, y, f, d);
    }
  };


  /// Perform a binary operation on two vectors
  template<int I>
  struct BinaryOperationVV {
    /// Function evaluation
    template<typename T> static inline void fcn(const T* x, const T* y, T* f, int n) {
      for (int i=0; i<n; ++i) {
        BinaryOperation<I>::fcn(*x++, *y++, *f++);
      }
    }

    /// Partial derivatives - binary function
    template<typename T> static inline void der(const T* x, const T* y, const T* f, T* d, int n) {
      for (int i=0; i<n; ++i, d+=2) {
        BinaryOperation<I>::der(*x++, *y++, *f++, d);
      }
    }
  };

  /// Perform a binary operation on a vector and a scalar
  template<int I>
  struct BinaryOperationVS {
    /// Function evaluation
    template<typename T> static inline void fcn(const T* x, const T& y, T* f, int n) {
      for (int i=0; i<n; ++i) {
        BinaryOperation<I>::fcn(*x++, y, *f++);
      }
    }

    /// Partial derivatives - binary function
    template<typename T> static inline void der(const T* x, const T& y, const T* f, T* d, int n) {
      for (int i=0; i<n; ++i, d+=2) {
        BinaryOperation<I>::der(*x++, y, *f++, d);
      }
    }
  };

  /// Perform a binary operation on a scalar and a vector
  template<int I>
  struct BinaryOperationSV {
    /// Function evaluation
    template<typename T> static inline void fcn(const T& x, const T* y, T* f, int n) {
      for (int i=0; i<n; ++i) {
        BinaryOperation<I>::fcn(x, *y++, *f++);
      }
    }

    /// Partial derivatives - binary function
    template<typename T> static inline void der(const T& x, const T* y, const T* f, T* d, int n) {
      for (int i=0; i<n; ++i, d+=2) {
        BinaryOperation<I>::der(x, *y++, *f++, d);
      }
    }
  };

  ///@{
  /// Smoothness (by default true)
  template<int I> struct SmoothChecker { static const bool check=true;};
  template<>      struct SmoothChecker<OP_LT>{ static const bool check=false;};
  template<>      struct SmoothChecker<OP_LE>{ static const bool check=false;};
  template<>      struct SmoothChecker<OP_FLOOR>{ static const bool check=false;};
  template<>      struct SmoothChecker<OP_CEIL>{ static const bool check=false;};
  template<>      struct SmoothChecker<OP_FMOD>{ static const bool check=false;};
  template<>      struct SmoothChecker<OP_EQ>{ static const bool check=false;};
  template<>      struct SmoothChecker<OP_NE>{ static const bool check=false;};
  template<>      struct SmoothChecker<OP_SIGN>{ static const bool check=false;};
  template<>      struct SmoothChecker<OP_COPYSIGN>{ static const bool check=false;};
  template<>      struct SmoothChecker<OP_NOT>{ static const bool check=false;};
  template<>      struct SmoothChecker<OP_AND>{ static const bool check=false;};
  template<>      struct SmoothChecker<OP_OR>{ static const bool check=false;};
  template<>      struct SmoothChecker<OP_IF_ELSE_ZERO>{ static const bool check=false;};
  ///@}

  ///@{
  /// If evaluated with the first argument zero, is the result zero?
  template<int I> struct F0XChecker { static const bool check=false;};
  template<>      struct F0XChecker<OP_ASSIGN>{ static const bool check=true;};
  template<>      struct F0XChecker<OP_MUL>{ static const bool check=true;};
  template<>      struct F0XChecker<OP_DIV>{ static const bool check=true;};
  template<>      struct F0XChecker<OP_NEG>{ static const bool check=true;};
  template<>      struct F0XChecker<OP_POW>{ static const bool check=true;};
  template<>      struct F0XChecker<OP_CONSTPOW>{ static const bool check=true;};
  template<>      struct F0XChecker<OP_SQRT>{ static const bool check=true;};
  template<>      struct F0XChecker<OP_SQ>{ static const bool check=true;};
  template<>      struct F0XChecker<OP_TWICE>{ static const bool check=true;};
  template<>      struct F0XChecker<OP_SIN>{ static const bool check=true;};
  template<>      struct F0XChecker<OP_TAN>{ static const bool check=true;};
  template<>      struct F0XChecker<OP_ATAN>{ static const bool check=true;};
  template<>      struct F0XChecker<OP_ASIN>{ static const bool check=true;};
  template<>      struct F0XChecker<OP_FLOOR>{ static const bool check=true;};
  template<>      struct F0XChecker<OP_CEIL>{ static const bool check=true;};
  template<>      struct F0XChecker<OP_FMOD>{ static const bool check=true;};
  template<>      struct F0XChecker<OP_FABS>{ static const bool check=true;};
  template<>      struct F0XChecker<OP_SIGN>{ static const bool check=true;};
  template<>      struct F0XChecker<OP_COPYSIGN>{ static const bool check=true;};
  template<>      struct F0XChecker<OP_ERF>{ static const bool check=true;};
  template<>      struct F0XChecker<OP_SINH>{ static const bool check=true;};
  template<>      struct F0XChecker<OP_TANH>{ static const bool check=true;};
  template<>      struct F0XChecker<OP_ASINH>{ static const bool check=true;};
  template<>      struct F0XChecker<OP_ATANH>{ static const bool check=true;};
  template<>      struct F0XChecker<OP_ERFINV>{ static const bool check=true;};
  template<>      struct F0XChecker<OP_AND>{ static const bool check=true;};
  template<>      struct F0XChecker<OP_IF_ELSE_ZERO>{ static const bool check=true;};
  ///@}

  ///@{
  /// If evaluated with the second argument zero, is the result zero?
  template<int I> struct Function0Checker { static const bool check=false;};
  template<>      struct Function0Checker<OP_MUL>{ static const bool check=true;};
  template<>      struct Function0Checker<OP_AND>{ static const bool check=true;};
  template<>      struct Function0Checker<OP_IF_ELSE_ZERO>{ static const bool check=true;};
  ///@}

  ///@{
  /// If evaluated with both arguments zero, is the result zero?
  template<int I> struct F00Checker { static const bool check=F0XChecker<I>::check;};
  template<>      struct F00Checker<OP_ADD>{ static const bool check=true;};
  template<>      struct F00Checker<OP_SUB>{ static const bool check=true;};
  template<>      struct F00Checker<OP_FMIN>{ static const bool check=true;};
  template<>      struct F00Checker<OP_FMAX>{ static const bool check=true;};
  template<>      struct F00Checker<OP_AND>{ static const bool check=true;};
  template<>      struct F00Checker<OP_OR>{ static const bool check=true;};
  template<>      struct F00Checker<OP_COPYSIGN>{ static const bool check=true;};
  ///@}

  ///@{
  /// Is commutative
  template<int I> struct CommChecker { static const bool check=false;};
  template<>      struct CommChecker<OP_ADD>{ static const bool check=true;};
  template<>      struct CommChecker<OP_MUL>{ static const bool check=true;};
  template<>      struct CommChecker<OP_EQ>{ static const bool check=true;};
  template<>      struct CommChecker<OP_NE>{ static const bool check=true;};
  template<>      struct CommChecker<OP_AND>{ static const bool check=true;};
  template<>      struct CommChecker<OP_OR>{ static const bool check=true;};
  ///@}

  ///@{
  /// Always non-negative (false by default)
  template<int I> struct NonnegativeChecker { static const bool check=false;};
  template<>      struct NonnegativeChecker<OP_SQRT>{ static const bool check=true;};
  template<>      struct NonnegativeChecker<OP_SQ>{ static const bool check=true;};
  template<>      struct NonnegativeChecker<OP_EXP>{ static const bool check=true;};
  template<>      struct NonnegativeChecker<OP_LT>{ static const bool check=true;};
  template<>      struct NonnegativeChecker<OP_LE>{ static const bool check=true;};
  template<>      struct NonnegativeChecker<OP_EQ>{ static const bool check=true;};
  template<>      struct NonnegativeChecker<OP_NE>{ static const bool check=true;};
  template<>      struct NonnegativeChecker<OP_NOT>{ static const bool check=true;};
  template<>      struct NonnegativeChecker<OP_AND>{ static const bool check=true;};
  template<>      struct NonnegativeChecker<OP_OR>{ static const bool check=true;};
  ///@}

  ///@{
  /// Is the operation binary as opposed to unary
  template<int I> struct BinaryChecker { static const bool check=false;};
  template<>      struct BinaryChecker<OP_ADD>{ static const bool check=true;};
  template<>      struct BinaryChecker<OP_SUB>{ static const bool check=true;};
  template<>      struct BinaryChecker<OP_MUL>{ static const bool check=true;};
  template<>      struct BinaryChecker<OP_DIV>{ static const bool check=true;};
  template<>      struct BinaryChecker<OP_POW>{ static const bool check=true;};
  template<>      struct BinaryChecker<OP_CONSTPOW>{ static const bool check=true;};
  template<>      struct BinaryChecker<OP_EQ>{ static const bool check=true;};
  template<>      struct BinaryChecker<OP_NE>{ static const bool check=true;};
  template<>      struct BinaryChecker<OP_AND>{ static const bool check=true;};
  template<>      struct BinaryChecker<OP_OR>{ static const bool check=true;};
  template<>      struct BinaryChecker<OP_FMIN>{ static const bool check=true;};
  template<>      struct BinaryChecker<OP_FMAX>{ static const bool check=true;};
  template<>      struct BinaryChecker<OP_PRINTME>{ static const bool check=true;};
  template<>      struct BinaryChecker<OP_ATAN2>{ static const bool check=true;};
  template<>      struct BinaryChecker<OP_IF_ELSE_ZERO>{ static const bool check=true;};
  template<>      struct BinaryChecker<OP_COPYSIGN>{ static const bool check=true;};
  ///@}

  /// Simple assignment
  template<>
  struct UnaryOperation<OP_ASSIGN>{
  public:
    template<typename T> static inline void fcn(const T& x, T& f) { f = x;}
    template<typename T> static inline void der(const T& x, const T& f, T* d) { d[0] = 1; }
  };

  /// Addition
  template<>
  struct BinaryOperation<OP_ADD>{
  public:
    template<typename T> static inline void fcn(const T& x, const T& y, T& f) { f = x+y;}
    template<typename T> static inline void der(const T& x, const T& y, const T& f, T* d) {
        d[0]=d[1]=1;}
  };

  /// Subtraction
  template<>
  struct BinaryOperation<OP_SUB>{
  public:
    template<typename T> static inline void fcn(const T& x, const T& y, T& f) { f = x-y;}
    template<typename T> static inline void der(const T& x, const T& y, const T& f, T* d) {
        d[0]=1; d[1]=-1;}
  };

  /// Multiplication
  template<>
  struct BinaryOperation<OP_MUL>{
  public:
    template<typename T> static inline void fcn(const T& x, const T& y, T& f) { f = x*y;}
    template<typename T> static inline void der(const T& x, const T& y, const T& f, T* d) {
        d[0]=y; d[1]=x;}
  };

  /// Division
  template<>
  struct BinaryOperation<OP_DIV>{
  public:
    template<typename T> static inline void fcn(const T& x, const T& y, T& f) { f = x/y;}
    template<typename T> static inline void der(const T& x, const T& y, const T& f, T* d) {
        d[0]=1/y; d[1]=-f/y;}
  };

  /// Negation
  template<>
  struct UnaryOperation<OP_NEG>{
  public:
    template<typename T> static inline void fcn(const T& x, T& f) { f = -x;}
    template<typename T> static inline void der(const T& x, const T& f, T* d) { d[0]=-1;}
  };

  /// Natural exponent
  template<>
  struct UnaryOperation<OP_EXP>{
  public:
    template<typename T> static inline void fcn(const T& x, T& f) { f = exp(x);}
    template<typename T> static inline void der(const T& x, const T& f, T* d) { d[0]=f;}
  };

  /// Natural logarithm
  template<>
  struct UnaryOperation<OP_LOG>{
  public:
    template<typename T> static inline void fcn(const T& x, T& f) { f = log(x);}
    template<typename T> static inline void der(const T& x, const T& f, T* d) { d[0]=1/x;}
  };

  /// Power, defined only for x>=0
  template<>
  struct BinaryOperation<OP_POW>{
  public:
    template<typename T> static inline void fcn(const T& x, const T& y, T& f) { f = pow(x, y);}
    // See issue #104 why d[0] is no longer y*f/x
    template<typename T> static inline void der(const T& x, const T& y, const T& f, T* d) {
        d[0]=y*pow(x, y-1); d[1]=log(x)*f;}
  };

  /// Power, defined only for y constant
  template<>
  struct BinaryOperation<OP_CONSTPOW>{
  public:
    template<typename T> static inline void fcn(const T& x, const T& y, T& f) { f = pow(x, y);}
    template<typename T> static inline void der(const T& x, const T& y, const T& f, T* d) {
        d[0]=y*pow(x, y-1); d[1]=0;}
  };

  /// Square root
  template<>
  struct UnaryOperation<OP_SQRT>{
  public:
    template<typename T> static inline void fcn(const T& x, T& f) { f = sqrt(x);}
    template<typename T> static inline void der(const T& x, const T& f, T* d) { d[0]=1/(twice(f));}
  };

  /// Square
  template<>
  struct UnaryOperation<OP_SQ>{
  public:
    template<typename T> static inline void fcn(const T& x, T& f) { f = sq(x);}
    template<typename T> static inline void der(const T& x, const T& f, T* d) { d[0]=twice(x);}
  };

  /// Times two
  template<>
  struct UnaryOperation<OP_TWICE>{
    template<typename T> static inline void fcn(const T& x, T& f) { f = 2.*x;}
    template<typename T> static inline void der(const T& x, const T& f, T* d) { d[0] = 2; }
  };

  /// Sine
  template<>
  struct UnaryOperation<OP_SIN>{
  public:
    template<typename T> static inline void fcn(const T& x, T& f) { f = sin(x);}
    template<typename T> static inline void der(const T& x, const T& f, T* d) { d[0]=cos(x);}
  };

  /// Cosine
  template<>
  struct UnaryOperation<OP_COS>{
  public:
    template<typename T> static inline void fcn(const T& x, T& f) { f = cos(x);}
    template<typename T> static inline void der(const T& x, const T& f, T* d) { d[0]=-sin(x);}
  };

  /// Tangent
  template<>
  struct UnaryOperation<OP_TAN>{
  public:
    template<typename T> static inline void fcn(const T& x, T& f) { f = tan(x);}
    template<typename T> static inline void der(const T& x, const T& f, T* d)
    { d[0] = 1/sq(cos(x));}
  };

  /// Arcus sine
  template<>
  struct UnaryOperation<OP_ASIN>{
  public:
    template<typename T> static inline void fcn(const T& x, T& f) { f = asin(x);}
    template<typename T> static inline void der(const T& x, const T& f, T* d) { d[0]=1/sqrt(1-x*x);}
  };

  /// Arcus cosine
  template<>
  struct UnaryOperation<OP_ACOS>{
  public:
    template<typename T> static inline void fcn(const T& x, T& f) { f = acos(x);}
    template<typename T> static inline void der(const T& x, const T& f, T* d)
    { d[0]=-1/sqrt(1-x*x);}
  };

  /// Arcus tangent
  template<>
  struct UnaryOperation<OP_ATAN>{
  public:
    template<typename T> static inline void fcn(const T& x, T& f) { f = atan(x);}
    template<typename T> static inline void der(const T& x, const T& f, T* d) { d[0] = 1/(1+x*x);}
  };

  /// Less than
  template<>
  struct BinaryOperation<OP_LT>{
  public:
    template<typename T> static inline void fcn(const T& x, const T& y, T& f) { f = x < y;}
    template<typename T> static inline void der(const T& x, const T& y, const T& f, T* d) {
        d[0]=d[1]=0;}
  };

  /// Less or equal to
  template<>
  struct BinaryOperation<OP_LE>{
  public:
    template<typename T> static inline void fcn(const T& x, const T& y, T& f) { f = x <= y;}
    template<typename T> static inline void der(const T& x, const T& y, const T& f, T* d) {
        d[0]=d[1]=0;}
  };

  /// Floor function
  template<>
  struct UnaryOperation<OP_FLOOR>{
  public:
    template<typename T> static inline void fcn(const T& x, T& f) { f = floor(x);}
    template<typename T> static inline void der(const T& x, const T& f, T* d) { d[0] = 0;}
  };

  /// Ceil function
  template<>
  struct UnaryOperation<OP_CEIL>{
  public:
    template<typename T> static inline void fcn(const T& x, T& f) { f = ceil(x);}
    template<typename T> static inline void der(const T& x, const T& f, T* d) { d[0] = 0;}
  };

  /// Remainder of division
  template<>
  struct BinaryOperation<OP_FMOD>{
    template<typename T> static inline void fcn(const T& x, const T& y, T& f) { f = fmod(x, y);}
    template<typename T> static inline void der(const T& x, const T& y, const T& f, T* d) {
      d[0]=1; d[1]=(f-x)/y;}
  };

  /// Equal to
  template<>
  struct BinaryOperation<OP_EQ>{
    template<typename T> static inline void fcn(const T& x, const T& y, T& f) { f = x==y;}
    template<typename T> static inline void der(const T& x, const T& y, const T& f, T* d) {
        d[0]=d[1]=0;}
  };

  /// Not equal to
  template<>
  struct BinaryOperation<OP_NE>{
    template<typename T> static inline void fcn(const T& x, const T& y, T& f) { f = x!=y;}
    template<typename T> static inline void der(const T& x, const T& y, const T& f, T* d) {
        d[0]=d[1]=0;}
  };

  /// Logical not
  template<>
  struct UnaryOperation<OP_NOT>{
  public:
    template<typename T> static inline void fcn(const T& x, T& f) { f = !x;}
    template<typename T> static inline void der(const T& x, const T& f, T* d) { d[0] = 0;}
  };

  /// Logical and
  template<>
  struct BinaryOperation<OP_AND>{
    template<typename T> static inline void fcn(const T& x, const T& y, T& f) { f = x && y;}
    template<typename T> static inline void der(const T& x, const T& y, const T& f, T* d) {
        d[0]=d[1]=0;}
  };

  /// Logical or
  template<>
  struct BinaryOperation<OP_OR>{
    template<typename T> static inline void fcn(const T& x, const T& y, T& f) { f = x || y;}
    template<typename T> static inline void der(const T& x, const T& y, const T& f, T* d) {
        d[0]=d[1]=0;}
  };

  /// Error function
  template<>
  struct UnaryOperation<OP_ERF>{
    template<typename T> static inline void fcn(const T& x, T& f) { f = erf(x);}
    template<typename T> static inline void der(const T& x, const T& f, T* d) {
        d[0] = (2/sqrt(M_PI))*exp(-x*x);}
  };

  /// Absolute value
  template<>
  struct UnaryOperation<OP_FABS>{
    template<typename T> static inline void fcn(const T& x, T& f) { f = fabs(x);}
    template<typename T> static inline void der(const T& x, const T& f, T* d) {
        d[0]=sign(x);}
  };

  /// Sign
  template<>
  struct UnaryOperation<OP_SIGN>{
    template<typename T> static inline void fcn(const T& x, T& f) { f = sign(x);}
    template<typename T> static inline void der(const T& x, const T& f, T* d) { d[0]=0;}
  };

  /// Copysign
  template<>
  struct BinaryOperation<OP_COPYSIGN>{
    template<typename T> static inline void fcn(const T& x, const T& y, T& f) { f = copysign(x, y);}
    template<typename T> static inline void der(const T& x, const T& y, const T& f, T* d) {
        T e = 1; d[0]=copysign(e, y); d[1]=0;}
  };

  /// Minimum
  template<>
  struct BinaryOperation<OP_FMIN>{
    template<typename T> static inline void fcn(const T& x, const T& y, T& f) { f = fmin(x, y);}
    template<typename T> static inline void der(const T& x, const T& y, const T& f, T* d) {
        d[0]=x<=y; d[1]=!d[0];}
  };

  /// Maximum
  template<>
  struct BinaryOperation<OP_FMAX>{
    template<typename T> static inline void fcn(const T& x, const T& y, T& f) { f = fmax(x, y);}
    template<typename T> static inline void der(const T& x, const T& y, const T& f, T* d) {
        d[0]=x>=y; d[1]=!d[0];}
  };

  /// Elementwise inverse
  template<>
  struct UnaryOperation<OP_INV>{
    template<typename T> static inline void fcn(const T& x, T& f) { f = 1./x;}
    template<typename T> static inline void der(const T& x, const T& f, T* d) { d[0] = -f*f; }
  };

  /// Hyperbolic sine
  template<>
  struct UnaryOperation<OP_SINH>{
    template<typename T> static inline void fcn(const T& x, T& f) { f = sinh(x);}
    template<typename T> static inline void der(const T& x, const T& f, T* d) { d[0] = cosh(x); }
  };

  /// Hyperbolic cosine
  template<>
  struct UnaryOperation<OP_COSH>{
    template<typename T> static inline void fcn(const T& x, T& f) { f = cosh(x);}
    template<typename T> static inline void der(const T& x, const T& f, T* d) { d[0] = -sinh(x); }
  };

  /// Hyperbolic tangent
  template<>
  struct UnaryOperation<OP_TANH>{
    template<typename T> static inline void fcn(const T& x, T& f) { f = tanh(x);}
    template<typename T> static inline void der(const T& x, const T& f, T* d) { d[0] = 1-f*f; }
  };

  /// Inverse hyperbolic sine
  template<>
  struct UnaryOperation<OP_ASINH>{
    template<typename T> static inline void fcn(const T& x, T& f) { f = asinh(x);}
    template<typename T> static inline void der(const T& x, const T& f, T* d) {
        d[0] = 1/sqrt(1+x*x); }
  };

  /// Inverse hyperbolic cosine
  template<>
  struct UnaryOperation<OP_ACOSH>{
    template<typename T> static inline void fcn(const T& x, T& f) { f = acosh(x);}
    template<typename T> static inline void der(const T& x, const T& f, T* d) {
        d[0] = 1/sqrt(x-1)/sqrt(x+1); }
  };

  /// Inverse hyperbolic tangent
  template<>
  struct UnaryOperation<OP_ATANH>{
    template<typename T> static inline void fcn(const T& x, T& f) { f = atanh(x);}
    template<typename T> static inline void der(const T& x, const T& f, T* d) { d[0] = 1/(1-x*x); }
  };

  /// Inverse of error function
  template<>
  struct UnaryOperation<OP_ERFINV>{
    template<typename T> static inline void fcn(const T& x, T& f) { f = erfinv(x);}
    template<typename T> static inline void der(const T& x, const T& f, T* d) {
        d[0] = (sqrt(M_PI)/2)*exp(f*f); }
  };

  /// Identity operator with the side effect of printing
  template<>
  struct BinaryOperation<OP_PRINTME>{
    template<typename T> static inline void fcn(const T& x, const T& y, T& f) {f = printme(x, y); }
    template<typename T> static inline void der(const T& x, const T& y, const T& f, T* d) {
        d[0]=1; d[1]=0;}
  };

  /// Arctan2
  template<>
  struct BinaryOperation<OP_ATAN2>{
  public:
    template<typename T> static inline void fcn(const T& x, const T& y, T& f) { f = atan2(x, y);}
    template<typename T> static inline void der(const T& x, const T& y, const T& f, T* d) {
        T t = x*x+y*y; d[0]=y/t; d[1]=-x/t;}
  };

  /// Conditional assignment
  template<>
  struct BinaryOperation<OP_IF_ELSE_ZERO>{
  public:
    template<typename T> static inline void fcn(const T& x, const T& y, T& f) {
        f = if_else_zero(x, y);}
    template<typename T> static inline void der(const T& x, const T& y, const T& f, T* d) {
        d[0]=0; d[1]=x;}
  };

  /// Inverse of error function
  template<>
  struct BinaryOperation<OP_LIFT>{
    template<typename T> static inline void fcn(const T& x, const T& y, T& f) { f = x;}
    template<typename T> static inline void der(const T& x, const T& y, const T& f, T* d) {
        d[0] = 1; d[1] = 0; }
  };

#endif // SWIG

} // namespace casadi

/// \endcond

#endif // CASADI_CALCULUS_HPP
