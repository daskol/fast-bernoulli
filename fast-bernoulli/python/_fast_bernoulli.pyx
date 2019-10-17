#   encoding: utf8
#   filename: _fast_bernoulli.pyx

from cython.operator cimport dereference as deref

from libcpp cimport bool
from libcpp.vector cimport vector


cdef extern from '<fast-bernoulli/fast-bernoulli.h>' namespace 'NFastBernoulli':

    cdef cppclass TRng:
        TRng(size_t seed)

    cdef cppclass TAlignedPtr:
        pass

    cdef cppclass TSamplerPtr:
        bool operator()(TRng &rng, TAlignedPtr &ptr)
        TAlignedPtr MakeBuffer(size_t nobits)

    cdef cppclass TSamplerOpts:
        double Probability_
        double Tolerance_
        bool UseJit_
        bool UseStdSampler_

    TSamplerPtr CreateSampler(const TSamplerOpts &opts)
    TSamplerPtr CreateSampler(double prob, double tol)
    vector[bool] Expand(const TAlignedPtr &ptr)


def sample(double prob, int nobits, double tol = 1e-3, int seed = 0):
    cdef TRng *rng = new TRng(seed)
    cdef TSamplerPtr sampler = CreateSampler(prob, tol)
    cdef TAlignedPtr ptr = sampler.MakeBuffer(nobits)
    cdef bool status = sampler(deref(rng), ptr)

    if not status:
        raise RuntimeError('Sampler is failed to draw instances.')

    cdef vector[bool] result = Expand(ptr)
    return result
