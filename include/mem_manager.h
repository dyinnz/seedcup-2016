//
// Created by coder on 16-9-8.
//

#pragma once

#include <cstdint>
#include <cassert>
#include <vector>
#include <iostream>

constexpr std::uint8_t kDefaultBlocksNum = 255;

struct Chunk {
  void Init(std::size_t block_size, uint8_t blocks_num) {
    assert(blocks_num > 0);
    mem_ = new uint8_t[block_size * blocks_num];
    first_ = 0;
    free_num_ = blocks_num;

    uint8_t *p = mem_;
    uint8_t index = 0;
    while (index < blocks_num) {
      index += 1;
      *p = index;
      p += block_size;
    }
  }

  void Release() {
    delete[] mem_;
    free_num_ = 0;
  }

  void *Allocate(std::size_t block_size) {
    if (0 == free_num_) return nullptr;

    uint8_t *alloc = mem_ + first_ * block_size;
    first_ = *alloc;
    free_num_ -= 1;
    return alloc;
  }

  void Deallocate(void *p, std::size_t block_size) {
    assert(mem_ <= p);
    uint8_t *released = static_cast<uint8_t *>(p);

    assert((released - mem_) % block_size == 0);
    *released = first_;
    first_ = static_cast<uint8_t>((released - mem_) / block_size);

    assert(first_ == (released - mem_) / block_size);
    free_num_ += 1;
  }

  uint8_t *mem_;
  uint8_t first_;
  uint8_t free_num_;
};

class FixedAllocator {
 public:
  FixedAllocator(std::size_t block_size, uint8_t blocks_num) :
      last_alloc_(nullptr), last_dealloc_(nullptr),
      block_size_(block_size), blocks_num_(blocks_num) {
  }

  ~FixedAllocator() {
    for (auto chunk : chunks_) {
      chunk.Release();
    }
  }

  void *Allocate() {
    if (nullptr == last_alloc_ || 0 == last_alloc_->free_num_) {
      auto iter = chunks_.begin();
      while (true) {
        if (iter == chunks_.end()) {
          chunks_.emplace_back();
          chunks_.back().Init(block_size_, blocks_num_);
          last_alloc_ = &chunks_.back();
          last_dealloc_ = &chunks_.back();
          break;
        } else if (iter->free_num_ > 0) {
          last_alloc_ = &*iter;
          break;
        }
        ++iter;
      }
    }

    return last_alloc_->Allocate(block_size_);
  }

  void Deallocate(void *p) {
    if (p < last_dealloc_->mem_
        || last_dealloc_->mem_ + block_size_ * blocks_num_ <= p) {

      for (auto iter = chunks_.begin(); iter != chunks_.end(); ++iter) {
        if (iter->mem_ <= p
            && p < iter->mem_ + block_size_ * blocks_num_) {
          last_dealloc_ = &*iter;
          break;
        }
      }
    }
    last_dealloc_->Deallocate(p, block_size_);
  }

  void Release() {
    for (auto chunk : chunks_) {
      chunk.Release();
    }
    chunks_.clear();
    chunks_.shrink_to_fit();
    last_alloc_ = nullptr;
    last_dealloc_ = nullptr;
  }

 private:
  std::vector<Chunk> chunks_;
  Chunk *last_alloc_;
  Chunk *last_dealloc_;
  std::size_t block_size_;
  uint8_t blocks_num_;
};

template<class T, std::uint8_t N = kDefaultBlocksNum>
class SmallObjPool {
 public:
  SmallObjPool() : fixed_alloc_(sizeof(T), N) {}

  ~SmallObjPool() {
    Release();
  }

  template<class... A>
  T *Create(A &&... args) {
    T *p = static_cast<T *>(fixed_alloc_.Allocate());
    new(p) T(std::forward<A>(args)...);
    records_.push_back(p);
    return p;
  }

  void Destroy(T *p) = delete;

  void Release() {
    for (auto p : records_) {
      p->~T();
    }
    fixed_alloc_.Release();
  }

 private:
  FixedAllocator fixed_alloc_;
  std::vector<T *> records_;
};
