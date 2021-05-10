#pragma once
#include<opencv2/opencv.hpp>
#include<vector>
#include "segment.h"

/**
 * Класс лабиринта
 */
class Maze {
public:
    Maze(cv::Mat& img, cv::Mat& gr_img, cv::Point& st, cv::Point en) :clim(img), grim(gr_img), start(st), end(en) {
        d.resize(clim.cols);
        for (auto& row : d) {
            row.resize(clim.rows, -1);
        }
    }
    // Очищает матрицу расстояний ставя
    // всем клеткам значение -1
    void clear_d() {
        for (auto& row : d)
            for (auto& item : row)
                item = -1;
    }

    // граница лабиринта в виде массива отрезков
    std::vector<Segment> boundary;
    // Цветное hsv изображение лабиринта и черно-белое
    cv::Mat clim, grim;
    // Матрица расстояний для bfs
    std::vector<std::vector<int>> d;
    // Точки входа, выхода из лабиринта и начала
    // и конца линии пользователя
    cv::Point start, end, start_line, end_line;
    // Цвет стен лабиринта и пола
    cv::Vec3b wall_color, floor_color, line_color;
};