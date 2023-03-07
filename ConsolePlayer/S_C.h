#pragma once

#include "Types.h"
#include "General.h"

namespace S_C
{
    void ExtractFrames(std::string& videoFilePath, Types::VectorFrames& frames)
    {
        try
        {
            cv::VideoCapture cap(videoFilePath);
            if (!cap.isOpened())
                std::cout << "Can not open video file" << std::endl;

            for (int frameNum = 0; frameNum < cap.get(cv::CAP_PROP_FRAME_COUNT); frameNum++)
            {
                Types::VideoFrame vf;
                vf.time_msec = cap.get(cv::CAP_PROP_POS_MSEC);
                cap >> vf.frame;
                frames.push_back(vf);
            }
        }
        catch (cv::Exception& e)
        {
            std::cout << e.msg << std::endl;
        }
    }

    void ExtractLowPixelFramesWhiteBlack(cv::Size size_pixel, std::string& videoFilePath, Types::VectorFrames& frames)
    {
        Types::VectorFrames _frames;
        ExtractFrames(videoFilePath, _frames);

        for (auto it : _frames)
        {
            Types::VideoFrame videoFrame;
            cv::Mat mat;
            videoFrame.time_msec = it.time_msec;
            General::ToWhiteBlack(it.frame, mat);
            General::PixelizationWithResizing(mat, videoFrame.frame, size_pixel);
            frames.push_back(videoFrame);
        }
    }

    void CreateVectorFrameString(Types::VectorFrames& src, Types::VectorFramesString& dst)
    {
        for (auto it : src)
        {
            Types::VideoFrameString frame_string;

            frame_string.time_msec = it.time_msec;
            General::CreateStringByFrame(it.frame, frame_string.frame);

            dst.push_back(frame_string);
        }
    }

    Types::VectorFramesString* Init(std::string path, cv::Size size)
    {
        bool run_thread = true;
        std::thread loading_thread(General::ShowLoadingMessage, &run_thread, 0, 0);

        Types::VectorFrames* vector_frames = new Types::VectorFrames();

        ExtractLowPixelFramesWhiteBlack(size, path, *vector_frames);

        Types::VectorFramesString* vector_frames_string = new Types::VectorFramesString();

        CreateVectorFrameString(*vector_frames, *vector_frames_string);

        vector_frames->clear();
        delete vector_frames;

        run_thread = false;
        loading_thread.join();

        return vector_frames_string;
    }

    int GetFrameIndexByTime(long long time, Types::VectorFramesString& frames)
    {
        static int index_last_frame = 0;

        for (int i = index_last_frame; i < frames.size(); i++)
        {
            if (frames[i].time_msec >= time)
            {
                index_last_frame = i;
                return i;
            }
        }

        return -1;
    }

    void Play(Types::VectorFramesString& frames)
    {
        int last_frame = INT_MAX;

        auto time_start = std::chrono::system_clock::now();

        while (true)
        {
            int current_frame = GetFrameIndexByTime((std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - time_start)).count(), frames);

            if (last_frame == current_frame)
            {
                continue;
            }

            last_frame = current_frame;

            if (current_frame == -1)
                break;

            General::GoToPosition();
            General::WriteToConsole(frames[current_frame].frame);
        }
    }
}
