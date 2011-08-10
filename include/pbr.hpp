// Copyright (c) 2011 Andy Jost
// Please see the file LICENSE.txt in this distribution for license terms.

#define BOOST_RANGE_ENABLE_CONCEPT_ASSERT 0

#include <boost/python.hpp>
#include <boost/python/object_core.hpp>
#include <boost/python/stl_iterator.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/iterator/iterator_adaptor.hpp>
#include <algorithm>
#include <utility>

namespace pbr { namespace aux
{
  using namespace boost::python;
  using boost::python::ssize_t;

  // object_item is like a reference to an object.  It requires a slot in a
  // mutable sequence, such as a list.
  using api::object_item;

  struct range_base
  {
    range_base(object obj)
      : m_obj(obj)
    {
    }
    object m_obj;
  };

  // Check that Python can fulfill the specified range concept.
  template<typename TraversalTag> inline bool check(range_base const &)
  {
    return false;
  }

  template<>
  inline bool
  check<boost::incrementable_traversal_tag>(range_base const & base)
  {
    PyObject * iter = PyObject_GetIter(base.m_obj.ptr());
    bool const result = iter != Py_None;
    Py_XDECREF(iter);
    return result;
  }
  template<>
  inline bool
  check<boost::random_access_traversal_tag>(range_base const & base)
  {
    return PySequence_Check(base.m_obj.ptr());
  }

  // Iterators.
  template<typename TraversalTag, typename Value> class iterator {};

  // Incrementable
  // Due to the way Python works, incrementable ranges are always constant;
  // TODO: concept check -- Value is an rvalue
  template<typename Value>
  struct iterator<boost::incrementable_traversal_tag, Value>
    : boost::iterator_adaptor<
          iterator<boost::incrementable_traversal_tag, Value>
        , stl_input_iterator<Value>
        , Value
        , boost::incrementable_traversal_tag
        , Value
        >
  {
    iterator()
      : iterator::iterator_adaptor_()
    {
    }
    iterator(range_base const & range, bool end)
      : iterator::iterator_adaptor_(
            end
                ? stl_input_iterator<Value>()
                : stl_input_iterator<Value>(range.m_obj)
          )
    {
    }
  };

  // RandomAccess
  template<typename Value>
  struct iterator<boost::random_access_traversal_tag, Value>
    : boost::iterator_facade<
          iterator<boost::random_access_traversal_tag, Value>
        , Value
        , boost::random_access_traversal_tag
        , Value
        >
  {
    iterator()
      : iterator::iterator_facade_(), m_obj(), m_loc(0)
    {
    }
    iterator(range_base const & range, bool end)
      : iterator::iterator_facade_(), m_obj(range.m_obj)
      , m_loc(end ? len(m_obj) : 0)
    {
    }
  private:
    // facade interface
    friend class boost::iterator_core_access;
    typename iterator::iterator_facade_::reference
      dereference() const { return extract<Value>(m_obj[m_loc]); }
    template<typename Y> bool equal(Y y) const { return m_loc == y.m_loc; }
    void increment() { ++m_loc; }
    void decrement() { --m_loc; }
    template<typename N> void advance(N n) { m_loc += n; }
    template<typename Z>
      typename iterator::iterator_facade_::difference_type
      distance_to(Z z) const { return z.m_loc - m_loc; }

    // data
    object m_obj;
    ssize_t m_loc;
  };

  // Specialization of iterator::dereference() for Value=object_item.  An
  // iterator over object_items can return the item proxy, even if the iterator
  // is const.
  template<>
  iterator<boost::random_access_traversal_tag, object_item>::iterator_facade_::reference
  iterator<boost::random_access_traversal_tag, object_item>::dereference() const
  { return (const_cast<object&>(m_obj))[m_loc]; }

