#include"boundary.h"

void binarize(Maze& maze) {
    threshold(maze.grim, maze.grim, 0, 255, cv::THRESH_BINARY + cv::THRESH_OTSU);
}

/**
 * Добавляет ребро в контур
 * @param edges        - контур
 * @param source       - начало ребра
 * @param destination  - конец ребра
 */
void add_edge(std::unordered_set<Segment, SegmentHash>& edges, const cv::Point& source, const cv::Point& destination) {
    // В случае если ребро уже есть в наборе мы его удаляем из него
    if (edges.count({ destination, source }) != 0) {
        edges.erase({ destination, source });
        return;
    }
    edges.insert({ source, destination });
}

/**
 * Строит треангуляцию Делоне
 * @param pts - набор точек
 * @return набор треугольников
 */
std::vector<std::vector<cv::Point2f>> Triangulate(std::vector<cv::Point2f>& pts){
    std::vector<std::vector<cv::Point2f>> result;

    std::vector<int> hull;
    std::vector<cv::Point2f> hull_pts;

    // Найдем выпуклую оболочку вокруг наших точек
    convexHull(pts, hull, false);

    // Перекинем точки оболочки в отдельный массив и найдем габарит этой оболочки
    // (нужно для инициализации триангулятора)

    cv::Point2f top_left(FLT_MAX, FLT_MAX);			   // min_x min_y
    cv::Point2f bottom_right(FLT_MIN, FLT_MIN);		   // max_x max_y


    for (int i = 0; i < hull.size(); i++){

        cv::Point2f p = pts[hull[i]];
        hull_pts.push_back(p);

        if (p.x < top_left.x) { top_left.x = p.x; }
        if (p.y < top_left.y) { top_left.y = p.y; }
        if (p.x > bottom_right.x) { bottom_right.x = p.x; }
        if (p.y > bottom_right.y) { bottom_right.y = p.y; }
    }

    // Немного раздвинем область
    top_left.x--;
    top_left.y--;
    bottom_right.x++;
    bottom_right.y++;

    // Габаритный прямоугольник наших точек, расширенный на 1 в каждую сторону
    cv::Rect region(top_left, bottom_right);

    // Инициализируем триангулятор
    cv::Subdiv2D subdiv(region);

    // Загружаем в него точки
    subdiv.insert(pts);


    // Набор треугольников
    std::vector<cv::Vec6f> triangleList;
    subdiv.getTriangleList(triangleList);

    std::vector<cv::Point2f> pt(3);

    for (size_t i = 0; i < triangleList.size(); i++){

        cv::Vec6f t = triangleList[i];


        pt[0] = cv::Point(t[0], t[1]);
        pt[1] = cv::Point(t[2], t[3]);
        pt[2] = cv::Point(t[4], t[5]);

        // нам нужна триангуляция выпуклого многогранника, а не всей области
        int p1 = pointPolygonTest(hull_pts, pt[0], false);
        int p2 = pointPolygonTest(hull_pts, pt[1], false);
        int p3 = pointPolygonTest(hull_pts, pt[2], false);

        // если точки треугольника внутри выпуклой оболчки, то запоминаем треугольник
        if (p1 > -1 && p2 > -1 && p3 > -1){
            result.push_back(pt);
        }
    }

    return result;
}


std::vector<Segment> alpha_shape(std::vector<cv::Point2f>& points, float alpha) {
    // Набор граней (контур)
    std::unordered_set<Segment,SegmentHash> edges;

    // Триангуляция Делоне
    std::vector<std::vector<cv::Point2f>> tri = Triangulate(points);

    for (auto& tr : tri) {
        cv::Point point_a = tr[0];
        cv::Point point_b = tr[1];
        cv::Point point_c = tr[2];

        // Находим радиус описанной кружности вокруг треугольника tr
        // S = a*b*c/4R => R = a*b*c/4S
        // S = sqrt(p*(p-a)*(p-b)*(p-c)) [формула Герона]

        // Длинны сторон треугольника
        float len_a = std::sqrt((point_a.x - point_b.x)* (point_a.x - point_b.x) + (point_a.y - point_b.y)* (point_a.y - point_b.y));
        float len_b = std::sqrt((point_b.x - point_c.x) * (point_b.x - point_c.x) + (point_b.y - point_c.y) * (point_b.y - point_c.y));
        float len_c = std::sqrt((point_c.x - point_a.x) * (point_c.x - point_a.x) + (point_c.y - point_a.y) * (point_c.y - point_a.y));

        // Полупериметр
        float p = (len_a + len_b + len_c) / 2.f;

        // Площадь
        float s = std::sqrt(p * (p - len_a) * (p - len_b) * (p - len_c));

        // Радиус описанной окружности
        float r = len_a * len_b * len_c / (4.f * s);

        // Если радиус меньше максимального (alpha), то добавляем ребро
        if (r < alpha) {
            add_edge(edges, point_a, point_b);
            add_edge(edges, point_b, point_c);
            add_edge(edges, point_c, point_a);
        }
    }


    // Записываем ответ в вектор
    std::vector<Segment> res;
    res.reserve(edges.size());

    for (auto& c : edges) {
        res.push_back(c);
    }

    return res;
}


void get_boundary(Maze& maze) {

    // Набор точек контуров изображения
    std::vector<std::vector<cv::Point>> cntr;

    // Поиск контуров
    findContours(maze.grim, cntr, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);
    std::vector<cv::Point2f> points;

    // Добавляем все точки всех контуров в один ветор
    // за исключением точек  на границе изображения
    for (const auto& c : cntr) {
        for (const auto& p : c) {
            if (p.x == 0 || p.y == 0 || p.x == maze.grim.cols - 1 || p.y == maze.grim.rows - 1) {
                continue;
            }
            points.emplace_back((float)p.x, (float)p.y);
        }
    }

    // Строим контур изображения
    std::vector<Segment> segments = alpha_shape(points, 100);

    maze.boundary = segments;
}