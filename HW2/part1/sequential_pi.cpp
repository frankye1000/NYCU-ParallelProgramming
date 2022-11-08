/* Monte Carlo method to estimate PI */
#include<iostream>
#include<cstdlib>
using namespace std;

double random_num(){
    double LO=-1.0, HI=1.0;
    return LO + static_cast <double> (rand()) /( static_cast <double> (RAND_MAX/(HI-LO)));
}

int main(int agrc, char* argv[]){
    long long number_of_tosses;
    long long number_in_circle = 0;
    double x,y,distance_squared,pi_estimate;

    number_of_tosses = strtoll(argv[1],NULL,10);


    for ( int toss = 0; toss < number_of_tosses; toss ++) {
        x = random_num();
        y = random_num();
        distance_squared = x * x + y * y;
        if ( distance_squared <= 1)
            number_in_circle++;
    }
    pi_estimate = 4 * number_in_circle /(( double ) number_of_tosses);

    cout<<"pi = "<<pi_estimate<<endl;
    return 0;
}