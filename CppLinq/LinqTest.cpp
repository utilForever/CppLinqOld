#include <vector>
#include <boost/lambda/lambda.hpp>

#include "Linq.h"

namespace CppLinq
{
	using namespace std;
	using boost::lambda::_1;

	struct Person
	{
		Person(const string& name, int age) :
			m_name(name), m_age(age)
		{

		}

		string m_name;
		int m_age;
	};

	class LinqTest
	{
	public:
		LinqTest()
		{
			m_guests.reset(new vector<Person>());

			m_guests->push_back(Person("John", 32));
			m_guests->push_back(Person("Mike", 28));
			m_guests->push_back(Person("Eliz", 27));
		}

		void PerformTest() const
		{
			PerformFromTest();
			PerformWhereTest();
			PerformSelectTest();
			PerformGroupTest();
			PerformIntoTest();
			PerformOrderbyTest();
		}

	private:
		void PerformFromTest() const
		{
			assert(from(m_guests).count() == 3);
		}

		void PerformWhereTest() const
		{
			assert(from(m_guests).where(&_1 ->* &Person::m_age > 30).count() == 1);
		}

		void PerformSelectTest() const
		{
			assert(from(m_guests).select<int>(&_1 ->* &Person::m_age).count() == 3);

			shared_ptr<vector<int>> results = from(m_guests).select<int>(&_1 ->* &Person::m_age).get();
			assert(results->size() == 3);
			assert((*results)[0] == 32);
		}

		void PerformGroupTest() const
		{
			assert(from(m_guests).group<int>(&_1 ->* &Person::m_age).count() == 3);

			vector<Person> people;
			people.push_back(Person("Joe", 20));
			assert(from(&people).group<int>(&_1 ->* &Person::m_age).count() == 1);
		}

		void PerformIntoTest() const
		{
			m_guests->push_back(Person("Joe", 18));

			DataSet<vector<Person>> results = insert(from(m_guests).where(&_1 ->* &Person::m_age > 30))
				.into(from(m_guests).where(&_1 ->* &Person::m_name == "Joe"));
			assert(results.count() == 2);

			shared_ptr<vector<int>> ages = results.select<int>(&_1 ->* &Person::m_age).get();
			assert(count(ages->begin(), ages->end(), 32) == 1);
			assert(count(ages->begin(), ages->end(), 18) == 1);
		}

		void PerformOrderbyTest() const
		{
			assert(from(m_guests).orderby<int>(&_1 ->* &Person::m_age).count() == 4);
			assert(from(m_guests).orderby<int>(&_1 ->* &Person::m_age, Ascending).count() == 4);
			assert(from(m_guests).orderby<int>(&_1 ->* &Person::m_age, Descending).count() == 4);
		}

		shared_ptr<vector<Person>> m_guests;
	};
}

int main()
{
	CppLinq::LinqTest test;
	test.PerformTest();
}