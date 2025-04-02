#pragma once

#include <vector>
#include <iostream>
#include <stdexcept>

/*
* Custom heap class. Uses vector as value storage.
* Use std::greater for max-heap and std::less for min-heap.
*/
template<typename KTy, typename Ty, typename Cmp = std::less<>>
class PriorityQueue
{
	std::vector<std::pair<KTy, Ty>> heap;

	size_t left(size_t ind) const;

	size_t right(size_t ind) const;

	size_t parent(size_t ind) const;

	void heapify(size_t ind);
public:
	PriorityQueue() = default;

	template<typename Container = std::vector<std::pair<KTy, Ty>>>
	PriorityQueue(Container&& init);

	const std::pair<KTy, Ty>& top() const;
	
	void insert(KTy key, Ty value);

	void changeKey(Ty item, KTy new_key);

	bool empty() const;

	void pop();

	void print();

	~PriorityQueue() = default;
};

template<typename KTy, typename Ty, typename Cmp>
inline size_t PriorityQueue<KTy, Ty, Cmp>::left(size_t ind) const
{
	return ind*2 + 1;
}

template<typename KTy, typename Ty, typename Cmp>
inline size_t PriorityQueue<KTy, Ty, Cmp>::right(size_t ind) const
{
	return ind*2 + 2;
}

template<typename KTy, typename Ty, typename Cmp>
inline size_t PriorityQueue<KTy, Ty, Cmp>::parent(size_t ind) const
{
	return (ind - 1)/2;
}

template<typename KTy, typename Ty, typename Cmp>
inline void PriorityQueue<KTy, Ty, Cmp>::heapify(size_t ind)
{
	size_t l = left(ind), r = right(ind);
	size_t largest = 0;
	if (l < heap.size() && Cmp()(heap[l].first, heap[ind].first))
		largest = l;
	else
		largest = ind;
	if (r < heap.size() && Cmp()(heap[r].first, heap[largest].first))
		largest = r;
	if (largest != ind)
	{
		std::swap(heap[ind], heap[largest]);
		heapify(largest);
	}
}

template<typename KTy, typename Ty, typename Cmp>
inline const std::pair<KTy, Ty>& PriorityQueue<KTy, Ty, Cmp>::top() const
{
	return heap.at(0);
}

template<typename KTy, typename Ty, typename Cmp>
inline void PriorityQueue<KTy, Ty, Cmp>::insert(KTy key, Ty value)
{
	heap.push_back({ key, value });
	changeKey(value, key);
}

template<typename KTy, typename Ty, typename Cmp>
inline void PriorityQueue<KTy, Ty, Cmp>::changeKey(Ty item, KTy new_key)
{
	size_t item_id = 0;
	for (item_id = 0; item_id < heap.size() && heap[item_id].second != item; item_id++);
	if (item_id >= heap.size())
		throw std::runtime_error("No such item\n");
	if (Cmp()(heap[item_id].first, new_key))
		throw std::runtime_error("New key is inappropriate\n");
	heap[item_id].first = new_key;
	while (item_id > 0 && Cmp()(heap[item_id].first, heap[parent(item_id)].first))
	{
		std::swap(heap[item_id], heap[parent(item_id)]);
		item_id = parent(item_id);
	}
}

template<typename KTy, typename Ty, typename Cmp>
inline bool PriorityQueue<KTy, Ty, Cmp>::empty() const
{
	return heap.empty();
}

template<typename KTy, typename Ty, typename Cmp>
inline void PriorityQueue<KTy, Ty, Cmp>::pop()
{
	heap.at(0) = heap.back();
	heap.pop_back();
	heapify(0);
}

template<typename KTy, typename Ty, typename Cmp>
inline void PriorityQueue<KTy, Ty, Cmp>::print()
{
	for (const auto& v : heap)
		std::cout << v.first<<' '<<v.second << " - ";
	std::cout << '\n';
}

template<typename KTy, typename Ty, typename Cmp>
template<typename Container>
inline PriorityQueue<KTy, Ty, Cmp>::PriorityQueue(Container&& init) :
	heap(std::forward<Container>(init))
{
	for (int i = heap.size() / 2; i >= 0; i--)
		heapify(i);
}
