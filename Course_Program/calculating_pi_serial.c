#include<stdio.h>
#include<stdlib.h>

int main(){
    int n;
    printf("please input n = ");
    scanf("%d",&n);
    double factor = 1.0;
    double sum = 0.0;
    for(int i=0;i<n;i++,factor=-factor){
        sum += factor/(2*i+1);
    }
    double pi = 4*sum;
    printf("Pi = %lf", pi);
    return 0;
}