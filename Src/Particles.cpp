#include "Particles.hpp"
#include "ParticleEmitterType.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/Graphics.hpp"

#include <chrono>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>

namespace jm
{
	static Buffer* vertexBuffer;
	
	ParticleManager::ParticleManager()
		: m_rng(std::chrono::high_resolution_clock::now().time_since_epoch().count()),
		  m_particlesBuffer(BufferFlags::AllowUpdate | BufferFlags::VertexBuffer, 0, nullptr)
	{
		m_vertexLayout.InitBinding(0, sizeof(glm::vec2), InputRate::Vertex);
		m_vertexLayout.InitBinding(1, sizeof(ParticleDrawData), InputRate::Instance);
		m_vertexLayout.InitAttribute(0, 0, DataType::Float32, 2, 0);
		m_vertexLayout.InitAttribute(1, 1, DataType::Float32, 4, offsetof(ParticleDrawData, textureArea));
		m_vertexLayout.InitAttribute(2, 1, DataType::Float32, 1, offsetof(ParticleDrawData, opacity));
		m_vertexLayout.InitAttribute(3, 1, DataType::Float32, 2, offsetof(ParticleDrawData, matrix) + 0 * sizeof(float));
		m_vertexLayout.InitAttribute(4, 1, DataType::Float32, 2, offsetof(ParticleDrawData, matrix) + 2 * sizeof(float));
		m_vertexLayout.InitAttribute(5, 1, DataType::Float32, 2, offsetof(ParticleDrawData, matrix) + 4 * sizeof(float));
		
		m_vertexLayout.SetVertexBuffer(0, *vertexBuffer, 0);
	}
	
	void ParticleManager::KeepAlive(std::shared_ptr<ParticleEmitter> emitter, float time)
	{
		m_keepAlive.emplace_back(time, std::move(emitter));
	}
	
	void ParticleManager::AddEmitter(std::weak_ptr<ParticleEmitter> emitter)
	{
		//Searches for an available emitter entry
		EmitterEntry* selEntry = nullptr;
		for (EmitterEntry& entry : m_emitters)
		{
			if (entry.available)
			{
				selEntry = &entry;
				break;
			}
		}
		
		if (selEntry == nullptr)
		{
			selEntry = &m_emitters.emplace_back();
		}
		
		std::shared_ptr<ParticleEmitter> lockedEmitter = emitter.lock();
		
		selEntry->available = false;
		selEntry->hasOldData = false;
		selEntry->timeNotEmitted = 0;
		selEntry->weakEmitter = std::move(emitter);
		selEntry->firstPage = nullptr;
		selEntry->type = &lockedEmitter->Type();
		
		//Processes texture areas
		const float textureWidth = lockedEmitter->Type().texture->Width();
		const float textureHeight = lockedEmitter->Type().texture->Height();
		for (const Rectangle& textureArea : lockedEmitter->Type().textureAreas)
		{
			TextureArea& areaOut = selEntry->textureAreas.emplace_back();
			areaOut.area = glm::vec4(
				textureArea.x / textureWidth,
				textureArea.y / textureHeight,
				textureArea.MaxX() / textureWidth,
				textureArea.MaxY() / textureHeight
			);
			areaOut.aspectRatio = textureArea.h / textureArea.w;
		}
	}
	
