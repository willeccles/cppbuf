#ifndef _CBUFFER_H__
#define _CBUFFER_H__

#include <cstdlib>
#include <cstring>
#include <functional>
#include <initializer_list>
#include <new>
#include <stdexcept>

template<class T>
class CBuffer {
public:
  using ContainedType = T;

  CBuffer(T* buf, std::size_t nmemb): len_(nmemb) {
    if (nmemb != 0) {
      buf_ = reinterpret_cast<T*>(std::calloc(nmemb, sizeof(T)));
      if (buf_ == nullptr) {
        throw std::bad_alloc();
      }

      if (buf != nullptr) {
        std::memcpy(reinterpret_cast<void*>(buf_),
                    reinterpret_cast<void*>(buf),
                    sizeof(T) * nmemb);
      }
    }
  }

  CBuffer(std::size_t nmemb): len_(nmemb) {
    if (nmemb != 0) {
      buf_ = reinterpret_cast<T*>(std::calloc(nmemb, sizeof(T)));
      if (buf_ == nullptr) {
        throw std::bad_alloc();
      }
    }
  }

  CBuffer(std::initializer_list<T> li) {
    buf_ = reinterpret_cast<T*>(std::calloc(li.size(), sizeof(T)));
    if (buf_ == nullptr) {
      throw std::bad_alloc();
    }

    len_ = 0;
    for (const T& val : li) {
      buf_[len_++] = val;
    }
  }

  CBuffer() {}

  ~CBuffer() {
    if (buf_ != nullptr) {
      std::free((void*)buf_);
    }
  }

  std::size_t count() const {
    return len_;
  }

  std::size_t size() const {
    return len_ * sizeof(T);
  }

  T& at(std::size_t n) {
    if (n >= len_) {
      throw std::out_of_range("buffer index out of bounds");
    }
    return buf_[n];
  }

  const T& at(std::size_t n) const {
    if (n >= len_) {
      throw std::out_of_range("buffer index out of bounds");
    }
    return buf_[n];
  }

  T& operator[] (std::size_t n) {
    return this->at(n);
  }

  const T& operator[] (std::size_t n) const {
    return this->at(n);
  }

  T& operator* () {
    return this->at(0);
  }

  const T& operator* () const {
    return this->at(0);
  }

  void resize(std::size_t nmemb) {
    if (nmemb != len_) {
      if (nmemb == 0) {
        if (buf_) {
          std::free(buf_);
          buf_ = nullptr;
          len_ = nmemb;
        }
      } else {
        T* tmp = reinterpret_cast<T*>(std::realloc(buf_, nmemb * sizeof(T)));
        if (tmp == nullptr) {
          throw std::bad_alloc();
        } else {
          buf_ = tmp;
          if (nmemb > len_) {
            std::memset(reinterpret_cast<void*>(buf_ + len_),
                        0, (nmemb - len_) * sizeof(T));
          }
          len_ = nmemb;
        }
      }
    }
  }

  void fill(const T& val) {
    for (std::size_t i = 0; i < len_; i++) {
      buf_[i] = val;
    }
  }

  void map(std::function<T(T)> fn) {
    for (std::size_t i = 0; i < len_; i++) {
      buf_[i] = fn(buf_[i]);
    }
  }

  template<typename nT> operator nT* () {
    return reinterpret_cast<nT*>(buf_);
  }

  T* begin() {
    return buf_;
  }

  const T* begin() const {
    return reinterpret_cast<const T*>(buf_);
  }

  T* end() {
    return buf_ + len_;
  }

  const T* end() const {
    return reinterpret_cast<const T*>(buf_ + len_);
  }

private:
  T* buf_ = nullptr;
  std::size_t len_ = 0;
};

#endif // _CBUFFER_H__
