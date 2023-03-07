#pragma once

#include <Windows.h>
#include <string>
#include <vector>
#include <iostream>
#include <chrono>
#include <thread>
#include <fstream>

#include "Types.h"
#include "Shades.h"

#define NAME_PROG "ConsolePlayer"

namespace General
{
    HWND MAIN_HWND = 0;
    HANDLE MAIN_CONSOLE = 0;

    HWND GetMainHwnd()
    {
        const int BUFSIZE = 1024;

        WCHAR windowTitle[BUFSIZE];
        GetConsoleTitle(windowTitle, BUFSIZE);

        return FindWindow(NULL, windowTitle);
    }

    inline void GoToPosition(int x = 0, int y = 0)
    {
        COORD coord{ x, y };

        SetConsoleCursorPosition(MAIN_CONSOLE, coord);
    }

    inline void ClearConsoleCls()
    {
        system("cls");
    }

    inline void WriteToConsole(std::string& strToWrite)
    {
        std::cout << strToWrite << std::flush;
    }

    inline void Sleep(long long time)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(time));
    }

    inline bool DirectoryExist(const std::string& dirPath)
    {
        DWORD fileAttr = GetFileAttributesA(dirPath.c_str());
        return (fileAttr != INVALID_FILE_ATTRIBUTES && (fileAttr & FILE_ATTRIBUTE_DIRECTORY));
    }

    inline bool FileExist(const std::string& path)
    {
        std::ifstream file(path.c_str());
        return file.good();
    }

    inline int GetRandomInt(int min, int max)
    {
        return min + (rand() % static_cast<int>(max - min + 1));
    }

    inline void SaveMatJpg(std::string outputFile, cv::Mat& mat)
    {
        std::vector<int> compression_params;
        compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
        compression_params.push_back(100);

        cv::imwrite(outputFile, mat, compression_params);
    }

    void ShowConsoleCursor(bool showFlag)
    {
        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(MAIN_CONSOLE, &cursorInfo);
        cursorInfo.bVisible = showFlag;
        SetConsoleCursorInfo(MAIN_CONSOLE, &cursorInfo);
    }

    inline void SetFontSize(SHORT size)
    {
        CONSOLE_FONT_INFOEX cfi = { sizeof(cfi) };
        GetCurrentConsoleFontEx(MAIN_CONSOLE, FALSE, &cfi);
        cfi.dwFontSize.Y = size;
        SetCurrentConsoleFontEx(MAIN_CONSOLE, FALSE, &cfi);

        ShowConsoleCursor(false);
        return;
    }

    inline void SetFontForPlay(cv::Size size)
    {
        RECT rect;
        GetWindowRect(MAIN_HWND, &rect);

        int width = rect.right - rect.left;
        int height = rect.bottom - rect.top;

        SetFontSize(height / size.height);

        return;
    }

    inline void SetFontForMessage()
    {
        SetFontSize(16);
    }

    void ShowLoadingMessage(bool* run, int pos_x = 0, int pos_y = 0)
    {
        General::SetFontForMessage();
        GoToPosition(pos_x, pos_y);
        std::cout << "Loading video [          ]";
        bool direction = true;
        int leftBorder = pos_x + 15;
        int rightBorder = pos_x + 24;
        int currentPosition = leftBorder;

        while (*run)
        {
            if (direction)
            {
                GoToPosition(currentPosition, pos_y);
                std::cout << ' ';

                if (currentPosition >= rightBorder)
                {
                    direction = false;
                    currentPosition -= 1;
                    GoToPosition(currentPosition, pos_y);
                    continue;
                }

                currentPosition++;

                std::cout << '*';
            }
            else
            {

                std::cout << ' ';
                GoToPosition(currentPosition, pos_y);

                if (currentPosition < leftBorder)
                {
                    direction = true;
                    currentPosition += 1;
                    continue;
                }


                std::cout << '*';
                GoToPosition(currentPosition, pos_y);
                currentPosition--;
            }

            Sleep((long long)50);
        }

        ClearConsoleCls();
    }

    inline void BaseInit()
    {
        MAIN_HWND = GetMainHwnd();
        SetWindowTextA(General::MAIN_HWND, NAME_PROG);
        MAIN_CONSOLE = GetStdHandle(STD_OUTPUT_HANDLE);

        cv::utils::logging::setLogLevel(cv::utils::logging::LogLevel::LOG_LEVEL_SILENT);

        ShowConsoleCursor(false);
    }

    inline void SetSizePositionCmd(cv::Size size)
    {
        ShowWindow(MAIN_HWND, SW_MAXIMIZE);
        ShowScrollBar(MAIN_HWND, SB_BOTH, FALSE);
        
        int width = GetSystemMetrics(SM_CXSCREEN);
        int height = GetSystemMetrics(SM_CYSCREEN);

        bool res = SetWindowPos(
            MAIN_HWND,
            HWND_TOP,
            (width - static_cast<int>(roundf(((float)height / 2.0 * 2.0) * ((float)size.width / (float)size.height))) - GetSystemMetrics(SM_CXVSCROLL)) / 2,
            0,
            static_cast<int>(roundf(((float)height / 2.0 * 2.0) * ((float)size.width / (float)size.height))) - GetSystemMetrics(SM_CXVSCROLL),
            height,
            SWP_SHOWWINDOW
        );

        return;
    }

    inline void PixelizationWithPreservationSize(cv::Mat& src, cv::Mat& dst, cv::Size& size)
    {
        cv::Mat tmp_mat;
        cv::resize(src, tmp_mat, size, 0, 0, cv::INTER_LINEAR);
        cv::resize(tmp_mat, dst, src.size(), 0, 0, cv::INTER_NEAREST);
    }

    inline void PixelizationWithResizing(cv::Mat& src, cv::Mat& dst, cv::Size& size)
    {
        cv::resize(src, dst, size, 0, 0, cv::INTER_LINEAR);
    }

    inline void CreateStringByFrame(cv::Mat& frame, std::string& result)
    {
        for (int i = 0; i < frame.rows; i++)
        {
            for (int j = 0; j < frame.cols; j++)
            {
                result.push_back(Shades::SHADES[static_cast<int>(frame.at<uchar>(i, j))]);
                result.append(" ");
            }
            result.append("\n");
        }
    }

    inline void ToWhiteBlack(cv::Mat& src, cv::Mat& dst)
    {
        cv::cvtColor(src, dst, cv::COLOR_BGR2GRAY);
    }

    inline void SaveFrames(Types::VectorFrames& frames, std::string& outputDir)
    {
        if (!DirectoryExist(outputDir))
        {
            std::cout << outputDir << "\t--\tdirectory not exist\n";
            return;
        }

        std::vector<int> compression_params;
        compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
        compression_params.push_back(100);

        try
        {
            for (Types::VectorFrames::iterator frame = frames.begin(); frame != frames.end(); ++frame) {
                std::string filePath = outputDir + ("\\") + std::to_string(static_cast<long long>((*frame).time_msec)) + ".jpg";
                cv::imwrite(filePath, (*frame).frame, compression_params);
            }
        }
        catch (cv::Exception& e)
        {
            std::cout << e.msg << std::endl;
        }
    }
}

