#include <a_star.h>
#include <maze.h>
#include <cmath>
using namespace ecn;

// a node is a x-y position, we move from 1 each time
class Position : public Point
{
    typedef std::unique_ptr<Position> PositionPtr;

public:
    // constructor from coordinates
    Position(int _x, int _y) : Point(_x, _y) {}

    // constructor from base ecn::Point
    Position(ecn::Point p) : Point(p.x, p.y) {}

    Position(int _x, int _y, int dist) : Point(_x, _y)
    {
        distance = dist;
    }

    // maze deals with the color, just tell the points
    void print(const Point &parent)
    {
        int sx = startPoint.x;
        int sy = startPoint.y;
        std::vector<Point> visitedCells;
        for (int k = 0; k < 4; k++)
        {
            visitedCells.clear();
            int offsetX = dx[k];
            int offsetY = dy[k];
            int cx = x + offsetX;
            int cy = y + offsetY;
            if (isFree(cx, cy))
            {
                visitedCells.push_back(Point(cx, cy));
                while (isCorridor(cx, cy, offsetX, offsetY) && (cx != sx || cy != sy))
                {
                    cx += offsetX;
                    cy += offsetY;
                    visitedCells.push_back(Point(cx, cy));
                }
                if (cx == parent.x && cy == parent.y)
                {
                    for (const auto &p : visitedCells)
                    {
                        maze.passThrough(p.x, p.y);
                    }
                    break;
                }
            }
        }
        maze.passThrough(x, y);
    }

    // online print, color depends on closed / open set
    void show(bool closed, const Point &parent)
    {
        const int b = closed ? 255 : 0, r = closed ? 0 : 255;
        int sx = startPoint.x;
        int sy = startPoint.y;
        std::vector<Point> visitedCells;
        for (int k = 0; k < 4; k++)
        {
            visitedCells.clear();
            int offsetX = dx[k];
            int offsetY = dy[k];
            int cx = x + offsetX;
            int cy = y + offsetY;

            if (isFree(cx, cy))
            {
                visitedCells.push_back(Point(cx, cy));
                while (isCorridor(cx, cy, offsetX, offsetY) && (cx != sx || cy != sy))
                {
                    cx += offsetX;
                    cy += offsetY;
                    visitedCells.push_back(Point(cx, cy));
                }
                if (cx == parent.x && cy == parent.y)
                {
                    for (const auto &p : visitedCells)
                    {
                        maze.write(p.x, p.y, r, 0, b, false);
                    }
                    break;
                }
            }
        }
        maze.write(x, y, r, 0, b);
    }

    int distToParent()
    {
        return distance;
    }

    bool isFree(int x, int y) const
    {
        return Position::maze.isFree(x, y);
    }

    bool isCorridor(int x, int y, int &dx, int &dy)
    {
        if (dx && (int)isFree(x + dx, y) + (int)isFree(x, y + 1) + (int)isFree(x, y - 1) == 1)
        {
            dy = isFree(x, y + 1) ? 1 : isFree(x, y - 1) ? -1 : dy;
            dx = isFree(x, y + 1) || isFree(x, y - 1) ? 0 : dx;
            return true;
        }
        else if (dy && (int)isFree(x, y + dy) + (int)isFree(x + 1, y) + (int)isFree(x - 1, y) == 1)
        {
            dy = isFree(x - 1, y) || isFree(x + 1, y) ? 0 : dy;
            dx = isFree(x-1,y) ? -1 : isFree(x+1, y) ? 1 : dx;
            return true;
        }
        return false;
    }
    std::vector<PositionPtr> children()
    {
        int ex = endPoint.x;
        int ey = endPoint.y;
        // this method should return  all positions reachable from this one
        std::vector<PositionPtr> generated;
        for (int k = 0; k < 4; k++)
        {
            int offsetX = dx[k];
            int offsetY = dy[k];
            int cx = x + offsetX;
            int cy = y + offsetY;

            if (isFree(cx, cy))
            {
                while (isCorridor(cx, cy, offsetX, offsetY) && (cx != ex || cy != ey))
                {
                    cx += offsetX;
                    cy += offsetY;
                }
                generated.push_back(std::make_unique<Position>(cx, cy, Point(cx, cy).h(Point(x,y), use_manhattan)));
            }
        }
        return generated;
    }
    static void setInterestPositions(Position s, Position e){
        startPoint = s;
        endPoint = e;
    }
    int distance = 0;
    bool use_manhattan = true;
    std::vector<int> dx{-1, 0, 1, 0};
    std::vector<int> dy{0, 1, 0, -1};
    static Point startPoint, endPoint;
};
Point Position::startPoint, Position::endPoint;

int main(int argc, char **argv)
{
    // load file
    std::string filename = "maze.png";
    if (argc == 2)
        filename = std::string(argv[1]);

    // let Point know about this maze
    Position::maze.load(filename);

    // initial and goal positions as Position's
    Position start = Position::maze.start(), goal = Position::maze.end();
    Position::setInterestPositions(start, goal);

    // call A* algorithm
    ecn::Astar(start, goal);

    // save final image
    Position::maze.saveSolution("corridor");
    cv::waitKey(0);
}
