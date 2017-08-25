//=============================================================================
//   This file is part of VTKEdge. See vtkedge.org for more information.
//
//   Copyright (c) 2010 Kitware, Inc.
//
//   VTKEdge may be used under the terms of the BSD License
//   Please see the file Copyright.txt in the root directory of
//   VTKEdge for further information.
//
//   Alternatively, you may see: 
//
//   http://www.vtkedge.org/vtkedge/project/license.html
//
//
//   For custom extensions, consulting services, or training for
//   this or any other Kitware supported open source project, please
//   contact Kitware at sales@kitware.com.
//
//
//=============================================================================

#include "vtkKWEFunctionToGLSL.h"
#include "vtkStdString.h"
#include "vtkObjectFactory.h"
#include <assert.h>

#include <vtksys/ios/sstream>

vtkCxxRevisionMacro(vtkKWEFunctionToGLSL, "$Revision: 1774 $");
vtkStandardNewMacro(vtkKWEFunctionToGLSL);

// GLSL Operator Precedence (from the GLSL spec). 1 is the highest,
// 17 the lowest.
// 1. parenthentical grouping
// 2. array subscript, function/constructor call, field selector
// 4. multiplicative
// 5. additive

// ----------------------------------------------------------------------------
// Description:
// Value modified by GenerateGLSL().
bool vtkKWEFunctionToGLSL::GetParseStatus()
{
  return this->ParseStatus;
}

// ----------------------------------------------------------------------------
// Description:
// Allocate string stack.
// \pre void: this->StringStack==0
// \pre positive_size: this->StackSize>0
// \post allocated: this->StringStack!=0
void vtkKWEFunctionToGLSL::AllocateStringStack()
{
  assert("pre: void" && this->StringStack==0);
  assert("pre: positive_size" && this->StackSize>0);

  this->StringStack=new vtkStdString[this->StackSize];

#if 0
  int i=0;
  while(i<this->StackSize)
    {
    this->StringStack[i]=0;
    ++i;
    }
#endif
  assert("post: allocated" && this->StringStack!=0);
}

// ----------------------------------------------------------------------------
// Description:
// Delete string stack.
void vtkKWEFunctionToGLSL::DeleteStringStack()
{
  if(this->StringStack!=0)
    {
#if 0
    int i=0;
    while(i<this->StackSize)
      {
      if(this->StringStack[i]!=0)
        {
        delete this->StringStack[i];
        this->StringStack[i]=0;
        }
      ++i;
      }
#endif
    delete[] this->StringStack;
    this->StringStack=0;
    }
}

// ----------------------------------------------------------------------------
// Description:
// Return the dimension of the result of the expression.
// \pre valid_expression: GetParseStatus()
// \post valid_result: result==1 || result==3
int vtkKWEFunctionToGLSL::GetResultDimension()
{
  assert("pre: valid_expression" && this->GetParseStatus());

  assert("post: valid_result" &&
         (this->ResultDimension==1 || this->ResultDimension==3));
  return this->ResultDimension;
}

