#include <stdio.h>
#include <stdlib.h>
#include "quirc.hpp"


using namespace std;

Quirc::Quirc():imgrgba(NULL), width(0), height(0){
    instance = quirc_new();
}

Quirc::~Quirc(){
    quirc_destroy(instance);
    free(imgrgba);
}

const char *Quirc::getVersion(){
    return quirc_version();
}

int Quirc::resize(int w, int h){
    if ((width == w) && (height == h)) {
        return 0;
    }
    
    width = w;
    height = h;
    
    int res = quirc_resize(instance, w, h);
    if (res < 0) {
        return res;
    }
    return res;
    uint8_t *img = (uint8_t *)calloc(w*4, h);
    if (img == NULL){
        return -1;
    }
    free(imgrgba);
    imgrgba = img;
    return 0;
}


uint8_t *Quirc::begin(){
    imggray = quirc_begin(instance, NULL, NULL);
    //return imgrgba;
    return imggray;
}
 
void Quirc::end(){
    // for (int i; i < width*height*4; i+=4) {
    //     imggray[i>>2] = (uint8_t) (( (imgrgba[i] * (uint16_t)66 + imgrgba[i + 1] * (uint16_t)129 + imgrgba[i + 2] * (uint16_t)25) + (uint16_t)4096) >> 8);
    // }
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
  return imggray[index];
   
}

int Quirc::getPixelRGBA(int index){
  return imgrgba[index];
   
}
