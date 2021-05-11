#include"line_recognizing.h"
#include<algorithm>
#include"boundary.h"


bool comp(cv::Vec3b a, cv::Vec3b b) {
    int black_v_max = 95;
    int white_s_max = 25;
    int white_v_min = 180;


    int h_a, s_a, v_a, h_b, s_b, v_b;
    h_a = a[0];
    s_a = a[1];
    v_a = a[2];

    h_b = b[0];
    s_b = b[1];
    v_b = b[2];

    if (s_a < white_s_max && s_b < white_s_max)
        return abs(v_a - v_b) < 60;


    if (s_a < white_s_max && v_a > white_v_min && s_b < white_s_max && v_b > white_v_min)
        return true;
    else if (v_a < black_v_max && v_b < black_v_max)
        return true;
    else if (!((s_a < white_s_max && v_a > white_v_min) || (s_b < white_s_max && v_b > white_v_min) || (v_a < black_v_max) || (v_b < black_v_max)))
        return std::min(std::abs(h_a - h_b), 180 - std::abs(h_a - h_b)) < 20;
    else
        return false;
}


void get_wall_color(Maze& maze) {
    long long h = 0, s = 0, v = 0;
    cv::Vec3b res;
    long long cnt = 0;
    for (const auto& c : maze.boundary) {
        cv::Point start = c.source;
        if (start.x - 1 >= 0 && (int)maze.grim.at<uchar>(cv::Point(start.x - 1, start.y)) == 0) {
            cv::Vec3b tmp = maze.clim.at<cv::Vec3b>(cv::Point(start.x - 1, start.y));
            h += tmp[0];
            s += tmp[1];
            v += tmp[2];
            cnt++;
        }
        else if (start.y - 1 >= 0 && (int)maze.grim.at<uchar>(cv::Point(start.x, start.y - 1)) == 0) {
            cv::Vec3b tmp = maze.clim.at<cv::Vec3b>(cv::Point(start.x, start.y - 1));
            h += tmp[0];
            s += tmp[1];
            v += tmp[2];
            cnt++;
        }
        else if (start.x + 1 < maze.grim.cols && (int)maze.grim.at<uchar>(cv::Point(start.x + 1, start.y)) == 0) {
            cv::Vec3b tmp = maze.clim.at<cv::Vec3b>(cv::Point(start.x + 1, start.y));
            h += tmp[0];
            s += tmp[1];
            v += tmp[2];
            cnt++;
        }
        else if (start.y + 1 < maze.grim.rows && (int)maze.grim.at<uchar>(cv::Point(start.x, start.y + 1)) == 0) {
            cv::Vec3b tmp = maze.clim.at<cv::Vec3b>(cv::Point(start.x, start.y + 1));
            h += tmp[0];
            s += tmp[1];
            v += tmp[2];
            cnt++;
        }
    }

    res[0] = h / cnt;
    res[1] = s / cnt;
    res[2] = v / cnt;

    maze.wall_color = res;
}

void find_line(Maze& maze) {
    maze.clear_d();
    maze.d[maze.start.x][maze.start.y] = 0;
    std::queue<cv::Point> points;

    points.push(maze.start);

    while (!points.empty()) {

        cv::Point tmp = points.front();

        points.pop();
        uchar p = maze.grim.at<uchar>({ tmp.x, tmp.y });


        cv::Vec3b cl = maze.clim.at<cv::Vec3b>(cv::Point(tmp.x, tmp.y));
        if (!comp(cl, maze.floor_color) && !comp(cl, maze.wall_color)) {
            maze.start_line = tmp;
            return;
        }

        if ((int)p == 0) {
            continue;
        }
        if (tmp.x - 1 >= 0 && maze.d[tmp.x - 1][tmp.y] == -1) {
            maze.d[tmp.x - 1][tmp.y] = maze.d[tmp.x][tmp.y] + 1;
            points.push({ tmp.x - 1, tmp.y });
        }
        if (tmp.y - 1 >= 0 && maze.d[tmp.x][tmp.y - 1] == -1) {
            maze.d[tmp.x][tmp.y - 1] = maze.d[tmp.x][tmp.y] + 1;
            points.push({ tmp.x, tmp.y - 1 });
        }
        if (tmp.x + 1 < maze.grim.cols && maze.d[tmp.x + 1][tmp.y] == -1) {
            maze.d[tmp.x + 1][tmp.y] = maze.d[tmp.x][tmp.y] + 1;
            points.push({ tmp.x + 1, tmp.y });
        }
        if (tmp.y + 1 < maze.grim.rows && maze.d[tmp.x][tmp.y + 1] == -1) {
            maze.d[tmp.x][tmp.y + 1] = maze.d[tmp.x][tmp.y] + 1;
            points.push({ tmp.x, tmp.y + 1 });
        }
    }
    maze.start_line = maze.start;
    maze.end_line = maze.start;
}


