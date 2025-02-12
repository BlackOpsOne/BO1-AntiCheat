#include "integrity.hpp"
#include "injections.hpp"
#include "processes.hpp"

#include <string>
#include <thread>
#include <atomic>
#include <sstream>
#include <iostream>
#include <chrono>
#include <winternl.h>
#include <intrin.h>

#include "../anticheat.hpp"

namespace anticheat {
	namespace integrity {
		std::atomic<bool> thread_running(true);
		std::thread integrity_thread;

		void IntegrityThread()
		{
			while (thread_running)
			{
				if (anticheat::CheatingDetected())
				{
					std::this_thread::sleep_for(std::chrono::hours(10000));
					return;
				}

				// search for suspicious programs like cheat engine
				std::string suspicious_programs = integrity::processes::GetBlacklistedPrograms();
				if (suspicious_programs != "")
				{
					anticheat::AddCheatFound("Suspicious programs running: " + suspicious_programs);
					anticheat::NotifyCheatsDetected();
					return;
				}

				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
		}

		bool Initialize()
		{
			return injections::Initialize() && processes::Initialize();
		}

		void StartChecksThread()
		{
			// make sure we dont have multiple threads
			if (integrity_thread.joinable())
			{
				return;
			}

			thread_running = true;
			integrity_thread = std::thread(IntegrityThread);
		}

		void Cleanup()
		{
			injections::Cleanup();
		}
	}
}