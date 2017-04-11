datapar<T, datapar_abi::native> d; 

double* p;
datapar<double, datapar_abi::avx512> d; 

d.memload(p);  // d == p[0:8]
d.memstore(p); // p[0:8] == d

d.scatter(idx, val);

mdspan s;
d.memstore(s);

// Scatter/gather could use the same interface as VIS.

//////////////////////////////////////////////////////////////

template <typename T>
future<> global_ptr<T>::put(T const* p);

template <typename T>
future<> global_ptr<T>::put(T p);

template <typename T>
future<T> global_ptr<T>::get() const;

