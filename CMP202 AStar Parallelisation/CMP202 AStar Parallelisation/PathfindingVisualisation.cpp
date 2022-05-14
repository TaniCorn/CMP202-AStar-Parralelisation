#include "PathfindingVisualisation.h"
#include "Input.h"

void PathfindingVisualisation::Init(Input* in, sf::RenderWindow* wind,sf::View* vie)
{


    input = in;
    window = wind;
    view = vie;
    viewCenter = vie->getCenter();

    //Set up shapes for viewing
    #pragma region  SHAPES
    std::vector<std::thread> threads;
    std::mutex lock;
    int shapeNumber = 0; const int shapesNeeded = 10000;
    for (int threadIndex = 0; threadIndex < std::thread::hardware_concurrency(); threadIndex++)
    {
        threads.push_back(std::thread([&]() {

            while (shapeNumber < shapesNeeded)
            {
                lock.lock();
                if (shapeNumber == shapesNeeded)
                {
                    lock.unlock();
                    return;
                }			
                rectDraw.push_back(sf::RectangleShape());
                int iterRD = shapeNumber;
                shapeNumber++;
                lock.unlock();

                rectDraw[iterRD].setSize(sf::Vector2f(10, 10));
                rectDraw[iterRD].setOutlineColor(sf::Color::Black);
                rectDraw[iterRD].setOutlineThickness(1.f);
                rectDraw[iterRD].setFillColor(sf::Color::Red);
            }
            }));
    }
    #pragma endregion
   
    int threadsToUse = 1;
    //Procedural Map Manager Setup
    pmm.threadsToUse = threadsToUse;
    pmm.Init(3, 100, 100, 100);
    pmm.GenerateMapGrid();
    pmm.ConnectRooms();

    //Pathfinding agent recieving maps from pmm
    pathfindingAgent_CPU = new A_Star_Pathfinding_Defined_SegmentedCPU();
    pathfindingAgent_CPU->threadsToUse = threadsToUse;
    for (int y = 0; y < pmm.yDimension; y++)
    {
        for (int x = 0; x < pmm.xDimension; x++)
        {
            pathfindingAgent_CPU->rooms.push_back(&pmm.roomsInMap[x][y]);
        }
    }

    //Initial Pathfinding
    Vector2<int> initialStart = Vector2<int>(pmm.roomsInMap[0][0].nodes[pmm.xRoomDimension / 2][pmm.yRoomDimension / 2].xPosition,
        pmm.roomsInMap[0][0].nodes[pmm.xRoomDimension / 2][pmm.yRoomDimension / 2].yPosition);
    Vector2<int> initialTarget = Vector2<int>(pmm.roomsInMap[0][0].nodes[20][20].xPosition, pmm.roomsInMap[0][0].nodes[20][20].yPosition);
    pathfindingAgent_CPU->FindPath(initialStart, initialTarget);
    pathfindingAgent_CPU->PrintRoute();

    std::for_each(threads.begin(), threads.end(), [](std::thread& t)
        {
            t.join();
        });

    UpdateObjects(Vector2<int>(0, 0), &pmm, &rectDraw);

}

void PathfindingVisualisation::Update()
{
    //Edit Map - updates current pathfinding route, in case user makes obstacles on route
    if (editMap && toggle)
    {
        toggle = 0;
        if (startSet)
        {
            startSet = 0;
            pathfindingAgent_CPU->ResetRoute();
            pathfindingAgent_CPU->EditNode(startPosition, Obstacle);
            PathfindUpdate(Vector2<int>(pathfindingAgent_CPU->root->xPosition, pathfindingAgent_CPU->root->yPosition), Vector2<int>(pathfindingAgent_CPU->target->xPosition, pathfindingAgent_CPU->target->yPosition));
        }
        if (endSet)
        {
            endSet = 0;
            pathfindingAgent_CPU->ResetRoute();
            pathfindingAgent_CPU->EditNode(endPosition, Free);
            PathfindUpdate(Vector2<int>(pathfindingAgent_CPU->root->xPosition, pathfindingAgent_CPU->root->yPosition), Vector2<int>(pathfindingAgent_CPU->target->xPosition, pathfindingAgent_CPU->target->yPosition));

        }
        return;
    }
    //New Pathfind Target
    if (startSet && endSet && toggle)
    {
        toggle = 0;
        endSet = 0;
        PathfindUpdate(startPosition, endPosition);
    }
}

