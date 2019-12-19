//
// Created by admin on 2019/12/19.
//

#ifndef REPHOTO_DESCRIPTOR_H
#define REPHOTO_DESCRIPTOR_H


using namespace std;

class Descriptor
{
public:
    float			xi, yi;		// The location
    vector<double>	fv;			// The feature vector

    Descriptor()
    {
    }

    Descriptor(float x, float y, vector<double> const& f)
    {
        xi = x;
        yi = y;
        fv = f;
    }
};
#endif //REPHOTO_DESCRIPTOR_H
