# C++ Linq

###### C++ Linq Library compatible with C++98/03 (not use Modern C++)

```C++
#include <vector>
#include <boost/lambda/lambda.hpp>

#include "Linq.h"

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

int main()
{
	CppLinq::LinqTest test;
	test.PerformTest();
}
```

## Supported Features

* from clause
* where clause
* select clause
* group clause
* into
* orderby clause

## Not Supported Features

* join clause
* let clause
* ascending
* descending
* on
* equals
* by
* in

## Query

### from clause

###### Specifies a data source and a range variable (similar to an iteration variable)

### where clause

###### Filters source elements based on one or more Boolean expressions separated by logical AND and OR operators

### select clause

###### Specifies the type and shape that the elements in the returned sequence will have when the query is executed

### group clause

###### Groups query results according to a specified key value

### into

###### Provides an identifier that can serve as a reference to the results of a join, group or select clause

### orderby clause

###### Sorts query results in ascending or descending order based on the default comparer for the element type

## Dependencies

* Boost 1.61.0

## License

<img align="right" src="http://opensource.org/trademarks/opensource/OSI-Approved-License-100x137.png">

The class is licensed under the [MIT License](http://opensource.org/licenses/MIT):

Copyright &copy; 2016 [Chris Ohk](http://www.github.com/utiLForever)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
