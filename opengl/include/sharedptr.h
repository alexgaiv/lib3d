#ifndef _SHARED_PTR_H_
#define _SHARED_PTR_H_

template<class T>
class my_shared_ptr
{
public:
	my_shared_ptr(T *ptr = 0) : ptr(ptr) {
		refCount = new int(1);
	}

	my_shared_ptr(const my_shared_ptr &p) {
		addref(p);
	}

	~my_shared_ptr() {
		release();
	}

	my_shared_ptr &operator=(const my_shared_ptr &p) {
		release();
		addref(p);
		return *this;
	}

	T *Get() { return ptr; }
	const T *Get() const { return ptr; }

	T *operator->() { return ptr; }
	T &operator*() { return *ptr; }
	const T *operator->() const { return ptr; }
	const T &operator*() const { return *ptr; }
private:
	T *ptr;
	int *refCount;

	void addref(const my_shared_ptr &p) {
		ptr = p.ptr;
		refCount = p.refCount;
		++*refCount;
	}

	void release() {
		if (--*refCount == 0) {
			delete ptr;
			delete refCount;
		}
	}
};

#endif // _SHARED_PTR_H_