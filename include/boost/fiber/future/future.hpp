
//          Copyright Oliver Kowalke 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_FIBERS_FUTURE_HPP
#define BOOST_FIBERS_FUTURE_HPP

#include <algorithm>
#include <chrono>
#include <exception>

#include <boost/config.hpp>

#include <boost/fiber/detail/config.hpp>
#include <boost/fiber/exceptions.hpp>
#include <boost/fiber/future/detail/shared_state.hpp>
#include <boost/fiber/future/future_status.hpp>

namespace boost {
namespace fibers {
namespace detail {

template< typename R >
struct future_base {
    typedef typename shared_state< R >::ptr_t   ptr_t;

    ptr_t           state_{};

    constexpr future_base() = default;

    explicit future_base( ptr_t const& p) noexcept :
        state_{ p } {
    }

    ~future_base() = default;

    future_base( future_base const& other) :
        state_{ other.state_ } {
    }

    future_base( future_base && other) noexcept :
        state_{ other.state_ } {
        other.state_.reset();
    }

    future_base & operator=( future_base const& other) noexcept {
        if ( this == & other) return * this;
        state_ = other.state_;
        return * this;
    }

    future_base & operator=( future_base && other) noexcept {
        if ( this == & other) return * this;
        state_ = other.state_;
        other.state_.reset();
        return * this;
    }

    bool valid() const noexcept {
        return nullptr != state_.get();
    }

    std::exception_ptr get_exception_ptr() {
        if ( ! valid() ) {
            throw future_uninitialized{};
        }
        return state_->get_exception_ptr();
    }

    void wait() const {
        if ( ! valid() ) {
            throw future_uninitialized{};
        }
        state_->wait();
    }

    template< class Rep, class Period >
    future_status wait_for( std::chrono::duration< Rep, Period > const& timeout_duration) const {
        if ( ! valid() ) {
            throw future_uninitialized{};
        }
        return state_->wait_for( timeout_duration);
    }

    template< typename Clock, typename Duration >
    future_status wait_until( std::chrono::time_point< Clock, Duration > const& timeout_time) const {
        if ( ! valid() ) {
            throw future_uninitialized{};
        }
        return state_->wait_until( timeout_time);
    }
};

}

template< typename R >
class shared_future;

template< typename R >
class future : private detail::future_base< R > {
private:
    typedef detail::future_base< R >  base_t;

    friend class shared_future< R >;

public:
    constexpr future() = default;

    explicit future( typename base_t::ptr_t const& p) noexcept :
        base_t{ p } {
    }

    future( future const&) = delete;
    future & operator=( future const&) = delete;

    future( future && other) noexcept :
        base_t{ std::forward< future >( other) } {
    }

    future & operator=( future && other) noexcept {
        if ( this == & other) return * this;
        base_t::operator=( std::forward< future >( other) );
        return * this;
    }

    shared_future< R > share();

    R get() {
        if ( ! base_t::valid() ) {
            throw future_uninitialized{};
        }
        typename base_t::ptr_t tmp{};
        tmp.swap( base_t::state_);
        return std::move( tmp->get() );
    }

    using base_t::valid;
    using base_t::get_exception_ptr;
    using base_t::wait;

    template< class Rep, class Period >
    future_status wait_for( std::chrono::duration< Rep, Period > const& timeout_duration) const {
        return base_t::wait_for( timeout_duration);
    }

    template< typename Clock, typename Duration >
    future_status wait_until( std::chrono::time_point< Clock, Duration > const& timeout_time) const {
        return base_t::wait_until( timeout_time);
    }
};

template< typename R >
class future< R & > : private detail::future_base< R & > {
private:
    typedef detail::future_base< R & >  base_t;

    friend class shared_future< R & >;

public:
    constexpr future() = default;

    explicit future( typename base_t::ptr_t const& p) noexcept :
        base_t{ p  } {
    }

    future( future const&) = delete;
    future & operator=( future const&) = delete;

