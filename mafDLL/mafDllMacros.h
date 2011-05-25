#ifndef DllExportH
#define DllExportH

#if defined(_MSC_VER)

#ifdef _DEBUG
#  define class_it_base class
#else
#  define class_it_base struct
#endif

#define EXPORT_STL_VECTOR(declspec_, T_) \
	template class declspec_ std::allocator<T_ >; \
	template class declspec_ std::vector<T_ >;

#define EXPORT_STL_VECTOR_CONST_ITERATOR(declspec_, T_) \
	class_it_base declspec_ std::_Iterator_base; \
	template class declspec_ std::_Vector_const_iterator<T_,\
	std::allocator<T_ > >;

#define EXPORT_STL_DEQUE(declspec_, T_) \
	template class declspec_ std::allocator<T_ >; \
	template class declspec_ std::allocator<T_* >; \
	template class declspec_ std::deque<T_ >;

#define EXPORT_STL_LIST(declspec_, T_) \
	template class declspec_ std::allocator<T_ >; \
	template class declspec_ std::allocator<std::_List_nod<T_,\
	std::allocator<T_ > >::_Node>; \
	template class declspec_ std::allocator<std::_List_nod<T_,\
	std::allocator<T_ > >::_Node*>; \
	template class declspec_ std::list<T_ >;

#define EXPORT_STL_SET(declspec_, T_) \
	template class declspec_ std::allocator<T_ >; \
	template struct declspec_ std::less<T_ >; \
	template class declspec_ std::allocator<std::_Tree_nod< \
	std::_Tset_traits<T_, std::less<T_ >, std::allocator<T_ >, false> \
	>::_Node>; \
	template class declspec_ std::allocator<std::_Tree_ptr< \
	std::_Tset_traits<T_, std::less<T_ >, std::allocator<T_ >, false> \
	>::_Node*>; \
	template class declspec_ std::set<T_ >;

#define EXPORT_STL_MULTISET(declspec_, T_) \
	template class declspec_ std::allocator<T_ >; \
	template struct declspec_ std::less<T_ >; \
	template class declspec_ std::allocator<std::_Tree_nod< \
	std::_Tset_traits<T_, std::less<T_ >, std::allocator<T_ >, true> \
	>::_Node>; \
	template class declspec_ std::allocator<std::_Tree_ptr< \
	std::_Tset_traits<T_, std::less<T_ >, std::allocator<T_ >, true> \
	>::_Node*>; \
	template class declspec_ std::multiset<T_ >;

#define EXPORT_STL_MAP(declspec_, K_, V_) \
	template struct declspec_ std::less<K_ >; \
	template class declspec_ std::allocator<std::_Tree_nod< \
	std::_Tmap_traits<K_, V_, std::less<K_ >, \
	std::allocator<std::pair<const K_, V_ > >, false> >::_Node>; \
	template class declspec_ std::allocator<std::_Tree_nod< \
	std::_Tmap_traits<K_, V_, std::less<K_ >, \
	std::allocator<std::pair<const K_, V_ > >, false> >::_Node*>; \
	template class declspec_ std::allocator<std::pair<const K_, V_ > >; \
	template class declspec_ std::map<K_, V_ >;

#define EXPORT_STL_MAP_CONST_ITERATOR(declspec_, K_, V_) \
	class_it_base declspec_ std::_Iterator_base; \
	template class declspec_ std::_Tree<std::_Tmap_traits<K_, V_, \
	std::less<K_>, std::allocator<std::pair<const K_, V_ > >, false> \
	>::const_iterator;


#define EXPORT_STL_MULTIMAP(declspec_, K_, V_) \
	template struct declspec_ std::less<K_ >; \
	template class declspec_ std::allocator<std::_Tree_nod< \
	std::_Tmap_traits<K_, V_, std::less<K_ >, \
	std::allocator<std::pair<const K_, V_ > >, true> >::_Node>; \
	template class declspec_ std::allocator<std::_Tree_nod< \
	std::_Tmap_traits<K_, V_, std::less<K_ >, \
	std::allocator<std::pair<const K_, V_ > >, true> >::_Node*>; \
	template class declspec_ std::allocator<std::pair<const K_, V_ > >; \
	template class declspec_ std::multimap<K_, V_ >;

#else

#define EXPORT_STL_VECTOR(declspec_, T_)
#define EXPORT_STL_VECTOR_CONST_ITERATOR(declspec_, T_)
#define EXPORT_STL_DEQUE(declspec_, T_)
#define EXPORT_STL_LIST(declspec_, T_)
#define EXPORT_STL_SET(declspec_, T_)
#define EXPORT_STL_MULTISET(declspec_, T_)
#define EXPORT_STL_MAP(declspec_, K_, V_)
#define EXPORT_STL_MAP_CONST_ITERATOR(declspec_, K_, V_)
#define EXPORT_STL_MULTIMAP(declspec_, K_, V_)

#endif

#endif