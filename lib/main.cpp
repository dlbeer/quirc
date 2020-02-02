#include <stdio.h>
#include "quirc.hpp"

int main(){
    Quirc q;
    printf("Hello %s\n", q.getVersion());
    printf("%s\n", q.strerror(Quirc::ERROR_DATA_ECC));
    
    return 0;
}
