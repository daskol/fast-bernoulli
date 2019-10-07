/**
 * \file executor_testdata.inc
 */

#include <array>

alignas(32) static std::array<uint64_t, 4> dst;

alignas(32) static std::array<uint64_t, 4> res{
    0xad9feab5f39ad7c8ul, 0xadafb6267b9e3fdful, 0xbbe096efeba19652ul,
    0xbb5f6c4a9dfb6f6dul,
};

alignas(32) static std::array<uint64_t, 64> src{
    0x4e2c0b87bdc42752ul, 0x2686249db756a007ul, 0x234c116ce9e78323ul,
    0x999a6732615af95ful, 0x0bb8707c23cd53b7ul, 0x9fc5a8bba173962cul,
    0xa3433e2ff992b2b9ul, 0x8f46d6d4da3c1015ul, 0x12d16ae0ed373b9dul,
    0xae6f8225e588fa0eul, 0xe70b1773a0adc760ul, 0x0c52498b801cb216ul,
    0xf404862f1fc55ea4ul, 0x2c3b5edd6bdb10e8ul, 0xa4df613b379a749aul,
    0x7270154b2f6728bful, 0x2b21fe6c881071ebul, 0x4a6218673b790830ul,
    0x8b11d7bc20253048ul, 0x1dd687340028de27ul, 0xad7b7fc53335c3f7ul,
    0x3e8bf2f4774a106ful, 0x0bf29a36bc6ece4aul, 0x7e694d7fb8e7afadul,
    0xcc5b62cb9ae471e6ul, 0xba3874ebd874bd14ul, 0xf3a54e07a6575be8ul,
    0xa2150eef98f4c576ul, 0x580a031a571bc72dul, 0x7e8a306cf494896cul,
    0x2da88ba8184918c4ul, 0x7ed049d98461c820ul, 0x6a013150d539554ful,
    0x7e3bb03bf54187b8ul, 0xfbe3c9ce2f8c2c46ul, 0x79aa1d332fe63c90ul,
    0x1a7523a29735e0a5ul, 0x238b0b6a610bde82ul, 0x535e4644addee63cul,
    0x811d1497a9e0b0c3ul, 0xf877efc9dcf6f907ul, 0xefd3c10e1eb593e6ul,
    0xb1baeff4fb15dfd7ul, 0x24c331355b3e7be9ul, 0xd6704f76a0d84700ul,
    0xc489863e7473f0d2ul, 0x22949763ab811e53ul, 0x449fef12345e69fdul,
    0xd4d7deb6a2da758dul, 0x666cefa3f36fceb0ul, 0xf6fd946c9e6c7b7ful,
    0x0dee8fcf743f2d38ul, 0x02d6aabafecf245ful, 0x31d9fb96521c9e82ul,
    0x9d3ef4c1680fb12bul, 0x74e20c84f7f4a825ul, 0xb5d7f04f0db0118eul,
    0x891b18e81af1dd33ul, 0x1513ed917bac91ceul, 0xcef0b59c4686fbb6ul,
    0x80a3f23676249e39ul, 0x7676bcdba873045eul, 0xba13ee7a166e8dfcul,
    0x76a293bd626690d6ul,
};
