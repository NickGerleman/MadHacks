#include "pch.h"
#include "PeriodicTimer.h"

using namespace boost::asio;
using namespace std;

PeriodicTimer::PeriodicTimer(io_service& _ioService,
	const chrono::microseconds& _period,
	const TimerTask& _task)
	: ioService{ _ioService }
	, timer{ ioService }
	, period{ _period }
	, task{ _task }
	, running{ false } {
}

void PeriodicTimer::start(bool runOnStart) {
	std::cout << "[Info] PeriodicTimer::start" << std::endl;

	running = true;

	timer.expires_from_now(period);
	timer.async_wait([this](const boost::system::error_code& ec) {
		cbTimer(ec);
	});
	
	if (runOnStart) {
		//Run the task at time t=0
		ioService.post(task);
	}
}

void PeriodicTimer::stop() {
	std::cout << "[Info] PeriodicTimer::stop" << std::endl;

	running = false;
	timer.cancel();
}

void PeriodicTimer::cbTimer(const boost::system::error_code& ec) {
	if (running) {
		if (!ec) {
			task();

			timer.expires_at(timer.expires_at() + period);
			timer.async_wait([this](const boost::system::error_code& ec) {
				cbTimer(ec);
			});
		}
		else if (ec != boost::asio::error::operation_aborted) {
			std::cerr << "[Error] PeriodicTimer::timerLambda: "
				<< ec.message() << std::endl;
		}
	}
}