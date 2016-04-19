#ifndef __NSENSOR_DOUBLE_LINKED_LIST__
#define __NSENSOR_DOUBLE_LINKED_LIST__
template <typename T>
struct Node {
	T obj;
	Node *prev;
	Node *next;
};

template <class T>
class DList {
private:
	int m_size;			//current size
	Node<T> *m_head;
	Node<T> *m_tail;
	word	m_max;		//max size
	int m_pos;        	//current position of browser pointer
	Node<T> *m_pointer;
	
	void _reduce_max() {
		if (size() > m_max && m_max != 0)
			pop_back();
	}
protected:
	//add head if size = 1
	bool addHead(Node<T> *&p) {
		m_head = p;
		m_tail = p;
		m_pointer = p;
		m_pos = 0;
		return true;
	}
public:
	//canADd
	const bool canAdd() const {
		return m_max == 0 || m_size < m_max;
	}

	//create a node
	Node<T> *&getNode(T obj) {
		Node<T> *p = new Node<T>;
		if (p != NULL) {
			p->obj = obj;
			p->next = p->prev = NULL;
			m_size++;
		}
		return p;
	}
	
	
	
	DList(): m_size(0), m_head(NULL), m_tail(NULL), m_pos(0), m_pointer(NULL), m_max(0) {}

	Node<T> *at(const int pos) {
		if (m_pointer == NULL || pos < 0 || pos >= size())
			return NULL;
		
		if (pos == 0) {
			m_pointer = m_head;
		} else if (pos == size() - 1)
			m_pointer = m_tail;
		else {
			while (m_pos < pos) {
				m_pointer = m_pointer->next;
				m_pos++;
			}

			while (m_pos > pos) {
				m_pointer = m_pointer->prev;
				m_pos--;
			}
		}
		m_pos = pos;
		return m_pointer;
	}
	
	Node<T> *find(T obj) {
		for (word i = 0; i < size(); i++) {
			Node<T> *node = at(i);
			if (node->obj == obj)
				return node;
		}
		return NULL;
	}

	//set max element
	void setMax(const word max) {
		m_max = max;
	}

	//get max element count
	word getMax() const {
		return m_max;
	}

	//check queue is empty
	int empty() const {
		return m_size == 0;
	}

	//get size
	int size() const {
		return m_size;
	}

	//get front object
	T front() {
		if (m_head == NULL)
			return NULL;
		return m_head->obj;
	}


	//get tail object
	T back() {
		if (m_tail == NULL)
			return NULL;
		return m_tail->obj;
	}
	

	//push back
	bool push_back(T obj) {
		Node<T> *p = getNode(obj);
		if (p == NULL)
			return false;

		if (m_size == 1)
			return addHead(p);
		else {
			p->prev = m_tail;
			m_tail->next = p;
			m_tail = p;
			_reduce_max();
		}
		return true;
	}

	//push front
	bool push_front(T obj) {
		Node<T> *p = getNode(obj);
		if (p == NULL)
			return false;
	
		if (m_size == 1)
			return addHead(p);
		else {
			p->next = m_head;
			m_head->prev = p;
			m_head = p;

			//increase pos
			m_pos++;

			_reduce_max();
		}
		return true;
	}

	//pop front
	bool pop_front() {
		if (empty())
			return false;

		if (size() == 1) {
			delete m_head;
			m_pointer = m_head = m_tail = NULL;
			m_size--;
			m_pos = 0;
			return true;
		}
		
		m_size--;
		m_head->next->prev = NULL;
		Node<T> *p = m_head->next;
		delete m_head;
		m_head = p;
		m_pos--;
		if (m_pos < 0) {
			m_pos = 0;
			m_pointer = m_head;
		}
		return true;
	}

	//pop back
	bool pop_back() {
		if (empty())
			return false;

		
		if (size() == 1) {
			delete m_tail;
			m_pointer = m_head = m_tail = NULL;			
			m_size--;
			m_pos = 0;
			return true;
		}
		
		m_size--;
		m_tail->prev->next = NULL;
		Node<T> *p = m_tail->prev;
		delete m_tail;
		m_tail = p;

		if (m_pos >= m_size) {
			m_pos = m_size - 1;
			m_pointer = m_tail;
		}
		return true;
	}
	
	//pot at i
	bool pop_at(word i) {
		if (empty())
			return false;
		if (size() == 1) {
			delete m_tail;
			m_pointer = m_head = m_tail = NULL;			
			m_size--;
			m_pos = 0;
			return true;
		}
	}
	
	//remove node
	bool removeNode(Node<T> *&node) {
		if (empty())
			return false;
		
		if (node == m_head)
			return pop_front();
		else if (node == m_tail)
			return pop_back();
		else {
			node->prev->next = node->next;
			node->next->prev = node->prev;
			delete node;
		}
	}
};
#endif