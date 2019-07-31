#pragma once

#include "Audio.hpp"

#include <vector>
#include <pcg_random.hpp>

namespace jm
{
	struct SoundEffectParams
	{
		float volume = 1;
		float pitch = 1;
		float rolloffFactor = 1;
		float refDistance = 0;
	};
	
	JAPI void PlaySoundEffect(const AudioClip& clip, glm::vec3 position, SoundEffectParams params = { });
	
	inline void PlaySoundEffect(const AudioClip& clip, glm::vec2 position, SoundEffectParams params = { })
	{
		PlaySoundEffect(clip, glm::vec3(position, 0), params);
	}
	
	class JAPI MusicPlayer
	{
	public:
		MusicPlayer();
		
		void SetTracks(std::vector<const AudioClip*> tracks);
		
		void Update(float dt);
		
		void SetFadeTime(float fadeTime)
		{
			m_volumeFadeTimeScale = 1.0f / fadeTime;
		}
		
		float volume = 1.0f;
		bool active = true;
		
	private:
		AudioSource m_source;
		std::vector<const AudioClip*> m_tracks;
		size_t m_nextTrack = 0;
		
		float m_volumeFadeTimeScale = 1.0f;
		float m_volumeFade = 1.0f;
		bool m_isFadingOut = false;
		
		pcg32_fast m_rng;
	};
}
