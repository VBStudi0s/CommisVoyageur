#pragma once

#include <iostream>
#include <vector>
#include <list>
#include <utility>
#include <string>
#include <fstream>
#include <limits>
#include <stack>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <map>
#include <algorithm>

constexpr int MAX_INT2 = std::numeric_limits<int>::max();

/*
* Class for solving The Travelling Salesman Problem.
* Implements two possible solutions:
* - precise one using Branch and Bound method;
* - approximate one using solution modification method.
*/
class Old
{
	// Matrix representing problem weigthed graph
	std::vector<std::vector<int>> mMatrix;

	// Cities of the problem (aka N in mMatrix = N x N)
	int mCitiesNum = 0;

	int32_t mBestPathCost = INT32_MAX;		// Best found path length (by sum of weights) at the moment
	std::list<int> mBestPath;				// Best found path at the moment
	std::list<int> mCurPath = { 0 };		// List storing current partial path
	int mCurPathCost = 0;					// Length (by sum of weights) of current partial path

	int countBnB = 0;						// Iterations counter


	/*
	* Method getting lower bound of remaining path with added vertex
	* cost by the lightest edges.
	* Receives set of vertices that are not currently in the path,
	* and vertex to be added to the path.
	* Returns int as lower bound.
	*/
	int lowerBoundLightestEdges(std::unordered_set<int> unvisitedVertices, int newVertex);

	/*
	* Method getting lower bound of remaining path with added vertex
	* cost by its MST (mini,al spanning tree).
	* Implements Prim's algorithm based on std::priority_queue.
	* Returns int as lower bound.
	*/
	int findPrimMST(std::unordered_set<int> graph, int newVertex) const;

	/*
	* Recursive method implementing Branch and Bound problem solution.
	* Uses two types of lower bounds and euristics to choose branches.
	* Overrites mBestPath and mBestPathCost fields.
	*/
	void BnB(std::unordered_set<int> unvisitedVertices);

	/*
	* Debug method for showing input graph matrix.
	*/
	void printPath(const std::list<int>& path) const;

public:
	Old(const std::vector<std::vector<int>>& matrix);
	Old(std::vector<std::vector<int>>&& matrix);
	Old(std::string filePath);

	// Prints read graph matrix
	void printMatrix();

	/*
	* Precise BnB method caller.
	* Uses mMatrix written information.
	* Returns pair <best path cost, best path>.
	*/
	std::pair<int, std::list<int>> preciseSolve();

	/*
	* Method implementing solution modification algorithm.
	* Based on swapping the cities insive the chain.
	* Gives not precise answer.
	*/
	std::pair<int, std::list<int>> approxSolve();
};


Old::Old(const std::vector<std::vector<int>>& matrix) :
	mMatrix(matrix)
{
	mCitiesNum = mMatrix.size();
}

Old::Old(std::vector<std::vector<int>>&& matrix) :
	mMatrix(matrix)
{
	mCitiesNum = mMatrix.size();
}

inline Old::Old(std::string filePath)
{
	std::ifstream file(filePath);
	if (file.fail())
	{
		std::cout << "File opening error\n";
		std::terminate();
	}
	file >> mCitiesNum;
	mMatrix.resize(mCitiesNum);
	for (size_t i = 0; i < mCitiesNum; i++)
	{
		mMatrix[i].resize(mCitiesNum);
		for (size_t j = 0; j < mCitiesNum; j++)
		{
			file >> mMatrix[i][j];
		}
	}
}

inline void Old::printMatrix()
{
	for (const auto& str : mMatrix)
	{
		for (const auto& val : str)
			std::cout << val << ' ';
		std::cout << '\n';
	}
}

