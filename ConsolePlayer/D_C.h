#pragma once

#include "Types.h"
#include "General.h"

namespace D_C
{
    enum ResultGetFrame
    {
        end_sequence,
        time_not_come,
        new_frame
    };

    cv::VideoCapture* Init(std::string path)
    {
        cv::VideoCapture* cap = new cv::VideoCapture(path);
        if (!cap->isOpened())
        {
            std::cout << "Can not open Video file" << std::endl;
            exit(1);
        }

        return cap;
    }

    ResultGetFrame GetFrameByTime(cv::VideoCapture& cap, long long time, cv::Mat& result)
    {
        static long long last_frame_time = -1;
        static int last_frame_index = 0;

        if (last_frame_time > time)
        {
            return time_not_come;
        }

        for (int frameNum = last_frame_index; frameNum < cap.get(cv::CAP_PROP_FRAME_COUNT); frameNum++)
        {
            long long time_msec = cap.get(cv::CAP_PROP_POS_MSEC);
            cap >> result;

            if (time_msec > time)
            {
                last_frame_index = frameNum;
                last_frame_time = time_msec;

                return new_frame;
            }
        }

        return end_sequence;
    }

    void Play(cv::VideoCapture& cap, cv::Size size)
    {
        auto time_start = std::chrono::system_clock::now();

        cv::Mat frame_src;
        cv::Mat frame_dst;
        std::string frame_string;
        ResultGetFrame status;

        while (true)
        {
            status = GetFrameByTime(cap, (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - time_start)).count(), frame_src);

            switch (status)
            {
            case end_sequence:
            {
                return;
            }
            case time_not_come:
            {
                continue;
            }
            case new_frame:
            {
                General::PixelizationWithResizing(frame_src, frame_dst, size);
                General::ToWhiteBlack(frame_dst, frame_src);
                General::CreateStringByFrame(frame_src, frame_string);
                General::GoToPosition();
                General::WriteToConsole(frame_string);
                frame_string.clear();
            }
            }
        }
    }
}