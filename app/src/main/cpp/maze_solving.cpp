#include "maze_solving.h"
#include "line_recognizing.h"

/**
 * Находит средний отступ от краев лабиринта
 * @param line_color - цвет линии
 * @param clim - цветное hsv изображение лабиринта
 * @param grim - черно-белое изображение лабиринта
 * @return среднее расстояние между стенками лабиринта
 */
int get_gap(cv::Vec3b line_color, cv::Mat& clim, cv::Mat& grim) {
    long long cnt = 1;
    long long sum = 1;

    int x = 0, y = 0;
    bool fl = false;
    while (y < clim.rows) {
        x = 0;
        while (x < clim.cols) {
            while (x < clim.cols && (int)grim.at<uchar>({ x,y }) == 0 && !comp(clim.at<cv::Vec3b>({ x,y }), line_color)) {
                fl = true;
                x++;
            }
            if (fl && x != clim.cols) {
                int tmp = 0;
                while (x < clim.cols && ((int)grim.at<uchar>({ x,y }) == 255 || comp(clim.at<cv::Vec3b>({ x,y }), line_color))) {
                    tmp++;
                    x++;
                }
                if (x != clim.cols) {
                    cnt++;
                    sum += tmp;
                }
                fl = false;
            }
            else {
                while (x < clim.cols && ((int)grim.at<uchar>({ x,y }) == 255 || comp(clim.at<cv::Vec3b>({ x,y }), line_color)))
                    x++;
            }
        }
        y++;
    }
    return sum / cnt;
}


std::vector<cv::Point> get_trace(Maze& maze) {
    maze.clear_d();

    std::vector<cv::Point> result;
    cv::Vec3d line_color = maze.clim.at<cv::Vec3b>(maze.end_line);
    if(maze.start_line != maze.start)
        line_color = maze.line_color;


    maze.d[maze.end_line.x][maze.end_line.y] = 0;
    std::queue<cv::Point> q;
    q.push(maze.end_line);

    // Функция получения цвета текущей точки
    auto cur_col = [&maze](int x, int y) {return maze.clim.at<cv::Vec3b>({ x,y }); };

    // Находим отступ
    int gap = std::max(get_gap(line_color, maze.clim, maze.grim) / 6, 2);

    // bfs
    while (!q.empty()) {
        cv::Point p = q.front();
        if (p == maze.end)
            break;
        q.pop();

        if (maze.d[p.x][p.y] < 0)
            continue;



        // Проверка точки на отступ
        int i = 1;
        while (i < gap) {
            if (p.x - i >= 0 && cv::Point{ p.x - i, p.y } == maze.end
                || p.y - i >= 0 && cv::Point(p.x, p.y - i) == maze.end
                || p.y + i <= maze.clim.rows && cv::Point(p.x, p.y + i) == maze.end
                || p.x + i <= maze.clim.cols && cv::Point(p.x + i, p.y) == maze.end) {
                i = gap;
                break;
            }

            if (!(p.x - i >= 0 && ((int)maze.grim.at<uchar>({ p.x - i, p.y }) == 255 || comp(cur_col(p.x - i, p.y), line_color))))
                break;
            if (!(p.y - i >= 0 && ((int)maze.grim.at<uchar>({ p.x, p.y - i }) == 255 || comp(cur_col(p.x, p.y - i), line_color))))
                break;
            if (!(p.y + i < maze.clim.rows && ((int)maze.grim.at<uchar>({ p.x, p.y + i }) == 255 || comp(cur_col(p.x, p.y + i), line_color))))
                break;
            if (!(p.x + i < maze.clim.cols && ((int)maze.grim.at<uchar>({ p.x + i, p.y }) == 255 || comp(cur_col(p.x + i, p.y), line_color))))
                break;
            i++;
        }
        if (i != gap)
            continue;

        // Добавление новых точек в очередь
        if (p.x - 1 >= 0 && maze.d[p.x - 1][p.y] == -1 && ((int)maze.grim.at<uchar>({ p.x - 1, p.y }) == 255 || comp(cur_col(p.x - 1, p.y), line_color))) {
            maze.d[p.x - 1][p.y] = maze.d[p.x][p.y] + 1;
            q.push({ p.x - 1, p.y });
        }
        if (p.y - 1 >= 0 && maze.d[p.x][p.y - 1] == -1 && ((int)maze.grim.at<uchar>({ p.x, p.y - 1 }) == 255 || comp(cur_col(p.x, p.y - 1), line_color))) {
            maze.d[p.x][p.y - 1] = maze.d[p.x][p.y] + 1;
            q.push({ p.x, p.y - 1 });
        }
        if (p.y + 1 < maze.clim.rows && maze.d[p.x][p.y + 1] == -1 && ((int)maze.grim.at<uchar>({ p.x, p.y + 1 }) == 255 || comp(cur_col(p.x, p.y + 1), line_color))) {
            maze.d[p.x][p.y + 1] = maze.d[p.x][p.y] + 1;
            q.push({ p.x, p.y + 1 });
        }
        if (p.x + 1 < maze.clim.cols && maze.d[p.x + 1][p.y] == -1 && ((int)maze.grim.at<uchar>({ p.x + 1, p.y }) == 255 || comp(cur_col(p.x + 1, p.y), line_color))) {
            maze.d[p.x + 1][p.y] = maze.d[p.x][p.y] + 1;
            q.push({ p.x + 1, p.y });
        }
    }

    // Восстанавливаем путь с конца
    cv::Point cur = maze.end;
    if (maze.d[cur.x][cur.y] == -1)
        throw std::exception();
    int dir = -1;

    // Функции проверки хода влево,вправо,вниз,вверх
    auto left = [&cur,&dir, &maze] {
        if (cur.x - 1 >= 0 && maze.d[cur.x - 1][cur.y] + 1 == maze.d[cur.x][cur.y]) {
            cur = { cur.x - 1, cur.y };
            dir = 0;
            return true;
        }
        return false;
    };
    auto up = [&cur,&dir, &maze] {
        if (cur.y - 1 >= 0 && maze.d[cur.x][cur.y - 1] + 1 == maze.d[cur.x][cur.y]) {
            cur = { cur.x, cur.y - 1 };
            dir = 1;
            return true;
        }
        return false;
    };
    auto right = [&dir ,&cur,&maze] {
        if (cur.x + 1 < maze.clim.cols && maze.d[cur.x + 1][cur.y] + 1 == maze.d[cur.x][cur.y]) {
            cur = { cur.x + 1, cur.y };
            dir = 2;
            return true;
        }
        return false;
    };
    auto down = [&dir, &cur, &maze] {
        if (cur.y + 1 < maze.clim.rows && maze.d[cur.x][cur.y + 1] + 1 == maze.d[cur.x][cur.y]) {
            cur = { cur.x, cur.y + 1 };
            dir = 3;
            return true;
        }
        return false;
    };
    std::vector<std::function<bool()>> v = { left, up, right, down };

    // Восстанавливаем путь при этом
    // пытаемся при каждом ходе менять направление,
    // если это возможно
    while (maze.d[cur.x][cur.y] != 0) {
        result.push_back(cur);

        if (dir == -1) {
            int i = 0;
            while (i < 4 && !(v[i]()))i++;
        }
        else {
            int i = 0;
            bool fl = false;
            while (i < 4) {
                if (dir != i) {
                    if (v[i]()) {
                        fl = true;
                        break;
                    }
                }
                i++;
            }
            if (!fl)
                v[dir]();
        }

    }

    return result;
}