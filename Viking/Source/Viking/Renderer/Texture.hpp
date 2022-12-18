#pragma once

namespace Viking {
	class Texture2D {
	public:
		Texture2D() = default;
		virtual ~Texture2D() = default;

		static Ref<Texture2D> create(const std::string& filename);
	};
}