void get_floor_color(Maze& maze) {
    std::queue<cv::Point> q;
    q.push(maze.start);

    maze.clear_d();

    while (!q.empty()) {
        cv::Point tmp = q.front();
        q.pop();

        maze.d[tmp.x][tmp.y] = 0;
        if ((int)maze.grim.at<uchar>(tmp) == 255) {
            maze.floor_color = maze.clim.at<cv::Vec3b>(tmp);
            return;
        }

        if (tmp.x > 0 && maze.d[tmp.x - 1][tmp.y] == -1)
            q.push({ tmp.x - 1, tmp.y });
        if (tmp.y > 0 && maze.d[tmp.x][tmp.y - 1] == -1)
            q.push({ tmp.x, tmp.y - 1 });
        if (tmp.x < maze.grim.cols - 1 && maze.d[tmp.x + 1][tmp.y] == -1)
            q.push({ tmp.x + 1, tmp.y });
        if (tmp.y < maze.grim.rows - 1 && maze.d[tmp.x][tmp.y + 1] == -1)
            q.push({ tmp.x, tmp.y + 1 });
    }
}


void get_end_of_line(Maze& maze) {
    int x_max = maze.start_line.x, y_max = maze.start_line.y;

    maze.clear_d();
    maze.d[maze.start_line.x][maze.start_line.y] = 0;
    std::queue<cv::Point> q;
    q.push(maze.start_line);
    auto line_col = maze.clim.at<cv::Vec3b>(maze.start_line);
    long long h = line_col[0], s = line_col[1], v = line_col[2], cnt = 1;
    while (!q.empty()) {
        cv::Point p = q.front();
        q.pop();

        if (maze.d[p.x][p.y] > maze.d[x_max][y_max]) {
            x_max = p.x;
            y_max = p.y;
        }
        auto cur_col = [&maze](int x, int y) {return maze.clim.at<cv::Vec3b>({ x,y }); };
        if (p.x - 1 >= 0 && maze.d[p.x - 1][p.y] == -1 && comp(cur_col(p.x - 1, p.y), line_col)) {
            cnt++;
            h += cur_col(p.x - 1, p.y)[0];
            s += cur_col(p.x - 1, p.y)[1];
            v += cur_col(p.x - 1, p.y)[2];
            q.push({ p.x - 1, p.y });
            maze.d[p.x - 1][p.y] = maze.d[p.x][p.y] + 1;
        }
        if (p.y - 1 >= 0 && maze.d[p.x][p.y - 1] == -1 && comp(cur_col(p.x, p.y - 1), line_col)) {
            cnt++;
            h += cur_col(p.x, p.y - 1)[0];
            s += cur_col(p.x, p.y - 1)[1];
            v += cur_col(p.x, p.y - 1)[2];
            q.push({ p.x, p.y - 1 });
            maze.d[p.x][p.y - 1] = maze.d[p.x][p.y] + 1;
        }
        if (p.x + 1 < maze.clim.cols && maze.d[p.x + 1][p.y] == -1 && comp(cur_col(p.x + 1, p.y), line_col)) {
            cnt++;
            h += cur_col(p.x + 1, p.y)[0];
            s += cur_col(p.x + 1, p.y)[1];
            v += cur_col(p.x + 1, p.y)[2];
            q.push({ p.x + 1, p.y });
            maze.d[p.x + 1][p.y] = maze.d[p.x][p.y] + 1;
        }
        if (p.y + 1 < maze.clim.rows && maze.d[p.x][p.y + 1] == -1 && comp(cur_col(p.x, p.y + 1), line_col)) {
            cnt++;
            h += cur_col(p.x, p.y+1)[0];
            s += cur_col(p.x, p.y+1)[1];
            v += cur_col(p.x, p.y+1)[2];
            q.push({ p.x, p.y + 1 });
            maze.d[p.x][p.y + 1] = maze.d[p.x][p.y] + 1;
        }
    }
    maze.line_color[0] = h / cnt;
    maze.line_color[1] = s / cnt;
    maze.line_color[2] = v / cnt;
    maze.end_line = {x_max, y_max};
}

