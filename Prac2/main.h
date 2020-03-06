#include <stdlib.h>
//------------------------------------------------------------------------------

#include "Timer.h"
#include "OpenCL_Wrapper.h"
//------------------------------------------------------------------------------

// Useful globals
int    N;
size_t LocalSize[2] = {1, 1};
float serial_times[30];
float opencl_times[100];
float transfer_overhead;
float transfer_overheads[30];
float processing_time;
float processing_times[30];

//------------------------------------------------------------------------------

// CPU Memory Handles
float* A;
float* B;
float* Output_Serial;
float* Output_OpenCL;
//------------------------------------------------------------------------------

// GPU Memory Handles
cl_mem A_Buffer;
cl_mem B_Buffer;
cl_mem OutputBuffer;
//------------------------------------------------------------------------------
