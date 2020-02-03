#include <iostream>
#include <fstream>
#include "quirc.hpp"

using namespace std;

int main(){

    ifstream infile;
    uint8_t *image;
   
    Quirc q;
    cout << "Quirc Version: " << q.getVersion() << endl;

    q.resize(474, 632);
    image = q.begin(NULL, NULL);

    infile.open("jpeg/qr-codes-xavier.474x632.raw", ios::binary | ios::in);
    infile.read((char*)image, 474*632);
    infile.close();
    q.end();
    cout << "Codes found: " << q.count() << endl;

    return 0;
}
