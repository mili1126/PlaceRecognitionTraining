//
//  main.cpp
//  Localization
//
//  Created by Ming Li on 4/27/16.
//  Copyright © 2016 Ming Li. All rights reserved.
//

//opencv
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"
//C
#include <stdio.h>
//C++
#include <iostream>
#include <sstream>

using namespace cv;
using namespace std;
using namespace cv::xfeatures2d;

//constants
const int WIDTH = 800;
const int HEIGHT = 600;
const double SCALE = 0.5;

//global variables
Mat frame; //current frame
int keyboard; //input from keyboard

//function declaration
void help();
void processVideo(char* videoFilename);
void processImages(char* firstFrameFilename);

void help()
{
    cout
    << "--------------------------------------------------------------------------" << endl
    << "This program shows how to use the feature points detectors, descriptors"
    << " and matching framework found inside OpenCV."                               << endl
    << " You can process both videos (-vid) and images (-img)."                     << endl
    << endl
    << "Usage:"                                                                     << endl
    << "./main <video filename> --FEACHER"                       << endl
    << "for example: ./main -vid video.mov"                                         << endl
    << "or: ./main -img 1.png"                                                      << endl
    << "--------------------------------------------------------------------------" << endl
    << endl;
}

int main(int argc, char * argv[]) {
    //print help information
    help();
    
    //check for the input parameter correctness
    if(argc != 3) {
        cerr <<"Incorret input list" << endl;
        cerr <<"exiting..." << endl;
        return EXIT_FAILURE;
    }
    //create GUI windows
    namedWindow("Frame");
    if(strcmp(argv[1], "-vid") == 0) {
        //input data coming from a video
        processVideo(argv[2]);
    }
    else if(strcmp(argv[1], "-img") == 0) {
        //input data coming from a sequence of images
        processImages(argv[2]);
    }
    else {
        //error in reading input parameters
        cerr <<"Please, check the input parameters." << endl;
        cerr <<"Exiting..." << endl;
        return EXIT_FAILURE;
    }
    //destroy GUI windows
    destroyAllWindows();
    return EXIT_SUCCESS;
}

void processVideo(char* videoFilename) {
    //create the capture object
    VideoCapture capture(videoFilename);
    if(!capture.isOpened()){
        //error in opening the video input
        cerr << "Unable to open video file: " << videoFilename << endl;
        exit(EXIT_FAILURE);
    }
    //read input data. ESC or 'q' for quitting
    while( (char)keyboard != 'q' && (char)keyboard != 27 ){
        //read the current frame
        if(!capture.read(frame)) {
            cerr << "Unable to read next frame." << endl;
            cerr << "Exiting..." << endl;
            exit(EXIT_FAILURE);
        }
        
        //get the frame number and write it on the current frame
        stringstream ss;
        rectangle(frame, cv::Point(10, 2), cv::Point(100,20),
                  cv::Scalar(255,255,255), -1);
        ss << capture.get(CAP_PROP_POS_FRAMES);
        string frameNumberString = ss.str();
        putText(frame, frameNumberString.c_str(), cv::Point(15, 15),
                FONT_HERSHEY_SIMPLEX, 0.5 , cv::Scalar(0,0,0));
        //resize and show the current frame
        Size targetSize(frame.size().width*SCALE, frame.size().height*SCALE);
        resize(frame, frame, targetSize);
        
        //-- Step 1: Detect the keypoints using SURF Detector
        int minHessian = 400;
        Ptr<SURF> detector = SURF::create( minHessian );
        std::vector<KeyPoint> keypoints;
        detector->detect( frame, keypoints );
        //-- Draw keypoints
        Mat img_keypoints;
        drawKeypoints( frame, keypoints, img_keypoints, Scalar::all(-1), DrawMatchesFlags::DEFAULT );
        //-- Show detected (drawn) keypoints
        imshow("Keypoints ", img_keypoints);

        
        //get the input from the keyboard
        keyboard = waitKey( 30 );
    }
    //delete capture object
    capture.release();
}
void processImages(char* fistFrameFilename) {
    //read the first file of the sequence
    frame = imread(fistFrameFilename);
    if(frame.empty()){
        //error in opening the first image
        cerr << "Unable to open first image frame: " << fistFrameFilename << endl;
        exit(EXIT_FAILURE);
    }
    //current image filename
    string fn(fistFrameFilename);
    //read input data. ESC or 'q' for quitting
    while( (char)keyboard != 'q' && (char)keyboard != 27 ){
        //get the frame number and write it on the current frame
        size_t index = fn.find_last_of("/");
        if(index == string::npos) {
            index = fn.find_last_of("\\");
        }
        size_t index2 = fn.find_last_of(".");
        string prefix = fn.substr(0,index+1);
        string suffix = fn.substr(index2);
        string frameNumberString = fn.substr(index+1, index2-index-1);
        istringstream iss(frameNumberString);
        int frameNumber = 0;
        iss >> frameNumber;
        rectangle(frame, cv::Point(10, 2), cv::Point(100,20),
                  cv::Scalar(255,255,255), -1);
        putText(frame, frameNumberString.c_str(), cv::Point(15, 15),
                FONT_HERSHEY_SIMPLEX, 0.5 , cv::Scalar(0,0,0));
        //resize and show the current frame
        Size targetSize(frame.size().width*SCALE, frame.size().height*SCALE);
        resize(frame, frame, targetSize);
        
        //-- Step 1: Detect the keypoints using SURF Detector
        int minHessian = 400;
        Ptr<SURF> detector = SURF::create( minHessian );
        std::vector<KeyPoint> keypoints;
        detector->detect( frame, keypoints );
        //-- Draw keypoints
        Mat img_keypoints;
        Scalar keypointColor = Scalar(0, 255, 0);     // Scalar::all(-1).
        drawKeypoints( frame, keypoints, img_keypoints, keypointColor, DrawMatchesFlags::DEFAULT );
        //-- Show detected (drawn) keypoints
        imshow("Keypoints ", img_keypoints);
        
        
        drawKeypoints(frame, keypoints, img_keypoints, keypointColor, DrawMatchesFlags::DEFAULT);
        
        
        
        
        
        imwrite("surf.png", img_keypoints);
        
        //get the input from the keyboard
        keyboard = waitKey( 0 );
        //search for the next image in the sequence
        ostringstream oss;
        oss << (frameNumber + 1);
        string nextFrameNumberString = oss.str();
        string nextFrameFilename = prefix + nextFrameNumberString + suffix;
        //read the next frame
        frame = imread(nextFrameFilename);
        if(frame.empty()){
            //error in opening the next image in the sequence
            cerr << "Unable to open image frame: " << nextFrameFilename << endl;
            exit(EXIT_FAILURE);
        }
        //update the path of the current frame
        fn.assign(nextFrameFilename);
    }
}
