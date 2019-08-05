#pragma once

#include "API.hpp"
#include "Graphics/Buffer.hpp"
#include "Graphics/VertexLayout.hpp"
#include "ParticleEmitterType.hpp"

#include <glm/glm.hpp>
#include <pcg_random.hpp>
#include <vector>
#include <memory>
#include <list>

namespace jm
{
	class JAPI ParticleEmitter
	{
	public:
		explicit ParticleEmitter(const struct ParticleEmitterType& type)
			: m_type(&type) { }
		
		bool enabled = true;
		glm::vec2 position;
		float rotation = 0;
		
		const struct ParticleEmitterType& Type() const
		{
			return *m_type;
		}
		
	private:
		const struct ParticleEmitterType* m_type;
	};
	
	class JAPI ParticleManager
	{
	public:
		ParticleManager();
		
		void AddEmitter(std::weak_ptr<ParticleEmitter> emitter);
		
		void KeepAlive(std::shared_ptr<ParticleEmitter> emitter, float time);
		
		void Update(float dt);
		
		void Draw(const glm::mat3& viewMatrix);
		
		size_t TotalParticles() const
		{
			return m_particleDrawData.size();
		}
		
	private:
		pcg32_fast m_rng;
		
		static constexpr size_t PARTICLES_PER_PAGE = 1024;
		
		struct ParticlePage
		{
			uint32_t numParticles;
			ParticlePage* next;
			glm::vec2 pos[PARTICLES_PER_PAGE];
			glm::vec2 vel[PARTICLES_PER_PAGE];
			glm::vec2 sizeStartEnd[PARTICLES_PER_PAGE];
			glm::vec2 opacityStartEnd[PARTICLES_PER_PAGE];
			float rotation[PARTICLES_PER_PAGE];
			float rotationVelocity[PARTICLES_PER_PAGE];
			float elapsedLifeTime[PARTICLES_PER_PAGE];
			float totalLifeTime[PARTICLES_PER_PAGE];
			int textureArea[PARTICLES_PER_PAGE];
		};
		
		std::list<ParticlePage> m_allPages;
		ParticlePage* m_availablePages = nullptr;
		
		struct TextureArea
		{
			glm::vec4 area;
			float aspectRatio;
		};
		
		struct EmitterEntry
		{
			std::weak_ptr<ParticleEmitter> weakEmitter;
			const ParticleEmitterType* type;
			
			std::vector<TextureArea> textureAreas;
			
			ParticlePage* firstPage;
			
			uint32_t firstParticle;
			uint32_t totalParticles;
			
			float timeNotEmitted;
			
			bool available;
			bool hasOldData;
			glm::vec2 oldPosition;
			float oldRotation;
		};
		
		std::vector<EmitterEntry> m_emitters;
		
#pragma pack(push, 1)
		struct ParticleDrawData
		{
			glm::vec4 textureArea;
			float opacity;
			glm::mat3x2 matrix;
		};
#pragma pack(pop)
		
		std::vector<ParticleDrawData> m_particleDrawData;
		Buffer m_particlesBuffer;
		VertexLayout m_vertexLayout;
		
		std::vector<std::pair<float, std::shared_ptr<ParticleEmitter>>> m_keepAlive;
	};
}
