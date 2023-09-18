#include <Configs.h>
#include "Tools.hpp"

double curve_algorithm(double x){
    return 1.0 / (1.0 + pow(E, -x));
}