inline int Old::lowerBoundLightestEdges(std::unordered_set<int> unvisitedVertices, int newVertex)
{
	// If all vertices are visited, return 0 (because branch cost is calculated later)
	if (mCurPath.size() + 1 == mCitiesNum)
		return 0;

#ifdef DEBUG
	std::cout << "\nGetting lower bound by the lightest edges for path: ";
	printPath(mCurPath); std::cout << "-> " << newVertex << '\n';
#endif // DEBUG

	// Preparations for algorithm
	std::list<int> newPath = mCurPath;			// Creates new path
	newPath.push_back(newVertex);
	int newPathStart = newPath.front();
	int newPathEnd = newPath.back();
	unvisitedVertices.insert(newPathStart);		// Changes vertices to look for

	int32_t lowerBound = 0;

	// Calculating lower bounds for current path
	// Outcoming
	int pathEndBound = MAX_INT;
	for (const auto& vertex : unvisitedVertices)
	{
		if (vertex != newPathStart && vertex != newPathEnd && mMatrix[newPathEnd][vertex] < pathEndBound)
			pathEndBound = mMatrix[newPathEnd][vertex];
	}

	// Incoming
	int pathStartBound = MAX_INT;
	for (const auto& vertex : unvisitedVertices)
	{
		if (vertex != newPathStart && vertex != newPathEnd && mMatrix[vertex][newPathStart] < pathStartBound)
			pathStartBound = mMatrix[vertex][newPathStart];
	}

#ifdef DEBUG
	std::cout << pathEndBound << " + " << pathStartBound << " + ";
#endif // DEBUG

	lowerBound += pathEndBound + pathStartBound;

	// Bounds for all other vertices
	for (const auto& vertex : unvisitedVertices)
	{
		if (vertex != newPathEnd && vertex != newPathStart)			// if edge is allowed
		{
			int32_t minOutcoming = INT32_MAX;
			// Outcoming from vertex: vertex -> i
			for (const auto& i : unvisitedVertices)
			{
				if (i != vertex && i != newPathEnd && mMatrix[vertex][i] < minOutcoming)
				{
					minOutcoming = mMatrix[vertex][i];
				}
			}

			int32_t minIncoming = INT32_MAX;
			// Incoming to vertex: i -> vertex
			for (const auto& i : unvisitedVertices)
			{
				if (i != vertex && i != newPathStart && mMatrix[i][vertex] < minIncoming)
				{
					minIncoming = mMatrix[i][vertex];
				}
			}

			lowerBound += minIncoming + minOutcoming;

#ifdef DEBUG
			std::cout << minOutcoming << " + " << minIncoming << " + ";
#endif // DEBUG
		}
	}
	lowerBound /= 2;

#ifdef DEBUG
	std::cout << "\nCalculated lower bound by the lightest edges L = " << lowerBound << '\n';
#endif // DEBUG

	return lowerBound;
}

inline int Old::findPrimMST(std::unordered_set<int> graph, int newVertex) const
{
	if (graph.empty()) return 0;

	int totalCost = 0;
	int start = newVertex;		// Vertex to start grow tree from

	graph.insert(newVertex);

	std::unordered_map<int, int> keys;		// {vertex, min distance to the tree}
	std::unordered_map<int, int> parent;	// {vertex, parent}
	for (auto vertex : graph)
	{
		keys.insert({ vertex, MAX_INT });
		parent.insert({ vertex, -1 });
	}
	keys[newVertex] = 0;
	
	using Edge = std::pair<int, int>;
	std::priority_queue<Edge, std::vector<Edge>, std::greater<>> q;		// queue of closest to the tree vertices
	q.push({ 0, start });

	while (!q.empty())
	{
		auto [costU, u] = q.top();		// getting new vertex
		q.pop();
		for (auto v : graph)			// checking all vertices to update distance to the tree
		{
			if (v != u && mMatrix[u][v] < keys[v])		// need to update distance
			{
				parent[v] = u;
				keys[v] = mMatrix[u][v];
				q.push({ mMatrix[u][v], v });
			}
		}
	}

#ifdef DEBUG
	std::cout << "Found Minimal spanning tree:\n";
#endif // DEBUG


	// getting total cost using knowlegde of vertices parentness
	for (auto vertex : graph)
	{
		if (parent[vertex] != -1)
		{
			totalCost += mMatrix[parent[vertex]][vertex];
#ifdef DEBUG
			std::cout << parent[vertex] << " -> " << vertex << " : cost = " << mMatrix[parent[vertex]][vertex] << '\n';
#endif // DEBUG

		}
	}


#ifdef DEBUG
	std::cout << "MST lower bound for path "; printPath(mCurPath); std::cout << "-> " << newVertex << " L = " << totalCost << '\n';
#endif // DEBUG

	return totalCost;
}

