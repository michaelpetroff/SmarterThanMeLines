#pragma once
#include<opencv2/opencv.hpp>

/**
 * Распознает и решает лабиринт
 * @param start - точка входа в лабиринт
 * @param end   - точка выхода из лабиринта
 * @param img   - изображение лабиринта
 * @return      - изображение с подсказкой
 */
cv::Mat solve(cv::Point start, cv::Point end, const cv::Mat& img);