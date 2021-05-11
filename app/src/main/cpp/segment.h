#pragma once
#include<opencv2/opencv.hpp>

// Отрезок
struct Segment {
    cv::Point source;
    cv::Point destination;

    bool operator==(const Segment& a) const {
        return source == a.source && destination == a.destination;
    }
};


struct SegmentHash {
    std::hash<int> int_hash;
    std::size_t operator()(const Segment& seg) const {
            std::size_t seed = 0;
            seed ^= int_hash(seg.source.x) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= int_hash(seg.source.y) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= int_hash(seg.destination.x) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= int_hash(seg.destination.y) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            return seed;
    }
};