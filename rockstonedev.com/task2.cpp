#include <istream>
#include <ostream>
#include <cinttypes>
#include <math.h>
#include <thread>
#define RADIANS_TO_DEGREES 180.f / PI_;
#include <atomic>
#include <type_traits>
#include <vector>
#include <mutex>
#include <iostream>
#include <fstream>

class Vector;

typedef std::vector<Vector*> vectors;

using namespace std;

#define OUT std::cout

class Vector
{
public:
	static constexpr float PI_ = 3.14;

	float x;
	float y;

	double get_magnitude()
	{
		return sqrt(x *x + y *y);
	}

	Vector()
	{
		this->y = 0;
		this->x = 0;
	}

	static auto angle(const Vector* const  firstVector, Vector const * const secondVector) -> long double
	{
		auto firstNormalized_vector = firstVector->normalized();
		auto secondNormalized_vector = secondVector->normalized();

		const Vector diff = firstNormalized_vector.sub(secondNormalized_vector);
		const Vector *diff_ptr = &diff;

		(const_cast<Vector*>(diff_ptr))->normalize();

		return atan2(diff.y, diff.x) *RADIANS_TO_DEGREES;

	}

	Vector sub(const Vector other)
	{
                Vector result;

		result.x = this->x - other.x;
		result.y = this->y - other.y;

		return result;
	}

	Vector(float x, float y) :
	x(x), y(y)
	{
	}

	void normalize();
	Vector normalized() const;
};

void Vector::normalize()
{
	auto size = get_magnitude();

	if (size == 0){
	        return;

	}

	this->x /= size;
	this->y /= size;
}

Vector Vector::normalized() const
{
	Vector v = *this;
	v.normalize();
	return v;
}

static vectors vector_of_vectors;
std::vector<Vector> vector_of_sums;
std::mutex mt;

void normalize_list(uint32_t begin, uint32_t end)
{
	std::remove_pointer<decltype(vector_of_vectors)::value_type>::type summ = {0, 0};

	uint64_t count = 0;
	for (size_t i = 0; i < end - begin; ++i)
	{
                int index = begin + i;
                summ.x += vector_of_vectors[index]->normalized().x;
                summ.y += vector_of_vectors[index]->normalized().y;

                count++;

	}

	mt.lock();
	vector_of_sums.push_back(summ);

	mt.unlock();
}

Vector getSum()
{
	mt.lock();

	Vector result;
	for (Vector vector : vector_of_sums)
	{
		result = {result.x + vector.x, result.y + vector.y};
	}

	mt.unlock();

	result =
	{
		result.x / (float)vector_of_sums.size(),
		result.y / (float)vector_of_sums.size()
	};

	return result.normalized();
}

float get_angle_value()
{
	auto first = getSum();
	auto second = Vector(0, 0);

	return Vector::angle(&first, &second);
}

std::vector<std::thread>
getThreads()
{
	decltype(getThreads()) result;
	const uint8_t THREADS_COUNT = 10;
	for (int i = 0; i < THREADS_COUNT; i++)
	{
		float delta = vector_of_vectors.size() / THREADS_COUNT;
		int begin = i * delta;
		int finish = (i+1) * delta;
		result.push_back(std::thread(std::bind(normalize_list, begin, finish)));

	}

	return std::move(result);
}

int main()
{
	std::fstream f("./vectors", std::fstream::in);

	while (!f.eof() && !f.fail())
	{
		float a, b;

		f >> a >> b;

		vector_of_vectors.push_back(new Vector(a, b));
	}

	if (vector_of_vectors.size() == 0)
		return 0;

	auto threads = getThreads();

	for (std::vector<std::thread>::iterator i = threads.begin(); i != threads.end(); i++)	(*i).join();

	OUT << "angle:" << "\n";
	OUT << "\t" << get_angle_value() << std::endl;

	for (Vector * v : vector_of_vectors)
		delete v;

	return EXIT_SUCCESS;
}