// ----------------------------------------------------------------------------
// Description:
// Generate GLSL source code in GLSLCode.
// Update ParseStatus.
void vtkKWEFunctionToGLSL::GenerateGLSL()
{
  // Almost like vtkFunctionParser::Evaluate()

  int numBytesProcessed;
  int numImmediatesProcessed = 0;
  int stackPosition = -1;
  this->ResultDimension=0; // 1 or 3.

  if (this->FunctionMTime.GetMTime() > this->ParseMTime.GetMTime())
    {
    this->ParseStatus=this->Parse()==1;
    if(this->StringStack!=0)
      {
      this->DeleteStringStack();
      }
    if(this->PrecedenceStack!=0)
      {
      delete[] this->PrecedenceStack;
      this->PrecedenceStack=0;
      }
    if (!this->ParseStatus)
      {
      return;
      }
    if (this->StackSize>0)
      {
      this->AllocateStringStack();
      this->PrecedenceStack = new int[this->StackSize];
      if(this->PrecedenceStack==0)
        {
        vtkErrorMacro("Parse: Out of memory");
        this->ParseStatus=false;
        return;
        }
      }
    this->BuildGLSLVariableNames();
    }

  int dim=0; // dimension of the result of the last operation (1 or 3).

  for (numBytesProcessed = 0; numBytesProcessed < this->ByteCodeSize;
       numBytesProcessed++)
    {
    vtksys_ios::ostringstream ost;
    switch (this->ByteCode[numBytesProcessed])
      {
      case VTK_PARSER_IMMEDIATE:
        ++stackPosition;

        // we use ost.setf/unsetf(ios::showpoint) instead of
        // ost<<std::showpoint<<std::noshowpoint because of a VS6 bug.
        ost.setf(ios::showpoint);
        ost<<this->Immediates[numImmediatesProcessed++];
        ost.unsetf(ios::showpoint);
        this->StringStack[stackPosition]=ost.str();
        this->PrecedenceStack[stackPosition]=1; // highest GLSL priority.
        dim=1;
        break;
      case VTK_PARSER_UNARY_MINUS:
        ost<<"-";
        if(this->PrecedenceStack[stackPosition]>3) // unary is 3
          {
          // need to be protected by parenthesis
          ost<<"(";
          }
        ost<<this->StringStack[stackPosition];
        if(this->PrecedenceStack[stackPosition]>3) // unary is 3
          {
          // need to be protected by parenthesis
          ost<<")";
          }
        this->StringStack[stackPosition]=ost.str();
        // Update the precedence.
        this->PrecedenceStack[stackPosition]=3;
        dim=1;
        break;
      case VTK_PARSER_ADD:
        if(this->PrecedenceStack[stackPosition-1]>5) // additive is 5
          {
          // need to be protected by parenthesis
          ost<<"(";
          }
        ost<<this->StringStack[stackPosition-1];
        if(this->PrecedenceStack[stackPosition-1]>5) // additive is 5
          {
          // need to be protected by parenthesis
          ost<<")";
          }
        ost<<"+";
        if(this->PrecedenceStack[stackPosition]>5) // additive is 5
          {
          // need to be protected by parenthesis
          ost<<"(";
          }
        ost<<this->StringStack[stackPosition];
        if(this->PrecedenceStack[stackPosition]>5) // additive is 5
          {
          // need to be protected by parenthesis
          ost<<")";
          }
        this->StringStack[stackPosition-1]=ost.str();
        // Update the precedence.
        this->PrecedenceStack[stackPosition-1]=5;
        --stackPosition;
        dim=1;
        break;
      case VTK_PARSER_SUBTRACT:
        if(this->PrecedenceStack[stackPosition-1]>5) // additive is 5
          {
          // need to be protected by parenthesis
          ost<<"(";
          }
        ost<<this->StringStack[stackPosition-1];
        if(this->PrecedenceStack[stackPosition-1]>5) // additive is 5
          {
          // need to be protected by parenthesis
          ost<<")";
          }
        ost<<"-";
        if(this->PrecedenceStack[stackPosition]>5) // additive is 5
          {
          // need to be protected by parenthesis
          ost<<"(";
          }
        ost<<this->StringStack[stackPosition];
        if(this->PrecedenceStack[stackPosition]>5) // additive is 5
          {
          // need to be protected by parenthesis
          ost<<")";
          }
        this->StringStack[stackPosition-1]=ost.str();
        // Update the precedence.
        this->PrecedenceStack[stackPosition-1]=5;
        --stackPosition;
        dim=1;
        break;
      case VTK_PARSER_MULTIPLY:
        if(this->PrecedenceStack[stackPosition-1]>4) // multiplicative is 4
          {
          // need to be protected by parenthesis
          ost<<"(";
          }
        ost<<this->StringStack[stackPosition-1];
        if(this->PrecedenceStack[stackPosition-1]>4) // multiplicative is 4
          {
          // need to be protected by parenthesis
          ost<<")";
          }
        ost<<"*";
        if(this->PrecedenceStack[stackPosition]>4) // multiplicative is 4
          {
          // need to be protected by parenthesis
          ost<<"(";
          }
        ost<<this->StringStack[stackPosition];
        if(this->PrecedenceStack[stackPosition]>4) // multiplicative is 4
          {
          // need to be protected by parenthesis
          ost<<")";
          }
        this->StringStack[stackPosition-1]=ost.str();
        // Update the precedence.
        this->PrecedenceStack[stackPosition-1]=4;
        --stackPosition;
        dim=1;
        break;
      case VTK_PARSER_DIVIDE:
        if(this->PrecedenceStack[stackPosition-1]>4) // multiplicative is 4
          {
          // need to be protected by parenthesis
          ost<<"(";
          }
        ost<<this->StringStack[stackPosition-1];
        if(this->PrecedenceStack[stackPosition-1]>4) // multiplicative is 4
          {
          // need to be protected by parenthesis
          ost<<")";
          }
        ost<<"/";
        if(this->PrecedenceStack[stackPosition]>4) // multiplicative is 4
          {
          // need to be protected by parenthesis
          ost<<"(";
          }
        ost<<this->StringStack[stackPosition];
        if(this->PrecedenceStack[stackPosition]>4) // multiplicative is 4
          {
          // need to be protected by parenthesis
          ost<<")";
          }
        this->StringStack[stackPosition-1]=ost.str();
        // Update the precedence.
        this->PrecedenceStack[stackPosition-1]=4;
        --stackPosition;
        dim=1;
        break;
      case VTK_PARSER_POWER:
        ost<<"pow(";
        ost<<this->StringStack[stackPosition-1];
        ost<<",";
        ost<<this->StringStack[stackPosition];
        ost<<")";
        this->StringStack[stackPosition-1]=ost.str();
        // Update the precedence.
        this->PrecedenceStack[stackPosition-1]=2; // function is 2
        --stackPosition;
        dim=1;
        break;
      case VTK_PARSER_ABSOLUTE_VALUE:
        ost<<"fabs(";
        ost<<this->StringStack[stackPosition];
        ost<<")";
        this->StringStack[stackPosition]=ost.str();
        // Update the precedence.
        this->PrecedenceStack[stackPosition]=2; // function is 2
        dim=1;
        break;
      case VTK_PARSER_EXPONENT:
        ost<<"exp(";
        ost<<this->StringStack[stackPosition];
        ost<<")";
        this->StringStack[stackPosition]=ost.str();
        // Update the precedence.
        this->PrecedenceStack[stackPosition]=2; // function is 2
        dim=1;
        break;
      case VTK_PARSER_CEILING:
        ost<<"ceil(";
        ost<<this->StringStack[stackPosition];
        ost<<")";
        this->StringStack[stackPosition]=ost.str();
        // Update the precedence.
        this->PrecedenceStack[stackPosition]=2; // function is 2
        dim=1;
        break;
      case VTK_PARSER_FLOOR:
        ost<<"floor(";
        ost<<this->StringStack[stackPosition];
        ost<<")";
        this->StringStack[stackPosition]=ost.str();
        // Update the precedence.
        this->PrecedenceStack[stackPosition]=2; // function is 2
        dim=1;
        break;
      case VTK_PARSER_LOGARITHM:
      case VTK_PARSER_LOGARITHME:
        ost<<"log(";
        ost<<this->StringStack[stackPosition];
        ost<<")";
        this->StringStack[stackPosition]=ost.str();
        // Update the precedence.
        this->PrecedenceStack[stackPosition]=2; // function is 2
        dim=1;
        break;
      case VTK_PARSER_LOGARITHM10:
        ost<<"log(";
        ost<<this->StringStack[stackPosition];
        ost<<")/log(10.0)";
        this->StringStack[stackPosition]=ost.str();
        // Update the precedence.
        this->PrecedenceStack[stackPosition]=4; // multiplicative is 4
        dim=1;
        break;
      case VTK_PARSER_SQUARE_ROOT:
        ost<<"sqrt(";
        ost<<this->StringStack[stackPosition];
        ost<<")";
        this->StringStack[stackPosition]=ost.str();
        // Update the precedence.
        this->PrecedenceStack[stackPosition]=2; // function is 2
        dim=1;
        break;
      case VTK_PARSER_SINE:
        ost<<"sin(";
        ost<<this->StringStack[stackPosition];
        ost<<")";
        this->StringStack[stackPosition]=ost.str();
        // Update the precedence.
        this->PrecedenceStack[stackPosition]=2; // function is 2
        dim=1;
        break;
      case VTK_PARSER_COSINE:
        ost<<"cos(";
        ost<<this->StringStack[stackPosition];
        ost<<")";
        this->StringStack[stackPosition]=ost.str();
        // Update the precedence.
        this->PrecedenceStack[stackPosition]=2; // function is 2
        dim=1;
        break;
      case VTK_PARSER_TANGENT:
        ost<<"tan(";
        ost<<this->StringStack[stackPosition];
        ost<<")";
        this->StringStack[stackPosition]=ost.str();
        // Update the precedence.
        this->PrecedenceStack[stackPosition]=2; // function is 2
        dim=1;
        break;
      case VTK_PARSER_ARCSINE:
        ost<<"asin(";
        ost<<this->StringStack[stackPosition];
        ost<<")";
        this->StringStack[stackPosition]=ost.str();
        // Update the precedence.
        this->PrecedenceStack[stackPosition]=2; // function is 2
        dim=1;
        break;
      case VTK_PARSER_ARCCOSINE:
        ost<<"acos(";
        ost<<this->StringStack[stackPosition];
        ost<<")";
        this->StringStack[stackPosition]=ost.str();
        // Update the precedence.
        this->PrecedenceStack[stackPosition]=2; // function is 2
        dim=1;
        break;
      case VTK_PARSER_ARCTANGENT:
        ost<<"atan(";
        ost<<this->StringStack[stackPosition];
        ost<<")";
        this->StringStack[stackPosition]=ost.str();
        // Update the precedence.
        this->PrecedenceStack[stackPosition]=2; // function is 2
        dim=1;
        break;
      case VTK_PARSER_HYPERBOLIC_SINE:
        // sinh(x)=0.5*(exp(x)-exp(-x))
        ost<<"0.5*(exp(";
        ost<<this->StringStack[stackPosition];
        ost<<")-exp(-";
        if(this->PrecedenceStack[stackPosition]>3) // unary is 3
          {
          // need to be protected by parenthesis
          ost<<"(";
          }
        ost<<this->StringStack[stackPosition];
        if(this->PrecedenceStack[stackPosition]>3) // unary is 3
          {
          // need to be protected by parenthesis
          ost<<")";
          }
        ost<<"))";
        this->StringStack[stackPosition]=ost.str();
        // Update the precedence.
        this->PrecedenceStack[stackPosition]=4; // multiplicative is 4
        dim=1;
        break;
      case VTK_PARSER_HYPERBOLIC_COSINE:
        // cosh(x)=0.5*(exp(x)+exp(-x))
        ost<<"0.5*(exp(";
        ost<<this->StringStack[stackPosition];
        ost<<")+exp(-";
        if(this->PrecedenceStack[stackPosition]>3) // unary is 3
          {
          // need to be protected by parenthesis
          ost<<"(";
          }
        ost<<this->StringStack[stackPosition];
        if(this->PrecedenceStack[stackPosition]>3) // unary is 3
          {
          // need to be protected by parenthesis
          ost<<")";
          }
        ost<<"))";
        this->StringStack[stackPosition]=ost.str();
        // Update the precedence.
        this->PrecedenceStack[stackPosition]=4; // multiplicative is 4
        dim=1;
        break;
      case VTK_PARSER_HYPERBOLIC_TANGENT:
        // tanh(x)=(exp(2.0*x)-1.0)/(exp(2.0*x)+1.0)
        ost<<"(exp(2.0*";
        if(this->PrecedenceStack[stackPosition]>4) // multiplicative is 4
          {
          // need to be protected by parenthesis
          ost<<"(";
          }
        ost<<this->StringStack[stackPosition];
        if(this->PrecedenceStack[stackPosition]>4) // multiplicative is 4
          {
          // need to be protected by parenthesis
          ost<<")";
          }
        ost<<")-1.0)/(exp(2.0*";
        if(this->PrecedenceStack[stackPosition]>4) // multiplicative is 4
          {
          // need to be protected by parenthesis
          ost<<"(";
          }
        ost<<this->StringStack[stackPosition];
        if(this->PrecedenceStack[stackPosition]>4) // multiplicative is 4
          {
          // need to be protected by parenthesis
          ost<<")";
          }
        ost<<")+1.0)";
        this->StringStack[stackPosition]=ost.str();
        // Update the precedence.
        this->PrecedenceStack[stackPosition]=4; // multiplicative is 4
        dim=1;
        break;
      case VTK_PARSER_MIN:
        ost<<"min(";
        ost<<this->StringStack[stackPosition-1];
        ost<<",";
        ost<<this->StringStack[stackPosition];
        ost<<")";
        this->StringStack[stackPosition-1]=ost.str();
        // Update the precedence.
        this->PrecedenceStack[stackPosition-1]=2; // function is 2
        --stackPosition;
        dim=1;
        break;
      case VTK_PARSER_MAX:
        ost<<"max(";
        ost<<this->StringStack[stackPosition-1];
        ost<<",";
        ost<<this->StringStack[stackPosition];
        ost<<")";
        this->StringStack[stackPosition-1]=ost.str();
        // Update the precedence.
        this->PrecedenceStack[stackPosition-1]=2; // function is 2
        --stackPosition;
        dim=1;
        break;
      case VTK_PARSER_CROSS: // vector
        ost<<"cross(";
#if 0 // with field selector
        if(this->PrecedenceStack[stackPosition-1]>2) // field selector is 2
          {
          // need to be protected by parenthesis
          ost<<"(";
          }
        ost<<this->StringStack[stackPosition-1];
        if(this->PrecedenceStack[stackPosition-1]>2) // field selector is 2
          {
          // need to be protected by parenthesis
          ost<<")";
          }
        ost<<".xyz,";
        if(this->PrecedenceStack[stackPosition]>2) // field selector is 2
          {
          // need to be protected by parenthesis
          ost<<"(";
          }
        ost<<this->StringStack[stackPosition];
        if(this->PrecedenceStack[stackPosition]>2) // field selector is 2
          {
          // need to be protected by parenthesis
          ost<<")";
          }
        ost<<".xyz)";
#else // without field selector
        ost<<this->StringStack[stackPosition-1];
        ost<<",";
        ost<<this->StringStack[stackPosition];
        ost<<")";
#endif

        // Update the precedence.
        this->PrecedenceStack[stackPosition-1]=2; // function is 2
        --stackPosition;
        dim=3;
        break;
      case VTK_PARSER_SIGN:
        ost<<"sign(";
        ost<<this->StringStack[stackPosition];
        ost<<")";
        this->StringStack[stackPosition]=ost.str();
        // Update the precedence.
        this->PrecedenceStack[stackPosition]=2; // function is 2
        dim=1;
        break;
      case VTK_PARSER_VECTOR_UNARY_MINUS:
        ost<<"-";
        if(this->PrecedenceStack[stackPosition]>3) // unary is 3
          {
          // need to be protected by parenthesis
          ost<<"(";
          }
        ost<<this->StringStack[stackPosition];
        if(this->PrecedenceStack[stackPosition]>3) // unary is 3
          {
          // need to be protected by parenthesis
          ost<<")";
          }
        this->StringStack[stackPosition]=ost.str();
        // Update the precedence.
        this->PrecedenceStack[stackPosition]=3;
        dim=3;
        break;
      case VTK_PARSER_DOT_PRODUCT:
        ost<<"dot(";
        ost<<this->StringStack[stackPosition-1];
        ost<<",";
        ost<<this->StringStack[stackPosition];
        ost<<")";
        this->StringStack[stackPosition-1]=ost.str();
        // Update the precedence.
        this->PrecedenceStack[stackPosition-1]=2; // function is 2
        --stackPosition;
        dim=1;
        break;
      case VTK_PARSER_VECTOR_ADD:
        if(this->PrecedenceStack[stackPosition-1]>5) // additive is 5
          {
          // need to be protected by parenthesis
          ost<<"(";
          }
        ost<<this->StringStack[stackPosition-1];
        if(this->PrecedenceStack[stackPosition-1]>5) // additive is 5
          {
          // need to be protected by parenthesis
          ost<<")";
          }
        ost<<"+";
        if(this->PrecedenceStack[stackPosition]>5) // additive is 5
          {
          // need to be protected by parenthesis
          ost<<"(";
          }
        ost<<this->StringStack[stackPosition];
        if(this->PrecedenceStack[stackPosition]>5) // additive is 5
          {
          // need to be protected by parenthesis
          ost<<")";
          }
        this->StringStack[stackPosition-1]=ost.str();
        // Update the precedence.
        this->PrecedenceStack[stackPosition-1]=5;
        --stackPosition;
        dim=3;
        break;
      case VTK_PARSER_VECTOR_SUBTRACT:
        if(this->PrecedenceStack[stackPosition-1]>5) // additive is 5
          {
          // need to be protected by parenthesis
          ost<<"(";
          }
        ost<<this->StringStack[stackPosition-1];
        if(this->PrecedenceStack[stackPosition-1]>5) // additive is 5
          {
          // need to be protected by parenthesis
          ost<<")";
          }
        ost<<"-";
        if(this->PrecedenceStack[stackPosition]>5) // additive is 5
          {
          // need to be protected by parenthesis
          ost<<"(";
          }
        ost<<this->StringStack[stackPosition];
        if(this->PrecedenceStack[stackPosition]>5) // additive is 5
          {
          // need to be protected by parenthesis
          ost<<")";
          }
        this->StringStack[stackPosition-1]=ost.str();
        // Update the precedence.
        this->PrecedenceStack[stackPosition-1]=5;
        --stackPosition;
        dim=3;
        break;
      case VTK_PARSER_SCALAR_TIMES_VECTOR:
        if(this->PrecedenceStack[stackPosition-1]>4) // multiplicative is 4
          {
          // need to be protected by parenthesis
          ost<<"(";
          }
        // scalar
        ost<<this->StringStack[stackPosition-1];
        if(this->PrecedenceStack[stackPosition-1]>4) // multiplicative is 4
          {
          // need to be protected by parenthesis
          ost<<")";
          }
        ost<<"*";
        if(this->PrecedenceStack[stackPosition]>4) // multiplicative is 4
          {
          // need to be protected by parenthesis
          ost<<"(";
          }
        // vector
        ost<<this->StringStack[stackPosition];
        if(this->PrecedenceStack[stackPosition]>4) // multiplicative is 4
          {
          // need to be protected by parenthesis
          ost<<")";
          }
        this->StringStack[stackPosition-1]=ost.str();
        // Update the precedence.
        this->PrecedenceStack[stackPosition-1]=4;
        --stackPosition;
        dim=3;
        break;
      case VTK_PARSER_VECTOR_TIMES_SCALAR:
        if(this->PrecedenceStack[stackPosition-1]>4) // multiplicative is 4
          {
          // need to be protected by parenthesis
          ost<<"(";
          }
        // vector
        ost<<this->StringStack[stackPosition-1];
        if(this->PrecedenceStack[stackPosition-1]>4) // multiplicative is 4
          {
          // need to be protected by parenthesis
          ost<<")";
          }
        ost<<"*";
        if(this->PrecedenceStack[stackPosition]>4) // multiplicative is 4
          {
          // need to be protected by parenthesis
          ost<<"(";
          }
        // scalar
        ost<<this->StringStack[stackPosition];
        if(this->PrecedenceStack[stackPosition]>4) // multiplicative is 4
          {
          // need to be protected by parenthesis
          ost<<")";
          }
        this->StringStack[stackPosition-1]=ost.str();
        // Update the precedence.
        this->PrecedenceStack[stackPosition-1]=4;
        --stackPosition;
        dim=3;
        break;
      case VTK_PARSER_MAGNITUDE:
        ost<<"length(";
        ost<<this->StringStack[stackPosition];
        ost<<")";
        this->StringStack[stackPosition]=ost.str();
        // Update the precedence.
        this->PrecedenceStack[stackPosition]=2; // function is 2
        dim=1;
        break;
      case VTK_PARSER_NORMALIZE:
        ost<<"normalize(";
        ost<<this->StringStack[stackPosition];
        ost<<")";
        this->StringStack[stackPosition]=ost.str();
        // Update the precedence.
        this->PrecedenceStack[stackPosition]=2; // function is 2
        dim=3;
        break;
      case VTK_PARSER_IHAT:
        ++stackPosition;
        this->StringStack[stackPosition]="vec3(1.0,0.0,0.0)";
        this->PrecedenceStack[stackPosition]=2; // constructor
        dim=3;
        break;
      case VTK_PARSER_JHAT:
        ++stackPosition;
        this->StringStack[stackPosition]="vec3(0.0,1.0,0.0)";
        this->PrecedenceStack[stackPosition]=2; // constructor
        dim=3;
        break;
      case VTK_PARSER_KHAT:
        ++stackPosition;
        this->StringStack[stackPosition]="vec3(0.0,0.0,1.0)";
        this->PrecedenceStack[stackPosition]=2; // constructor
        dim=3;
        break;
      case VTK_PARSER_LESS_THAN:
        if(this->PrecedenceStack[stackPosition-1]>7) // relational is 7
          {
          // need to be protected by parenthesis
          ost<<"(";
          }
        ost<<this->StringStack[stackPosition-1];
        if(this->PrecedenceStack[stackPosition-1]>7) // relational is 7
          {
          // need to be protected by parenthesis
          ost<<")";
          }
        ost<<"<";
        if(this->PrecedenceStack[stackPosition]>7) // relational is 7
          {
          // need to be protected by parenthesis
          ost<<"(";
          }
        ost<<this->StringStack[stackPosition];
        if(this->PrecedenceStack[stackPosition]>7) // relational is 7
          {
          // need to be protected by parenthesis
          ost<<")";
          }
        this->StringStack[stackPosition-1]=ost.str();
        // Update the precedence.
        this->PrecedenceStack[stackPosition-1]=7;
        --stackPosition;
        dim=1;
        break;
      case VTK_PARSER_GREATER_THAN:
        if(this->PrecedenceStack[stackPosition-1]>7) // relational is 7
          {
          // need to be protected by parenthesis
          ost<<"(";
          }
        ost<<this->StringStack[stackPosition-1];
        if(this->PrecedenceStack[stackPosition-1]>7) // relational is 7
          {
          // need to be protected by parenthesis
          ost<<")";
          }
        ost<<">";
        if(this->PrecedenceStack[stackPosition]>7) // relational is 7
          {
          // need to be protected by parenthesis
          ost<<"(";
          }
        ost<<this->StringStack[stackPosition];
        if(this->PrecedenceStack[stackPosition]>7) // relational is 7
          {
          // need to be protected by parenthesis
          ost<<")";
          }
        this->StringStack[stackPosition-1]=ost.str();
        // Update the precedence.
        this->PrecedenceStack[stackPosition-1]=7;
        --stackPosition;
        dim=1;
        break;
      case VTK_PARSER_EQUAL_TO:
        if(this->PrecedenceStack[stackPosition-1]>8) // equality is 8
          {
          // need to be protected by parenthesis
          ost<<"(";
          }
        ost<<this->StringStack[stackPosition-1];
        if(this->PrecedenceStack[stackPosition-1]>8) // equality is 8
          {
          // need to be protected by parenthesis
          ost<<")";
          }
        ost<<"==";
        if(this->PrecedenceStack[stackPosition]>8) // equality is 8
          {
          // need to be protected by parenthesis
          ost<<"(";
          }
        ost<<this->StringStack[stackPosition];
        if(this->PrecedenceStack[stackPosition]>8) // equality is 8
          {
          // need to be protected by parenthesis
          ost<<")";
          }
        this->StringStack[stackPosition-1]=ost.str();
        // Update the precedence.
        this->PrecedenceStack[stackPosition-1]=8;
        --stackPosition;
        dim=1;
        break;
      case VTK_PARSER_AND:
        if(this->PrecedenceStack[stackPosition-1]>12) // logical and is 12
          {
          // need to be protected by parenthesis
          ost<<"(";
          }
        ost<<this->StringStack[stackPosition-1];
        if(this->PrecedenceStack[stackPosition-1]>12) // logical and is 12
          {
          // need to be protected by parenthesis
          ost<<")";
          }
        ost<<"&&";
        if(this->PrecedenceStack[stackPosition]>12) // logical and is 12
          {
          // need to be protected by parenthesis
          ost<<"(";
          }
        ost<<this->StringStack[stackPosition];
        if(this->PrecedenceStack[stackPosition]>12) // logical and is 12
          {
          // need to be protected by parenthesis
          ost<<")";
          }
        this->StringStack[stackPosition-1]=ost.str();
        // Update the precedence.
        this->PrecedenceStack[stackPosition-1]=12;
        --stackPosition;
        dim=1;
        break;
      case VTK_PARSER_OR:
        if(this->PrecedenceStack[stackPosition-1]>14) // logical inclusive or is 14
          {
          // need to be protected by parenthesis
          ost<<"(";
          }
        ost<<this->StringStack[stackPosition-1];
        if(this->PrecedenceStack[stackPosition-1]>14) // logical inclusive or is 14
          {
          // need to be protected by parenthesis
          ost<<")";
          }
        ost<<"&&";
        if(this->PrecedenceStack[stackPosition]>14) // logical inclusive or is 14
          {
          // need to be protected by parenthesis
          ost<<"(";
          }
        ost<<this->StringStack[stackPosition];
        if(this->PrecedenceStack[stackPosition]>14) // logical inclusive or is 14
          {
          // need to be protected by parenthesis
          ost<<")";
          }
        this->StringStack[stackPosition-1]=ost.str();
        // Update the precedence.
        this->PrecedenceStack[stackPosition-1]=14;
        --stackPosition;
        dim=1;
        break;
      case VTK_PARSER_IF:
      {
      // Stack[stackPosition]=Stack[2] refers to the bool
      // argument of if(bool,valtrue,valfalse). Stack[1] is valtrue, and
      // Stack[0] is valfalse.
      if(this->PrecedenceStack[stackPosition]>15) // selection is 15
        {
        // need to be protected by parenthesis
        ost<<"(";
        }
      ost<<this->StringStack[stackPosition];
      if(this->PrecedenceStack[stackPosition]>15) // selection is 15
        {
        // need to be protected by parenthesis
        ost<<")";
        }
      ost<<"?";
      if(this->PrecedenceStack[stackPosition-1]>15) // selection is 15
        {
        // need to be protected by parenthesis
        ost<<"(";
        }
      ost<<this->StringStack[stackPosition-1];
      if(this->PrecedenceStack[stackPosition-1]>15) // selection is 15
        {
        // need to be protected by parenthesis
        ost<<")";
        }
      ost<<":";
      if(this->PrecedenceStack[stackPosition-2]>15) // selection is 15
        {
        // need to be protected by parenthesis
        ost<<"(";
        }
      ost<<this->StringStack[stackPosition-2];
      if(this->PrecedenceStack[stackPosition-2]>15) // selection is 15
        {
        // need to be protected by parenthesis
        ost<<")";
        }
      this->StringStack[stackPosition-2]=ost.str();
      // Update the precedence.
      this->PrecedenceStack[stackPosition-2]=15;
      stackPosition-=2;
      dim=1;
      break;
      }
      case VTK_PARSER_VECTOR_IF:
      {
      // Stack[stackPosition]=Stack[2] refers to the bool (scalar)
      // argument of if(bool,valtrue,valfalse). Stack[1] is valtrue
      // (vector), and Stack[0] is valfalse (vector).
      if(this->PrecedenceStack[stackPosition]>15) // selection is 15
        {
        // need to be protected by parenthesis
        ost<<"(";
        }
      ost<<this->StringStack[stackPosition];
      if(this->PrecedenceStack[stackPosition]>15) // selection is 15
        {
        // need to be protected by parenthesis
        ost<<")";
        }
      ost<<"?";
      if(this->PrecedenceStack[stackPosition-1]>15) // selection is 15
        {
        // need to be protected by parenthesis
        ost<<"(";
        }
      ost<<this->StringStack[stackPosition-1];
      if(this->PrecedenceStack[stackPosition-1]>15) // selection is 15
        {
        // need to be protected by parenthesis
        ost<<")";
        }
      ost<<":";
      if(this->PrecedenceStack[stackPosition-2]>15) // selection is 15
        {
        // need to be protected by parenthesis
        ost<<"(";
        }
      ost<<this->StringStack[stackPosition-2];
      if(this->PrecedenceStack[stackPosition-2]>15) // selection is 15
        {
        // need to be protected by parenthesis
        ost<<")";
        }
      this->StringStack[stackPosition-2]=ost.str();
      // Update the precedence.
      this->PrecedenceStack[stackPosition-2]=15;
      stackPosition-=2;
      dim=1;
      break;
      }
      default:
        ++stackPosition;
        int variableId=
          this->ByteCode[numBytesProcessed]-VTK_PARSER_BEGIN_VARIABLES;
        if(variableId< this->NumberOfScalarVariables) // scalar
          {
          ost<<"scalarValue";
          dim=1;
          }
        else // vector
          {
          variableId-=this->NumberOfScalarVariables;
          ost<<"vectorValue";
          dim=3;
          }
        ost<<variableId;
        this->StringStack[stackPosition]=ost.str();
        // Update the precedence.
        this->PrecedenceStack[stackPosition]=1; // highest GLSL one
      }
    }

  vtksys_ios::ostringstream ost2;

  // It has to be the first line of code.
  ost2 << "#version 110" << endl;

  int c=this->NumberOfScalarVariables;
  int i=0;
  while(i<c)
    {
    if(this->ScalarIsUsed[i])
      {
      ost2<<"uniform sampler2D scalar"<<i<<";"<<endl;
      }
    ++i;
    }
  c=this->NumberOfVectorVariables;
  i=0;
  while(i<c)
    {
    if(this->VectorIsUsed[i])
      {
      ost2<<"uniform sampler2D vector"<<i<<";"<<endl;
      }
    ++i;
    }
  ost2<<"void main(void) "<<endl;
  ost2<<"{"<<endl;

  c=this->NumberOfScalarVariables;
  i=0;
  while(i<c)
    {
    if(this->ScalarIsUsed[i])
      {
      ost2<<" float scalarValue"<<i<<"=texture2D(scalar"<<i<<",gl_TexCoord[0].xy).x;"<<endl;
      }
    ++i;
    }
  c=this->NumberOfVectorVariables;
  i=0;
  while(i<c)
    {
    if(this->VectorIsUsed[i])
      {
      ost2<<" vec3 vectorValue"<<i<<"=texture2D(vector"<<i<<",gl_TexCoord[0].xy).xyz;"<<endl;
      }
    ++i;
    }

  ost2<<" gl_FragData[0]=vec4("; // vector or scalar.
  ost2<<this->StringStack[stackPosition];
  if(dim==1)
    {
    ost2<<")";
    }
  else
    {
    assert("check: vector" && dim==3);
    ost2<<",0.0)";
    }
  ost2<<";"<<endl;

  ost2<<"}"<<endl;

  this->ResultDimension=dim;

  (*this->GLSLCode)=ost2.str();

  this->EvaluateMTime.Modified();
}

