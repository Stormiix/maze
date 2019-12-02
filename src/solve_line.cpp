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

    Position(int _x, int _y, int distance) : Point(_x, _y) {}

    int distToParent()
    {
        return distance;
    }

    bool isXCorridor(int x, int y) const {
        return !isFree(x, y+1) && !isFree(x, y-1);
    }

    bool isYCorridor(int x, int y) const {
        return !isFree(x+1, y) && !isFree(x-1, y);
    }
    bool isCorridor(int x, int y) const {
        return isXCorridor(x,y) || isYCorridor(x,y);
    }
    bool isFree(int x, int y) const {
        return Position::maze.isFree(x, y);
    }

    int freeCompassCells(int x, int y) const {
        int counter = 0;
        for (int i = -1; i <= 1; i++)
        {
            for (int j = -1; j <= 1; j++)
            {
                if(i!=j){
                    counter += isFree(x+i,y+j) ? 1 : 0;
                }
            }
        }
        return counter;
    }
    bool isCorner(int x, int y) const {
        bool is = false;
        // TR
        if(isFree(x,y+1) && isFree(x+1,y)){
            is = true;
        }
        // TL
        if(isFree(x,y+1) && isFree(x-1,y)){
            is = true;
        }
        // BR
        if(isFree(x,y-1) && isFree(x+1,y)){
            is = true;
        }
        // BL
        if(isFree(x,y-1) && isFree(x+1,y)){
            is = true;
        }  
        return is;
    }
    bool isIntersection(int x, int y) const {
        return freeCompassCells(x,y) >= 3;
    }
    bool isDeadend(int x, int y) const {
        return freeCompassCells(x,y) == 1;
    }
    bool isChild(int cx, int cy) const {
        return isCorner(cx, cy) || isIntersection(cx, cy) || isDeadend(cx,cy);
    }
    std::vector<PositionPtr> children()
    {
        // this method should return  all positions reachable from this one
        std::vector<PositionPtr> generated;
        std::cout << "---------------------------------- " << std::endl; 
        std::cout << "Currently at "<< x << "," << y << std::endl; 
        int cx;
        int cy;
        for (int i = -1; i <= 1; i++)
        {
            for (int j = -1; j <= 1; j++)
            {
                // Follow corridor
                cx = x+i;
                cy = y+j;
                std::cout << "Checking at "<< cx << "," << cy << " | Free: " << isFree(x+i,y+j) << std::endl; 
                if(i!=j && isFree(x+i,y+j)){
                    std::cout << "Going at "<< cx << "," << cy << std::endl; 
                    if(isCorridor(cx, cy)){
                        while(isCorridor(cx, cy)){
                            std::cout << "Following a corridor at "<< cx << "," << cy << std::endl; 
                            int xStep = 0;
                            int yStep = 0;
                            if(isXCorridor(cx,cy) && !isCorner(cx, cy) && !isIntersection(cx, cy) && !isDeadend(cx, cy) && i!=0){
                                xStep = isFree(cx + i, cy) ? i : isFree(cx - i, cy) ? -i : 0;
                            }
                            if(isYCorridor(cx,cy) && !isCorner(cx, cy) && !isIntersection(cx, cy) && !isDeadend(cx, cy)  && j!=0){
                                yStep = isFree(cx, cy+j) ? j : isFree(cx, cy-j) ? -j : 0;
                            }
                            
                            if(xStep == yStep && xStep == 0){
                                break;
                            }
                            cx += xStep;
                            cy += yStep;
                            if(isChild(cx,cy)){
                                std::cout << "Found at "<< cx << "," << cy << std::endl; 
                                if(isFree(cx, cy)){
                                    generated.push_back(std::make_unique<Position>(cx,cy, sqrt(pow(cx-x, 2) + pow(cy-y, 2))));
                                }
                                break;
                            }
                        }
                    }else if(isCorner(cx, cy) || isIntersection(cx, cy) || isDeadend(cx,cy)){
                        std::cout << "Found at "<< cx << "," << cy << std::endl; 
                        if(isFree(cx, cy)){
                            generated.push_back(std::make_unique<Position>(cx,cy, sqrt(pow(cx-x, 2) + pow(cy-y, 2))));
                        }
                    }

                }
            }
        }
        return generated;
    }
    int distance;
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
    Position::maze.saveSolution("cell");
    cv::waitKey(0);
}
