#include "Tools.hpp"
#include <Configs.h>

double curve_algorithm(double x){
    return 1.0 / (1.0 + pow(E, -x));
}