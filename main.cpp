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


using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;

//constants
const char * VIDEO_FOLDER = "videos/";
const char * OUTPUT_FOLDER = "outputs/";
const int WIDTH = 800;
const int HEIGHT = 600;
const double SCALE = 0.5;

//global variables
Mat frame; //current frame
Mat previousFrame;
int keyboard; //input from keyboard
char * videoId;
vector<KeyPoint> keypoints;
vector<KeyPoint> previousKeypoints;
Mat descriptor;
Mat previousDescriptor;
int keypointsNum;


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
  << endl
  << "Usage:"                                                                     << endl
  << "./main <video filename>"                                                    << endl
  << "for example: ./main 1.MOV"                                                  << endl
  << "--------------------------------------------------------------------------" << endl
  << endl;
}

int main(int argc, char * argv[]) {
  //print help information
  help();

  //check for the input parameter correctness
  if(argc != 2) {
    cerr <<"Incorret input list" << endl;
    cerr <<"exiting..." << endl;
    return EXIT_FAILURE;
  }
  //create GUI windows
  namedWindow("Frame");

  //input data coming from a video
  char * videoName = (char *) malloc(1 + strlen(VIDEO_FOLDER) + strlen(argv[1]));
  strcpy(videoName, VIDEO_FOLDER);
  strcat(videoName, argv[1]);
  videoId = strtok(argv[1],".");
  cout << "Processing Video " << videoId << endl;
  processVideo(videoName);

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

  int frameNum = 0;
  keypointsNum = 0;
  //read input data. ESC or 'q' for quitting
  while( (char)keyboard != 'q' && (char)keyboard != 27 ){
    //read the current frame
    if(!capture.read(frame)) {
      if (frameNum != 0) keypointsNum /= frameNum;
      cout << "Avg KeypointsNum = " << keypointsNum << endl;
      cerr << "Unable to read next frame." << endl;
      cerr << "Exiting..." << endl;
      exit(EXIT_FAILURE);
    }
    frameNum++;

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
    detector->detectAndCompute( frame, Mat(), keypoints, descriptor);

    keypointsNum+=keypoints.size();
    cout << frameNumberString << ": KeypointsNum = " << keypoints.size() << endl;
    // //convert keypoints to mat or save them to text file
    // std::vector<cv::Point2f> points;
    // std::vector<cv::KeyPoint>::iterator it;
    // for( it= keypoints.begin(); it!= keypoints.end();it++)
    // {
    //   points.push_back(it->pt);
    // }
    // cv::Mat pointmatrix(points);
    // string outputFile = string(OUTPUT_FOLDER) + string(videoId) + "/" + frameNumberString + ".yml";
    // cout << "outputFileName = " << outputFile << endl;
    // cv::FileStorage fs(outputFile.c_str(), cv::FileStorage::WRITE);
    // fs << "pointmatrix" << pointmatrix;
    // fs.release();

    // cout << descriptor << endl;

    // //-- Draw keypoints
    // Mat img_keypoints;
    // drawKeypoints( frame, keypoints, img_keypoints, Scalar::all(-1), DrawMatchesFlags::DEFAULT );
    // //-- Show detected (drawn) keypoints
    // imshow("Keypoints ", img_keypoints);


    //get the input from the keyboard
    keyboard = waitKey( 30 );
  }
  //delete capture object
  capture.release();
}
