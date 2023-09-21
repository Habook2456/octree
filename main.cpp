#include <iostream>
#include <vector>

using namespace std;

struct Point
{
    int x;
    int y;
    int z;

    Point() : x(-1), y(-1), z(-1) {}
    Point(int a, int b, int c) : x(a), y(b), z(c) {}
};

class Octree
{
public:
    vector<Octree *> children;
    vector<Point> points;

    // bottomLeft y h definen el espacio(cubo más grande)
    Point bottomLeft;
    double h;

    int pointsPerOctant; // puntos por octante
    int nPoints;         // puntos ingresados.

    Octree();
    Octree(double, Point, int);
    bool exist(const Point &);
    void insert(const Point &);
    Point find_closest(const Point &, int radius) const;
    bool isInside(const Point &) const;
};

// octree constructor without arguments
Octree::Octree()
{
    nPoints = 0;
    children = vector<Octree *>(8, nullptr);
    h = 0.0;
    bottomLeft = Point();
}
// octree constructor with arguments height and bottomLeft
Octree::Octree(double _h, Point _bottomLeft, int _pointsPerOctant)
{
    nPoints = 0;
    h = _h;
    bottomLeft = _bottomLeft;
    pointsPerOctant = _pointsPerOctant;
}

// check if point is inside the octree method
bool Octree::isInside(const Point &p) const
{
    return (p.x >= bottomLeft.x && p.x <= bottomLeft.x + h &&
            p.y >= bottomLeft.y && p.y <= bottomLeft.y + h &&
            p.z >= bottomLeft.z && p.z <= bottomLeft.z + h);
}

// insert point in octree
void Octree::insert(const Point &p)
{
    // check if point is inside the octree
    if (isInside(p) && nPoints < pointsPerOctant)
    {
        cout << "Point is inside the octree" << endl;
        // check if octree have children
        if (children.empty())
        {
            double newH = h / 2.0;
            cout << "children is empty" << endl;

            // create 8 new octrees
            for (int i = 0; i < 8; i++)
            {
                Point newBottomLeft(
                    bottomLeft.x + (i & 1) * newH,
                    bottomLeft.y + ((i >> 1) & 1) * newH,
                    bottomLeft.z + ((i >> 2) & 1) * newH);
                // cout << "newBottomLeft[" << i << "]: (" << newBottomLeft.x << ", " << newBottomLeft.y << ", " << newBottomLeft.z << ")" << endl;
                children.push_back(new Octree(newH, newBottomLeft, pointsPerOctant));
            }
        }

        // insert point into octant
        for (int i = 0; i < 8; i++)
        {
            if (children[i]->isInside(p) && children[i]->nPoints < children[i]->pointsPerOctant)
            {
                cout << "Point is inside the children[" << i << "]" << endl;
                children[i]->points.push_back(p);
                children[i]->nPoints++;
            }
            else
            {
                children[i]->insert(p);
            }
        }
    }
}

int main()
{

    Octree tree(600, Point(-300, -300, -300), 5);
    tree.insert(Point(-122, -177, -71));
    tree.insert(Point(260, 236, -8));
    tree.insert(Point(-135, 28, -41));
    tree.insert(Point(248, 283, -23));

    for (int i = 0; i < tree.children[0]->points.size(); i++)
    {
        cout << "(" << tree.children[0]->points[i].x << ", " << tree.children[0]->points[i].y << ", " << tree.children[0]->points[i].z << ")" << endl;
    }

    for (int i = 0; i < tree.children[2]->points.size(); i++)
    {
        cout << "(" << tree.children[2]->points[i].x << ", " << tree.children[2]->points[i].y << ", " << tree.children[2]->points[i].z << ")" << endl;
    }

    for (int i = 0; i < tree.children[3]->points.size(); i++)
    {
        cout << "(" << tree.children[3]->points[i].x << ", " << tree.children[3]->points[i].y << ", " << tree.children[3]->points[i].z << ")" << endl;
    }



    return 0;
}