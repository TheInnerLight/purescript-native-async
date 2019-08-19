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

exports["liftEffectA"] = [](const boxed& io) -> boxed {
  return [=](const boxed& ioservice) -> boxed {
    return [=](const boxed& cb) -> boxed {
      const auto& cbfn = unbox<std::function<boxed(const boxed&)>>(cb);
      auto& ec = *static_cast<boost::asio::io_service*>(ioservice.get());
      return cbfn(io());
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
        async(ioservice)(test);
        return boxed();
      };
    };
  };
};

exports["waitA"] = [](const boxed& millis_) -> boxed {
  const auto millis = unbox<int>(millis_);
  return [=](const boxed& ec_) -> boxed {
    return [=](const boxed& cb) -> boxed {
      auto& ec = *static_cast<boost::asio::io_service*>(ec_.get());
      auto timer = std::make_unique<boost::asio::steady_timer>(ec);
      const auto& cbfn = unbox<std::function<boxed(const boxed&)>>(cb);
      timer->expires_after(std::chrono::milliseconds(millis));
      timer->async_wait([=, timer{std::move(timer)}](const boost::system::error_code &ec){ cbfn(boxed()); });
      return boxed();
    };
  };
};

exports["parForA"] = [](const boxed& xs_) -> boxed {
  const auto& xs = unbox<array_t>(xs_);
  return [=](const boxed& f) -> boxed {
    return [=](const boxed& ioservice) -> boxed {
      return [=](const boxed& cb) -> boxed {
        const auto& cbfn = unbox<std::function<boxed(const boxed&)>>(cb);
        auto completed_ops = std::make_shared<std::atomic<size_t>>(0);
        auto results = std::make_shared<array_t>(xs.size());
        const auto storeResult =
          [=](int i) -> boxed {
            return box<std::function<boxed(const boxed&)>>([=](const boxed& a) mutable -> boxed {
              results->at(i) = a;
              if(completed_ops->fetch_add(1, std::memory_order_relaxed)+1 == xs.size()) {
                results->shrink_to_fit();
                cbfn(*results);
              }
              return boxed();
            });
          };
        int i = 0;
        for (auto it = xs.cbegin(), end = xs.cend(); it != end ; it++, i++) {
            f(*it)(ioservice)(storeResult(i));
        }
        return boxed();
      };
    };
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
