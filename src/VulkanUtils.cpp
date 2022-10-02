#include "VulkanUtils.h"

namespace vulkan {
namespace utils {
//Taken from sascha willems vulkan tools library https://github.com/SaschaWillems/Vulkan/blob/master/base/VulkanTools.cpp
std::string ErrorAsString(VkResult errorCode) {
  switch (errorCode) {
#define STR(r) case VK_ ##r: return #r
  STR(NOT_READY);
  STR(TIMEOUT);
  STR(EVENT_SET);
  STR(EVENT_RESET);
  STR(INCOMPLETE);
  STR(ERROR_OUT_OF_HOST_MEMORY);
  STR(ERROR_OUT_OF_DEVICE_MEMORY);
  STR(ERROR_INITIALIZATION_FAILED);
  STR(ERROR_DEVICE_LOST);
  STR(ERROR_MEMORY_MAP_FAILED);
  STR(ERROR_LAYER_NOT_PRESENT);
  STR(ERROR_EXTENSION_NOT_PRESENT);
  STR(ERROR_FEATURE_NOT_PRESENT);
  STR(ERROR_INCOMPATIBLE_DRIVER);
  STR(ERROR_TOO_MANY_OBJECTS);
  STR(ERROR_FORMAT_NOT_SUPPORTED);
  STR(ERROR_SURFACE_LOST_KHR);
  STR(ERROR_NATIVE_WINDOW_IN_USE_KHR);
  STR(SUBOPTIMAL_KHR);
  STR(ERROR_OUT_OF_DATE_KHR);
  STR(ERROR_INCOMPATIBLE_DISPLAY_KHR);
  STR(ERROR_VALIDATION_FAILED_EXT);
  STR(ERROR_INVALID_SHADER_NV);
#undef STR
  default:
    return "UNKNOWN_ERROR";
  }
}

void SetImageLayout(VkCommandBuffer cmdbuffer, VkImage image, VkImageLayout oldImageLayout, VkImageLayout newImageLayout,
		VkImageSubresourceRange subresourceRange, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask)
	{
		// Create an image barrier object
		VkImageMemoryBarrier imageMemoryBarrier = {VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
        imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.oldLayout = oldImageLayout;
		imageMemoryBarrier.newLayout = newImageLayout;
		imageMemoryBarrier.image = image;
		imageMemoryBarrier.subresourceRange = subresourceRange;

		// Source layouts (old)
		// Source access mask controls actions that have to be finished on the old layout
		// before it will be transitioned to the new layout
		switch (oldImageLayout)
		{
		case VK_IMAGE_LAYOUT_UNDEFINED:
			// Image layout is undefined (or does not matter)
			// Only valid as initial layout
			// No flags required, listed only for completeness
			imageMemoryBarrier.srcAccessMask = 0;
			break;

		case VK_IMAGE_LAYOUT_PREINITIALIZED:
			// Image is preinitialized
			// Only valid as initial layout for linear images, preserves memory contents
			// Make sure host writes have been finished
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
			// Image is a color attachment
			// Make sure any writes to the color buffer have been finished
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
			// Image is a depth/stencil attachment
			// Make sure any writes to the depth/stencil buffer have been finished
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
			// Image is a transfer source
			// Make sure any reads from the image have been finished
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			break;

		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
			// Image is a transfer destination
			// Make sure any writes to the image have been finished
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
			// Image is read by a shader
			// Make sure any shader reads from the image have been finished
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			break;
		default:
			// Other source layouts aren't handled (yet)
			break;
		}

		// Target layouts (new)
		// Destination access mask controls the dependency for the new image layout
		switch (newImageLayout)
		{
		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
			// Image will be used as a transfer destination
			// Make sure any writes to the image have been finished
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
			// Image will be used as a transfer source
			// Make sure any reads from the image have been finished
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			break;

		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
			// Image will be used as a color attachment
			// Make sure any writes to the color buffer have been finished
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
			// Image layout will be used as a depth/stencil attachment
			// Make sure any writes to depth/stencil buffer have been finished
			imageMemoryBarrier.dstAccessMask = imageMemoryBarrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
			// Image will be read in a shader (sampler, input attachment)
			// Make sure any writes to the image have been finished
			if (imageMemoryBarrier.srcAccessMask == 0)
			{
				imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
			}
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			break;
		default:
			// Other source layouts aren't handled (yet)
			break;
		}

		// Put barrier inside setup command buffer
		vkCmdPipelineBarrier(cmdbuffer, srcStageMask, dstStageMask, 0, 0, nullptr,
			0, nullptr, 1, &imageMemoryBarrier);
	}

void SetImageLayout(VkCommandBuffer cmdbuffer, VkImage image, VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout,
            VkImageLayout newImageLayout, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask)
{
    VkImageSubresourceRange subresourceRange = {};
    subresourceRange.aspectMask = aspectMask;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = 1;
    subresourceRange.layerCount = 1;
    SetImageLayout(cmdbuffer, image, oldImageLayout, newImageLayout, subresourceRange, srcStageMask, dstStageMask);
}

}

VkShaderModule ShaderLoader::LoadShader(VkDevice device, const char *path) {
  std::string tempPath     = path;
  std::string shaderName   = tempPath.rfind("/") != std::string::npos ? tempPath.substr(tempPath.rfind("/")) : tempPath;
  shaderc_shader_kind kind = shaderName.rfind(".frag") != std::string::npos
                               ? shaderc_fragment_shader
                               : shaderName.rfind(".vert") != std::string::npos ? shaderc_vertex_shader
	                           : shaderc_compute_shader;

  std::string source = ReadTextFile(path);

  //TODO: Move runtime compile to actually save the byte code and only compile if the hash differs
  //      Additionally hot reload on window re-focus if the hash differs.
  std::vector<uint32_t> spirv = CompileShader(shaderName, kind, source, true);

  VkShaderModuleCreateInfo createInfo = {
      VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
  createInfo.codeSize = spirv.size() * sizeof(unsigned int);
  createInfo.pCode    = spirv.data();

  VkShaderModule shaderModule = nullptr;

  VK_CHECK(vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule));
  return shaderModule;
}

std::vector<uint32_t> ShaderLoader::CompileShader(
    std::string_view shaderName, shaderc_shader_kind kind,
    const std::string &source, bool optimize) {
  std::unique_ptr<FileIncluder> includer(new FileIncluder());

  // Add any custom defines eg.
  // options.AddMacroDefinition("MY_DEFINE", "1");
  options.SetTargetEnvironment(shaderc_target_env_vulkan,
                               shaderc_env_version_vulkan_1_2);
  options.SetSourceLanguage(shaderc_source_language_glsl);
  options.SetTargetSpirv(shaderc_spirv_version_1_2);
  options.SetIncluder(std::move(includer));
  if (optimize)
    options.SetOptimizationLevel(shaderc_optimization_level_size);

  std::string processedSource = this->
      PreprocessShader(shaderName, kind, source);

  printf("Compiling Shader %s of Type %s\n", shaderName.data(),
            kind == shaderc_fragment_shader ? "Fragment" : "Vertex");
  shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(
      processedSource, kind, shaderName.data(), options);

  if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
    fprintf(stderr,"Shader Compile Error: %s\n", module.GetErrorMessage().c_str());
    return std::vector<uint32_t>();
  }

