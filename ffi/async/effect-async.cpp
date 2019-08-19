#include <functional>
#include <future>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "purescript.h"

// Tested with package v2.0.0

FOREIGN_BEGIN( Effect_Async )

// a -> ioservice -> (a -> unit)

exports["pureA"] = [](const boxed& a) -> boxed {
  return [=](const boxed& ioservice) -> boxed {
    return [=](const boxed& cb) -> boxed {
      const auto& cbfn = unbox<std::function<boxed(const boxed&)>>(cb);
      return cbfn(a);
    };
  };
};

// async : ioservice -> (a -> unit) -> unit
// f : a -> ioservice -> (b -> unit) -> unit
// r: ioservice -> (b -> unit) -> unit
// ioservice -> (a -> ioservice -> (b -> unit) -> unit) -> unit

exports["bindA"] = [](const boxed& async) -> boxed {
  return [=](const boxed& f) -> boxed {
    return [=](const boxed& ioservice) -> boxed {
      return [=](const boxed& cb) -> boxed {
        const auto test = box<std::function<boxed(const boxed&)>>([=](const boxed& a) -> boxed {
              return f(a)(ioservice)(cb);
          });
        async(ioservice)(test); // \x -> f x ioservice :: a -> (b -> unit)
        return boxed();
      };
    };
  };
};

// ioservice -> (unit -> unit)

exports["waitA"] = [](const boxed& ioservice) -> boxed {
  return [=](const boxed& cb) -> boxed {
    auto& ec = *static_cast<boost::asio::io_service*>(ioservice.get());
    boost::asio::steady_timer* timer = new boost::asio::steady_timer(ec);
    const auto& cbfn = unbox<std::function<boxed(const boxed&)>>(cb);
    timer->expires_after(std::chrono::seconds(5));
    timer->async_wait([=](const boost::system::error_code &ec){ cbfn(boxed()); });
    return boxed();
  };
};

exports["runA"] = [](const boxed& fn) -> boxed {
    return [=]() -> boxed {
        auto asio_io_service = box<boost::asio::io_service>();
        auto& u_asio_io_service = unbox<boost::asio::io_service>(asio_io_service);
        auto promise = std::promise<boxed>();
        auto fut = promise.get_future();
        const boxed& cb = box<std::function<boxed(const boxed&)>>(
          [&](const boxed& a) -> boxed {
              promise.set_value(a);
              return boxed();
          });
        fn(asio_io_service)(cb);
        u_asio_io_service.run();
        return fut.get();
        return boxed();
    };
};

FOREIGN_END
