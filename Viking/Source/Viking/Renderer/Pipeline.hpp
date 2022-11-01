#pragma once

#include "Viking/Renderer/Pipeline.hpp"

namespace Viking {
	class Pipeline {
	public:
		Pipeline() = default;
		virtual ~Pipeline() = default;

		static Ref<Pipeline> create();
	};
}
