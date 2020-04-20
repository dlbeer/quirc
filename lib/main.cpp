#include <iostream>
#include <fstream>
#include <sys/time.h> 
#include <unistd.h>
#include "quirc.hpp"

using namespace std;

double performanceNow(){
    struct timeval time;
    gettimeofday(&time, NULL);
    double now = 1000.0 * time.tv_sec + (time.tv_usec / 1000.0);
    return now; 
}

int main(){

    ifstream infile;
    uint8_t *image;
   
    Quirc q;
    cout << "Quirc Version: " << q.getVersion() << endl;

    q.resize(474, 632);
    image = q.begin();
  
    infile.open("../jpeg/qr-codes-xavier.474x632.raw", ios::binary | ios::in);
    if (!infile.is_open()) {
        cout << "Cannot open file!!" << endl;
        return -1;
    }
    infile.read((char*)image, 474*632);
    printf("pixels %d %d\n", image[0], image[1]);
    infile.close();
     printf ("getpixel: %d %d\n", q.getPixel(0), q.getPixel(1));
    double t0 = performanceNow();
for (int j=0; j<1; j++) {
    q.end();
     
    cout << "Codes found: " << q.count() << endl;

    for (int i=0; i<q.count(); i++){
        Quirc::Code code;
        code = q.extract(i);

        Quirc::Result result = q.decode(&code);
        cout << "Payload " << i << ":" << result.error << ":" << result.data.payload << endl;

    }
}
    double t1 = performanceNow();
    cout << (t1 - t0)/1000 << "ms" << endl;
    cout << "-----" << endl; 
    printf ("getpixel: %d %d\n", q.getPixel(0), q.getPixel(1));
    cout << "-----" << endl;
    

    return 0;
}
