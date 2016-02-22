#include <iostream>
#include <string>
#include <cmath>
#include <cstring>

#include "png++/png.hpp"

#include <boost/gil/image.hpp>
#include <boost/gil/typedefs.hpp>
#include <boost/gil/extension/io/png_io.hpp>
#include <boost/gil/extension/numeric/sampler.hpp>
#include <boost/gil/extension/numeric/resample.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/gil/extension/dynamic_image/any_image.hpp>

using namespace std;

void Resizing (std::string &name1, std::string &name2, int width, int height)
{
    using namespace boost::gil;
    rgb8_image_t img1;
    rgb8_image_t img2;
    rgb8_image_t square(width,height);
    png_read_image(name1,img1);
    png_read_image(name2,img2);
    resize_view(const_view(img1), view(square), bilinear_sampler());
    png_write_view("template.png",const_view(square));
    resize_view(const_view(img2), view(square), bilinear_sampler());
    png_write_view("experiment.png",const_view(square));
}

void FillIntensive (std::vector<std::vector<float> > &A, int &intensive, std::string name)
{
    png::image< png::rgb_pixel > image(name);
    A.resize(image.get_height());
    long long sum=0;
    for (int y = 0; y < image.get_height(); ++y)
    {
        A[y].resize(image.get_width());
        for (int x = 0; x < image.get_width(); ++x)
        {
            A[y][x] = (image[y][x].red + image[y][x].green + image[y][x].blue)/3;
            sum+=A[y][x];
        }
    }
    intensive=sum/(image.get_height()*image.get_width());
}

int Corr(std::vector<std::vector<float> > temp, std::vector<std::vector<float> > expr, int ft0, int fe0, int width, int height)
{
    int Rtx = 0;
    for (int i=0; i<height; ++i)
    {
        for (int j=0; j<width; ++j)
        {
           Rtx+=(temp[i][j]-ft0)*(expr[i][j]-fe0);
        }
    }
    //std::cout<<Rtx<<" ";
    return Rtx;
}

int AutoCorrExpr(std::vector<std::vector<float> > expr, int fe0, int width, int height)
{
    int Rxx = 0;
    for (int i=0; i<height; ++i)
    {
        for (int j=0; j<width; ++j)
        {
            Rxx+=(expr[i][j]-fe0)*(expr[i][j]-fe0);
        }
    }
    //std::cout<<Rxx<<" ";
    return Rxx;
}

int AutoCorrTemp(std::vector<std::vector<float> > temp, int ft0, int width, int height)
{
    int Rtt = 0;
    for (int i=0; i<height; ++i)
    {
        for (int j=0; j<width; ++j)
        {
            Rtt+=(temp[i][j]-ft0)*(temp[i][j]-ft0);
        }
    }
    //std::cout<<Rtt<<" ";
    return Rtt;
}

double CorrMatrix(std::vector<std::vector<float> > temp, std::vector<std::vector<float> > expr,int ft0, int fe0, int width, int height)
{
    return Corr(temp, expr, ft0, fe0, width, height)/(sqrt(AutoCorrExpr(expr, fe0 , width, height))*sqrt(AutoCorrTemp(temp, ft0, width, height)));
}

void ComparePicture(double normcorr, int width, int height)
{
     cout<<abs(normcorr)<<endl;
    if (abs(normcorr) >= 0.7)
        std::cout<<"The similar picture"<<endl;
    else
        std::cout<<"The different picture"<<endl;
}


int main()
{
    std::string name1;
    name1 = "file1.png";
    std::string name2;
    name2 = "file2.png";
    int width=100;
    int height=100;
    Resizing(name1, name2, width, height);
    std::vector<std::vector<float> > intens_t;
    std::vector<std::vector<float> > intens_e;
    double normcorr;
    int middle_intens_t, middle_intens_e;
    FillIntensive(intens_t, middle_intens_t, "template.png");
    FillIntensive(intens_e, middle_intens_e, "experiment.png");
    //cout<<middle_intens_e<<" "<<middle_intens_t<<endl;
    normcorr=CorrMatrix(intens_t,intens_e,middle_intens_t, middle_intens_e,width,height);
    ComparePicture(normcorr, width, height);
    return 0;
}
