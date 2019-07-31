#pragma once

#include "../API.hpp"

#include <glm/glm.hpp>

namespace jm
{
	namespace detail
	{
		void InitializeOpenAL();
		void CloseOpenAL();
		
		struct ALHandle
		{
			ALHandle()
				: handle(0), deleter(nullptr) { }
			
			~ALHandle()
			{
				if (deleter)
					deleter(1, &handle);
			}
			
			ALHandle(ALHandle&& other) noexcept
				: handle(other.handle), deleter(other.deleter)
			{
				other.deleter = nullptr;
			}
			
			ALHandle& operator=(ALHandle&& other) noexcept
			{
				handle = other.handle;
				deleter = other.deleter;
				other.deleter = nullptr;
				return *this;
			}
			
			uint32_t handle;
			void (*deleter)(int, const uint32_t*);
		};
	}
	
	JAPI void UpdateListener(const glm::vec3& position, const glm::vec3& velocity = glm::vec3(0.0f));
	JAPI void SetMasterVolume(float volume);
	
	inline void UpdateListener(const glm::vec2& position, const glm::vec2& velocity = glm::vec2(0.0f))
	{
		UpdateListener(glm::vec3(position, 0), glm::vec3(velocity, 0));
	}
	
	enum class AudioFormat
	{
		Mono8,
		Mono16,
		Stereo8,
		Stereo16
	};
	
	class JAPI AudioClip
	{
	public:
		friend class AudioSource;
		
		AudioClip();
		
		void SetData(AudioFormat format, size_t dataBytes, const void* data, int frequency);
		
	private:
		detail::ALHandle m_handle;
	};
	
	class JAPI AudioSource
	{
	public:
		AudioSource();
		
		explicit AudioSource(const AudioClip& clip)
		{
			SetClip(clip);
		}
		
		void SetClip(const AudioClip& clip);
		
		void SetIsLooping(bool isLooping);
		void SetTransformRelative(bool relativeTransform);
		
		void Play();
		void Stop();
		bool IsPlaying() const;
		
		void SetPosition(glm::vec3 position);
		void SetVelocity(glm::vec3 velocity);
		void SetDirection(glm::vec3 direction);
		
		void SetPosition(glm::vec2 position)
		{ SetPosition(glm::vec3(position, 0)); }
		void SetVelocity(glm::vec2 velocity)
		{ SetVelocity(glm::vec3(velocity, 0)); }
		void SetDirection(glm::vec2 direction)
		{ SetDirection(glm::vec3(direction, 0)); }
		
		void SetAttenuation(float rolloffFactor, float refDistance);
		
		void SetVolume(float volume);
		void SetPitch(float pitch);
		
	private:
		detail::ALHandle m_handle;
	};
}
