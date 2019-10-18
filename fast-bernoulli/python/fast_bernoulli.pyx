#   encoding: utf8
#   filename: fast_bernoulli.pyx
"""Module fast_bernoulli provides bindings to native implementation of
Bernoulli sampler.
"""

from cpython cimport Py_buffer
from cython.operator cimport dereference as deref

from libc.stdint cimport uint8_t

from libcpp cimport bool
from libcpp.vector cimport vector


cdef extern from '<fast-bernoulli/fast-bernoulli.h>' namespace 'NFastBernoulli':

    cdef cppclass TRng:
        TRng(size_t seed)

    cdef enum EInstructionSet:
        EInstructionSet_Auto 'NFastBernoulli::EInstructionSet::Auto'
        EInstructionSet_General 'NFastBernoulli::EInstructionSet::General'
        EInstructionSet_AVX 'NFastBernoulli::EInstructionSet::AVX'
        EInstructionSet_MMX 'NFastBernoulli::EInstructionSet::MMX'
        EInstructionSet_SSE 'NFastBernoulli::EInstructionSet::SSE'

    cdef cppclass TAlignedPtr:
        pass

    cdef cppclass TSamplerPtr:
        bool operator bool()
        bool operator()(TRng &rng, TAlignedPtr &ptr)
        TAlignedPtr MakeBuffer(size_t nobits)

    cdef cppclass TSamplerOpts:
        double Probability_
        double Tolerance_
        EInstructionSet Ise_
        bool UseJit_
        bool UseStdSampler_

    TSamplerPtr CreateSampler(const TSamplerOpts &opts)

    vector[T] Expand[T](const TAlignedPtr &ptr, size_t nobits)


cdef class TExpandedPtr:
    """Class TExpandedPtr wraps native C++ structures (like vector[T]) in order
    to provide memoryview intererface on expanded bit blocks.
    """

    cdef Py_ssize_t shape[1]
    cdef Py_ssize_t strides[1]
    cdef vector[uint8_t] bits

    def __cinit__(self, size_t nobits):
        self.shape[0] = nobits
        self.strides[0] = 1

    def __getbuffer__(self, Py_buffer *buffer, int flags):
        buffer.buf = self.bits.data();
        buffer.format = 'b'
        buffer.internal = NULL
        buffer.itemsize = sizeof(uint8_t)
        buffer.len = self.bits.size() * buffer.itemsize
        buffer.ndim = 1
        buffer.obj = self
        buffer.readonly = 1
        buffer.shape = self.shape
        buffer.strides = self.strides
        buffer.suboffsets = NULL

    def __releasebuffer__(self, Py_buffer *buffer):
        pass

    cdef reset(self, TAlignedPtr &ptr):
        self.bits = Expand[uint8_t](ptr, self.shape[0])
        return self


__all__ = (
    'InstructionSet',
    'sample',
)


cpdef enum InstructionSet:
    Auto = <uint8_t> EInstructionSet_Auto
    General = <uint8_t> EInstructionSet_General
    AVX = <uint8_t> EInstructionSet_AVX
    MMX = <uint8_t> EInstructionSet_MMX
    SSE = <uint8_t> EInstructionSet_SSE


def sample(double prob, size_t nobits, double tol = 1e-3, unsigned seed = 0,
           ise = InstructionSet.Auto,
           bool jit = False, bool std = False):
    """Function sample provides pythonic way to access native sampler.

    :param prob: Parameter of Bernoulli distribution.
    :param nobits: Number of bits to produce.
    :param tol: Maximal approximation error for distribution parameter.
    :param seed: Seed for (pseudo) random number generator.
    :param ise: Instruction set extension to use in sampling.
    :param jit: Use JIT compiler to get boost in performance (require LLVM).
    :param std: Use sampler from C++ standard library.
    :return: Memory view to buffer which contains random bits.
    """
    cdef TRng *rng = new TRng(seed)
    cdef TSamplerOpts opts

    opts.Probability_ = prob
    opts.Tolerance_ = tol
    opts.Ise_ = <EInstructionSet> (<uint8_t> ise)
    opts.UseJit_ = jit
    opts.UseStdSampler_ = std

    cdef TSamplerPtr sampler = CreateSampler(opts)

    if not <bool> sampler:
        raise RuntimeError('Failed to create an instance of ISampler.')

    cdef TAlignedPtr ptr = sampler.MakeBuffer(nobits)

    if not <bool> ptr:
        raise RuntimeError('Failed to allocate aligned buffer.')

    if <bool> sampler(deref(rng), ptr):
        raise RuntimeError('Sampler is failed to draw instances.')

    cdef TExpandedPtr expansion = TExpandedPtr(nobits)
    return expansion.reset(ptr);