inline void Old::BnB(std::unordered_set<int> unvisitedVertices)
{
#ifdef DEBUG
	std::cout << "\nAnalysing path: "; printPath(mCurPath); std::cout << '\n';
#endif // DEBUG

	countBnB++;
	if (mCurPath.size() == mCitiesNum)		// If that is solution
	{
		int fullPathCost = mCurPathCost + mMatrix[mCurPath.back()][0];

#ifdef DEBUG
		std::cout << "Got new solution: cost = " << fullPathCost << '\n';
		printPath(mCurPath); std::cout << '\n';
#endif // DEBUG

		if (fullPathCost <= mBestPathCost)	// Check for better than the best one
		{
			mBestPath = mCurPath;
			mBestPathCost = fullPathCost;

#ifdef DEBUG
			std::cout << "!!! Found new best path: cost = " << fullPathCost << '\n';
			printPath(mBestPath); std::cout << '\n';
#endif	// DEBUG
		}
		return;
	}	// end better solution check


	// map for sorting alowed vertices by cost of their remaining paths
	std::multimap<int, int> verticesToAdd;		// key = remaining path cost lower bound, val = vertex 
	for (const auto& vertex : unvisitedVertices)
	{
		int edgesLowerBound = lowerBoundLightestEdges(unvisitedVertices, vertex);
		int MSTLowerBound = findPrimMST(unvisitedVertices, vertex);
		int lowerBound = std::max(edgesLowerBound, MSTLowerBound);		// overall lowest bound

#ifdef DEBUG
		std::cout << "Overall lower bound L = " << lowerBound << '\n';
#endif // DEBUG

		verticesToAdd.insert({ lowerBound + mMatrix[mCurPath.back()][vertex], vertex });	// insert new vertex according to its path cost
	}

	int tailVartex = mCurPath.back();
	for (auto [lowerBound, vertex] : verticesToAdd)		// iterating through sorted by their cost vertices to reduce bad decisions
	{
#ifdef DEBUG
		std::cout << "Vertex to check: " << vertex << '\n';
#endif // DEBUG

		int transitionCost = mMatrix[tailVartex][vertex];
		if (transitionCost != -1 && lowerBound + mCurPathCost <= mBestPathCost)		// if this edge is to be perspective
		{
			mCurPathCost += transitionCost;		// adding vertex to the partial solution
			mCurPath.push_back(vertex);

#ifdef DEBUG
			std::cout << "Added " << vertex << " to "; printPath(mCurPath); std::cout << " with cost " << mCurPathCost << "\n";
#endif	// DEBUG

			std::unordered_set<int> newUnvisitedVertices = unvisitedVertices;
			newUnvisitedVertices.erase(vertex);

			BnB(newUnvisitedVertices);		// recursively analyse new branch

			mCurPathCost -= transitionCost;	// backtrack
			mCurPath.pop_back();

#ifdef DEBUG
			std::cout << "Removed " << vertex << " from path with cost " << mCurPathCost << "; now:\n";
			printPath(mCurPath); std::cout << '\n';
#endif	// DEBUG
		}
#ifdef DEBUG
		else
		{
			std::cout << "Path "; printPath(mCurPath);
			std::cout << "-> " << vertex << " is unperspective due to too high cost: " << lowerBound + mCurPathCost << ", best = " << mBestPathCost << "\n";
		}
#endif // DEBUG
	}
}

inline void Old::printPath(const std::list<int>& path) const
{
	int i = 0;
	int n = path.size();
	for (const auto& city : path)
	{
		std::cout << city;
		if (++i < n)
			std::cout << " -> ";
	}
	std::cout << ' ';
}

std::pair<int, std::list<int>> Old::preciseSolve()
{
#ifdef DEBUG
	std::cout << "Starting Branch and Bound:\n";
#endif // DEBUG

	if (mMatrix.empty())
		return { 0, {} };

	std::unordered_set<int> unvisitedVertices;
	for (int i = 1; i < mCitiesNum; i++)
		unvisitedVertices.insert(i);

	BnB(unvisitedVertices);

#ifdef DEBUG
	std::cout << "Iterated: " << countBnB << '\n';
#endif

	return { mBestPathCost, mBestPath };
}

