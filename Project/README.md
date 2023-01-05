# Parallel Programming Final Project
###### tags: `Parallel Programming`




## 1. Title: 
Parallel k-means clustering algorithm

## 2. The participants
310554031 葉詠富
310554037 黃乾哲
310553050 彭俊凱

## 3. Abstract

In this project, we aim to accelerate the computation of the k-means clustering algorithm through the use of parallelization. By utilizing OpenMP and Cuda, we significantly improve the computational efficiency of the k-means clustering algorithm compared to its execution in Sequence. In experiments, we compare the performance of the parallelized k-means to that of Sequence k-means on image grouping. The results show that the use of the parallelized k-means is many times faster than Sequence k-means in the case of classifying into more groups.

## 4. Introduction

When we perform image processing algorithms in machine learning, we often need to calculate each pixel of the image. When calculating large-scale images, there is often a problem that the calculation is too long. We hope to use the parallelization language to speed up the calculation by viewing the image as a matrix, reduce the calculation time, and select suitable hyperparameters during fine-tuning.

## 5. Problem Define

The clustering problem has always been a well-known problem in the field of machine learning, and it is necessary to calculate the relationship between each data point and all other data points. This part reminds us of the concept of SIMD. We decided to apply parallel processing to K-means Clustering, save time when you need to use K-means Clustering in the future.

Clustering is the task of assigning a set of objects into groups (called clusters) so that the objects in the same cluster are more similar (in some sense or another) to each other than to those in other clusters.


The process steps of k-means operation:

1. Select k initial points at Random
3. Calculate the distance from each data point to K cluster centroid.
4. Classify data points to the nearest cluster centroid.
5. Count the number of objects that changed their cluster 𝞭.
6. Average the centroids of new clusters. Using the objects inside the clusters.
7. Repeat Steps 2–5 until the 𝞭/N< threshold.





## 6. Proposed Solution

### OpenMP

The two parts of the main parallelization are

1. Find near cluster:
    Step2. Calculate the distance from each data point to K cluster centroid.
2. Compute delta:
    Step4. Count the number of objects that changed their cluster 𝞭.

The parallelized approach is that each thread computes (step 2.), (step 4.) and saves in its own local parameters, then uses thread 0 to aggregate the entire result and update the cluster.
Parallelization implementation with #pragma omp for, then set the random seed to fix initial center。
We fixed the number of executions to 500 times for the fairness of the comparison, 


### Cuda
The part of Cuda is to parallelize the processing of each pixel, assign the task of calculating each point to individual threads, and divide the task into two Kernels. Parallel kernel content two part.

1. Find near cluster:
    Step2. Calculate the distance from each data point to K cluster centroid.
    Step3. Classify data points to the nearest cluster centroid.
2. Compute delta:
    Step4. Count the number of objects that changed their cluster 𝞭.
    
#### Cuda setting:
* Number threads per block = 128. ( set to a multiple of 32).
* Use the `mollac()` and `cudaMemcpy()` to implement cuda method.




## 7. Experimental Methodology

The experimental method of this implementation is to find the time spent in Kmeans operation. A total of 5 times will be tested, and the median will be taken as the operation time of this method. The calculation time and speedup of each method will be compared.
In addition, we performed kmeans operations on small images (64, 64) and large images (256, 256) respectively, and compared the differences in the results of each implementation method.

## 8. Experimental Result

### Small image size

* Original image(64,64)

<img width="64" src="https://i.imgur.com/ja19kej.jpg">

* After kmeans image(64,64) on k=2

<img width="64" src="https://i.imgur.com/mnU1MIC.png">

* Execution time

<img width="480" src="https://i.imgur.com/ftMWhhg.png">

* Speed up

<img width="480" src="https://i.imgur.com/iovqCBl.png">

* [video](https://youtube.com/shorts/75vkMmTzOlQ?feature=share)



### Big image size

* Original image(256,256)

<img width="256" src="https://i.imgur.com/cpLW3mK.jpg">

* After kmeans image(256,256) on k=2

<img width="256" src="https://i.imgur.com/0gZn4Iz.png">

* Execution time

<img width="480" src="https://i.imgur.com/X8WOMtK.png">

* Speed up

<img width="480" src="https://i.imgur.com/1mdgg4B.png">


* [Video](https://youtube.com/shorts/8_6e-Ub8CAY?feature=share)



## 9. relative work

以下結果為論文[1]中(300,300)的執行時間比較。
![](https://i.imgur.com/MmQsj0X.png)

以下結果為論文[1]中(1164,1200)的執行時間比較。
![](https://i.imgur.com/XZxljET.png)


## 10. Conclusions

### Discuss on small image size
**ExeTime: cudaTime > ompTime > seqTime.**
1. Parallel calculate the distance from each data point to K cluster centroid, but the calculation with Sequential is fast.
2. Because the image is small and the number of pixels is small, the calculation with Sequential is fast.
3. OpenMP needs to allocate the calculations performed by the thread.
4. OpenMP thread has to wait for the thread to complete, but the time is relatively long.
5. CUDA needs to allocate memory at the beginning, and also needs to transfer the data to the memory of the device, so compared with Sequential, it takes a longer time.        
        
### Discuss on big image size
**ExeTime: ompTime > seqTime.**
1. parallel calculate the distance from each data point to K cluster centroid, but Small number of clusters;
2. And the calculation of image RGB only has three dimensions channel, so the calculation is less;
3. OpenMP spends a lot of time average the centroids of new clusters using the objects inside the clusters. 

### Discuss on cuda bottleneck
- Same image size : ( 256, 256)
- When K<32, the time usage of Find near cluster(step 2.、step 3. ) less than the memory carry time.
    
![](https://i.imgur.com/RcPPDJa.png)

### conclusion
    * small number of cluster、small image size => It is faster to use sequential directly.
    * big number of cluster、big image size => It is faster to use CUDA directly.
    * CUDA Bottleneck will appear in Comput nearest cluster(step 2.、step 3 ).
	
![](https://i.imgur.com/09iLlgn.png)


## 11. References
[1] Janki Bhimani 
Accelerating K-Means clustering with parallel implementations and GPU computing 
2015 IEEE
(https://ieeexplore.ieee.org/document/7322467/references#references)




