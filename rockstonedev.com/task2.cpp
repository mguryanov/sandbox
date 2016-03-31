#include <math.h>
#include <thread>
#include <vector>
#include <mutex>
#include <iostream>
#include <fstream>

#define RADIANS_TO_DEGREES 180.f / PI_;
#define OUT cout

using namespace std;

class Vector;

typedef vector<Vector*> vectors;


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

        Vector() :
            x(0),
            y(0)
        {}

        Vector(float x, float y) :
            x(x),
            y(y)
        {}

        static auto angle(const Vector* const firstVector,
                          const Vector* const secondVector) -> long double
	{
		auto firstNormalized_vector = firstVector->normalized();
		auto secondNormalized_vector = secondVector->normalized();

		const Vector diff = firstNormalized_vector.sub(secondNormalized_vector);
		const Vector *diff_ptr = &diff;

		(const_cast<Vector*>(diff_ptr))->normalize();

		return atan2(diff.y, diff.x) *RADIANS_TO_DEGREES;
        }

        Vector sub(const Vector& other) const
	{
                Vector result;

                result.x = x - other.x;
                result.y = y - other.y;

		return result;
	}

	void normalize();
	Vector normalized() const;
};

void Vector::normalize()
{
	auto size = get_magnitude();

        if (size == 0)
            return;

        x /= size;
        y /= size;
}

Vector Vector::normalized() const
{
	Vector v = *this;
	v.normalize();
	return v;
}

static vectors vector_of_vectors;
vector<Vector> vector_of_sums;
mutex mt;

void normalize_list(uint32_t begin, uint32_t end)
{
        remove_pointer<decltype(vector_of_vectors)::value_type>::type summ = {0, 0};

	for (size_t i = 0; i < end - begin; ++i)
	{
                size_t index = begin + i;
                summ.x += vector_of_vectors[index]->normalized().x;
                summ.y += vector_of_vectors[index]->normalized().y;
	}

	mt.lock();
	vector_of_sums.push_back(summ);
	mt.unlock();
}

Vector getSum()
{
	Vector result;
	for (Vector vector : vector_of_sums)
	{
		result = {result.x + vector.x, result.y + vector.y};
	}

	result =
	{
                result.x / static_cast<float>(vector_of_sums.size()),
                result.y / static_cast<float>(vector_of_sums.size())
	};

	return result.normalized();
}

float get_angle_value()
{
	auto first = getSum();
	auto second = Vector(0, 0);

	return Vector::angle(&first, &second);
}

vector<thread>
getThreads()
{
	decltype(getThreads()) result;
	const uint8_t THREADS_COUNT = 10;

        uint last = 0;
        uint delta = 1;
        uint tasks_per_thread_count = vector_of_vectors.size();

        if( vector_of_vectors.size() > THREADS_COUNT ) {
            delta = vector_of_vectors.size() / ( THREADS_COUNT - 1 );
            last = vector_of_vectors.size() % ( THREADS_COUNT - 1 );
            tasks_per_thread_count = THREADS_COUNT;
        }

        for (uint i = 0, begin=0, finish=0; i < tasks_per_thread_count; )
	{
                begin = i * delta;

                if( ++i != tasks_per_thread_count ) {
                    finish = begin + delta;
                }

                else {
                    finish = begin + last;
                }

                result.push_back(thread(bind(normalize_list, begin, finish)));
	}

        return move(result);
}

int main()
{
        fstream f("./vectors", fstream::in);

	while (!f.eof() && !f.fail())
	{
		float a, b;
		f >> a >> b;
		vector_of_vectors.push_back(new Vector(a, b));
	}

        if (vector_of_vectors.empty())
		return 0;

	auto threads = getThreads();

        for (vector<thread>::iterator i = threads.begin(); i != threads.end(); ++i)
            (*i).join();

	OUT << "angle:" << "\n";
        OUT << "\t" << get_angle_value() << endl;

	for (Vector * v : vector_of_vectors)
            if( v )
		delete v;

	return EXIT_SUCCESS;
}
