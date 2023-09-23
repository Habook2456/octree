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
    bool isLeaf;         // es hoja

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
    isLeaf = true;
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
        cout << "PUNTO PARA INSERTAR NORMAL: " << p.x << " " << p.y << " " << p.z << endl;
        // check if octree have children
        if (children.empty())
        {
            double newH = h / 2.0;
            cout << "children is empty" << endl;

            isLeaf = false;
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
                if (children[i]->isLeaf)
                {
                    cout << "PUNTO PARA INSERTADO EN OCTANTE[" << i << "]: " << p.x << " " << p.y << " " << p.z << endl;
                    children[i]->points.push_back(p);
                    children[i]->nPoints++;
                    break;
                }
                else
                {
                    cout << "no es hoja compa, que paso? :')" << endl;
                    children[i]->insert(p);
                    break;
                }
            }
            else if (children[i]->isInside(p) && children[i]->nPoints >= children[i]->pointsPerOctant)
            {
                cout << "PUNTO PARA RECURSION EN OCTANTE[" << i << "]: " << p.x << " " << p.y << " " << p.z << endl;
                children[i]->insert(p);
            }
        }
    }
    else
    {
        cout << "PUNTO PARA INSERTAR EN RECURSION: " << p.x << " " << p.y << " " << p.z << endl;

        vector<Point> tempPoints = points;
        tempPoints.push_back(p);
        points.clear();
        cout << "PUNTOS PARA RE-INSERCION: " << endl;
        for (int i = 0; i < tempPoints.size(); i++)
        {
            cout << "-> " << tempPoints[i].x << " " << tempPoints[i].y << " " << tempPoints[i].z << endl;
        }

        if (children.empty())
        {
            double newH = h / 2.0;
            cout << "children is empty RECURSION" << endl;
            isLeaf = false;
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

        // insert tempPoints into octants
        for (int i = 0; i < tempPoints.size(); i++)
        {
            for (int j = 0; j < 8; j++)
            {
                if (children[j]->isInside(tempPoints[i]) && children[j]->nPoints < children[j]->pointsPerOctant)
                {
                    cout << "PUNTO PARA RE-INSERTADO EN OCTANTE[" << j << "]: " << tempPoints[i].x << " " << tempPoints[i].y << " " << tempPoints[i].z << endl;
                    children[j]->points.push_back(tempPoints[i]);
                    children[j]->nPoints++;
                    nPoints--;
                    break;
                }
                else if (children[j]->isInside(tempPoints[i]) && children[j]->nPoints >= children[j]->pointsPerOctant)
                {
                    cout << "PUNTO PARA RECURSION EN OCTANTE[" << j << "]: " << tempPoints[i].x << " " << tempPoints[i].y << " " << tempPoints[i].z << endl;
                    children[j]->insert(tempPoints[i]);
                }
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
    tree.insert(Point(248, 283, -22));
    tree.insert(Point(248, 283, -21));
    tree.insert(Point(248, 283, -19));
    tree.insert(Point(248, 283, -170));
    tree.insert(Point(1, 1, -250));
    tree.insert(Point(2, 2, -51));

    cout << tree.isLeaf << endl;
    cout << tree.children[2]->isLeaf << endl;
    cout << tree.children[3]->isLeaf << endl;
    cout << tree.children[4]->isLeaf << endl;

    return 0;
}