// ----------------------------------------------------------------------------
// Description:
// Return if a given scalar is used in the function expression.
// \pre valid_index:index>=0 && index<this->NumberOfScalarVariables
bool vtkKWEFunctionToGLSL::GetScalarIsUsed(int index)
{
  assert("pre: valid_index" && index>=0 && index<this->NumberOfScalarVariables);

  return this->ScalarIsUsed[index];
}

// ----------------------------------------------------------------------------
// Description:
// Return if a given vector is used in the function expression.
// \pre valid_index:index>=0 && index<this->NumberOfVectorVariables
bool vtkKWEFunctionToGLSL::GetVectorIsUsed(int index)
{
  assert("pre: valid_index" && index>=0 && index<this->NumberOfVectorVariables);

  return this->VectorIsUsed[index];
}

// ----------------------------------------------------------------------------
// Description:
// Return the GLSL name attached to the a given scalar data array.
// \pre valid_index:index>=0 && index<this->NumberOfScalarVariables
// \pre used_scalar: this->GetScalarIsUsed(index)
vtkStdString *vtkKWEFunctionToGLSL::GetGLSLScalarName(int index)
{
  assert("pre: valid_index" && index>=0 && index<this->NumberOfScalarVariables);
  assert("pre: used_scalar" && this->GetScalarIsUsed(index));

  return &this->GLSLScalarNames[index];
}

