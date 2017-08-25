
#include <cutil.h>
#include <cufft.h>

// Defined in vtkImageFourierFilter.h
// We re-difine it here so nvcc can compile it.
typedef struct{
    float Real;
    float Imag;
  } vtkImageComplexf;


__global__ void Scale(float2 * in, int N, float scale)
{
  const int numThreads = blockDim.x * gridDim.x;
  const int threadID = blockIdx.x * blockDim.x + threadIdx.x;
  for (int i = threadID; i < N; i += numThreads)
    {
    in[i].x *= scale;
    in[i].y *= scale;
    }
}

extern "C"
void ExecuteRFft(vtkImageComplexf *in, vtkImageComplexf *out, int N)
{
  // If a given dimenion is 1 we just copy input to output. (e.q. when 
  // processing a 256 x 256 x 1 data set, we dont need to do a 3rd FFT 
  // in the Z dimension)
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
  CUFFT_SAFE_CALL(cufftExecC2C(plan, (cufftComplex *)d_signal, (cufftComplex *)d_signal, CUFFT_INVERSE));

  // Normalize the result
  Scale<<<32, N>>>(d_signal, N, 1.0f/N);

  // Check if kernel execution generated and error
  CUT_CHECK_ERROR("Kernel execution failed [ ComplexPointwiseMulAndScale ]");

  // Copy device memory to host
  CUDA_SAFE_CALL(cudaMemcpy(out, d_signal, mem_size, cudaMemcpyDeviceToHost));

  //Destroy CUFFT context
  CUFFT_SAFE_CALL(cufftDestroy(plan));

  // cleanup memory
  CUDA_SAFE_CALL(cudaFree(d_signal));
}