//////////
//////////Vector 2 files
//////////Written by Tanapat Somrid 
/////////Starting 01/12/2021
//////// Most Recent Update 06/01/2022
//////// Most Recent change: Changed Magnitude Function, added multiply function

#pragma once
#ifndef VECTOR2_H
#define VECTOR2_H

#include <math.h>
#include <iostream>

/// <summary>
/// Used for easy (and soon to be complex) Vector2 Operations
/// </summary>
/// <typeparam name="T"></typeparam>
template <typename T> class Vector2 {
public:

	#pragma region CONSTRUCTORS
	/// <summary>
/// Default Constructor
/// </summary>
	Vector2() {};
	/// <summary>
	/// Individual Coordinate Constructor
	/// </summary>
	Vector2(T x, T y) : Vector2() { this->x = x; this->y = y; }
	/// <summary>
	/// Assignment with different typename
	/// </summary>
	template <typename U> explicit Vector2(const Vector2<U>& difPrimitive) {
		x = static_cast<T>(difPrimitive.x); y = static_cast<T>(difPrimitive.y);
	}
	//Copy Constructors
	Vector2(const Vector2& rhs) { x = rhs.x; y = rhs.y; }//Required for assignment and required for below operatorr//	Vector2<int> dif = position;
	Vector2& operator =(const Vector2& copy) { x = copy.x; y = copy.y; return *this; }//So this will copy/assign//		Node* endNode = new Node(endPos);
#pragma endregion

	T x, y;

	#pragma region FUNCTIONS
	//T Magnitude() const{
	//	return (T)(sqrt(abs(x * x) + abs(y * y)));
	//}
	float Magnitude() const {
		return (float)(sqrt(abs(x * x) + abs(y * y)));
	}
	static float Magnitude(const Vector2<T>& vector) {
		return (float)(sqrt(abs(vector.x * vector.x) + abs(vector.y * vector.y)));
	}
	void Normalise() {
		*this = *this / static_cast<int>(this->Magnitude());
	}
	Vector2<float> Normalize() {
		return Vector2<float>(static_cast<Vector2<float>>(*this) / this->Magnitude());
	}
	/// <summary>
/// The EUCLIDEAN distance from this vector and the given coordinates (x and y)
/// </summary>
/// <returns>EUCLIDEAN DISTANCE</returns>
	//int DistanceFrom(T x, T y) { return sqrt((abs(x - this->x) * abs(x - this->x)) + (abs(y - this->y) * abs(y - this->y))); }
	///// <summary>
	///// The EUCLIDEAN distance from this vector and the given vector
	///// </summary>
	///// <returns>EUCLIDEAN DISTANCE</returns>
	//float DistanceFrom(const Vector2<T>& pos) {
	//	int xl = abs(pos.x - x); int yl = abs(pos.y - y);
	//	float sq = sqrt(abs(xl * xl) + abs(yl * yl));
	//	return sq;
	//}
	///// <summary>
	///// The EUCLIDEAN distance between two given vectors
	///// <para> From vector A to vector B</para>
	///// </summary>
	///// <param name="Vector A"></param>
	///// <param name="Vector B"></param>
	///// <returns>EUCLIDEAN DISTANCE</returns>
	//static float DistanceBetween(const Vector2& a, const Vector2& b) {
	//	int x = abs(b.x - a.x); int y = abs(b.y - a.y);
	//	float sq = sqrt(abs(x * x) + abs(y * y));
	//	return sq;
	//}	
	/// <summary>
	/// The EUCLIDEAN distance from this vector and the given vector
	/// </summary>
	/// <returns>EUCLIDEAN DISTANCE</returns>
	float DistanceFrom(const Vector2<T>& pos) {
		int xl = abs(pos.x - x); int yl = abs(pos.y - y);
		float sq = sqrt((xl*xl) + (yl * yl));
		return sq;
	}
	/// <summary>
	/// The EUCLIDEAN distance between two given vectors
	/// <para> From vector A to vector B</para>
	/// </summary>
	/// <param name="Vector A"></param>
	/// <param name="Vector B"></param>
	/// <returns>EUCLIDEAN DISTANCE</returns>
	static float DistanceBetween(const Vector2& a, const Vector2& b) {
		int x = abs(b.x - a.x); int y = abs(b.y - a.y);
		float sq = sqrt((x * x) + (y * y));
		return sq;
	}
#pragma endregion
};

//I had a slight understanding of operator overload already but this was the first time trying to do it in a templated class setting. 
//All operators besides the assignment operator seems to need to be outside the class,
//Although maybe when I test the others i'll find out the assigment/increments (+= -+) will need to be inside the class