// ----------------------------------------------------------------------------
// Description:
// Return the GLSL name attached to the a given vector data array.
// \pre valid_index:index>=0 && index<this->NumberOfVectorVariables
// \pre used_vector: this->GetVectorIsUsed(index)
vtkStdString *vtkKWEFunctionToGLSL::GetGLSLVectorName(int index)
{
  assert("pre: valid_index" && index>=0 && index<this->NumberOfVectorVariables);
  assert("pre: used_vector" && this->GetVectorIsUsed(index));

  return &this->GLSLVectorNames[index];
}

// ----------------------------------------------------------------------------
// Description:
// Return the number of scalar variables used in the expression.
// \post valid_result: result>=0 && result<=GetNumberOfScalarVariables()
int vtkKWEFunctionToGLSL::GetNumberOfUsedScalarVariables()
{
  assert("post: valid_result" && this->NumberOfUsedScalarVariables>=0 &&
         this->NumberOfUsedScalarVariables<=this->GetNumberOfScalarVariables());
  return this->NumberOfUsedScalarVariables;
}

// ----------------------------------------------------------------------------
// Description:
// Return the number of vector variables used in the expression.
// \post valid_result: result>=0 && result<=GetNumberOfVectorVariables()
int vtkKWEFunctionToGLSL::GetNumberOfUsedVectorVariables()
{
  assert("post: valid_result" && this->NumberOfUsedVectorVariables>=0 &&
         this->NumberOfUsedVectorVariables<=this->GetNumberOfVectorVariables());
  return this->NumberOfUsedVectorVariables;
}

