// Copyright (c) 2011 Andy Jost
// Please see the file LICENSE.txt in this distribution for license terms.

// Must be included first!
#include <Python.h>

#include <boost/python.hpp>
#include <boost/python/object_core.hpp>
#include <boost/python/stl_iterator.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/iterator/iterator_adaptor.hpp>
#include <algorithm>

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
  template<typename TraversalTag> bool check(range_base const &);

  template<>
  bool check<boost::incrementable_traversal_tag>(range_base const & base)
  {
    object attr = getattr(base.m_obj, "__iter__", object());
    return ! attr.is_none();
  }
  template<>
  bool check<boost::random_access_traversal_tag>(range_base const & base)
  {
    return PySequence_Check(base.m_obj.ptr());
  }

  // Iterators.
  template<typename TraversalTag, typename Value> class iterator {};
  template<typename TraversalTag, typename Value> class const_iterator {};

  // Incrementable
  // Due to the way Python works, incrementable ranges are always constant;
  // TODO: concept check -- Value is an rvalue
  template<typename Value>
  struct const_iterator<boost::incrementable_traversal_tag, Value>
    : boost::iterator_adaptor<
          const_iterator<boost::incrementable_traversal_tag, Value>
        , stl_input_iterator<Value>
        , Value
        , boost::incrementable_traversal_tag
        , Value
        >
  {
    const_iterator()
      : const_iterator::iterator_adaptor_()
    {
    }
    const_iterator(range_base const & range, bool end)
      : const_iterator::iterator_adaptor_(
            end
                ? stl_input_iterator<Value>()
                : stl_input_iterator<Value>(range.m_obj)
          )
    {
    }
  };

  // RandomAccess
  template<typename Value>
  struct const_iterator<boost::random_access_traversal_tag, Value>
    : boost::iterator_facade<
          const_iterator<boost::random_access_traversal_tag, Value>
        , Value
        , boost::random_access_traversal_tag
        , Value
        >
  {
    const_iterator()
      : const_iterator::iterator_facade_(), m_obj(), m_loc(0)
    {
    }
    const_iterator(range_base const & range, bool end)
      : const_iterator::iterator_facade_(), m_obj(range.m_obj)
      , m_loc(end ? len(m_obj) : 0)
    {
    }
  private:
    // facade interface
    friend class boost::iterator_core_access;
    typename const_iterator::iterator_facade_::reference
      dereference() const { return extract<Value>(m_obj[m_loc]); }
    template<typename Y>
      bool equal(Y y) const { return m_loc == y.m_loc && m_obj == y.m_obj; }
    void increment() { ++m_loc; }
    void decrement() { --m_loc; }
    template<typename N> void advance(N n) { m_loc += n; }
    template<typename Z>
      typename const_iterator::iterator_facade_::difference_type
      distance_to(Z z) const { return z.m_loc - m_loc; }

    // data
    object m_obj;
    ssize_t m_loc;
  };

  // Mutable RandomAccess
  template<>
  struct iterator<boost::random_access_traversal_tag, object_item>
    : boost::iterator_facade<
          iterator<boost::random_access_traversal_tag, object_item>
        , object_item
        , boost::random_access_traversal_tag
        , object_item
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
    iterator::iterator_facade_::reference
      dereference() const { return (const_cast<object &>(m_obj))[m_loc]; }
    template<typename Y>
      bool equal(Y y) const { return m_loc == y.m_loc && m_obj == y.m_obj; }
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

  #define PBR_define_range_class(article, prefix, basename, itertype)          \
    template<typename Value = aux::object>                                     \
    class prefix##basename##_range                                             \
      : aux::range_base                                                        \
    {                                                                          \
      typedef boost::basename##_traversal_tag tag;                             \
    public:                                                                    \
      prefix##basename##_range(aux::object const & obj)                        \
        : aux::range_base(obj)                                                 \
      {                                                                        \
        if(!aux::check<tag>(*this))                                            \
          throw bad_range("cannot form " #article " " #basename " range");     \
      }                                                                        \
      typedef itertype<tag, Value> iterator;                                   \
      typedef iterator const_iterator;                                         \
      const_iterator begin() const                                             \
      {                                                                        \
        return const_iterator(this->m_obj, false);                             \
      }                                                                        \
      const_iterator end() const                                               \
      {                                                                        \
        return const_iterator(this->m_obj, true);                              \
      }                                                                        \
    }

  // --- incrementable_range ---
  PBR_define_range_class(an,, incrementable, aux::const_iterator);
  // --- random_access_range ---
  PBR_define_range_class(a,, random_access, aux::const_iterator);

  // --- mutable_random_access_range ---
  // The Value type may not be specified.  It is always a mutable reference to
  // a Python object.
  PBR_define_range_class(a, _mutable_, random_access, aux::iterator);

  // Make object_item and the one and only version of this range available.
  using aux::object_item;
  typedef _mutable_random_access_range<object_item> mutable_random_access_range;

  #undef PBR_define_range_class
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

