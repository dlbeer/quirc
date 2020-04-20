#include <stdio.h>
#include <stdlib.h>
#include "quirc.hpp"


using namespace std;

Quirc::Quirc(){
    instance = quirc_new();
}

Quirc::~Quirc(){
    quirc_destroy(instance);
}

const char *Quirc::getVersion(){
    return quirc_version();
}

int Quirc::resize(int w, int h){
    return quirc_resize(instance, w, h);
}


uint8_t *Quirc::begin(){
    uint8_t *img = quirc_begin(instance, NULL, NULL);
    return img;
}
 
void Quirc::end(){
    quirc_end(instance);
}
 
const char *Quirc::strError(Quirc::DecodeError err){
    return quirc_strerror((quirc_decode_error_t)err);
 }

int Quirc::count(){
    return quirc_count(instance);
}

Quirc::Code Quirc::extract(int index){
    struct Quirc::Code x;
    quirc_extract(instance, index, (quirc_code *)&x);
    return x;
}

Quirc::Result Quirc::decode(const Quirc::Code *code){
    struct Quirc::Result result;
    result.error = (Quirc::DecodeError)quirc_decode((const quirc_code *)code, (quirc_data *)&result.data);
    return result;
}

int Quirc::getPixel(int index){
  uint8_t *img;
  img = begin();
  return img[index];
   
}
