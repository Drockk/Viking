#pragma once

namespace Viking {
	class Context {
	public:
		Context() = default;
		virtual ~Context() = default;
		virtual void init(const std::string& name) = 0;

		static Scope<Context> create();
	};
}
