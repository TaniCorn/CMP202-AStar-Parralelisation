
#include "ProceduralMapManager.h"
#include "Input.h"
#include "SFML\Graphics.hpp"
#include "PathfindingVisualisation.h"

void ResetRoomCalculations(Room* rm) {
    for (int x = 0; x < rm->GetXSize(); x++)
    {
        for (int y = 0; y < rm->GetYSize(); y++)
        {
            rm->nodes[x][y].SetFCost(NULL);
            rm->nodes[x][y].SetGCost(NULL);
            rm->nodes[x][y].SetHCost(NULL);
            rm->nodes[x][y].SetParent(nullptr);
        }
    }
}

void sfmlEvents(sf::RenderWindow* window, Input* in) {
	sf::Event event;
	while (window->pollEvent(event))
	{
		switch (event.type)
		{
		case sf::Event::Closed:
			window->close();
			break;
		case sf::Event::Resized:
			window->setView(sf::View(sf::FloatRect(0.f, 0.f, (float)event.size.width, (float)event.size.height)));
			break;
        case sf::Event::KeyPressed:
            // update input class
            in->setKeyDown(event.key.code);
            break;
        case sf::Event::KeyReleased:
            //update input class
            in->setKeyUp(event.key.code);
            break;
        case sf::Event::MouseMoved:
            //update input class
            in->setMousePosition(event.mouseMove.x, event.mouseMove.y);
            break;
        case sf::Event::MouseButtonPressed:
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                //update input class
                in->setLeftMouse(Input::MouseState::DOWN);
            }
            else if (event.mouseButton.button == sf::Mouse::Right)
            {
                in->setRightMouse(Input::MouseState::DOWN);
            }
            break;
        case sf::Event::MouseButtonReleased:
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                //update input class
                in->setLeftMouse(Input::MouseState::UP);
            }
            else if (event.mouseButton.button == sf::Mouse::Right)
            {
                in->setRightMouse(Input::MouseState::UP);
            }
		default:
			// don't handle other events
			break;
		}
	}
}



std::condition_variable cv;
std::mutex lock;
void RunUpdate(PathfindingVisualisation* pv, sf::RenderWindow* win) {
    while (win->isOpen())
    {
       // std::unique_lock<std::mutex> l(lock);
        pv->Update();
        pv->HandleInput();
        //cv.wait(l);
        cv.notify_all();
    }
}

int main() {
    srand(time(0));
    
    sf::View view;
    sf::RenderWindow window(sf::VideoMode(1000, 1000), "A Star Visualisation");
    Input* input;
    input = new Input();
    //Level for pathfinding, input, visuals
    PathfindingVisualisation pathfindingVisuals;
    pathfindingVisuals.Init(input, &window, &view);
    std::thread updateThread = std::thread(RunUpdate,&pathfindingVisuals, &window);

    while (window.isOpen())
    {
        sfmlEvents(&window, input);
        window.setView(view);
        std::unique_lock<std::mutex> l(lock);
        cv.wait(l);

        window.clear(sf::Color(0, 0, 0));
        pathfindingVisuals.Render();
        window.display();

    }
    updateThread.join();
    return 0;
}

