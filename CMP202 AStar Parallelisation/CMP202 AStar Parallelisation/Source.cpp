
#include "CustomLoader.h"
#include "ProceduralMapManager.h"
#include "Input.h"
#include <chrono>
#include "SFML\Graphics.hpp"

typedef std::chrono::steady_clock the_clock;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::nanoseconds;
using std::chrono::microseconds;

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

void UpdateObjects(Vector2<int> mapArray, ProceduralMapManager* pmm, std::vector<sf::RectangleShape>* drawableShapes) {
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
            else {
                drawableShapes->at(i).setFillColor(sf::Color::Red);
            }
            drawableShapes->at(i).setPosition((positionStart.x * 10) + (x * 10), (10 * positionStart.y) + (y * 10));
            i++;

        }
    }
}
//TODO: There is currently no memory management, assume only one run is allowed
int main() {
    srand(time(0));
    sf::View view;
    sf::RenderWindow window(sf::VideoMode(1600, 920), "A Star Visualisation");
    Input* input;
    input = new Input();

#pragma region TESTINGCA
    //CellularAutomata ca;
    //ca.GenerateMap();

    //size 10 = position * 10
    // size 1 = position * 1
    //std::vector<sf::RectangleShape> rectCA;
    //for (int y = 0; y < ca.height; y++)
    //{
    //    for (int x = 0; x < ca.width; x++)
    //    {
    //        if (ca.map[x][y] == 1)
    //        {
    //            rectCA.push_back(sf::RectangleShape());
    //            rectCA.back().setSize(sf::Vector2f(10,10));
    //            rectCA.back().setPosition(x * 10, y * 10);
    //            //rectCA.back().setOutlineColor(sf::Color::Black);
    //            //rectCA.back().setOutlineThickness(1);
    //            rectCA.back().setFillColor(sf::Color::Red);

    //        }else{
    //            rectCA.push_back(sf::RectangleShape());
    //            rectCA.back().setSize(sf::Vector2f(10, 10));
    //            rectCA.back().setPosition(x * 10, y * 10);
    //           // rectCA.back().setOutlineColor(sf::Color::Black);
    //            //rectCA.back().setOutlineThickness(1);
    //            rectCA.back().setFillColor(sf::Color::Blue);

    //        }
    //    }
    //}
#pragma endregion

#pragma region TESTINGPMM
    ProceduralMapManager pmm;
    pmm.Init(2, 2, 100, 100);
    pmm.GenerateMapGrid();

    std::vector<sf::RectangleShape> rectDraw;
    for (int i = 0; i < 10000; i++)
    {
            rectDraw.push_back(sf::RectangleShape());
            rectDraw.back().setSize(sf::Vector2f(10,10));
            rectDraw.back().setOutlineColor(sf::Color::Black);
            rectDraw.back().setOutlineThickness(1);
            rectDraw.back().setFillColor(sf::Color::Red);
            
            //rectFree.push_back(sf::RectangleShape());
            //rectFree.back().setSize(sf::Vector2f(10,10));
            //rectFree.back().setOutlineColor(sf::Color::Black);
            //rectFree.back().setOutlineThickness(1);
            //rectFree.back().setFillColor(sf::Color::Blue);
    }
    UpdateObjects(Vector2<int>(0, 0), &pmm, &rectDraw);
    Vector2<int> position = Vector2<int>(0, 0);
    sf::Vector2f viewCenter = view.getCenter();
    sf::Vector2f addViewCenter;
#pragma endregion

    while (window.isOpen())
    {
        sfmlEvents(&window, input);
        window.setView(view);

        addViewCenter = sf::Vector2f(0, 0);
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
        window.clear(sf::Color(0, 0, 0));
        //Draw things here:
        for (int i = 0; i < rectDraw.size(); i++)
        {
            window.draw(rectDraw[i]);
        }
        window.display();
        view.setCenter(viewCenter);

    }
    return 0;
//    A_Star_Pathfinding_Defined Def;
//    A_Star_Pathfinding_Undefined Pro;
//    A_Star_Pathfinding_Defined_Segmented Seg;
//    Base_A_Star_Pathfinding* currentAStar = &Seg;
//
//    std::vector<float> ntimes;
//    std::vector<float> utimes;
//    std::vector<float> mtimes;
//    int choice = 1;
//    std::ofstream myFile;
//    myFile.open("Readings.txt");
//    //std::ofstream myFile1;
//    //myFile1.open("Readings1.txt");    
//    //std::ofstream myFile2;
//    //myFile2.open("Readings2.txt");
//#pragma region ROOMS
//    Room nm;
//    Room nm1;
//    Room nm2;
//    Room nm3;
//    Room nm4; Room nm5;Room nm6; Room nm7;
//    Room nmD;
//    Room nmD1;
//    Room nmD2;
//    Room nmD3;
//    Room nmD4; Room nmD5; Room nmD6; Room nmD7;
//    Room* cm;
//    Room* cm1;
//    Room* cm2;
//    Room* cm3;
//    Room* cm4;    Room* cm5;    Room* cm6;Room* cm7;
//    std::cout << "What to load? 1 for High Resolution, 2 for low resolution" << std::endl;
//    std::cin >> choice;
//    if (choice == 1)
//    {
//        cm = &nm;
//        cm1 = &nm1;
//        cm2 = &nm2;
//        cm3 = &nm3;
//        cm4 = &nm4;
//        cm5 = &nm5;
//        cm6 = &nm6;
//        cm7 = &nm7;
//
//
//    CompleteMapRead(nm, "Final Testing Map ClosedRightOpen.txt");
//    nm.LinkNeighbours(nm);
//    //PrintMap(nm.GetXSize(), nm.GetYSize(), nm.nodes);
//
//    CompleteMapRead(nm1, "Final Testing Map ClosedLeftTopBottomOpen.txt");
//    nm1.LinkNeighbours(nm1);
//    //DisplaceNodeMap(nm1, Vector2<int>(1, 0));
//    //PrintMap(nm1.GetXSize(), nm1.GetYSize(), nm1.nodes);
//
//    CompleteMapRead(nm2, "Final Testing Map ClosedLeftTopOpen.txt");
//    nm2.LinkNeighbours(nm2);
//   // DisplaceNodeMap(nm2, Vector2<int>(1, 1));
//    //PrintMap(nm2.GetXSize(), nm2.GetYSize(), nm2.nodes);
//
//
//    CompleteMapRead(nm3, "Final Testing Map ClosedRightOpen.txt");
//    nm3.LinkNeighbours(nm3);
//    //DisplaceNodeMap(nm3, Vector2<int>(0, 1));
//    //PrintMap(nm3.GetXSize(), nm3.GetYSize(), nm3.nodes);
//
//    CompleteMapRead(nm4, "Final Testing Map ClosedOpen.txt");
//    nm4.LinkNeighbours(nm4);
//    //DisplaceNodeMap(nm4, Vector2<int>(0, 1));
//    //PrintMap(nm3.GetXSize(), nm3.GetYSize(), nm3.nodes);    
//    CompleteMapRead(nm5, "Final Testing Map ClosedOpen.txt");
//    nm5.LinkNeighbours(nm5);
//    //DisplaceNodeMap(nm4, Vector2<int>(0, 1));
//    //PrintMap(nm3.GetXSize(), nm3.GetYSize(), nm3.nodes);    
//    CompleteMapRead(nm6, "Final Testing Map ClosedOpen.txt");
//    nm6.LinkNeighbours(nm6);
//    //DisplaceNodeMap(nm4, Vector2<int>(0, 1));
//    //PrintMap(nm3.GetXSize(), nm3.GetYSize(), nm3.nodes);    
//    CompleteMapRead(nm7, "Final Testing Map ClosedOpen.txt");
//    nm7.LinkNeighbours(nm7);
//    //DisplaceNodeMap(nm4, Vector2<int>(0, 1));
//    //PrintMap(nm3.GetXSize(), nm3.GetYSize(), nm3.nodes);
//    }
//    else {
//
//        cm = &nmD;
//        cm1 = &nmD1;
//        cm2 = &nmD2;
//        cm3 = &nmD3;
//        cm4 = &nmD4;
//        cm5 = &nmD5;
//        cm6 = &nmD6;
//        cm7 = &nmD7;
//    //Deubugging
//    CompleteMapRead(nmD, "SmallFinal Testing MapClosedRightOpen.txt");
//    nmD.LinkNeighbours(nmD);
//    //PrintMap(nmD.GetXSize(), nmD.GetYSize(), nmD.nodes);
//
//    CompleteMapRead(nmD1, "SmallFinal Testing MapClosedLeftBottomTop.txt");
//    nmD1.LinkNeighbours(nmD1);
//    //DisplaceNodeMap(nmD1, Vector2<int>(1, 0));
//    //PrintMap(nmD1.GetXSize(), nmD1.GetYSize(), nmD1.nodes);
//
//    CompleteMapRead(nmD2, "SmallFinal Testing MapClosedLeftBottomTop.txt");
//    nmD2.LinkNeighbours(nmD2);
//    //DisplaceNodeMap(nmD2, Vector2<int>(1, 1));
//    //PrintMap(nmD2.GetXSize(), nmD2.GetYSize(), nmD2.nodes);
//
//
//    CompleteMapRead(nmD3, "SmallFinal Testing MapClosedRightOpen.txt");
//    nmD3.LinkNeighbours(nmD3);
//    //DisplaceNodeMap(nmD3, Vector2<int>(0, 1));
//    //PrintMap(nm3.GetXSize(), nm3.GetYSize(), nm3.nodes);
//
//    CompleteMapRead(nmD4, "SmallFinal Testing MapClosedOpen.txt");
//    nmD4.LinkNeighbours(nmD4);
//    //DisplaceNodeMap(nmD4, Vector2<int>(0, 1));
//    //PrintMap(nm3.GetXSize(), nm3.GetYSize(), nm3.nodes);    
//    CompleteMapRead(nmD5, "SmallFinal Testing MapClosedOpen.txt");
//    nmD5.LinkNeighbours(nmD5);
//    //DisplaceNodeMap(nmD4, Vector2<int>(0, 1));
//    //PrintMap(nm3.GetXSize(), nm3.GetYSize(), nm3.nodes);   
//    CompleteMapRead(nmD6, "SmallFinal Testing MapClosedOpen.txt");
//    nmD6.LinkNeighbours(nmD6);
//    //DisplaceNodeMap(nmD4, Vector2<int>(0, 1));
//    //PrintMap(nm3.GetXSize(), nm3.GetYSize(), nm3.nodes);    
//    CompleteMapRead(nmD7, "SmallFinal Testing MapClosedOpen.txt");
//    nmD7.LinkNeighbours(nmD7);
//    //DisplaceNodeMap(nmD4, Vector2<int>(0, 1));
//    //PrintMap(nm3.GetXSize(), nm3.GetYSize(), nm3.nodes);
//    }
//
//#pragma endregion
//
//#pragma region MANUAL_NEIGHBOUR_LINKING
//
//    std::cout << "What Scenario should we simulate?\n 1 for linear 2x2. \n 2 for linear 1x4 \n 3 for complex 2x2" << std::endl;
//    std::cin >> choice;
//        //All manual for now, if i had time, i'd prefer to make a file system or at least some functions that fully automate this process
//
//    if (choice == 1)
//    {
//        DisplaceNodeMap(*cm1, Vector2<int>(1, 0));
//        DisplaceNodeMap(*cm2, Vector2<int>(1, 1));
//        DisplaceNodeMap(*cm3, Vector2<int>(0, 1));
//        DisplaceNodeMap(*cm4, Vector2<int>(5, 0));
//
//        cm->AddNeighbouringRoom(cm1);
//
//        cm1->AddNeighbouringRoom(cm2);
//        cm1->AddNeighbouringRoom(cm);
//
//        cm2->AddNeighbouringRoom(cm1);
//        cm2->AddNeighbouringRoom(cm3);
//
//        cm3->AddNeighbouringRoom(cm2);
//
//        cm->AutoDualLinkRouteNodes();
//        cm1->AutoDualLinkRouteNodes();
//        cm2->AutoDualLinkRouteNodes();
//        cm3->AutoDualLinkRouteNodes();
//    }
//    else if (choice == 2)
//    {
//        cm1 = cm4;
//        cm2 = cm5;
//        cm3 = cm6;
//
//        DisplaceNodeMap(*cm1, Vector2<int>(1, 0));
//        DisplaceNodeMap(*cm2, Vector2<int>(2, 0));
//        DisplaceNodeMap(*cm3, Vector2<int>(3, 0));
//
//
//        cm->AddNeighbouringRoom(cm1);
//
//        cm1->AddNeighbouringRoom(cm2);
//        cm1->AddNeighbouringRoom(cm);
//
//        cm2->AddNeighbouringRoom(cm1);
//        cm2->AddNeighbouringRoom(cm3);
//
//        cm3->AddNeighbouringRoom(cm2);
//
//        cm->AutoDualLinkRouteNodes();
//        cm1->AutoDualLinkRouteNodes();
//        cm2->AutoDualLinkRouteNodes();
//        cm3->AutoDualLinkRouteNodes();
//    }
//    else {
//        cm = cm7;
//        cm1 = cm4;
//        cm2 = cm5;
//        cm3 = cm6;
//
//        DisplaceNodeMap(*cm1, Vector2<int>(1, 0));
//        DisplaceNodeMap(*cm2, Vector2<int>(1, 1));
//        DisplaceNodeMap(*cm3, Vector2<int>(0, 1));
//
//        cm->AddNeighbouringRoom(cm1);
//        cm->AddNeighbouringRoom(cm3);
//
//        cm1->AddNeighbouringRoom(cm2);
//        cm1->AddNeighbouringRoom(cm);
//
//        cm2->AddNeighbouringRoom(cm1);
//        cm2->AddNeighbouringRoom(cm3);
//
//        cm3->AddNeighbouringRoom(cm2);
//        cm3->AddNeighbouringRoom(cm);
//
//        cm->AutoDualLinkRouteNodes();
//        cm1->AutoDualLinkRouteNodes();
//        cm2->AutoDualLinkRouteNodes();
//        cm3->AutoDualLinkRouteNodes();
//    }
//    //cm->AddNeighbouringRoom(cm1);
//
//    //cm1->AddNeighbouringRoom(cm2);
//    //cm1->AddNeighbouringRoom(cm);
//
//    //cm2->AddNeighbouringRoom(cm1);
//    //cm2->AddNeighbouringRoom(cm3);
//
//
//    //cm3->AddNeighbouringRoom(cm2);
//
//    //cm->AutoDualLinkRouteNodes();
//    //cm1->AutoDualLinkRouteNodes();
//    //cm2->AutoDualLinkRouteNodes();
//    //cm3->AutoDualLinkRouteNodes();
//
//    //Node* a = nm.GetRouteNodes()[0]; Node* b = nm1.GetRouteNodes()[0];
//    //nm.DualLinkRouteNodes(*a, *b, EAST);
//    //a = nm.GetRouteNodes()[1]; b = nm1.GetRouteNodes()[1];
//    //nm.DualLinkRouteNodes(*a, *b, EAST);
//
//    //a = nm1.GetRouteNodes()[2]; b = nm2.GetRouteNodes()[3];
//    //nm.DualLinkRouteNodes(*a, *b, SOUTH);
//
//    //a = nm2.GetRouteNodes()[0]; b = nm3.GetRouteNodes()[2];
//    //nm.DualLinkRouteNodes(*a, *b, WEST);
//
//    Seg.rooms.push_back(cm); Seg.rooms.push_back(cm1); Seg.rooms.push_back(cm2); Seg.rooms.push_back(cm3);
//
//    Seg.SetCurrentRoom(cm);
//    Def.rooms.push_back(cm); Def.rooms.push_back(cm1); Def.rooms.push_back(cm2); Def.rooms.push_back(cm3);
//
//    Def.SetCurrentRoom(cm);
//    Pro.rooms.push_back(cm); Pro.rooms.push_back(cm1); Pro.rooms.push_back(cm2); Pro.rooms.push_back(cm3);
//
//    Pro.SetCurrentRoom(cm);
//#pragma endregion
//
//    while (choice != 0)
//    {
//
//        Vector2<int> target;
//        std::cout << "1 for Segmented, 2 for Default, 3 for Procedural" << std::endl;
//        std::cin >> choice;
//
//        switch (choice)
//        {
//        case 1:
//            currentAStar = &Seg;
//            break;
//        case 2:
//            currentAStar = &Def;
//            break;
//        case 3:
//            currentAStar = &Pro;
//            break;
//        default:
//            choice = 0;
//            break;
//        }
//        if (choice == 0)
//        {
//            break;
//        }
//        std::cout << "Please choose a target: \n";
//        std::cout << "1 for One Room path, \n 2 for Two Rooms path, \n 3 for 4 rooms horizontal path \n 4 for 4 rooms path " << std::endl;
//        std::cin >> choice;
//
//        switch (choice)
//        {
//        case 1:
//            target = Vector2<int>(600,900);// Works for all shorter  path one room
//            break;
//        case 2:
//            target = Vector2<int>(3100, 1550);//works for all two rooms
//            break;
//        case 3:
//            target = Vector2<int>(7000, 300);
//            break;
//        case 4:
//            target = Vector2<int>(1100, 3500);//Seg works, works for def, does not work for undef, 4 rooms
//            break;
//        default:
//            choice = 0;
//            break;
//        }
//        if (choice == 0)
//        {
//            break;
//        }
//
//
//            myFile << "\n=(";
//        //myFile2 << "\n=(";
//
//       for (int i = 0; i < 3; i++)
//        {
//
//         for (int i = 0; i < 10; i++)
//         {
//            ResetRoomCalculations(cm);
//            ResetRoomCalculations(cm1);
//            ResetRoomCalculations(cm2);
//            ResetRoomCalculations(cm3);
//            // Start timing
//            the_clock::time_point start = the_clock::now();
//            //path.SetCurrentRoom(&nm);
//            //currentAStar->FindPath(Vector2<int>(200, 100), Vector2<int>(600, 900));// Works for all shorter  path one room
//           // currentAStar->FindPath(Vector2<int>(200, 100), Vector2<int>(1600, 1500)); //works for all longer path one room
//            //currentAStar->FindPath(Vector2<int>(200, 100), Vector2<int>(1050, 1500)); //works for all longer path one room
//            currentAStar->FindPath(Vector2<int>(200, 200), target); //works for all two rooms
//            //currentAStar->FindPath(Vector2<int>(200, 100), Vector2<int>(1100, 3500)); //Seg works, works for def, does not work for undef
//                                                                                      
//             //How long did it take
//            the_clock::time_point end = the_clock::now();
//            auto nanoSec = duration_cast<nanoseconds>(end - start).count();
//            auto microSec= duration_cast<microseconds>(end - start).count();
//            auto miliSec = duration_cast<milliseconds>(end - start).count();
//            std::cout << "It took " << nanoSec << " ns.\n";
//            std::cout << "It took " << microSec << " us.\n";
//            std::cout << "It took " << miliSec << " ms.\n";
//            //cout << total.front();
//            ntimes.push_back(nanoSec);
//            utimes.push_back(microSec);
//            mtimes.push_back(miliSec);
//            //myFile << nanoSec << std::endl;
//        }
//
//        float med = 0;
//        std::sort(ntimes.begin(), ntimes.end());
//        med = ntimes.at(5);
//
//
//        myFile << med << "+";
//        std::cout << "Median n:" << med;
//
//        med = 0;
//        std::sort(utimes.begin(), utimes.end());
//        med = utimes.at(5);
//        //myFile1 << med << "+";
//        std::cout << "Median u:" << med;
//
//        med = 0;
//        std::sort(mtimes.begin(), mtimes.end());
//        med = mtimes.at(5);
//        //myFile2 << med << "+";
//        std::cout << "Median m:" << med;
//
//        ntimes.clear();
//        utimes.clear();
//        mtimes.clear();
//       }
//        myFile << "0)/3";
//       // myFile2 << "0)/3";
//
//        //Debugging
//        //std::cout << "\n Would you like to see the path? 1 for Yes, 2 for No" << std::endl;
//        //std::cin >> choice;
//        //if (choice = 1)
//        //{
//        //    //currentAStar->PrintPath();
//        //    currentAStar->PrintRoute();
//        //    //return 0;//Have to return if we print the path as it overwrites the path nodes to be route nodes just to see easily
//        //}
//
//        std::cout << "Exit program? 0 for Yes, 1 for no" << std::endl;
//        std::cin >> choice;
//    }
//    myFile.close();
   // myFile1.close();
   // myFile2.close();
    //path.PrintRoute();

    return 0;
}