    future( future && other) noexcept :
        base_t{ std::forward< future >( other) } {
    }

    future & operator=( future && other) noexcept {
        if ( this == & other) return * this;
        base_t::operator=( std::forward< future >( other) );
        return * this;
    }

    shared_future< R & > share();

    R & get() {
        if ( ! base_t::valid() ) {
            throw future_uninitialized{};
        }
        typename base_t::ptr_t tmp{};
        tmp.swap( base_t::state_);
        return tmp->get();
    }

    using base_t::valid;
    using base_t::get_exception_ptr;
    using base_t::wait;

    template< class Rep, class Period >
    future_status wait_for( std::chrono::duration< Rep, Period > const& timeout_duration) const {
        return base_t::wait_for( timeout_duration);
    }

    template< typename Clock, typename Duration >
    future_status wait_until( std::chrono::time_point< Clock, Duration > const& timeout_time) const {
        return base_t::wait_until( timeout_time);
    }
};

template<>
class future< void > : private detail::future_base< void > {
private:
    typedef detail::future_base< void >  base_t;

    friend class shared_future< void >;

public:
    constexpr future() = default;

    explicit future( base_t::ptr_t const& p) noexcept :
        base_t{ p } {
    }

    future( future const&) = delete;
    future & operator=( future const&) = delete;

    inline
    future( future && other) noexcept :
        base_t{ std::forward< future >( other) } {
    }

    inline
    future & operator=( future && other) noexcept {
        if ( this == & other) return * this;
        base_t::operator=( std::forward< future >( other) );
        return * this;
    }

    shared_future< void > share();

    inline
    void get() {
        if ( ! base_t::valid() ) {
            throw future_uninitialized{};
        }
        base_t::ptr_t tmp{};
        tmp.swap( base_t::state_);
        tmp->get();
    }

    using base_t::valid;
    using base_t::get_exception_ptr;
    using base_t::wait;

    template< class Rep, class Period >
    future_status wait_for( std::chrono::duration< Rep, Period > const& timeout_duration) const {
        return base_t::wait_for( timeout_duration);
    }

    template< typename Clock, typename Duration >
    future_status wait_until( std::chrono::time_point< Clock, Duration > const& timeout_time) const {
        return base_t::wait_until( timeout_time);
    }
};


template< typename R >
class shared_future : private detail::future_base< R > {
private:
    typedef detail::future_base< R >   base_t;

    explicit shared_future( typename base_t::ptr_t const& p) noexcept :
        base_t{ p } {
    }

public:
    constexpr shared_future() = default;

    ~shared_future() = default;

    shared_future( shared_future const& other) :
        base_t{ other } {
    }

    shared_future( shared_future && other) noexcept :
        base_t{ std::forward< shared_future >( other) } {
    }

    shared_future( future< R > && other) noexcept :
        base_t{ std::forward< future< R > >( other) } {
    }

    shared_future & operator=( shared_future const& other) noexcept {
        if ( this == & other) return * this;
        base_t::operator=( other); 
        return * this;
    }

    shared_future & operator=( shared_future && other) noexcept {
        if ( this == & other) return * this;
        base_t::operator=( std::forward< shared_future >( other) ); 
        return * this;
    }

    shared_future & operator=( future< R > && other) noexcept {
        base_t::operator=( std::forward< future< R > >( other) ); 
        return * this;
    }

    R const& get() const {
        if ( ! valid() ) {
            throw future_uninitialized{};
        }
        return base_t::state_->get();
    }

    using base_t::valid;
    using base_t::get_exception_ptr;
    using base_t::wait;

    template< class Rep, class Period >
    future_status wait_for( std::chrono::duration< Rep, Period > const& timeout_duration) const {
        return base_t::wait_for( timeout_duration);
    }

    template< typename Clock, typename Duration >
    future_status wait_until( std::chrono::time_point< Clock, Duration > const& timeout_time) const {
        return base_t::wait_until( timeout_time);
    }
};

template< typename R >
class shared_future< R & > : private detail::future_base< R & > {
private:
    typedef detail::future_base< R & >  base_t;

