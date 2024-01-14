#include <stdexcept>
#include <utility>

// Исключение этого типа должно генерироватся при обращении к пустому optional
class BadOptionalAccess : public std::exception {
public:
    using exception::exception;

    virtual const char* what() const noexcept override {
        return "Bad optional access";
    }
};

template <typename T>
class Optional {
public:

    Optional() = default;

    Optional(const T& item) {
        buff_ = new (&data_[0]) T(item);

        is_initialized_ = true;
    }

    Optional(T&& item) {
        buff_ = new (&data_[0]) T(std::forward<T>(item));

        is_initialized_ = true;
    }

    Optional(const Optional& other) {

        (!other.HasValue() && HasValue()) ? Reset() :
        (other.HasValue() && !this->HasValue()) ? (void)(buff_ = new (&data_[0]) T(other.Value()), is_initialized_ = true) :
        (other.HasValue() && HasValue()) ? (void)(*buff_ = other.Value(), is_initialized_ = true) : void();

    }

    Optional(Optional&& other) noexcept {

        (!other.HasValue() && this->HasValue()) ? Reset() :
        (other.HasValue() && !HasValue()) ? (void)(buff_ = new (&data_[0]) T(std::forward<T>(other.Value())), is_initialized_ = true) :
        (other.HasValue() && HasValue()) ? (void)(*buff_ = std::forward<T>(other.Value()), is_initialized_ = true) : void();

    }

    ~Optional() {
        is_initialized_ ? buff_->~T() : void();
    }

    bool HasValue() const {return is_initialized_;}
    T& Value() {if (!is_initialized_) {throw BadOptionalAccess();} else {return *buff_;}}
    const T& Value() const {if (!is_initialized_) {throw BadOptionalAccess();} else {return *buff_;}}

    void Reset() {is_initialized_ ? (is_initialized_ = false, buff_->~T()) : void();}

    Optional& operator=(const T& item) {

        HasValue() ? (void)(*buff_ = item ) : (void)(buff_ = new (&data_[0]) T(item), is_initialized_ = true);

        return *this;
    }

    Optional& operator=(T&& item) {

        HasValue() ? (void)(*buff_ = std::forward<T>(item))
        : (void)(buff_ = new (&data_[0]) T(std::forward<T>(item)), is_initialized_ = true);

        return *this;
    }

    Optional& operator=(const Optional& other) {

        (!other.HasValue() && HasValue()) ? Reset() :
        (other.HasValue() && !HasValue()) ? (void)(buff_ = new (&data_[0]) T(other.Value()), is_initialized_ = true) :
        (other.HasValue() && HasValue()) ? (void)(*buff_ = other.Value(), is_initialized_ = true) : void();

        return *this;
    }

    Optional& operator=(Optional&& other) noexcept {

        (!other.HasValue() && HasValue()) ? Reset() :
        (other.HasValue() && !HasValue()) ? (void)(buff_ = new (&data_[0]) T(std::forward<T>(other.Value())), is_initialized_ = true) :
        (other.HasValue() && HasValue()) ? (void)(*buff_ = std::forward<T>(other.Value()), is_initialized_ = true) : void();

        return *this;
    }

    T& operator*() {return Value();}
    const T& operator*() const {return Value();}
    T* operator->() {return &Value();}
    const T* operator->() const {return &Value();}

private:
    T* buff_ = nullptr;

    alignas(T) char data_[sizeof(T)]{};
    bool is_initialized_ = false;
};
