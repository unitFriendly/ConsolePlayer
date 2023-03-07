#pragma once

#include "opencv2/core.hpp"
#include "opencv2/video.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp" 
#include <opencv2/core/utils/logger.hpp>

#include <vector>
#include <list>

namespace Types
{
#define TYPE_D_C "d_c"
#define TYPE_S_C "s_c"
#define TYPE_C_C "c_c"

    enum TypePlay
    {
        d_c,
        s_c,
        c_c
    };

    enum ResultGetFrame
    {
        end_sequence,
        time_not_come,
        new_frame,
        frame_not_loaded
    };

    struct VideoFrame
    {
        cv::Mat frame;
        long long time_msec = 0;
    };

    struct VideoFrameString
    {
        std::string frame;
        long long time_msec = 0;
    };

    typedef std::vector<VideoFrame> VectorFrames;
    typedef std::vector<VideoFrameString> VectorFramesString;
    typedef std::list<VideoFrameString> ListFramesString;
}