	void ParticleManager::Update(float dt)
	{
		m_particleDrawData.clear();
		
		for (EmitterEntry& entry : m_emitters)
		{
			if (entry.available)
				continue;
			
			std::shared_ptr<ParticleEmitter> emitter = entry.weakEmitter.lock();
			if (emitter == nullptr)
			{
				if (entry.totalParticles == 0)
				{
					entry.available = true;
					entry.weakEmitter.reset();
					
					if (entry.firstPage != nullptr)
					{
						ParticlePage* lastPage = entry.firstPage;
						while (lastPage->next)
							lastPage = lastPage->next;
						lastPage->next = m_availablePages;
						m_availablePages = entry.firstPage;
					}
					
					continue;
				}
			}
			else if (!entry.hasOldData)
			{
				entry.oldPosition = emitter->position;
				entry.oldRotation = emitter->rotation;
				entry.hasOldData = true;
			}
			
			auto& emitterType = const_cast<ParticleEmitterType&>(*entry.type);
			
			float oldTimeNotEmitted = entry.timeNotEmitted;
			int numToEmit = 0;
			
			float emitDelay = 1.0f / emitterType.emissionRate;
			if (emitter && emitter->enabled)
			{
				entry.timeNotEmitted += dt;
				numToEmit = std::floor(oldTimeNotEmitted * emitterType.emissionRate);
			}
			float nextParticleEmitTime = entry.timeNotEmitted;
			int numEmitted = 0;
			
			glm::vec2 gravity = emitterType.gravity * dt;
			
			//Updates and spawns new particles
			ParticlePage* lastPage = nullptr;
			for (ParticlePage* page = entry.firstPage; true; lastPage=page, page = page->next)
			{
				if (page == nullptr)
				{
					if (numEmitted >= numToEmit)
						break;
					
					if (m_availablePages != nullptr)
					{
						page = m_availablePages;
						m_availablePages = m_availablePages->next;
					}
					else
					{
						page = &m_allPages.emplace_back();
					}
					
					page->numParticles = 0;
					page->next = nullptr;
					if (lastPage == nullptr)
						entry.firstPage = page;
					else
						lastPage->next = page;
				}
				else
				{
					//Updates time and kills particles
					for (int i = (int)page->numParticles - 1; i >= 0; i--)
					{
						page->elapsedLifeTime[i] += dt;
						if (page->elapsedLifeTime[i] > page->totalLifeTime[i])
						{
							page->numParticles--;
							page->pos[i] = page->pos[page->numParticles]; 
							page->vel[i] = page->vel[page->numParticles]; 
							page->sizeStartEnd[i] = page->sizeStartEnd[page->numParticles]; 
							page->opacityStartEnd[i] = page->opacityStartEnd[page->numParticles]; 
							page->rotation[i] = page->rotation[page->numParticles]; 
							page->rotationVelocity[i] = page->rotationVelocity[page->numParticles]; 
							page->elapsedLifeTime[i] = page->elapsedLifeTime[page->numParticles]; 
							page->totalLifeTime[i] = page->totalLifeTime[page->numParticles]; 
							page->textureArea[i] = page->textureArea[page->numParticles]; 
						}
					}
					
					//Updates rotation
					for (uint32_t i = 0; i < page->numParticles; i++)
					{
						page->rotation[i] += page->rotationVelocity[i] * dt;
					}
					
					//Updates position and velocity
					for (uint32_t i = 0; i < page->numParticles; i++)
					{
						page->vel[i] += gravity;
						page->pos[i] += page->vel[i] * dt;
					}
				}
				
				//Emits new particles, emitter must not be null here
				while (numEmitted < numToEmit && page->numParticles < PARTICLES_PER_PAGE)
				{
					const uint32_t idx = page->numParticles++;
					
					const float a = (nextParticleEmitTime - oldTimeNotEmitted) /
						(entry.timeNotEmitted - oldTimeNotEmitted);
					
					//Generates position
					page->pos[idx] = emitterType.positionGenerator(m_rng) +
						glm::mix(entry.oldPosition, emitter->position, a);
					
					//Generates velocity
					float dir = RandomFloat(emitterType.directionMin, emitterType.directionMax, m_rng) + emitter->rotation;
					float speed = RandomFloat(emitterType.speedMin, emitterType.speedMax, m_rng);
					page->vel[idx] = glm::vec2(std::cos(dir), std::sin(dir)) * speed;
					
					//Generates rotation
					page->rotation[idx] = RandomFloat(emitterType.rotationMin, emitterType.rotationMax, m_rng) + emitter->rotation;
					
					page->rotationVelocity[idx] = RandomFloat(emitterType.rotationVelMin, emitterType.rotationVelMax, m_rng);
					page->totalLifeTime[idx] = RandomFloat(emitterType.lifeTimeMin, emitterType.lifeTimeMax, m_rng);
					page->elapsedLifeTime[idx] = 0;
					page->sizeStartEnd[idx].x = RandomFloat(emitterType.sizeMin, emitterType.sizeMax, m_rng);
					page->sizeStartEnd[idx].y = page->sizeStartEnd[idx].x * RandomFloat(emitterType.finalSizeScaleMin, emitterType.finalSizeScaleMax, m_rng);
					page->opacityStartEnd[idx].x = RandomFloat(emitterType.opacityMin, emitterType.opacityMax, m_rng);
					page->opacityStartEnd[idx].y = page->opacityStartEnd[idx].x * RandomFloat(emitterType.finalOpacityScaleMin, emitterType.finalOpacityScaleMax, m_rng);;
					page->textureArea[idx] = RandomInt(0, (int)emitterType.textureAreas.size() - 1, m_rng);
					
					numEmitted++;
					nextParticleEmitTime -= emitDelay;
				}
			}
			
			//Adds particles to the draw buffer
			entry.firstParticle = m_particleDrawData.size();
			for (ParticlePage* page = entry.firstPage; page; page = page->next)
			{
				for (uint32_t i = 0; i < page->numParticles; i++)
				{
					float life = page->elapsedLifeTime[i] / page->totalLifeTime[i];
					
					TextureArea textureArea = entry.textureAreas[page->textureArea[i]];
					
					ParticleDrawData& drawData = m_particleDrawData.emplace_back();
					drawData.textureArea = textureArea.area;
					
					drawData.opacity = glm::mix(page->opacityStartEnd[i].x, page->opacityStartEnd[i].y, life);
					
					float size = glm::mix(page->sizeStartEnd[i].x, page->sizeStartEnd[i].y, life);
					glm::vec2 size2(size, size * textureArea.aspectRatio);
					
					float rotation = page->rotation[i];
					if (emitterType.directionToRotation && glm::length2(page->vel[i]) > 1E-5f)
					{
						rotation += std::atan2(page->vel[i].y, page->vel[i].x);
					}
					
					drawData.matrix =
						glm::translate(glm::mat3(1), page->pos[i]) *
						glm::rotate(glm::mat3(1), rotation) *
						glm::scale(glm::mat3(1), size2);
				}
			}
			entry.totalParticles = m_particleDrawData.size() - entry.firstParticle;
			
			entry.timeNotEmitted -= (float)numToEmit * emitDelay;
			
			if (emitter)
			{
				entry.oldPosition = emitter->position;
				entry.oldRotation = emitter->rotation;
			}
		}
		
		//Uploads particles to the GPU
		if (!m_particleDrawData.empty())
		{
			const size_t reqBufferSize = sizeof(ParticleDrawData) * m_particleDrawData.size();
			if (m_particlesBuffer.Size() >= reqBufferSize)
			{
				m_particlesBuffer.Update(0, reqBufferSize, m_particleDrawData.data());
			}
			else
			{
				m_particlesBuffer.Realloc(reqBufferSize, m_particleDrawData.data());
			}
		}
		
		for (int64_t i = (int64_t)m_keepAlive.size() - 1; i >= 0; i--)
		{
			m_keepAlive[i].first -= dt;
			if (m_keepAlive[i].first < 0)
			{
				m_keepAlive[i].second.swap(m_keepAlive.back().second);
				m_keepAlive.pop_back();
			}
		}
	}
	
