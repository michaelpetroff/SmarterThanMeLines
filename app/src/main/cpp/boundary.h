#pragma once

#include<opencv2/opencv.hpp>
#include<unordered_set>
#include "segment.h"
#include "maze.h"


/**
 * Бинаризует изображение лабиринта
 * @param maze - лабиринт
 */
void binarize(Maze& maze);

/**
 * Находит границу лабиринта и устанавливает ее
 * в поле maze.boundary
 * @param maze - лабиринт
 */
void get_boundary(Maze& maze);