inline std::pair<int, std::list<int>> Old::approxSolve()
{
#ifdef DEBUG
	std::cout << "Starting approximate solution:\n";
#endif // DEBUG

	std::vector<int> bestSolution(mCitiesNum);
	for (int i = 0; i < mCitiesNum; i++)
		bestSolution[i] = i;

	auto printSolution = [](const std::vector<int>& sol)
		{
			int k = 0;
			for (auto& i : sol)
			{
				std::cout << i;
				if (++k < sol.size())
					std::cout << " -> ";
			}
			std::cout << '\n';
		};

	auto calcCost = [&](const std::vector<int>& sol) -> int {
		int cost = 0;
		for (int i = 0; i < mCitiesNum; i++) {
			int from = sol[i];
			int to = sol[(i + 1) % mCitiesNum];
			cost += mMatrix[from][to];
		}
		return cost;
		};

	int bestSolutionCost = calcCost(bestSolution);

#ifdef DEBUG
	std::cout << "Basic solution is: Cost = " << bestSolutionCost << "\n"; printSolution(bestSolution);
#endif // DEBUG


	int count = 0;
	bool successModification = true;
	while (count < mCitiesNum && successModification)
	{
#ifdef DEBUG
		std::cout << '\n' << count + 1 << ") Iteration. Current best solution: cost = " << bestSolutionCost << "\n"; printSolution(bestSolution);
#endif // DEBUG

		successModification = false;
		int candidateCity = -1;

		// delta shows is it good to this city be here
		std::multimap<int, int, std::greater<>> deltas; // {delta, vertex}

#ifdef DEBUG
		std::cout << "Choosing city to move\n";
#endif // DEBUG


		for (int i = 1; i < mCitiesNum; i++)	// calculating delta for each vertex (but 0 is fixed)
		{
			int prev = bestSolution[(i - 1 + mCitiesNum) % mCitiesNum];
			int cur = bestSolution[i];
			int next = bestSolution[(i + 1) % mCitiesNum];

			int currentCost = mMatrix[prev][cur] + mMatrix[cur][next];
			int withoutCity = mMatrix[prev][next];
			int delta = currentCost - withoutCity;

			deltas.insert({ delta, i });

#ifdef DEBUG
			std::cout << bestSolution[i] << " delta = " << delta << '\n';
#endif // DEBUG

		}

		// Check if all cities fits good
		if ((*(deltas.begin())).first < 0)
		{
#ifdef DEBUG
			std::cout << "No city that affects bad is found\n";
#endif // DEBUG

			break;
		}

#ifdef DEBUG
		std::cout << "\nIterating through city to move, then iterating through position to move to:\n";
#endif // DEBUG


		// Iterating through sorted by delta cities
		for (auto [delta, city] : deltas)
		{
			candidateCity = city;
			int bestLocalCost = bestSolutionCost;
			std::vector<int> bestLocalSolution = bestSolution;

#ifdef DEBUG
			std::cout << "\nObserving city " << candidateCity << " with delta " << delta << "\nLooking for new place:\n";
#endif // DEBUG

			for (int j = 1; j < mCitiesNum; j++)
			{
				std::vector<int> newSolution = bestLocalSolution;
				std::swap(newSolution[candidateCity], newSolution[j]);
				int newCost = calcCost(newSolution);

#ifdef DEBUG
				std::cout << "\nMove to " << j << ": cost = " << newCost << '\n'; printSolution(newSolution);
#endif // DEBUG

				if (newCost < bestLocalCost)
				{
					bestLocalCost = newCost;
					bestLocalSolution = newSolution;

#ifdef DEBUG
					std::cout << "Solution with cost = " << bestLocalCost << " is new local best\n"; printSolution(bestLocalSolution);
#endif // DEBUG
				}
#ifdef DEBUG
				else {
					std::cout << "Does not improve; next\n";
				}
#endif // DEBUG

			}

			if (bestLocalCost < bestSolutionCost) {
				bestSolution = bestLocalSolution;
				bestSolutionCost = bestLocalCost;
				successModification = true;
				count++;

#ifdef DEBUG
				std::cout << "Solution with cost = " << bestSolutionCost << " is new best\n"; printSolution(bestSolution);
				std::cout << "Breaking\n";
#endif // DEBUG
				break;
			}
		}
	}

	std::list<int> solution;
	for (auto& vertex : bestSolution)
		solution.push_back(vertex);

	return { bestSolutionCost, solution };
}