  return {module.cbegin(), module.cend()};
}

std::string ShaderLoader::PreprocessShader(std::string_view shaderName,
                                           shaderc_shader_kind kind,
                                           const std::string &source) {
  auto result = compiler.PreprocessGlsl(source, kind, shaderName.data(),
                                        options);
  if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
    printf("Shader Pre-process Error: %s\n",
               result.GetErrorMessage().c_str());
    return "";
  }
  return {result.cbegin(), result.cend()};
}

//Pulled from https://github.com/jbikker/lighthouse2/blob/master/lib/rendercore_vulkan_rt/vulkan_shader.cpp It appeared to be much more reliable than my file read...
std::string ShaderLoader::ReadTextFile(const std::string_view &fileName) {
  std::string buffer;
  std::ifstream fileStream(fileName.data());
  if (!fileStream.is_open())
    printf("Could not open file %s", fileName.data());

  std::string temp;
  while (getline(fileStream, temp))
    buffer.append(temp), buffer.append("\n");

  fileStream >> buffer;
  fileStream.close();
  return buffer;
}

FileIncluder::~FileIncluder() = default;

std::string ReadTextFile(const std::string_view &fileName) {
  std::string buffer;
  std::ifstream fileStream(fileName.data());
  if (!fileStream.is_open())
    printf("SHADER PREPROCESS: Failed to find shader include %s",
               fileName.data());

  std::string temp;
  while (getline(fileStream, temp))
    buffer.append(temp), buffer.append("\n");

  fileStream >> buffer;
  fileStream.close();
  return buffer;
}

shaderc_include_result *FileIncluder::GetInclude(
    const char *requested_source, shaderc_include_type include_type,
    const char *requesting_source, size_t) {

  std::string full_path = "";
  if (include_type == shaderc_include_type_relative) {
    full_path = "Shaders/" + std::string(requested_source);
  } else {
    full_path = "Shaders/" + std::string(requested_source);
  }

  if (included_files.find(full_path) != included_files.end()) {
    return included_files.at(full_path);
  }

  std::string includeCode = ReadTextFile(full_path);
  std::vector<char> contents(includeCode.cbegin(), includeCode.cend());

  // In principle, several threads could be resolving includes at the same
  // time.  Protect the included_files.

  // Read the file and save its full path and contents into stable addresses.

  auto result = new shaderc_include_result{
      full_path.data(),
      full_path.length(),
      contents.data(),
      contents.size()
  };

  included_files.emplace(full_path, result);

  return result;
}

void FileIncluder::ReleaseInclude(shaderc_include_result *include_result) {
  std::string full_path(include_result->source_name);
  if (included_files.find(full_path) != included_files.end()) {
    delete included_files.at(full_path);
  }
}
}