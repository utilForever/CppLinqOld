#include <vector>

#include <boost/lambda/lambda.hpp>

#include "Linq.h"
#include <iostream>

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

		void PerformTest()
		{
			assert(from(m_guests).count() == 3);

			assert(from(m_guests).orderby<int>(&_1 ->* &Person::m_age, Descending).count() == 3);
			for (int i = 0; i < m_guests->size(); ++i)
			{
				cout << m_guests->at(i).m_name << ' ' << m_guests->at(i).m_age << endl;
			}
			
			assert(from(m_guests).where(&_1 ->* &Person::m_age > 30).count() == 1);
			
			assert(from(m_guests).select<int>(&_1 ->* &Person::m_age).count() == 3);
			shared_ptr<vector<int>> results = from(m_guests).select<int>(&_1 ->* &Person::m_age).get();
			assert(results->size() == 3);
			assert((*results)[0] == 32);

			assert(from(m_guests).group<int>(&_1 ->* &Person::m_age).count() == 3);

			vector<Person> people;
			people.push_back(Person("Joe", 20));
			assert(from(&people).group<int>(&_1 ->* &Person::m_age).count() == 1);

			m_guests->push_back(Person("Joe", 18));
			DataSet<vector<Person>> results2 = insert(from(m_guests).where(&_1 ->* &Person::m_age > 30)).into(from(m_guests).where(&_1 ->* &Person::m_name == "Joe"));
			assert(results2.count() == 2);
			shared_ptr<vector<int>> ages = results2.select<int>(&_1 ->* &Person::m_age).get();
			assert(count(ages->begin(), ages->end(), 32) == 1);
			assert(count(ages->begin(), ages->end(), 18) == 1);
		}

	private:
		shared_ptr<vector<Person>> m_guests;
	};
}

int main()
{
	CppLinq::LinqTest test;
	test.PerformTest();
}