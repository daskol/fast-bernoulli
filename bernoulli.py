#!/usr/bin/env python
#   filename: bernoulli.h
#   encoding: utf8

import llvmlite
import llvmlite.binding as llvm
import llvmlite.ir as ir
import logging

from argparse import ArgumentParser
from ctypes import CFUNCTYPE, c_ulong, c_long
from enum import Enum
from random import randint
from typing import List


class Op(Enum):

    AND = 0
    OR = 1
    NOT = 2


def quantize_probability(prob, tol):
    lhs, rhs = 0.0, 1.0
    num = 0

    for base in range(0, 65):
        if prob - lhs <= tol and rhs - prob <= tol:
            return num, base

        mid = lhs + 0.5 * (rhs - lhs)

        if mid == prob:
            return 2 * num + 1, base + 1
        elif prob > mid:
            lhs = mid
            num = num * 2 + 1
        elif prob < mid:
            rhs = mid
            num = num * 2

    raise RuntimeError('Failed to find appropriate base.')


def build_op_sequence(numerator, base):
    ops = []
    while base != 1:
        if numerator > 2 ** (base - 1):
            numerator = 2 ** base - numerator
            ops.append(Op.NOT)

        ops.append(Op.AND)
        base -= 1
    return ops[::-1]


class Ops:

    def __init__(self, ops: List[Op]):
        self.ops = ops

    def __call__(self, bits: List[int]) -> int:
        bit = bits[0]
        bit_index = 1
        op_index = 0
        while len(self.ops[op_index:]) != 0:
            op = self.ops[op_index]
            op_index += 1
            if op == Op.NOT:
                bit = 1 - bit
            elif op == Op.AND:
                bit = bit & bits[bit_index]
                bit_index += 1
        return bit

    def __str__(self) -> str:
        out = 'b0'
        depth = 0
        for op in self.ops:
            if depth != 0:
                out = f'({out})'

            if op == Op.NOT:
                out = f'not {out}'
            elif op == Op.AND:
                depth += 1
                out = f'b{depth} and {out}'
        return out


class PyBernoulliGenerator:

    def __init__(self, probability: float, tolerance: float, seed: int = None):
        self.prob = probability
        self.tol = tolerance
        self.num, self.base = quantize_probability(self.prob, self.tol)
        self.ops = Ops(build_op_sequence(self.num, self.base))

    def __call__(self, nobits: int = 32):
        return [self.ops(bits=[randint(0, 1) for _ in range(nobits)])
                for _ in range(32)]


def init_execution_engine():
    target = llvm.Target.from_default_triple()
    target_machine = target.create_target_machine()
    backing_mod = llvm.parse_assembly('')
    engine = llvm.create_mcjit_compiler(backing_mod, target_machine)
    return engine


def compile_ir(engine, llvm_ir):
    mod = llvm.parse_assembly(llvm_ir)
    mod.verify()
    engine.add_module(mod)
    engine.finalize_object()
    engine.run_static_constructors()
    return mod


def generate_ir(name, base, ops):
    uint64_t = ir.IntType(64)
    func_t= ir.FunctionType(uint64_t, [uint64_t] * base)
    module = ir.Module(name=f'{name}.py')
    func = ir.Function(module, func_t, name=name)

    block = func.append_basic_block()
    builder = ir.IRBuilder(block)
    bits = func.args
    bit = bits[0]
    bit_index = 1
    op_index = 0
    while len(ops[op_index:]) != 0:
        op = ops[op_index]
        op_index += 1
        if op == Op.NOT:
            bit = builder.not_(bit)
        elif op == Op.AND:
            bit = builder.and_(bit, bits[bit_index])
            bit_index += 1

    builder.ret(bit)
    return module


class LLVMBernoulliGenerator(PyBernoulliGenerator):

    UINT64_MAX = 0xffffffff

    def __init__(self, probability: float, tolerance: float, seed: int = None):
        super().__init__(probability, tolerance, seed)

        self.name = f'bernoulli_{self.base:02d}bits_{hex(self.num)}'
        self.code = generate_ir(self.name, self.base, self.ops.ops)
        logging.info('module %s:\n%s', self.name, self.code)

        llvm.initialize()
        llvm.initialize_native_target()
        llvm.initialize_native_asmprinter()

        self.engine = init_execution_engine()
        self.mod = compile_ir(self.engine, str(self.code))
        self.func_ptr = self.engine.get_function_address(self.name)
        self.cfunc = CFUNCTYPE(c_long, *([c_long] * self.base))(self.func_ptr)

    def __call__(self, nobits: int = 32):
        bits = [randint(0, self.UINT64_MAX) for _ in range(nobits)]
        word = self.cfunc(*bits)
        return [(word & (1 << i)) >> i for i in range(len(bits))]


def main(probability: float, tolerance: float):
    logging.basicConfig(format='%(asctime)s : %(levelname)s : %(message)s',
                        level=logging.INFO)

    num, base = quantize_probability(probability, tolerance)
    logging.info('num = %d; base = %d; prob = %f', num, base, num / 2 ** base)

    ops = build_op_sequence(num, base)
    logging.info('ops = %s', Ops(ops))

    logging.info('combine all together in py')
    gen = PyBernoulliGenerator(probability, tolerance)
    samples = [gen() for _ in range(10)]

    logging.info('samples = %s', samples[0])
    logging.info('sum = %d; total = 32; expect = %f',
                 sum(sum(seq) for seq in samples),
                 sum(sum(seq) for seq in samples) / 320)

    logging.info('combine all together with LLVM')
    gen = LLVMBernoulliGenerator(probability, tolerance)
    samples = [gen() for _ in range(10)]

    logging.info('samples = %s', samples[0])
    logging.info('sum = %.6f; total = 32; expect = %f',
                 sum(sum(seq) for seq in samples),
                 sum(sum(seq) for seq in samples) / 320)
    logging.info('done.')


if __name__ == '__main__':
    argparser = ArgumentParser()
    argparser.add_argument('-p', '--probability',
                           default=0.5,
                           help='Parameter of Bernoulli distribution.')
    argparser.add_argument('-t', '--tolerance',
                           default=1e-6,
                           help='Quantization error of Bernoulli parameter.')
    args = argparser.parse_args()
    main(float(args.probability), float(args.tolerance))
