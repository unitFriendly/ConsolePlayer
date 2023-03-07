#pragma once

#include "Types.h"
#include "General.h"

namespace C_C
{
    Types::ListFramesString buffer_play;
    cv::VideoCapture video_capture;
    size_t size_buffer;
    cv::Size size_frame;
    bool show_size_buffer;

    void BufferAddFrame()
    {
        static Types::VideoFrameString video_frame_string;
        static cv::Mat frame_src;
        static cv::Mat frame_dst;

        video_frame_string.time_msec = video_capture.get(cv::CAP_PROP_POS_MSEC);
        video_capture >> frame_src;

        General::PixelizationWithResizing(frame_src, frame_dst, size_frame);
        General::ToWhiteBlack(frame_dst, frame_src);
        General::CreateStringByFrame(frame_src, video_frame_string.frame);

        buffer_play.push_back(video_frame_string);

        video_frame_string.frame.clear();
    }

    void FillingBuffer()
    {
        static int count_frame_read = size_buffer;

        while (true)
        {
            if (show_size_buffer)
            {
                SetWindowTextA(General::MAIN_HWND, std::to_string(buffer_play.size()).c_str());
            }

            if (count_frame_read >= video_capture.get(cv::CAP_PROP_FRAME_COUNT))
            {
                buffer_play.push_back({ "", LLONG_MAX });
                return;
            }
            else
            {
                if (buffer_play.size() < size_buffer)
                {
                    count_frame_read++;
                    BufferAddFrame();
                }
            }
        }
    }

    void Init(std::string path, size_t _size_buffer, cv::Size size, bool _show_size_buffer)
    {
        video_capture = cv::VideoCapture(path);
        if (!video_capture.isOpened())
        {
            std::cout << "Can not open Video file" << std::endl;
            exit(1);
        }

        size_buffer = _size_buffer;
        size_frame = size;
        show_size_buffer = _show_size_buffer;

        bool run_thread = true;
        std::thread loading_thread(General::ShowLoadingMessage, &run_thread, 0, 0);

        for (int i = 0; ((i < size_buffer) && (i < video_capture.get(cv::CAP_PROP_FRAME_COUNT))); i++)
        {
            BufferAddFrame();
        }

        if (size_buffer >= video_capture.get(cv::CAP_PROP_FRAME_COUNT))
        {
            buffer_play.push_back({ "", LLONG_MAX });
        }

        run_thread = false;
        loading_thread.join();
    }

    Types::ResultGetFrame GetFrameByTime(long long time, Types::VideoFrameString& result)
    {
        static long long last_frame_time = -1;

        if (result.time_msec == LLONG_MAX)
        {
            return Types::end_sequence;
        }

        if (last_frame_time > time)
        {
            return Types::time_not_come;
        }

        while (!buffer_play.empty())
        {
            result = buffer_play.front();
            buffer_play.pop_front();

            if (result.time_msec > time)
            {
                last_frame_time = result.time_msec;

                return Types::new_frame;
            }
        }

        return Types::frame_not_loaded;
    }

    void Play(cv::Size size)
    {
        std::thread create_frame_thread(FillingBuffer);

        Types::VideoFrameString current_frame;
        Types::ResultGetFrame status;

        auto time_start = std::chrono::system_clock::now();

        while (true)
        {
            status = GetFrameByTime((std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - time_start)).count(), current_frame);

            switch (status)
            {
            case Types::end_sequence:
            {
                create_frame_thread.join();
                return;
            }
            case Types::time_not_come:
            {
                continue;
            }
            case Types::frame_not_loaded:
            {
                continue;
            }
            case Types::new_frame:
            {
                General::GoToPosition();
                General::WriteToConsole(current_frame.frame);
            }
            }
        }
    }
}