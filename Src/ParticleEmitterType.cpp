#include "ParticleEmitterType.hpp"
#include "Utils.hpp"
#include "Asset.hpp"

#include <gsl/span>
#include <string>
#include <yaml-cpp/yaml.h>

namespace jm
{
	inline void ParseMinMax(const YAML::Node& node, float& min, float& max, float defMin, float defMax)
	{
		if (node.IsSequence() && node.size() == 1)
		{
			min = node[0].as<float>(defMin);
			max = node[0].as<float>(defMax);
		}
		else if (node.IsSequence() && node.size() > 1)
		{
			min = node[0].as<float>(defMin);
			max = node[1].as<float>(defMax);
		}
		else
		{
			min = node.as<float>(defMin);
			max = node.as<float>(defMax);
		}
	}
	
	ParticleEmitterType LoadParticleEmitterAsset(gsl::span<const char> fileData, const std::string& name)
	{
		std::string codeCopy(fileData.begin(), fileData.end());
		YAML::Node rootYaml = YAML::Load(codeCopy);
		
		ParticleEmitterType emitter;
		
		emitter.emissionRate = rootYaml["emissionRate"].as<float>(100);
		
		ParseMinMax(rootYaml["lifeTime"], emitter.lifeTimeMin, emitter.lifeTimeMax, 1.0f, 1.0f);
		ParseMinMax(rootYaml["direction"], emitter.directionMin, emitter.directionMax, 0.0f, 360.0f);
		ParseMinMax(rootYaml["speed"], emitter.speedMin, emitter.speedMax, 1.0f, 1.0f);
		ParseMinMax(rootYaml["rotation"], emitter.rotationMin, emitter.rotationMax, 0.0f, 360.0f);
		ParseMinMax(rootYaml["angularVelocity"], emitter.rotationVelMin, emitter.rotationVelMax, 0.0f, 0.0f);
		ParseMinMax(rootYaml["opacity"], emitter.opacityMin, emitter.opacityMax, 1.0f, 1.0f);
		ParseMinMax(rootYaml["endOpacity"], emitter.finalOpacityScaleMin, emitter.finalOpacityScaleMax, 1.0f, 1.0f);
		ParseMinMax(rootYaml["size"], emitter.sizeMin, emitter.sizeMax, 1.0f, 1.0f);
		ParseMinMax(rootYaml["endSize"], emitter.finalSizeScaleMin, emitter.finalSizeScaleMax, 1.0f, 1.0f);
		
		emitter.directionMin = glm::radians(emitter.directionMin);
		emitter.directionMax = glm::radians(emitter.directionMax);
		emitter.rotationMin = glm::radians(emitter.rotationMin);
		emitter.rotationMax = glm::radians(emitter.rotationMax);
		emitter.rotationVelMin = glm::radians(emitter.rotationVelMin);
		emitter.rotationVelMax = glm::radians(emitter.rotationVelMax);
		
		emitter.directionToRotation = rootYaml["dirToRotation"].as<bool>(false);
		
		std::string blendMode = rootYaml["blend"].as<std::string>("alpha");
		if (blendMode == "additive")
		{
			emitter.blendAdditive = true;
		}
		else if (blendMode != "alpha")
		{
			std::cout << "Unknown particle blend mode: '" << blendMode << "'. Should be 'alpha' or 'additive'." << std::endl;
		}
		
		emitter.gravity.y = rootYaml["gravity"].as<float>(0);
		emitter.drag = rootYaml["drag"].as<float>(0);
		
		float spawnRadius = rootYaml["spawnRadius"].as<float>(0);
		emitter.positionGenerator = [spawnRadius] (pcg32_fast& rng) -> glm::vec2
		{
			float theta = RandomFloat(0.0f, TWO_PI, rng);
			float radius = RandomFloat(0.0f, spawnRadius, rng);
			return glm::vec2(std::cos(theta) * radius, std::sin(theta) * radius);
		};
		
		std::string textureName = rootYaml["texture"].as<std::string>();
		std::string fullTextureName = Concat({ ParentPath(name, true), textureName });
		emitter.texture = &GetAsset<Texture2D>(fullTextureName);
		
		if (rootYaml["textureAreas"].IsDefined())
		{
			for (const YAML::Node& textureAreaNode : rootYaml["textureAreas"])
			{
				Rectangle& area = emitter.textureAreas.emplace_back();
				area.x = textureAreaNode["x"].as<float>(0);
				area.y = textureAreaNode["y"].as<float>(0);
				area.w = textureAreaNode["w"].as<float>(emitter.texture->Width());
				area.h = textureAreaNode["h"].as<float>(emitter.texture->Height());
			}
		}
		else
		{
			emitter.textureAreas.emplace_back(0, 0, emitter.texture->Width(), emitter.texture->Height());
		}
		
		return emitter;
	}
	
	void RegisterParticleEmitterAssetLoader()
	{
		RegisterAssetLoader<ParticleEmitterType>("ype", &LoadParticleEmitterAsset);
	}
}
