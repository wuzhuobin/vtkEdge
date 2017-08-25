
#include <cutil.h>
#include <cufft.h>

// Defined in vtkImageFourierFilter.h
// We re-difine it here so nvcc can compile it.
typedef struct{
    float Real;
    float Imag;
  } vtkImageComplexf;

extern "C"
void ExecuteFft(vtkImageComplexf *in, vtkImageComplexf *out, int N)
{
  if(N == 1)
    {
    *out++ = *in++;
    return;
    }

  //CUT_DEVICE_INIT();

  int mem_size = sizeof(vtkImageComplexf) * N;

  // Allocate device memory for signal
  float2 * d_signal;
  CUDA_SAFE_CALL(cudaMalloc((void**)&d_signal, mem_size));
  // Copy host memory to device
  CUDA_SAFE_CALL(cudaMemcpy(d_signal, in, mem_size, cudaMemcpyHostToDevice));

  // CUFFT plan
  cufftHandle plan;
  CUFFT_SAFE_CALL(cufftPlan1d(&plan, N, CUFFT_C2C, 1));

  // Transform signal
  CUFFT_SAFE_CALL(cufftExecC2C(plan, (cufftComplex *)d_signal, (cufftComplex *)d_signal, CUFFT_FORWARD));

  // Check if kernel execution generated and error
  CUT_CHECK_ERROR("Kernel execution failed [ ComplexPointwiseMulAndScale ]");

  // Copy device memory to host
  CUDA_SAFE_CALL(cudaMemcpy(out, d_signal, mem_size, cudaMemcpyDeviceToHost));

  //Destroy CUFFT context
  CUFFT_SAFE_CALL(cufftDestroy(plan));

  // cleanup memory
  CUDA_SAFE_CALL(cudaFree(d_signal));
}