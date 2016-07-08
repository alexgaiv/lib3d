#ifndef _NULLABLE_H_
#define _NULLABLE_H_

template<class T>
class Nullable
{
public:
	Nullable() : val(0) { }

	Nullable(const T &value) {
		val = new T(value);
	}

	Nullable(const Nullable<T> &obj) {
		val = obj.val ? new T(*obj.val) : 0;
	}

	~Nullable() { delete val; }

	bool operator==(const Nullable &obj) const {
		if (val == obj.val) return true;
		if (val && obj.val)
			return *val == *obj.val;
		return false;
	}

	bool operator!=(const Nullable &obj) const {
		return !operator==(obj);
	}

	Nullable &operator=(const Nullable &obj) {
		delete val;
		val = obj.val ? new T(*obj.val) : 0;
		return *this;
	}

	Nullable &operator=(const T &value) {
		if (val) *val = value;
		else val = new T(value);
		return *this;
	}
	
	Nullable &operator=(int) {
		delete val;
		val = 0;
		return *this;
	}

	operator T*() { return val; }
	operator const T*() const { return val; }

	T &operator*() { return *val; }
	T *operator->() { return val; }
	const T &operator*() const { return *val; }
	const T *operator->() const { return val; }
private:
	T *val;
};

#endif // _NULLABLE_H_