void PathfindingVisualisation::HandleInput()
{
    addViewCenter = sf::Vector2f(0, 0);
    input->update();
#pragma region BENCHMARKING_CONTROLS
    if (input->isPressed(sf::Keyboard::LControl))
    {
        std::cout << "The distance currently is " << Node::GetDistance(*pathfindingAgent_CPU->root, *pathfindingAgent_CPU->target) << std::endl;
    }
    if (input->isPressed(sf::Keyboard::Escape))
    {
        ExportBenchmark::ExportBenchmarkingInfo();
    }
    if (input->isPressed(sf::Keyboard::Space))
    {
        ExportBenchmark::current->clear();
    }
    if (input->isPressed(sf::Keyboard::Num1))
    {
        ExportBenchmark::Switch(1);
        pathfindingAgent_CPU->threadsToUse = 1;
    }
    if (input->isPressed(sf::Keyboard::Num2))
    {
        ExportBenchmark::Switch(2);
        pathfindingAgent_CPU->threadsToUse = 2;
    }
    if (input->isPressed(sf::Keyboard::Num3))
    {
        ExportBenchmark::Switch(4);
        pathfindingAgent_CPU->threadsToUse = 4;
    }
    if (input->isPressed(sf::Keyboard::Num4))
    {
        ExportBenchmark::Switch(8);
        pathfindingAgent_CPU->threadsToUse = 8;
    }
    if (input->isPressed(sf::Keyboard::Num5))
    {
        ExportBenchmark::Switch(16);
        pathfindingAgent_CPU->threadsToUse = 16;
    }
    if (input->isPressed(sf::Keyboard::Num0))
    {
        automatic = !automatic;
        std::cout << "Auto: " << automatic << std::endl;
    }
#pragma endregion
    //Map Navigation - by use of arrow keys
    #pragma region  MAP_NAVIGATION
    if (input->isPressed(sf::Keyboard::Right))
    {
        if (position.x != pmm.xDimension - 1)
        {
            addViewCenter += sf::Vector2f(1000, 0);
            position.x = position.x + 1;
            UpdateObjects(position, &pmm, &rectDraw);
        }

    }
    if (input->isPressed(sf::Keyboard::Left))
    {
        if (position.x != 0)
        {
            addViewCenter += sf::Vector2f(-1000, 0);
            position.x = position.x - 1;
            UpdateObjects(position, &pmm, &rectDraw);
        }

    }
    if (input->isPressed(sf::Keyboard::Down))
    {
        if (position.y != pmm.yDimension - 1)
        {
            addViewCenter += sf::Vector2f(0, 1000);
            position.y = position.y + 1;
            UpdateObjects(position, &pmm, &rectDraw);
        }

    }
    if (input->isPressed(sf::Keyboard::Up))
    {
        if (position.y != 0)
        {
            addViewCenter += sf::Vector2f(0, -1000);
            position.y = position.y - 1;
            UpdateObjects(position, &pmm, &rectDraw);
        }

    }
    viewCenter += addViewCenter;
    #pragma endregion

    //Map Editing/Path Editing - by use of left shift and mouse clicks
    #pragma region MAP_EDITING/PATH_EDITING
    if (input->isLeftMouseDown())
    {
        int roomOffsetX = window->getSize().x * position.x;
        int roomOffsetY = window->getSize().y * position.y;
        int wx = window->getSize().x / 100;
        int wy = window->getSize().y / 100;
        int xMousePos = floor((input->getMouseX() + roomOffsetX) / wx);
        int yMousePos = floor((input->getMouseY() + roomOffsetY)/ wy);

        startPosition = Vector2<int>(xMousePos, yMousePos);
        startSet = 1; 
        toggle = 1;
    }    
    if (input->isRightMouseDown() || automatic)
    {
        int roomOffsetX = window->getSize().x * position.x;
        int roomOffsetY = window->getSize().y * position.y;
        int wx = window->getSize().x / 100;
        int wy = window->getSize().y / 100;
        int xMousePos = floor((input->getMouseX() + roomOffsetX) / wx);
        int yMousePos = floor((input->getMouseY() + roomOffsetY) / wy);

        endPosition = Vector2<int>(xMousePos, yMousePos);
        endSet = 1;
        toggle = 1;
    }
    if (input->isPressed(sf::Keyboard::LShift))
    {
        editMap = !editMap;
    }
#pragma endregion


}

void PathfindingVisualisation::Render()
{
    for (int i = 0; i < rectDraw.size(); i++)
    {
        window->draw(rectDraw[i]);
    }
    view->setCenter(viewCenter);
}

//Simply updates the map and the objects colors
void PathfindingVisualisation::UpdateObjects(Vector2<int> mapArray, ProceduralMapManager* pmm, std::vector<sf::RectangleShape>* drawableShapes) {
    int i = 0;
    Room* roomToRender = &pmm->roomsInMap[mapArray.x][mapArray.y];
    sf::Vector2f positionStart = sf::Vector2f(roomToRender->GetLowestXCoord(), roomToRender->GetLowestYCoord());

    for (int x = 0; x < roomToRender->GetXSize(); x++)
    {
        for (int y = 0; y < roomToRender->GetYSize(); y++)
        {
            if (roomToRender->nodes[x][y].nodeType == Free)
            {
                drawableShapes->at(i).setFillColor(sf::Color::Blue);
            }
            else if (roomToRender->nodes[x][y].nodeType == Obstacle) {
                drawableShapes->at(i).setFillColor(sf::Color::Red);
            }
            else if(roomToRender->nodes[x][y].nodeType == Path) {
                drawableShapes->at(i).setFillColor(sf::Color::Yellow);
            }           
            else {
                drawableShapes->at(i).setFillColor(sf::Color::White);
            }
            drawableShapes->at(i).setPosition((positionStart.x * 10) + (x * 10), (10 * positionStart.y) + (y * 10));
            i++;

        }
    }
}

void PathfindingVisualisation::PathfindUpdate(Vector2<int> start, Vector2<int> end)
{
    pathfindingAgent_CPU->FindPath(start, end);
    pathfindingAgent_CPU->PrintRoute();
    UpdateObjects(position, &pmm, &rectDraw);
}
