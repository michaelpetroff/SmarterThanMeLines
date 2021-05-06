#pragma once

#include<opencv2/opencv.hpp>
#include<queue>
#include<vector>
#include"segment.h"
#include "maze.h"

/**
 * Сравнивает два цвета в HSV
 * @param a первый цвет
 * @param b второй
 * @return true  - если цвета похожи
 *         false - если цвета не похожи
 */
bool comp(cv::Vec3b a, cv::Vec3b b);

/**
 * Находит цвет пола и устанавливает цвет пола полю
 * maze.wall_color
 * @param maze - лабиринт
 */
void get_wall_color(Maze& maze);

/**
 * Находит точку начала линии и
 * устанавливает значение полю
 * maze.start_line
 * @param maze - лабиринт
 */
void find_line(Maze& maze);

/**
 * Находит цвет пола и устанавливает
 * значение полю maze.floor_color
 * @param maze - лабиринт
 */
void get_floor_color(Maze& maze);

/**
 * Находит точку конца линии и
 * устанавливает значение полю
 * maze.end_line
 * @param maze - лабиринт
 */
void get_end_of_line(Maze& maze);
