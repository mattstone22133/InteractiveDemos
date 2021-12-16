
#include<cmath>
#include "Tools/Curves/Curves.h"

namespace Engine
{
	CurveManager::CurveManager()
	{
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Create curves in ctor so that subclasses cannot forget to call super.
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		curve_sigmoidmedp = generateSigmoid_medp(3.0);

	}

	float CurveManager::sampleAnalyticSigmoid(float a, float tuning)
	{
		// math: https://stats.stackexchange.com/questions/214877/is-there-a-formula-for-an-s-shaped-curve-with-domain-and-range-0-1

		//float sigmoidSample = 1.f / (1 + std::powf(a / (1.f - a), -tuning)); no powf on gcc?
		float sigmoidSample = 1.f / (1 + std::pow(a / (1.f - a), -tuning));
		return sigmoidSample;
	}

	/*static*/ const Engine::CurveManager& CurveManager::get()
	{
		static CurveManager cs;
		return cs;
	}

	Engine::Curve_highp CurveManager::sigmoid_medp() const
	{
		//this intentionally returns a copy so that any bad code that has out of bound memory access will not be able to corrupt this system's memory.
		return curve_sigmoidmedp;
	}

	Engine::Curve_highp CurveManager::generateSigmoid_medp(float tuning /*= 3.0f*/) const
	{
		Curve_highp curve;

		for (size_t sample = 0; sample < curve.curveSteps.size(); ++sample)
		{
			curve.curveSteps[sample] = sampleAnalyticSigmoid(float(sample) / (curve.curveSteps.size() - 1), tuning); //have to remember to -1 to get lats element at 1
		}

		return curve;
	}

}
