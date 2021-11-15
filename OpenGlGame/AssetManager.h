#pragma once

#include "AssetId.h"

#include <unordered_map>
#include <queue>
#include <functional>
#include <filesystem>
#include <mutex>

namespace Game
{
	class Texture;
	struct Material;

	class Shader;
	class ShaderProgram;

	class AssetManager
	{
		class LoadOrder
		{
		public:
			virtual ~LoadOrder() = default;

			bool Ready = false;
			virtual void Update() = 0;
		};

		class TextureOrder : public LoadOrder
		{
		public:
			std::filesystem::path Path;
			Pointer<Texture> Texture = nullptr;
			
			void Update() override;
		};

		Pointer<Texture> m_DefaultTexture;
		Pointer<Material> m_DefaultMaterial;

		Pointer<Shader> m_DefaultVertexShader;
		Pointer<Shader> m_DefaultFragmentShader;

		Pointer<ShaderProgram> m_DefaultShader;

		std::unordered_map<AssetId, Pointer<Material>> m_MaterialCash;
		std::unordered_map<AssetId, Pointer<Texture>> m_TextureCash;

		std::queue<std::function<void()>> m_GlQueue;
		std::vector<LoadOrder*> m_Orders;

		std::mutex m_Mutex;
		
	public:
		AssetManager();

		bool LoadShader(const std::filesystem::path& path);
		bool LoadTexture(const std::filesystem::path& path);
		bool LoadModel(const std::filesystem::path& path);

		void Update();
	private:
		void Run();
		
	};
}