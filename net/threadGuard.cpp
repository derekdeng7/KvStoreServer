#include "threadGuard.hpp"

namespace KvStoreServer {

	ThreadGuard::ThreadGuard(std::thread thd) : thd_(std::move(thd))
	{
		if (!this->thd_.joinable())
		{
			throw std::logic_error("No thread!");
		}
		std::cout << "[i] thread id :" << std::this_thread::get_id() << " construct ThreadGuard..."  << std::endl;
	}

	ThreadGuard::~ThreadGuard()
	{
		this->thd_.join();
		std::cout << "[i] thread id :" << std::this_thread::get_id() << " deconstruct ThreadGuard..." << std::endl;
	}

}