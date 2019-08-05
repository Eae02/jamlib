#include "Camera2D.hpp"
#include "Utils.hpp"
#include "Graphics/Graphics2D.hpp"

#include <array>
#include <random>
#include <pcg_random.hpp>

namespace jm
{
	struct ShakeWave
	{
		glm::vec2 direction;
		float phase;
	};
	
	static std::array<ShakeWave, 8> shakeWaves;
	static bool shakeWavesGenerated = false;
	
	Camera2D::Camera2D()
		: shakeRoughness(70.0f), rotationShakeMagnitude(0.01f),
		  m_shakeBeginTime(std::chrono::high_resolution_clock::now())
	{
		
	}
	
	void Camera2D::Shake(float magnitude, float duration)
	{
		m_shakeMagnitude = magnitude;
		m_shakeFalloff = magnitude / duration;
		m_shakeBeginTime = std::chrono::high_resolution_clock::now();
	}
	
	std::pair<glm::mat3, glm::mat3> Camera2D::ViewMatrixAndInverse(glm::vec2 target, float zoom, float rotation) const
	{
		if (!shakeWavesGenerated)
		{
			pcg32_fast rng;
			std::uniform_real_distribution<float> phaseDist(0, 2 * M_PI);
			
			for (size_t i = 0; i < shakeWaves.size(); i++)
			{
				float t = (float)i * (PI / shakeWaves.size());
				shakeWaves[i].direction = glm::vec2(std::cos(t), std::sin(t));
				shakeWaves[i].phase = phaseDist(rng);
			}
			
			shakeWavesGenerated = true;
		}
		
		float shakeElapsed = (float)(std::max<int64_t>((std::chrono::high_resolution_clock::now() - m_shakeBeginTime).count() / 1000, 0) * 1E-6);
		float shakeMagnitude = m_shakeMagnitude - shakeElapsed * m_shakeFalloff;
		
		if (shakeMagnitude > 0)
		{
			float t = shakeElapsed * shakeRoughness;
			
			glm::vec2 shakeDelta;
			for (const ShakeWave& wave : shakeWaves)
			{
				shakeDelta += wave.direction * std::sin(t + wave.phase);
			}
			target += shakeDelta * shakeMagnitude;
			
			rotation += (std::sin(t * 0.25f) + std::sin(t * 0.5f + PI)) * shakeMagnitude * rotationShakeMagnitude;
		}
		
		if (constrainToBounds)
		{
			target = ConstrainTargetToBounds(target, zoom);
		}
		
		return { MakeViewMatrix2D(target, zoom, rotation), MakeInverseViewMatrix2D(target, zoom, rotation) };
	}
	
	glm::vec2 Camera2D::ConstrainTargetToBounds(glm::vec2 target, float zoom) const
	{
		glm::vec2 viewRad = glm::vec2(CurrentRTWidth(), CurrentRTHeight()) / (2 * zoom);
		
		glm::vec2 max = target + viewRad;
		if (max.x > bounds.MaxX())
			target.x += bounds.MaxX() - max.x;
		if (max.y > bounds.MaxY())
			target.y += bounds.MaxY() - max.y;
		
		glm::vec2 min = target - viewRad;
		if (min.x < bounds.x)
			target.x += bounds.x - min.x;
		if (min.y < bounds.y)
			target.y += bounds.y - min.y;
		
		return target;
	}
}
