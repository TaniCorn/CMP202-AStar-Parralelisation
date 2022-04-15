#include "PathfindingVisualisation.h"
#include "Input.h"

void PathfindingVisualisation::Init(Input* in, sf::RenderWindow* wind,sf::View* vie)
{
    input = in;
    window = wind;
    view = vie;
    viewCenter = vie->getCenter();


    pmm.Init(5, 5, 100, 100);
    pmm.GenerateMapGrid();
    pmm.ConnectRooms();

    for (int i = 0; i < 10000; i++)
    {
        rectDraw.push_back(sf::RectangleShape());
        rectDraw.back().setSize(sf::Vector2f(10, 10));
        rectDraw.back().setOutlineColor(sf::Color::Black);
        rectDraw.back().setOutlineThickness(1);
        rectDraw.back().setFillColor(sf::Color::Red);

        //rectFree.push_back(sf::RectangleShape());
        //rectFree.back().setSize(sf::Vector2f(10,10));
        //rectFree.back().setOutlineColor(sf::Color::Black);
        //rectFree.back().setOutlineThickness(1);
        //rectFree.back().setFillColor(sf::Color::Blue);
    }

    pathfindingAgent = new A_Star_Pathfinding_Defined_Segmented();
    pathfindingAgent->SetCurrentRoom(&pmm.roomsInMap[0][0]);
    for (int y = 0; y < pmm.yDimension; y++)
    {
        for (int x = 0; x < pmm.xDimension; x++)
        {
            pathfindingAgent->rooms.push_back(&pmm.roomsInMap[x][y]);
        }
    }
    pathfindingAgent->FindPath(pmm.roomsInMap[0][0].nodes[pmm.xRoomDimension / 2][pmm.yRoomDimension / 2].position, pmm.roomsInMap[2][1].nodes[pmm.xRoomDimension / 2][pmm.yRoomDimension / 2].position);
    pathfindingAgent->PrintRoute();

    UpdateObjects(Vector2<int>(0, 0), &pmm, &rectDraw);

}

void PathfindingVisualisation::Update()
{
    addViewCenter = sf::Vector2f(0, 0);

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
            else if(roomToRender->nodes[x][y].nodeType == Obstacle) {
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