// ----------------------------------------------------------------------------
// Description:
// Build the name of the variable for GLSL. Only load the variables used
// in the function expression.
// \pre parsed: bytcode exists and is valid
void vtkKWEFunctionToGLSL::BuildGLSLVariableNames()
{
  if(this->ScalarIsUsed!=0 &&
     this->ScalarIsUsedSize!=this->NumberOfScalarVariables)
    {
#if 0
    int c=this->ScalarIsUsedSize;
    int i=0;
    while(i<c)
      {
      if(this->GLSLScalarNames[i]!=0)
        {
        delete this->GLSLScalarNames[i];
        }
      ++i;
      }
#endif
    delete[] this->GLSLScalarNames;
    this->GLSLScalarNames=0;

    delete[] this->ScalarIsUsed;
    this->ScalarIsUsed=0;
    this->ScalarIsUsedSize=0;
    }

  if(this->VectorIsUsed!=0 &&
     this->VectorIsUsedSize!=this->NumberOfVectorVariables)
    {
#if 0
    int c=this->VectorIsUsedSize;
    int i=0;
    while(i<c)
      {
      if(this->GLSLVectorNames[i]!=0)
        {
        delete this->GLSLVectorNames[i];
        }
      ++i;
      }
#endif
    delete[] this->GLSLVectorNames;
    this->GLSLVectorNames=0;

    delete[] this->VectorIsUsed;
    this->VectorIsUsed=0;
    this->VectorIsUsedSize=0;
    }

  int i;
  int c;

  c=this->NumberOfScalarVariables;
  if(c>0)
    {
    if(this->ScalarIsUsed==0)
      {
      this->ScalarIsUsed=new bool[c];
      this->ScalarIsUsedSize=c;
      this->GLSLScalarNames=new vtkStdString[c];
      }
    i=0;
    while(i<c)
      {
      this->ScalarIsUsed[i]=false;
      vtksys_ios::ostringstream ost;
      ost<<"scalar";
      ost<<i;
      this->GLSLScalarNames[i]=ost.str();
      ++i;
      }
    }

  c=this->NumberOfVectorVariables;
  if(c>0)
    {
    if(this->VectorIsUsed==0)
      {
      this->VectorIsUsed=new bool[c];
      this->VectorIsUsedSize=c;
      this->GLSLVectorNames=new vtkStdString[c];
      }

    i=0;
    while(i<c)
      {
      this->VectorIsUsed[i]=false;
      vtksys_ios::ostringstream ost;
      ost<<"vector";
      ost<<i;
      this->GLSLVectorNames[i]=ost.str();
      ++i;
      }
    }

  this->NumberOfUsedScalarVariables=0;
  this->NumberOfUsedVectorVariables=0;

  i=0;
  c=this->ByteCodeSize;
  while(i<c)
    {
    int j=this->ByteCode[i]-VTK_PARSER_BEGIN_VARIABLES;
    if(j>=0)
      {
      if(j<this->NumberOfScalarVariables)
        {
        // scalar variable
        if(!this->ScalarIsUsed[j])
          {
          this->ScalarIsUsed[j]=true;
          ++this->NumberOfUsedScalarVariables;
          }
        }
      else
        {
        // vector variable
        j-=this->NumberOfScalarVariables;
        if(!this->VectorIsUsed[j])
          {
          this->VectorIsUsed[j]=true;
          ++this->NumberOfUsedVectorVariables;
          }
        }
      }
    ++i;
    }

#if 0
  if(usedScalars!=0)
    {
    delete[] usedScalars;
    }
  if(usedVectors!=0)
    {
    delete[] usedVectors;
    }
#endif
}

