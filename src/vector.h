#ifndef FILE_VECTOR_H
#define FILE_VECTOR_H

#include <iostream>
#include "shared_mat_vec.h"
#include "matrix.h"
#include "expression.h"
#include <math.h>

namespace bla
{

	// template <typename T = double, typename TDIST = std::integral_constant<size_t, 1>>
	// class VectorView;


	template <typename T, typename TDIST>
	class VectorView : public VecExpr<VectorView<T, TDIST>>
	{
	protected:
		size_t size_;
		TDIST dist_;
		T *data_;

	public:
		VectorView(size_t size, T *data)
			: data_(data), size_(size) {}

		VectorView(size_t size, TDIST dist, T *data)
			: data_(data), size_(size), dist_(dist) {}

		template <typename TB>
		VectorView &operator=(const VecExpr<TB> &v2)
		{
			for (size_t i = 0; i < size_; i++)
				data_[dist_ * i] = v2(i);
			return *this;
		}

		VectorView &operator=(const VectorView &v2)
		{
			for (size_t i = 0; i < size_; i++)
				data_[dist_ * i] = v2(i);
			return *this;
		}

		VectorView &operator=(T scal)
		{
			for (size_t i = 0; i < size_; i++)
				data_[dist_ * i] = scal;
			return *this;
		}

		auto Upcast() const { return VectorView(size_, dist_, data_); }
		size_t Size() const { return size_; }
		T &operator()(size_t i) { return data_[dist_ * i]; }
		const T &operator()(size_t i) const { return data_[dist_ * i]; }
		// const T *Data() const { return data_; }
		T *Data() const { return data_; }
		auto Range(size_t first, size_t next) const
		{
			return VectorView(next - first, dist_, data_ + first * dist_);
		}

		auto Slice(size_t first, size_t slice) const
		{
			return VectorView<T, size_t>(size_ / slice, dist_ * slice, data_ + first * dist_);
		}

		VectorView& operator*=(T scal)
		{
			for (size_t i = 0; i < size_; i++)
				data_[dist_ * i] *= scal;
			return *this;
		}

		VectorView& operator/=(T scal)
		{
			for (size_t i = 0; i < size_; i++)
				data_[dist_ * i] /= scal;
			return *this;
		}

		template <typename TB>
		VectorView& operator+=(const VecExpr<TB> &v)
		{
			for (size_t i = 0; i < size_; i++)
				data_[dist_ * i] += v(i);
			return *this;
		}

		template <typename TB>
		VectorView& operator-=(const VecExpr<TB> &v)
		{
			for (size_t i = 0; i < size_; i++)
				data_[dist_ * i] -= v(i);
			return *this;
		}

		auto AsMatrix(size_t rows, size_t cols) const
		{
			return MatrixView<T, ORDERING::RowMajor>(rows, cols, cols, data_);
		}
	};

	template <typename T=double>
	class Vector : public VectorView<T>
	{
		typedef VectorView<T> BASE;
		using BASE::data_;
		using BASE::size_;

	public:
		Vector(size_t size)
			: VectorView<T>(size, new T[size]) { ; }

		Vector(const Vector &v)
			: Vector(v.Size())
		{
			*this = v;
		}

		Vector(Vector &&v)
			: VectorView<T>(0, nullptr)
		{
			std::swap(size_, v.size_);
			std::swap(data_, v.data_);
		}

		template <typename TB>
		Vector(const VecExpr<TB> &v)
			: Vector(v.Size())
		{
			*this = v;
		}

		Vector(std::initializer_list<T> list)
			: VectorView<T> (list.size(), new T[list.size()]) {
			for (size_t i = 0; i < list.size(); i++)
				data_[i] = list.begin()[i];
		}

		~Vector() { delete[] data_; }

		using BASE::operator=;
		Vector &operator=(const Vector &v2)
		{
			for (size_t i = 0; i < size_; i++)
				data_[i] = v2(i);
			return *this;
		}

		Vector &operator=(Vector &&v2)
		{
			for (size_t i = 0; i < size_; i++)
				data_[i] = v2(i);
			return *this;
		}
	};

	template <int SIZE, typename T = double>
	class Vec : public VecExpr<Vec<SIZE,T>>
	{
	private:
		T data[SIZE];

	public:
		Vec() { };
		Vec(std::initializer_list<T> list) 
		{
			for (size_t i = 0; i < list.size(); i++)
				data[i] = list.begin()[i];
		}

		Vec(T scal) 
		{
			for (size_t i = 0; i < SIZE; i++)
				data[i] = scal;
		}

		Vec(const Vec &v2)
		{
			for (size_t i = 0; i < SIZE; i++)
				data[i] = v2(i);
		}

		template <typename TB>
		Vec(const VecExpr<TB> &v2)
		{
			for (size_t i = 0; i < SIZE; i++)
				data[i] = v2(i);
		}

		template <typename TB>
		Vec(const VectorView<TB> &v2)
		{
			for (size_t i = 0; i < SIZE; i++)
				data[i] = v2(i);
		}

		Vec &operator=(const Vec &v2)
		{
			for (size_t i = 0; i < SIZE; i++)
				data[i] = v2(i);
			return *this;
		}
		Vec &operator=(Vec &&v2)
		{
			for (size_t i = 0; i < SIZE; i++)
				data[i] = v2(i);
			return *this;
		}
		Vec &operator=(const T &scal)
		{
			for (size_t i = 0; i < SIZE; i++)
				data[i] = scal;
			return *this;
		}
		auto Upcast() const { return *this; }
		size_t Size() const { return SIZE; }
		T &operator()(size_t i) { return data[i]; }
		const T &operator()(size_t i) const { return data[i]; }
		T *Data() const { return data; }
	};

	template <typename... Args>
	std::ostream &operator<<(std::ostream &ost, const VectorView<Args...> &v)
	{
		if (v.Size() > 0)
			ost << v(0);
		for (size_t i = 1; i < v.Size(); i++)
			ost << ", " << v(i);
		return ost;
	}

	template <typename T>
	T L2NormSquared(const Vector<T> &v)
	{
		return v * v;
	}

	template <typename T>
	auto L2Norm(const VecExpr<T> &v)
	{
		typedef decltype(v(0)) Tres;
		Tres r = 0;
		for (size_t i = 0; i < v.Size(); i++)
			r += v(i) * v(i);
		return sqrt(r);
	}

}

#endif
