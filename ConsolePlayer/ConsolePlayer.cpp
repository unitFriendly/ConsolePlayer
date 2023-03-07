#include <iostream>

#include "Types.h"
#include "General.h"
#include "D_C.h"
#include "S_C.h"
#include "C_C.h"

int main(int argc, char* argv[])
{
    General::BaseInit();

    if (argc == 1)
    {
        std::cout << "Error: Specify the path to the file!" << std::endl;
        return 1;
    }

    int current_arg = 1;

    std::string path_to_video = std::string(argv[current_arg]);

    if (!General::FileExist(path_to_video))
    {
        std::cout << "Error: File [" << path_to_video << "] not found!" << std::endl;
        return 1;
    }

    current_arg++;

    Types::TypePlay type_play = Types::s_c;
    cv::Size size_frame{ 128,128 };
    size_t size_buffer = 100;
    bool show_size_buffer = false;

    if (argc > 2)
    {
        if (strcmp(argv[current_arg], TYPE_D_C) == 0)
        {
            type_play = Types::d_c;
            current_arg++;
        }
        else if (strcmp(argv[current_arg], TYPE_S_C) == 0)
        {
            type_play = Types::s_c;
            current_arg++;
        }
        else if (strcmp(argv[current_arg], TYPE_C_C) == 0)
        {
            type_play = Types::c_c;
            current_arg++;

            if (current_arg < argc)
            {
                if (strcmp(argv[current_arg], "bs") == 0)
                {
                    show_size_buffer = true;
                    current_arg++;
                }
            }
        }

        if (current_arg < argc)
        {
            if (argv[current_arg][0] == 'b')
            {
                try
                {
                    std::string size_string(argv[current_arg]);
                    size_buffer = static_cast<size_t>(std::stoi(size_string.substr(0, size_string.size() - 1)));
                    current_arg++;
                }
                catch (const std::exception&)
                {
                    std::cout << "Error: Size buffer [" << argv[current_arg] << "] is incorrect!" << std::endl;
                    return 1;
                }
            }
        }

        if (current_arg < argc)
        {
            try
            {
                size_frame.width = static_cast<size_t>(std::stoi(std::string(argv[current_arg])));
                current_arg++;
            }
            catch (const std::exception&)
            {
                std::cout << "Error: Size [" << argv[current_arg] << "] is incorrect!" << std::endl;
                return 1;
            }

            if (current_arg < argc)
            {
                try
                {
                    size_frame.height = static_cast<size_t>(std::stoi(std::string(argv[current_arg])));
                    current_arg++;
                }
                catch (const std::exception&)
                {
                    std::cout << "Error: Size [" << argv[current_arg] << "] is incorrect!" << std::endl;
                    return 1;
                }
            }
            else
            {
                std::cout << "Error: Specify the full size!" << std::endl;
                return 1;
            }
        }   

        if (current_arg != argc)
        {
            std::cout << "Error: One or more arguments are incorrect!" << std::endl;
            return 1;
        }
    }

    General::SetSizePositionCmd(size_frame);

    switch (type_play)
    {
    case Types::d_c:
    {
        cv::VideoCapture* cap = D_C::Init(path_to_video);
        
        std::cout << "Press enter to start";

        std::getchar();
        General::ClearConsoleCls();

        General::SetFontForPlay(size_frame);
        D_C::Play(*cap, size_frame);

        delete cap;

        break;
    }
    case Types::s_c:
    {
        Types::VectorFramesString* frames = S_C::Init(path_to_video, size_frame);

        std::cout << "Press enter to start";

        std::getchar();
        General::ClearConsoleCls();

        General::SetFontForPlay(size_frame);
        S_C::Play(*frames);

        frames->clear();
        delete frames;

        break;
    }
    case Types::c_c:
    {
        C_C::Init(path_to_video, size_buffer, size_frame, show_size_buffer);

        std::cout << "Press enter to start";

        std::getchar();
        General::ClearConsoleCls();

        General::SetFontForPlay(size_frame);
        C_C::Play(size_frame);

        break;
    }
    }



    return 0;
}




