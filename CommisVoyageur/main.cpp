// CommisVoyageur.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <random>

//#define DEBUG

#include "CommisVoyageur.h"


int main()
{
    srand(time(NULL));

    CommisVoyageur cv("../test10.txt");
    //CommisVoyageur cv({ {-1, 1, 1}, {1, -1, 1}, {1, 1, -1} });
    cv.printMatrix();
    auto ans = cv.preciseSolve();
    std::cout << "Best = " << ans.first << '\n';
    for (const auto& vertex : ans.second)
        std::cout << vertex << ' ';
}