	static const char* VertexShaderCode = R"(
layout(location=0) in vec2 quadPosition_in;
layout(location=1) in vec4 texArea_in;
layout(location=2) in float opacity_in;
layout(location=3) in mat3x2 transform_in;

out float vOpacity;
out vec2 vTexCoord;

uniform mat3 uViewMatrix;

void main()
{
	vTexCoord = mix(texArea_in.xy, texArea_in.zw, quadPosition_in + 0.5);
	vOpacity = opacity_in;
	
	vec2 worldPos = transform_in * vec3(quadPosition_in, 1.0);
	gl_Position = vec4((uViewMatrix * vec3(worldPos, 1.0)).xy, 0.0, 1.0);
}
)";
	
	static const char* FragmentShaderCode = R"(
layout(location=0) out vec4 color_out;

in float vOpacity;
in vec2 vTexCoord;

uniform sampler2D uTexSampler;
uniform bool uAdditiveBlend;

void main()
{
	color_out = texture(uTexSampler, vTexCoord);
	
	if (uAdditiveBlend)
	{
		color_out.a = 0.0;
		color_out.rgb *= vOpacity;
	}
	else
	{
		color_out.a *= vOpacity;
		color_out.rgb *= color_out.a;
	}
}
)";
	
	static Shader* shader;
	static int additiveBlendUniformLocation;
	static int viewMatrixUniformLocation;
	
	void CreateParticleSystemShader()
	{
		shader = new Shader;
		shader->AddVertexShader(VertexShaderCode);
		shader->AddFragmentShader(FragmentShaderCode);
		shader->Link();
		
		shader->SetUniformI(shader->GetUniformLocation("uTexSampler"), 0);
		additiveBlendUniformLocation = shader->GetUniformLocation("uAdditiveBlend");
		viewMatrixUniformLocation = shader->GetUniformLocation("uViewMatrix");
		
		const float vertices[] =
		{
			-0.5f, -0.5f,
			 0.5f, -0.5f,
			-0.5f,  0.5f,
			 0.5f,  0.5f
		};
		vertexBuffer = new Buffer(BufferFlags::VertexBuffer, sizeof(vertices), vertices);
	}
	
	void DestroyParticleSystemShader()
	{
		delete shader;
		delete vertexBuffer;
	}
	
	void ParticleManager::Draw(const glm::mat3& viewMatrix)
	{
		if (m_particleDrawData.empty())
			return;
		
		m_vertexLayout.Bind();
		
		SetBlendState(&AlphaBlendPreMultiplied);
		
		shader->Bind();
		
		shader->SetUniformM3(viewMatrixUniformLocation, viewMatrix);
		
		for (const EmitterEntry& emitter : m_emitters)
		{
			if (emitter.available || emitter.totalParticles == 0)
				continue;
			
			shader->SetUniformI(additiveBlendUniformLocation, emitter.type->blendAdditive);
			
			m_vertexLayout.SetVertexBuffer(1, m_particlesBuffer, sizeof(ParticleDrawData) * emitter.firstParticle);
			
			emitter.type->texture->Bind(0);
			
			jm::Draw(DrawTopology::TriangleStrip, 0, 4, emitter.totalParticles);
		}
		
		SetBlendState(nullptr);
	}
}
