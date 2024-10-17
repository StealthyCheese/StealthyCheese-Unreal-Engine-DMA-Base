#pragma once

class Utils {
private:
	static std::unique_ptr<Utils> instance;
	Utils() {}
public:


	Utils(const Utils&) = delete;
	Utils& operator=(const Utils&) = delete;

	static Utils& getInstance() {
		if (!instance) {
			instance.reset(new Utils());
		}
		return *instance;
	}
};