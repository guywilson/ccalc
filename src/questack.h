#include <string>
#include <cstring>
#include <queue>
#include <stack>

using namespace std;

#ifndef __INCL_QSTACK
#define __INCL_QSTACK

class Stack {
    private:
        stack<std::string>      _stack;

    public:
        void push(string & s) {
            _stack.push(s);
        }

        string & pop() {
            string & s = _stack.top();
            _stack.pop();

            return s;
        }

        string & peek() {
            string & s = _stack.top();

            return s;
        }

        size_t size() {
            return _stack.size();
        }

        bool isEmpty() {
            return _stack.empty();
        }
};

class Queue {
    private:
        queue<std::string>      _queue;

    public:
        void put(string & s) {
            _queue.push(s);
        }

        string & get() {
            string & s = _queue.front();
            _queue.pop();

            return s;
        }

        size_t size() {
            return _queue.size();
        }

        bool isEmpty() {
            return _queue.empty();
        }
};

#endif
