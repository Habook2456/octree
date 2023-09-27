#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

using namespace std;

struct Point
{
    double x, y, z;

    Point() : x(-1), y(-1), z(-1) {}
    Point(double a, double b, double c) : x(a), y(b), z(c) {}
};

class Octree
{
public:
    vector<Octree *> children;
    vector<Point> points;

    Point bottomLeft;
    double h;

    int pointsPerOctant;
    int nPoints;
    bool isLeaf;

    Octree();
    Octree(double, Point, int);
    bool exist(const Point &);
    void insert(const Point &);
    vector<Point> find_closest(const Point &, int radius) const;
    bool isInside(const Point &) const;
    void createLineActors(std::vector<vtkSmartPointer<vtkActor>> &lineActors) const;
    bool intersectsWithSphere(const Point &, int radius) const;
    double distanceToOctant(const Point &, int radius) const;
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

void Octree::insert(const Point &p)
{

    if (isInside(p) && nPoints < pointsPerOctant)
    {
        if (children.empty())
        {
            double newH = h / 2.0;

            isLeaf = false;
            for (int i = 0; i < 8; i++)
            {
                Point newBottomLeft(
                    bottomLeft.x + (i & 1) * newH,
                    bottomLeft.y + ((i >> 1) & 1) * newH,
                    bottomLeft.z + ((i >> 2) & 1) * newH);
                children.push_back(new Octree(newH, newBottomLeft, pointsPerOctant));
            }
        }

        for (int i = 0; i < 8; i++)
        {

            if (children[i]->isInside(p) && children[i]->nPoints < children[i]->pointsPerOctant)
            {
                if (children[i]->isLeaf)
                {
                    children[i]->points.push_back(p);
                    children[i]->nPoints++;
                    break;
                }
                else
                {
                    children[i]->insert(p);
                    break;
                }
            }
            else if (children[i]->isInside(p) && children[i]->nPoints >= children[i]->pointsPerOctant)
            {
                children[i]->insert(p);
            }
        }
    }
    else
    {

        vector<Point> tempPoints = points;
        tempPoints.push_back(p);
        points.clear();

        if (children.empty())
        {
            double newH = h / 2.0;
            isLeaf = false;
            for (int i = 0; i < 8; i++)
            {
                Point newBottomLeft(
                    bottomLeft.x + (i & 1) * newH,
                    bottomLeft.y + ((i >> 1) & 1) * newH,
                    bottomLeft.z + ((i >> 2) & 1) * newH);
                children.push_back(new Octree(newH, newBottomLeft, pointsPerOctant));
            }
        }

        for (int i = 0; i < tempPoints.size(); i++)
        {
            for (int j = 0; j < 8; j++)
            {
                if (children[j]->isInside(tempPoints[i]) && children[j]->nPoints < children[j]->pointsPerOctant)
                {
                    children[j]->points.push_back(tempPoints[i]);
                    children[j]->nPoints++;
                    nPoints--;
                    break;
                }
                else if (children[j]->isInside(tempPoints[i]) && children[j]->nPoints >= children[j]->pointsPerOctant)
                {
                    children[j]->insert(tempPoints[i]);
                }
            }
        }
    }
}

// Implementación de createLineActors para generar actores de líneas en nodos hoja.
void Octree::createLineActors(std::vector<vtkSmartPointer<vtkActor>> &lineActors) const
{
    for (int i = 0; i < children.size(); i++)
    {
        if (children[i]->isLeaf)
        {
            double x0 = bottomLeft.x;
            double y0 = bottomLeft.y;
            double z0 = bottomLeft.z;
            double x1 = x0 + h;
            double y1 = y0 + h;
            double z1 = z0 + h;

            // Crear las líneas que representan el cubo del octante hoja.
            vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
            points->InsertNextPoint(x0, y0, z0);
            points->InsertNextPoint(x1, y0, z0);
            points->InsertNextPoint(x1, y1, z0);
            points->InsertNextPoint(x0, y1, z0);
            points->InsertNextPoint(x0, y0, z1);
            points->InsertNextPoint(x1, y0, z1);
            points->InsertNextPoint(x1, y1, z1);
            points->InsertNextPoint(x0, y1, z1);

            vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();
            lines->InsertNextCell(2);
            lines->InsertCellPoint(0);
            lines->InsertCellPoint(1);
            lines->InsertNextCell(2);
            lines->InsertCellPoint(1);
            lines->InsertCellPoint(2);
            lines->InsertNextCell(2);
            lines->InsertCellPoint(2);
            lines->InsertCellPoint(3);
            lines->InsertNextCell(2);
            lines->InsertCellPoint(3);
            lines->InsertCellPoint(0);
            lines->InsertNextCell(2);
            lines->InsertCellPoint(4);
            lines->InsertCellPoint(5);
            lines->InsertNextCell(2);
            lines->InsertCellPoint(5);
            lines->InsertCellPoint(6);
            lines->InsertNextCell(2);
            lines->InsertCellPoint(6);
            lines->InsertCellPoint(7);
            lines->InsertNextCell(2);
            lines->InsertCellPoint(7);
            lines->InsertCellPoint(4);
            lines->InsertNextCell(2);
            lines->InsertCellPoint(0);
            lines->InsertCellPoint(4);
            lines->InsertNextCell(2);
            lines->InsertCellPoint(1);
            lines->InsertCellPoint(5);
            lines->InsertNextCell(2);
            lines->InsertCellPoint(2);
            lines->InsertCellPoint(6);
            lines->InsertNextCell(2);
            lines->InsertCellPoint(3);
            lines->InsertCellPoint(7);

            vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
            polyData->SetPoints(points);
            polyData->SetLines(lines);

            vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper->SetInputData(polyData);

            vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
            actor->SetMapper(mapper);

            lineActors.push_back(actor);
        }
        else
        {
            children[i]->createLineActors(lineActors);
        }
    }
}

bool Octree::exist(const Point &p)
{
    if (!isInside(p))
    {
        cout << "Point not found" << endl;
        return false;
    }

    for (int i = 0; i < children.size(); i++)
    {
        if (children[i]->isInside(p))
        {
            cout << "Point is inside children [" << i << "]" << endl;
            if (children[i]->isLeaf)
            {
                for (const Point &point : children[i]->points)
                {
                    if (point.x == p.x && point.y == p.y && point.z == p.z)
                    {
                        cout << "Point found" << endl;
                        return true;
                    }
                }
            }
            else
            {
                return children[i]->exist(p);
            }
        }
    }

    cout << "Point not found" << endl;
    return false;
}

bool Octree::intersectsWithSphere(const Point &queryPoint, int radius) const
{
    double distance = distanceToOctant(queryPoint, radius);
    double maxDistance = radius + (h / 2.0);
    return distance <= maxDistance;
}

double euclideanDistance(const Point &point1, const Point &point2)
{
    return sqrt(pow(point1.x - point2.x, 2) + pow(point1.y - point2.y, 2) + pow(point1.z - point2.z, 2));
}

double Octree::distanceToOctant(const Point &queryPoint, int radius) const
{
    double x0 = bottomLeft.x + (h / 2.0);
    double y0 = bottomLeft.y + (h / 2.0);
    double z0 = bottomLeft.z + (h / 2.0);

    return euclideanDistance(queryPoint, Point(x0, y0, z0));
}

vector<Point> Octree::find_closest(const Point &queryPoint, int radius) const
{
    vector<Point> closestPoints;

    if (!isInside(queryPoint))
    {
        return closestPoints;
    }

    for (int i = 0; i < children.size(); i++)
    {
        if (children[i]->intersectsWithSphere(queryPoint, radius))
        {
            if (children[i]->isLeaf)
            {
                for (int j = 0; j < children[i]->points.size(); j++)
                {
                    double distance = euclideanDistance(queryPoint, children[i]->points[j]);
                    cout << distance << " <= " << radius << endl;
                    if (distance <= radius)
                    {
                        closestPoints.push_back(children[i]->points[j]);
                    }
                }
            }
            else
            {
                vector<Point> subOctantPoints = children[i]->find_closest(queryPoint, radius);
                closestPoints.insert(closestPoints.end(), subOctantPoints.begin(), subOctantPoints.end());
            }
        }
    }

    return closestPoints;
}

int main()
{
    //ctree tree(600, Point(-300, -300, -300), 15);
    Octree tree(400, Point(-200, -200, -200), 100);
    // Abre el archivo CSV
    std::ifstream file("points2.csv");

    if (!file.is_open())
    {
        std::cerr << "No se pudo abrir el archivo CSV." << std::endl;
        return 1;
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string token;
        std::vector<double> values; // Usar double en lugar de int para manejar valores decimales.

        while (std::getline(iss, token, ','))
        {
            double value = std::stod(token); // Cambio a std::stod para manejar valores decimales.
            values.push_back(value);
        }

        if (values.size() == 3)
        {
            Point point(values[0], values[1], values[2]);
            tree.insert(point);
        }
        else
        {
            std::cerr << "Formato de línea CSV incorrecto: " << line << std::endl;
        }
    }

    

    // Crear una ventana de renderizado VTK.
    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();

    // Crear un renderizador VTK.
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();

    // Conectar el renderizador a la ventana de renderizado.
    renderWindow->AddRenderer(renderer);

    // Crear un intérprete de ventana de renderizado VTK.
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();

    renderWindowInteractor->SetRenderWindow(renderWindow);

    // Recorre el octree y crea actores de líneas para los cubos de los octantes hoja.
    std::vector<vtkSmartPointer<vtkActor>> lineActors;
    tree.createLineActors(lineActors);

    // Agrega los actores de líneas al renderizador.
    for (auto actor : lineActors)
    {
        renderer->AddActor(actor);
    }

    // Inicia el renderizado interactivo.
    renderWindow->Render();
    renderWindowInteractor->Start();

    return 0;
}