#pragma region OPERATOR FUNCTIONS
/// <summary>
/// This works
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="lhs"></param>
/// <param name="rhs"></param>
/// <returns></returns>
template <typename T> Vector2<T> operator +(const Vector2<T>& lhs, const Vector2<T>& rhs) {
	return Vector2<T>((lhs.x + rhs.x), (lhs.y + rhs.y));
}
/// <summary>
/// This works
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="lhs"></param>
/// <param name="rhs"></param>
/// <returns></returns>
template <typename T> Vector2<T> operator -(const Vector2<T>& lhs, const Vector2<T>& rhs) {
	return Vector2<T>((lhs.x - rhs.x), (lhs.y - rhs.y));
}
template <typename T> Vector2<float> operator -(const Vector2<T>& lhs, const Vector2<float>& rhs) {
	return Vector2<float>((lhs.x - rhs.x), (lhs.y - rhs.y));
}
template <typename T, typename U> Vector2<float> operator -(const Vector2<T>& lhs, const Vector2<U>& rhs) {
	return Vector2<float>((lhs.x - rhs.x), (lhs.y - rhs.y));
}
/// <summary>
/// This works
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="lhs"></param>
/// <param name="rhs"></param>
/// <returns></returns>
template <typename T> Vector2<T> operator /(const Vector2<T>& lhs, const T& rhs) {
	return Vector2<T>((lhs.x / rhs), (lhs.y / rhs));
}
template <typename T> Vector2<T> operator *(const Vector2<T>& lhs, const T& rhs) {
	return Vector2<T>((lhs.x * rhs), (lhs.y * rhs));
}
/// <summary>
/// This works
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="lhs"></param>
/// <param name="rhs"></param>
/// <returns></returns>
template <typename T> void operator +=(Vector2<T>& lhs, const Vector2<T>& rhs) {
	//lhs.x = lhs.x + rhs.x;
	//lhs.y = lhs.y + rhs.y;
	//return *lhs;
	lhs.x = lhs.x + rhs.x;
	lhs.y = lhs.y + rhs.y;
	return;
}

/// <summary>
/// This Works
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="lhs"></param>
/// <param name="rhs"></param>
/// <returns></returns>
template <typename T> void operator -=(Vector2<T>& lhs, const Vector2<T>& rhs) {
	//lhs.x = lhs.x - rhs.x;
	//lhs.y = lhs.y - rhs.y;
	//return *lhs;
	lhs.x = lhs.x - rhs.x;
	lhs.y = lhs.y - rhs.y;
	return;
}
/// <summary>
/// untested
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="lhs"></param>
/// <param name="rhs"></param>
template <typename T> void operator /=(const Vector2<T>& lhs, const T& rhs) {
	lhs.x = lhs.x / rhs.x;
	lhs.y = lhs.y / rhs.y;
	return;
}
/// <summary>
/// Untested
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="lhs"></param>
/// <param name="rhs"></param>
/// <returns></returns>
template <typename T> bool operator ==(const Vector2<T>& lhs, const Vector2<T>& rhs) {
	return (lhs.x == rhs.x && lhs.y == rhs.y);
}
/// <summary>
/// Untested
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="lhs"></param>
/// <param name="rhs"></param>
/// <returns></returns>
template <typename T> bool operator !=(const Vector2<T>& lhs, const Vector2<T>& rhs) {
	return !(lhs.x == rhs.x && lhs.y == rhs.y);
}

/// <summary>
/// This works
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="lhs"></param>
/// <param name="rhs"></param>
/// <returns></returns>
template <typename T> bool operator >(const Vector2<T>& lhs, const Vector2<T>& rhs) {
	return (lhs.Magnitude() > rhs.Magnitude());
}
/// <summary>
/// This works
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="lhs"></param>
/// <param name="rhs"></param>
/// <returns></returns>
template <typename T> bool operator <(const Vector2<T>& lhs, const Vector2<T>& rhs){
	return (lhs.Magnitude() < rhs.Magnitude());
}

/// <summary>
/// Streams out "X: vector.x Y: vector.y"
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="stream"></param>
/// <param name="vector"></param>
/// <returns></returns>
template<typename T> std::ostream& operator<<(std::ostream& stream, Vector2<T> const& vector)
{
	return stream << "X:" << vector.x << " Y:" << vector.y;
}
#pragma endregion


#endif // !VECTOR2_H

/*
///Not called?
//template <typename T> Vector2<T> operator +(const Vector2<T>& rhs) {
//	this.x += rhs.x;
//	this.y += rhs.y;
//	return this;
//
//	//Vector2<T> newVector;
//	//newVector.x = lhs.x + rhs.x;
//	//newVector.y = lhs.y + rhs.y;
//	//return newVector;
//}
	////bool operator <(const Vector2<T>* a) {
	////	return (Magnitude() < a->Magnitude());
	////}
	////bool operator >(const Vector2<T>* a) {
	////	return (Magnitude() > a->Magnitude());
	////}

	////constexpr Vector2<T>& operator ()(const Vector2<T>& copy) {
	////	lhs->x = x + rhs->x;
	////	lhs->y = y + rhs->y;
	////	return lhs;
	////}
	//// Vector2<T>& operator -(const Vector2<T>& rhs) { //messy
	////	//x = x - rhs.x;
	////	//y = y - rhs.y;
	////	//return *this;
	////	std::shared_ptr<Vector2<T>> newVector(new Vector2<T>);
	////	newVector->x = x - rhs.x;
	////	newVector->y = y - rhs.y;
	////	return *newVector;
	////}
	//// Vector2<T> operator +(const Vector2<T>& rhs) { //messy
	////	 std::shared_ptr<Vector2<T>> newVector(new Vector2<T>);
	////	 newVector->x = x + rhs.x;
	////	 newVector->y = y + rhs.y;
	////	 return *newVector;
	////}
	*/