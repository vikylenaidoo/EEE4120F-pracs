#include "main.h"
//------------------------------------------------------------------------------

// GPU reset?  See <http://stackoverflow.com/questions/12259044/
//                  limitations-of-work-item-load-in-gpu-cuda-opencl>

// <https://social.technet.microsoft.com/Forums/windows/en-US/
// 92a45329-3dd1-4c42-8a53-42dd232edd81/
// how-to-turn-off-timeout-detection-and-recovery-of-gpus>
//------------------------------------------------------------------------------

void Process_Serial(){
 int i, j, k;
 float result;

 for(j = 0; j < N; j++){
  for(i = 0; i < N; i++){
   result = 0;
   for(k = 0; k < N; k++){
    result += A[N*i + k] * B[N*k + j];
   }
   Output_Serial[N*j + i] = result;
  }
 }
}
//------------------------------------------------------------------------------

void Process_OpenCL(){
 printf("\n");
 OpenCL_ConstantInt(3, N);
 
 transfer_overhead = 0;
 processing_time = 0;

 tic();
 OpenCL_WriteData(A_Buffer, N*N*sizeof(float), A);
 OpenCL_WriteData(B_Buffer, N*N*sizeof(float), B);
 transfer_overheads[N-1] = toc()/1e-3;
 
 //processing time
 tic();
 OpenCL_Run(N, LocalSize);
 processing_times[N-1] = toc()/1e-3; 
 printf("\nProcessing Time: %lg ms\n\n", processing_time);

 tic();
 OpenCL_ReadData(OutputBuffer, N*N*sizeof(float), Output_OpenCL);
 transfer_overheads[N-1] += toc()/1e-3;
 printf("\nTransfer Time: %lg ms\n\n", transfer_overhead);
}
//------------------------------------------------------------------------------

void Compare(){
 int i, j;

 printf("A:\n");
 for(j = 0; j < N; j++){
  for(i = 0; i < N; i++){
   printf("\t%g", A[N*j + i]);
  }
  printf("\n");
 }

 printf("B:\n");
 for(j = 0; j < N; j++){
  for(i = 0; i < N; i++){
   printf("\t%g", B[N*j + i]);
  }
  printf("\n");
 }

 printf("Output_Serial:\n");
 for(j = 0; j < N; j++){
  for(i = 0; i < N; i++){
   printf("\t%g", Output_Serial[N*j + i]);
  }
  printf("\n");
 }

 printf("Output_OpenCL:\n");
 for(j = 0; j < N; j++){
  for(i = 0; i < N; i++){
   printf("\t%g", Output_OpenCL[N*j + i]);
  }
  printf("\n");
 }
}
//------------------------------------------------------------------------------

void Fill(float* A){
 int i, j;

 for(j = 0; j < N; j++){
  for(i = 0; i < N; i++){
   A[N*j + i] = rand() % 20;
  }
 }
}
//------------------------------------------------------------------------------

int main(){
N=1;
while(N<=30){
printf("N = %d", N);
 // Initialise OpenCL
 if(
  !OpenCL_Init("NVIDIA"                ) && // nVidia
  !OpenCL_Init("Advanced Micro Devices") && // AMD
  !OpenCL_Init(0                       )    // Default
 ){
  printf("Error: Cannot initialise OpenCL.\n");
  return 1;
 }

 // Load a kernel
 if(!OpenCL_LoadKernel("OpenCL/Kernel.cl", "Multiply")) return 1;

 
 //N = 250;
 size_t BufferSize = N*N*sizeof(float);

 // Allocate CPU RAM
 A = (float*)malloc(BufferSize);
 B = (float*)malloc(BufferSize);

 Output_Serial = (float*)malloc(BufferSize);
 Output_OpenCL = (float*)malloc(BufferSize);

 Fill(A);
 Fill(B);

 // Allocate GPU RAM
 OpenCL_PrepareLocalSize(N, LocalSize);
 A_Buffer     = OpenCL_CreateBuffer(0, CL_MEM_READ_ONLY , BufferSize);
 B_Buffer     = OpenCL_CreateBuffer(1, CL_MEM_READ_ONLY , BufferSize);
 OutputBuffer = OpenCL_CreateBuffer(2, CL_MEM_WRITE_ONLY, BufferSize);

 // Process the matrices
 tic();
 Process_Serial();
 //printf("Serial: %lg ms\n", toc()/1e-3);
 serial_times[N-1] = toc()/1e-3;
 //printf("Serial: %lg ms\n", serial_times[N-1]);
 

 //tic();
 Process_OpenCL();
 //printf("OpenCL: %lg ms\n", toc()/1e-3);
// opencl_times[N-1] = toc()/1e-3;
// printf("\nOpenCL: %lg ms\n\n", opencl_times[N-1]);

 // Compare results
 //Compare();

 // Clean-up
 free(A);
 free(B);
 free(Output_Serial);
 free(Output_OpenCL);

 OpenCL_FreeBuffer(A_Buffer    );
 OpenCL_FreeBuffer(B_Buffer    );
 OpenCL_FreeBuffer(OutputBuffer);
 OpenCL_Destroy();

N=N+1;
}

printf("N,Transfer,Processing,Serial\n");
for(int i=0; i<30; i++){
	printf("%d,%f,%f,%f\n", (i+1), transfer_overheads[i], processing_times[i],serial_times[i]);
}
 return 0;
}
//------------------------------------------------------------------------------
