// CommisVoyageur.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <random>

//#define DEBUG

#include "CommisVoyageur.h"

using graph = std::vector<std::vector<int>>;

std::vector<std::vector<int>> generateTest(int n)
{
    std::vector<std::vector<int>> matrix(n);
    for (int i = 0; i < n; i++)
    {
        matrix[i].resize(n);
        for (int j = 0; j < n; j++)
        {
            if (i == j)
                matrix[i][j] = -1;
            else
                matrix[i][j] = rand() % 10 + 1;
        }
    }
    return matrix;
}

int main()
{
    srand(time(NULL));
    CommisVoyageur cv("../test5.txt");
    //CommisVoyageur cv({ {-1, 1, 1}, {1, -1, 1}, {1, 1, -1} });
    cv.printMatrix();
    auto ans = cv.preciseSolve();
    std::cout << "Best = " << ans.first << '\n';
    for (const auto& vertex : ans.second)
        std::cout << vertex << ' ';
}