// ----------------------------------------------------------------------------
// Description:
// Return the GLSLCode generated by GenerateGLSL() in a string.
// \pre valid_expression: GetParseStatus()
vtkStdString *vtkKWEFunctionToGLSL::GetGLSLCode()
{
  assert("pre: valid_expression" && this->GetParseStatus());
  return this->GLSLCode;
}

// ----------------------------------------------------------------------------
// Description:
// Default constructor. The function expression is a null pointer.
// The GLSLCode is a null pointer. ParseStatus is false.
vtkKWEFunctionToGLSL::vtkKWEFunctionToGLSL()
{
  this->ParseStatus=false;
  this->GLSLCode=new vtkStdString;
  this->StringStack=0;
  this->PrecedenceStack=0;
  this->ScalarIsUsed=0;
  this->ScalarIsUsedSize=0;
  this->VectorIsUsed=0;
  this->VectorIsUsedSize=0;
  this->NumberOfUsedScalarVariables=0;
  this->NumberOfUsedVectorVariables=0;
}

// ----------------------------------------------------------------------------
// Description:
// Destructor.
vtkKWEFunctionToGLSL::~vtkKWEFunctionToGLSL()
{
  if(this->GLSLCode!=0)
    {
    delete this->GLSLCode;
    this->GLSLCode=0;
    }
  if(this->StringStack!=0)
    {
    this->DeleteStringStack();
    }
  if(this->PrecedenceStack!=0)
    {
    delete [] this->PrecedenceStack;
    this->PrecedenceStack=0;
    }

  if(this->ScalarIsUsed!=0)
    {
    delete[] this->GLSLScalarNames;
    this->GLSLScalarNames=0;

    delete[] this->ScalarIsUsed;
    this->ScalarIsUsed=0;
    this->ScalarIsUsedSize=0;
    }

  if(this->VectorIsUsed!=0)
    {
    delete[] this->GLSLVectorNames;
    this->GLSLVectorNames=0;

    delete[] this->VectorIsUsed;
    this->VectorIsUsed=0;
    this->VectorIsUsedSize=0;
    }
}

// ----------------------------------------------------------------------------
void vtkKWEFunctionToGLSL::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
