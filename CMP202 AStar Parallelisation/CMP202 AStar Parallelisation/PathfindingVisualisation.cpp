#include "PathfindingVisualisation.h"
#include "Input.h"

void PathfindingVisualisation::Init(Input* in, sf::RenderWindow* wind,sf::View* vie)
{
    input = in;
    window = wind;
    view = vie;
    viewCenter = vie->getCenter();

    std::vector<std::thread> threads;
    std::mutex lock;
    int shapeNumber = 0; const int shapesNeeded = 10000;
    for (int threadIndex = 0; threadIndex < std::thread::hardware_concurrency()/2; threadIndex++)
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
    //for (int i = 0; i < 10000; i++)
    //{
    //    rectDraw.push_back(sf::RectangleShape());
    //    rectDraw.back().setSize(sf::Vector2f(10, 10));
    //    rectDraw.back().setOutlineColor(sf::Color::Black);
    //    rectDraw.back().setOutlineThickness(1);
    //    rectDraw.back().setFillColor(sf::Color::Red);
    //}

    pmm.Init(5, 5, 100, 100);
    pmm.GenerateMapGrid();
    pmm.ConnectRooms();


    pathfindingAgent_CPU = new A_Star_Pathfinding_Defined_SegmentedGPU();
    for (int y = 0; y < pmm.yDimension; y++)
    {
        for (int x = 0; x < pmm.xDimension; x++)
        {
            pathfindingAgent_CPU->rooms.push_back(&pmm.roomsInMap[x][y]);
        }
    }
    pathfindingAgent_CPU->FindPath(pmm.roomsInMap[0][0].nodes[pmm.xRoomDimension / 2][pmm.yRoomDimension / 2].position, pmm.roomsInMap[1][1].nodes[20][20].position);
    pathfindingAgent_CPU->PrintRoute();

    std::for_each(threads.begin(), threads.end(), [](std::thread& t)
        {
            t.join();
        });

    UpdateObjects(Vector2<int>(0, 0), &pmm, &rectDraw);

}

void PathfindingVisualisation::Update()
{
    addViewCenter = sf::Vector2f(0, 0);
    if (editMap && toggle)
    {
        toggle = 0;
        if (startSet)
        {
            startSet = 0;
            pathfindingAgent_CPU->ResetRoute();
            pathfindingAgent_CPU->EditNode(startPosition, Obstacle);
            PathfindUpdate(pathfindingAgent_CPU->root->position, pathfindingAgent_CPU->target->position);
        }
        if (endSet)
        {
            endSet = 0;
            pathfindingAgent_CPU->ResetRoute();
            pathfindingAgent_CPU->EditNode(endPosition, Free);
            PathfindUpdate(pathfindingAgent_CPU->root->position, pathfindingAgent_CPU->target->position);

        }
        return;
    }
    if (startSet && endSet && toggle)
    {
        toggle = 0;
        endSet = 0;
        PathfindUpdate(startPosition, endPosition);
        pathfindingAgent_CPU->man = true;
    }
}

void PathfindingVisualisation::HandleInput()
{
    input->update();

    if (input->isPressed(sf::Keyboard::Right))
    {
        if (position.x != pmm.xDimension - 1)
        {
            addViewCenter = sf::Vector2f(1000, 0);
            position.x = position.x + 1;
            UpdateObjects(position, &pmm, &rectDraw);
        }

    }
    if (input->isPressed(sf::Keyboard::Left))
    {
        if (position.x != 0)
        {
            addViewCenter = sf::Vector2f(-1000, 0);
            position.x = position.x - 1;
            UpdateObjects(position, &pmm, &rectDraw);
        }

    }
    if (input->isPressed(sf::Keyboard::Down))
    {
        if (position.y != pmm.yDimension - 1)
        {
            addViewCenter = sf::Vector2f(0, 1000);
            position.y = position.y + 1;
            UpdateObjects(position, &pmm, &rectDraw);
        }

    }
    if (input->isPressed(sf::Keyboard::Up))
    {
        if (position.y != 0)
        {
            addViewCenter = sf::Vector2f(0, -1000);
            position.y = position.y - 1;
            UpdateObjects(position, &pmm, &rectDraw);
        }

    }
    viewCenter += addViewCenter;

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
    if (input->isRightMouseDown())
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
}

void PathfindingVisualisation::Render()
{
    for (int i = 0; i < rectDraw.size(); i++)
    {
        window->draw(rectDraw[i]);
    }
    view->setCenter(viewCenter);
}
void PathfindingVisualisation::UpdateObjects(Vector2<int> mapArray, ProceduralMapManager* pmm, std::vector<sf::RectangleShape>* drawableShapes) {
    int i = 0;
    Room* roomToRender = &pmm->roomsInMap[mapArray.x][mapArray.y];

    sf::Vector2f positionStart = sf::Vector2f(roomToRender->GetLowestCoord().x, roomToRender->GetLowestCoord().y);
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