  // Mapping
  //
  // Python fuses increment and dereference for mapping types.  That means
  // there is no way to iterate over a mapping sequence without actually
  // generating the items.  Contrast this with iteration in C++, where a value
  // is not actually produced unless the iterator is dereferenced.
  //
  // This class pre-increments the Python iterator in the constructor and store
  // its return value.  Dereferencing simply returns the stored value.  When
  // the end of the sequence is reached, the iterator is converted into an end
  // iterator.
  template<typename Key, typename Mapped>
  struct mapping_iterator
    : boost::iterator_facade<
          mapping_iterator<Key, Mapped> // Derived
        , std::pair<Key, Mapped> const  // Value
        , boost::incrementable_traversal_tag // Category
        , std::pair<Key, Mapped> const  // Reference
        >
  {
    typedef Key key_type;
    typedef Mapped mapped_type;
    typedef std::pair<Key, Mapped> value_type;

    mapping_iterator()
      : mapping_iterator::iterator_facade_(), m_iteritems(), m_pos()
    {
    }
    // This iterator stores an iterator for the map in m_obj, then returns the
    // key/value pairs.
    mapping_iterator(range_base const & range, bool end)
      : mapping_iterator::iterator_facade_()
      , m_iteritems(range.m_obj.attr("iteritems")())
      , m_pos()
    {
      // Pre-increment to get the first item.
      if(!end)
        this->increment();
    }
  private:
    // facade interface
    friend class boost::iterator_core_access;
    typename mapping_iterator::iterator_facade_::reference
    dereference() const
    {
      Key key = extract<Key>(m_pos[0]);
      Mapped item = extract<Mapped>(m_pos[1]);
      return std::make_pair(key, item);
    }
    void increment()
    {
      try { m_pos = call_method<tuple>(m_iteritems.ptr(), "next"); }
      catch(error_already_set const &)
      {
        // On stop iteration, make this into an end iterator.
        if(PyErr_ExceptionMatches(PyExc_StopIteration))
        {
          *this = mapping_iterator();
          PyErr_Clear();
        }
        else
          throw;
      }
    }
    template<typename Y>
      bool equal(Y y) const { return m_pos == y.m_pos; }

    // data
    object m_iteritems; // iterator used to observe the Python mappable
    tuple m_pos; // key/value pair
  };
}}

namespace pbr
{
  struct bad_range
    : std::exception
  {
    bad_range(std::string const & msg = "<no context>")
      : m_msg(msg)
    {
    }
    virtual ~bad_range() throw() {}
    virtual char const * what() const throw() { return this->m_msg.c_str(); }
  private:
    std::string m_msg;
  };

  #define PBR_define_range_class(name, traversal)            \
    template<typename Value = aux::object>                   \
    class name                                               \
      : aux::range_base                                      \
    {                                                        \
      typedef boost::traversal##_traversal_tag tag;          \
    public:                                                  \
      typedef Value value_type;                              \
      name(aux::object const & obj)                          \
        : aux::range_base(obj)                               \
      {                                                      \
        if(!aux::check<tag>(*this)) throw bad_range(#name);  \
      }                                                      \
      typedef aux::iterator<tag, Value> iterator;            \
      typedef iterator const_iterator;                       \
      const_iterator begin() const                           \
      {                                                      \
        return const_iterator(this->m_obj, false);           \
      }                                                      \
      const_iterator end() const                             \
      {                                                      \
        return const_iterator(this->m_obj, true);            \
      }                                                      \
    }

  // --- incrementable_range ---
  PBR_define_range_class(incrementable_range, incrementable);
  // --- random_access_range ---
  PBR_define_range_class(random_access_range, random_access);

  // --- mutable_random_access_range ---
  // The Value type may not be specified.  It is always a mutable reference to
  // a Python object.
  PBR_define_range_class(mutable_random_access_range_impl, random_access);

  using aux::object_item;
  typedef mutable_random_access_range_impl<object_item> mutable_random_access_range;

  #undef PBR_define_range_class

  // --- mapping_range ---
  template<typename Key = aux::object, typename Mapped = aux::object>
  class mapping_range
    : aux::range_base
  {
    typedef boost::incrementable_traversal_tag tag;
  public:
    typedef Key key_type;
    typedef Mapped mapped_type;
    typedef std::pair<Key, Mapped> value_type;
    typedef aux::mapping_iterator<Key, Mapped> iterator;
    typedef iterator const_iterator;
    mapping_range(aux::object const & obj)
      : aux::range_base(obj)
    {
      if(!aux::check<tag>(*this) || ! PyMapping_Check(m_obj.ptr()))
        throw bad_range("mapping_range");
    }
    const_iterator begin() const
    {
      return const_iterator(this->m_obj, false);
    }
    const_iterator end() const
    {
      return const_iterator(this->m_obj, true);
    }
  };
}

// Overload std::swap and std::iter_swap for Python objects.
namespace std
{
  #define PBR_item boost::python::api::object_item
  #define PBR_object boost::python::api::object

  template<> void swap<PBR_item>(PBR_item & a, PBR_item & b)
  {
    PBR_object tmp0(a);
    a = b;
    b = tmp0;
  }

  // This is needed to work around STL implementations that use iter_swap to
  // implement algorithms such as random_shuffle.

  #define PBR_iterator pbr::aux::iterator<         \
      boost::random_access_traversal_tag, PBR_item \
    >

  template<>
  void iter_swap<PBR_iterator, PBR_iterator>(PBR_iterator a, PBR_iterator b)
  {
    PBR_item a_(*a), b_(*b);
    swap(a_, b_);
  }

  #undef PBR_iterator
  #undef PBR_object
  #undef PBR_item
}

