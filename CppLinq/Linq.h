#ifndef CPP_LINQ_H
#define CPP_LINQ_H

#include <vector>

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

namespace CppLinq
{
	using std::vector;
	using boost::shared_ptr;
	using boost::function;

	enum MutationMode
	{
		IMMUTABLE,	// Leave the old container alone and create new ones
		INPLACE,	// Modify the container in place
	};

	template <typename T, typename TPtr = shared_ptr<T>>
	class DataSet
	{
	public:
		typedef T underlyingContainerType;
		
		DataSet(TPtr);

		TPtr get();

		unsigned int count();
		DataSet<T> where(function<bool(typename T::value_type)> pred);

		template <typename S>
		DataSet<vector<S>> select(function<S(typename T::value_type)> sel);

		template <typename S>
		DataSet<vector<S>> group(function<S(typename T::value_type)> sel)
		{
			return select<S>(sel);
		}

	private:
		TPtr m_container;
	};

	template <typename T, typename TPtr>
	DataSet<T, TPtr>::DataSet(TPtr container) :
		m_container(container)
	{
		
	}

	template <typename T, typename TPtr>
	TPtr DataSet<T, TPtr>::get()
	{
		return m_container;
	}

	template <typename T, typename TPtr>
	unsigned int DataSet<T, TPtr>::count()
	{
		return m_container->size();
	}

	template <typename T, typename TPtr>
	DataSet<T> DataSet<T, TPtr>::where(function<bool(typename T::value_type)> pred)
	{
		shared_ptr<T> newContainer(new T());

		for (typename T::const_iterator iter = m_container->begin(); iter != m_container->end(); ++iter)
		{
			if (pred(*iter))
			{
				newContainer->push_back(*iter);
			}
		}

		return DataSet<T>(newContainer);
	}

	template <typename T, typename TPtr>
	template <typename S>
	DataSet<vector<S>> DataSet<T, TPtr>::select(function<S(typename T::value_type)> sel)
	{
		shared_ptr<vector<S>> newContainer(new vector<S>());

		for (typename T::const_iterator iter = m_container->begin(); iter != m_container->end(); ++iter)
		{
			newContainer->push_back(sel(*iter));
		}

		return DataSet<vector<S>>(newContainer);
	}

	template <typename T>
	DataSet<T> from(shared_ptr<T> source)
	{
		return DataSet<T>(source);
	}

	template <typename T>
	DataSet<T, T*> from(T* source)
	{
		return DataSet<T, T*>(source);
	}

	template <typename T>
	class Inserter
	{
	public:
		Inserter(const T& dataSet) :
			m_dataSet(dataSet)
		{
			
		}

		template <typename S>
		S into(S target, MutationMode mode = IMMUTABLE);

	private:
		T m_dataSet;
	};

	template <typename T>
	template <typename S>
	S Inserter<T>::into(S target, MutationMode mode)
	{
		shared_ptr<typename S::underlyingContainerType> targetContainer;
		shared_ptr<typename T::underlyingContainerType> sourceContainer = m_dataSet.get();

		if (mode == IMMUTABLE)
		{
			targetContainer.reset(new typename S::underlyingContainerType);
			*targetContainer = *(target.get());
		}
		else
		{
			targetContainer = target.get();
		}

		typename T::underlyingContainerType::const_iterator iter;
		for (iter = sourceContainer->begin(); iter != sourceContainer->end(); ++iter)
		{
			targetContainer->push_back(*iter);
		}

		return from(targetContainer);
	}

	template <typename C>
	Inserter<DataSet<C>> insert(DataSet<C> dataSet)
	{
		return Inserter<DataSet<C>>(dataSet);
	}
}

#endif