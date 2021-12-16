#pragma once

#include <vector>
#include <array>
#include "Transform.h"

constexpr bool CLAMP_CURVE_PARAMS = true;

namespace Engine
{
	class CurveManager;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Numerical representation of a curve with fast lookup
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	template<std::size_t RESOLUTION = 20>
	struct Curve
	{
		/** Give a float percentage on range [0,1] and a mapped float on range [0,1] will be returned. */
		float eval_rigid(float rawAlpha) const
		{
			if constexpr (CLAMP_CURVE_PARAMS) { rawAlpha = glm::clamp(rawAlpha, 0.f, 1.f); } //this can be removed at compile time for performance; though that is not advised

			size_t stepIdx = size_t((RESOLUTION - 1)*rawAlpha + 0.5f);
			return curveSteps[stepIdx];
		}

		float eval_smooth(float rawAlpha) const
		{
			if constexpr (CLAMP_CURVE_PARAMS) { rawAlpha = glm::clamp(rawAlpha, 0.f, 1.f); } //this can be removed at compile time for performance; though that is not advised

			const size_t bottomIdx = size_t((RESOLUTION - 1) * rawAlpha);
			const size_t topIdx = glm::clamp<size_t>(bottomIdx + 1, 0, RESOLUTION - 1);
			const float bottomSample = curveSteps[bottomIdx];
			const float topSample = curveSteps[topIdx];
			const float lerpAlpha = rawAlpha - (float(bottomIdx) / RESOLUTION); //remove bottom fraction from passed alpha, this will let use see how far into sampleDelta we are, ie how close we are to next sample index
			const float sample = glm::mix(bottomSample, topSample, lerpAlpha);
			
			return sample;
		}
	private:
		friend CurveManager;
		std::array<float, RESOLUTION> curveSteps;
	};


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Curve system that can be used to get curves.
	// This is hardly a system and perhaps should be a standalone utility.
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	using Curve_highp = Curve<200>;
	class CurveManager
	{
	public:
		static const CurveManager& get();
	public: //curves are provided by copy to avoid memory corruption issues; cache these in your PostConstruct and use them.
		Curve_highp sigmoid_medp() const;
		Curve_highp generateSigmoid_medp(float tuning = 3.0f) const;
	private:
		CurveManager();
	public:
		static float sampleAnalyticSigmoid(float a, float tuning = 3.0f);
	private:
		Curve_highp curve_sigmoidmedp;
	};

}