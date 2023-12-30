#pragma once

inline void PutDebug(std::string msg) {
	std::cout << "[GLUMMY LOADER]: " << msg << std::endl;
}

inline VOID DebugToFile(std::string szInput)
{
	std::ofstream log("spartacus-proxy-version.log", std::ios_base::app | std::ios_base::out);
	log << szInput;
	log << "\n";
}

inline void TimeStampDebug(std::string msg) {
	std::time_t t = std::time(0); // get time now
	std::tm* now = std::localtime(&t);

	std::string timeStamp = now->tm_hour
		+ ':'
		+ now->tm_min
		+ ':'
		+ now->tm_sec
		+ "-> ";

	std::cout << timeStamp;
	PutDebug(timeStamp);
}

inline void InitConsole() {
	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);
	PutDebug("Console initialized");
}
