#pragma once

#include<vector>
#include<opencv2/opencv.hpp>
#include "maze.h"

/**
 * Находит путь от конца линии пользователя до
 * конца лабиринта
 * @param maze - лабиринт
 * @return массив точек пути
 */
std::vector<cv::Point> get_trace(Maze& maze);