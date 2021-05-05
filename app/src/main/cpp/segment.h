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
        std::size_t lhs = int_hash(seg.source.x);
        std::size_t rhs = int_hash(seg.source.y);
        lhs ^= rhs + 0x9e3779b9 + (lhs << 6) + (lhs >> 2);
        rhs = int_hash(seg.destination.x);
        lhs ^= rhs + 0x9e3779b9 + (lhs << 6) + (lhs >> 2);
        rhs = int_hash(seg.destination.y);
        lhs ^= rhs + 0x9e3779b9 + (lhs << 6) + (lhs >> 2);
        return lhs;
    }
};