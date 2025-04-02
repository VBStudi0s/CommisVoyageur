// CommisVoyageur.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <random>

#define DEBUG

#include "CommisVoyageur.h"
#include "PriorityQueue.h"

#include "Old.h"

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

using solution = const std::pair<int, std::list<int>>;

void runTests()
{
    for (int i = 0; i < 100; i++)
    {
        auto test = generateTest(6);
        CommisVoyageur cv(test);
        Old o(test);
        o.printMatrix();
        auto oo = o.preciseSolve();
        cv.printMatrix();
        auto nn = cv.preciseSolve();
        if (oo.first !=nn.first)
        {
            std::cout << "Pizda\n";
            std::cout << "New: " << nn.first << " , Old: " << oo.first << '\n';
            std::cout << "Maxrix is:\n";
            for (auto& v : test)
            {
                for (auto k : v)
                    std::cout << k << ' ';
                std::cout<<'\n';
            }
            std::cout << "\n Old: ";
            for (auto v : oo.second)
                std::cout << v << " -> ";
            std::cout << '\n';
            std::cout << "\n New: ";
            for (auto v : nn.second)
                std::cout << v << " -> ";
            std::cout << '\n';
        }
        else
            std::cout << "Passed\n";
    }
}

void runApproxTests(int times)
{
    double total = 0;
    for (int i = 0; i < times; i++)
    {
        CommisVoyageur cv(generateTest(16));
        auto accur = cv.preciseSolve();
        auto approx = cv.approxSolve();
        double diff = 100 * (static_cast<double>(approx.first - accur.first) / static_cast<int>(accur.first));
        std::cout << accur.first << " vs " << approx.first << " - Approx is worse by " << diff << "%\n";
        total += diff;
    }
    total /= times;
    std::cout << "V srednem: " << total << "%\n";
}



int main()
{
    srand(time(NULL));

    CommisVoyageur cv("../test10.txt");
    //CommisVoyageur cv(generateTest(5));
    //CommisVoyageur cv({ {-1, 1, 1}, {1, -1, 1}, {1, 1, -1} });
    cv.printMatrix();
    auto ans = cv.preciseSolve();
    std::cout << "Best = " << ans.first << '\n';
    for (const auto& vertex : ans.second)
        std::cout << vertex << ' ';

    //runTests();
    ////runApproxTests(1);


    //std::vector<std::pair<int, int>> v = { {1, 1}, {2, 2}, {8, 8}, {5, 5} };
    //PriorityQueue<int, int> q(v);
    //q.print();
    //std::cout << q.top().first<<' '<<q.top().second << '\n';
    //q.pop();
    //q.print();
    //std::cout << q.top().first << ' ' << q.top().second << '\n';
    //q.changeKey(5, 1);
    //q.print();
    //std::cout << q.top().first << ' ' << q.top().second << '\n';
    //q.insert(999, 999);
    //q.print();
    //std::cout << q.top().first << ' ' << q.top().second << '\n';
    //q.changeKey(999, 1);
    //q.print();
    //std::cout << q.top().first << ' ' << q.top().second << '\n';
}