/*
1.FindMapFor(StartNode)
2.while(!IsTargetInCurrentMap()){
3.ImplementRoomAStar()  
4.}
5.ImplementDefaultAStar()
6.return;}


3.1ImplementRoomAStar(){
3.2FindMapFor(EndNode)
3.3ImplementDefaultAStarOnRoomScale()
3.4 While temporaryTargets is not empty{
3.5 ImplementDefaultAStarToTemporaryTarget
3.6}//When we reach a route node we should calculate a new route using that route node and the next route node as the start and endpoints
3.7}

3.3.1ImplementDefaultAStarOnRoomScale(){
3.3.2//When path found
3.3.3FindRouteNodeConnections();
3.3.4}

3.3.3.1FindRoutNodeConnections(){
3.3.3.2std::Queue<Node*> temporaryTargets;//FirstInFirstOut
3.3.3.3for each routeNodes in MapRoute//starting from current
3.3.3.4    for each routeNode neighbours
3.3.3.5        if neighbour is in (MapRoute + 1) then
3.3.3.6            add routeNode to temporaryTargets
3.3.3.7            break;
}


What this means:
Each Node can remain the same
Each NodeMap will have to be reconfigured to act like nodes as well, but assune that they have infinite neighbours(door in the center teleports to under ground map for example)
//The need to have neighbours and parents//Maybe there's a nicer way we can do this?
//Or we could create temporary nodes that take place of the maps, but we just make sure the neihbouring of those are correct. 
//Then when the path is found we can use the nodes path and map that to the room pathing, with parenting or just a queue
//Map route doesn't need to be a parenting thing, we could jsut make it a queue/stack
A Star Default might have to be rebuilt to allow segment to use it?

As for searching the Maps, we might be better off bruteforcing it.

//Currently it's assumed that each node costs 1 to move, change in the future?

*/