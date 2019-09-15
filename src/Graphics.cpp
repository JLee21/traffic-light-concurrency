#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "Graphics.h"
#include "Intersection.h"

void Graphics::simulate()
{
    this->loadBackgroundImg();
    while (true)
    {
        // sleep at every iteration to reduce CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        // update graphics
        this->drawTrafficObjects();
    }
}

void Graphics::loadBackgroundImg()
{
    // create window
    _windowName = "Concurrency Traffic Simulation";
    cv::namedWindow(_windowName, cv::WINDOW_NORMAL);

    // load image and create copy to be used for semi-transparent overlay
    cv::Mat background = cv::imread(_bgFilename);
    _images.push_back(background);         // first element is the original background
    _images.push_back(background.clone()); // second element will be the transparent overlay
    _images.push_back(background.clone()); // third element will be the result image for display
}

void Graphics::drawTrafficObjects()
{
    // reset images
    _images.at(1) = _images.at(0).clone();
    _images.at(2) = _images.at(0).clone();

    // create overlay from all traffic objects
    for (auto it : _trafficObjects)
    {
        double posx, posy;
        it->getPosition(posx, posy);

        if (it->getType() == ObjectType::objectIntersection)
        {
            // cast object type from TrafficObject to Intersection
            std::shared_ptr<Intersection> intersection = std::dynamic_pointer_cast<Intersection>(it);

            // set color according to traffic light and draw the intersection as a circle
            cv::Scalar trafficLightColor = intersection->trafficLightIsGreen() == true ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255);
            cv::circle(_images.at(1), cv::Point2d(posx, posy), 50, trafficLightColor, -1);
            cv::putText(_images.at(1), "i."+std::to_string(it->getID()), cv::Point2d(posx - 35, posy + 15), 0, 1, cv::Scalar(0, 0, 0), 3);            
        }
        // else if (it->getType() == ObjectType::objectTrafficLight)
        // {
        //     std::cout << "asdf\n";
        //     // cast object type from TrafficObject to Intersection
        //     std::shared_ptr<Intersection> intersection = std::dynamic_pointer_cast<Intersection>(it);

        //     cv::circle(_images.at(1), cv::Point2d(posx, posy+50), 50, cv::Scalar(100, 155, 155), -1);
        //     cv::putText(_images.at(1), "tl."+std::to_string(it->getID()), cv::Point2d(posx - 35, posy + 50), 0, 1, cv::Scalar(0, 0, 0), 3);
        // }
        else if (it->getType() == ObjectType::objectVehicle)
        {
            cv::RNG rng(it->getID());
            int b = rng.uniform(0, 255);
            int g = rng.uniform(0, 255);
            int r = sqrt(255 * 255 - g * g - r * r); // ensure that length of color vector is always 255
            cv::Scalar vehicleColor = cv::Scalar(b, g, r);
            cv::circle(_images.at(1), cv::Point2d(posx, posy), 35, vehicleColor, -1);
            cv::putText(_images.at(1), "v."+std::to_string(it->getID()), cv::Point2d(posx - 35, posy + 15), 2, 1, cv::Scalar(0, 0, 0), 2);
        }
    }

    float opacity = 0.95;
    cv::addWeighted(_images.at(1), opacity, _images.at(0), 1.0 - opacity, 0, _images.at(2));

    // display background and overlay image
    cv::imshow(_windowName, _images.at(2));
    cv::waitKey(33);
}
