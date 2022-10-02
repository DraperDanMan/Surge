#pragma once

#include <string>

#include "vulkan/vulkan.h"

namespace Surge
{

enum class ImageFormat
{
	None = 0,
	RGBA,
	RGBA32F
};

class Image
{
public:
	Image(std::string_view path);
	Image(uint32_t width, uint32_t height, ImageFormat format, const void* data = nullptr);
	~Image();

	void SetData(const void* data);
	void GetData( void* data ) const;

	bool SaveToFile( std::string filepath );

	[[nodiscard]] VkDescriptorSet GetDescriptorSet() const { return m_descriptorSet; }
	[[nodiscard]] VkImage GetVkImage() const { return m_image; }
	[[nodiscard]] VkImageView GetVkImageView() const { return m_imageView; }
	[[nodiscard]] VkSampler GetVkSampler() const { return m_sampler; }
	[[nodiscard]] std::string GetFilename() const { return m_filepath; }

	[[nodiscard]] uint32_t GetWidth() const { return m_width; }
	[[nodiscard]] uint32_t GetHeight() const { return m_height; }
private:
	void AllocateMemory(uint64_t size);
private:
	uint32_t m_width = 0, m_height = 0;

	VkImage m_image = nullptr;
	VkImageView m_imageView = nullptr;
	VkDeviceMemory m_memory = nullptr;
	VkSampler m_sampler = nullptr;

	ImageFormat m_format = ImageFormat::None;

	VkBuffer m_stagingBuffer = nullptr;
	VkDeviceMemory m_stagingBufferMemory = nullptr;

	size_t m_alignedSize = 0;

	VkDescriptorSet m_descriptorSet = nullptr;

	std::string m_filepath;
};

}