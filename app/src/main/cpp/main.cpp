#include <iostream>
#include<vector>
#include"main.h"
#include"boundary.h"
#include"line_recognizing.h"
#include "maze_solving.h"
#include "maze.h"


cv::Mat solve(cv::Point start, cv::Point end, const cv::Mat& img) {

    // Черно-белое изображение
    cv::Mat gray_img;

    // Применяем размытие для устранения мелких артефактов
    medianBlur(img, img, 3);
    cvtColor(img, gray_img, cv::COLOR_RGB2GRAY);

    // Изображение в цветовой моделе - HSV
    cv::Mat img_hsv;
    cvtColor(img, img_hsv, cv::COLOR_BGR2HSV);

    Maze maze(img_hsv, gray_img, start, end);

    // Бинаризуем черно белое изображение
    binarize(maze);

    // Находим границу лабиринта
    get_boundary(maze);

    // Поиск цвета стен и пола лабиринта
    get_wall_color(maze);
    get_floor_color(maze);

    // Поиск начала и конца линии пользователя
    find_line(maze);
    if(maze.start_line != maze.start)
        get_end_of_line(maze);

    // Отрисовка границы на черно-белом изображении
    for (const auto& c : maze.boundary) {
        cv::line(maze.grim, c.source, c.destination, {0, 255, 0 }, 3);
    }


    // Поиск пути от конца линии до конца лабиринта
    std::vector<cv::Point> trace = get_trace(maze);

    std::reverse(trace.begin(), trace.end());

    // Отрисовка подсказки на цветном изображении
    for (int i = 0; i < std::min(500, (int)trace.size() - 1); i++)
        cv::line(img, trace[i], trace[i + 1], { 255,0,0 ,100 }, 2);

    return img;
}