    explicit shared_future( typename base_t::ptr_t const& p) noexcept :
        base_t{ p } {
    }

public:
    constexpr shared_future() = default;

    ~shared_future() = default;

    shared_future( shared_future const& other) :
        base_t{ other } {
    }

    shared_future( shared_future && other) noexcept :
        base_t{ std::forward< shared_future >( other) } {
    }

    shared_future( future< R & > && other) noexcept :
        base_t{ std::forward< future< R & > >( other) } {
    }

    shared_future & operator=( shared_future const& other) noexcept {
        if ( this == & other) return * this;
        base_t::operator=( other);
        return * this;
    }

    shared_future & operator=( shared_future && other) noexcept {
        if ( this == & other) return * this;
        base_t::operator=( std::forward< shared_future >( other) );
        return * this;
    }

    shared_future & operator=( future< R & > && other) noexcept {
        base_t::operator=( std::forward< future< R & > >( other) );
        return * this;
    }

    R & get() const {
        if ( ! valid() ) {
            throw future_uninitialized{};
        }
        return base_t::state_->get();
    }

    using base_t::valid;
    using base_t::get_exception_ptr;
    using base_t::wait;

    template< class Rep, class Period >
    future_status wait_for( std::chrono::duration< Rep, Period > const& timeout_duration) const {
        return base_t::wait_for( timeout_duration);
    }

    template< typename Clock, typename Duration >
    future_status wait_until( std::chrono::time_point< Clock, Duration > const& timeout_time) const {
        return base_t::wait_until( timeout_time);
    }
};

template<>
class shared_future< void > : private detail::future_base< void > {
private:
    typedef detail::future_base< void > base_t;

    explicit shared_future( base_t::ptr_t const& p) noexcept :
        base_t{ p } {
    }

public:
    constexpr shared_future() = default;

    ~shared_future() = default;

    inline
    shared_future( shared_future const& other) :
        base_t{ other } {
    }

    inline
    shared_future( shared_future && other) noexcept :
        base_t{ std::forward< shared_future >( other) } {
    }

    inline
    shared_future( future< void > && other) noexcept :
        base_t{ std::forward< future< void > >( other) } {
    }

    inline
    shared_future & operator=( shared_future const& other) noexcept {
        if ( this == & other) return * this;
        base_t::operator=( other);
        return * this;
    }

    inline
    shared_future & operator=( shared_future && other) noexcept {
        if ( this == & other) return * this;
        base_t::operator=( std::forward< shared_future >( other) );
        return * this;
    }

    inline
    shared_future & operator=( future< void > && other) noexcept {
        base_t::operator=( std::forward< future< void > >( other) );
        return * this;
    }

    inline
    void get() const {
        if ( ! valid() ) {
            throw future_uninitialized{};
        }
        base_t::state_->get();
    }

    using base_t::valid;
    using base_t::get_exception_ptr;
    using base_t::wait;

    template< class Rep, class Period >
    future_status wait_for( std::chrono::duration< Rep, Period > const& timeout_duration) const {
        return base_t::wait_for( timeout_duration);
    }

    template< typename Clock, typename Duration >
    future_status wait_until( std::chrono::time_point< Clock, Duration > const& timeout_time) const {
        return base_t::wait_until( timeout_time);
    }
};


template< typename R >
shared_future< R >
future< R >::share() {
    if ( ! base_t::valid() ) {
        throw future_uninitialized{};
    }
    return shared_future< R >{ std::move( * this) };
}

template< typename R >
shared_future< R & >
future< R & >::share() {
    if ( ! base_t::valid() ) {
        throw future_uninitialized{};
    }
    return shared_future< R & >{ std::move( * this) };
}

inline
shared_future< void >
future< void >::share() {
    if ( ! base_t::valid() ) {
        throw future_uninitialized{};
    }
    return shared_future< void >{ std::move( * this) };
}

}}

#endif
