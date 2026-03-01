#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include "exceptions.hpp"

namespace sjtu {
/**
 * @brief a container like std::priority_queue which is a heap internal.
 * **Exception Safety**: The `Compare` operation might throw exceptions for certain data.
 * In such cases, any ongoing operation should be terminated, and the priority queue should be restored to its original state before the operation began.
 */
template<typename T, class Compare = std::less<T>>
class priority_queue {
private:
	struct Node {
		T value;
		Node *left;
		Node *right;
		size_t dist;

		explicit Node(const T &v)
				: value(v), left(nullptr), right(nullptr), dist(1) {}
	};

	Node *root;
	size_t cnt;

	static size_t get_dist(Node *x) {
		return x == nullptr ? 0 : x->dist;
	}

	void clear(Node *x) {
		if (x == nullptr) return;
		clear(x->left);
		clear(x->right);
		delete x;
	}

	Node *clone(Node *x) {
		if (x == nullptr) return nullptr;
		Node *ret = new Node(x->value);
		try {
			ret->left = clone(x->left);
			ret->right = clone(x->right);
			ret->dist = x->dist;
		} catch (...) {
			clear(ret->left);
			clear(ret->right);
			delete ret;
			throw;
		}
		return ret;
	}

	Node *mergeNode(Node *a, Node *b) {
		if (a == nullptr) return b;
		if (b == nullptr) return a;

		Compare cmp;
		if (cmp(a->value, b->value)) {
			Node *tmp = a;
			a = b;
			b = tmp;
		}

		Node *mergedRight = mergeNode(a->right, b);
		a->right = mergedRight;

		if (get_dist(a->left) < get_dist(a->right)) {
			Node *tmp = a->left;
			a->left = a->right;
			a->right = tmp;
		}
		a->dist = get_dist(a->right) + 1;
		return a;
	}

public:
	/**
	 * @brief default constructor
	 */
	priority_queue() : root(nullptr), cnt(0) {}

	/**
	 * @brief copy constructor
	 * @param other the priority_queue to be copied
	 */
	priority_queue(const priority_queue &other) : root(nullptr), cnt(other.cnt) {
		root = clone(other.root);
	}

	/**
	 * @brief deconstructor
	 */
	~priority_queue() {
		clear(root);
		root = nullptr;
		cnt = 0;
	}

	/**
	 * @brief Assignment operator
	 * @param other the priority_queue to be assigned from
	 * @return a reference to this priority_queue after assignment
	 */
	priority_queue &operator=(const priority_queue &other) {
		if (this == &other) return *this;
		Node *newRoot = clone(other.root);
		clear(root);
		root = newRoot;
		cnt = other.cnt;
		return *this;
	}

	/**
	 * @brief get the top element of the priority queue.
	 * @return a reference of the top element.
	 * @throws container_is_empty if empty() returns true
	 */
	const T & top() const {
		if (root == nullptr) throw container_is_empty();
		return root->value;
	}

	/**
	 * @brief push new element to the priority queue.
	 * @param e the element to be pushed
	 */
	void push(const T &e) {
		Node *node = new Node(e);
		try {
			root = mergeNode(root, node);
			++cnt;
		} catch (...) {
			delete node;
			throw runtime_error();
		}
	}

	/**
	 * @brief delete the top element from the priority queue.
	 * @throws container_is_empty if empty() returns true
	 */
	void pop() {
		if (root == nullptr) throw container_is_empty();
		Node *old = root;
		try {
			root = mergeNode(root->left, root->right);
			--cnt;
			delete old;
		} catch (...) {
			throw runtime_error();
		}
	}

	/**
	 * @brief return the number of elements in the priority queue.
	 * @return the number of elements.
	 */
	size_t size() const {
		return cnt;
	}

	/**
	 * @brief check if the container is empty.
	 * @return true if it is empty, false otherwise.
	 */
	bool empty() const {
		return cnt == 0;
	}

	/**
	 * @brief merge another priority_queue into this one.
	 * The other priority_queue will be cleared after merging.
	 * The complexity is at most O(logn).
	 * @param other the priority_queue to be merged.
	 */
	void merge(priority_queue &other) {
		if (this == &other || other.root == nullptr) return;
		try {
			root = mergeNode(root, other.root);
			cnt += other.cnt;
			other.root = nullptr;
			other.cnt = 0;
		} catch (...) {
			throw runtime_error();
		}
	}
};

}

#endif