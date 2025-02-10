#include "statuses.h"

namespace anticheat {
	void Initialize();
	void OnGameOpened();
	void OnGameClosed();
	void AttemptIntegrityCheck();
	void AddCheatFound(std::string cheating_method);
	void NotifyCheatsDetected(bool show_detections = true);
	void WaitForBlackOpsProcess();

	bool CheatingDetected();
	std::string GetMainStatus();
	std::string GetInfoStatus();

	bool IsMapValid(const char* map_name);
}