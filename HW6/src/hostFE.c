#include <stdio.h>
#include <stdlib.h>
#include "hostFE.h"
#include "helper.h"


void hostFE(int filterWidth, float *filter, int imageHeight, int imageWidth,
            float *inputImage, float *outputImage, cl_device_id *device,
            cl_context *context, cl_program *program)
{
    cl_int ciErrNum;
    int filterSize = filterWidth * filterWidth * sizeof(float);
    int mem_size = imageHeight * imageWidth * sizeof(float);

    /*step 0: initialize Device
              create command queue */
    cl_command_queue myqueue;
    myqueue = clCreateCommandQueue(*context, *device, 0, &ciErrNum);

    /*step 1: Create Buffers
              allocate device memory 
              Input data is read-only
              Output data is write-only */
    cl_mem d_filter = clCreateBuffer(*context, CL_MEM_READ_ONLY, filterSize, NULL, &ciErrNum);
    cl_mem d_ip = clCreateBuffer(*context, CL_MEM_READ_ONLY, mem_size, NULL, &ciErrNum);
    cl_mem d_op = clCreateBuffer(*context, CL_MEM_WRITE_ONLY, mem_size, NULL, &ciErrNum);

    // Transfer input data to device
    ciErrNum = clEnqueueWriteBuffer(myqueue, d_filter, CL_TRUE, 0, filterSize, (void *)filter, 0, NULL, NULL);
    CHECK(ciErrNum, "clEnqueueWriteBuffer");
    ciErrNum = clEnqueueWriteBuffer(myqueue, d_ip, CL_TRUE, 0, mem_size, (void *)inputImage, 0, NULL, NULL);
    CHECK(ciErrNum, "clEnqueueWriteBuffer");

    /*step 2: Build Program, Select Kernel
              create kernel function */
    cl_kernel mykernel = clCreateKernel(*program, "convolution", ciErrNum);
    CHECK(ciErrNum, "clCreateKernel");

    /*step 3: Set Arguments, Enqueue Kernel
              set kernel function args */
    clSetKernelArg(mykernel, 0, sizeof(cl_int), (void *)&filterWidth);
    clSetKernelArg(mykernel, 1, sizeof(cl_mem), (void *)&d_filter);
    clSetKernelArg(mykernel, 2, sizeof(cl_int), (void *)&imageHeight);
    clSetKernelArg(mykernel, 3, sizeof(cl_int), (void *)&imageWidth);
    clSetKernelArg(mykernel, 4, sizeof(cl_mem), (void *)&d_ip);
    clSetKernelArg(mykernel, 5, sizeof(cl_mem), (void *)&d_op);

    // workgroups parameter
    size_t localws[2]  = {20, 20};
    size_t globalws[2] = {imageWidth, imageHeight};

    // execute kernel function
    ciErrNum = clEnqueueNDRangeKernel(myqueue, mykernel, 2, 0, globalws, localws, 0, NULL, NULL);
    CHECK(ciErrNum, "clEnqueueNDRangeKernel");

    /*step 4: Read Back Result
              copy data from device memory to host memory*/
    ciErrNum = clEnqueueReadBuffer(myqueue, d_op, CL_TRUE, 0, mem_size, (void *)outputImage, NULL, NULL, NULL);
    CHECK(ciErrNum, "clEnqueueReadBuffer");

    // release opencl object
    ciErrNum = clReleaseCommandQueue(myqueue);
    ciErrNum = clReleaseMemObject(d_filter);
    ciErrNum = clReleaseMemObject(d_ip);
    ciErrNum = clReleaseMemObject(d_op);
    ciErrNum = clReleaseKernel(mykernel);
}
