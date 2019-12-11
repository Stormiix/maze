#include <a_star.h>
#include <maze.h>
#include <cmath>
using namespace std;
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

    Position(int _x, int _y, int dist) : Point(_x, _y) {
        distance = dist;
    }

    int distToParent()
    {
        return distance;
    }
    bool isFree(int x, int y) const
    {
        return Position::maze.isFree(x, y);
    } 

    int freeNeighboors(int x, int y){
        int counter = 0;
        int cx;
        int cy;
        for (int i = 0; i < 4; i++)
        {
            cx = x + dx[i];
            cy = y + dy[i];
            if(isFree(cx,cy)){
                counter++;
            }
        }
        return counter;
    }

    bool isCorridor(int x, int y){
        return freeNeighboors(x,y) == 2;
    }
    std::vector<PositionPtr> children()
    {
        // this method should return  all positions reachable from this one
        std::vector<PositionPtr> generated;
        int cx;
        int cy;
        for (int i = 0; i < 4; i++)
        {
            cx = x + dx[i];
            cy = y + dy[i];
            if (isFree(cx , cy))
            {
                while(isCorridor(cx, cy)){
                    if(isFree(cx + dx[i], cy +dy[i])){
                        cx += dx[i];
                        cy += dy[i];
                    }else{
                        break;
                    }
                }
                generated.push_back(std::make_unique<Position>(cx, cy, Point(cx, cy).h(Point(x,y), use_manhattan))); // TODO: use h()
            }
        }
        return generated;
    }
    bool use_manhattan = true;
    int distance = 0; 
    std::vector<int> dx{-1, 0, 1, 0};
    std::vector<int> dy{0, 1, 0, -1};
};

int main(int argc, char **argv)
{
    // load file
    std::string filename = "maze.png";
    if (argc == 2)
        filename = std::string(argv[1]);

    // let Point know about this maze
    Position::maze.load(filename);

    // initial and goal positions as Position's
    Position start = Position::maze.start(),
             goal = Position::maze.end();

    // call A* algorithm
    ecn::Astar(start, goal);

    // save final image
    Position::maze.saveSolution("line");
    cv::waitKey(0);
}
