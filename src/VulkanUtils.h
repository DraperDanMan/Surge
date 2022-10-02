#pragma once

#include <vulkan/vulkan.h>
#include <shaderc/shaderc.hpp>
#include <string>
#include <assert.h>
#include <vector>
#include <fstream>
#include <unordered_map>

#define VK_NONE 0

// Default fence timeout in nanoseconds
#define DEFAULT_FENCE_TIMEOUT 100000000000

#define VK_CHECK(call) do { VkResult result = call;if (result != VK_SUCCESS) {printf("VKCheck Error, VkResult: %s in %s at line %d",vulkan::utils::ErrorAsString(result).c_str(),__FILE__,__LINE__); assert(result == VK_SUCCESS);}} while(0)

namespace vulkan {
namespace utils {
std::string ErrorAsString(VkResult errorCode);
void SetImageLayout(VkCommandBuffer cmdbuffer, VkImage image, VkImageLayout oldImageLayout, VkImageLayout newImageLayout,
        VkImageSubresourceRange subresourceRange, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask);
void SetImageLayout(VkCommandBuffer cmdbuffer, VkImage image, VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout,
        VkImageLayout newImageLayout, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask);
}

// An includer for files implementing shaderc's includer interface. It responds
// to the file including query from the compiler with the full path and content
// of the file to be included. In the case that the file is not found or cannot
// be opened, the full path field of in the response will point to an empty
// string, and error message will be passed to the content field.
// This class provides the basic thread-safety guarantee.
class FileIncluder : public shaderc::CompileOptions::IncluderInterface {
public:
  explicit FileIncluder() {
  };

  ~FileIncluder() override;

  // Resolves a requested source file of a given type from a requesting
  // source into a shaderc_include_result whose contents will remain valid
  // until it's released.
  shaderc_include_result *GetInclude(const char *requested_source,
                                     shaderc_include_type type,
                                     const char *requesting_source,
                                     size_t include_depth) override;
  // Releases an include result.
  void ReleaseInclude(shaderc_include_result *include_result) override;

  // Returns a reference to the member storing the set of included files.
  const std::unordered_map<std::string, shaderc_include_result *> &
  file_path_trace() const {
    return included_files;
  }


private:
  // The set of full paths of included files.
  std::unordered_map<std::string, shaderc_include_result *> included_files;
};

struct ShaderLoader {
public:
  VkShaderModule LoadShader(VkDevice device, const char *path);
  std::string ReadTextFile(const std::string_view &fileName);
private:
  shaderc::Compiler compiler;
  shaderc::CompileOptions options;
  std::vector<uint32_t> CompileShader(std::string_view shaderName,
                                      shaderc_shader_kind kind,
                                      const std::string &source, bool optimize);
  std::string PreprocessShader(std::string_view shaderName,
                               shaderc_shader_kind kind,
                               const std::string &source);

};


}