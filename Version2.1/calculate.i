%module calculate

%{
#include "calculate.cpp"
%}

%include "carrays.i"
%array_class(int, intArray);

int find_the_best(int *arr, int currentBotColor);
