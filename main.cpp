#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/photo/photo.hpp>
#include <iostream>
#include <sstream>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

using namespace boost::program_options;
using namespace std;
using namespace cv;

void CheckImageIsOK(cv::Mat& _img)
{
    if(_img.rows<=100 || _img.cols<=100)
    {
        throw std::runtime_error("Image must be at least 100x100 pixels. (verify your file name)");
    }
    if(_img.channels() != 3)
    {
        throw std::runtime_error("Image must countain 3 channels (R/G/B)");
    }
}

int main(int argc, const char **argv)
{
    options_description desc;
    desc.add_options()
            ("help", "Produit ce message d'aide")
            ("file",                value<string>(), "Fichier à traiter")
            ("seuilRouge",   value<int>(),        "Seuil rouge pour considérer comme une tache")
            ("plusGrosseTache",    value<float>(),  "taille de la plus grosse tache détectable en % de l'image (0.02->0.07)");
    variables_map vmap;
    store(parse_command_line(argc, argv, desc), vmap);
    boost::program_options::notify(vmap);
    if(vmap.count("help"))
    {
        cout << desc << endl;
        return false;
    }
    std::string file;
    int epsRouge;
    float epsTaille;
    try
    {
        file = vmap["file"].as<std::string>();
        epsRouge = vmap["seuilRouge"].as<int>();
        epsTaille = vmap["plusGrosseTache"].as<float>();
    }catch(std::exception e)
    {
        cout << "Ne peut lire un des argument : " << e.what() << endl;
        return 0;
    }

    Mat img = imread(file);
    CheckImageIsOK(img);

    // extraction de la composante rouge :
    vector<Mat> channels;
    split(img, channels);
    Mat red = channels[2];
    Mat binary;
    threshold(red, binary, static_cast<double>(epsRouge), 255., THRESH_BINARY);
    Mat dilated;
    dilate(binary, dilated, getStructuringElement(MORPH_ELLIPSE, Size(10,10)));
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    Mat dilatedCpy(dilated.clone());
    findContours(dilatedCpy, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    int idx = 0;
    Scalar color( 255, 255, 255);
    Mat mask(binary.size(), CV_8UC1);
    int tailleMax = binary.size().width*binary.size().height*epsTaille/100.;
    cout << "Taille max : " << tailleMax << " pixels" << endl;
    for( ; idx < contours.size(); idx++  )
    {
        double area = contourArea(contours[idx]);
        if(area < tailleMax)
        {
            drawContours( mask, contours, idx, color, -1, 8 );
        }
    }
    Mat res;
    inpaint(img, mask, res, 3, INPAINT_TELEA);
    std::stringstream outFile;
    
    boost::filesystem::path p(file);
    outFile << p.parent_path().string() << "/UNDESTED_" << p.filename().string(); 
    imwrite(outFile.str(), res);
    std::cout << "result written in : " << outFile.str() << std::endl;
    return 0;
}
